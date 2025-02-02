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

#ifndef _L_BASIC_CHAT_ROOM_H_
#define _L_BASIC_CHAT_ROOM_H_

#include "chat/chat-room/chat-room.h"
#include "conference/conference-id.h"

// =============================================================================

LINPHONE_BEGIN_NAMESPACE

class BasicChatRoomPrivate;

class LINPHONE_PUBLIC BasicChatRoom : public ChatRoom {
	friend class Core;
	friend class CorePrivate;

public:
	void allowCpim(bool value) override;
	void allowMultipart(bool value) override;
	bool canHandleCpim() const override;
	bool canHandleMultipart() const override;

	CapabilitiesMask getCapabilities() const override;
	bool hasBeenLeft() const override;
	bool isReadOnly() const override;

	const std::shared_ptr<Address> &getConferenceAddress() const override;

	bool removeParticipant(const std::shared_ptr<Participant> &participant) override;
	bool removeParticipants(const std::list<std::shared_ptr<Participant>> &participants) override;

	std::shared_ptr<Participant> findParticipant(const std::shared_ptr<Address> &addr) const override;

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

	const ConferenceId &getConferenceId() const override;

	bool addParticipant(const std::shared_ptr<Address> &participantAddress) override;
	bool addParticipant(std::shared_ptr<Call> call) override;
	bool addParticipants(const std::list<std::shared_ptr<Address>> &addresses) override;
	void join(const std::shared_ptr<Address> &participantAddress) override;
	bool update(const ConferenceParamsInterface &newParameters) override;

	State getState() const override;
	void setState(ConferenceInterface::State newState) override;

protected:
	explicit BasicChatRoom(BasicChatRoomPrivate &p,
	                       const std::shared_ptr<Core> &core,
	                       const ConferenceId &conferenceId,
	                       const std::shared_ptr<ChatRoomParams> &params);

private:
	BasicChatRoom(const std::shared_ptr<Core> &core,
	              const ConferenceId &conferenceId,
	              const std::shared_ptr<ChatRoomParams> &params);

	ConferenceId conferenceId;

	ConferenceInterface::State state = ConferenceInterface::State::None;

	L_DECLARE_PRIVATE(BasicChatRoom);
	L_DISABLE_COPY(BasicChatRoom);
};

LINPHONE_END_NAMESPACE

#endif // ifndef _L_BASIC_CHAT_ROOM_H_
