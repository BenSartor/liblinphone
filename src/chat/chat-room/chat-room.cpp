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

#include "c-wrapper/c-wrapper.h"
#include "call/call.h"
#include "chat/chat-message/chat-message-p.h"
#include "chat/chat-message/imdn-message.h"
#include "chat/chat-message/is-composing-message.h"
#include "chat/chat-message/notification-message-p.h"
#include "chat/chat-room/chat-room-p.h"
#include "content/content-manager.h"
#include "core/core-p.h"
#include "linphone/utils/algorithm.h"
#include "linphone/utils/utils.h"
#include "logger/logger.h"

// =============================================================================

using namespace std;

LINPHONE_BEGIN_NAMESPACE

#define NEW_LINE 0x2028
#define CRLF 0x0D0A
#define LF 0x0A

// -----------------------------------------------------------------------------

void ChatRoomPrivate::sendChatMessage(const shared_ptr<ChatMessage> &chatMessage) {
	L_Q();

	shared_ptr<Call> call = q->getCall();
	if (call && call->getCurrentParams()->realtimeTextEnabled()) {
		chatMessage->putCharacter(NEW_LINE);
	} else {
		ChatMessagePrivate *dChatMessage = chatMessage->getPrivate();
		dChatMessage->setTime(ms_time(0));
		if (!q->canHandleCpim()) {
			// if not using cpim, ImdnMessageId = SIP Message call id, so should be computed each time, specially in
			// case of resend.
			dChatMessage->setImdnMessageId("");
		}
		dChatMessage->send();
	}
}

void ChatRoomPrivate::onChatMessageSent(const shared_ptr<ChatMessage> &chatMessage) {
	L_Q();

	LinphoneChatRoom *cr = getCChatRoom();
	unique_ptr<MainDb> &mainDb = q->getCore()->getPrivate()->mainDb;
	shared_ptr<EventLog> eventLog = mainDb->getEvent(mainDb, chatMessage->getStorageId());

	_linphone_chat_room_notify_chat_message_sent(cr, L_GET_C_BACK_PTR(eventLog));
	linphone_core_notify_message_sent(q->getCore()->getCCore(), cr, L_GET_C_BACK_PTR(chatMessage));

	if (isComposing) isComposing = false;
	isComposingHandler->stopIdleTimer();
	isComposingHandler->stopRefreshTimer();
}

void ChatRoomPrivate::sendIsComposingNotification() {
	L_Q();
	LinphoneImNotifPolicy *policy = linphone_core_get_im_notif_policy(q->getCore()->getCCore());
	if (!linphone_im_notif_policy_get_send_is_composing(policy)) return;

	auto isComposingMsg = createIsComposingMessage();
	isComposingMsg->getPrivate()->send();
}

// -----------------------------------------------------------------------------

void ChatRoomPrivate::addEvent(const shared_ptr<EventLog> &eventLog) {
	L_Q();

	q->getCore()->getPrivate()->mainDb->addEvent(eventLog);

	EventLog::Type type = eventLog->getType();
	if (type == EventLog::Type::ConferenceParticipantDeviceAdded ||
	    type == EventLog::Type::ConferenceParticipantDeviceRemoved ||
	    type == EventLog::Type::ConferenceParticipantDeviceStatusChanged) {
		// Do not update last event time on the chat room for those events
		// because they are invisible and will cause the chat room to move
		// up in the list and the user won't know why
	} else {
		setLastUpdateTime(eventLog->getCreationTime());
		q->getCore()->getPrivate()->mainDb->updateChatRoomLastUpdatedTime(q->getConferenceId(), lastUpdateTime);

		if (type == EventLog::Type::ConferenceChatMessage) {
			setIsEmpty(false);
		}
	}
}

void ChatRoomPrivate::addTransientEvent(const shared_ptr<EventLog> &eventLog) {
	auto it = find(transientEvents, eventLog);
	if (it == transientEvents.end()) transientEvents.push_back(eventLog);
}

void ChatRoomPrivate::removeTransientEvent(const shared_ptr<EventLog> &eventLog) {
	auto it = find(transientEvents, eventLog);
	if (it != transientEvents.end()) transientEvents.erase(it);
}

void ChatRoomPrivate::addTransientChatMessage(const shared_ptr<ChatMessage> &message) {
	auto it = find(transientMessages, message);
	if (it == transientMessages.end()) transientMessages.push_back(message);
}

void ChatRoomPrivate::removeTransientChatMessage(const shared_ptr<ChatMessage> &message) {
	auto it = find(transientMessages, message);
	if (it != transientMessages.end()) transientMessages.erase(it);
}

std::list<std::shared_ptr<ChatMessage>> ChatRoomPrivate::getTransientChatMessages() {
	return transientMessages;
}

void ChatRoomPrivate::setIsEmpty(const bool empty) {
	isEmpty = empty;
}

void ChatRoomPrivate::realtimeTextReceived(uint32_t character, const shared_ptr<Call> &call) {
	L_Q();

	shared_ptr<Core> core = q->getCore();
	LinphoneCore *cCore = core->getCCore();

	if (call && call->getCurrentParams()->realtimeTextEnabled()) {
		receivedRttCharacters.push_back(character);
		remoteIsComposing.push_back(q->getPeerAddress());
		linphone_core_notify_is_composing_received(cCore, getCChatRoom());

		if ((character == NEW_LINE) || (character == CRLF) || (character == LF)) {
			// End of message
			string completeText = Utils::unicodeToUtf8(lastMessageCharacters);

			shared_ptr<ChatMessage> pendingMessage = q->createChatMessage();
			pendingMessage->getPrivate()->setDirection(ChatMessage::Direction::Incoming);
			Content *content = new Content();
			content->setContentType(ContentType::PlainText);
			content->setBodyFromUtf8(completeText);
			pendingMessage->addContent(content);

			bctbx_debug("New line received, forge a message with content [%s]", content->getBodyAsString().c_str());
			pendingMessage->getPrivate()->setState(ChatMessage::State::Delivered);
			pendingMessage->getPrivate()->setTime(::ms_time(0));

			if (linphone_config_get_int(linphone_core_get_config(cCore), "misc", "store_rtt_messages", 1) == 1) {
				pendingMessage->getPrivate()->storeInDb();
			}

			onChatMessageReceived(pendingMessage);
			lastMessageCharacters.clear();
		} else {
			lastMessageCharacters.push_back(character);
			string completeText = Utils::unicodeToUtf8(lastMessageCharacters);
			bctbx_debug("Received RTT character: [%llu], pending text is [%s]", character, completeText.c_str());
		}
	}
}

// -----------------------------------------------------------------------------

shared_ptr<ChatMessage> ChatRoomPrivate::createChatMessage(ChatMessage::Direction direction) {
	L_Q();
	shared_ptr<ChatMessage> message = shared_ptr<ChatMessage>(new ChatMessage(q->getSharedFromThis(), direction));
	if (q->ephemeralEnabled() && direction == ChatMessage::Direction::Outgoing) {
		lDebug() << "Create an outgoing ephemeral message " << message << " with lifetime " << q->getEphemeralLifetime()
		         << " in chat room [" << q->getConferenceId() << "]";
		message->getPrivate()->enableEphemeralWithTime(q->getEphemeralLifetime());
	}
	return message;
}

shared_ptr<ImdnMessage> ChatRoomPrivate::createImdnMessage(const list<shared_ptr<ChatMessage>> &deliveredMessages,
                                                           const list<shared_ptr<ChatMessage>> &displayedMessages) {
	L_Q();
	return shared_ptr<ImdnMessage>(new ImdnMessage(q->getSharedFromThis(), deliveredMessages, displayedMessages));
}

shared_ptr<ImdnMessage> ChatRoomPrivate::createImdnMessage(const list<Imdn::MessageReason> &nonDeliveredMessages) {
	L_Q();
	return shared_ptr<ImdnMessage>(new ImdnMessage(q->getSharedFromThis(), nonDeliveredMessages));
}

shared_ptr<ImdnMessage> ChatRoomPrivate::createImdnMessage(const shared_ptr<ImdnMessage> &message) {
	return shared_ptr<ImdnMessage>(new ImdnMessage(message));
}

shared_ptr<IsComposingMessage> ChatRoomPrivate::createIsComposingMessage() {
	L_Q();
	return shared_ptr<IsComposingMessage>(
	    new IsComposingMessage(q->getSharedFromThis(), *isComposingHandler.get(), isComposing));
}

list<shared_ptr<ChatMessage>> ChatRoomPrivate::findChatMessages(const string &messageId) const {
	L_Q();
	return q->getCore()->getPrivate()->mainDb->findChatMessages(q->getConferenceId(), messageId);
}

list<shared_ptr<ChatMessage>> ChatRoomPrivate::findChatMessages(const list<string> &messageIds) const {
	L_Q();
	return q->getCore()->getPrivate()->mainDb->findChatMessages(q->getConferenceId(), messageIds);
}

// -----------------------------------------------------------------------------

void ChatRoomPrivate::sendDeliveryErrorNotification(const shared_ptr<ChatMessage> &chatMessage, LinphoneReason reason) {
	LinphoneImNotifPolicy *policy = linphone_core_get_im_notif_policy(chatMessage->getCore()->getCCore());
	ChatMessagePrivate *dChatMessage = chatMessage->getPrivate();
	if (linphone_im_notif_policy_get_send_imdn_delivered(policy) &&
	    chatMessage->getPrivate()->getNegativeDeliveryNotificationRequired()) {
		dChatMessage->setNegativeDeliveryNotificationRequired(false);
		imdnHandler->notifyDeliveryError(chatMessage, reason);
	}
}

void ChatRoomPrivate::sendDeliveryNotification(const shared_ptr<ChatMessage> &chatMessage) {
	LinphoneImNotifPolicy *policy = linphone_core_get_im_notif_policy(chatMessage->getCore()->getCCore());
	ChatMessagePrivate *dChatMessage = chatMessage->getPrivate();
	if (linphone_im_notif_policy_get_send_imdn_delivered(policy) &&
	    dChatMessage->getPositiveDeliveryNotificationRequired()) {
		dChatMessage->setPositiveDeliveryNotificationRequired(false);
		imdnHandler->notifyDelivery(chatMessage);
	}
}

void ChatRoomPrivate::sendDeliveryNotifications(const std::shared_ptr<ChatMessage> &chatMessage) {
	ChatMessagePrivate *dChatMessage = chatMessage->getPrivate();
	if (dChatMessage->getPositiveDeliveryNotificationRequired()) {
		dChatMessage->setPositiveDeliveryNotificationRequired(false);
		imdnHandler->notifyDelivery(chatMessage);
	}
}

void ChatRoomPrivate::sendDisplayNotification(const shared_ptr<ChatMessage> &chatMessage) {
	L_Q();
	LinphoneImNotifPolicy *policy = linphone_core_get_im_notif_policy(q->getCore()->getCCore());
	ChatMessagePrivate *dChatMessage = chatMessage->getPrivate();
	if (linphone_im_notif_policy_get_send_imdn_displayed(policy) &&
	    chatMessage->getPrivate()->getDisplayNotificationRequired()) {
		dChatMessage->setPositiveDeliveryNotificationRequired(false);
		dChatMessage->setDisplayNotificationRequired(false);
		imdnHandler->notifyDisplay(chatMessage);
	}
}

// -----------------------------------------------------------------------------

void ChatRoomPrivate::notifyChatMessageReceived(const shared_ptr<ChatMessage> &chatMessage) {
	L_Q();
	LinphoneChatRoom *cr = getCChatRoom();
	if (!chatMessage->getPrivate()->getText().empty()) {
		/* Legacy API */
		LinphoneAddress *fromAddress = chatMessage->getFromAddress()->toC();
		linphone_core_notify_text_message_received(q->getCore()->getCCore(), cr, fromAddress,
		                                           chatMessage->getPrivate()->getText().c_str());
	}
	_linphone_chat_room_notify_message_received(cr, L_GET_C_BACK_PTR(chatMessage));
	linphone_core_notify_message_received(q->getCore()->getCCore(), cr, L_GET_C_BACK_PTR(chatMessage));
}

void ChatRoomPrivate::notifyIsComposingReceived(const std::shared_ptr<Address> &remoteAddress, bool isComposing) {
	L_Q();

	auto it = find_if(remoteIsComposing.cbegin(), remoteIsComposing.cend(),
	                  [&remoteAddress](const auto &address) { return (*remoteAddress == *address); });

	if (isComposing) {
		if (it == remoteIsComposing.cend()) {
			remoteIsComposing.push_back(remoteAddress);
		}
	} else {
		if (it != remoteIsComposing.cend()) {
			remoteIsComposing.erase(it);
		}
	}

	LinphoneChatRoom *cr = getCChatRoom();
	LinphoneAddress *lAddr = remoteAddress->toC();
	_linphone_chat_room_notify_is_composing_received(cr, lAddr, !!isComposing);
	// Legacy notification
	linphone_core_notify_is_composing_received(q->getCore()->getCCore(), cr);
}

void ChatRoomPrivate::notifyStateChanged() {
	L_Q();
	LinphoneChatRoom *cr = getCChatRoom();
	// Do not output this log while Core is starting up, a lot of them may happen
	if (q->getCore()->getCCore()->state == LinphoneGlobalStartup) {
		lDebug() << "Chat room [" << q->getConferenceId() << "] state changed to: " << Utils::toString(q->getState());
	} else {
		lInfo() << "Chat room [" << q->getConferenceId() << "] state changed to: " << Utils::toString(q->getState());
	}
	LinphoneChatRoomState chatRoomCState =
	    linphone_conference_state_to_chat_room_state(static_cast<LinphoneConferenceState>(q->getState()));
	linphone_core_notify_chat_room_state_changed(q->getCore()->getCCore(), cr, chatRoomCState);
	_linphone_chat_room_notify_state_changed(cr, chatRoomCState);
}

void ChatRoomPrivate::notifyUndecryptableChatMessageReceived(const shared_ptr<ChatMessage> &chatMessage) {
	L_Q();
	LinphoneChatRoom *cr = getCChatRoom();
	_linphone_chat_room_notify_undecryptable_message_received(cr, L_GET_C_BACK_PTR(chatMessage));
	linphone_core_notify_message_received_unable_decrypt(q->getCore()->getCCore(), cr, L_GET_C_BACK_PTR(chatMessage));
}

// -----------------------------------------------------------------------------

std::shared_ptr<ChatMessage> ChatRoomPrivate::getMessageFromSal(SalOp *op, const SalMessage *message) {
	L_Q();

	shared_ptr<ChatMessage> msg;

	auto from = Address::create(op->getFrom());
	msg = createChatMessage((*from == *q->getLocalAddress()) ? ChatMessage::Direction::Outgoing
	                                                         : ChatMessage::Direction::Incoming);

	Content content;
	if (message->url && ContentType(message->content_type) == ContentType::ExternalBody) {
		lInfo() << "Received a message with an external body URL " << message->url;
		content.setContentType(ContentType::FileTransfer);
		content.setBodyFromUtf8(msg->getPrivate()->createFakeFileTransferFromUrl(message->url));
	} else {
		content.setContentType(ContentType(message->content_type));
		content.setBodyFromUtf8(message->text ? message->text : "");
	}
	msg->setInternalContent(content);

	msg->getPrivate()->setTime(message->time);
	ostringstream messageId;
	if (op->hasDialog()) {
		/* If this message has been received part of a dialog (which is unlikely to happen for IM),
		 * set an IMDN Message ID abitrary to be the SIP Call-ID followed by the CSeq number.
		 * This avoids considering incoming SIP MESSAGE received within a dialog as being duplicates. */
		messageId << op->getCallId() << "-" << op->getRemoteCSeq();
	} else {
		messageId << op->getCallId();
	}

	// Don't do it for flexisip backend chat rooms, we need to know if the real message id from CPIM was retrieved or
	// not Based on that we will send IMDNs or not In case CPIM was enabled on a Basic chat room, IMDN message ID will
	// be overwritten by real one
	if (q->getCapabilities().isSet(ChatRoom::Capabilities::Basic)) {
		msg->getPrivate()->setImdnMessageId(messageId.str());
	}
	msg->getPrivate()->setCallId(op->getCallId());

	const SalCustomHeader *ch = op->getRecvCustomHeaders();
	if (ch) msg->getPrivate()->setSalCustomHeaders(sal_custom_header_clone(ch));

	addTransientChatMessage(msg);

	return msg;
}

LinphoneReason ChatRoomPrivate::onSipMessageReceived(SalOp *op, const SalMessage *message) {
	auto msg = getMessageFromSal(op, message);
	LinphoneReason reason = msg->getPrivate()->receive();
	return reason;
}

void ChatRoomPrivate::onChatMessageReceived(const shared_ptr<ChatMessage> &chatMessage) {
	L_Q();

	shared_ptr<Core> core = q->getCore();
	LinphoneCore *cCore = core->getCCore();

	if (chatMessage->getPrivate()->getContentType() == ContentType::ImIsComposing) {
		onIsComposingReceived(chatMessage->getFromAddress(), chatMessage->getPrivate()->getText());
		if (linphone_config_get_int(linphone_core_get_config(cCore), "sip", "deliver_imdn", 0) != 1) return;
	} else if (chatMessage->getPrivate()->getContentType() == ContentType::Imdn) {
		onImdnReceived(chatMessage);
		if (linphone_config_get_int(linphone_core_get_config(cCore), "sip", "deliver_imdn", 0) != 1) return;
	}

	const std::shared_ptr<Address> &fromAddress = chatMessage->getFromAddress();
	if ((chatMessage->getPrivate()->getContentType() != ContentType::ImIsComposing) &&
	    (chatMessage->getPrivate()->getContentType() != ContentType::Imdn)) {
		isComposingHandler->stopRemoteRefreshTimer(fromAddress->toString());
		notifyIsComposingReceived(fromAddress, false);
	}

	if (core->isCurrentlyAggregatingChatMessages()) {
		lDebug() << "[Chat Room] [" << q->getConferenceId()
		         << "] Core is currently aggregating chat messages, push message to list";
		aggregatedMessages.push_back(chatMessage);
	} else {
		// No aggregation, notify right away
		lDebug() << "[Chat Room] [" << q->getConferenceId() << "] No aggregation, notify right away";

		bool chatMessagesAggregationEnabled = !!linphone_core_get_chat_messages_aggregation_enabled(cCore);
		if (chatMessagesAggregationEnabled) { // Need to notify using aggregated callback even if there is only on
			                                  // message
			// This can happen when auto download is enabled and auto download takes longer than the aggregation delay
			aggregatedMessages.push_back(chatMessage);
			notifyAggregatedChatMessages();
		} else {
			notifyMessageReceived(chatMessage);
		}
	}
}

void ChatRoomPrivate::notifyMessageReceived(const shared_ptr<ChatMessage> &chatMessage) {
	shared_ptr<ConferenceChatMessageEvent> event =
	    make_shared<ConferenceChatMessageEvent>(::time(nullptr), chatMessage);
	_linphone_chat_room_notify_chat_message_received(getCChatRoom(), L_GET_C_BACK_PTR(event));
	// Legacy.
	notifyChatMessageReceived(chatMessage);

	sendDeliveryNotification(chatMessage);
}

void ChatRoomPrivate::notifyAggregatedChatMessages() {
	L_Q();

	if (aggregatedMessages.empty()) {
		lDebug() << "[Chat Room] [" << q->getConferenceId() << "] No aggregated message to notify";
		return;
	}

	size_t aggregatedMessagesSize = aggregatedMessages.size();
	if (aggregatedMessagesSize == 1) {
		lDebug() << "[Chat Room] [" << q->getConferenceId() << "] There is 1 aggregated message to notify";
	} else {
		lDebug() << "[Chat Room] [" << q->getConferenceId() << "] There are " << aggregatedMessagesSize
		         << " aggregated messages to notify";
	}
	LinphoneChatRoom *cChatRoom = getCChatRoom();
	auto core = q->getCore()->getCCore();

	// Notify as ChatMessages
	bctbx_list_t *cMessages = L_GET_RESOLVED_C_LIST_FROM_CPP_LIST(aggregatedMessages);
	_linphone_chat_room_notify_messages_received(cChatRoom, cMessages);
	linphone_core_notify_messages_received(core, cChatRoom, cMessages);

	// Notify as Events
	std::list<std::shared_ptr<ConferenceChatMessageEvent>> eventsList;
	for (auto &chatMessage : aggregatedMessages) {
		shared_ptr<ConferenceChatMessageEvent> event =
		    make_shared<ConferenceChatMessageEvent>(::time(nullptr), chatMessage);
		eventsList.push_back(event);
	}
	bctbx_list_t *cEvents = L_GET_RESOLVED_C_LIST_FROM_CPP_LIST(eventsList);
	_linphone_chat_room_notify_chat_messages_received(cChatRoom, cEvents);

	// Notify delivery
	for (auto &chatMessage : aggregatedMessages) {
		sendDeliveryNotification(chatMessage);
	}

	bctbx_list_free_with_data(cMessages, (bctbx_list_free_func)linphone_chat_message_unref);
	bctbx_list_free_with_data(cEvents, (bctbx_list_free_func)linphone_event_log_unref);

	aggregatedMessages.clear();
}

void ChatRoomPrivate::onImdnReceived(const shared_ptr<ChatMessage> &chatMessage) {
	Imdn::parse(chatMessage);
}

void ChatRoomPrivate::onIsComposingReceived(const std::shared_ptr<Address> &remoteAddress, const string &text) {
	isComposingHandler->parse(remoteAddress, text);
}

void ChatRoomPrivate::onIsComposingRefreshNeeded() {
	sendIsComposingNotification();
}

void ChatRoomPrivate::onIsComposingStateChanged(bool isComposing) {
	this->isComposing = isComposing;
	sendIsComposingNotification();
}

void ChatRoomPrivate::onIsRemoteComposingStateChanged(const std::shared_ptr<Address> &remoteAddress, bool isComposing) {
	notifyIsComposingReceived(remoteAddress, isComposing);
}

bool ChatRoomPrivate::isSubscriptionUnderWay() const {
	return false;
}

void ChatRoomPrivate::addPendingMessage(BCTBX_UNUSED(const std::shared_ptr<ChatMessage> &chatMessage)) {
	lInfo() << __func__ << ": not implemented";
}

// -----------------------------------------------------------------------------

LinphoneChatRoom *ChatRoomPrivate::getCChatRoom() const {
	L_Q();
	if (proxyChatRoom) return L_GET_C_BACK_PTR(proxyChatRoom);
	else return L_GET_C_BACK_PTR(q);
}

// =============================================================================

ChatRoom::ChatRoom(ChatRoomPrivate &p,
                   const shared_ptr<Core> &core,
                   const std::shared_ptr<ChatRoomParams> &params,
                   const shared_ptr<Conference> &conf)
    : AbstractChatRoom(p, core) {
	L_D();

	d->params = params;
	d->imdnHandler.reset(new Imdn(this));
	d->isComposingHandler.reset(new IsComposing(core->getCCore(), d));

	this->conference = conf;
}

ChatRoom::~ChatRoom() {
	L_D();

	if (d->params) {
		d->params.reset();
	}
	d->imdnHandler.reset();
}

// -----------------------------------------------------------------------------

const std::shared_ptr<Address> &ChatRoom::getPeerAddress() const {
	return getConferenceId().getPeerAddress();
}

const std::shared_ptr<Address> &ChatRoom::getLocalAddress() const {
	return getConferenceId().getLocalAddress();
}

// -----------------------------------------------------------------------------

time_t ChatRoom::getCreationTime() const {
	L_D();
	return d->creationTime;
}

time_t ChatRoom::getLastUpdateTime() const {
	L_D();
	return d->lastUpdateTime;
}

// -----------------------------------------------------------------------------

ConferenceInterface::State ChatRoom::getState() const {
	if (conference) {
		return conference->getState();
	} else {
		return ConferenceInterface::State::None;
	}
}

ChatRoom::SecurityLevel ChatRoom::getSecurityLevel() const {
	return ChatRoom::SecurityLevel::ClearText;
}

// -----------------------------------------------------------------------------

list<shared_ptr<EventLog>> ChatRoom::getMessageHistory(int nLast) const {
	return getCore()->getPrivate()->mainDb->getHistory(getConferenceId(), nLast,
	                                                   MainDb::Filter::ConferenceChatMessageFilter);
}

list<shared_ptr<EventLog>> ChatRoom::getMessageHistoryRange(int begin, int end) const {
	return getCore()->getPrivate()->mainDb->getHistoryRange(getConferenceId(), begin, end,
	                                                        MainDb::Filter::ConferenceChatMessageFilter);
}

list<shared_ptr<ChatMessage>> ChatRoom::getUnreadChatMessages() const {
	return getCore()->getPrivate()->mainDb->getUnreadChatMessages(getConferenceId());
}

int ChatRoom::getMessageHistorySize() const {
	return getCore()->getPrivate()->mainDb->getHistorySize(getConferenceId(),
	                                                       MainDb::Filter::ConferenceChatMessageFilter);
}

list<shared_ptr<EventLog>> ChatRoom::getHistory(int nLast) const {
	return getCore()->getPrivate()->mainDb->getHistory(
	    getConferenceId(), nLast,
	    MainDb::FilterMask(
	        {MainDb::Filter::ConferenceChatMessageFilter, MainDb::Filter::ConferenceInfoNoDeviceFilter}));
}

list<shared_ptr<EventLog>> ChatRoom::getHistoryRange(int begin, int end) const {
	return getCore()->getPrivate()->mainDb->getHistoryRange(
	    getConferenceId(), begin, end,
	    MainDb::FilterMask(
	        {MainDb::Filter::ConferenceChatMessageFilter, MainDb::Filter::ConferenceInfoNoDeviceFilter}));
}

int ChatRoom::getHistorySize() const {
	return getCore()->getPrivate()->mainDb->getHistorySize(getConferenceId());
}

void ChatRoom::deleteFromDb() {
	// Keep a ref, otherwise the object might be destroyed before we can set the Deleted state
	shared_ptr<AbstractChatRoom> ref = this->getSharedFromThis();
	Core::deleteChatRoom(ref);
	setState(ConferenceInterface::State::Deleted);
}

void ChatRoom::deleteHistory() {
	L_D();
	getCore()->getPrivate()->mainDb->cleanHistory(getConferenceId());
	d->setIsEmpty(true);
}

void ChatRoom::deleteMessageFromHistory(const shared_ptr<ChatMessage> &message) {
	L_D();

	shared_ptr<LinphonePrivate::EventLog> event =
	    LinphonePrivate::MainDb::getEvent(getCore()->getPrivate()->mainDb, message->getStorageId());
	if (event) {
		LinphonePrivate::EventLog::deleteFromDatabase(event);
		d->setIsEmpty(getCore()->getPrivate()->mainDb->isChatRoomEmpty(getConferenceId()));
	}
}

shared_ptr<ChatMessage> ChatRoom::getLastChatMessageInHistory() const {
	return getCore()->getPrivate()->mainDb->getLastChatMessage(getConferenceId());
}

bool ChatRoom::isEmpty() const {
	L_D();

	return d->isEmpty;
}

int ChatRoom::getChatMessageCount() const {
	return getCore()->getPrivate()->mainDb->getChatMessageCount(getConferenceId());
}

int ChatRoom::getUnreadChatMessageCount() const {
	return getCore()->getPrivate()->mainDb->getUnreadChatMessageCount(getConferenceId());
}

// -----------------------------------------------------------------------------

void ChatRoom::compose() {
	L_D();
	if (!d->isComposing) {
		d->isComposing = true;
		d->sendIsComposingNotification();
		d->isComposingHandler->startRefreshTimer();
	}
	d->isComposingHandler->startIdleTimer();
}

bool ChatRoom::isRemoteComposing() const {
	L_D();
	return !d->remoteIsComposing.empty();
}

list<std::shared_ptr<Address>> ChatRoom::getComposingAddresses() const {
	L_D();
	return d->remoteIsComposing;
}

// -----------------------------------------------------------------------------

shared_ptr<ChatMessage> ChatRoom::createChatMessage() {
	L_D();
	return d->createChatMessage(ChatMessage::Direction::Outgoing);
}

// Deprecated
shared_ptr<ChatMessage> ChatRoom::createChatMessage(const string &text) {
	shared_ptr<ChatMessage> chatMessage = createChatMessage();
	Content *content = new Content();
	content->setContentType(ContentType::PlainText);
	content->setBodyFromLocale(text);
	chatMessage->addContent(content);
	return chatMessage;
}

shared_ptr<ChatMessage> ChatRoom::createChatMessageFromUtf8(const string &text) {
	shared_ptr<ChatMessage> chatMessage = createChatMessage();
	Content *content = new Content();
	content->setContentType(ContentType::PlainText);
	content->setBodyFromUtf8(text);
	chatMessage->addContent(content);
	return chatMessage;
}

shared_ptr<ChatMessage> ChatRoom::createFileTransferMessage(FileContent *content) {
	shared_ptr<ChatMessage> chatMessage = createChatMessage();
	chatMessage->addContent(content);
	return chatMessage;
}

shared_ptr<ChatMessage> ChatRoom::createForwardMessage(const shared_ptr<ChatMessage> &msg) {
	shared_ptr<ChatMessage> chatMessage = createChatMessage();
	for (const Content *c : msg->getContents()) {
		chatMessage->addContent(c->clone());
	}

	// set forward info
	std::string fInfo;
	bool hidden = linphone_core_is_sender_name_hidden_in_forward_message(getCore()->getCCore());
	if (hidden) {
		fInfo = "Anonymous";
	} else {
		fInfo = msg->getForwardInfo().empty() ? msg->getFromAddress()->asStringUriOnly() : msg->getForwardInfo();
	}

	chatMessage->getPrivate()->setForwardInfo(fInfo);

	return chatMessage;
}

shared_ptr<ChatMessage> ChatRoom::createReplyMessage(const shared_ptr<ChatMessage> &msg) {
	shared_ptr<ChatMessage> chatMessage = createChatMessage();
	chatMessage->getPrivate()->setReplyToMessageIdAndSenderAddress(msg->getImdnMessageId(), msg->getFromAddress());
	return chatMessage;
}

// -----------------------------------------------------------------------------

list<shared_ptr<ChatMessage>> ChatRoom::findChatMessages(const list<string> &messageIds) const {
	L_D();
	list<shared_ptr<ChatMessage>> chatMessages = d->findChatMessages(messageIds);
	return chatMessages;
}

shared_ptr<ChatMessage> ChatRoom::findChatMessage(const string &messageId) const {
	L_D();
	list<shared_ptr<ChatMessage>> chatMessages = d->findChatMessages(messageId);
	return chatMessages.empty() ? nullptr : chatMessages.front();
}

shared_ptr<ChatMessage> ChatRoom::findChatMessage(const string &messageId, ChatMessage::Direction direction) const {
	L_D();
	for (auto &chatMessage : d->findChatMessages(messageId))
		if (chatMessage->getDirection() == direction) return chatMessage;
	return nullptr;
}

void ChatRoom::markAsRead() {
	L_D();

	// Mark any message currently waiting aggregation as read
	for (auto &chatMessage : d->aggregatedMessages) {
		chatMessage->getPrivate()->markAsRead();
		// Do not set the message state has displayed if it contains a file transfer (to prevent imdn sending)
		if (!chatMessage->getPrivate()->hasFileTransferContent()) {
			chatMessage->getPrivate()->setState(ChatMessage::State::Displayed);
		}
	}

	CorePrivate *dCore = getCore()->getPrivate();
	for (auto &chatMessage : dCore->mainDb->getUnreadChatMessages(getConferenceId())) {
		chatMessage->getPrivate()->markAsRead();
		// Do not set the message state has displayed if it contains a file transfer (to prevent imdn sending)
		if (!chatMessage->getPrivate()->hasFileTransferContent()) {
			chatMessage->getPrivate()->setState(ChatMessage::State::Displayed);
		}
	}

	dCore->mainDb->markChatMessagesAsRead(getConferenceId());
	linphone_core_notify_chat_room_read(getCore()->getCCore(), d->getCChatRoom());
}

const std::shared_ptr<ChatRoomParams> &ChatRoom::getCurrentParams() const {
	L_D();

	return d->params;
}

void ChatRoom::enableEphemeral(BCTBX_UNUSED(bool ephem), BCTBX_UNUSED(bool updateDb)) {
	lDebug() << "Ephemeral message is only supported in conference based chat room!";
}

bool ChatRoom::ephemeralEnabled() const {
	return false;
}

void ChatRoom::setEphemeralLifetime(BCTBX_UNUSED(long lifetime), BCTBX_UNUSED(bool updateDb)) {
	lError() << "Ephemeral message is only supported in conference based chat room!";
}

long ChatRoom::getEphemeralLifetime() const {
	return 0;
}

void ChatRoom::setEphemeralMode(BCTBX_UNUSED(AbstractChatRoom::EphemeralMode mode), BCTBX_UNUSED(bool updateDb)) {
	lError() << "Ephemeral message is only supported in conference based chat room!";
}

AbstractChatRoom::EphemeralMode ChatRoom::getEphemeralMode() const {
	lError() << "Ephemeral message is only supported in conference based chat room!";
	return AbstractChatRoom::EphemeralMode::DeviceManaged;
}

bool ChatRoom::ephemeralSupportedByAllParticipants() const {
	return false;
}

void ChatRoom::setState(ConferenceInterface::State newState) {
	L_D();

	if (conference) {
		if (getState() != newState) {
			conference->setState(newState);
			d->notifyStateChanged();
		}
	}
}

void ChatRoom::addListener(std::shared_ptr<ConferenceListenerInterface> listener) {
	if (conference) {
		conference->addListener(listener);
	}
}

bool ChatRoom::removeParticipants(const list<shared_ptr<Participant>> &participants) {
	bool soFarSoGood = true;
	for (const auto &p : participants)
		soFarSoGood &= removeParticipant(p);
	return soFarSoGood;
}

bool ChatRoom::addParticipants(const std::list<std::shared_ptr<Address>> &addresses) {
	list<std::shared_ptr<Address>> sortedAddresses(addresses);
	sortedAddresses.sort([](const auto &addr1, const auto &addr2) { return *addr1 < *addr2; });
	sortedAddresses.unique([](const auto &addr1, const auto &addr2) { return addr1->weakEqual(*addr2); });

	bool soFarSoGood = true;
	for (const auto &address : sortedAddresses)
		soFarSoGood &= addParticipant(address);
	return soFarSoGood;
}

uint32_t ChatRoom::getChar() {
	L_D();
	uint32_t character = 0;

	if (d->readCharacterIndex < d->receivedRttCharacters.size()) {
		character = d->receivedRttCharacters.at(d->readCharacterIndex);
		d->readCharacterIndex += 1;
	}

	if (d->readCharacterIndex == d->receivedRttCharacters.size()) {
		d->readCharacterIndex = 0;
		d->receivedRttCharacters.clear();
	}

	return character;
}

std::shared_ptr<Call> ChatRoom::getCall() const {
	L_D();
	return getCore()->getCallByCallId(d->callId);
}
LINPHONE_END_NAMESPACE
