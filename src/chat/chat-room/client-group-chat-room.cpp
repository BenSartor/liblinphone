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

#include <algorithm>

#include <bctoolbox/defs.h>

#include "linphone/utils/utils.h"

#include "address/address.h"
#include "basic-to-client-group-chat-room.h"
#include "c-wrapper/c-wrapper.h"
#include "call/call.h"
#include "chat/chat-message/chat-message-p.h"
#include "client-group-chat-room-p.h"
#include "conference/handlers/remote-conference-event-handler.h"
#include "conference/handlers/remote-conference-list-event-handler.h"
#include "conference/participant-device.h"
#include "conference/participant.h"
#include "conference/remote-conference.h"
#include "conference/session/call-session-p.h"
#include "content/content-disposition.h"
#include "content/content-type.h"
#include "core/core-p.h"
#include "factory/factory.h"
#include "logger/logger.h"
#include "sal/refer-op.h"

// =============================================================================

using namespace std;

LINPHONE_BEGIN_NAMESPACE

// -----------------------------------------------------------------------------
// Removes own address and existing participants from the list.
// Also removes gruu from kept addresses
list<std::shared_ptr<Address>>
ClientGroupChatRoomPrivate::cleanAddressesList(const list<std::shared_ptr<Address>> &addresses) const {
	L_Q();
	list<std::shared_ptr<Address>> cleanedList(addresses);

	cleanedList.sort([](const auto &addr1, const auto &addr2) { return *addr1 < *addr2; });
	cleanedList.unique([](const auto &addr1, const auto &addr2) { return addr1->weakEqual(*addr2); });
	for (auto it = cleanedList.begin(); it != cleanedList.end();) {
		if (q->findParticipant(*it) || (*q->getMe()->getAddress() == **it)) {
			it = cleanedList.erase(it);
		} else {
			it++;
		}
	}
	return cleanedList;
}

shared_ptr<CallSession> ClientGroupChatRoomPrivate::createSessionTo(std::shared_ptr<Address> sessionTo) {
	L_Q();

	CallSessionParams csp;
	csp.addCustomHeader("Require", "recipient-list-invite");
	csp.addCustomContactParameter("text");
	if (capabilities & ClientGroupChatRoom::Capabilities::OneToOne) csp.addCustomHeader("One-To-One-Chat-Room", "true");
	if (capabilities & ClientGroupChatRoom::Capabilities::Encrypted)
		csp.addCustomHeader("End-To-End-Encrypted", "true");
	if (capabilities & ClientGroupChatRoom::Capabilities::Ephemeral) {
		csp.addCustomHeader("Ephemerable", "true");
		csp.addCustomHeader("Ephemeral-Life-Time", to_string(params->getEphemeralLifetime()));
	}

	shared_ptr<Participant> &focus = static_pointer_cast<RemoteConference>(q->getConference())->focus;
	shared_ptr<CallSession> session = focus->createSession(*q->getConference().get(), &csp, false, callSessionListener);
	std::shared_ptr<Address> meCleanedAddress = Address::create(q->getMe()->getAddress()->getUriWithoutGruu());

	session->configure(LinphoneCallOutgoing, nullptr, nullptr, meCleanedAddress, sessionTo);
	session->initiateOutgoing();
	session->getPrivate()->createOp();

	return session;
}

shared_ptr<CallSession> ClientGroupChatRoomPrivate::createSession() {
	L_Q();
	const std::shared_ptr<Address> &peerAddress(q->getConferenceId().getPeerAddress());
	shared_ptr<Participant> &focus = static_pointer_cast<RemoteConference>(q->getConference())->focus;
	const std::shared_ptr<Address> sessionTo =
	    (peerAddress && peerAddress->isValid()) ? peerAddress : focus->getAddress();
	return createSessionTo(sessionTo);
}

void ClientGroupChatRoomPrivate::notifyReceived(const Content &content) {
	L_Q();
	static_pointer_cast<RemoteConference>(q->getConference())->eventHandler->notifyReceived(content);
}

void ClientGroupChatRoomPrivate::multipartNotifyReceived(const Content &content) {
	L_Q();
	static_pointer_cast<RemoteConference>(q->getConference())->eventHandler->multipartNotifyReceived(content);
}

// -----------------------------------------------------------------------------

void ClientGroupChatRoomPrivate::setCallSessionListener(CallSessionListener *listener) {
	L_Q();

	callSessionListener = listener;
	shared_ptr<CallSession> session = static_pointer_cast<RemoteConference>(q->getConference())->focus->getSession();
	if (session) session->getPrivate()->setCallSessionListener(listener);
	for (const auto &participant : q->getParticipants()) {
		session = participant->getSession();
		if (session) session->getPrivate()->setCallSessionListener(listener);
	}
}

void ClientGroupChatRoomPrivate::addOneToOneCapability() {
	capabilities |= ClientGroupChatRoom::Capabilities::OneToOne;
}

unsigned int ClientGroupChatRoomPrivate::getLastNotifyId() const {
	L_Q();
	return q->getConference()->getLastNotify();
}

void ClientGroupChatRoomPrivate::enableEphemeral(bool ephem) {
	isEphemeral = ephem;
}

// -----------------------------------------------------------------------------

void ClientGroupChatRoomPrivate::confirmJoining(SalCallOp *op) {
	L_Q();

	auto focus = static_pointer_cast<RemoteConference>(q->getConference())->focus;
	auto focusSession = focus->getSession();
	bool previousSession = (focusSession != nullptr);

	auto from = Address::create(op->getFrom());
	auto to = Address::create(op->getTo());
	shared_ptr<AbstractChatRoom> exhumedChatRoom =
	    q->getCore()->getPrivate()->findExumedChatRoomFromPreviousConferenceId(ConferenceId(from, to));

	if (previousSession && !exhumedChatRoom) {
		// Prevents leak
		auto focusOp = focusSession->getPrivate()->getOp();
		lInfo() << "Releasing focus session " << focusSession << " (from: " << *from << " to " << *to << ")";
		focusOp->terminate();
		focusOp->release();
	}

	auto session = focus->createSession(*q->getConference().get(), nullptr, false, this);
	session->configure(LinphoneCallIncoming, nullptr, op, from, to);
	session->startIncomingNotification(false);

	bool found = false;
	for (auto it = previousConferenceIds.begin(); it != previousConferenceIds.end(); it++) {
		ConferenceId confId = static_cast<ConferenceId>(*it);
		if (*confId.getPeerAddress() == Address(op->getRemoteContact())) {
			found = true;
			break;
		}
	}

	// If INVITE is for a previous conference ID, only accept the session to acknowledge the BYE
	if (!previousSession && !found) {
		q->setState(ConferenceInterface::State::CreationPending);
		// Handle participants addition
		list<std::shared_ptr<Address>> identAddresses = Utils::parseResourceLists(op->getRemoteBody());
		for (const auto &addr : identAddresses) {
			auto participant = q->findParticipant(addr);
			if (!participant) {
				participant = Participant::create(q->getConference().get(), addr);
				q->getConference()->participants.push_back(participant);
			}
		}
	}

	acceptSession(session);
}

// -----------------------------------------------------------------------------

void ClientGroupChatRoomPrivate::onChatRoomInsertRequested(const shared_ptr<AbstractChatRoom> &chatRoom) {
	L_Q();
	q->getCore()->getPrivate()->insertChatRoom(chatRoom);
}

void ClientGroupChatRoomPrivate::onChatRoomInsertInDatabaseRequested(const shared_ptr<AbstractChatRoom> &chatRoom) {
	L_Q();

	unsigned int notifyId = q->getConference()->getLastNotify();
	q->getCore()->getPrivate()->insertChatRoomWithDb(chatRoom, notifyId);
}

void ClientGroupChatRoomPrivate::onChatRoomDeleteRequested(const shared_ptr<AbstractChatRoom> &chatRoom) {
	L_Q();
	q->getCore()->deleteChatRoom(chatRoom);
	q->setState(ConferenceInterface::State::Deleted);
}

bool ClientGroupChatRoomPrivate::isSubscriptionUnderWay() const {
	L_Q();

	bool underWay = false;
	if (q->getCore()->getPrivate()->remoteListEventHandler->findHandler(q->getConferenceId())) {
		underWay = q->getCore()->getPrivate()->remoteListEventHandler->getInitialSubscriptionUnderWayFlag(
		    q->getConferenceId());
	} else {
		underWay = static_pointer_cast<RemoteConference>(q->getConference())
		               ->eventHandler->getInitialSubscriptionUnderWayFlag();
	}

	return underWay;
}

std::pair<bool, shared_ptr<AbstractChatRoom>> ClientGroupChatRoomPrivate::needToMigrate() const {
	L_Q();

	bool performMigration = false;
	shared_ptr<AbstractChatRoom> chatRoom = nullptr;
	if (q->getCore()->getPrivate()->basicToFlexisipChatroomMigrationEnabled() && (q->getParticipantCount() == 1) &&
	    (capabilities & ClientGroupChatRoom::Capabilities::OneToOne)) {
		chatRoom = q->getCore()->findOneToOneChatRoom(q->getMe()->getAddress(),
		                                              q->getParticipants().front()->getAddress(), true, false,
		                                              capabilities & ClientGroupChatRoom::Capabilities::Encrypted);

		if (chatRoom) {
			auto oneToOneCapabilities = chatRoom->getCapabilities();

			if ((oneToOneCapabilities & ChatRoom::Capabilities::Basic) &&
			    (oneToOneCapabilities & ChatRoom::Capabilities::Migratable)) {
				performMigration = true;
			}
		}
	}

	return std::make_pair(performMigration, chatRoom);
}

// -----------------------------------------------------------------------------

void ClientGroupChatRoomPrivate::onCallSessionSetReleased(const shared_ptr<CallSession> &session) {
	L_Q();

	shared_ptr<Participant> participant = static_pointer_cast<RemoteConference>(q->getConference())->focus;
	if (session == participant->getSession()) participant->removeSession();
}

void ClientGroupChatRoomPrivate::onCallSessionStateChanged(const shared_ptr<CallSession> &session,
                                                           CallSession::State newState,
                                                           const string &message) {
	L_Q();
	const auto ref = q->getSharedFromThis();
	const auto proxyChatRoomRef = proxyChatRoom ? proxyChatRoom->getSharedFromThis() : nullptr;
	if (newState == CallSession::State::Connected) {
		if (q->getState() == ConferenceInterface::State::CreationPending) {
			auto migration = needToMigrate();
			if (localExhumePending) {
				onLocallyExhumedConference(session->getRemoteContactAddress());
			} else {
				onChatRoomCreated(session->getRemoteContactAddress());
			}

			if (!migration.first) {
				chatRoomListener->onChatRoomInsertInDatabaseRequested(q->getSharedFromThis());
			}

		} else if (q->getState() == ConferenceInterface::State::TerminationPending) {
			/* This is the case where we have re-created the session in order to quit the chatroom.
			 * In this case, defer the sending of the bye so that it is sent after the ACK.
			 * Indeed, the ACK is sent immediately after being notified of the Connected state.*/
			q->getCore()->doLater(
			    [q]() { static_pointer_cast<RemoteConference>(q->getConference())->focus->getSession()->terminate(); });
		}
	} else if (newState == CallSession::State::End) {
		const auto errorInfo = session->getErrorInfo();

		if (errorInfo != nullptr && linphone_error_info_get_protocol_code(errorInfo) > 299) {
			lWarning() << "Chat room [" << q->getConferenceId()
			           << "] received a BYE with reason: " << linphone_error_info_get_protocol_code(errorInfo)
			           << ", not leaving it.";
		} else {
			const auto &remoteConferenceAddress = session->getRemoteAddress();
			bool found = false;
			for (auto it = previousConferenceIds.begin(); it != previousConferenceIds.end(); it++) {
				ConferenceId confId = static_cast<ConferenceId>(*it);
				if (*confId.getPeerAddress() == *remoteConferenceAddress) {
					lInfo() << "Found previous chat room conference ID [" << confId
					        << "] for chat room with current ID [" << q->getConferenceId() << "]";
					removeConferenceIdFromPreviousList(confId);
					found = true;
					break;
				}
			}

			if (found) {
				/* This is the case where we are accepting a BYE for an already exhumed chat room, don't change it's
				 * state */
				lInfo() << "Chat room [" << remoteConferenceAddress << "] from before the exhume has been terminated";
			} else {
				q->setState(ConferenceInterface::State::TerminationPending);
			}
		}
	} else if (newState == CallSession::State::Released) {
		if (q->getState() == ConferenceInterface::State::TerminationPending) {
			if (session->getReason() == LinphoneReasonNone || session->getReason() == LinphoneReasonDeclined) {
				// Everything is fine, the chat room has been left on the server side.
				// Or received 603 Declined, the chat room has been left on the server side but
				// remains local.
				q->onConferenceTerminated(q->getConferenceAddress());
			} else {
				// Go to state TerminationFailed and then back to Created since it has not been terminated
				q->setState(ConferenceInterface::State::TerminationFailed);
				q->setState(ConferenceInterface::State::Created);
			}
		}
	} else if (newState == CallSession::State::Error) {
		if (q->getState() == ConferenceInterface::State::CreationPending) {
			q->setState(ConferenceInterface::State::CreationFailed);
			// If there are chat message pending chat room creation, set state to NotDelivered and remove them from
			// queue.
			for (const auto &message : pendingCreationMessages) {
				message->getPrivate()->setState(ChatMessage::State::NotDelivered);
			}
			pendingCreationMessages.clear();
		} else if (q->getState() == ConferenceInterface::State::TerminationPending) {
			if (session->getReason() == LinphoneReasonNotFound) {
				// Somehow the chat room is no longer known on the server, so terminate it
				q->onConferenceTerminated(q->getConferenceAddress());
			} else {
				// Go to state TerminationFailed and then back to Created since it has not been terminated
				q->setState(ConferenceInterface::State::TerminationFailed);
				q->setState(ConferenceInterface::State::Created);
			}
		}
	}
	if (getCChatRoom()) {
		linphone_chat_room_notify_session_state_changed(getCChatRoom(), static_cast<LinphoneCallState>(newState),
		                                                message.c_str());
	}
}

void ClientGroupChatRoomPrivate::addPendingMessage(const std::shared_ptr<ChatMessage> &chatMessage) {
	pendingCreationMessages.push_back(chatMessage);
}

void ClientGroupChatRoomPrivate::onChatRoomCreated(const std::shared_ptr<Address> &remoteContact) {
	L_Q();

	q->onConferenceCreated(remoteContact);
	if (remoteContact->hasParam("isfocus")) {
		if (q->getCore()->getPrivate()->remoteListEventHandler->findHandler(q->getConferenceId())) {
			q->getCore()->getPrivate()->remoteListEventHandler->subscribe();
		} else {
			bgTask.start(q->getCore(), 32); // It will be stopped when receiving the first notify
			static_pointer_cast<RemoteConference>(q->getConference())->eventHandler->subscribe(q->getConferenceId());
		}
	}
}

// -----------------------------------------------------------------------------

void ClientGroupChatRoomPrivate::acceptSession(const shared_ptr<CallSession> &session) {
	if (session->getState() == CallSession::State::UpdatedByRemote) session->acceptUpdate();
	else session->accept();
}

// =============================================================================

ClientGroupChatRoom::ClientGroupChatRoom(const shared_ptr<Core> &core,
                                         const std::shared_ptr<Address> &focus,
                                         const ConferenceId &conferenceId,
                                         const string &subject,
                                         const Content &content,
                                         CapabilitiesMask capabilities,
                                         const std::shared_ptr<ChatRoomParams> &params)
    : ChatRoom(*new ClientGroupChatRoomPrivate(capabilities | ChatRoom::Capabilities::Conference),
               core,
               params,
               make_shared<RemoteConference>(
                   core, conferenceId.getLocalAddress(), nullptr, ConferenceParams::create(core->getCCore()))) {
	L_D();

	static_pointer_cast<RemoteConference>(getConference())->eventHandler =
	    std::make_shared<RemoteConferenceEventHandler>(getConference().get(), this);
	addListener(std::shared_ptr<ConferenceListenerInterface>(static_cast<ConferenceListenerInterface *>(this),
	                                                         [](BCTBX_UNUSED(ConferenceListenerInterface * p)) {}));

	getConference()->setSubject(subject);
	for (const auto &addr : Utils::parseResourceLists(content))
		getConference()->participants.push_back(Participant::create(getConference().get(), addr));

	if (params->getEphemeralMode() == AbstractChatRoom::EphemeralMode::AdminManaged) {
		d->capabilities |= ClientGroupChatRoom::Capabilities::Ephemeral;
		d->enableEphemeral(params->getEphemeralLifetime() > 0);
	}

	setConferenceId(conferenceId);

	// if preserve_backward_compatibility, force creation of secure room in all cases
	if (params->isEncrypted() || linphone_config_get_bool(linphone_core_get_config(getCore()->getCCore()), "lime",
	                                                      "preserve_backward_compatibility", FALSE))
		d->capabilities |= ClientGroupChatRoom::Capabilities::Encrypted;
	static_pointer_cast<RemoteConference>(getConference())->focus = Participant::create(getConference().get(), focus);
	static_pointer_cast<RemoteConference>(getConference())->focus->addDevice(focus);

	static_pointer_cast<RemoteConference>(getConference())->confParams->enableChat(true);
}

ClientGroupChatRoom::ClientGroupChatRoom(const shared_ptr<Core> &core,
                                         const string &factoryUri,
                                         const std::shared_ptr<const Address> &me,
                                         const string &subject,
                                         CapabilitiesMask capabilities,
                                         const std::shared_ptr<ChatRoomParams> &params)
    : ClientGroupChatRoom(
          core, Address::create(factoryUri), ConferenceId(nullptr, me), subject, Content(), capabilities, params) {
}

ClientGroupChatRoom::ClientGroupChatRoom(const shared_ptr<Core> &core,
                                         const ConferenceId &conferenceId,
                                         shared_ptr<Participant> &me,
                                         AbstractChatRoom::CapabilitiesMask capabilities,
                                         const std::shared_ptr<ChatRoomParams> &params,
                                         const string &subject,
                                         list<shared_ptr<Participant>> &&newParticipants,
                                         unsigned int lastNotifyId,
                                         bool hasBeenLeft)
    : ChatRoom(
          *new ClientGroupChatRoomPrivate(capabilities | ClientGroupChatRoom::Capabilities::Conference),
          core,
          params,
          make_shared<RemoteConference>(core, me->getAddress(), nullptr, ConferenceParams::create(core->getCCore()))) {
	L_D();

	if (params->getEphemeralMode() == AbstractChatRoom::EphemeralMode::AdminManaged) {
		d->capabilities |= ClientGroupChatRoom::Capabilities::Ephemeral;
		d->enableEphemeral(params->getEphemeralLifetime() > 0);
	}

	static_pointer_cast<RemoteConference>(getConference())->eventHandler =
	    std::make_shared<RemoteConferenceEventHandler>(getConference().get(), this);
	addListener(std::shared_ptr<ConferenceListenerInterface>(static_cast<ConferenceListenerInterface *>(this),
	                                                         [](BCTBX_UNUSED(ConferenceListenerInterface * p)) {}));
	const std::shared_ptr<Address> &peerAddress = conferenceId.getPeerAddress();
	static_pointer_cast<RemoteConference>(getConference())->focus =
	    Participant::create(getConference().get(), peerAddress);
	static_pointer_cast<RemoteConference>(getConference())->focus->addDevice(peerAddress);
	static_pointer_cast<RemoteConference>(getConference())->focus->setFocus(true);
	getConference()->participants = std::move(newParticipants);

	setConferenceId(conferenceId);
	static_pointer_cast<RemoteConference>(getConference())->confParams->setConferenceAddress(peerAddress);
	static_pointer_cast<RemoteConference>(getConference())->confParams->setSubject(subject);
	static_pointer_cast<RemoteConference>(getConference())->confParams->enableChat(true);

	getMe()->setAdmin(me->isAdmin());
	for (const auto &device : me->getDevices())
		getMe()->addDevice(device->getAddress(), device->getName());

	bool_t forceFullState = linphone_config_get_bool(linphone_core_get_config(getCore()->getCCore()), "misc",
	                                                 "conference_event_package_force_full_state", FALSE);
	getConference()->setLastNotify(forceFullState ? 0 : lastNotifyId);
	if (linphone_core_get_global_state(getCore()->getCCore()) == LinphoneGlobalStartup) {
		lDebug() << "Last notify set to [" << getConference()->getLastNotify() << "] for conference [" << this << "]";
	} else {
		lInfo() << "Last notify set to [" << getConference()->getLastNotify() << "] for conference [" << this << "]";
	}

	if (!hasBeenLeft) {
		getCore()->getPrivate()->remoteListEventHandler->addHandler(
		    static_pointer_cast<RemoteConference>(getConference())->eventHandler.get());
		d->listHandlerUsed = getCore()->getPrivate()->remoteListEventHandler->findHandler(getConferenceId());
		if (!d->listHandlerUsed) {
			static_pointer_cast<RemoteConference>(getConference())->eventHandler->subscribe(getConferenceId());
		}
	}
}

ClientGroupChatRoom::~ClientGroupChatRoom() {
	L_D();
	try {
		if (getCore()->getPrivate()->remoteListEventHandler && d->listHandlerUsed) {
			getCore()->getPrivate()->remoteListEventHandler->removeHandler(
			    static_pointer_cast<RemoteConference>(getConference())->eventHandler.get());
		}
	} catch (const bad_weak_ptr &) {
		// Unable to unregister listener here. Core is destroyed and the listener doesn't exist.
	}
	d->setCallSessionListener(nullptr);
}

void ClientGroupChatRoom::setConferenceId(const ConferenceId &conferenceId) {
	getConference()->setConferenceId(conferenceId);

	shared_ptr<Participant> &focus = static_pointer_cast<RemoteConference>(getConference())->focus;
	// Try to update the to field of the call log if the focus is defined.
	if (focus) {
		shared_ptr<CallSession> session = static_pointer_cast<RemoteConference>(getConference())->focus->getSession();
		if (session) {
			shared_ptr<CallLog> sessionLog = session->getLog();
			if (conferenceId.getPeerAddress()->isValid()) {
				// Use the peer address of the conference ID because it has also the conf-id param hence the To field
				// can be used to search in the map of chat rooms
				sessionLog->setToAddress(conferenceId.getPeerAddress());
			} else {
				// If the conference ID peer address is not valid, use the address of the focus
				shared_ptr<Participant> &focus = static_pointer_cast<RemoteConference>(getConference())->focus;
				sessionLog->setToAddress(focus->getAddress());
			}
		}
	}
}

shared_ptr<Core> ClientGroupChatRoom::getCore() const {
	return ChatRoom::getCore();
}

void ClientGroupChatRoom::allowCpim(BCTBX_UNUSED(bool value)) {
}

void ClientGroupChatRoom::allowMultipart(BCTBX_UNUSED(bool value)) {
}

bool ClientGroupChatRoom::canHandleCpim() const {
	return true;
}

bool ClientGroupChatRoom::canHandleMultipart() const {
	return true;
}

ClientGroupChatRoom::CapabilitiesMask ClientGroupChatRoom::getCapabilities() const {
	L_D();
	shared_ptr<Call> call = getCall();
	if (call && call->getCurrentParams()->realtimeTextEnabled()) {
		CapabilitiesMask capabilities = CapabilitiesMask(d->capabilities);
		capabilities |= Capabilities::RealTimeText;
		return capabilities;
	}
	return d->capabilities;
}

ChatRoom::SecurityLevel ClientGroupChatRoom::getSecurityLevel() const {
	return getSecurityLevelExcept(nullptr);
}

ChatRoom::SecurityLevel
ClientGroupChatRoom::getSecurityLevelExcept(const std::shared_ptr<ParticipantDevice> &ignoredDevice) const {
	L_D();
	auto encryptionEngine = getCore()->getEncryptionEngine();
	if (!encryptionEngine) {
		lWarning() << "Asking participant security level but there is no encryption engine enabled";
		return AbstractChatRoom::SecurityLevel::ClearText;
	}

	if (!(d->capabilities & ClientGroupChatRoom::Capabilities::Encrypted)) {
		lDebug() << "Chatroom SecurityLevel = ClearText";
		return AbstractChatRoom::SecurityLevel::ClearText;
	}

	// Until participant list & self devices list is populated, don't assume chat room is safe but encrypted
	if (getParticipants().size() == 0 && getMe()->getDevices().size() == 0) {
		lDebug() << "Chatroom SecurityLevel = Encrypted";
		return AbstractChatRoom::SecurityLevel::Encrypted;
	}

	// populate a list of all devices in the chatroom
	// first step, all participants, including me
	auto participants = getParticipants();
	participants.push_back(getMe());

	std::list<std::string> allDevices{};
	for (const auto &participant : participants) {
		for (const auto &device : participant->getDevices()) {
			allDevices.push_back(device->getAddress()->asStringUriOnly());
		}
	}
	if (ignoredDevice != nullptr) {
		allDevices.remove(ignoredDevice->getAddress()->asStringUriOnly());
	}
	allDevices.remove(getLocalAddress()->asStringUriOnly()); // remove local device from the list

	if (allDevices.empty()) {
		return AbstractChatRoom::SecurityLevel::Safe;
	}
	auto level = encryptionEngine->getSecurityLevel(allDevices);
	lDebug() << "Chatroom SecurityLevel = " << level;
	return level;
}

bool ClientGroupChatRoom::hasBeenLeft() const {
	return (getState() != State::Created);
}

bool ClientGroupChatRoom::isReadOnly() const {
	L_D();
	if (d->capabilities & ClientGroupChatRoom::Capabilities::OneToOne) {
		return getState() != State::Created && getState() != State::Terminated;
	}
	return getState() != State::Created;
}

const std::shared_ptr<Address> &ClientGroupChatRoom::getConferenceAddress() const {
	return getConference()->getConferenceAddress();
}

void ClientGroupChatRoom::deleteFromDb() {
	L_D();
	if (!hasBeenLeft()) {
		d->deletionOnTerminationEnabled = true;
		leave();
		return;
	}
	d->chatRoomListener->onChatRoomDeleteRequested(d->proxyChatRoom ? d->proxyChatRoom->getSharedFromThis()
	                                                                : getSharedFromThis());
}

list<shared_ptr<EventLog>> ClientGroupChatRoom::getHistory(int nLast) const {
	L_D();
	return getCore()->getPrivate()->mainDb->getHistory(
	    getConferenceId(), nLast,
	    (d->capabilities & Capabilities::OneToOne)
	        ? MainDb::Filter::ConferenceChatMessageSecurityFilter
	        : MainDb::FilterMask(
	              {MainDb::Filter::ConferenceChatMessageFilter, MainDb::Filter::ConferenceInfoNoDeviceFilter}));
}

list<shared_ptr<EventLog>> ClientGroupChatRoom::getHistoryRange(int begin, int end) const {
	L_D();
	return getCore()->getPrivate()->mainDb->getHistoryRange(
	    getConferenceId(), begin, end,
	    (d->capabilities & Capabilities::OneToOne)
	        ? MainDb::Filter::ConferenceChatMessageSecurityFilter
	        : MainDb::FilterMask(
	              {MainDb::Filter::ConferenceChatMessageFilter, MainDb::Filter::ConferenceInfoNoDeviceFilter}));
}

int ClientGroupChatRoom::getHistorySize() const {
	L_D();
	return getCore()->getPrivate()->mainDb->getHistorySize(
	    getConferenceId(), (d->capabilities & Capabilities::OneToOne)
	                           ? MainDb::Filter::ConferenceChatMessageSecurityFilter
	                           : MainDb::FilterMask({MainDb::Filter::ConferenceChatMessageFilter,
	                                                 MainDb::Filter::ConferenceInfoNoDeviceFilter}));
}

bool ClientGroupChatRoom::addParticipant(const std::shared_ptr<Address> &participantAddress) {
	list<std::shared_ptr<Address>> addressesList({participantAddress});
	return addParticipants(addressesList);
}

bool ClientGroupChatRoom::addParticipants(const list<std::shared_ptr<Address>> &addresses) {
	L_D();
	if ((getState() != ConferenceInterface::State::Instantiated) &&
	    (getState() != ConferenceInterface::State::Created)) {
		lError() << "Cannot add participants to the ClientGroupChatRoom in a state other than Instantiated or Created";
		return false;
	}

	list<std::shared_ptr<Address>> addressesList = d->cleanAddressesList(addresses);
	if (addressesList.empty()) {
		lError() << "No participants given.";
		return false;
	}
	if (getCapabilities() & ClientGroupChatRoom::Capabilities::OneToOne &&
	    (addressesList.size() > 1 || getParticipantCount() != 0)) {
		lError() << "Cannot add more than one participant in a one-to-one chatroom";
		return false;
	}

	if (getState() == ConferenceInterface::State::Instantiated) {
		auto session = d->createSession();
		sendInvite(session, addressesList);
		setState(ConferenceInterface::State::CreationPending);
	} else {
		SalReferOp *referOp = new SalReferOp(getCore()->getCCore()->sal.get());
		LinphoneAddress *lAddr = getConferenceAddress()->toC();
		linphone_configure_op(getCore()->getCCore(), referOp, lAddr, nullptr, true);
		for (const auto &addr : addresses) {
			Address referToAddr(*addr);
			referToAddr.setParam("text");
			referOp->sendRefer(referToAddr.getImpl());
		}
		referOp->unref();
	}
	return true;
}

void ClientGroupChatRoom::sendInvite(std::shared_ptr<CallSession> &session,
                                     const list<std::shared_ptr<Address>> &addressList) {
	Content content;
	content.setBodyFromUtf8(Utils::getResourceLists(addressList));
	content.setContentType(ContentType::ResourceLists);
	content.setContentDisposition(ContentDisposition::RecipientList);
	if (linphone_core_content_encoding_supported(getCore()->getCCore(), "deflate")) {
		content.setContentEncoding("deflate");
	}
	session->startInvite(nullptr, getUtf8Subject(), &content);
}

bool ClientGroupChatRoom::removeParticipant(const shared_ptr<Participant> &participant) {
	LinphoneCore *cCore = getCore()->getCCore();

	// TODO handle one-to-one case ?
	SalReferOp *referOp = new SalReferOp(cCore->sal.get());
	LinphoneAddress *lAddr = getConferenceAddress()->toC();
	linphone_configure_op(cCore, referOp, lAddr, nullptr, false);
	Address referToAddr(*participant->getAddress());
	referToAddr.setParam("text");
	referToAddr.setUriParam("method", "BYE");
	referOp->sendRefer(referToAddr.getImpl());
	referOp->unref();

	return true;
}

shared_ptr<Participant> ClientGroupChatRoom::findParticipant(const std::shared_ptr<Address> &addr) const {
	return getConference()->findParticipant(addr);
}

shared_ptr<Participant> ClientGroupChatRoom::getMe() const {
	return getConference()->getMe();
}

int ClientGroupChatRoom::getParticipantCount() const {
	return getConference()->getParticipantCount();
}

const list<shared_ptr<ParticipantDevice>> ClientGroupChatRoom::getParticipantDevices() const {
	return getConference()->getParticipantDevices();
}

const list<shared_ptr<Participant>> &ClientGroupChatRoom::getParticipants() const {
	return getConference()->getParticipants();
}

void ClientGroupChatRoom::setParticipantAdminStatus(const shared_ptr<Participant> &participant, bool isAdmin) {
	if (isAdmin == participant->isAdmin()) return;

	if (!getMe()->isAdmin()) {
		lError() << "Cannot change the participant admin status because I am not admin";
		return;
	}

	LinphoneCore *cCore = getCore()->getCCore();

	SalReferOp *referOp = new SalReferOp(cCore->sal.get());
	LinphoneAddress *lAddr = getConferenceAddress()->toC();
	linphone_configure_op(cCore, referOp, lAddr, nullptr, false);
	Address referToAddr(*participant->getAddress());
	referToAddr.setParam("text");
	referToAddr.setParam("admin", Utils::toString(isAdmin));
	referOp->sendRefer(referToAddr.getImpl());
	referOp->unref();
}

const string &ClientGroupChatRoom::getSubject() const {
	return getConference()->getSubject();
}

void ClientGroupChatRoom::setSubject(const string &subject) {
	L_D();

	if (getState() != ConferenceInterface::State::Created) {
		lError() << "Cannot change the ClientGroupChatRoom subject in a state other than Created";
		return;
	}

	if (!getMe()->isAdmin()) {
		lError() << "Cannot change the ClientGroupChatRoom subject because I am not admin";
		return;
	}

	shared_ptr<CallSession> session = static_pointer_cast<RemoteConference>(getConference())->focus->getSession();
	if (session) session->update(nullptr, CallSession::UpdateMethod::Default, Utils::localeToUtf8(subject));
	else {
		session = d->createSession();
		session->startInvite(nullptr, Utils::localeToUtf8(subject), nullptr);
	}
}

void ClientGroupChatRoom::join() {
	L_D();

	shared_ptr<CallSession> session = static_pointer_cast<RemoteConference>(getConference())->focus->getSession();
	if (!session && ((getState() == ConferenceInterface::State::Instantiated) ||
	                 (getState() == ConferenceInterface::State::Terminated))) {
		session = d->createSession();
	}
	if (session) {
		if (getState() != ConferenceInterface::State::TerminationPending) session->startInvite(nullptr, "", nullptr);
		if (getState() != ConferenceInterface::State::Created) setState(ConferenceInterface::State::CreationPending);
	}
}

void ClientGroupChatRoom::leave() {
	L_D();

	static_pointer_cast<RemoteConference>(getConference())->eventHandler->unsubscribe();
	shared_ptr<CallSession> session = static_pointer_cast<RemoteConference>(getConference())->focus->getSession();
	if (session) session->terminate();
	else {
		session = d->createSession();
		session->startInvite(nullptr, "", nullptr);
	}

	setState(ConferenceInterface::State::TerminationPending);
}

void ClientGroupChatRoom::exhume() {
	L_D();
	const auto &conference = getConference();

	if (getState() != ChatRoom::State::Terminated) {
		lError() << "Cannot exhume a non terminated chat room";
		return;
	}
	if (!(d->capabilities & ClientGroupChatRoom::Capabilities::OneToOne)) {
		lError() << "Cannot exhume a non one-to-one chat room";
		return;
	}
	if (getParticipants().size() == 0) {
		lError() << "Cannot exhume a chat room without any participant";
		return;
	}

	const std::shared_ptr<Address> &remoteParticipant = getParticipants().front()->getAddress();
	lInfo() << "Exhuming chat room [" << conference->getConferenceId() << "] with participant [" << remoteParticipant
	        << "]";
	d->localExhumePending = true;

	Content content;
	list<std::shared_ptr<Address>> addresses;
	addresses.push_front(remoteParticipant);
	content.setBodyFromUtf8(Utils::getResourceLists(addresses));
	content.setContentType(ContentType::ResourceLists);
	content.setContentDisposition(ContentDisposition::RecipientList);
	if (linphone_core_content_encoding_supported(getCore()->getCCore(), "deflate")) {
		content.setContentEncoding("deflate");
	}

	string conferenceFactoryUri = Core::getConferenceFactoryUri(getCore(), getConferenceId().getLocalAddress());
	std::shared_ptr<Address> conferenceFactoryAddress = Address::create(conferenceFactoryUri);
	auto session = d->createSessionTo(conferenceFactoryAddress);
	session->startInvite(nullptr, getUtf8Subject(), &content);
	setState(ConferenceInterface::State::CreationPending);
}

void ClientGroupChatRoomPrivate::sendChatMessage(const shared_ptr<ChatMessage> &chatMessage) {
	L_Q();
	const auto &conference = q->getConference();

	if (q->getState() == ConferenceInterface::State::Terminated &&
	    (capabilities & ClientGroupChatRoom::Capabilities::OneToOne)) {
		lInfo() << "Trying to send message into a terminated 1-1 chat room [" << conference->getConferenceId()
		        << "], exhuming it first";
		q->exhume();

		auto it = std::find(pendingExhumeMessages.begin(), pendingExhumeMessages.end(), chatMessage);
		if (it == pendingExhumeMessages.end()) pendingExhumeMessages.push_back(chatMessage);
	} else if (q->getState() == ConferenceInterface::State::Instantiated ||
	           q->getState() == ConferenceInterface::State::CreationPending) {
		lInfo() << "Trying to send a message [" << chatMessage
		        << "] in a chat room that's not created yet, queuing the message and it will be sent later";
		auto it = std::find(pendingCreationMessages.begin(), pendingCreationMessages.end(), chatMessage);
		if (it == pendingCreationMessages.end()) pendingCreationMessages.push_back(chatMessage);
	} else if (q->getState() == ConferenceInterface::State::Created) {
		ChatRoomPrivate::sendChatMessage(chatMessage);
	} else {
		lError() << "Can't send a chat message in a chat room that is in state " << q->getState();
	}
}

void ClientGroupChatRoomPrivate::onExhumedConference(const ConferenceId &oldConfId, const ConferenceId &newConfId) {
	L_Q();

	const auto &conference = q->getConference();
	const std::shared_ptr<Address> &addr = newConfId.getPeerAddress();

	conference->setConferenceAddress(addr);
	static_pointer_cast<RemoteConference>(conference)->confParams->setConferenceAddress(addr);
	static_pointer_cast<RemoteConference>(conference)->focus->setAddress(addr);
	static_pointer_cast<RemoteConference>(conference)->focus->clearDevices();
	static_pointer_cast<RemoteConference>(conference)->focus->addDevice(addr);

	conference->setConferenceId(newConfId);
	q->getCore()->getPrivate()->updateChatRoomConferenceId(q->getSharedFromThis(), oldConfId);
}

// Will be called on A when A is sending a message into a chat room with B previously terminated by B
void ClientGroupChatRoomPrivate::onLocallyExhumedConference(const std::shared_ptr<Address> &remoteContact) {
	L_Q();

	const auto &conference = q->getConference();
	ConferenceId oldConfId = conference->getConferenceId();
	ConferenceId newConfId = ConferenceId(remoteContact, oldConfId.getLocalAddress());

	lInfo() << "Conference [" << oldConfId << "] has been exhumed into [" << newConfId << "]";

	onExhumedConference(oldConfId, newConfId);

	q->setState(ConferenceInterface::State::Created);

	static_pointer_cast<RemoteConference>(q->getConference())
	    ->eventHandler->unsubscribe(); // Required for next subscribe to be sent
	q->getConference()->setLastNotify(0);
	q->getCore()->getPrivate()->remoteListEventHandler->addHandler(
	    static_pointer_cast<RemoteConference>(q->getConference())->eventHandler.get());
	static_pointer_cast<RemoteConference>(q->getConference())->eventHandler->subscribe(q->getConferenceId());

	lInfo() << "Found " << pendingExhumeMessages.size() << " messages waiting for exhume";
	for (auto &chatMessage : pendingExhumeMessages) {
		chatMessage->getPrivate()->setChatRoom(q->getSharedFromThis());
		ChatRoomPrivate::sendChatMessage(chatMessage);
	}

	pendingExhumeMessages.clear();
	localExhumePending = false;
}

// Will be called on A when B exhumes a chat room previously terminated by B
void ClientGroupChatRoomPrivate::onRemotelyExhumedConference(SalCallOp *op) {
	L_Q();

	const auto &conference = q->getConference();
	ConferenceId oldConfId = conference->getConferenceId();
	ConferenceId newConfId = ConferenceId(Address::create(op->getRemoteContact()), oldConfId.getLocalAddress());

	if (q->getState() != ChatRoom::State::Terminated) {
		lWarning() << "Conference is being exhumed but wasn't terminated first!";

		if (oldConfId == newConfId) {
			lWarning() << "Conference is being exhumed but with the same conference id!";
		} else {
			addConferenceIdToPreviousList(oldConfId);
		}
	}

	lInfo() << "Conference [" << oldConfId << "] is being exhumed into [" << newConfId << "]";

	onExhumedConference(oldConfId, newConfId);

	if (q->getState() != ChatRoom::State::Terminated) {
		// Wait for chat room to have been updated before inserting the previous ID in db
		if (oldConfId != newConfId) {
			q->getCore()->getPrivate()->mainDb->insertNewPreviousConferenceId(newConfId, oldConfId);
		}
	}

	confirmJoining(op);

	q->setState(ConferenceInterface::State::Created);

	static_pointer_cast<RemoteConference>(q->getConference())
	    ->eventHandler->unsubscribe(); // Required for next subscribe to be sent
	q->getConference()->setLastNotify(0);
	q->getCore()->getPrivate()->remoteListEventHandler->addHandler(
	    static_pointer_cast<RemoteConference>(q->getConference())->eventHandler.get());
	static_pointer_cast<RemoteConference>(q->getConference())->eventHandler->subscribe(q->getConferenceId());
}

void ClientGroupChatRoomPrivate::removeConferenceIdFromPreviousList(const ConferenceId &confId) {
	L_Q();

	previousConferenceIds.remove(confId);
	q->getCore()->getPrivate()->mainDb->removePreviousConferenceId(confId);
}

// -----------------------------------------------------------------------------
void ClientGroupChatRoom::onConferenceCreated(const std::shared_ptr<Address> &addr) {
	L_D();

	setConferenceId(ConferenceId(addr, getConferenceId().getLocalAddress()));
	lInfo() << "Conference [" << conference->getConferenceId() << "] has been created";

	static_pointer_cast<RemoteConference>(getConference())->confParams->setConferenceAddress(addr);
	static_pointer_cast<RemoteConference>(getConference())->focus->setAddress(addr);
	static_pointer_cast<RemoteConference>(getConference())->focus->clearDevices();
	static_pointer_cast<RemoteConference>(getConference())->focus->addDevice(addr);

	d->chatRoomListener->onChatRoomInsertRequested(getSharedFromThis());
	setState(ConferenceInterface::State::Created);
}

void ClientGroupChatRoom::onConferenceKeywordsChanged(const vector<string> &keywords) {
	L_D();
	if (find(keywords.cbegin(), keywords.cend(), "one-to-one") != keywords.cend())
		d->capabilities |= ClientGroupChatRoom::Capabilities::OneToOne;
	if (find(keywords.cbegin(), keywords.cend(), "ephemeral") != keywords.cend())
		d->capabilities |= ClientGroupChatRoom::Capabilities::Ephemeral;
}

void ClientGroupChatRoom::onConferenceTerminated(BCTBX_UNUSED(const std::shared_ptr<Address> &addr)) {
	L_D();

	static_pointer_cast<RemoteConference>(getConference())->eventHandler->unsubscribe();
	getConference()->resetLastNotify();
	// remove event handler from list event handler if used
	if (d->listHandlerUsed && getCore()->getPrivate()->remoteListEventHandler)
		getCore()->getPrivate()->remoteListEventHandler->removeHandler(
		    static_pointer_cast<RemoteConference>(getConference())->eventHandler.get());

	setState(ConferenceInterface::State::Terminated);

	auto event = make_shared<ConferenceEvent>(EventLog::Type::ConferenceTerminated, time(nullptr), getConferenceId());
	d->addEvent(event);

	LinphoneChatRoom *cr = d->getCChatRoom();
	_linphone_chat_room_notify_conference_left(cr, L_GET_C_BACK_PTR(event));

	if (d->deletionOnTerminationEnabled) {
		d->deletionOnTerminationEnabled = false;
		d->chatRoomListener->onChatRoomDeleteRequested(d->proxyChatRoom ? d->proxyChatRoom->getSharedFromThis()
		                                                                : getSharedFromThis());
	}
}

void ClientGroupChatRoom::onFirstNotifyReceived(BCTBX_UNUSED(const std::shared_ptr<Address> &addr)) {
	L_D();

	if (getState() != ConferenceInterface::State::Created) {
		lWarning() << "First notify received in ClientGroupChatRoom that is not in the Created state [" << getState()
		           << "], ignoring it!";
		return;
	}

	auto event = make_shared<ConferenceEvent>(EventLog::Type::ConferenceCreated, time(nullptr), getConferenceId());

	bool_t forceFullState = linphone_config_get_bool(linphone_core_get_config(getCore()->getCCore()), "misc",
	                                                 "conference_event_package_force_full_state", FALSE);
	if (!forceFullState) // to avoid this event to be repeated for each full state
		d->addEvent(event);

	LinphoneChatRoom *cr = d->getCChatRoom();
	_linphone_chat_room_notify_conference_joined(cr, L_GET_C_BACK_PTR(event));

	d->bgTask.stop();
}

void ClientGroupChatRoom::onFullStateReceived() {
	L_D();

	auto migration = d->needToMigrate();
	if (migration.first) {
		BasicToClientGroupChatRoom::migrate(getSharedFromThis(), migration.second);
	} else {
		d->chatRoomListener->onChatRoomInsertInDatabaseRequested(getSharedFromThis());
	}

	// Now that chat room has been inserted in database, we can send any pending message
	for (const auto &message : d->pendingCreationMessages) {
		lInfo() << "Found message [" << message << "] waiting for chat room to be created, sending it now";
		// First we need to update from & to address of the message,
		// as it was created at a time where the remote address of the chat room may not have been known
		message->getPrivate()->setChatRoom(getSharedFromThis());
		d->sendChatMessage(message);
	}
	d->pendingCreationMessages.clear();
}

void ClientGroupChatRoom::onParticipantAdded(const shared_ptr<ConferenceParticipantEvent> &event,
                                             BCTBX_UNUSED(const std::shared_ptr<Participant> &participant)) {
	L_D();

	if (event->getFullState()) return;

	d->addEvent(event);

	LinphoneChatRoom *cr = d->getCChatRoom();
	_linphone_chat_room_notify_participant_added(cr, L_GET_C_BACK_PTR(event));
}

void ClientGroupChatRoom::onParticipantRemoved(const shared_ptr<ConferenceParticipantEvent> &event,
                                               BCTBX_UNUSED(const std::shared_ptr<Participant> &participant)) {
	L_D();

	d->addEvent(event);

	LinphoneChatRoom *cr = d->getCChatRoom();
	_linphone_chat_room_notify_participant_removed(cr, L_GET_C_BACK_PTR(event));
}

void ClientGroupChatRoom::onParticipantSetAdmin(const shared_ptr<ConferenceParticipantEvent> &event,
                                                BCTBX_UNUSED(const std::shared_ptr<Participant> &participant)) {
	L_D();

	if (event->getFullState()) return;

	d->addEvent(event);

	LinphoneChatRoom *cr = d->getCChatRoom();
	_linphone_chat_room_notify_participant_admin_status_changed(cr, L_GET_C_BACK_PTR(event));
}

void ClientGroupChatRoom::onParticipantDeviceStateChanged(
    const std::shared_ptr<ConferenceParticipantDeviceEvent> &event, const std::shared_ptr<ParticipantDevice> &device) {
	L_D();

	if (event->getFullState()) return;

	d->addEvent(event);

	LinphoneChatRoom *cr = d->getCChatRoom();
	_linphone_chat_room_notify_participant_device_state_changed(cr, L_GET_C_BACK_PTR(event),
	                                                            (LinphoneParticipantDeviceState)device->getState());
}

void ClientGroupChatRoom::onParticipantDeviceMediaAvailabilityChanged(
    const std::shared_ptr<ConferenceParticipantDeviceEvent> &event,
    BCTBX_UNUSED(const std::shared_ptr<ParticipantDevice> &device)) {
	L_D();

	if (event->getFullState()) return;

	d->addEvent(event);

	LinphoneChatRoom *cr = d->getCChatRoom();
	_linphone_chat_room_notify_participant_device_media_availability_changed(cr, L_GET_C_BACK_PTR(event));
}

void ClientGroupChatRoom::onSecurityEvent(const shared_ptr<ConferenceSecurityEvent> &event) {
	L_D();
	shared_ptr<ConferenceSecurityEvent> finalEvent = nullptr;
	shared_ptr<ConferenceSecurityEvent> cleanEvent = nullptr;

	// Remove faulty device if its address is invalid
	std::shared_ptr<Address> faultyDevice = event->getFaultyDeviceAddress();
	if (!faultyDevice || !faultyDevice->isValid()) {
		cleanEvent = make_shared<ConferenceSecurityEvent>(event->getCreationTime(), event->getConferenceId(),
		                                                  event->getSecurityEventType());
	}
	finalEvent = cleanEvent ? cleanEvent : event;

	d->addEvent(event);

	LinphoneChatRoom *cr = d->getCChatRoom();
	_linphone_chat_room_notify_security_event(cr, L_GET_C_BACK_PTR(event));
}

void ClientGroupChatRoom::onSubjectChanged(const shared_ptr<ConferenceSubjectEvent> &event) {
	L_D();

	if (event->getFullState()) return;

	d->addEvent(event);

	LinphoneChatRoom *cr = d->getCChatRoom();
	_linphone_chat_room_notify_subject_changed(cr, L_GET_C_BACK_PTR(event));
	linphone_core_notify_chat_room_subject_changed(linphone_chat_room_get_core(cr), cr);
}

void ClientGroupChatRoom::onParticipantDeviceAdded(const shared_ptr<ConferenceParticipantDeviceEvent> &event,
                                                   const std::shared_ptr<ParticipantDevice> &device) {
	L_D();

	const std::shared_ptr<Address> &addr = event->getParticipantAddress();
	shared_ptr<Participant> participant;
	if (getConference()->isMe(addr)) participant = getMe();
	else participant = findParticipant(addr);

	// Check if new device degrades the chatroom security level and return corresponding security event
	shared_ptr<ConferenceSecurityEvent> securityEvent = nullptr;

	auto encryptionEngine = getCore()->getEncryptionEngine();
	if (encryptionEngine) {
		ChatRoom::SecurityLevel currentSecurityLevel = getSecurityLevelExcept(device);
		securityEvent = encryptionEngine->onDeviceAdded(event->getDeviceAddress(), participant, getSharedFromThis(),
		                                                currentSecurityLevel);
	}

	if (event->getFullState()) return;

	d->addEvent(event);

	if (securityEvent) onSecurityEvent(securityEvent);

	LinphoneChatRoom *cr = d->getCChatRoom();
	_linphone_chat_room_notify_participant_device_added(cr, L_GET_C_BACK_PTR(event));
}

void ClientGroupChatRoom::onParticipantDeviceRemoved(const shared_ptr<ConferenceParticipantDeviceEvent> &event,
                                                     BCTBX_UNUSED(const std::shared_ptr<ParticipantDevice> &device)) {
	L_D();

	d->addEvent(event);

	LinphoneChatRoom *cr = d->getCChatRoom();
	_linphone_chat_room_notify_participant_device_removed(cr, L_GET_C_BACK_PTR(event));
}

void ClientGroupChatRoom::onParticipantsCleared() {
	// clear from db as well
	const list<shared_ptr<Participant>> participants = getConference()->getParticipants();
	for (const auto &participant : participants) {
		getCore()->getPrivate()->mainDb->deleteChatRoomParticipant(getSharedFromThis(), participant->getAddress());
		for (const auto &device : participant->getDevices()) {
			getCore()->getPrivate()->mainDb->deleteChatRoomParticipantDevice(getSharedFromThis(), device);
		}
	}
	getCore()->getPrivate()->mainDb->deleteChatRoomParticipant(getSharedFromThis(), getMe()->getAddress());
	for (const auto &device : getMe()->getDevices()) {
		getCore()->getPrivate()->mainDb->deleteChatRoomParticipantDevice(getSharedFromThis(), device);
	}
	getConference()->clearParticipants();
}

void ClientGroupChatRoom::onEphemeralModeChanged(const shared_ptr<ConferenceEphemeralMessageEvent> &event) {
	L_D();

	d->addEvent(event);

	LinphoneChatRoom *cr = d->getCChatRoom();
	_linphone_chat_room_notify_ephemeral_event(cr, L_GET_C_BACK_PTR(event));
}

void ClientGroupChatRoom::onEphemeralMessageEnabled(const shared_ptr<ConferenceEphemeralMessageEvent> &event) {
	L_D();

	d->addEvent(event);

	LinphoneChatRoom *cr = d->getCChatRoom();
	_linphone_chat_room_notify_ephemeral_event(cr, L_GET_C_BACK_PTR(event));
}

void ClientGroupChatRoom::onEphemeralLifetimeChanged(const shared_ptr<ConferenceEphemeralMessageEvent> &event) {
	L_D();

	d->addEvent(event);

	LinphoneChatRoom *cr = d->getCChatRoom();
	_linphone_chat_room_notify_ephemeral_event(cr, L_GET_C_BACK_PTR(event));
}

void ClientGroupChatRoom::setEphemeralMode(AbstractChatRoom::EphemeralMode mode, bool updateDb) {
	L_D();

	if (!getMe()->isAdmin()) {
		lError() << "Only admins can choose who can manage ephemeral messages on chatroom " << getConferenceAddress();
		return;
	}

	if (getEphemeralMode() == mode) {
		lWarning() << "Ephemeral messages are already managed by "
		           << ((mode == AbstractChatRoom::EphemeralMode::AdminManaged) ? "the admins" : "each participant");
		return;
	}

	if (!(d->capabilities & ClientGroupChatRoom::Capabilities::Ephemeral)) {
		lWarning() << "Ephemeral message mode cannot be changed if chatroom has capabiltiy Ephemeral disabled";
		return;
	}

	d->params->setEphemeralMode(mode);

	const auto &lifetime = d->params->getEphemeralLifetime();

	if (getState() == ConferenceInterface::State::Created) {
		shared_ptr<CallSession> session = static_pointer_cast<RemoteConference>(getConference())->focus->getSession();
		auto csp = session->getParams()->clone();
		csp->removeCustomHeader("Ephemeral-Life-Time");
		if (mode == AbstractChatRoom::EphemeralMode::AdminManaged) {
			csp->addCustomHeader("Ephemeral-Life-Time", to_string(lifetime));
		}
		lInfo() << "Changing ephemeral mode to " << Utils::toString(mode);
		session->update(csp, CallSession::UpdateMethod::Default, getUtf8Subject());
		delete csp;
	} else {
		lError() << "Cannot change the ClientGroupChatRoom ephemeral lifetime in a state other than Created";
	}

	if (updateDb) {
		shared_ptr<ConferenceEphemeralMessageEvent> event;
		if (mode == AbstractChatRoom::EphemeralMode::AdminManaged) {
			event = make_shared<ConferenceEphemeralMessageEvent>(
			    EventLog::Type::ConferenceEphemeralMessageManagedByAdmin, time(nullptr), getConferenceId(), lifetime);
		} else {
			event = make_shared<ConferenceEphemeralMessageEvent>(
			    EventLog::Type::ConferenceEphemeralMessageManagedByParticipants, time(nullptr), getConferenceId(),
			    lifetime);
		}
		d->addEvent(event);

		LinphoneChatRoom *cr = d->getCChatRoom();
		_linphone_chat_room_notify_ephemeral_event(cr, L_GET_C_BACK_PTR(event));
	}
}

AbstractChatRoom::EphemeralMode ClientGroupChatRoom::getEphemeralMode() const {
	L_D();
	return d->params->getEphemeralMode();
}

void ClientGroupChatRoom::enableEphemeral(bool ephem, bool updateDb) {
	L_D();

	if (d->isEphemeral == ephem) {
		if (linphone_core_get_global_state(getCore()->getCCore()) != LinphoneGlobalStartup) {
			lWarning() << "Ephemeral messages of chat room " << getConferenceId() << " are already "
			           << (ephem ? "enabled" : "disabled");
		}
		return;
	}

	LinphoneImNotifPolicy *policy = linphone_core_get_im_notif_policy(getCore()->getCCore());
	if (!linphone_im_notif_policy_get_send_imdn_displayed(policy) && ephem) {
		lWarning() << "Ephemeral messages may not work correctly because IMDN messages are disabled";
	}

	d->enableEphemeral(ephem);
	const string active = ephem ? "enabled" : "disabled";
	lDebug() << "Ephemeral message is " << active << " in chat room [" << getConferenceId() << "]";

	auto lifetime = d->params->getEphemeralLifetime();
	if (d->params->getEphemeralMode() == AbstractChatRoom::EphemeralMode::AdminManaged) {
		if (!getMe()->isAdmin()) {
			lError() << "Only admins can enable or disable ephemeral messages on chatroom " << getConferenceAddress();
			return;
		}

		if (getState() == ConferenceInterface::State::Created) {
			if (ephem && (lifetime == 0)) {
				lifetime = linphone_core_get_default_ephemeral_lifetime(getCore()->getCCore());
				d->params->setEphemeralLifetime(lifetime);
				if (updateDb) {
					lInfo() << "Reset ephemeral lifetime of chat room " << getConferenceId()
					        << " to the default value of " << lifetime
					        << " because ephemeral messages were enabled with a time equal to 0.";
					getCore()->getPrivate()->mainDb->updateChatRoomEphemeralLifetime(getConferenceId(), lifetime);

					shared_ptr<ConferenceEphemeralMessageEvent> event = make_shared<ConferenceEphemeralMessageEvent>(
					    EventLog::Type::ConferenceEphemeralMessageLifetimeChanged, time(nullptr), getConferenceId(),
					    lifetime);
					d->addEvent(event);
				}
			}
			sendEphemeralUpdate();
		} else {
			lError() << "Cannot change the ClientGroupChatRoom ephemeral lifetime in a state other than Created";
		}
	}

	if (updateDb) {
		getCore()->getPrivate()->mainDb->updateChatRoomEphemeralEnabled(getConferenceId(), ephem);
		shared_ptr<ConferenceEphemeralMessageEvent> event;
		if (ephem)
			event = make_shared<ConferenceEphemeralMessageEvent>(EventLog::Type::ConferenceEphemeralMessageEnabled,
			                                                     time(nullptr), getConferenceId(), lifetime);
		else
			event = make_shared<ConferenceEphemeralMessageEvent>(EventLog::Type::ConferenceEphemeralMessageDisabled,
			                                                     time(nullptr), getConferenceId(), lifetime);
		d->addEvent(event);

		LinphoneChatRoom *cr = d->getCChatRoom();
		_linphone_chat_room_notify_ephemeral_event(cr, L_GET_C_BACK_PTR(event));
	}
}

bool ClientGroupChatRoom::ephemeralEnabled() const {
	L_D();
	return d->isEphemeral;
}

void ClientGroupChatRoom::setEphemeralLifetime(long lifetime, bool updateDb) {
	L_D();

	if (lifetime == d->params->getEphemeralLifetime()) {
		if (updateDb)
			lWarning() << "Ephemeral lifetime of chat room " << getConferenceId()
			           << " will not be changed! Trying to set the same ephemaral lifetime as before : " << lifetime;
		return;
	}

	if (getState() == ConferenceInterface::State::Instantiated) {
		// Do not print this log when creating chat room from DB
		if (updateDb)
			lInfo() << "Set new ephemeral lifetime " << lifetime << " of chat room " << getConferenceId()
			        << ", used to be " << d->params->getEphemeralLifetime() << ".";
		d->params->setEphemeralLifetime(lifetime);
		return;
	}

	if (d->params->getEphemeralMode() == AbstractChatRoom::EphemeralMode::AdminManaged) {
		if (!getMe()->isAdmin()) {
			lError() << "Cannot change the ClientGroupChatRoom ephemeral lifetime because I am not admin";
			return;
		}

		if (getState() == ConferenceInterface::State::Created) {
			if (updateDb)
				lInfo() << "Set new ephemeral lifetime " << lifetime << " of chat room " << getConferenceId()
				        << ", used to be " << d->params->getEphemeralLifetime() << ".";
			d->params->setEphemeralLifetime(lifetime);
			const bool enable = (lifetime != 0);
			// If only changing the value of the message lifetime
			if (ephemeralEnabled() == enable) {
				sendEphemeralUpdate();
			} else {
				enableEphemeral(enable, updateDb);
			}
		} else {
			lError() << "Cannot change the ephemeral lifetime of chat room " << getConferenceId()
			         << " in a state other than Created";
		}
	} else {
		if (updateDb)
			lInfo() << "Set new ephemeral lifetime " << lifetime << ", used to be " << d->params->getEphemeralLifetime()
			        << ".";
		d->params->setEphemeralLifetime(lifetime);
	}

	if (updateDb) {
		getCore()->getPrivate()->mainDb->updateChatRoomEphemeralLifetime(getConferenceId(), lifetime);

		if (d->isEphemeral) { // Do not create event if ephemeral feature is disabled
			shared_ptr<ConferenceEphemeralMessageEvent> event = make_shared<ConferenceEphemeralMessageEvent>(
			    EventLog::Type::ConferenceEphemeralMessageLifetimeChanged, time(nullptr), getConferenceId(), lifetime);
			d->addEvent(event);

			LinphoneChatRoom *cr = d->getCChatRoom();
			_linphone_chat_room_notify_ephemeral_event(cr, L_GET_C_BACK_PTR(event));
		}
	}
}

long ClientGroupChatRoom::getEphemeralLifetime() const {
	L_D();
	return d->params->getEphemeralLifetime();
}

void ClientGroupChatRoom::sendEphemeralUpdate() {
	L_D();
	shared_ptr<CallSession> session = static_pointer_cast<RemoteConference>(getConference())->focus->getSession();
	if (session) {
		auto csp = session->getParams()->clone();
		csp->removeCustomHeader("Ephemeral-Life-Time");
		csp->addCustomHeader("Ephemeral-Life-Time", (ephemeralEnabled() ? to_string(getEphemeralLifetime()) : "0"));
		session->update(csp, CallSession::UpdateMethod::Default, getUtf8Subject());
		delete csp;
	} else {
		session = d->createSession();

		const std::shared_ptr<Address> &remoteParticipant = getParticipants().front()->getAddress();
		lInfo() << "Re-INVITing " << remoteParticipant << " because ephemeral settings of chat room ["
		        << conference->getConferenceId() << "] have changed";

		session->startInvite(nullptr, getUtf8Subject(), nullptr);
	}
}

bool ClientGroupChatRoom::ephemeralSupportedByAllParticipants() const {
	// TODO
	return false;
}

LINPHONE_END_NAMESPACE
