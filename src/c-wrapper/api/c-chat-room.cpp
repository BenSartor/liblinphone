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
#include <iterator>

#include <bctoolbox/defs.h>

// TODO: Remove me later.
#include "linphone/chat.h"

#include "linphone/api/c-chat-room.h"
#include "linphone/wrapper_utils.h"

#include "address/address.h"
#include "c-wrapper/c-wrapper.h"
#include "call/call.h"
#include "chat/chat-message/chat-message-p.h"
#include "chat/chat-room/abstract-chat-room-p.h"
#ifdef HAVE_ADVANCED_IM
#include "chat/chat-room/client-group-chat-room-p.h"
#include "chat/chat-room/server-group-chat-room-p.h"
#endif
#include "conference/participant.h"
#include "core/core-p.h"
#include "event-log/event-log.h"
#include "linphone/utils/utils.h"

// =============================================================================

using namespace std;

static void _linphone_chat_room_constructor(LinphoneChatRoom *cr);
static void _linphone_chat_room_destructor(LinphoneChatRoom *cr);

L_DECLARE_C_OBJECT_IMPL_WITH_XTORS(ChatRoom,
                                   _linphone_chat_room_constructor,
                                   _linphone_chat_room_destructor,
                                   bctbx_list_t *callbacks; /* A list of LinphoneCallCbs object */
                                   LinphoneChatRoomCbs *
                                   currentCbs; /* The current LinphoneCallCbs object used to call a callback */
                                   mutable bctbx_list_t * composingAddresses;)

static void _linphone_chat_room_constructor(BCTBX_UNUSED(LinphoneChatRoom *cr)) {
}

static void _linphone_chat_room_destructor(LinphoneChatRoom *cr) {
	_linphone_chat_room_clear_callbacks(cr);
}

void linphone_chat_room_allow_multipart(LinphoneChatRoom *room) {
	L_GET_CPP_PTR_FROM_C_OBJECT(room)->allowMultipart(true);
}

void linphone_chat_room_allow_cpim(LinphoneChatRoom *room) {
	L_GET_CPP_PTR_FROM_C_OBJECT(room)->allowCpim(true);
}

// =============================================================================
// Public functions.
// =============================================================================

const LinphoneChatRoomParams *linphone_chat_room_get_current_params(const LinphoneChatRoom *cr) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getCurrentParams()->toC();
}

// Deprecated
void linphone_chat_room_send_message(LinphoneChatRoom *cr, const char *msg) {
	L_GET_CPP_PTR_FROM_C_OBJECT(cr)->createChatMessage(msg)->send();
}

bool_t linphone_chat_room_is_remote_composing(const LinphoneChatRoom *cr) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(cr)->isRemoteComposing();
}

LinphoneCore *linphone_chat_room_get_lc(const LinphoneChatRoom *cr) {
	return linphone_chat_room_get_core(cr);
}

LinphoneCore *linphone_chat_room_get_core(const LinphoneChatRoom *cr) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getCore()->getCCore();
}

const LinphoneAddress *linphone_chat_room_get_peer_address(LinphoneChatRoom *cr) {
	const auto &address = L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getPeerAddress();
	if (address && address->isValid()) {
		return address->toC();
	} else {
		return NULL;
	}
}

const LinphoneAddress *linphone_chat_room_get_local_address(LinphoneChatRoom *cr) {
	const auto &address = L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getLocalAddress();
	if (address && address->isValid()) {
		return address->toC();
	} else {
		return NULL;
	}
}

LinphoneChatMessage *linphone_chat_room_create_empty_message(LinphoneChatRoom *cr) {
	shared_ptr<LinphonePrivate::ChatMessage> cppPtr = L_GET_CPP_PTR_FROM_C_OBJECT(cr)->createChatMessage();
	LinphoneChatMessage *object = L_INIT(ChatMessage);
	L_SET_CPP_PTR_FROM_C_OBJECT(object, cppPtr);
	return object;
}

LinphoneChatMessage *linphone_chat_room_create_message_from_utf8(LinphoneChatRoom *cr, const char *message) {
	shared_ptr<LinphonePrivate::ChatMessage> cppPtr =
	    L_GET_CPP_PTR_FROM_C_OBJECT(cr)->createChatMessageFromUtf8(L_C_TO_STRING(message));
	LinphoneChatMessage *object = L_INIT(ChatMessage);
	L_SET_CPP_PTR_FROM_C_OBJECT(object, cppPtr);
	return object;
}

// Deprecated
LinphoneChatMessage *linphone_chat_room_create_message(LinphoneChatRoom *cr, const char *message) {
	shared_ptr<LinphonePrivate::ChatMessage> cppPtr =
	    L_GET_CPP_PTR_FROM_C_OBJECT(cr)->createChatMessage(L_C_TO_STRING(message));
	LinphoneChatMessage *object = L_INIT(ChatMessage);
	L_SET_CPP_PTR_FROM_C_OBJECT(object, cppPtr);
	return object;
}

LinphoneChatMessage *linphone_chat_room_create_file_transfer_message(LinphoneChatRoom *cr,
                                                                     LinphoneContent *initial_content) {
	LinphoneChatMessage *msg = linphone_chat_room_create_empty_message(cr);
	linphone_chat_message_add_file_content(msg, initial_content);
	return msg;
}

// Deprecated
LinphoneChatMessage *linphone_chat_room_create_message_2(LinphoneChatRoom *cr,
                                                         const char *message,
                                                         const char *external_body_url,
                                                         LinphoneChatMessageState state,
                                                         time_t time,
                                                         BCTBX_UNUSED(bool_t is_read),
                                                         BCTBX_UNUSED(bool_t is_incoming)) {
	LinphoneChatMessage *msg = linphone_chat_room_create_message(cr, message);

	linphone_chat_message_set_external_body_url(msg, external_body_url ? ms_strdup(external_body_url) : NULL);

	LinphonePrivate::ChatMessagePrivate *dMsg = L_GET_PRIVATE_FROM_C_OBJECT(msg);
	dMsg->setTime(time);
	dMsg->setState(static_cast<LinphonePrivate::ChatMessage::State>(state));

	return msg;
}

LinphoneChatMessage *linphone_chat_room_create_forward_message(LinphoneChatRoom *cr, LinphoneChatMessage *msg) {
	shared_ptr<LinphonePrivate::ChatMessage> cppPtr =
	    L_GET_CPP_PTR_FROM_C_OBJECT(cr)->createForwardMessage(L_GET_CPP_PTR_FROM_C_OBJECT(msg));
	LinphoneChatMessage *object = L_INIT(ChatMessage);
	L_SET_CPP_PTR_FROM_C_OBJECT(object, cppPtr);
	return object;
}

LinphoneChatMessage *linphone_chat_room_create_reply_message(LinphoneChatRoom *cr, LinphoneChatMessage *msg) {
	shared_ptr<LinphonePrivate::ChatMessage> cppPtr =
	    L_GET_CPP_PTR_FROM_C_OBJECT(cr)->createReplyMessage(L_GET_CPP_PTR_FROM_C_OBJECT(msg));
	LinphoneChatMessage *object = L_INIT(ChatMessage);
	L_SET_CPP_PTR_FROM_C_OBJECT(object, cppPtr);
	return object;
}

LinphoneChatMessage *linphone_chat_room_create_voice_recording_message(LinphoneChatRoom *cr,
                                                                       LinphoneRecorder *recorder) {
	LinphoneChatMessage *chat_message = linphone_chat_room_create_empty_message(cr);

	LinphoneContent *c_content = linphone_recorder_create_content(recorder);
	if (c_content != nullptr) {
		linphone_chat_message_add_content(chat_message, c_content);
		linphone_content_unref(c_content);
	}

	return chat_message;
}

void linphone_chat_room_send_chat_message_2(BCTBX_UNUSED(LinphoneChatRoom *cr), LinphoneChatMessage *msg) {
	linphone_chat_message_ref(msg);
	L_GET_CPP_PTR_FROM_C_OBJECT(msg)->send();
}

void linphone_chat_room_send_chat_message(BCTBX_UNUSED(LinphoneChatRoom *cr), LinphoneChatMessage *msg) {
	L_GET_CPP_PTR_FROM_C_OBJECT(msg)->send();
}

void linphone_chat_room_receive_chat_message(BCTBX_UNUSED(LinphoneChatRoom *cr), LinphoneChatMessage *msg) {
	L_GET_PRIVATE_FROM_C_OBJECT(msg)->receive();
}

uint32_t linphone_chat_room_get_char(LinphoneChatRoom *cr) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getChar();
}

void linphone_chat_room_compose(LinphoneChatRoom *cr) {
	L_GET_CPP_PTR_FROM_C_OBJECT(cr)->compose();
}

LinphoneCall *linphone_chat_room_get_call(const LinphoneChatRoom *cr) {
	shared_ptr<LinphonePrivate::Call> call = L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getCall();
	if (call) return call->toC();
	return nullptr;
}

void linphone_chat_room_set_call(LinphoneChatRoom *cr, LinphoneCall *call) {
	L_GET_PRIVATE_FROM_C_OBJECT(cr)->setCallId(linphone_call_log_get_call_id(linphone_call_get_call_log(call)));
}

void linphone_chat_room_mark_as_read(LinphoneChatRoom *cr) {
	L_GET_CPP_PTR_FROM_C_OBJECT(cr)->markAsRead();
}

void linphone_chat_room_set_ephemeral_mode(LinphoneChatRoom *cr, LinphoneChatRoomEphemeralMode mode) {
	L_GET_CPP_PTR_FROM_C_OBJECT(cr)->setEphemeralMode(
	    static_cast<LinphonePrivate::AbstractChatRoom::EphemeralMode>(mode), true);
}

LinphoneChatRoomEphemeralMode linphone_chat_room_get_ephemeral_mode(const LinphoneChatRoom *cr) {
	return static_cast<LinphoneChatRoomEphemeralMode>(L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getEphemeralMode());
}

void linphone_chat_room_enable_ephemeral(LinphoneChatRoom *cr, bool_t ephem) {
	L_GET_CPP_PTR_FROM_C_OBJECT(cr)->enableEphemeral(!!ephem, true);
}

bool_t linphone_chat_room_ephemeral_enabled(const LinphoneChatRoom *cr) {
	return (bool_t)L_GET_CPP_PTR_FROM_C_OBJECT(cr)->ephemeralEnabled();
}

void linphone_chat_room_set_ephemeral_lifetime(LinphoneChatRoom *cr, long time) {
	L_GET_CPP_PTR_FROM_C_OBJECT(cr)->setEphemeralLifetime(time, true);
}

long linphone_chat_room_get_ephemeral_lifetime(const LinphoneChatRoom *cr) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getEphemeralLifetime();
}

bool_t linphone_chat_room_ephemeral_supported_by_all_participants(const LinphoneChatRoom *cr) {
	return (bool_t)L_GET_CPP_PTR_FROM_C_OBJECT(cr)->ephemeralSupportedByAllParticipants();
}

int linphone_chat_room_get_unread_messages_count(LinphoneChatRoom *cr) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getUnreadChatMessageCount();
}

int linphone_chat_room_get_history_size(LinphoneChatRoom *cr) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getChatMessageCount();
}

bool_t linphone_chat_room_is_empty(LinphoneChatRoom *cr) {
	return (bool_t)L_GET_CPP_PTR_FROM_C_OBJECT(cr)->isEmpty();
}

void linphone_chat_room_delete_message(LinphoneChatRoom *cr, LinphoneChatMessage *msg) {
	L_GET_CPP_PTR_FROM_C_OBJECT(cr)->deleteMessageFromHistory(L_GET_CPP_PTR_FROM_C_OBJECT(msg));
}

void linphone_chat_room_delete_history(LinphoneChatRoom *cr) {
	L_GET_CPP_PTR_FROM_C_OBJECT(cr)->deleteHistory();
}

bctbx_list_t *linphone_chat_room_get_history_range(LinphoneChatRoom *cr, int startm, int endm) {
	list<shared_ptr<LinphonePrivate::ChatMessage>> chatMessages;
	for (auto &event : L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getMessageHistoryRange(startm, endm))
		chatMessages.push_back(
		    static_pointer_cast<LinphonePrivate::ConferenceChatMessageEvent>(event)->getChatMessage());

	return L_GET_RESOLVED_C_LIST_FROM_CPP_LIST(chatMessages);
}

bctbx_list_t *linphone_chat_room_get_history(LinphoneChatRoom *cr, int nb_message) {
	return linphone_chat_room_get_history_range(cr, 0, nb_message);
}

bctbx_list_t *linphone_chat_room_get_unread_history(LinphoneChatRoom *cr) {
	return L_GET_RESOLVED_C_LIST_FROM_CPP_LIST(L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getUnreadChatMessages());
}

bctbx_list_t *linphone_chat_room_get_history_range_message_events(LinphoneChatRoom *cr, int startm, int endm) {
	return L_GET_RESOLVED_C_LIST_FROM_CPP_LIST(L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getMessageHistoryRange(startm, endm));
}

bctbx_list_t *linphone_chat_room_get_history_message_events(LinphoneChatRoom *cr, int nb_events) {
	return L_GET_RESOLVED_C_LIST_FROM_CPP_LIST(L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getMessageHistory(nb_events));
}

bctbx_list_t *linphone_chat_room_get_history_events(LinphoneChatRoom *cr, int nb_events) {
	return L_GET_RESOLVED_C_LIST_FROM_CPP_LIST(L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getHistory(nb_events));
}

bctbx_list_t *linphone_chat_room_get_history_range_events(LinphoneChatRoom *cr, int begin, int end) {
	return L_GET_RESOLVED_C_LIST_FROM_CPP_LIST(L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getHistoryRange(begin, end));
}

int linphone_chat_room_get_history_events_size(LinphoneChatRoom *cr) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getHistorySize();
}

LinphoneChatMessage *linphone_chat_room_get_last_message_in_history(LinphoneChatRoom *cr) {
	shared_ptr<LinphonePrivate::ChatMessage> cppPtr = L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getLastChatMessageInHistory();
	if (!cppPtr) return nullptr;

	return linphone_chat_message_ref(L_GET_C_BACK_PTR(cppPtr));
}

LinphoneChatMessage *linphone_chat_room_find_message(LinphoneChatRoom *cr, const char *message_id) {
	shared_ptr<LinphonePrivate::ChatMessage> cppPtr = L_GET_CPP_PTR_FROM_C_OBJECT(cr)->findChatMessage(message_id);
	if (!cppPtr) return nullptr;

	return linphone_chat_message_ref(L_GET_C_BACK_PTR(cppPtr));
}

LinphoneChatRoomState linphone_chat_room_get_state(const LinphoneChatRoom *cr) {
	return linphone_conference_state_to_chat_room_state(
	    static_cast<LinphoneConferenceState>(L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getState()));
}

bool_t linphone_chat_room_has_been_left(const LinphoneChatRoom *cr) {
	return (bool_t)L_GET_CPP_PTR_FROM_C_OBJECT(cr)->hasBeenLeft();
}

bool_t linphone_chat_room_is_read_only(const LinphoneChatRoom *cr) {
	return (bool_t)L_GET_CPP_PTR_FROM_C_OBJECT(cr)->isReadOnly();
}

time_t linphone_chat_room_get_creation_time(const LinphoneChatRoom *cr) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getCreationTime();
}

time_t linphone_chat_room_get_last_update_time(const LinphoneChatRoom *cr) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getLastUpdateTime();
}

void linphone_chat_room_add_participant(LinphoneChatRoom *cr, LinphoneAddress *addr) {
	L_GET_CPP_PTR_FROM_C_OBJECT(cr)->addParticipant(LinphonePrivate::Address::toCpp(addr)->getSharedFromThis());
}

bool_t linphone_chat_room_add_participants(LinphoneChatRoom *cr, const bctbx_list_t *addresses) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(cr)->addParticipants(
	    LinphonePrivate::Utils::bctbxListToCppSharedPtrList<LinphoneAddress, LinphonePrivate::Address>(addresses));
}

LinphoneParticipant *linphone_chat_room_find_participant(const LinphoneChatRoom *cr, LinphoneAddress *addr) {
	std::shared_ptr<LinphonePrivate::Participant> participant =
	    L_GET_CPP_PTR_FROM_C_OBJECT(cr)->findParticipant(LinphonePrivate::Address::toCpp(addr)->getSharedFromThis());
	if (participant) {
		return participant->toC();
	}
	return NULL;
}

bool_t linphone_chat_room_can_handle_participants(const LinphoneChatRoom *cr) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(cr)->canHandleParticipants();
}

LinphoneChatRoomCapabilitiesMask linphone_chat_room_get_capabilities(const LinphoneChatRoom *cr) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getCapabilities();
}

bool_t linphone_chat_room_has_capability(const LinphoneChatRoom *cr, int mask) {
	return static_cast<bool_t>(L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getCapabilities() & mask);
}

const LinphoneAddress *linphone_chat_room_get_conference_address(const LinphoneChatRoom *cr) {
	const auto &confAddress = L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getConferenceAddress();
	if (confAddress && confAddress->isValid()) {
		return confAddress->toC();
	} else {
		return NULL;
	}
}

LinphoneParticipant *linphone_chat_room_get_me(const LinphoneChatRoom *cr) {
	std::shared_ptr<LinphonePrivate::Participant> me = L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getMe();
	if (me) {
		return me->toC();
	}
	return NULL;
}

int linphone_chat_room_get_nb_participants(const LinphoneChatRoom *cr) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getParticipantCount();
}

bctbx_list_t *linphone_chat_room_get_participants(const LinphoneChatRoom *cr) {
	return LinphonePrivate::Participant::getCListFromCppList(L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getParticipants());
}

const char *linphone_chat_room_get_subject(const LinphoneChatRoom *cr) {
	return L_STRING_TO_C(L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getSubject());
}

LinphoneChatRoomSecurityLevel linphone_chat_room_get_security_level(LinphoneChatRoom *cr) {
	return (LinphoneChatRoomSecurityLevel)L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getSecurityLevel();
}

void linphone_chat_room_leave(LinphoneChatRoom *cr) {
	L_GET_CPP_PTR_FROM_C_OBJECT(cr)->leave();
}

void linphone_chat_room_remove_participant(LinphoneChatRoom *cr, LinphoneParticipant *participant) {
	L_GET_CPP_PTR_FROM_C_OBJECT(cr)->removeParticipant(
	    LinphonePrivate::Participant::toCpp(participant)->getSharedFromThis());
}

void linphone_chat_room_remove_participants(LinphoneChatRoom *cr, const bctbx_list_t *participants) {
	L_GET_CPP_PTR_FROM_C_OBJECT(cr)->removeParticipants(
	    LinphonePrivate::Participant::getCppListFromCList(participants));
}

void linphone_chat_room_set_participant_admin_status(LinphoneChatRoom *cr,
                                                     LinphoneParticipant *participant,
                                                     bool_t isAdmin) {
	shared_ptr<LinphonePrivate::Participant> p = LinphonePrivate::Participant::toCpp(participant)->getSharedFromThis();
	L_GET_CPP_PTR_FROM_C_OBJECT(cr)->setParticipantAdminStatus(p, !!isAdmin);
}

void linphone_chat_room_set_subject(LinphoneChatRoom *cr, const char *subject) {
	L_GET_CPP_PTR_FROM_C_OBJECT(cr)->setSubject(L_C_TO_STRING(subject));
}

const bctbx_list_t *linphone_chat_room_get_composing_addresses(LinphoneChatRoom *cr) {
	list<shared_ptr<LinphonePrivate::Address>> addresses = L_GET_CPP_PTR_FROM_C_OBJECT(cr)->getComposingAddresses();
	cr->composingAddresses =
	    LinphonePrivate::Utils::listToCBctbxList<LinphoneAddress, LinphonePrivate::Address>(addresses);
	return cr->composingAddresses;
}

#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif // _MSC_VER
void linphone_chat_room_set_conference_address(LinphoneChatRoom *cr, LinphoneAddress *confAddr) {
#ifdef HAVE_ADVANCED_IM
	LinphonePrivate::ServerGroupChatRoomPrivate *sgcr =
	    dynamic_cast<LinphonePrivate::ServerGroupChatRoomPrivate *>(L_GET_PRIVATE_FROM_C_OBJECT(cr));
	if (sgcr) {
		std::shared_ptr<LinphonePrivate::Address> idAddr =
		    LinphonePrivate::Address::toCpp(confAddr)->getSharedFromThis();
		sgcr->setConferenceAddress(idAddr);
	}
#else
	lWarning() << "Advanced IM such as group chat is disabled!";
#endif
}
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif // _MSC_VER

#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif // _MSC_VER
void linphone_chat_room_set_participant_devices(LinphoneChatRoom *cr,
                                                LinphoneAddress *partAddr,
                                                const bctbx_list_t *deviceIdentities) {
#ifdef HAVE_ADVANCED_IM
	list<shared_ptr<LinphonePrivate::ParticipantDeviceIdentity>> lDevicesIdentities =
	    LinphonePrivate::ParticipantDeviceIdentity::getCppListFromCList(deviceIdentities);
	LinphonePrivate::ServerGroupChatRoomPrivate *sgcr =
	    dynamic_cast<LinphonePrivate::ServerGroupChatRoomPrivate *>(L_GET_PRIVATE_FROM_C_OBJECT(cr));
	if (sgcr)
		sgcr->setParticipantDevices(LinphonePrivate::Address::toCpp(partAddr)->getSharedFromThis(), lDevicesIdentities);
#else
	lWarning() << "Advanced IM such as group chat is disabled!";
#endif
}
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif // _MSC_VER

#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif // _MSC_VER
void linphone_chat_room_notify_participant_device_registration(LinphoneChatRoom *cr,
                                                               const LinphoneAddress *participant_device) {
#ifdef HAVE_ADVANCED_IM
	LinphonePrivate::ServerGroupChatRoomPrivate *sgcr =
	    dynamic_cast<LinphonePrivate::ServerGroupChatRoomPrivate *>(L_GET_PRIVATE_FROM_C_OBJECT(cr));
	if (sgcr)
		sgcr->notifyParticipantDeviceRegistration(
		    LinphonePrivate::Address::toCpp(const_cast<LinphoneAddress *>(participant_device))->getSharedFromThis());
#else
	lWarning() << "Advanced IM such as group chat is disabled!";
#endif
}
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif // _MSC_VER

// =============================================================================
// Callbacks
// =============================================================================

void _linphone_chat_room_clear_callbacks(LinphoneChatRoom *cr) {
	bctbx_list_free_with_data(cr->callbacks, (bctbx_list_free_func)linphone_chat_room_cbs_unref);
	cr->callbacks = nullptr;
}

void linphone_chat_room_add_callbacks(LinphoneChatRoom *cr, LinphoneChatRoomCbs *cbs) {
	cr->callbacks = bctbx_list_append(cr->callbacks, linphone_chat_room_cbs_ref(cbs));
}

void linphone_chat_room_remove_callbacks(LinphoneChatRoom *cr, LinphoneChatRoomCbs *cbs) {
	cr->callbacks = bctbx_list_remove(cr->callbacks, cbs);
	linphone_chat_room_cbs_unref(cbs);
}

LinphoneChatRoomCbs *linphone_chat_room_get_current_callbacks(const LinphoneChatRoom *cr) {
	return cr->currentCbs;
}

void linphone_chat_room_set_current_callbacks(LinphoneChatRoom *cr, LinphoneChatRoomCbs *cbs) {
	cr->currentCbs = cbs;
}

const bctbx_list_t *linphone_chat_room_get_callbacks_list(const LinphoneChatRoom *cr) {
	return cr->callbacks;
}

#define NOTIFY_IF_EXIST(cbName, functionName, ...)                                                                     \
	do {                                                                                                               \
		bctbx_list_t *callbacksCopy = bctbx_list_copy_with_data(linphone_chat_room_get_callbacks_list(cr),             \
		                                                        (bctbx_list_copy_func)belle_sip_object_ref);           \
		for (bctbx_list_t *it = callbacksCopy; it; it = bctbx_list_next(it)) {                                         \
			LinphoneChatRoomCbs *cbs = static_cast<LinphoneChatRoomCbs *>(bctbx_list_get_data(it));                    \
			linphone_chat_room_set_current_callbacks(cr, cbs);                                                         \
			LinphoneChatRoomCbs##cbName##Cb cb = linphone_chat_room_cbs_get_##functionName(cbs);                       \
			if (cb) cb(__VA_ARGS__);                                                                                   \
		}                                                                                                              \
		linphone_chat_room_set_current_callbacks(cr, nullptr);                                                         \
		bctbx_list_free_with_data(callbacksCopy, (bctbx_list_free_func)belle_sip_object_unref);                        \
	} while (0)

void _linphone_chat_room_notify_is_composing_received(LinphoneChatRoom *cr,
                                                      const LinphoneAddress *remoteAddr,
                                                      bool_t isComposing) {
	NOTIFY_IF_EXIST(IsComposingReceived, is_composing_received, cr, remoteAddr, isComposing);
}

void linphone_chat_room_notify_session_state_changed(LinphoneChatRoom *cr,
                                                     LinphoneCallState cstate,
                                                     const char *message) {
	NOTIFY_IF_EXIST(SessionStateChanged, session_state_changed, cr, cstate, message);
}

void _linphone_chat_room_notify_message_received(LinphoneChatRoom *cr, LinphoneChatMessage *msg) {
	NOTIFY_IF_EXIST(MessageReceived, message_received, cr, msg);
}

void _linphone_chat_room_notify_messages_received(LinphoneChatRoom *cr, const bctbx_list_t *chat_messages) {
	NOTIFY_IF_EXIST(MessagesReceived, messages_received, cr, chat_messages);
}

void _linphone_chat_room_notify_new_event(LinphoneChatRoom *cr, const LinphoneEventLog *event_log) {
	NOTIFY_IF_EXIST(NewEvent, new_event, cr, event_log);
}

void _linphone_chat_room_notify_new_events(LinphoneChatRoom *cr, const bctbx_list_t *event_logs) {
	NOTIFY_IF_EXIST(NewEvents, new_events, cr, event_logs);
}

void _linphone_chat_room_notify_participant_added(LinphoneChatRoom *cr, const LinphoneEventLog *event_log) {
	_linphone_chat_room_notify_new_event(cr, event_log);
	NOTIFY_IF_EXIST(ParticipantAdded, participant_added, cr, event_log);
}

void _linphone_chat_room_notify_participant_removed(LinphoneChatRoom *cr, const LinphoneEventLog *event_log) {
	_linphone_chat_room_notify_new_event(cr, event_log);
	NOTIFY_IF_EXIST(ParticipantRemoved, participant_removed, cr, event_log);
}

void _linphone_chat_room_notify_participant_device_added(LinphoneChatRoom *cr, const LinphoneEventLog *event_log) {
	_linphone_chat_room_notify_new_event(cr, event_log);
	NOTIFY_IF_EXIST(ParticipantDeviceAdded, participant_device_added, cr, event_log);
}

void _linphone_chat_room_notify_participant_device_removed(LinphoneChatRoom *cr, const LinphoneEventLog *event_log) {
	_linphone_chat_room_notify_new_event(cr, event_log);
	NOTIFY_IF_EXIST(ParticipantDeviceRemoved, participant_device_removed, cr, event_log);
}

void _linphone_chat_room_notify_participant_device_state_changed(LinphoneChatRoom *cr,
                                                                 const LinphoneEventLog *event_log,
                                                                 const LinphoneParticipantDeviceState state) {
	_linphone_chat_room_notify_new_event(cr, event_log);
	NOTIFY_IF_EXIST(ParticipantDeviceStateChanged, participant_device_state_changed, cr, event_log, state);
}

void _linphone_chat_room_notify_participant_device_media_availability_changed(LinphoneChatRoom *cr,
                                                                              const LinphoneEventLog *event_log) {
	_linphone_chat_room_notify_new_event(cr, event_log);
	NOTIFY_IF_EXIST(ParticipantDeviceMediaAvailabilityChanged, participant_device_media_availability_changed, cr,
	                event_log);
}

void _linphone_chat_room_notify_participant_admin_status_changed(LinphoneChatRoom *cr,
                                                                 const LinphoneEventLog *event_log) {
	_linphone_chat_room_notify_new_event(cr, event_log);
	NOTIFY_IF_EXIST(ParticipantAdminStatusChanged, participant_admin_status_changed, cr, event_log);
}

void _linphone_chat_room_notify_state_changed(LinphoneChatRoom *cr, LinphoneChatRoomState newState) {
	NOTIFY_IF_EXIST(StateChanged, state_changed, cr, newState);
}

void _linphone_chat_room_notify_security_event(LinphoneChatRoom *cr, const LinphoneEventLog *event_log) {
	_linphone_chat_room_notify_new_event(cr, event_log);
	NOTIFY_IF_EXIST(SecurityEvent, security_event, cr, event_log);
}

void _linphone_chat_room_notify_subject_changed(LinphoneChatRoom *cr, const LinphoneEventLog *event_log) {
	_linphone_chat_room_notify_new_event(cr, event_log);
	NOTIFY_IF_EXIST(SubjectChanged, subject_changed, cr, event_log);
}

void _linphone_chat_room_notify_conference_joined(LinphoneChatRoom *cr, const LinphoneEventLog *event_log) {
	_linphone_chat_room_notify_new_event(cr, event_log);
	NOTIFY_IF_EXIST(ConferenceJoined, conference_joined, cr, event_log);
}

void _linphone_chat_room_notify_conference_left(LinphoneChatRoom *cr, const LinphoneEventLog *event_log) {
	_linphone_chat_room_notify_new_event(cr, event_log);
	NOTIFY_IF_EXIST(ConferenceLeft, conference_left, cr, event_log);
}

void _linphone_chat_room_notify_ephemeral_event(LinphoneChatRoom *cr, const LinphoneEventLog *event_log) {
	_linphone_chat_room_notify_new_event(cr, event_log);
	NOTIFY_IF_EXIST(EphemeralEvent, ephemeral_event, cr, event_log);
}

void _linphone_chat_room_notify_ephemeral_message_timer_started(LinphoneChatRoom *cr,
                                                                const LinphoneEventLog *event_log) {
	_linphone_chat_room_notify_new_event(cr, event_log);
	NOTIFY_IF_EXIST(EphemeralMessageTimerStarted, ephemeral_message_timer_started, cr, event_log);
}

void _linphone_chat_room_notify_ephemeral_message_deleted(LinphoneChatRoom *cr, const LinphoneEventLog *event_log) {
	_linphone_chat_room_notify_new_event(cr, event_log);
	NOTIFY_IF_EXIST(EphemeralMessageDeleted, ephemeral_message_deleted, cr, event_log);
}

void _linphone_chat_room_notify_undecryptable_message_received(LinphoneChatRoom *cr, LinphoneChatMessage *msg) {
	NOTIFY_IF_EXIST(UndecryptableMessageReceived, undecryptable_message_received, cr, msg);
}

void _linphone_chat_room_notify_chat_message_received(LinphoneChatRoom *cr, const LinphoneEventLog *event_log) {
	_linphone_chat_room_notify_new_event(cr, event_log);
	NOTIFY_IF_EXIST(ChatMessageReceived, chat_message_received, cr, event_log);
}

void _linphone_chat_room_notify_chat_messages_received(LinphoneChatRoom *cr, const bctbx_list_t *event_logs) {
	_linphone_chat_room_notify_new_events(cr, event_logs);
	NOTIFY_IF_EXIST(ChatMessagesReceived, chat_messages_received, cr, event_logs);
}

void _linphone_chat_room_notify_chat_message_sending(LinphoneChatRoom *cr, const LinphoneEventLog *event_log) {
	_linphone_chat_room_notify_new_event(cr, event_log);
	NOTIFY_IF_EXIST(ChatMessageSending, chat_message_sending, cr, event_log);
}

void _linphone_chat_room_notify_chat_message_sent(LinphoneChatRoom *cr, const LinphoneEventLog *event_log) {
	_linphone_chat_room_notify_new_event(cr, event_log);
	NOTIFY_IF_EXIST(ChatMessageSent, chat_message_sent, cr, event_log);
}

void _linphone_chat_room_notify_conference_address_generation(LinphoneChatRoom *cr) {
	NOTIFY_IF_EXIST(ConferenceAddressGeneration, conference_address_generation, cr);
}

void _linphone_chat_room_notify_participant_registration_subscription_requested(
    LinphoneChatRoom *cr, const LinphoneAddress *participantAddr) {
	NOTIFY_IF_EXIST(ParticipantRegistrationSubscriptionRequested, participant_registration_subscription_requested, cr,
	                participantAddr);
}

void _linphone_chat_room_notify_participant_registration_unsubscription_requested(
    LinphoneChatRoom *cr, const LinphoneAddress *participantAddr) {
	NOTIFY_IF_EXIST(ParticipantRegistrationUnsubscriptionRequested, participant_registration_unsubscription_requested,
	                cr, participantAddr);
}

void _linphone_chat_room_notify_chat_message_should_be_stored(LinphoneChatRoom *cr, LinphoneChatMessage *msg) {
	NOTIFY_IF_EXIST(ShouldChatMessageBeStored, chat_message_should_be_stored, cr, msg);
}

void _linphone_chat_room_notify_chat_message_participant_imdn_state_changed(LinphoneChatRoom *cr,
                                                                            LinphoneChatMessage *msg,
                                                                            const LinphoneParticipantImdnState *state) {
	NOTIFY_IF_EXIST(ChatMessageParticipantImdnStateChanged, chat_message_participant_imdn_state_changed, cr, msg,
	                state);
}

// =============================================================================
// Reference and user data handling functions.
// =============================================================================

LinphoneChatRoom *linphone_chat_room_ref(LinphoneChatRoom *cr) {
	belle_sip_object_ref(cr);
	return cr;
}

void linphone_chat_room_unref(LinphoneChatRoom *cr) {
	belle_sip_object_unref(cr);
}

void *linphone_chat_room_get_user_data(const LinphoneChatRoom *cr) {
	return L_GET_USER_DATA_FROM_C_OBJECT(cr);
}

void linphone_chat_room_set_user_data(LinphoneChatRoom *cr, void *ud) {
	L_SET_USER_DATA_FROM_C_OBJECT(cr, ud);
}

// =============================================================================
// Constructor and destructor functions.
// =============================================================================

#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif // _MSC_VER
LinphoneChatRoom *_linphone_server_group_chat_room_new(LinphoneCore *core, LinphonePrivate::SalCallOp *op) {
#ifdef HAVE_ADVANCED_IM
	LinphoneChatRoom *cr = L_INIT(ChatRoom);
	L_SET_CPP_PTR_FROM_C_OBJECT(
	    cr, make_shared<LinphonePrivate::ServerGroupChatRoom>(L_GET_CPP_PTR_FROM_C_OBJECT(core), op));
	L_GET_CPP_PTR_FROM_C_OBJECT(cr)->setState(LinphonePrivate::ConferenceInterface::State::Instantiated);
	L_GET_PRIVATE_FROM_C_OBJECT(cr, ServerGroupChatRoom)->confirmCreation();
	return cr;
#else
	lWarning() << "Advanced IM such as group chat is disabled!";
	return NULL;
#endif
}
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif // _MSC_VER

// Convert chat room enum to conference state enum
LinphoneConferenceState linphone_chat_room_state_to_conference_state(LinphoneChatRoomState state) {
	// No default statement to trigger an error in case a new value is added to LinphoneChatRoomState
	switch (state) {
		case LinphoneChatRoomStateNone:
			return LinphoneConferenceStateNone;
		case LinphoneChatRoomStateInstantiated:
			return LinphoneConferenceStateInstantiated;
		case LinphoneChatRoomStateCreationPending:
			return LinphoneConferenceStateCreationPending;
		case LinphoneChatRoomStateCreated:
			return LinphoneConferenceStateCreated;
		case LinphoneChatRoomStateCreationFailed:
			return LinphoneConferenceStateCreationFailed;
		case LinphoneChatRoomStateTerminationPending:
			return LinphoneConferenceStateTerminationPending;
		case LinphoneChatRoomStateTerminated:
			return LinphoneConferenceStateTerminated;
		case LinphoneChatRoomStateTerminationFailed:
			return LinphoneConferenceStateTerminationFailed;
		case LinphoneChatRoomStateDeleted:
			return LinphoneConferenceStateDeleted;
	}
	return LinphoneConferenceStateNone;
}

// Convert conference state enum to chat room state enum
LinphoneChatRoomState linphone_conference_state_to_chat_room_state(LinphoneConferenceState state) {
	// No default statement to trigger an error in case a new value is added to LinphoneConferenceState
	switch (state) {
		case LinphoneConferenceStateNone:
			return LinphoneChatRoomStateNone;
		case LinphoneConferenceStateInstantiated:
			return LinphoneChatRoomStateInstantiated;
		case LinphoneConferenceStateCreationPending:
			return LinphoneChatRoomStateCreationPending;
		case LinphoneConferenceStateCreated:
			return LinphoneChatRoomStateCreated;
		case LinphoneConferenceStateCreationFailed:
			return LinphoneChatRoomStateCreationFailed;
		case LinphoneConferenceStateTerminationPending:
			return LinphoneChatRoomStateTerminationPending;
		case LinphoneConferenceStateTerminated:
			return LinphoneChatRoomStateTerminated;
		case LinphoneConferenceStateTerminationFailed:
			return LinphoneChatRoomStateTerminationFailed;
		case LinphoneConferenceStateDeleted:
			return LinphoneChatRoomStateDeleted;
	}
	return LinphoneChatRoomStateNone;
}

const char *linphone_chat_room_state_to_string(const LinphoneChatRoomState state) {
	switch (state) {
		case LinphoneChatRoomStateNone:
			return "LinphoneChatRoomStateNone";
		case LinphoneChatRoomStateInstantiated:
			return "LinphoneChatRoomStateInstantiated";
		case LinphoneChatRoomStateCreationPending:
			return "LinphoneChatRoomStateCreationPending";
		case LinphoneChatRoomStateCreated:
			return "LinphoneChatRoomStateCreated";
		case LinphoneChatRoomStateCreationFailed:
			return "LinphoneChatRoomStateCreationFailed";
		case LinphoneChatRoomStateTerminationPending:
			return "LinphoneChatRoomStateTerminationPending";
		case LinphoneChatRoomStateTerminated:
			return "LinphoneChatRoomStateTerminated";
		case LinphoneChatRoomStateTerminationFailed:
			return "LinphoneChatRoomStateTerminationFailed";
		case LinphoneChatRoomStateDeleted:
			return "LinphoneChatRoomStateDeleted";
	}
	return "Unknown state";
}
