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

#ifndef _L_CALL_SESSION_H_
#define _L_CALL_SESSION_H_

#include "address/address.h"
#include "conference/conference-id.h"
#include "conference/conference.h"
#include "conference/params/call-session-params.h"
#include "core/core-listener.h"
#include "linphone/misc.h"
#include "object/object.h"
#include "sal/call-op.h"

// =============================================================================

LINPHONE_BEGIN_NAMESPACE

class CallSessionPrivate;
class Content;
class Core;

namespace MediaConference {
class Conference;
}

class LINPHONE_PUBLIC CallSession : public Object, public CoreAccessor {
	friend class Call;
	friend class Core;
	friend class ClientGroupChatRoom;
	friend class ClientGroupChatRoomPrivate;
	friend class Conference;
	friend class CorePrivate;
	friend class ServerGroupChatRoom;
	friend class ServerGroupChatRoomPrivate;
	friend class ParticipantDevice;

	friend class MediaConference::Conference;
	friend class MediaConference::LocalConference;
	friend class MediaConference::RemoteConference;

public:
	L_OVERRIDE_SHARED_FROM_THIS(CallSession);

	enum class State {
		Idle = LinphoneCallStateIdle,
		IncomingReceived = LinphoneCallStateIncomingReceived,
		PushIncomingReceived = LinphoneCallStatePushIncomingReceived,
		OutgoingInit = LinphoneCallStateOutgoingInit,
		OutgoingProgress = LinphoneCallStateOutgoingProgress,
		OutgoingRinging = LinphoneCallStateOutgoingRinging,
		OutgoingEarlyMedia = LinphoneCallStateOutgoingEarlyMedia,
		Connected = LinphoneCallStateConnected,
		StreamsRunning = LinphoneCallStateStreamsRunning,
		Pausing = LinphoneCallStatePausing,
		Paused = LinphoneCallStatePaused,
		Resuming = LinphoneCallStateResuming,
		Referred = LinphoneCallStateReferred,
		Error = LinphoneCallStateError,
		End = LinphoneCallStateEnd,
		PausedByRemote = LinphoneCallStatePausedByRemote,
		UpdatedByRemote = LinphoneCallStateUpdatedByRemote,
		IncomingEarlyMedia = LinphoneCallStateIncomingEarlyMedia,
		Updating = LinphoneCallStateUpdating,
		Released = LinphoneCallStateReleased,
		EarlyUpdatedByRemote = LinphoneCallStateEarlyUpdatedByRemote,
		EarlyUpdating = LinphoneCallStateEarlyUpdating
	};
	// casting to int to get rid of the enum compare warning.
	// Here we are comparing two enums serving the same purpose
	static_assert((int)CallSession::State::EarlyUpdating == (int)LinphoneCallStateEarlyUpdating,
	              "LinphoneCallState and CallSession::State are not synchronized, fix this !");

	enum class UpdateMethod { Default = 0, Invite = 1, Update = 2 };

	enum class PredefinedSubjectType {
		Conference = 0,
		InternalUpdate = 1,
		Refresh = 2,
		MediaChange = 3,
		CallOnHold = 4,
		BothPartiesOnHold = 5
	};

	static const std::map<PredefinedSubjectType, std::string> predefinedSubject;

	CallSession(const std::shared_ptr<Core> &core, const CallSessionParams *params, CallSessionListener *listener);
	void setListener(CallSessionListener *listener);
	void setStateToEnded();
	~CallSession();

	// This virtual is a dirty hack until CallSession/MediaSession are refactored.
	virtual void acceptDefault();
	LinphoneStatus accept(const CallSessionParams *csp = nullptr);
	LinphoneStatus acceptUpdate(const CallSessionParams *csp = nullptr);
	virtual void configure(LinphoneCallDir direction,
	                       LinphoneProxyConfig *cfg,
	                       SalCallOp *op,
	                       const std::shared_ptr<Address> &from,
	                       const std::shared_ptr<Address> &to);
	void configure(LinphoneCallDir direction, const std::string &callid);
	bool isOpConfigured();
	LinphoneStatus decline(LinphoneReason reason);
	LinphoneStatus decline(const LinphoneErrorInfo *ei);
	LinphoneStatus declineNotAnswered(LinphoneReason reason);
	virtual LinphoneStatus deferUpdate();
	bool hasTransferPending();
	bool isCapabilityNegotiationEnabled() const;
	const std::list<LinphoneMediaEncryption> getSupportedEncryptions() const;
	virtual void initiateIncoming();
	virtual bool initiateOutgoing(const std::string &subject = "", const Content *content = nullptr);
	virtual void iterate(time_t currentRealTime, bool oneSecondElapsed);
	LinphoneStatus redirect(const std::string &redirectUri);
	LinphoneStatus redirect(const Address &redirectAddr);
	virtual void startIncomingNotification(bool notifyRinging = true);
	void startBasicIncomingNotification(bool notifyRinging = true);
	void startPushIncomingNotification();
	virtual int startInvite(const std::shared_ptr<Address> &destination,
	                        const std::string &subject = "",
	                        const Content *content = nullptr);
	LinphoneStatus terminate(const LinphoneErrorInfo *ei = nullptr);
	LinphoneStatus transfer(const std::shared_ptr<CallSession> &dest);
	LinphoneStatus transfer(const std::shared_ptr<Address> &dest);
	LinphoneStatus transfer(const std::string &dest);
	LinphoneStatus update(const CallSessionParams *csp,
	                      const UpdateMethod method = UpdateMethod::Default,
	                      const std::string &subject = "",
	                      const Content *content = nullptr);

	CallSessionParams *getCurrentParams() const;
	LinphoneCallDir getDirection() const;
	const std::shared_ptr<Address> getDiversionAddress() const;
	int getDuration() const;
	const LinphoneErrorInfo *getErrorInfo() const;
	const std::shared_ptr<Address> getLocalAddress() const;
	const std::shared_ptr<Address> getContactAddress() const;
	std::shared_ptr<CallLog> getLog() const;
	virtual const CallSessionParams *getParams() const;
	LinphoneReason getReason() const;
	std::shared_ptr<CallSession> getReferer() const;
	const std::string getReferTo() const;
	const std::shared_ptr<Address> &getReferToAddress() const;
	const std::shared_ptr<Address> getRemoteAddress() const;
	const std::string &getRemoteContact() const;
	const std::shared_ptr<Address> getRemoteContactAddress() const;
	const CallSessionParams *getRemoteParams();
	const std::string &getRemoteUserAgent() const;
	std::shared_ptr<CallSession> getReplacedCallSession() const;
	CallSession::State getState() const;
	const std::shared_ptr<Address> getToAddress() const;
	const std::shared_ptr<Address> getRequestAddress() const;
	CallSession::State getTransferState() const;
	std::shared_ptr<CallSession> getTransferTarget() const;
	const char *getToHeader(const std::string &name) const;

	const std::string getFromTag() const;
	const std::string getToTag() const;

	void updateContactAddressInOp();

	static bool isEarlyState(CallSession::State state);
	void accepting();
	// bool isDelinedEarly ();
	// const LinphoneErrorInfo * getErrorInfoCache () const;

	void addPendingAction(std::function<LinphoneStatus()> f);

	static bool isPredefinedSubject(const std::string &subject);

protected:
	explicit CallSession(CallSessionPrivate &p, const std::shared_ptr<Core> &core);
	CallSession::State getPreviousState() const;
	CallSession::State getLastStableState() const;
	void updateContactAddress(Address &contactAddress) const;

private:
	// bool mIsDeclining = false;
	bool mIsAccepting = false;
	// LinphoneErrorInfo *mErrorCache = nullptr;
	L_DECLARE_PRIVATE(CallSession);
	L_DISABLE_COPY(CallSession);
};

inline std::ostream &operator<<(std::ostream &str, CallSession::State state) {
	str << linphone_call_state_to_string(static_cast<LinphoneCallState>(state));
	return str;
}

LINPHONE_END_NAMESPACE

#endif // ifndef _L_CALL_SESSION_H_
