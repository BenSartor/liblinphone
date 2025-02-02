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

#ifndef _L_PROXY_CHAT_ROOM_H_
#define _L_PROXY_CHAT_ROOM_H_

#include "abstract-chat-room.h"

// =============================================================================

LINPHONE_BEGIN_NAMESPACE

class ChatRoom;
class ProxyChatRoomPrivate;

class LINPHONE_PUBLIC ProxyChatRoom : public AbstractChatRoom {
	friend class CorePrivate;

public:
	const ConferenceId &getConferenceId() const override;

	const std::shared_ptr<Address> &getPeerAddress() const override;
	const std::shared_ptr<Address> &getLocalAddress() const override;

	time_t getCreationTime() const override;
	time_t getLastUpdateTime() const override;

	CapabilitiesMask getCapabilities() const override;
	State getState() const override;
	SecurityLevel getSecurityLevel() const override;
	bool hasBeenLeft() const override;
	bool isReadOnly() const override;

	std::list<std::shared_ptr<EventLog>> getMessageHistory(int nLast) const override;
	std::list<std::shared_ptr<EventLog>> getMessageHistoryRange(int begin, int end) const override;
	std::list<std::shared_ptr<ChatMessage>> getUnreadChatMessages() const override;
	int getMessageHistorySize() const override;
	std::list<std::shared_ptr<EventLog>> getHistory(int nLast) const override;
	std::list<std::shared_ptr<EventLog>> getHistoryRange(int begin, int end) const override;
	int getHistorySize() const override;

	void deleteFromDb() override;
	void deleteHistory() override;
	void deleteMessageFromHistory(const std::shared_ptr<ChatMessage> &message) override;

	std::shared_ptr<ChatMessage> getLastChatMessageInHistory() const override;
	bool isEmpty() const override;
	int getChatMessageCount() const override;
	int getUnreadChatMessageCount() const override;

	void compose() override;
	bool isRemoteComposing() const override;
	std::list<std::shared_ptr<Address>> getComposingAddresses() const override;

	std::shared_ptr<ChatMessage> createChatMessage() override;
	std::shared_ptr<ChatMessage>
	createChatMessage(const std::string &text) override; // Deprecated. Text is in System Locale
	std::shared_ptr<ChatMessage> createChatMessageFromUtf8(const std::string &text) override;

	std::shared_ptr<ChatMessage> createFileTransferMessage(FileContent *content) override;
	std::shared_ptr<ChatMessage> createForwardMessage(const std::shared_ptr<ChatMessage> &msg) override;
	std::shared_ptr<ChatMessage> createReplyMessage(const std::shared_ptr<ChatMessage> &msg) override;

	std::shared_ptr<ChatMessage> findChatMessage(const std::string &messageId) const override;
	std::shared_ptr<ChatMessage> findChatMessage(const std::string &messageId,
	                                             ChatMessage::Direction direction) const override;
	virtual std::list<std::shared_ptr<ChatMessage>>
	findChatMessages(const std::list<std::string> &messageIds) const override;

	void markAsRead() override;
	void enableEphemeral(bool ephem, bool updateDb) override;
	bool ephemeralEnabled() const override;
	void setEphemeralLifetime(long lifetime, bool updateDb) override;
	long getEphemeralLifetime() const override;
	void setEphemeralMode(AbstractChatRoom::EphemeralMode mode, bool updateDb) override;
	AbstractChatRoom::EphemeralMode getEphemeralMode() const override;
	bool ephemeralSupportedByAllParticipants() const override;

	const std::shared_ptr<Address> &getConferenceAddress() const override;
	std::shared_ptr<Conference> getConference() const override;
	bool canHandleParticipants() const override;

	void allowCpim(bool value) override;
	void allowMultipart(bool value) override;
	bool canHandleCpim() const override;
	bool canHandleMultipart() const override;

	bool removeParticipant(const std::shared_ptr<Participant> &participant) override;
	bool removeParticipants(const std::list<std::shared_ptr<Participant>> &participants) override;

	std::shared_ptr<Participant> findParticipant(const std::shared_ptr<Address> &participantAddress) const override;

	std::shared_ptr<Participant> getMe() const override;
	int getParticipantCount() const override;
	const std::list<std::shared_ptr<Participant>> &getParticipants() const override;
	const std::list<std::shared_ptr<ParticipantDevice>> getParticipantDevices() const override;

	void setParticipantAdminStatus(const std::shared_ptr<Participant> &participant, bool isAdmin) override;

	const std::string &getSubject() const override;
	void setSubject(const std::string &subject) override;

	// TODO: Delete
	// Addressing compilation error -Werror=overloaded-virtual
	using LinphonePrivate::ConferenceInterface::join;
	void join() override;
	void leave() override;

	const std::shared_ptr<ChatRoomParams> &getCurrentParams() const override;

	const std::shared_ptr<AbstractChatRoom> &getProxiedChatRoom() const;

	void addListener(std::shared_ptr<ConferenceListenerInterface> listener) override {
		chatListeners.push_back(listener);
	}

	bool addParticipant(const std::shared_ptr<Address> &participantAddress) override;
	bool addParticipant(std::shared_ptr<Call> call) override;
	bool addParticipants(const std::list<std::shared_ptr<Address>> &addresses) override;
	void join(const std::shared_ptr<Address> &participantAddress) override;
	bool update(const ConferenceParamsInterface &newParameters) override;

	void setState(ConferenceInterface::State state) override;

	uint32_t getChar() override;
	std::shared_ptr<Call> getCall() const override;

protected:
	ProxyChatRoom(ProxyChatRoomPrivate &p, const std::shared_ptr<ChatRoom> &chatRoom);

	std::list<std::shared_ptr<ConferenceListenerInterface>> chatListeners;

private:
	L_DECLARE_PRIVATE(ProxyChatRoom);
	L_DISABLE_COPY(ProxyChatRoom);
};

LINPHONE_END_NAMESPACE

#endif // ifndef _L_PROXY_CHAT_ROOM_H_
