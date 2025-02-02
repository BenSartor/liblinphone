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

#ifndef _L_CLIENT_GROUP_CHAT_ROOM_H_
#define _L_CLIENT_GROUP_CHAT_ROOM_H_

#include "chat/chat-room/chat-room.h"
#include "conference/remote-conference.h"

// =============================================================================

LINPHONE_BEGIN_NAMESPACE

class ClientGroupChatRoomPrivate;
class RemoteConferenceEventHandler;
enum class SecurityLevel;

class LINPHONE_PUBLIC ClientGroupChatRoom : public ConferenceListener,
                                            public ConferenceListenerInterface,
                                            public ChatRoom {
	friend class BasicToClientGroupChatRoomPrivate;
	friend class ClientGroupToBasicChatRoomPrivate;
	friend class CorePrivate;
	friend class LimeX3dhEncryptionEngine;
	friend class MainDb;
	friend class RemoteConferenceEventHandler;

public:
	L_OVERRIDE_SHARED_FROM_THIS(ClientGroupChatRoom);

	virtual ~ClientGroupChatRoom();

	std::shared_ptr<Core> getCore() const;

	void allowCpim(bool value) override;
	void allowMultipart(bool value) override;
	bool canHandleCpim() const override;
	bool canHandleMultipart() const override;

	CapabilitiesMask getCapabilities() const override;
	ChatRoom::SecurityLevel getSecurityLevel() const override;
	ChatRoom::SecurityLevel getSecurityLevelExcept(const std::shared_ptr<ParticipantDevice> &ignoredDevice) const;
	bool hasBeenLeft() const override;
	bool isReadOnly() const override;

	const std::shared_ptr<Address> &getConferenceAddress() const override;

	void deleteFromDb() override;

	std::list<std::shared_ptr<EventLog>> getHistory(int nLast) const override;
	std::list<std::shared_ptr<EventLog>> getHistoryRange(int begin, int end) const override;
	int getHistorySize() const override;

	bool addParticipant(const std::shared_ptr<Address> &participantAddress) override;
	bool addParticipant(std::shared_ptr<Call> call) override {
		return getConference()->addParticipant(call);
	};
	bool addParticipants(const std::list<std::shared_ptr<Address>> &addresses) override;

	void join(const std::shared_ptr<Address> &participantAddress) override {
		getConference()->join(participantAddress);
	};
	bool update(const ConferenceParamsInterface &newParameters) override {
		return getConference()->update(newParameters);
	};

	bool removeParticipant(const std::shared_ptr<Participant> &participant) override;

	std::shared_ptr<Participant> findParticipant(const std::shared_ptr<Address> &addr) const override;

	std::shared_ptr<Participant> getMe() const override;
	int getParticipantCount() const override;
	const std::list<std::shared_ptr<Participant>> &getParticipants() const override;
	const std::list<std::shared_ptr<ParticipantDevice>> getParticipantDevices() const override;

	void setParticipantAdminStatus(const std::shared_ptr<Participant> &participant, bool isAdmin) override;

	const std::string &getSubject() const override;
	void setSubject(const std::string &subject) override;

	void join() override;
	void leave() override;
	void exhume();

	void enableEphemeral(bool ephem, bool updateDb) override;
	bool ephemeralEnabled() const override;
	void setEphemeralLifetime(long lifetime, bool updateDb) override;
	long getEphemeralLifetime() const override;
	void setEphemeralMode(AbstractChatRoom::EphemeralMode mode, bool updateDb) override;
	AbstractChatRoom::EphemeralMode getEphemeralMode() const override;
	bool ephemeralSupportedByAllParticipants() const override;

	const ConferenceId &getConferenceId() const override {
		return getConference()->getConferenceId();
	};

private:
	ClientGroupChatRoom(const std::shared_ptr<Core> &core,
	                    const std::shared_ptr<Address> &focus,
	                    const ConferenceId &conferenceId,
	                    const std::string &subject,
	                    const Content &content,
	                    CapabilitiesMask capabilities,
	                    const std::shared_ptr<ChatRoomParams> &params);

	ClientGroupChatRoom(const std::shared_ptr<Core> &core,
	                    const std::string &factoryUri,
	                    const std::shared_ptr<const Address> &me,
	                    const std::string &subject,
	                    CapabilitiesMask capabilities,
	                    const std::shared_ptr<ChatRoomParams> &params);

	// Create a chat room from the main database.
	ClientGroupChatRoom(const std::shared_ptr<Core> &core,
	                    const ConferenceId &conferenceId,
	                    std::shared_ptr<Participant> &me,
	                    AbstractChatRoom::CapabilitiesMask capabilities,
	                    const std::shared_ptr<ChatRoomParams> &params,
	                    const std::string &subject,
	                    std::list<std::shared_ptr<Participant>> &&participants,
	                    unsigned int lastNotifyId,
	                    bool hasBeenLeft = false);

	void sendInvite(std::shared_ptr<CallSession> &session, const std::list<std::shared_ptr<Address>> &addressList);
	void setConferenceId(const ConferenceId &conferenceId);
	void sendEphemeralUpdate();

	// TODO: Move me in ClientGroupChatRoomPrivate.
	// ALL METHODS AFTER THIS POINT.

	void onConferenceCreated(const std::shared_ptr<Address> &addr) override;
	void onConferenceKeywordsChanged(const std::vector<std::string> &keywords) override;
	void onConferenceTerminated(const std::shared_ptr<Address> &addr) override;
	void onSecurityEvent(const std::shared_ptr<ConferenceSecurityEvent> &event) override;
	void onFirstNotifyReceived(const std::shared_ptr<Address> &addr) override;
	void onFullStateReceived() override;
	void onParticipantAdded(const std::shared_ptr<ConferenceParticipantEvent> &event,
	                        const std::shared_ptr<Participant> &participant) override;
	void onParticipantDeviceAdded(const std::shared_ptr<ConferenceParticipantDeviceEvent> &event,
	                              const std::shared_ptr<ParticipantDevice> &device) override;
	void onParticipantDeviceRemoved(const std::shared_ptr<ConferenceParticipantDeviceEvent> &event,
	                                const std::shared_ptr<ParticipantDevice> &device) override;
	void onParticipantRemoved(const std::shared_ptr<ConferenceParticipantEvent> &event,
	                          const std::shared_ptr<Participant> &participant) override;
	void onParticipantSetAdmin(const std::shared_ptr<ConferenceParticipantEvent> &event,
	                           const std::shared_ptr<Participant> &participant) override;
	void onParticipantDeviceStateChanged(const std::shared_ptr<ConferenceParticipantDeviceEvent> &event,
	                                     const std::shared_ptr<ParticipantDevice> &device) override;
	void onParticipantDeviceMediaAvailabilityChanged(const std::shared_ptr<ConferenceParticipantDeviceEvent> &event,
	                                                 const std::shared_ptr<ParticipantDevice> &device) override;
	void onSubjectChanged(const std::shared_ptr<ConferenceSubjectEvent> &event) override;

	void onParticipantsCleared() override;

	void onEphemeralModeChanged(const std::shared_ptr<ConferenceEphemeralMessageEvent> &event) override;
	void onEphemeralMessageEnabled(const std::shared_ptr<ConferenceEphemeralMessageEvent> &event) override;
	void onEphemeralLifetimeChanged(const std::shared_ptr<ConferenceEphemeralMessageEvent> &event) override;

	L_DECLARE_PRIVATE(ClientGroupChatRoom);
	L_DISABLE_COPY(ClientGroupChatRoom);
};

LINPHONE_END_NAMESPACE

#endif // ifndef _L_CLIENT_GROUP_CHAT_ROOM_H_
