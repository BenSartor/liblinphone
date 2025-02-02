/*
 * Copyright (c) 2010-2022 Belledonne Communications SARL.
 *
 * This file is part of Liblinphone
 * (see https://gitlab.linphone.org/BC/public/liblinphone).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "account.h"

#include "core/core.h"
#include "linphone/api/c-account-params.h"
#include "linphone/api/c-account.h"
#include "push-notification/push-notification-config.h"
#include <bctoolbox/defs.h>
#ifdef HAVE_ADVANCED_IM
#ifdef HAVE_LIME_X3DH
#include "chat/encryption/lime-x3dh-encryption-engine.h"
#endif // HAVE_LIME_X3DH
#endif // HAVE_ADVANCED_IM
#include "c-wrapper/c-wrapper.h"
#include "c-wrapper/internal/c-tools.h"
#include "linphone/core.h"
#include "private.h"
#include "utils/custom-params.h"

// =============================================================================

using namespace std;

LINPHONE_BEGIN_NAMESPACE

Account::Account(LinphoneCore *lc, std::shared_ptr<AccountParams> params) {
	mCore = lc;
	mParams = params;
	applyParamsChanges();
	bctbx_message("LinphoneAccount[%p] created with params", toC());
}

Account::Account(LinphoneCore *lc, std::shared_ptr<AccountParams> params, LinphoneProxyConfig *config)
    : Account(lc, params) {
	mConfig = config;
	bctbx_message("LinphoneAccount[%p] created with proxy config", toC());
}

Account::Account(const Account &other) : HybridObject(other) {
	bctbx_message("LinphoneAccount[%p] created from copy constructor", toC());
}

Account::~Account() {
	bctbx_message("LinphoneAccount[%p] destroyed", toC());
	if (mSentHeaders) sal_custom_header_free(mSentHeaders);
	setDependency(nullptr);
	if (mErrorInfo) linphone_error_info_unref(mErrorInfo);
	if (mPresenceModel) linphone_presence_model_unref(mPresenceModel);

	releaseOps();
}

Account *Account::clone() const {
	return new Account(*this);
}

// -----------------------------------------------------------------------------

static std::string appendLinphoneAddress(const std::shared_ptr<Address> &addr, const std::string &out) {
	auto res = out;
	if (addr) {
		res.append(addr->toString());
	}
	return res;
}

static std::string appendString(const std::string &string, const std::string &out) {
	auto res = out;
	if (!string.empty()) {
		res.append(string);
	}
	return res;
}

bool Account::computePublishParamsHash() {
	std::string source;
	char hash[33];
	char saved;
	unsigned long long previous_hash[2];
	previous_hash[0] = mPreviousPublishParamsHash[0];
	previous_hash[1] = mPreviousPublishParamsHash[1];

	source.append(std::to_string(static_cast<int>(mParams->mPrivacy)));
	source = appendLinphoneAddress(mParams->mIdentityAddress, source);
	source = appendString(mParams->mProxy, source);
	const auto &routes = mParams->mRoutes;
	for (const auto &route : routes) {
		source = appendLinphoneAddress(route, source);
	}
	source = appendString(mParams->mRealm, source);
	source.append(std::to_string(mParams->mPublishExpires));
	source.append(std::to_string(mParams->mPublishEnabled ? 1 : 0));
	belle_sip_auth_helper_compute_ha1(source.c_str(), "dummy", "dummy", hash);
	saved = hash[16];
	hash[16] = '\0';
	mPreviousPublishParamsHash[0] = strtoull(hash, (char **)NULL, 16);
	hash[16] = saved;
	mPreviousPublishParamsHash[1] = strtoull(&hash[16], (char **)NULL, 16);
	return previous_hash[0] != mPreviousPublishParamsHash[0] || previous_hash[1] != mPreviousPublishParamsHash[1];
}

LinphoneAccountAddressComparisonResult Account::compareLinphoneAddresses(const std::shared_ptr<Address> &a,
                                                                         const std::shared_ptr<Address> &b) {
	if (a == NULL && b == NULL) return LinphoneAccountAddressEqual;
	else if (!a || !b) return LinphoneAccountAddressDifferent;

	if (*a == *b) return LinphoneAccountAddressEqual;
	if (a->weakEqual(*b)) {
		/*also check both transport and uri */
		if (a->getSecure() == b->getSecure() && a->getTransport() == b->getTransport())
			return LinphoneAccountAddressWeakEqual;
		else return LinphoneAccountAddressDifferent;
	}
	return LinphoneAccountAddressDifferent; /*either username, domain or port ar not equals*/
}

LinphoneAccountAddressComparisonResult Account::isServerConfigChanged(std::shared_ptr<AccountParams> oldParams,
                                                                      std::shared_ptr<AccountParams> newParams) {
	std::shared_ptr<Address> oldProxy =
	    oldParams != nullptr && !oldParams->mProxy.empty() ? Address::create(oldParams->mProxy) : NULL;
	std::shared_ptr<Address> newProxy = !newParams->mProxy.empty() ? Address::create(newParams->mProxy) : NULL;
	LinphoneAccountAddressComparisonResult result_identity;
	LinphoneAccountAddressComparisonResult result;

	result = compareLinphoneAddresses(oldParams != nullptr ? oldParams->mIdentityAddress : NULL,
	                                  newParams->mIdentityAddress);
	if (result == LinphoneAccountAddressDifferent) goto end;
	result_identity = result;

	result = compareLinphoneAddresses(oldProxy, newProxy);

	if (mContactAddress && !mContactAddress->getUriParamValue("gr").empty() && result != LinphoneAccountAddressEqual) {
		// Returning weak equal to be sure no unregister will be done
		result = LinphoneAccountAddressWeakEqual;
		goto end;
	}

	// This is the legacy mode, if there is no gruu and result is different,
	// then an unregister will be triggered.
	if (result == LinphoneAccountAddressDifferent) goto end;
	/** If the proxies are equal use the result of the difference between the identities,
	 * otherwise the result is weak-equal and so weak-equal must be returned even if the
	 * identities were equal.
	 */
	if (result == LinphoneAccountAddressEqual) result = result_identity;

end:
	lInfo() << "linphoneAccountIsServerConfigChanged : " << result;

	return result;
}

LinphoneStatus Account::setAccountParams(std::shared_ptr<AccountParams> params) {
	mOldParams = mParams ? mParams : nullptr;

	// Equivalent of the old proxy_config_edit
	computePublishParamsHash();

	if (mParams->mPublishEnabled && mPresencePublishEvent) {
		mPresencePublishEvent->pause();
	}

	// Replacing the old params by the updated one
	mParams = params;

	// Some changes in AccountParams needs a special treatment in Account
	applyParamsChanges();

	// Equivalent of the old proxy_config_done
	return done();
}

std::shared_ptr<const AccountParams> Account::getAccountParams() const {
	return mParams;
}

bool Account::customContactChanged() {
	if (!mOldParams) return false;
	if (mParams->mCustomContact == nullptr && mOldParams->mCustomContact == nullptr) return false;
	if (mParams->mCustomContact != nullptr && mOldParams->mCustomContact == nullptr) return true;
	if (mParams->mCustomContact == nullptr && mOldParams->mCustomContact != nullptr) return true;
	return !((*mOldParams->mCustomContact) != (*mParams->mCustomContact));
}

void Account::applyParamsChanges() {
	if (mOldParams == nullptr || mOldParams->mInternationalPrefix != mParams->mInternationalPrefix)
		onInternationalPrefixChanged();

	if (mOldParams == nullptr || mOldParams->mConferenceFactoryUri != mParams->mConferenceFactoryUri)
		onConferenceFactoryUriChanged(mParams->mConferenceFactoryUri);

	if (mOldParams == nullptr ||
	    ((mOldParams->mAudioVideoConferenceFactoryAddress != nullptr) ^
	     (mParams->mAudioVideoConferenceFactoryAddress != nullptr)) ||
	    ((mOldParams->mAudioVideoConferenceFactoryAddress != nullptr) &&
	     (mParams->mAudioVideoConferenceFactoryAddress != nullptr) &&
	     (*mOldParams->mAudioVideoConferenceFactoryAddress == *mParams->mAudioVideoConferenceFactoryAddress)))
		onAudioVideoConferenceFactoryAddressChanged(mParams->mAudioVideoConferenceFactoryAddress);

	if (mOldParams == nullptr || mOldParams->mNatPolicy != mParams->mNatPolicy)
		if (mParams->mNatPolicy != nullptr) onNatPolicyChanged(mParams->mNatPolicy);

	if (mOldParams == nullptr || mOldParams->mLimeServerUrl != mParams->mLimeServerUrl) {
		onLimeServerUrlChanged(mParams->mLimeServerUrl);
		mRegisterChanged = true;
	}

	if (mOldParams == nullptr || mOldParams->mRegisterEnabled != mParams->mRegisterEnabled ||
	    mOldParams->mExpires != mParams->mExpires || mOldParams->mContactParameters != mParams->mContactParameters ||
	    mOldParams->mContactUriParameters != mParams->mContactUriParameters ||
	    mOldParams->mPushNotificationAllowed != mParams->mPushNotificationAllowed ||
	    mOldParams->mRemotePushNotificationAllowed != mParams->mRemotePushNotificationAllowed ||
	    !(mOldParams->mPushNotificationConfig->isEqual(*mParams->mPushNotificationConfig)) || customContactChanged()) {
		mRegisterChanged = true;
	}
}

// -----------------------------------------------------------------------------

void Account::setAuthFailure(int authFailure) {
	mAuthFailure = authFailure;
}

void Account::setRegisterChanged(bool registerChanged) {
	mRegisterChanged = registerChanged;
}

void Account::setSendPublish(bool sendPublish) {
	mSendPublish = sendPublish;
}

void Account::setNeedToRegister(bool needToRegister) {
	mNeedToRegister = needToRegister;
}

void Account::setDeletionDate(time_t deletionDate) {
	mDeletionDate = deletionDate;
}

void Account::setSipEtag(const std::string &sipEtag) {
	mSipEtag = sipEtag;
}

void Account::setCore(LinphoneCore *lc) {
	mCore = lc;
}

void Account::setErrorInfo(LinphoneErrorInfo *errorInfo) {
	mErrorInfo = errorInfo;
}

void Account::setContactAddress(const std::shared_ptr<Address> contact) {
	mContactAddress = nullptr;
	if (contact) mContactAddress = contact->clone()->toSharedPtr();
	setContactAddressWithoutParams(contact);
}

void Account::setContactAddressWithoutParams(const std::shared_ptr<Address> contact) {
	mContactAddressWithoutParams = nullptr;

	if (contact) {
		mContactAddressWithoutParams = contact->clone()->toSharedPtr();
		mContactAddressWithoutParams->clean();
		mContactAddressWithoutParams->setPort(-1);
		mContactAddressWithoutParams->setDomain(nullptr);
		mContactAddressWithoutParams->setDisplayName(nullptr);
	}
}

void Account::setPendingContactAddress(std::shared_ptr<Address> contact) {
	if (mPendingContactAddress) {
		mPendingContactAddress = nullptr;
	}

	if (contact) mPendingContactAddress = contact;
}

void Account::setServiceRouteAddress(std::shared_ptr<Address> serviceRoute) {
	mServiceRouteAddress = nullptr;

	if (serviceRoute) mServiceRouteAddress = serviceRoute->clone()->toSharedPtr();
}

// Enable register on account dependent on the given one (if any).
// Also force contact address of dependent account to the given one
void Account::updateDependentAccount(LinphoneRegistrationState state, const std::string &message) {
	if (!mCore) return;
	bctbx_list_t *it = mCore->sip_conf.accounts;

	for (; it; it = it->next) {
		LinphoneAccount *tmp = static_cast<LinphoneAccount *>(it->data);
		auto params = Account::toCpp(tmp)->mParams;
		lInfo() << "updateDependentAccount(): " << this << " is registered, checking for [" << tmp
		        << "] ->dependency=" << linphone_account_get_dependency(tmp);

		if (tmp != this->toC() && linphone_account_get_dependency(tmp) == this->toC()) {
			auto tmpCpp = Account::toCpp(tmp);
			if (!params->mRegisterEnabled) {
				lInfo() << "Dependant account [" << tmp << "] has registration disabled, so it will not register.";
				continue;
			}
			auto copyParams = params->clone()->toSharedPtr();
			if (state == LinphoneRegistrationOk) {
				// Force dependent account to re-register
				params->mRegisterEnabled = false;
				copyParams->mRegisterEnabled = true;
				const SalAddress *salAddr = mOp->getContactAddress();

				if (!mContactAddress) {
					mContactAddress = Address::create();
				}
				if (salAddr) {
					mContactAddress->setImpl(salAddr);
				}
			} else if (state == LinphoneRegistrationCleared || state == LinphoneRegistrationFailed) {
				tmpCpp->pauseRegister();
				tmpCpp->setState(state, message);
			}
			tmpCpp->setAccountParams(copyParams);
			tmpCpp->update();
		}
	}
}

void Account::setState(LinphoneRegistrationState state, const std::string &message) {
	if (mState != state ||
	    state == LinphoneRegistrationOk) { /*allow multiple notification of LinphoneRegistrationOk for refreshing*/
		const auto identity = (mParams) ? mParams->getIdentity().c_str() : std::string();
		if (!mParams) lWarning() << "AccountParams not set for Account [" << this->toC() << "]";
		lInfo() << "Account [" << this << "] for identity [" << identity << "] moving from state ["
		        << linphone_registration_state_to_string(mState) << "] to ["
		        << linphone_registration_state_to_string(state) << "] on core [" << mCore << "]";

		if (state == LinphoneRegistrationOk) {

			const auto salAddr = mOp->getContactAddress();
			if (salAddr) {
				if (!mContactAddress) {
					mContactAddress = (new Address())->toSharedPtr();
				}
				mContactAddress->setImpl(salAddr);
			}
			mOldParams = nullptr; // We can drop oldParams, since last registration was successful.
		}

		LinphoneRegistrationState previousState = mState;
		mState = state;
		if (!mDependency) {
			updateDependentAccount(state, message);
		}

		_linphone_account_notify_registration_state_changed(this->toC(), state, message.c_str());
		if (mCore) linphone_core_notify_account_registration_state_changed(mCore, this->toC(), state, message.c_str());
		if (mConfig && mCore) {
			// Compatibility with proxy config
			linphone_core_notify_registration_state_changed(mCore, mConfig, state, message.c_str());
		}

		if (linphone_core_should_subscribe_friends_only_when_registered(mCore) && state == LinphoneRegistrationOk &&
		    previousState != state) {
			linphone_core_update_friends_subscriptions(mCore);
		}
	} else {
		/*state already reported*/
	}
}

void Account::setOp(SalRegisterOp *op) {
	mOp = op;
}

void Account::setCustomheader(const std::string &headerName, const std::string &headerValue) {
	mSentHeaders = sal_custom_header_append(mSentHeaders, headerName.c_str(), headerValue.c_str());
	mRegisterChanged = true;
}

void Account::setPresencePublishEvent(const std::shared_ptr<EventPublish> &presencePublishEvent) {
	mPresencePublishEvent = presencePublishEvent;
}

void Account::setDependency(std::shared_ptr<Account> dependency) {
	if (!mParams) {
		lWarning() << "setDependency is called but no AccountParams is set on Account [" << this->toC() << "]";
		return;
	}

	if (dependency && (dependency != getSharedFromThis())) {
		mDependency = dependency;
		mParams->mDependsOn = dependency->mParams->mIdKey;
	} else {
		mDependency = nullptr;
		mParams->mDependsOn = "";
	}
}

// -----------------------------------------------------------------------------

int Account::getAuthFailure() const {
	return mAuthFailure;
}

bool Account::getRegisterChanged() const {
	return mRegisterChanged;
}

time_t Account::getDeletionDate() const {
	return mDeletionDate;
}

const std::string &Account::getSipEtag() const {
	return mSipEtag;
}

LinphoneCore *Account::getCore() const {
	return mCore;
}

const LinphoneErrorInfo *Account::getErrorInfo() {
	if (!mErrorInfo) mErrorInfo = linphone_error_info_new();
	linphone_error_info_from_sal_op(mErrorInfo, mOp);
	return mErrorInfo;
}

const std::shared_ptr<Address> &Account::getContactAddress() const {
	return mContactAddress;
}

const std::shared_ptr<Address> &Account::getContactAddressWithoutParams() const {
	return mContactAddressWithoutParams;
}

const std::shared_ptr<Address> &Account::getPendingContactAddress() const {
	return mPendingContactAddress;
}

const std::shared_ptr<Address> Account::getServiceRouteAddress() const {
	if (!mOp) return nullptr;

	const auto salAddr = mOp->getServiceRoute();
	if (!salAddr) return nullptr;

	if (!mServiceRouteAddress) {
		mServiceRouteAddress = Address::create();
	}
	mServiceRouteAddress->setImpl(salAddr);

	return mServiceRouteAddress;
}

LinphoneRegistrationState Account::getState() const {
	return mState;
}

SalRegisterOp *Account::getOp() const {
	return mOp;
}

const char *Account::getCustomHeader(const std::string &headerName) const {
	if (!mOp) return nullptr;

	return sal_custom_header_find(mOp->getRecvCustomHeaders(), headerName.c_str());
}

std::shared_ptr<EventPublish> Account::getPresencePublishEvent() const {
	return mPresencePublishEvent;
}

std::shared_ptr<Account> Account::getDependency() {
	return mDependency;
}

// -----------------------------------------------------------------------------

std::shared_ptr<Address> Account::guessContactForRegister() {
	std::shared_ptr<Address> result = nullptr;

	if (mDependency) {
		// In case of dependent account, force contact of 'master' account, but only after a successful register
		return mDependency->mContactAddress;
	}
	std::shared_ptr<Address> proxy = Address::create(mParams->mProxy);
	if (!proxy) return nullptr;
	const auto host = proxy->getDomain();
	if (!host.empty()) {
		result = mParams->mIdentityAddress->clone()->toSharedPtr();
		if (!mParams->mContactParameters.empty()) {
			// We want to add a list of contacts params to the linphone address
			result->setParams(mParams->mContactParameters);
		}

		bool successfullyPreparedPushParameters = false;
		auto newParams = mParams->clone()->toSharedPtr();

		if (mCore && mCore->push_notification_enabled) {
			if (!newParams->isPushNotificationAvailable()) {
				lError() << "Couldn't compute automatic push notifications parameters on account [" << this->toC()
				         << "] because account params do not have available push notifications";
			} else if (newParams->mPushNotificationAllowed || newParams->mRemotePushNotificationAllowed) {
				if (newParams->mPushNotificationConfig->getProvider().empty()) {
					bool tester_env = !!linphone_config_get_int(mCore->config, "tester", "test_env", FALSE);
					if (tester_env) newParams->mPushNotificationConfig->setProvider("liblinphone_tester");
#if TARGET_OS_IPHONE
					if (tester_env) newParams->mPushNotificationConfig->setProvider("apns.dev");
#endif
				}
				newParams->mPushNotificationConfig->generatePushParams(newParams->mPushNotificationAllowed,
				                                                       newParams->mRemotePushNotificationAllowed);
				successfullyPreparedPushParameters = true;
			}
		}

		if (!newParams->mContactUriParameters.empty()) {
			if (successfullyPreparedPushParameters) {
				// build an Address to make use of useful param management functions
				std::shared_ptr<Address> contactParamsWrapper =
				    Address::create(string("sip:dummy;" + newParams->mContactUriParameters));
				bool didRemoveParams = false;
				for (auto pushParam : newParams->mPushNotificationConfig->getPushParamsMap()) {
					string paramName = pushParam.first;
					if (!contactParamsWrapper->getUriParamValue(paramName).empty()) {
						contactParamsWrapper->removeUriParam(paramName);
						didRemoveParams = true;
						lError() << "Removing '" << paramName << "' from account [" << this
						         << "] contact uri parameters because it will be generated automatically since core "
						            "has push notification enabled";
					}
				}

				if (didRemoveParams) {
					string newContactUriParams;
					const auto &uriParamMap = contactParamsWrapper->getUriParams();
					for (const auto &param : uriParamMap) {
						if (!param.second.empty()) {
							newContactUriParams = newContactUriParams + param.first + "=" + param.second + ";";
						}
					}

					lWarning() << "Account [" << this << "] contact uri parameters changed from '"
					           << newParams->mContactUriParameters << "' to '" << newContactUriParams << "'";
					newParams->mContactUriParameters = newContactUriParams;
				}
			}
			result->setUriParams(newParams->mContactUriParameters);
		}

		if (successfullyPreparedPushParameters) {
			result->setUriParam(PushConfigPridKey, newParams->getPushNotificationConfig()->getPrid());
			result->setUriParam(PushConfigProviderKey, newParams->getPushNotificationConfig()->getProvider());
			result->setUriParam(PushConfigParamKey, newParams->getPushNotificationConfig()->getParam());

			auto &pushParams = newParams->getPushNotificationConfig()->getPushParamsMap();
			result->setUriParam(PushConfigSilentKey, pushParams.at(PushConfigSilentKey));
			result->setUriParam(PushConfigTimeoutKey, pushParams.at(PushConfigTimeoutKey));

			if (mParams->mRemotePushNotificationAllowed) {
				result->setUriParam(PushConfigMsgStrKey, newParams->getPushNotificationConfig()->getMsgStr());
				result->setUriParam(PushConfigCallStrKey, newParams->getPushNotificationConfig()->getCallStr());
				result->setUriParam(PushConfigGroupChatStrKey,
				                    newParams->getPushNotificationConfig()->getGroupChatStr());
				result->setUriParam(PushConfigCallSoundKey, newParams->getPushNotificationConfig()->getCallSnd());
				result->setUriParam(PushConfigMsgSoundKey, newParams->getPushNotificationConfig()->getMsgSnd());
			}
			lInfo() << "Added push notification informations '"
			        << newParams->getPushNotificationConfig()->asString(mParams->mRemotePushNotificationAllowed)
			        << "' added to account [" << this << "]";
			setAccountParams(newParams);
		}
	}
	return result;
}

std::list<SalAddress *> Account::getOtherContacts() {
	std::list<SalAddress *> ret;
	if (mPendingContactAddress) {
		SalAddress *toRemove = sal_address_clone(mPendingContactAddress->getImpl());
		sal_address_set_params(toRemove, "expires=0");
		ret.push_back(toRemove);
	}
	if (mParams->mCustomContact) {
		SalAddress *toAdd = sal_address_clone(mParams->mCustomContact->getImpl());
		ret.push_back(toAdd);
	}
	if (mOldParams && mOldParams->mCustomContact) {
		if (!mParams->mCustomContact || (*mOldParams->mCustomContact != *mParams->mCustomContact)) {
			/* need to remove previously used custom contact */
			SalAddress *toRemove = sal_address_clone(mOldParams->mCustomContact->getImpl());
			sal_address_set_params(toRemove, "expires=0");
			ret.push_back(toRemove);
		}
	}
	return ret;
}

void Account::registerAccount() {
	if (mParams->mRegisterEnabled) {

		std::shared_ptr<Address> proxy = Address::create(mParams->mProxy);
		if (proxy == nullptr) {
			lError() << "Can't register LinphoneAccount [" << this << "] without a proxy";
			return;
		}

		lInfo() << "LinphoneAccount [" << this
		        << "] about to register (LinphoneCore version: " << linphone_core_get_version() << ")";
		auto proxy_string = proxy->asStringUriOnly();

		if (mOp) mOp->release();
		mOp = new SalRegisterOp(mCore->sal.get());

		linphone_configure_op(mCore, mOp, mParams->mIdentityAddress->toC(), mSentHeaders, FALSE);

		std::shared_ptr<Address> contactAddress = guessContactForRegister();
		if (contactAddress) {
			mOp->setContactAddress(contactAddress->getImpl());
		}
		mOp->setUserPointer(this->toC());

		auto otherContacts = getOtherContacts();
		const auto identity = (mParams) ? mParams->getIdentity() : std::string();
		if (mOp->sendRegister(proxy_string.c_str(), identity, mParams->mExpires, otherContacts) == 0) {
			if (mPendingContactAddress) {
				mPendingContactAddress = nullptr;
			}
			setState(LinphoneRegistrationProgress, "Registration in progress");
		} else {
			setState(LinphoneRegistrationFailed, "Registration failed");
		}
		for (auto ct : otherContacts)
			sal_address_unref(ct);
	} else {
		/* unregister if registered*/
		unregister();
		if (mState == LinphoneRegistrationProgress) {
			setState(LinphoneRegistrationCleared, "Registration cleared");
		}
	}
}

void Account::refreshRegister() {
	if (!mParams) {
		lWarning() << "refreshRegister is called but no AccountParams is set on Account [" << this->toC() << "]";
		return;
	}

	if (mParams->mRegisterEnabled && mOp && mState != LinphoneRegistrationProgress) {
		if (mOp->refreshRegister(mParams->mExpires) == 0) {
			setState(LinphoneRegistrationProgress, "Refresh registration");
		}
	}
}

void Account::pauseRegister() {
	if (mOp) mOp->stopRefreshing();
}

void Account::unregister() {
	if (mOp &&
	    (mState == LinphoneRegistrationOk || (mState == LinphoneRegistrationProgress && mParams->mExpires != 0))) {
		mOp->unregister();
	}
}

void Account::unpublish() {
	if (mPresencePublishEvent &&
	    (mPresencePublishEvent->getState() == LinphonePublishOk ||
	     (mPresencePublishEvent->getState() == LinphonePublishProgress && mParams->mPublishExpires != 0))) {
		mPresencePublishEvent->unpublish();
	}
	if (!mSipEtag.empty()) {
		mSipEtag = "";
	}
}

void Account::notifyPublishStateChanged(LinphonePublishState state) {
	if (mPresencePublishEvent != nullptr) {
		switch (state) {
			case LinphonePublishCleared:
				setSipEtag("");
				BCTBX_NO_BREAK;
			case LinphonePublishError:
				mPresencePublishEvent->unref();
				mPresencePublishEvent = nullptr;
				break;
			case LinphonePublishOk: {
				const string &etag = mPresencePublishEvent->getCustomHeader("SIP-ETag");
				if (!etag.empty()) setSipEtag(etag);
				else {
					lWarning() << "SIP-Etag is missing in custom header. The server must provide it for PUBLISH.";
					setSipEtag("");
				}
				break;
			}
			default:
				break;
		}
	}
}

void Account::stopRefreshing() {
	std::shared_ptr<Address> contact_addr = nullptr;
	const SalAddress *sal_addr = mOp && mState == LinphoneRegistrationOk ? mOp->getContactAddress() : nullptr;
	if (sal_addr) {
		char *buf = sal_address_as_string(sal_addr);
		contact_addr = buf ? Address::create(buf) : nullptr;
		ms_free(buf);
	}

	/*with udp, there is a risk of port reuse, so I prefer to not do anything for now*/
	if (contact_addr) {
		if (contact_addr->getTransport() != LinphonePrivate::Transport::Udp &&
		    linphone_config_get_int(mCore->config, "sip", "unregister_previous_contact", 0)) {
			mPendingContactAddress = contact_addr;
		}
	}

	if (mPresencePublishEvent) { /*might probably do better*/
		mPresencePublishEvent->setState(LinphonePublishNone);
		mPresencePublishEvent->unref();
		mPresencePublishEvent = nullptr;
	}

	if (mOp) {
		mOp->release();
		mOp = nullptr;
	}
}

LinphoneReason Account::getError() {
	return linphone_error_info_get_reason(getErrorInfo());
}

static LinphoneTransportType salTransportToLinphoneTransport(SalTransport sal) {
	switch (sal) {
		case SalTransportUDP:
			return LinphoneTransportUdp;

		case SalTransportTCP:
			return LinphoneTransportTcp;

		case SalTransportDTLS:
			return LinphoneTransportDtls;

		case SalTransportTLS:
			return LinphoneTransportTls;
	}

	return LinphoneTransportUdp;
}

LinphoneTransportType Account::getTransport() {
	std::string addr;
	LinphoneTransportType ret = LinphoneTransportUdp; /*default value*/
	const SalAddress *route_addr = nullptr;
	if (getServiceRouteAddress()) {
		route_addr = getServiceRouteAddress()->getImpl();
	} else if (mParams && !mParams->getRoutes().empty()) {
		// get first route
		route_addr = mParams->getRoutes().front()->getImpl();
	} else if (mParams && !mParams->getServerAddressAsString().empty()) {
		route_addr = mParams->getServerAddress()->getImpl();
	} else {
		lError() << "Cannot guess transport for account with identity [" << this->toC() << "]";
		return ret;
	}
	ret = salTransportToLinphoneTransport(sal_address_get_transport(route_addr));

	return ret;
}

bool Account::isAvpfEnabled() const {
	if (!mParams) {
		lWarning() << "isAvpfEnabled is called but no AccountParams is set on Account [" << this->toC() << "]";
		return false;
	}

	if (mParams->mAvpfMode == LinphoneAVPFDefault && mCore) {
		return linphone_core_get_avpf_mode(mCore) == LinphoneAVPFEnabled;
	}

	return mParams->mAvpfMode == LinphoneAVPFEnabled;
}

const LinphoneAuthInfo *Account::findAuthInfo() const {
	if (!mParams) {
		lWarning() << "findAuthInfo is called but no AccountParams is set on Account [" << this->toC() << "]";
		return nullptr;
	}

	const std::string username = mParams->mIdentityAddress ? mParams->mIdentityAddress->getUsername() : std::string();
	const std::string domain = mParams->mIdentityAddress ? mParams->mIdentityAddress->getDomain() : std::string();
	return linphone_core_find_auth_info(mCore, mParams->mRealm.c_str(), username.c_str(), domain.c_str());
}

int Account::getUnreadChatMessageCount() const {
	if (!mParams) {
		lWarning() << "getUnreadMessageCount is called but no AccountParams is set on Account [" << this->toC() << "]";
		return -1;
	}

	return L_GET_CPP_PTR_FROM_C_OBJECT(mCore)->getUnreadChatMessageCount(mParams->mIdentityAddress);
}

void Account::writeToConfigFile(int index) {
	if (!mParams) {
		lWarning() << "writeToConfigFile is called but no AccountParams is set on Account [" << this->toC() << "]";
		return;
	}

	mParams->writeToConfigFile(mCore->config, index);
}

void Account::addCustomParam(const std::string &key, const std::string &value) {
	mParams->addCustomParam(key, value);
}

const std::string &Account::getCustomParam(const std::string &key) const {
	return mParams->getCustomParam(key);
}

bool Account::canRegister() {
	if (mCore->sip_conf.register_only_when_network_is_up && !mCore->sip_network_state.global_state) {
		return false;
	}
	if (mDependency) {
		return mDependency->getState() == LinphoneRegistrationOk;
	}
	return true;
}

int Account::done() {
	if (!check()) return -1;

	/*check if server address has changed*/
	LinphoneAccountAddressComparisonResult res = isServerConfigChanged(mOldParams, mParams);
	if (res != LinphoneAccountAddressEqual) {
		/* server config has changed, need to unregister from previous first*/
		if (mOp) {
			if (res == LinphoneAccountAddressDifferent) {
				unregister();
			}
			mOp->setUserPointer(NULL); /*we don't want to receive status for this un register*/
			mOp->unref();              /*but we keep refresher to handle authentication if needed*/
			mOp = nullptr;
		}
		if (mPresencePublishEvent) {
			if (res == LinphoneAccountAddressDifferent) {
				unpublish();
			}
		}
		mNeedToRegister = true;
	}

	if (mRegisterChanged) {
		mNeedToRegister = true;
		mRegisterChanged = false;
	}

	if (mNeedToRegister) {
		pauseRegister();
	}

	if (computePublishParamsHash()) {
		lInfo() << "Publish params have changed on account [" << this->toC() << "]";
		if (mPresencePublishEvent) {
			/*publish is terminated*/
			mPresencePublishEvent->terminate();
		}
		if (mParams->mPublishEnabled) mSendPublish = true;
	} else {
		lInfo() << "Publish params have not changed on account [" << this->toC() << "]";
	}

	if (mCore) {
		linphone_proxy_config_write_all_to_config_file(mCore); // TODO: change it when removing all proxy_config
	}

	return 0;
}

void Account::update() {
	if (mNeedToRegister) {
		if (canRegister()) {
			registerAccount();
			mNeedToRegister = false;
		}
	}
	if (mSendPublish && (mState == LinphoneRegistrationOk || mState == LinphoneRegistrationCleared)) {
		if (mPresenceModel == nullptr) {
			setPresenceModel(mCore->presence_model);
		}
		sendPublish();
		mSendPublish = false;
	}
}

void Account::apply(LinphoneCore *lc) {
	mOldParams = nullptr; // remove old params to make sure we will register since we only call apply when adding
	                      // accounts to core
	mCore = lc;

	if (mDependency != nullptr) {
		// disable register if master account is not yet registered
		if (mDependency->mState != LinphoneRegistrationOk) {
			if (mParams->mRegisterEnabled != FALSE) {
				mRegisterChanged = TRUE;
			}
			// We do not call enableRegister on purpose here
			// Explicitely disabling register on a dependent config puts it in a disabled state (see
			// cfg->reg_dependent_disabled) to avoid automatic re-enable if masterCfg reach LinphoneRegistrationOk
		}
	}

	done();
}

shared_ptr<EventPublish> Account::createPublish(const std::string event, int expires) {
	if (!mCore) {
		lError() << "Cannot create publish from account [" << this->toC() << "] not attached to any core";
		return nullptr;
	}
	return dynamic_pointer_cast<EventPublish>(
	    (new EventPublish(L_GET_CPP_PTR_FROM_C_OBJECT(mCore), getSharedFromThis(), NULL, event, expires))
	        ->getSharedFromThis());
}

void Account::setPresenceModel(LinphonePresenceModel *presence) {
	if (mPresenceModel) {
		linphone_presence_model_unref(mPresenceModel);
		mPresenceModel = nullptr;
	}
	if (presence) mPresenceModel = linphone_presence_model_ref(presence);
}

int Account::sendPublish() {
	if (mPresenceModel == nullptr) {
		lError() << "No presence model has been set for this account, can't send the PUBLISH";
		return -1;
	}

	int err = 0;
	if (mState == LinphoneRegistrationOk || mState == LinphoneRegistrationCleared) {
		int publishExpires = mParams->getPublishExpires();

		if (mPresencePublishEvent != nullptr) {
			LinphonePublishState state = mPresencePublishEvent->getState();
			if (state != LinphonePublishOk && state != LinphonePublishProgress) {
				lInfo() << "Presence publish state is [" << linphone_publish_state_to_string(state)
				        << "], destroying it and creating a new one instead";
				mPresencePublishEvent->unref();
				mPresencePublishEvent = nullptr;
			}
		}

		if (mPresencePublishEvent == nullptr) {
			mPresencePublishEvent = createPublish("presence", publishExpires);
		}

		mPresencePublishEvent->setInternal(true);
		if (publishExpires != 1) {
			// Force manual refresh mode so we can go through this method again
			// when PUBLISH is about to expire, so we can update the presence model timestamp
			mPresencePublishEvent->setManualRefresherMode(true);
		}
		const auto &identityAddress = mParams->getIdentityAddress();
		mPresencePublishEvent->setUserData(identityAddress->toC());

		LinphoneConfig *config = linphone_core_get_config(mCore);
		if (linphone_config_get_bool(config, "sip", "update_presence_model_timestamp_before_publish_expires_refresh",
		                             FALSE)) {
			unsigned int nbServices = linphone_presence_model_get_nb_services(mPresenceModel);
			if (nbServices > 0) {
				LinphonePresenceService *latest_service =
				    linphone_presence_model_get_nth_service(mPresenceModel, nbServices - 1);
				linphone_presence_service_set_timestamp(latest_service, ms_time(NULL));
			}
		}

		if (linphone_presence_model_get_presentity(mPresenceModel) == NULL) {
			lInfo() << "No presentity set for model [" << mPresenceModel << "], using identity from account ["
			        << this->toC() << "]: " << *identityAddress;
			linphone_presence_model_set_presentity(mPresenceModel, identityAddress->toC());
		}

		const auto currentPresentity = linphone_presence_model_get_presentity(mPresenceModel);
		std::shared_ptr<const Address> presentityAddress = nullptr;
		char *contact = NULL;
		if (!linphone_address_equal(currentPresentity, identityAddress->toC())) {
			lInfo() << "Presentity for model [" << mPresenceModel << "] differs account [" << this->toC()
			        << "], using account " << *identityAddress;
			presentityAddress = Address::toCpp(currentPresentity)->getSharedFromThis(); /*saved, just in case*/
			if (linphone_presence_model_get_contact(mPresenceModel)) {
				contact = bctbx_strdup(linphone_presence_model_get_contact(mPresenceModel));
			}
			linphone_presence_model_set_presentity(mPresenceModel, identityAddress->toC());
			linphone_presence_model_set_contact(mPresenceModel, NULL); /*it will be automatically computed*/
		}

		char *presence_body;
		if (!(presence_body = linphone_presence_model_to_xml(mPresenceModel))) {
			lError() << "Cannot publish presence model [" << mPresenceModel << "] for account [" << this->toC()
			         << "] because of xml serialization error";
			return -1;
		}

		if (!mSipEtag.empty()) {
			mPresencePublishEvent->addCustomHeader("SIP-If-Match", mSipEtag);
			mSipEtag = "";
		}

		LinphoneContent *content = linphone_content_new();
		linphone_content_set_buffer(content, (const uint8_t *)presence_body, strlen(presence_body));
		linphone_content_set_type(content, "application");
		linphone_content_set_subtype(content, "pidf+xml");

		err = mPresencePublishEvent->send(content);
		linphone_content_unref(content);
		ms_free(presence_body);

		if (presentityAddress) {
			lInfo() << "Restoring previous presentity address " << *presentityAddress << " for model ["
			        << mPresenceModel << "]";

			linphone_presence_model_set_presentity(mPresenceModel, presentityAddress->toC());
		}
		if (contact) {
			linphone_presence_model_set_contact(mPresenceModel, contact);
			bctbx_free(contact);
		}
	} else
		mSendPublish = true; /*otherwise do not send publish if registration is in progress, this will be done later*/

	return err;
}

bool Account::check() {
	if (mParams->mProxy.empty()) return false;
	if (mParams->mIdentityAddress == NULL) return false;
	resolveDependencies();
	return TRUE;
}

void Account::releaseOps() {
	if (mOp) {
		mOp->release();
		mOp = nullptr;
	}

	if (mPresencePublishEvent) {
		mPresencePublishEvent->terminate();
		mPresencePublishEvent->unref();
		mPresencePublishEvent = nullptr;
	}

	if (mDependency) {
		mDependency = nullptr;
	}
}

void Account::resolveDependencies() {
	if (!mCore) return;

	const bctbx_list_t *elem;
	for (elem = mCore->sip_conf.accounts; elem != NULL; elem = elem->next) {
		LinphoneAccount *account = (LinphoneAccount *)elem->data;

		LinphoneAccount *dependency = linphone_account_get_dependency(account);
		string dependsOn = Account::toCpp(account)->mParams->mDependsOn;
		if (dependency != NULL && !dependsOn.empty()) {
			LinphoneAccount *master_account = linphone_core_get_account_by_idkey(mCore, dependsOn.c_str());
			if (master_account != NULL && master_account != dependency) {
				lError() << "LinphoneAccount has a dependency but idkeys do not match: [" << dependsOn << "] != ["
				         << linphone_account_params_get_idkey(linphone_account_get_params(dependency))
				         << "], breaking dependency now.";
				linphone_account_unref(dependency);
				linphone_account_set_dependency(account, NULL);
				return;
			} else if (master_account == NULL) {
				lWarning() << "LinphoneAccount [" << account << "] depends on account [" << dependency
				           << "], which is not currently in the list.";
			}
		}
		if (!dependsOn.empty() && dependency == NULL) {
			LinphoneAccount *dependency_acc = linphone_core_get_account_by_idkey(mCore, dependsOn.c_str());

			if (dependency_acc == NULL) {
				lWarning() << "LinphoneAccount marked as dependent but no account found for idkey [" << dependsOn
				           << "]";
				return;
			} else {
				lInfo() << "LinphoneAccount [" << account << "] now depends on master LinphoneAccount ["
				        << dependency_acc << "]";
				linphone_account_set_dependency(account, dependency_acc);
			}
		}
	}
}

// -----------------------------------------------------------------------------

void Account::onInternationalPrefixChanged() {
	/* Ensure there is a default account otherwise after invalidating friends maps we won't be able to recompute phone
	 * numbers */
	/* Also it is useless to do it if the account being edited isn't the default one */
	if (mCore && this->toC() == linphone_core_get_default_account(mCore)) {
		linphone_core_invalidate_friends_maps(mCore);
	}
}

void Account::onConferenceFactoryUriChanged(const std::string &conferenceFactoryUri) {
	std::string conferenceSpec("conference/");
	conferenceSpec.append(Core::conferenceVersionAsString());
	std::string groupchatSpec("groupchat/");
	groupchatSpec.append(Core::groupChatVersionAsString());
	std::string ephemeralSpec("ephemeral/");
	ephemeralSpec.append(Core::ephemeralVersionAsString());

	if (!conferenceFactoryUri.empty()) {
		if (mCore) {
			linphone_core_add_linphone_spec(mCore, L_STRING_TO_C(conferenceSpec));
			linphone_core_add_linphone_spec(mCore, L_STRING_TO_C(groupchatSpec));
			linphone_core_add_linphone_spec(mCore, L_STRING_TO_C(ephemeralSpec));
		}
	} else if (mCore) {
		bool remove = true;
		bool removeAudioVideoConfAddress = true;
		// Check that no other account needs the specs before removing it
		for (bctbx_list_t *it = mCore->sip_conf.accounts; it; it = it->next) {
			if (it->data != this->toC()) {
				const char *confUri = linphone_account_params_get_conference_factory_uri(
				    linphone_account_get_params((LinphoneAccount *)it->data));
				if (confUri && strlen(confUri)) {
					remove = false;
					removeAudioVideoConfAddress = false;
					break;
				}

				const auto audioVideoConfUri = linphone_account_params_get_audio_video_conference_factory_address(
				    linphone_account_get_params((LinphoneAccount *)it->data));
				if (audioVideoConfUri) {
					removeAudioVideoConfAddress = false;
				}
			}
		}
		if (removeAudioVideoConfAddress) {
			linphone_core_remove_linphone_spec(mCore, L_STRING_TO_C(conferenceSpec));
		}
		if (remove) {
			linphone_core_remove_linphone_spec(mCore, L_STRING_TO_C(groupchatSpec));
			linphone_core_remove_linphone_spec(mCore, L_STRING_TO_C(ephemeralSpec));
		}
	}
}

void Account::onAudioVideoConferenceFactoryAddressChanged(
    const std::shared_ptr<Address> &audioVideoConferenceFactoryAddress) {
	std::string conferenceSpec("conference/");
	conferenceSpec.append(Core::conferenceVersionAsString());

	if (audioVideoConferenceFactoryAddress) {
		if (mCore) {
			linphone_core_add_linphone_spec(mCore, L_STRING_TO_C(conferenceSpec));
		}
	} else if (mCore) {
		bool remove = true;
		// Check that no other account needs the specs before removing it
		for (bctbx_list_t *it = mCore->sip_conf.accounts; it; it = it->next) {
			if (it->data != this->toC()) {
				const char *confUri = linphone_account_params_get_conference_factory_uri(
				    linphone_account_get_params((LinphoneAccount *)it->data));
				const auto audioVideoConfUri = linphone_account_params_get_audio_video_conference_factory_address(
				    linphone_account_get_params((LinphoneAccount *)it->data));
				if ((confUri && strlen(confUri)) || audioVideoConfUri) {
					remove = false;
					break;
				}
			}
		}
		if (remove) {
			linphone_core_remove_linphone_spec(mCore, L_STRING_TO_C(conferenceSpec));
		}
	}
}

void Account::onNatPolicyChanged(BCTBX_UNUSED(const std::shared_ptr<NatPolicy> &policy)) {
}

LinphoneProxyConfig *Account::getConfig() const {
	return mConfig;
}

void Account::setConfig(LinphoneProxyConfig *config) {
	mConfig = config;
}

LinphoneAccountAddressComparisonResult Account::isServerConfigChanged() {
	return isServerConfigChanged(mOldParams, mParams);
}

LinphoneAccountCbsRegistrationStateChangedCb AccountCbs::getRegistrationStateChanged() const {
	return mRegistrationStateChangedCb;
}

void AccountCbs::setRegistrationStateChanged(LinphoneAccountCbsRegistrationStateChangedCb cb) {
	mRegistrationStateChangedCb = cb;
}

#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif // _MSC_VER
void Account::onLimeServerUrlChanged(const std::string &limeServerUrl) {
#ifdef HAVE_LIME_X3DH
	if (!limeServerUrl.empty()) {
		if (mCore) {
			linphone_core_add_linphone_spec(mCore, "lime");
		}
	} else if (mCore) {
		// If LIME server URL is still set in the Core, do not remove the spec
		const char *core_lime_server_url = linphone_core_get_lime_x3dh_server_url(mCore);
		if (core_lime_server_url && strlen(core_lime_server_url)) {
			return;
		}

		bool remove = true;
		// Check that no other account needs the spec before removing it
		for (bctbx_list_t *it = mCore->sip_conf.accounts; it; it = it->next) {
			if (it->data != this->toC()) {
				const char *lime_server_url = linphone_account_params_get_lime_server_url(
				    linphone_account_get_params((LinphoneAccount *)it->data));
				if (lime_server_url && strlen(lime_server_url)) {
					remove = false;
					break;
				}
			}
		}
		if (remove) {
			linphone_core_remove_linphone_spec(mCore, "lime");
		}
	}

	if (mCore) {
		// If the lime server URL has changed, then propagate the change to the encryption engine
		auto encryptionEngine = L_GET_CPP_PTR_FROM_C_OBJECT(mCore)->getEncryptionEngine();
		if (encryptionEngine && (encryptionEngine->getEngineType() == EncryptionEngine::EngineType::LimeX3dh)) {
			encryptionEngine->onServerUrlChanged(getSharedFromThis(), limeServerUrl);
		}
	}

#else
	lWarning() << "Lime X3DH support is not available";
#endif
}
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif // _MSC_VER

LINPHONE_END_NAMESPACE
