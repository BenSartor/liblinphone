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

#ifndef _L_ABSTRACT_CHAT_ROOM_H_
#define _L_ABSTRACT_CHAT_ROOM_H_

#include <bctoolbox/defs.h>

#include "linphone/utils/enum-mask.h"

#include "chat/chat-message/chat-message.h"
#include "conference/conference-interface.h"
#include "core/core-accessor.h"

// =============================================================================

LINPHONE_BEGIN_NAMESPACE

class AbstractChatRoomPrivate;
class ConferenceId;
class EventLog;
class ChatRoomParams;

class LINPHONE_PUBLIC AbstractChatRoom : public Object, public CoreAccessor, public ConferenceInterface {
	friend class Call;
	friend class ChatMessage;
	friend class ChatMessagePrivate;
	friend class ClientGroupToBasicChatRoomPrivate;
	friend class Core;
	friend class CorePrivate;
	friend class MainDb;
	friend class ProxyChatRoomPrivate;

public:
	L_OVERRIDE_SHARED_FROM_THIS(AbstractChatRoom);

	/*enum class is used to create namespaces for Enums
	  doing this prevents the compiler to confuse members from different Enums with same name.
	    i.e. "None" for the "State" Enum and "None" from the Capabilities Enum */
	enum class Capabilities {
		None = LinphoneChatRoomCapabilitiesNone,
		Basic = LinphoneChatRoomCapabilitiesBasic,
		RealTimeText = LinphoneChatRoomCapabilitiesRealTimeText,
		Conference = LinphoneChatRoomCapabilitiesConference,
		Proxy = LinphoneChatRoomCapabilitiesProxy,
		Migratable = LinphoneChatRoomCapabilitiesMigratable,
		OneToOne = LinphoneChatRoomCapabilitiesOneToOne,
		Encrypted = LinphoneChatRoomCapabilitiesEncrypted,
		Ephemeral = 1 << 7 // Entered like this to check enum order in static_assert below
	};

	enum class SecurityLevel {
		Unsafe = LinphoneChatRoomSecurityLevelUnsafe,
		ClearText = LinphoneChatRoomSecurityLevelClearText,
		Encrypted = LinphoneChatRoomSecurityLevelEncrypted,
		Safe
	};

	enum class EphemeralMode {
		DeviceManaged = LinphoneChatRoomEphemeralModeDeviceManaged,
		AdminManaged = LinphoneChatRoomEphemeralModeAdminManaged
	};

	typedef enum EphemeralMode ChatRoomEphemeralMode;

	// casting to int to get rid of the enum compare warning.
	// Here we are comparing two enums serving the same purpose
	//  char as in byte
	static_assert((char)AbstractChatRoom::Capabilities::Encrypted == (char)LinphoneChatRoomCapabilitiesEncrypted,
	              "LinphoneChatRoomCapabilities and AbstractChatRoom::Capabilities are not synchronized, fix this !");

	// casting to int to get rid of the enum compare warning.
	// Here we are comparing two enums serving the same purpose
	static_assert(
	    (int)AbstractChatRoom::SecurityLevel::Safe == (int)LinphoneChatRoomSecurityLevelSafe,
	    "LinphoneEncryptionEngineSecurityLevel and AbstractChatRoom::SecurityLevel are not synchronized, fix this !");

	typedef EnumMask<Capabilities> CapabilitiesMask;

	virtual void allowCpim(bool value) = 0;
	virtual void allowMultipart(bool value) = 0;
	virtual bool canHandleCpim() const = 0;
	virtual bool canHandleMultipart() const = 0;

	virtual const std::shared_ptr<Address> &getPeerAddress() const = 0;
	virtual const std::shared_ptr<Address> &getLocalAddress() const = 0;

	virtual time_t getCreationTime() const = 0;
	virtual time_t getLastUpdateTime() const = 0;

	virtual CapabilitiesMask getCapabilities() const = 0;
	virtual SecurityLevel getSecurityLevel() const = 0;
	virtual bool hasBeenLeft() const = 0;
	virtual bool isReadOnly() const = 0;

	virtual std::list<std::shared_ptr<EventLog>> getMessageHistory(int nLast) const = 0;
	virtual std::list<std::shared_ptr<EventLog>> getMessageHistoryRange(int begin, int end) const = 0;
	virtual std::list<std::shared_ptr<ChatMessage>> getUnreadChatMessages() const = 0;
	virtual int getMessageHistorySize() const = 0;
	virtual std::list<std::shared_ptr<EventLog>> getHistory(int nLast) const = 0;
	virtual std::list<std::shared_ptr<EventLog>> getHistoryRange(int begin, int end) const = 0;
	virtual int getHistorySize() const = 0;

	virtual void deleteFromDb() = 0;
	virtual void deleteHistory() = 0;
	virtual void deleteMessageFromHistory(const std::shared_ptr<ChatMessage> &message) = 0;

	virtual std::shared_ptr<ChatMessage> getLastChatMessageInHistory() const = 0;
	virtual bool isEmpty() const = 0;
	virtual int getChatMessageCount() const = 0;
	virtual int getUnreadChatMessageCount() const = 0;

	virtual void compose() = 0;
	virtual bool isRemoteComposing() const = 0;
	virtual std::list<std::shared_ptr<Address>> getComposingAddresses() const = 0;

	virtual std::shared_ptr<ChatMessage> createChatMessage() = 0;
	virtual std::shared_ptr<ChatMessage> createChatMessage(const std::string &text) = 0;
	virtual std::shared_ptr<ChatMessage> createChatMessageFromUtf8(const std::string &text) = 0;

	virtual std::shared_ptr<ChatMessage> createFileTransferMessage(FileContent *content) = 0;
	virtual std::shared_ptr<ChatMessage> createForwardMessage(const std::shared_ptr<ChatMessage> &msg) = 0;
	virtual std::shared_ptr<ChatMessage> createReplyMessage(const std::shared_ptr<ChatMessage> &msg) = 0;

	virtual std::shared_ptr<ChatMessage> findChatMessage(const std::string &messageId) const = 0;
	virtual std::shared_ptr<ChatMessage> findChatMessage(const std::string &messageId,
	                                                     ChatMessage::Direction direction) const = 0;
	virtual std::list<std::shared_ptr<ChatMessage>>
	findChatMessages(const std::list<std::string> &messageIds) const = 0;

	virtual void markAsRead() = 0;
	virtual void enableEphemeral(bool ephem, bool updateDb) = 0;
	virtual bool ephemeralEnabled() const = 0;
	virtual void setEphemeralLifetime(long lifetime, bool updateDb) = 0;
	virtual long getEphemeralLifetime() const = 0;
	virtual void setEphemeralMode(AbstractChatRoom::EphemeralMode mode, bool updateDb) = 0;
	virtual AbstractChatRoom::EphemeralMode getEphemeralMode() const = 0;
	virtual bool ephemeralSupportedByAllParticipants() const = 0;

	virtual const std::shared_ptr<ChatRoomParams> &getCurrentParams() const = 0;

	virtual bool update(BCTBX_UNUSED(const ConferenceParamsInterface &newParameters)) override {
		return false;
	};

	virtual bool canHandleParticipants() const = 0;
	virtual std::shared_ptr<Conference> getConference() const = 0;

	virtual uint32_t getChar() = 0;
	virtual std::shared_ptr<Call> getCall() const = 0;

	virtual void setUtf8Subject(const std::string &subject) override;
	virtual const std::string getUtf8Subject() const override;

protected:
	explicit AbstractChatRoom(AbstractChatRoomPrivate &p, const std::shared_ptr<Core> &core);

private:
	L_DECLARE_PRIVATE(AbstractChatRoom);
	L_DISABLE_COPY(AbstractChatRoom);
};

std::ostream &operator<<(std::ostream &lhs, AbstractChatRoom::Capabilities e);

std::ostream &operator<<(std::ostream &lhs, AbstractChatRoom::SecurityLevel e);

std::ostream &operator<<(std::ostream &lhs, AbstractChatRoom::EphemeralMode e);

LINPHONE_END_NAMESPACE

#endif // ifndef _L_ABSTRACT_CHAT_ROOM_H_
