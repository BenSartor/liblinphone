/*
 * Copyright (c) 2010-2023 Belledonne Communications SARL.
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

#ifndef _L_ACCOUNT_H_
#define _L_ACCOUNT_H_

#include <memory>

#include "account-params.h"
#include "c-wrapper/c-wrapper.h"
#include "c-wrapper/internal/c-sal.h"
#include "event/event-publish.h"
#include "linphone/api/c-types.h"
#include "sal/register-op.h"

// =============================================================================

LINPHONE_BEGIN_NAMESPACE

typedef enum _LinphoneAccountAddressComparisonResult {
	LinphoneAccountAddressDifferent,
	LinphoneAccountAddressEqual,
	LinphoneAccountAddressWeakEqual
} LinphoneAccountAddressComparisonResult;

class AccountCbs;

class Account : public bellesip::HybridObject<LinphoneAccount, Account>,
                public UserDataAccessor,
                public CallbacksHolder<AccountCbs> {
public:
	Account(LinphoneCore *lc, std::shared_ptr<AccountParams> params);
	Account(LinphoneCore *lc, std::shared_ptr<AccountParams> params, LinphoneProxyConfig *config);
	Account(const Account &other);
	virtual ~Account();

	Account *clone() const override;

	// Account params configuration
	LinphoneStatus setAccountParams(std::shared_ptr<AccountParams> params);
	std::shared_ptr<const AccountParams> getAccountParams() const;

	// Setters
	void setAuthFailure(int authFailure);
	void setRegisterChanged(bool registerChanged);
	void setSendPublish(bool sendPublish);
	void setNeedToRegister(bool needToRegister);
	void setDeletionDate(time_t deletionDate);
	void setSipEtag(const std::string &sipEtag);
	void setCore(LinphoneCore *lc);
	void setErrorInfo(LinphoneErrorInfo *errorInfo);
	void setContactAddress(const std::shared_ptr<Address> contact);
	void setContactAddressWithoutParams(const std::shared_ptr<Address> contact);
	void setPendingContactAddress(std::shared_ptr<Address> contact);
	void setServiceRouteAddress(std::shared_ptr<Address> serviceRoute);
	void setState(LinphoneRegistrationState state, const std::string &message);
	void setOp(SalRegisterOp *op);
	void setCustomheader(const std::string &headerName, const std::string &headerValue);
	void setPresencePublishEvent(const std::shared_ptr<EventPublish> &presencePublishEvent);
	void setDependency(std::shared_ptr<Account> dependency);

	// Getters
	int getAuthFailure() const;
	bool getRegisterChanged() const;
	time_t getDeletionDate() const;
	const std::string &getSipEtag() const;
	LinphoneCore *getCore() const;
	const LinphoneErrorInfo *getErrorInfo();
	const std::shared_ptr<Address> &getContactAddress() const;
	const std::shared_ptr<Address> &getContactAddressWithoutParams() const;
	const std::shared_ptr<Address> &getPendingContactAddress() const;
	const std::shared_ptr<Address> getServiceRouteAddress() const;
	LinphoneRegistrationState getState() const;
	SalRegisterOp *getOp() const;
	const char *getCustomHeader(const std::string &headerName) const;
	std::shared_ptr<EventPublish> getPresencePublishEvent() const;
	std::shared_ptr<Account> getDependency();

	// Other
	bool check();
	bool isAvpfEnabled() const;
	int getUnreadChatMessageCount() const;
	void setPresenceModel(LinphonePresenceModel *presence);
	int sendPublish();
	void apply(LinphoneCore *lc);
	void notifyPublishStateChanged(LinphonePublishState state);
	void pauseRegister();
	void refreshRegister();
	void registerAccount();
	void releaseOps();
	void stopRefreshing();
	void unpublish();
	void unregister();
	void update();
	void addCustomParam(const std::string &key, const std::string &value);
	const std::string &getCustomParam(const std::string &key) const;
	void writeToConfigFile(int index);
	const LinphoneAuthInfo *findAuthInfo() const;
	std::shared_ptr<EventPublish> createPublish(const std::string event, int expires);
	LinphoneReason getError();
	LinphoneTransportType getTransport();

	// Callbacks

	// Utils
	static LinphoneAccountAddressComparisonResult compareLinphoneAddresses(const std::shared_ptr<Address> &a,
	                                                                       const std::shared_ptr<Address> &b);

	// To be removed when not using proxy config anymore
	LinphoneProxyConfig *getConfig() const;
	void setConfig(LinphoneProxyConfig *config);
	LinphoneAccountAddressComparisonResult isServerConfigChanged();

private:
	bool canRegister();
	bool computePublishParamsHash();
	int done();
	void applyParamsChanges();
	void resolveDependencies();
	void updateDependentAccount(LinphoneRegistrationState state, const std::string &message);
	LinphoneAccountAddressComparisonResult isServerConfigChanged(std::shared_ptr<AccountParams> oldParams,
	                                                             std::shared_ptr<AccountParams> newParams);
	std::shared_ptr<Address> guessContactForRegister();

	void onInternationalPrefixChanged();
	void onConferenceFactoryUriChanged(const std::string &conferenceFactoryUri);
	void
	onAudioVideoConferenceFactoryAddressChanged(const std::shared_ptr<Address> &audioVideoConferenceFactoryAddress);
	void onNatPolicyChanged(const std::shared_ptr<NatPolicy> &policy);
	void onLimeServerUrlChanged(const std::string &limeServerUrl);
	bool customContactChanged();
	std::list<SalAddress *> getOtherContacts();

	std::shared_ptr<AccountParams> mParams;

	int mAuthFailure;

	bool mNeedToRegister = false;
	bool mRegisterChanged = false;
	bool mSendPublish = false;

	time_t mDeletionDate;

	std::string mSipEtag;

	LinphoneCore *mCore = nullptr;

	LinphoneErrorInfo *mErrorInfo = nullptr;

	std::shared_ptr<Address> mContactAddress = nullptr;
	std::shared_ptr<Address> mContactAddressWithoutParams = nullptr;
	std::shared_ptr<Address> mPendingContactAddress = nullptr;
	mutable std::shared_ptr<Address> mServiceRouteAddress = nullptr;

	LinphoneRegistrationState mState = LinphoneRegistrationNone;

	SalRegisterOp *mOp = nullptr;
	SalCustomHeader *mSentHeaders = nullptr;

	std::shared_ptr<EventPublish> mPresencePublishEvent = nullptr;
	LinphonePresenceModel *mPresenceModel = nullptr;

	std::shared_ptr<Account> mDependency = nullptr;

	unsigned long long mPreviousPublishParamsHash[2] = {0};
	std::shared_ptr<AccountParams> mOldParams;

	// This is a back pointer intended to keep both LinphoneProxyConfig and Account
	// api to be usable at the same time. This should be removed as soon as
	// proxy configs can be replaced.
	LinphoneProxyConfig *mConfig = nullptr;
};

class AccountCbs : public bellesip::HybridObject<LinphoneAccountCbs, AccountCbs>, public Callbacks {
public:
	LinphoneAccountCbsRegistrationStateChangedCb getRegistrationStateChanged() const;
	void setRegistrationStateChanged(LinphoneAccountCbsRegistrationStateChangedCb cb);

private:
	LinphoneAccountCbsRegistrationStateChangedCb mRegistrationStateChangedCb = nullptr;
};

LINPHONE_END_NAMESPACE

#endif // ifndef _L_ACCOUNT_H_
