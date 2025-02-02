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

#include "object/object-p.h"

#include "conference_private.h"

#include "linphone/api/c-content.h"
#include "linphone/core.h"
#include "linphone/lpconfig.h"
#include "linphone/utils/utils.h"

#include "address/address.h"
#include "c-wrapper/c-wrapper.h"
#include "call/call.h"
#include "chat/chat-message/chat-message-listener.h"
#include "chat/chat-message/chat-message-p.h"
#include "chat/chat-room/chat-room-p.h"
#include "chat/chat-room/client-group-to-basic-chat-room.h"
#include "chat/ics/ics.h"
#include "chat/modifier/cpim-chat-message-modifier.h"
#include "chat/modifier/encryption-chat-message-modifier.h"
#include "chat/modifier/multipart-chat-message-modifier.h"
#include "chat/notification/imdn.h"
#include "conference/participant-imdn-state.h"
#include "conference/participant.h"
#include "content/content-disposition.h"
#include "content/content-manager.h"
#include "content/header/header-param.h"
#include "core/core-p.h"
#include "core/core.h"
#include "factory/factory.h"
#include "logger/logger.h"
#include "sip-tools/sip-headers.h"

#include "ortp/b64.h"

#include "db/main-db-key-p.h"
#include "db/main-db-p.h"
// =============================================================================

using namespace std;

using namespace B64_NAMESPACE;

LINPHONE_BEGIN_NAMESPACE

ChatMessagePrivate::ChatMessagePrivate(const std::shared_ptr<AbstractChatRoom> &cr, ChatMessage::Direction dir)
    : fileTransferChatMessageModifier(cr->getCore()->getCCore()->http_provider) {
	direction = dir;
	setChatRoom(cr);
}

ChatMessagePrivate::~ChatMessagePrivate() {
	for (Content *content : contents) {
		if (content->isFileTransfer()) {
			FileTransferContent *fileTransferContent = static_cast<FileTransferContent *>(content);
			delete fileTransferContent->getFileContent();
		}
		delete content;
	}

	if (salOp) {
		salOp->setUserPointer(nullptr);
		salOp->unref();
	}
	if (salCustomHeaders) sal_custom_header_unref(salCustomHeaders);
}

void ChatMessagePrivate::setStorageId(long long id) {
	L_Q();

	if (id < 0) {
		// Negative IDs invalidate the message therefore it will be deleted from the cache
		q->deleteChatMessageFromCache();
	}

	storageId = id;
}

void ChatMessagePrivate::resetStorageId() {
	setStorageId(-1);
}

void ChatMessagePrivate::setDirection(ChatMessage::Direction dir) {
	direction = dir;
}

void ChatMessagePrivate::setTime(time_t t) {
	time = t;
}

void ChatMessagePrivate::setIsReadOnly(bool readOnly) {
	isReadOnly = readOnly;
}

void ChatMessagePrivate::markAsRead() {
	markedAsRead = true;
}

bool ChatMessagePrivate::isMarkedAsRead() const {
	return markedAsRead;
}

void ChatMessagePrivate::setParticipantState(const std::shared_ptr<Address> &participantAddress,
                                             ChatMessage::State newState,
                                             time_t stateChangeTime) {
	L_Q();

	if (!q->isValid()) return;

	if (q->getChatRoom()->getCapabilities().isSet(ChatRoom::Capabilities::Basic)) {
		// Basic Chat Room doesn't support participant state
		setState(newState);
		return;
	}

	unique_ptr<MainDb> &mainDb = q->getChatRoom()->getCore()->getPrivate()->mainDb;
	shared_ptr<EventLog> eventLog = mainDb->getEvent(mainDb, q->getStorageId());
	ChatMessage::State currentState = mainDb->getChatMessageParticipantState(eventLog, participantAddress);

	if (!isValidStateTransition(currentState, newState)) return;

	lInfo() << "Chat message " << q->getSharedFromThis() << ": moving participant '" << participantAddress->toString()
	        << "' state to " << Utils::toString(newState);
	mainDb->setChatMessageParticipantState(eventLog, participantAddress, newState, stateChangeTime);

	LinphoneChatMessage *msg = L_GET_C_BACK_PTR(q);
	LinphoneChatRoom *cr = L_GET_C_BACK_PTR(q->getChatRoom());
	auto me = q->getChatRoom()->getMe();
	auto participant =
	    (*participantAddress == *me->getAddress()) ? me : q->getChatRoom()->findParticipant(participantAddress);
	ParticipantImdnState imdnState(participant, newState, stateChangeTime);
	const LinphoneParticipantImdnState *c_state = _linphone_participant_imdn_state_from_cpp_obj(imdnState);

	// Legacy callbacks, deprecated !
	LinphoneChatMessageCbs *cbs = linphone_chat_message_get_callbacks(msg);
	if (cbs && linphone_chat_message_cbs_get_participant_imdn_state_changed(cbs)) {
		linphone_chat_message_cbs_get_participant_imdn_state_changed(cbs)(
		    msg, _linphone_participant_imdn_state_from_cpp_obj(imdnState));
	}

	_linphone_chat_message_notify_participant_imdn_state_changed(msg, c_state);
	_linphone_chat_room_notify_chat_message_participant_imdn_state_changed(cr, msg, c_state);

	if (linphone_config_get_bool(linphone_core_get_config(q->getChatRoom()->getCore()->getCCore()), "misc",
	                             "enable_simple_group_chat_message_state", FALSE)) {
		setState(newState);
		return;
	}

	list<ChatMessage::State> states = mainDb->getChatMessageParticipantStates(eventLog);
	size_t nbDisplayedStates = 0;
	size_t nbDeliveredToUserStates = 0;
	size_t nbNotDeliveredStates = 0;
	for (const auto &state : states) {
		switch (state) {
			case ChatMessage::State::Displayed:
				nbDisplayedStates++;
				break;
			case ChatMessage::State::DeliveredToUser:
				nbDeliveredToUserStates++;
				break;
			case ChatMessage::State::NotDelivered:
				nbNotDeliveredStates++;
				break;
			default:
				break;
		}
	}

	if (nbNotDeliveredStates > 0) {
		setState(ChatMessage::State::NotDelivered);
	} else if ((states.size() > 0) && (nbDisplayedStates == states.size())) {
		setState(ChatMessage::State::Displayed);
	} else if ((states.size() > 0) && ((nbDisplayedStates + nbDeliveredToUserStates) == states.size())) {
		setState(ChatMessage::State::DeliveredToUser);
	}

	// When we already marked an incoming message as displayed, start ephemeral countdown when all other recipients have
	// displayed it as well
	if (isEphemeral && state == ChatMessage::State::Displayed) {
		if (direction == ChatMessage::Direction::Incoming &&
		    nbDisplayedStates == states.size() - 1) { // -1 is for ourselves, our own display state isn't stored in db
			startEphemeralCountDown();
		}
	}
}

void ChatMessagePrivate::setState(ChatMessage::State newState) {
	L_Q();

	// 1. Check valid transition.
	if (!isValidStateTransition(state, newState)) return;

	const shared_ptr<ChatMessage> &sharedMessage = q->getSharedFromThis();

	// 2. Update state and notify changes.
	lInfo() << "Chat message " << sharedMessage << " of chat room "
	        << (q->getChatRoom() ? q->getChatRoom()->getConferenceId() : ConferenceId()) << " : moving from "
	        << Utils::toString(state) << " to " << Utils::toString(newState);
	ChatMessage::State oldState = state;
	state = newState;

	if (state == ChatMessage::State::NotDelivered) {
		if (salOp) {
			salOp->setUserPointer(nullptr);
			salOp->unref();
			salOp = nullptr;
		}
	}

	if (direction == ChatMessage::Direction::Outgoing) {
		// Delivered state isn't triggered by IMDN, so participants state won't be set unless we manually do so here
		if (state == ChatMessage::State::Delivered) {
			for (auto participant : q->getChatRoom()->getParticipants()) {
				setParticipantState(participant->getAddress(), state, q->getTime());
			}
		}

		if (state == ChatMessage::State::NotDelivered || state == ChatMessage::State::Delivered ||
		    state == ChatMessage::State::DeliveredToUser || state == ChatMessage::State::Displayed) {
			q->getChatRoom()->getPrivate()->removeTransientChatMessage(sharedMessage);
		}
	}

	LinphoneChatMessage *msg = L_GET_C_BACK_PTR(q);
	if (linphone_chat_message_get_message_state_changed_cb(msg))
		linphone_chat_message_get_message_state_changed_cb(msg)(
		    msg, LinphoneChatMessageState(state), linphone_chat_message_get_message_state_changed_cb_user_data(msg));

	LinphoneChatMessageCbs *cbs = linphone_chat_message_get_callbacks(msg);
	if (cbs && linphone_chat_message_cbs_get_msg_state_changed(cbs))
		linphone_chat_message_cbs_get_msg_state_changed(cbs)(msg, (LinphoneChatMessageState)state);
	_linphone_chat_message_notify_msg_state_changed(msg, (LinphoneChatMessageState)state);

	auto listenersCopy = listeners; // To allow listener to be removed while iterating
	for (auto &listener : listenersCopy) {
		listener->onChatMessageStateChanged(q->getSharedFromThis(), state);
	}
	if (state == ChatMessage::State::Displayed) {
		listeners.clear();
	}

	// 3. Specific case, change to displayed once all file transfers haven been downloaded, and only if chat message has
	// been marked as read.
	if (state == ChatMessage::State::FileTransferDone && direction == ChatMessage::Direction::Incoming) {
		if (!hasFileTransferContent() && isMarkedAsRead()) {
			setState(ChatMessage::State::Displayed);
			return;
		}
	}

	// 4. Specific case, upon reception do not attempt to store in db before asking the user if he wants to do so or not
	if (state == ChatMessage::State::Delivered && oldState == ChatMessage::State::Idle &&
	    direction == ChatMessage::Direction::Incoming && !q->isValid()) {
		// If we're here it's because message is because we're in the middle of the receive() method and
		// we won't have a valid dbKey until the chat room callback asking if message should be store will be called
		// and that's happen in the notifyReceiving() called at the of the receive() method we're in.
		// This prevents the error log: Invalid db key [%p] associated to message [%p]
		return;
	}

	// 5. Send notification
	if ((state == ChatMessage::State::Displayed) && direction == ChatMessage::Direction::Incoming) {
		// Wait until all files are downloaded before sending displayed IMDN
		static_cast<ChatRoomPrivate *>(q->getChatRoom()->getPrivate())->sendDisplayNotification(sharedMessage);
	}

	// 6. update in database for ephemeral message if necessary.
	if (isEphemeral && state == ChatMessage::State::Displayed) {
		bool allParticipantsAreInDisplayedState = false;
		if (q->getChatRoom()->getCapabilities().isSet(ChatRoom::Capabilities::OneToOne)) {
			allParticipantsAreInDisplayedState = true;
		} else {
			if (direction == ChatMessage::Direction::Incoming) {
				unique_ptr<MainDb> &mainDb = q->getChatRoom()->getCore()->getPrivate()->mainDb;
				shared_ptr<EventLog> eventLog = mainDb->getEvent(mainDb, q->getStorageId());

				list<ChatMessage::State> states = mainDb->getChatMessageParticipantStates(eventLog);
				size_t nbDisplayedStates = 0;
				for (const auto &state : states) {
					switch (state) {
						case ChatMessage::State::Displayed:
							nbDisplayedStates++;
							break;
						default:
							break;
					}
				}

				allParticipantsAreInDisplayedState =
				    nbDisplayedStates ==
				    states.size() - 1; // -1 is for ourselves, our own display state isn't stored in db
			} else {
				// For outgoing messages state is never displayed until all participants are in display state
				allParticipantsAreInDisplayedState = true;
			}
		}

		if (allParticipantsAreInDisplayedState) {
			lInfo() << "All participants are in displayed state, starting ephemeral countdown";
			startEphemeralCountDown();
		}
	}

	// 7. Update in database if necessary.
	if (state != ChatMessage::State::InProgress && state != ChatMessage::State::FileTransferError &&
	    state != ChatMessage::State::FileTransferInProgress) {
		updateInDb();
	}
}

void ChatMessagePrivate::startEphemeralCountDown() {
	L_Q();

	// set ephemeral message expired time
	ephemeralExpireTime = ::ms_time(NULL) + (long)ephemeralLifetime;
	unique_ptr<MainDb> &mainDb = q->getChatRoom()->getCore()->getPrivate()->mainDb;
	q->getChatRoom()->getCore()->getPrivate()->mainDb->updateEphemeralMessageInfos(storageId, ephemeralExpireTime);

	const shared_ptr<ChatMessage> &sharedMessage = q->getSharedFromThis();
	q->getChatRoom()->getCore()->getPrivate()->updateEphemeralMessages(sharedMessage);

	lInfo() << "Starting ephemeral countdown with life time: " << ephemeralLifetime;

	// notify start !
	shared_ptr<AbstractChatRoom> chatRoom = q->getChatRoom();
	shared_ptr<LinphonePrivate::EventLog> event = LinphonePrivate::MainDb::getEvent(mainDb, q->getStorageId());
	if (chatRoom && event) {
		_linphone_chat_room_notify_ephemeral_message_timer_started(L_GET_C_BACK_PTR(chatRoom), L_GET_C_BACK_PTR(event));
		LinphoneChatMessage *msg = L_GET_C_BACK_PTR(q);
		LinphoneChatMessageCbs *cbs = linphone_chat_message_get_callbacks(msg);
		if (cbs && linphone_chat_message_cbs_get_ephemeral_message_timer_started(cbs))
			linphone_chat_message_cbs_get_ephemeral_message_timer_started(cbs)(msg);
		_linphone_chat_message_notify_ephemeral_message_timer_started(msg);
	}
}

// -----------------------------------------------------------------------------

belle_http_request_t *ChatMessagePrivate::getHttpRequest() const {
	return fileTransferChatMessageModifier.getHttpRequest();
}

void ChatMessagePrivate::setHttpRequest(belle_http_request_t *request) {
	fileTransferChatMessageModifier.setHttpRequest(request);
}

// -----------------------------------------------------------------------------

void ChatMessagePrivate::disableDeliveryNotificationRequiredInDatabase() {
	L_Q();
	unique_ptr<MainDb> &mainDb = q->getChatRoom()->getCore()->getPrivate()->mainDb;
	if (q->isValid()) mainDb->disableDeliveryNotificationRequired(mainDb->getEvent(mainDb, q->getStorageId()));
}

void ChatMessagePrivate::disableDisplayNotificationRequiredInDatabase() {
	L_Q();
	unique_ptr<MainDb> &mainDb = q->getChatRoom()->getCore()->getPrivate()->mainDb;
	const std::shared_ptr<const EventLog> &eventLog = mainDb->getEvent(mainDb, q->getStorageId());
	if (q->isValid() && eventLog) mainDb->disableDisplayNotificationRequired(eventLog);
}

// -----------------------------------------------------------------------------

SalOp *ChatMessagePrivate::getSalOp() const {
	return salOp;
}

void ChatMessagePrivate::setSalOp(SalOp *op) {
	salOp = op;
}

SalCustomHeader *ChatMessagePrivate::getSalCustomHeaders() const {
	return salCustomHeaders;
}

void ChatMessagePrivate::setSalCustomHeaders(SalCustomHeader *headers) {
	salCustomHeaders = headers;
}

void ChatMessagePrivate::addSalCustomHeader(const string &name, const string &value) {
	salCustomHeaders = sal_custom_header_append(salCustomHeaders, name.c_str(), value.c_str());
}

void ChatMessagePrivate::removeSalCustomHeader(const string &name) {
	salCustomHeaders = sal_custom_header_remove(salCustomHeaders, name.c_str());
}

string ChatMessagePrivate::getSalCustomHeaderValue(const string &name) const {
	return L_C_TO_STRING(sal_custom_header_find(salCustomHeaders, name.c_str()));
}

// -----------------------------------------------------------------------------
// Below methods are only for C API backward compatibility...
// -----------------------------------------------------------------------------

bool ChatMessagePrivate::hasTextContent() const {
	for (const Content *c : getContents()) {
		if (c->getContentType() == ContentType::PlainText) {
			return true;
		}
	}
	return false;
}

const Content *ChatMessagePrivate::getTextContent() const {
	for (const Content *c : getContents()) {
		if (c->getContentType() == ContentType::PlainText) {
			return c;
		}
	}
	return &Utils::getEmptyConstRefObject<Content>();
}

bool ChatMessagePrivate::hasConferenceInvitationContent() const {
	for (const Content *c : getContents()) {
		if (c->getContentType().strongEqual(ContentType::Icalendar)) {
			return true;
		}
	}
	return false;
}

bool ChatMessagePrivate::hasFileTransferContent() const {
	for (const Content *c : contents) {
		if (c->isFileTransfer()) {
			return true;
		}
	}
	return false;
}

const Content *ChatMessagePrivate::getFileTransferContent() const {
	for (const Content *c : contents) {
		if (c->isFileTransfer()) {
			return c;
		}
	}
	return &Utils::getEmptyConstRefObject<Content>();
}

const string &ChatMessagePrivate::getFileTransferFilepath() const {
	return fileTransferFilePath;
}

void ChatMessagePrivate::setFileTransferFilepath(const string &path) {
	fileTransferFilePath = path;
}

void ChatMessagePrivate::setEphemeralExpireTime(time_t expireTime) {
	ephemeralExpireTime = expireTime;
}

const string &ChatMessagePrivate::getAppdata() const {
	for (const Content *c : getContents()) {
		if (!c->getAppData("legacy").empty()) {
			return c->getAppData("legacy");
		}
	}
	return Utils::getEmptyConstRefObject<string>();
}

void ChatMessagePrivate::setAppdata(const string &data) {
	bool contentFound = false;
	for (Content *c : getContents()) {
		c->setAppData("legacy", data);
		contentFound = true;
		break;
	}
	if (contentFound) {
		updateInDb();
	}
}

const string &ChatMessagePrivate::getExternalBodyUrl() const {
	if (!externalBodyUrl.empty()) {
		return externalBodyUrl;
	}
	if (hasFileTransferContent()) {
		FileTransferContent *content = (FileTransferContent *)getFileTransferContent();
		return content->getFileUrl();
	}
	return Utils::getEmptyConstRefObject<string>();
}

void ChatMessagePrivate::setExternalBodyUrl(const string &url) {
	externalBodyUrl = url;
}

const ContentType &ChatMessagePrivate::getContentType() const {
	loadContentsFromDatabase();
	if (direction == ChatMessage::Direction::Incoming) {
		if (!contents.empty()) {
			Content *content = contents.front();
			cContentType = content->getContentType();
		} else {
			cContentType = internalContent.getContentType();
		}
	} else {
		if (internalContent.getContentType().isValid()) {
			cContentType = internalContent.getContentType();
		} else {
			if (!contents.empty()) {
				Content *content = contents.front();
				cContentType = content->getContentType();
			}
		}
	}
	return cContentType;
}

void ChatMessagePrivate::setContentType(const ContentType &contentType) {
	loadContentsFromDatabase();
	if (!contents.empty() && internalContent.getContentType().isEmpty() && internalContent.isEmpty()) {
		internalContent.setBody(contents.front()->getBody());
	}
	internalContent.setContentType(contentType);

	if ((currentSendStep & ChatMessagePrivate::Step::Started) != ChatMessagePrivate::Step::Started) {
		// if not started yet the sending also alter the first content
		if (!contents.empty()) contents.front()->setContentType(contentType);
	}
}

const string &ChatMessagePrivate::getText() const {
	loadContentsFromDatabase();
	if (direction == ChatMessage::Direction::Incoming) {
		if (hasTextContent()) {
			cText = getTextContent()->getBodyAsString();
		} else if (!contents.empty()) {
			Content *content = contents.front();
			cText = content->getBodyAsString();
		} else {
			cText = internalContent.getBodyAsString();
		}
	} else {
		if (!internalContent.isEmpty()) {
			cText = internalContent.getBodyAsString();
		} else {
			if (!contents.empty()) {
				Content *content = contents.front();
				cText = content->getBodyAsString();
			}
		}
	}
	return cText;
}

void ChatMessagePrivate::setText(const string &text) {
	loadContentsFromDatabase();
	if (!contents.empty() && internalContent.getContentType().isEmpty() && internalContent.isEmpty()) {
		internalContent.setContentType(contents.front()->getContentType());
	}
	internalContent.setBodyFromLocale(text);

	if ((currentSendStep & ChatMessagePrivate::Step::Started) != ChatMessagePrivate::Step::Started) {
		// if not started yet the sending also alter the first content
		if (!contents.empty()) contents.front()->setBodyFromLocale(text);
	}
}

const string &ChatMessagePrivate::getUtf8Text() const {
	loadContentsFromDatabase();
	if (direction == ChatMessage::Direction::Incoming) {
		if (hasTextContent()) {
			cText = getTextContent()->getBodyAsUtf8String();
		} else if (!contents.empty()) {
			Content *content = contents.front();
			cText = content->getBodyAsUtf8String();
		} else {
			cText = internalContent.getBodyAsUtf8String();
		}
	} else {
		if (!internalContent.isEmpty()) {
			cText = internalContent.getBodyAsUtf8String();
		} else {
			if (!contents.empty()) {
				Content *content = contents.front();
				cText = content->getBodyAsUtf8String();
			}
		}
	}
	return cText;
}

void ChatMessagePrivate::setUtf8Text(const string &text) {
	loadContentsFromDatabase();
	if (!contents.empty() && internalContent.getContentType().isEmpty() && internalContent.isEmpty()) {
		internalContent.setContentType(contents.front()->getContentType());
	}
	internalContent.setBodyFromUtf8(text);

	if ((currentSendStep & ChatMessagePrivate::Step::Started) != ChatMessagePrivate::Step::Started) {
		// if not started yet the sending also alter the first content
		if (!contents.empty()) contents.front()->setBodyFromUtf8(text);
	}
}

const Content *ChatMessagePrivate::getFileTransferInformation() const {
	if (hasFileTransferContent()) {
		return getFileTransferContent();
	}
	for (const Content *c : getContents()) {
		if (c->isFile()) {
			FileContent *fileContent = (FileContent *)c;
			return fileContent;
		}
	}
	return nullptr;
}

bool ChatMessagePrivate::downloadFile() {
	L_Q();

	for (auto &content : getContents())
		if (content->isFileTransfer()) return q->downloadFile(static_cast<FileTransferContent *>(content));

	return false;
}

void ChatMessagePrivate::addContent(Content *content) {
	getContents().push_back(content);
}

void ChatMessagePrivate::removeContent(Content *content) {
	getContents().remove(content);
}

void ChatMessagePrivate::replaceContent(Content *contentToRemove, Content *contentToAdd) {
	list<Content *>::iterator it = contents.begin();
	while (it != contents.end()) {
		Content *content = *it;
		if (content == contentToRemove) {
			it = contents.erase(it);
			it = contents.insert(it, contentToAdd);
			break;
		} else {
			it++;
		}
	}
}

void ChatMessagePrivate::loadFileTransferUrlFromBodyToContent() {
	L_Q();
	int errorCode = 0;
	fileTransferChatMessageModifier.decode(q->getSharedFromThis(), errorCode);
}

std::string ChatMessagePrivate::createFakeFileTransferFromUrl(const std::string &url) {
	return fileTransferChatMessageModifier.createFakeFileTransferFromUrl(url);
}

void ChatMessagePrivate::setChatRoom(const shared_ptr<AbstractChatRoom> &cr) {
	chatRoom = cr;
	const ConferenceId &conferenceId(cr->getConferenceId());
	if (direction == ChatMessage::Direction::Outgoing) {
		fromAddress = conferenceId.getLocalAddress();
		toAddress = conferenceId.getPeerAddress();
	} else {
		fromAddress = conferenceId.getPeerAddress();
		toAddress = conferenceId.getLocalAddress();
	}
}

// -----------------------------------------------------------------------------

static void forceUtf8Content(Content &content) {
	// TODO: Deal with other content type in the future.
	ContentType contentType = content.getContentType();
	if (contentType != ContentType::PlainText) return;

	string charset = contentType.getParameter("charset").getValue();
	if (charset.empty()) return;

	size_t n = charset.find("charset=");
	if (n == string::npos) return;

	L_BEGIN_LOG_EXCEPTION

	size_t begin = n + sizeof("charset");
	size_t end = charset.find(";", begin);
	charset = charset.substr(begin, end - begin);

	if (Utils::stringToLower(charset) != "utf-8") {
		string utf8Body = Utils::convertAnyToUtf8(content.getBodyAsUtf8String(), charset);
		if (!utf8Body.empty()) {
			// TODO: use move operator if possible in the future!
			content.setBodyFromUtf8(utf8Body);
			contentType.addParameter("charset", "UTF-8");
			content.setContentType(contentType);
		}
	}

	L_END_LOG_EXCEPTION
}

LinphoneReason ChatMessagePrivate::receive() {
	L_Q();
	int errorCode = 0;
	LinphoneReason reason = LinphoneReasonNone;

	shared_ptr<Core> core = q->getCore();
	shared_ptr<AbstractChatRoom> chatRoom = q->getChatRoom();

	// ---------------------------------------
	// Start of message modification
	// ---------------------------------------

	if ((currentRecvStep & ChatMessagePrivate::Step::Encryption) == ChatMessagePrivate::Step::Encryption) {
		lInfo() << "Encryption step already done, skipping";
	} else {
		EncryptionChatMessageModifier ecmm;
		ChatMessageModifier::Result result = ecmm.decode(q->getSharedFromThis(), errorCode);
		if (result == ChatMessageModifier::Result::Error) {
			/* Unable to decrypt message */
			chatRoom->getPrivate()->notifyUndecryptableChatMessageReceived(q->getSharedFromThis());
			reason = linphone_error_code_to_reason(errorCode);
			static_cast<ChatRoomPrivate *>(q->getChatRoom()->getPrivate())
			    ->sendDeliveryErrorNotification(q->getSharedFromThis(), reason);
			return reason;
		}

		if (result == ChatMessageModifier::Result::Suspended) {
			currentRecvStep |= ChatMessagePrivate::Step::Encryption;
			return LinphoneReasonNone;
		}
		currentRecvStep |= ChatMessagePrivate::Step::Encryption;
	}

	// Sender Authentication
	// In secured chat rooms, the authenticatedFromAddress is already the decrypted CPIM From Address
	// In plain text basic chat rooms, the authenticatedFromAddress must be set here as the SIP From Address
	// In plain text group chat rooms the sender authentication is disabled
	if (!(q->getSharedFromThis()->getChatRoom()->getCapabilities() & ChatRoom::Capabilities::Encrypted)) {
		if (q->getSharedFromThis()->getChatRoom()->getCapabilities() & ChatRoom::Capabilities::Basic) {
			const auto &sipFromAddress = q->getSharedFromThis()->getFromAddress();
			setAuthenticatedFromAddress(*sipFromAddress);
		} else {
			lInfo() << "Sender authentication disabled for clear text group chat";
			senderAuthenticationEnabled = false;
		}
	}

	if ((currentRecvStep & ChatMessagePrivate::Step::Cpim) == ChatMessagePrivate::Step::Cpim) {
		lInfo() << "Cpim step already done, skipping";
	} else {
		if (internalContent.getContentType() == ContentType::Cpim) {
#ifdef HAVE_ADVANCED_IM
			CpimChatMessageModifier ccmm;
			ccmm.decode(q->getSharedFromThis(), errorCode);
#else
			lWarning() << "Cpim support disabled.";
#endif
		}
		currentRecvStep |= ChatMessagePrivate::Step::Cpim;
	}

	// Go through multipart otherwise Imdn::isError won't work in case of aggregated IMDN
	if ((currentRecvStep & ChatMessagePrivate::Step::Multipart) == ChatMessagePrivate::Step::Multipart) {
		lInfo() << "Multipart step already done, skipping";
	} else {
		MultipartChatMessageModifier mcmm;
		mcmm.decode(q->getSharedFromThis(), errorCode);
		currentRecvStep |= ChatMessagePrivate::Step::Multipart;
	}

	// Check if incoming message was flagged as unencrypted in an encrypted context
	if (getUnencryptedContentWarning()) {
		lWarning() << "Unencrypted content warning raised by encryption engine";
		// Allow error IMDN exclusively
		if (q->getSharedFromThis()->getInternalContent().getContentType() != ContentType::Imdn &&
		    !Imdn::isError(q->getSharedFromThis())) {
			lWarning() << "Discarding message of type "
			           << q->getSharedFromThis()->getInternalContent().getContentType();
			errorCode = 415;
			return linphone_error_code_to_reason(errorCode);
		}
	}

	if ((currentRecvStep & ChatMessagePrivate::Step::FileDownload) == ChatMessagePrivate::Step::FileDownload) {
		lInfo() << "File download step already done, skipping";
	} else {
		// This will check if internal content is FileTransfer and make the appropriate changes
		loadFileTransferUrlFromBodyToContent();
		currentRecvStep |= ChatMessagePrivate::Step::FileDownload;
	}

	if (contents.empty()) {
		// All previous modifiers only altered the internal content, let's fill the content list
		contents.push_back(new Content(internalContent));
	}

	for (auto &content : contents) {
		forceUtf8Content(*content);
	}

	// ---------------------------------------
	// End of message modification
	// ---------------------------------------

	// Remove internal content as it is not needed anymore and will confuse some old methods like getText()
	internalContent.setBodyFromUtf8("");
	internalContent.setContentType(ContentType(""));
	// Also remove current step so we go through all modifiers if message is re-received (in case of recursive call from
	// a modifier)
	currentRecvStep = ChatMessagePrivate::Step::None;

	setState(ChatMessage::State::Delivered);

	// Check if this is in fact an outgoing message (case where this is a message sent by us from an other device).
	if (chatRoom->getCapabilities() & ChatRoom::Capabilities::Conference &&
	    chatRoom->getLocalAddress()->weakEqual(*fromAddress)) {
		setDirection(ChatMessage::Direction::Outgoing);
	}

	// Check if this is a duplicate message.
	if (!imdnId.empty() && chatRoom->findChatMessage(imdnId, direction)) {
		lInfo() << "Duplicated SIP MESSAGE, ignored.";
		return core->getCCore()->chat_deny_code;
	}

	if (errorCode <= 0) {
		bool foundSupportContentType = false;
		for (Content *c : contents) {
			ContentType ct(c->getContentType());
			ct.cleanParameters();
			string contenttype = ct.getType() + "/" + ct.getSubType();
			if (linphone_core_is_content_type_supported(core->getCCore(), contenttype.c_str())) {
				foundSupportContentType = true;
				break;
			} else {
				lError() << "Unsupported content-type: " << contenttype;
			}
		}

		if (!foundSupportContentType) {
			errorCode = 415;
			lError() << "No content-type in the contents list is supported...";
		}
	}

	if (errorCode > 0) {
		reason = linphone_error_code_to_reason(errorCode);
		static_cast<ChatRoomPrivate *>(q->getChatRoom()->getPrivate())
		    ->sendDeliveryErrorNotification(q->getSharedFromThis(), reason);
		return reason;
	}

	// If message was outgoing, mark it as read
	if (direction == ChatMessage::Direction::Outgoing) {
		markAsRead();
	}

	if (getContentType() != ContentType::Imdn && getContentType() != ContentType::ImIsComposing) {
		// If we receive a message that is Outgoing it means we are in a flexisip based chat room and this message was
		// sent by us from another device, storing it
		if (direction == ChatMessage::Direction::Outgoing) {
			toBeStored = true;
		} else {
			_linphone_chat_room_notify_chat_message_should_be_stored(
			    static_pointer_cast<ChatRoom>(q->getChatRoom())->getPrivate()->getCChatRoom(),
			    L_GET_C_BACK_PTR(q->getSharedFromThis()));
		}

		if (toBeStored) {
			storeInDb();
		}
	} else {
		toBeStored = false;
		// For compatibility, when CPIM is not used
		positiveDeliveryNotificationRequired = false;
		negativeDeliveryNotificationRequired = false;
		displayNotificationRequired = false;
	}

	handleAutoDownload();

	return reason;
}

void ChatMessagePrivate::handleAutoDownload() {
	L_Q();

	if ((currentRecvStep & ChatMessagePrivate::Step::AutoFileDownload) == ChatMessagePrivate::Step::AutoFileDownload) {
		lInfo() << "Auto file download step already done, skipping";
	} else {
		int maxSize = linphone_core_get_max_size_for_auto_download_incoming_files(q->getCore()->getCCore());
		bool_t autoDownloadVoiceRecordings =
		    linphone_core_is_auto_download_voice_recordings_enabled(q->getCore()->getCCore());
		bool_t autoDownloadIcalendars = linphone_core_is_auto_download_icalendars_enabled(q->getCore()->getCCore());
		for (Content *c : contents) {
			if (c->isFileTransfer()) {
				FileTransferContent *ftc = static_cast<FileTransferContent *>(c);
				ContentType fileContentType = ftc->getFileContentType();

				if ((maxSize == 0 || (maxSize > 0 && ftc->getFileSize() <= (size_t)maxSize)) ||
				    (autoDownloadVoiceRecordings && fileContentType.strongEqual(ContentType::VoiceRecording)) ||
				    (autoDownloadIcalendars && fileContentType.strongEqual(ContentType::Icalendar))) {
					string downloadPath = q->getCore()->getDownloadPath();
					if (downloadPath.empty()) {
						lWarning() << "Download path is empty, won't be able to do auto download";
						break;
					} else {
						ostringstream sstream;
						size_t randomSize = 12;
						for (size_t i = 0; i < randomSize; i++) {
							sstream << rand() % 10;
						}
						string file = sstream.str();
						string filepath = downloadPath + file;
						lInfo() << "Automatically downloading file to " << filepath;
						ftc->setFilePath(filepath);
						setAutoFileTransferDownloadInProgress(true);
						q->downloadFile(ftc);
						return;
					}
				}
			}
		}
		currentRecvStep |= ChatMessagePrivate::Step::AutoFileDownload;
	}

	q->getChatRoom()->getPrivate()->removeTransientChatMessage(q->getSharedFromThis());
	setAutoFileTransferDownloadInProgress(false);
	setState(ChatMessage::State::Delivered);
	q->getChatRoom()->getPrivate()->onChatMessageReceived(q->getSharedFromThis());

	for (Content *c : contents) {
		ContentType contentType = c->getContentType();

		if (contentType.strongEqual(ContentType::Icalendar)) {
			LinphoneConferenceInfo *cConfInfo = linphone_factory_create_conference_info_from_icalendar_content(
			    linphone_factory_get(), L_GET_C_BACK_PTR(c));

			if (cConfInfo != nullptr) {
				auto confInfo = ConferenceInfo::toCpp(cConfInfo)->getSharedFromThis();
#ifdef HAVE_DB_STORAGE
				unique_ptr<MainDb> &mainDb = q->getCore()->getPrivate()->mainDb;
				mainDb->insertConferenceInfo(confInfo);
#endif // HAVE_DB_STORAGE
				linphone_core_notify_conference_info_received(q->getCore()->getCCore(), cConfInfo);
				linphone_conference_info_unref(cConfInfo);
			}
		}
	}

	return;
}

void ChatMessagePrivate::restoreFileTransferContentAsFileContent() {
	if (contents.size() == 0) {
		lWarning() << "Contents list is empty, nothing to restore";
		return;
	}

	// Restore FileContents and remove FileTransferContents
	list<Content *>::iterator it = contents.begin();
	while (it != contents.end()) {
		Content *content = *it;
		if (content && content->isFileTransfer()) {
			FileTransferContent *fileTransferContent = static_cast<FileTransferContent *>(content);
			FileContent *fileContent = fileTransferContent->getFileContent();
			if (fileContent) {
				it = contents.erase(it);
				it = contents.insert(it, fileContent);
				delete fileTransferContent;
			} else {
				lWarning() << "Found FileTransferContent but no associated FileContent";
				it++;
			}
		} else {
			it++;
		}
	}
}

void ChatMessagePrivate::send() {
	L_Q();

	shared_ptr<AbstractChatRoom> chatRoom(q->getChatRoom());
	if (!chatRoom) return;

	const auto &chatRoomState = chatRoom->getState();
	const auto &chatRoomParams = chatRoom->getCurrentParams();
	AbstractChatRoomPrivate *dChatRoom = chatRoom->getPrivate();
	if ((getContentType() == ContentType::Imdn) && chatRoomParams->isEncrypted() &&
	    (dChatRoom->isSubscriptionUnderWay() || (chatRoomState == ConferenceInterface::State::Instantiated) ||
	     (chatRoomState == ConferenceInterface::State::CreationPending))) {
		lInfo() << "IMDN message is being sent while the subscription is underway or the conference is not yet full "
		           "created";
		dChatRoom->addPendingMessage(q->getSharedFromThis());
	}

	markAsRead();
	SalOp *op = salOp;
	LinphoneCall *lcall = nullptr;
	int errorCode = 0;
	bool isResend = state == ChatMessage::State::NotDelivered;
	// Remove the sent flag so the message will be sent by the OP in case of resend
	currentSendStep &= ~ChatMessagePrivate::Step::Sent;

	currentSendStep |= ChatMessagePrivate::Step::Started;
	q->getChatRoom()->getPrivate()->addTransientChatMessage(q->getSharedFromThis());
	// imdnId.clear(); //moved into  ChatRoomPrivate::sendChatMessage

	if (toBeStored && (currentSendStep == (ChatMessagePrivate::Step::Started | ChatMessagePrivate::Step::None))) {
		storeInDb();

		if (!isResend && getContentType() != ContentType::Imdn && getContentType() != ContentType::ImIsComposing) {
			if ((currentSendStep & ChatMessagePrivate::Step::Sending) != ChatMessagePrivate::Step::Sending) {
				LinphoneChatRoom *cr = L_GET_C_BACK_PTR(q->getChatRoom());
				unique_ptr<MainDb> &mainDb = q->getCore()->getPrivate()->mainDb;
				shared_ptr<EventLog> eventLog = mainDb->getEvent(mainDb, q->getStorageId());
				_linphone_chat_room_notify_chat_message_sending(cr, L_GET_C_BACK_PTR(eventLog));
				currentSendStep |= ChatMessagePrivate::Step::Sending;
			}
		}
	}

	if ((currentSendStep & ChatMessagePrivate::Step::FileUpload) == ChatMessagePrivate::Step::FileUpload) {
		lInfo() << "File upload step already done, skipping";
	} else {
		ChatMessageModifier::Result result = fileTransferChatMessageModifier.encode(q->getSharedFromThis(), errorCode);
		if (result == ChatMessageModifier::Result::Error) {
			setState(ChatMessage::State::NotDelivered);
			// Remove current step so we go through all modifiers if message is re-sent
			currentSendStep = ChatMessagePrivate::Step::None;
			return;
		}

		if (result == ChatMessageModifier::Result::Suspended) {
			setState(ChatMessage::State::FileTransferInProgress);
			return;
		}
		currentSendStep |= ChatMessagePrivate::Step::FileUpload;
	}

	shared_ptr<Core> core = q->getCore();
	const auto toAddr = toAddress->toC();
	const auto fromAddr = fromAddress->toC();
	if (linphone_config_get_int(core->getCCore()->config, "sip", "chat_use_call_dialogs", 0) != 0) {
		lcall = linphone_core_get_call_by_remote_address2(core->getCCore(), toAddr);
		if (lcall) {
			shared_ptr<Call> call = LinphonePrivate::Call::toCpp(lcall)->getSharedFromThis();
			if ((call->getState() == CallSession::State::Connected) ||
			    (call->getState() == CallSession::State::StreamsRunning) ||
			    (call->getState() == CallSession::State::Paused) || (call->getState() == CallSession::State::Pausing) ||
			    (call->getState() == CallSession::State::PausedByRemote)) {
				lInfo() << "Send SIP msg through the existing call";
				op = call->getOp();
				string identity =
				    linphone_core_find_best_identity(core->getCCore(), linphone_call_get_remote_address(lcall));
				if (identity.empty()) {
					LinphoneProxyConfig *proxy = linphone_core_lookup_known_proxy(core->getCCore(), toAddr);
					if (proxy) {
						identity = Address::toCpp(linphone_proxy_config_get_identity_address(proxy))->toString();
					} else {
						identity = linphone_core_get_primary_contact(core->getCCore());
					}
				}
			}
		}
	}

	if (!op) {
		/* Sending out of call */
		salOp = op = new SalMessageOp(core->getCCore()->sal.get());
		linphone_configure_op_2(core->getCCore(), op, fromAddr, toAddr, getSalCustomHeaders(),
		                        !!linphone_config_get_int(core->getCCore()->config, "sip", "chat_msg_with_contact", 0));
		op->setUserPointer(q); /* If out of call, directly store msg */
	}
	op->setFromAddress(fromAddress->getImpl());
	op->setToAddress(toAddress->getImpl());

	// ---------------------------------------
	// Start of message modification
	// ---------------------------------------

	if (applyModifiers) {
		// Do not multipart or encapsulate with CPIM in an old ChatRoom to maintain backward compatibility
		if (chatRoom->canHandleMultipart()) {
			if ((currentSendStep & ChatMessagePrivate::Step::Multipart) == ChatMessagePrivate::Step::Multipart) {
				lInfo() << "Multipart step already done, skipping";
			} else {
				if (contents.size() > 1) {
					MultipartChatMessageModifier mcmm;
					mcmm.encode(q->getSharedFromThis(), errorCode);
				}
				currentSendStep |= ChatMessagePrivate::Step::Multipart;
			}
		} else {
			if (contents.size() > 1) {
				lError() << "Chat room doesn't support multipart, but has multiple parts !";
			} else lInfo() << "Chat room doesn't support multipart, skipping this modifier";
		}

#ifdef HAVE_ADVANCED_IM
		if (chatRoom->canHandleCpim()) {
			if ((currentSendStep & ChatMessagePrivate::Step::Cpim) == ChatMessagePrivate::Step::Cpim) {
				lInfo() << "Cpim step already done, skipping";
			} else {
				CpimChatMessageModifier ccmm;
				ccmm.encode(q->getSharedFromThis(), errorCode);
				currentSendStep |= ChatMessagePrivate::Step::Cpim;
			}
		} else {
			lInfo() << "Chat room doesn't support CPIM, skipping this modifier";
		}
#else
		lWarning() << "Cpim support disabled.";
#endif

		if ((currentSendStep & ChatMessagePrivate::Step::Encryption) == ChatMessagePrivate::Step::Encryption) {
			lInfo() << "Encryption step already done, skipping";
		} else {
			if (!encryptionPrevented) {
				currentSendStep |= ChatMessagePrivate::Step::Encryption;
				EncryptionChatMessageModifier ecmm;
				ChatMessageModifier::Result result = ecmm.encode(q->getSharedFromThis(), errorCode);
				if (result == ChatMessageModifier::Result::Error) {
					sal_error_info_set((SalErrorInfo *)op->getErrorInfo(), SalReasonNotAcceptable, "SIP", errorCode,
					                   "Unable to encrypt IM", nullptr);
					// Remove current step so we go through all modifiers if message is re-sent
					currentSendStep = ChatMessagePrivate::Step::None;
					restoreFileTransferContentAsFileContent();
					setState(
					    ChatMessage::State::NotDelivered); // Do it after the restore to have the correct message in db
					q->getChatRoom()->getPrivate()->removeTransientChatMessage(q->getSharedFromThis());
					return;
				} else if (result == ChatMessageModifier::Result::Suspended) {
					return;
				}
			} else {
				lInfo() << "Encryption has been prevented, skipping this modifier";
			}
		}
	} else if (linphone_core_conference_server_enabled(q->getCore()->getCCore())) {
		if (!encryptionPrevented) {
			EncryptionChatMessageModifier ecmm;
			ChatMessageModifier::Result result = ecmm.encode(q->getSharedFromThis(), errorCode);
			if (result == ChatMessageModifier::Result::Error) return;
		} else {
			lInfo() << "[server] Encryption has been prevented, skipping this modifier";
		}
	}

	// ---------------------------------------
	// End of message modification
	// ---------------------------------------

	if (internalContent.isEmpty()) {
		if (!contents.empty()) {
			internalContent = *(contents.front());
		} else if (externalBodyUrl.empty()) { // When using external body url, there is no content
			lError() << "Trying to send a message without any content !";
			return;
		}
	}

	// If message already sent by LIME X3DH synchronous encryption, do not send another one
	if ((currentSendStep & ChatMessagePrivate::Step::Sent) == ChatMessagePrivate::Step::Sent) {
		lInfo() << "Send step already done, skipping";
		return;
	}

	auto msgOp = dynamic_cast<SalMessageOpInterface *>(op);

	if (!externalBodyUrl.empty()) {
		Content content;
		ContentType contentType(ContentType::ExternalBody);
		contentType.addParameter("access-type", "URL");
		contentType.addParameter("URL", "\"" + externalBodyUrl + "\"");
		content.setContentType(contentType);
		currentSendStep |= ChatMessagePrivate::Step::Sent;
		msgOp->sendMessage(content);
	} else {
		if (!internalContent.getContentType().isValid()) internalContent.setContentType(ContentType::PlainText);
		if (!contentEncoding.empty()) internalContent.setContentEncoding(contentEncoding);
		currentSendStep |= ChatMessagePrivate::Step::Sent;
		msgOp->sendMessage(internalContent);
	}

	restoreFileTransferContentAsFileContent();

	// Remove internal content as it is not needed anymore and will confuse some old methods like getContentType()
	internalContent.setBodyFromUtf8("");
	internalContent.setContentType(ContentType(""));

	// Wait for message to be either Sent or NotDelivered unless it is an IMDN or COMPOSING
	if (getContentType() == ContentType::Imdn || getContentType() == ContentType::ImIsComposing) {
		q->getChatRoom()->getPrivate()->removeTransientChatMessage(q->getSharedFromThis());
	}

	if (imdnId.empty()) {
		setImdnMessageId(op->getCallId()); /* must be known at that time */
	}

	if (isResend) {
		// If it is a resend, reset participant states to Idle.
		// Not doing so, it will lead to the message being incorrectly marked as not delivered when at least one
		// participant hasn't received it yet.
		for (auto participant : q->getChatRoom()->getParticipants()) {
			setParticipantState(participant->getAddress(), ChatMessage::State::Idle, q->getTime());
		}
	} else if (toBeStored) {
		// Composing messages and IMDN aren't stored in DB so do not try, it will log an error message Invalid db key
		// for nothing.
		updateInDb();
	}

	if (lcall && linphone_call_get_op(lcall) == op) {
		/* In this case, chat delivery status is not notified, so unrefing chat message right now */
		/* Might be better fixed by delivering status, but too costly for now */
		return;
	}

	/* If operation failed, we should not change message state */
	if (direction == ChatMessage::Direction::Outgoing) {
		setIsReadOnly(true);
		setState(ChatMessage::State::InProgress);
	}

	// Do not notify message sent callback when it's a resend or an IMDN/Composing
	if (!isResend && getContentType() != ContentType::Imdn && getContentType() != ContentType::ImIsComposing) {
		q->getChatRoom()->getPrivate()->onChatMessageSent(q->getSharedFromThis());
	}
}

void ChatMessagePrivate::storeInDb() {
	L_Q();

	// TODO: store message in the future
	if (linphone_core_conference_server_enabled(q->getCore()->getCCore())) return;

	if (q->isValid()) {
		updateInDb();
		return;
	}

	shared_ptr<EventLog> eventLog = make_shared<ConferenceChatMessageEvent>(time, q->getSharedFromThis());

	// Avoid transaction in transaction if contents are not loaded.
	loadContentsFromDatabase();

	shared_ptr<AbstractChatRoom> chatRoom(q->getChatRoom());
	if (!chatRoom) return;

	AbstractChatRoomPrivate *dChatRoom = chatRoom->getPrivate();
	dChatRoom->addEvent(eventLog); // From this point forward the chat message will have a valid dbKey

	if (direction == ChatMessage::Direction::Incoming) {
		if (hasFileTransferContent()) {
			// Keep the event in the transient list, message storage can be updated in near future
			dChatRoom->addTransientEvent(eventLog);
		}
	} else {
		// Keep event in transient to be able to store in database state changes
		dChatRoom->addTransientEvent(eventLog);
	}
}

void ChatMessagePrivate::updateInDb() {
	L_Q();

	if (!q->isValid()) {
		lError() << "Invalid storage ID [" << storageId << "] associated to message [" << q->getSharedFromThis() << "]";
		return;
	}

	unique_ptr<MainDb> &mainDb = q->getChatRoom()->getCore()->getPrivate()->mainDb;
	shared_ptr<EventLog> eventLog = mainDb->getEvent(mainDb, q->getStorageId());

	if (!eventLog) {
		lError() << "cannot find eventLog for storage ID [" << storageId << "] associated to message ["
		         << q->getSharedFromThis() << "]";
		return;
	}
	// Avoid transaction in transaction if contents are not loaded.
	loadContentsFromDatabase();
	mainDb->updateEvent(eventLog);

	if (direction == ChatMessage::Direction::Incoming) {
		if (!hasFileTransferContent()) {
			// Incoming message doesn't have any download waiting anymore, we can remove it's event from the transients
			q->getChatRoom()->getPrivate()->removeTransientEvent(eventLog);
		}
	} else {
		if (state == ChatMessage::State::Delivered || state == ChatMessage::State::NotDelivered) {
			// Once message has reached this state it won't change anymore so we can remove the event from the
			// transients
			q->getChatRoom()->getPrivate()->removeTransientEvent(eventLog);
		}
	}
}

// -----------------------------------------------------------------------------

bool ChatMessagePrivate::isValidStateTransition(ChatMessage::State currentState, ChatMessage::State newState) {
	if (newState == currentState) return false;

	return !((currentState == ChatMessage::State::Displayed || currentState == ChatMessage::State::DeliveredToUser) &&
	         (newState == ChatMessage::State::DeliveredToUser || newState == ChatMessage::State::Delivered ||
	          newState == ChatMessage::State::NotDelivered));
}

// -----------------------------------------------------------------------------

ChatMessage::ChatMessage(const shared_ptr<AbstractChatRoom> &chatRoom, ChatMessage::Direction direction)
    : Object(*new ChatMessagePrivate(chatRoom, direction)), CoreAccessor(chatRoom->getCore()) {
}

ChatMessage::ChatMessage(ChatMessagePrivate &p) : Object(p), CoreAccessor(p.getPublic()->getChatRoom()->getCore()) {
}

ChatMessage::~ChatMessage() {
	fileUploadEndBackgroundTask();
	deleteChatMessageFromCache();
}

bool ChatMessage::isValid() const {
	std::shared_ptr<Core> core = nullptr;
	try {
		core = getCore();
	} catch (const bad_weak_ptr &) {
		return false; // Return false if core is destroyed.
	}

	return (core && (getStorageId() >= 0));
}

void ChatMessage::deleteChatMessageFromCache() {
	if (isValid()) {
		// Delete chat message from the cache
		unique_ptr<MainDb> &mainDb = getCore()->getPrivate()->mainDb;
		mainDb->getPrivate()->storageIdToChatMessage.erase(getStorageId());
	}
}

long long ChatMessage::getStorageId() const {
	L_D();
	return d->storageId;
}

shared_ptr<AbstractChatRoom> ChatMessage::getChatRoom() const {
	L_D();

	shared_ptr<AbstractChatRoom> chatRoom(d->chatRoom.lock());
	if (!chatRoom) lError() << "Unable to get valid chat room instance.";

	return chatRoom;
}

// -----------------------------------------------------------------------------

time_t ChatMessage::getTime() const {
	L_D();
	return d->time;
}

bool ChatMessage::isSecured() const {
	L_D();
	return d->isSecured;
}

void ChatMessage::setIsSecured(bool isSecured) {
	L_D();
	d->isSecured = isSecured;
}

ChatMessage::Direction ChatMessage::getDirection() const {
	L_D();
	return d->direction;
}

ChatMessage::State ChatMessage::getState() const {
	L_D();
	return d->state;
}

const string &ChatMessage::getImdnMessageId() const {
	L_D();
	return d->imdnId;
}

void ChatMessagePrivate::setImdnMessageId(const string &id) {
	imdnId = id;
}

const string &ChatMessagePrivate::getCallId() const {
	return callId;
}

void ChatMessagePrivate::setCallId(const string &id) {
	callId = id;
}

void ChatMessagePrivate::setForwardInfo(const string &fInfo) {
	forwardInfo = fInfo;
}

void ChatMessagePrivate::setReplyToMessageIdAndSenderAddress(const string &id, const std::shared_ptr<Address> &sender) {
	replyingToMessageId = id;
	replyingToMessageSender = sender;
}

bool ChatMessage::isReply() const {
	L_D();
	return !d->replyingToMessageId.empty();
}

const string &ChatMessage::getReplyToMessageId() const {
	L_D();
	return d->replyingToMessageId;
}

const std::shared_ptr<Address> &ChatMessage::getReplyToSenderAddress() const {
	L_D();
	return d->replyingToMessageSender;
}

shared_ptr<ChatMessage> ChatMessage::getReplyToMessage() const {
	if (!isReply()) return nullptr;
	return getChatRoom()->findChatMessage(getReplyToMessageId());
}

void ChatMessagePrivate::enableEphemeralWithTime(long time) {
	isEphemeral = true;
	ephemeralLifetime = time;
}

void ChatMessagePrivate::loadContentsFromDatabase() const {
	L_Q();

	if (contentsNotLoadedFromDatabase) {
		isReadOnly = false;
		contentsNotLoadedFromDatabase = false;
		q->getChatRoom()->getCore()->getPrivate()->mainDb->loadChatMessageContents(
		    const_pointer_cast<ChatMessage>(q->getSharedFromThis()));

		isReadOnly = true;
	}
}

bool ChatMessage::isRead() const {
	L_D();
	return d->markedAsRead || d->state == State::Displayed;
}

const Address &ChatMessage::getAuthenticatedFromAddress() const {
	L_D();
	return d->authenticatedFromAddress;
}

const std::shared_ptr<Address> &ChatMessage::getFromAddress() const {
	L_D();
	return d->fromAddress;
}

const std::shared_ptr<Address> &ChatMessage::getToAddress() const {
	L_D();
	return d->toAddress;
}

const std::shared_ptr<Address> &ChatMessage::getLocalAddress() const {
	L_D();
	if (getDirection() == Direction::Outgoing) return d->fromAddress;
	else return d->toAddress;
}

const std::shared_ptr<Address> &ChatMessage::getRecipientAddress() const {
	L_D();
	return d->recipientAddress;
}

const std::string &ChatMessage::getForwardInfo() const {
	L_D();
	return d->forwardInfo;
}

bool ChatMessage::getToBeStored() const {
	L_D();
	return d->toBeStored;
}

bool ChatMessage::isEphemeral() const {
	L_D();
	return d->isEphemeral;
}

long ChatMessage::getEphemeralLifetime() const {
	L_D();
	return d->ephemeralLifetime;
}

time_t ChatMessage::getEphemeralExpireTime() const {
	L_D();
	return d->ephemeralExpireTime;
}

void ChatMessage::setToBeStored(bool value) {
	L_D();
	d->toBeStored = value;
}

// -----------------------------------------------------------------------------

list<ParticipantImdnState> ChatMessage::getParticipantsByImdnState(ChatMessage::State state) const {
	list<ParticipantImdnState> result;
	if (!(getChatRoom()->getCapabilities() & AbstractChatRoom::Capabilities::Conference) || !isValid()) return result;

	unique_ptr<MainDb> &mainDb = getChatRoom()->getCore()->getPrivate()->mainDb;
	shared_ptr<EventLog> eventLog = mainDb->getEvent(mainDb, getStorageId());
	list<MainDb::ParticipantState> dbResults = mainDb->getChatMessageParticipantsByImdnState(eventLog, state);
	for (const auto &dbResult : dbResults) {
		auto sender = getChatRoom()->findParticipant(getFromAddress());
		auto participant = getChatRoom()->findParticipant(dbResult.address);
		if (participant && (participant != sender))
			result.emplace_back(participant, dbResult.state, dbResult.timestamp);
	}

	return result;
}

// -----------------------------------------------------------------------------

const LinphoneErrorInfo *ChatMessage::getErrorInfo() const {
	L_D();
	if (!d->errorInfo) d->errorInfo = linphone_error_info_new(); // let's do it mutable
	linphone_error_info_from_sal_op(d->errorInfo, d->salOp);
	return d->errorInfo;
}

bool ChatMessage::isReadOnly() const {
	L_D();
	return d->isReadOnly;
}

const list<Content *> &ChatMessage::getContents() const {
	L_D();
	return d->getContents();
}

void ChatMessage::addContent(Content *content) {
	L_D();
	if (!d->isReadOnly) d->addContent(content);
}

void ChatMessage::removeContent(Content *content) {
	L_D();
	if (!d->isReadOnly) d->removeContent(content);
}

const Content &ChatMessage::getInternalContent() const {
	L_D();
	return d->internalContent;
}

void ChatMessage::setInternalContent(const Content &content) {
	L_D();
	d->internalContent = content;
}

void ChatMessage::send() {
	L_D();

	// Do not allow sending a message that is already being sent or that has been correctly delivered/displayed
	if ((d->state == State::InProgress) || (d->state == State::Delivered) || (d->state == State::FileTransferDone) ||
	    (d->state == State::DeliveredToUser) || (d->state == State::Displayed) ||
	    (d->state == State::FileTransferInProgress)) {
		lWarning() << "Cannot send chat message in state " << Utils::toString(d->state);
		return;
	}

	// Remove the modifiers flag so the message will go through CPIM, Multipart and Encryption again in case of resent
	d->currentSendStep &= ~ChatMessagePrivate::Step::Multipart;
	d->currentSendStep &= ~ChatMessagePrivate::Step::Cpim;
	d->currentSendStep &= ~ChatMessagePrivate::Step::Encryption;

	d->loadContentsFromDatabase();

	getChatRoom()->getPrivate()->sendChatMessage(getSharedFromThis());
}

bool ChatMessage::downloadFile(FileTransferContent *fileTransferContent) {
	L_D();
	return d->fileTransferChatMessageModifier.downloadFile(getSharedFromThis(), fileTransferContent);
}

bool ChatMessage::isFileTransferInProgress() const {
	L_D();
	return d->fileTransferChatMessageModifier.isFileTransferInProgressAndValid();
}

void ChatMessage::cancelFileTransfer() {
	L_D();
	if (d->fileTransferChatMessageModifier.isFileTransferInProgressAndValid()) {
		lWarning() << "Canceling file transfer on message [" << getSharedFromThis() << "]";
		d->fileTransferChatMessageModifier.cancelFileTransfer();
		lInfo() << "File transfer on message [" << getSharedFromThis() << "] has been cancelled";

		if (d->state == State::FileTransferInProgress) {
			lInfo() << "File transfer on message [" << getSharedFromThis() << "] was in progress, updating state";
			// For auto download messages, set the state back to Delivered
			if (d->isAutoFileTransferDownloadInProgress()) {
				d->setState(State::Delivered);
				getChatRoom()->getPrivate()->removeTransientChatMessage(getSharedFromThis());
			} else {
				d->setState(State::NotDelivered);
			}
		}
	} else {
		lInfo() << "No existing file transfer - nothing to cancel";
	}
}

int ChatMessage::putCharacter(uint32_t character) {
	L_D();

	constexpr uint32_t newLine = 0x2028;
	constexpr uint32_t crlf = 0x0D0A;
	constexpr uint32_t lf = 0x0A;

	shared_ptr<AbstractChatRoom> chatRoom = getChatRoom();
	if (!(chatRoom->getCapabilities() & LinphonePrivate::ChatRoom::Capabilities::RealTimeText)) {
		lError() << "Chat room [" << chatRoom << "] that created the message doesn't have RealTimeText capability";
		return -1;
	}

	shared_ptr<Call> call = chatRoom->getCall();
	if (!call || !call->getMediaStream(LinphoneStreamTypeText)) {
		lError() << "Failed to find Text stream from call [" << call << "]";
		return -1;
	}

	if (character == newLine || character == crlf || character == lf) {
		shared_ptr<Core> core = getCore();
		if (linphone_config_get_int(core->getCCore()->config, "misc", "store_rtt_messages", 1) == 1) {
			lInfo() << "New line sent, forge a message with content " << d->rttMessage;
			d->state = State::Displayed;
			d->setText(d->rttMessage);
			d->storeInDb();
			d->rttMessage = "";
		}
	} else {
		string value = LinphonePrivate::Utils::unicodeToUtf8(character);
		d->rttMessage += value;
		lDebug() << "Sent RTT character: " << value << "(" << (unsigned long)character << "), pending text is "
		         << d->rttMessage;
	}

	text_stream_putchar32(reinterpret_cast<TextStream *>(call->getMediaStream(LinphoneStreamTypeText)), character);

	return 0;
}

void ChatMessage::fileUploadEndBackgroundTask() {
	L_D();
	d->fileTransferChatMessageModifier.fileUploadEndBackgroundTask();
}

void ChatMessage::addListener(shared_ptr<ChatMessageListener> listener) {
	L_D();
	d->listeners.push_back(listener);
}

void ChatMessage::removeListener(shared_ptr<ChatMessageListener> listener) {
	L_D();
	d->listeners.remove(listener);
}

std::ostream &operator<<(std::ostream &lhs, ChatMessage::State e) {
	switch (e) {
		case ChatMessage::State::Idle:
			lhs << "Idle";
			break;
		case ChatMessage::State::InProgress:
			lhs << "InProgress";
			break;
		case ChatMessage::State::Delivered:
			lhs << "Delivered";
			break;
		case ChatMessage::State::NotDelivered:
			lhs << "NotDelivered";
			break;
		case ChatMessage::State::FileTransferError:
			lhs << "FileTransferError";
			break;
		case ChatMessage::State::FileTransferDone:
			lhs << "FileTransferDone";
			break;
		case ChatMessage::State::DeliveredToUser:
			lhs << "DeliveredToUser";
			break;
		case ChatMessage::State::Displayed:
			lhs << "Displayed";
			break;
		case ChatMessage::State::FileTransferInProgress:
			lhs << "FileTransferInProgress";
			break;
	}
	return lhs;
}

LINPHONE_END_NAMESPACE
