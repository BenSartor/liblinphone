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

#ifndef _L_PARTICIPANT_H_
#define _L_PARTICIPANT_H_

#include <ctime>
#include <list>

#include <belle-sip/object++.hh>

#include "address/address.h"
#include "chat/chat-room/abstract-chat-room.h"
#include "conference/params/call-session-params.h"

#include "conference/params/call-session-params.h"
#include "conference/participant-device.h"
#include "conference/session/call-session-listener.h"
#include "conference/session/call-session.h"

// =============================================================================

class LocalConferenceTester;

namespace LinphoneTest {
class LocalConferenceTester;
}

LINPHONE_BEGIN_NAMESPACE

namespace MediaConference {
class Conference;
class LocalConference;
class RemoteConference;
} // namespace MediaConference

class ClientGroupChatRoom;
class Conference;

class LINPHONE_PUBLIC Participant : public bellesip::HybridObject<LinphoneParticipant, Participant> {
	// TODO: Remove... It's ugly.
	friend class Call;
	friend class ClientGroupChatRoom;
	friend class ClientGroupChatRoomPrivate;
	friend class Core;
	friend class CorePrivate;
	friend class Conference;
	friend class MediaConference::Conference;
	friend class MediaConference::LocalConference;
	friend class MediaConference::RemoteConference;
	friend class LimeX3dhEncryptionEngine;
	friend class LocalConference;
	friend class LocalConferenceEventHandler;
	friend class LocalConferenceListEventHandler;
	friend class MainDb;
	friend class MainDbPrivate;
	friend class MediaSessionPrivate;
	friend class ParticipantDevice;
	friend class RemoteConference;
	friend class RemoteConferenceEventHandler;
	friend class ServerGroupChatRoom;
	friend class ServerGroupChatRoomPrivate;

	friend class LinphoneTest::LocalConferenceTester;
	friend class ::LocalConferenceTester;

public:
	explicit Participant(Conference *conference,
	                     const std::shared_ptr<Address> &address,
	                     std::shared_ptr<CallSession> callSession);
	explicit Participant(Conference *conference, const std::shared_ptr<Address> &address);
	Participant();
	virtual ~Participant();
	// non clonable object
	Participant *clone() const override {
		return nullptr;
	}

	void configure(Conference *conference, const std::shared_ptr<Address> &address);

	const std::shared_ptr<Address> &getAddress() const;
	AbstractChatRoom::SecurityLevel getSecurityLevel() const;
	AbstractChatRoom::SecurityLevel
	getSecurityLevelExcept(const std::shared_ptr<ParticipantDevice> &ignoredDevice) const;

	const std::list<std::shared_ptr<ParticipantDevice>> &getDevices() const;
	std::shared_ptr<ParticipantDevice> findDevice(const std::shared_ptr<Address> &address,
	                                              const bool logFailure = true) const;
	std::shared_ptr<ParticipantDevice> findDevice(const std::shared_ptr<const CallSession> &session,
	                                              const bool logFailure = true) const;
	std::shared_ptr<ParticipantDevice> findDevice(const std::string &label, const bool logFailure = true) const;
	std::shared_ptr<ParticipantDevice> findDeviceByCallId(const std::string &callId,
	                                                      const bool logFailure = true) const;

	inline void setAdmin(bool isAdmin) {
		this->isThisAdmin = isAdmin;
	}
	bool isAdmin() const;

	inline void setFocus(bool isFocus) {
		this->isThisFocus = isFocus;
	}
	bool isFocus() const;

	inline void setPreserveSession(bool preserve) {
		this->preserveSession = preserve;
	}
	bool getPreserveSession() const;

	time_t getCreationTime() const;

	void *getUserData() const;
	void setUserData(void *ud);

protected:
	std::shared_ptr<Core> getCore() const {
		return mConference ? mConference->getCore() : nullptr;
	}
	Conference *getConference() const {
		return mConference;
	}
	void setConference(Conference *conference) {
		mConference = conference;
	}

	std::shared_ptr<CallSession> createSession(const Conference &conference,
	                                           const CallSessionParams *params,
	                                           bool hasMedia,
	                                           CallSessionListener *listener);

	// TODO: Delete
	// Temporary method to unify audio video conference and conference codes for group chats
	std::shared_ptr<CallSession> createSession(const std::shared_ptr<Core> &core,
	                                           const CallSessionParams *params,
	                                           bool hasMedia,
	                                           CallSessionListener *listener);
	inline void setSession(std::shared_ptr<CallSession> callSession) {
		session = callSession;
	}
	inline std::shared_ptr<CallSession> getSession() const {
		return session;
	}
	inline void removeSession() {
		session.reset();
	}
	inline void setAddress(const std::shared_ptr<Address> &addr) {
		this->addr = addr;
	}

	std::shared_ptr<ParticipantDevice> addDevice(const std::shared_ptr<LinphonePrivate::CallSession> &session,
	                                             const std::string &name = "");
	std::shared_ptr<ParticipantDevice> addDevice(const std::shared_ptr<Address> &address, const std::string &name = "");
	void clearDevices();
	void removeDevice(const std::shared_ptr<Address> &gruu);
	void removeDevice(const std::shared_ptr<const CallSession> &session);

private:
	Conference *mConference = nullptr;
	std::shared_ptr<Address> addr;
	bool isThisAdmin = false;
	bool isThisFocus = false;
	std::shared_ptr<CallSession> session;
	std::list<std::shared_ptr<ParticipantDevice>> devices;
	time_t creationTime;
	bool preserveSession = false;

	void *mUserData = nullptr;

	L_DISABLE_COPY(Participant);
};

inline std::ostream &operator<<(std::ostream &os, const Participant &participant) {
	return os << participant.getAddress()->toString();
	return os;
}

LINPHONE_END_NAMESPACE

#endif // ifndef _L_PARTICIPANT_H_
