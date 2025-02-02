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

#include "linphone/api/c-chat-room-cbs.h"

#include "c-wrapper/c-wrapper.h"

// =============================================================================

struct _LinphoneChatRoomCbs {
	belle_sip_object_t base;
	void *userData;
	LinphoneChatRoomCbsSessionStateChangedCb sessionStateChangedCb;
	LinphoneChatRoomCbsIsComposingReceivedCb isComposingReceivedCb;
	LinphoneChatRoomCbsMessageReceivedCb messageReceivedCb;
	LinphoneChatRoomCbsMessagesReceivedCb messagesReceivedCb;
	LinphoneChatRoomCbsParticipantAddedCb participantAddedCb;
	LinphoneChatRoomCbsParticipantRemovedCb participantRemovedCb;
	LinphoneChatRoomCbsParticipantDeviceAddedCb participantDeviceAddedCb;
	LinphoneChatRoomCbsParticipantDeviceRemovedCb participantDeviceRemovedCb;
	LinphoneChatRoomCbsParticipantDeviceMediaAvailabilityChangedCb participantDeviceMediaAvailabilityChangedCb;
	LinphoneChatRoomCbsParticipantDeviceStateChangedCb participantDeviceStateChangedCb;
	LinphoneChatRoomCbsParticipantAdminStatusChangedCb participantAdminStatusChangedCb;
	LinphoneChatRoomCbsStateChangedCb stateChangedCb;
	LinphoneChatRoomCbsSecurityEventCb securityEventCb;
	LinphoneChatRoomCbsSubjectChangedCb subjectChangedCb;
	LinphoneChatRoomCbsConferenceJoinedCb conferenceJoinedCb;
	LinphoneChatRoomCbsConferenceLeftCb conferenceLeftCb;
	LinphoneChatRoomCbsUndecryptableMessageReceivedCb undecryptableMessageReceivedCb;
	LinphoneChatRoomCbsChatMessageReceivedCb chatMessageReceivedCb;
	LinphoneChatRoomCbsChatMessagesReceivedCb chatMessagesReceivedCb;
	LinphoneChatRoomCbsChatMessageSendingCb chatMessageSendingCb;
	LinphoneChatRoomCbsChatMessageSentCb chatMessageSentCb;
	LinphoneChatRoomCbsConferenceAddressGenerationCb conferenceAddressGenerationCb;
	LinphoneChatRoomCbsParticipantRegistrationSubscriptionRequestedCb participantRegistrationSubscriptionRequestedCb;
	LinphoneChatRoomCbsParticipantRegistrationUnsubscriptionRequestedCb
	    participantRegistrationUnsubscriptionRequestedCb;
	LinphoneChatRoomCbsShouldChatMessageBeStoredCb shouldMessageBeStoredCb;
	LinphoneChatRoomCbsEphemeralEventCb ephemeralEventCb;
	LinphoneChatRoomCbsEphemeralMessageTimerStartedCb EphemeralMessageTimerStartedCb;
	LinphoneChatRoomCbsEphemeralMessageDeletedCb ephemeralMessageDeletedCb;
	LinphoneChatRoomCbsChatMessageParticipantImdnStateChangedCb chatMessageParticipantImdnStateChangedCb;
	LinphoneChatRoomCbsNewEventCb newEventCb;
	LinphoneChatRoomCbsNewEventsCb newEventsCb;
};

BELLE_SIP_DECLARE_VPTR_NO_EXPORT(LinphoneChatRoomCbs);

BELLE_SIP_DECLARE_NO_IMPLEMENTED_INTERFACES(LinphoneChatRoomCbs);

BELLE_SIP_INSTANCIATE_VPTR(LinphoneChatRoomCbs,
                           belle_sip_object_t,
                           NULL, // destroy
                           NULL, // clone
                           NULL, // marshal
                           FALSE);

// =============================================================================

LinphoneChatRoomCbs *_linphone_chat_room_cbs_new(void) {
	return belle_sip_object_new(LinphoneChatRoomCbs);
}

LinphoneChatRoomCbs *linphone_chat_room_cbs_ref(LinphoneChatRoomCbs *cbs) {
	belle_sip_object_ref(cbs);
	return cbs;
}

void linphone_chat_room_cbs_unref(LinphoneChatRoomCbs *cbs) {
	belle_sip_object_unref(cbs);
}

void *linphone_chat_room_cbs_get_user_data(const LinphoneChatRoomCbs *cbs) {
	return cbs->userData;
}

void linphone_chat_room_cbs_set_user_data(LinphoneChatRoomCbs *cbs, void *ud) {
	cbs->userData = ud;
}

void linphone_chat_room_cbs_set_session_state_changed(LinphoneChatRoomCbs *cbs,
                                                      LinphoneChatRoomCbsSessionStateChangedCb cb) {
	cbs->sessionStateChangedCb = cb;
}

LinphoneChatRoomCbsSessionStateChangedCb
linphone_chat_room_cbs_get_session_state_changed(const LinphoneChatRoomCbs *cbs) {
	return cbs->sessionStateChangedCb;
}

LinphoneChatRoomCbsIsComposingReceivedCb
linphone_chat_room_cbs_get_is_composing_received(const LinphoneChatRoomCbs *cbs) {
	return cbs->isComposingReceivedCb;
}

void linphone_chat_room_cbs_set_is_composing_received(LinphoneChatRoomCbs *cbs,
                                                      LinphoneChatRoomCbsIsComposingReceivedCb cb) {
	cbs->isComposingReceivedCb = cb;
}

LinphoneChatRoomCbsMessageReceivedCb linphone_chat_room_cbs_get_message_received(const LinphoneChatRoomCbs *cbs) {
	return cbs->messageReceivedCb;
}

void linphone_chat_room_cbs_set_message_received(LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsMessageReceivedCb cb) {
	cbs->messageReceivedCb = cb;
}

void linphone_chat_room_cbs_set_chat_messages_received(LinphoneChatRoomCbs *cbs,
                                                       LinphoneChatRoomCbsChatMessagesReceivedCb cb) {
	cbs->chatMessagesReceivedCb = cb;
}

LinphoneChatRoomCbsMessagesReceivedCb linphone_chat_room_cbs_get_messages_received(const LinphoneChatRoomCbs *cbs) {
	return cbs->messagesReceivedCb;
}

LinphoneChatRoomCbsNewEventCb linphone_chat_room_cbs_get_new_event(const LinphoneChatRoomCbs *cbs) {
	return cbs->newEventCb;
}

void linphone_chat_room_cbs_set_new_event(LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsNewEventCb cb) {
	cbs->newEventCb = cb;
}

LinphoneChatRoomCbsNewEventsCb linphone_chat_room_cbs_get_new_events(const LinphoneChatRoomCbs *cbs) {
	return cbs->newEventsCb;
}

void linphone_chat_room_cbs_set_new_events(LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsNewEventsCb cb) {
	cbs->newEventsCb = cb;
}

LinphoneChatRoomCbsChatMessageReceivedCb
linphone_chat_room_cbs_get_chat_message_received(const LinphoneChatRoomCbs *cbs) {
	return cbs->chatMessageReceivedCb;
}

void linphone_chat_room_cbs_set_chat_message_received(LinphoneChatRoomCbs *cbs,
                                                      LinphoneChatRoomCbsChatMessageReceivedCb cb) {
	cbs->chatMessageReceivedCb = cb;
}

LinphoneChatRoomCbsChatMessagesReceivedCb
linphone_chat_room_cbs_get_chat_messages_received(const LinphoneChatRoomCbs *cbs) {
	return cbs->chatMessagesReceivedCb;
}

void linphone_chat_room_cbs_set_messages_received(LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsMessagesReceivedCb cb) {
	cbs->messagesReceivedCb = cb;
}

LinphoneChatRoomCbsChatMessageSendingCb
linphone_chat_room_cbs_get_chat_message_sending(const LinphoneChatRoomCbs *cbs) {
	return cbs->chatMessageSendingCb;
}

void linphone_chat_room_cbs_set_chat_message_sending(LinphoneChatRoomCbs *cbs,
                                                     LinphoneChatRoomCbsChatMessageSendingCb cb) {
	cbs->chatMessageSendingCb = cb;
}

LinphoneChatRoomCbsChatMessageSentCb linphone_chat_room_cbs_get_chat_message_sent(const LinphoneChatRoomCbs *cbs) {
	return cbs->chatMessageSentCb;
}

void linphone_chat_room_cbs_set_chat_message_sent(LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsChatMessageSentCb cb) {
	cbs->chatMessageSentCb = cb;
}

LinphoneChatRoomCbsParticipantAddedCb linphone_chat_room_cbs_get_participant_added(const LinphoneChatRoomCbs *cbs) {
	return cbs->participantAddedCb;
}

void linphone_chat_room_cbs_set_participant_added(LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsParticipantAddedCb cb) {
	cbs->participantAddedCb = cb;
}

LinphoneChatRoomCbsParticipantRemovedCb linphone_chat_room_cbs_get_participant_removed(const LinphoneChatRoomCbs *cbs) {
	return cbs->participantRemovedCb;
}

void linphone_chat_room_cbs_set_participant_removed(LinphoneChatRoomCbs *cbs,
                                                    LinphoneChatRoomCbsParticipantRemovedCb cb) {
	cbs->participantRemovedCb = cb;
}

LinphoneChatRoomCbsParticipantAdminStatusChangedCb
linphone_chat_room_cbs_get_participant_admin_status_changed(const LinphoneChatRoomCbs *cbs) {
	return cbs->participantAdminStatusChangedCb;
}

void linphone_chat_room_cbs_set_participant_admin_status_changed(
    LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsParticipantAdminStatusChangedCb cb) {
	cbs->participantAdminStatusChangedCb = cb;
}

LinphoneChatRoomCbsStateChangedCb linphone_chat_room_cbs_get_state_changed(const LinphoneChatRoomCbs *cbs) {
	return cbs->stateChangedCb;
}

void linphone_chat_room_cbs_set_state_changed(LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsStateChangedCb cb) {
	cbs->stateChangedCb = cb;
}

LinphoneChatRoomCbsSecurityEventCb linphone_chat_room_cbs_get_security_event(const LinphoneChatRoomCbs *cbs) {
	return cbs->securityEventCb;
}

void linphone_chat_room_cbs_set_security_event(LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsSecurityEventCb cb) {
	cbs->securityEventCb = cb;
}

LinphoneChatRoomCbsSubjectChangedCb linphone_chat_room_cbs_get_subject_changed(const LinphoneChatRoomCbs *cbs) {
	return cbs->subjectChangedCb;
}

void linphone_chat_room_cbs_set_subject_changed(LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsSubjectChangedCb cb) {
	cbs->subjectChangedCb = cb;
}

LinphoneChatRoomCbsUndecryptableMessageReceivedCb
linphone_chat_room_cbs_get_undecryptable_message_received(const LinphoneChatRoomCbs *cbs) {
	return cbs->undecryptableMessageReceivedCb;
}

void linphone_chat_room_cbs_set_undecryptable_message_received(LinphoneChatRoomCbs *cbs,
                                                               LinphoneChatRoomCbsUndecryptableMessageReceivedCb cb) {
	cbs->undecryptableMessageReceivedCb = cb;
}

LinphoneChatRoomCbsParticipantDeviceAddedCb
linphone_chat_room_cbs_get_participant_device_added(const LinphoneChatRoomCbs *cbs) {
	return cbs->participantDeviceAddedCb;
}

void linphone_chat_room_cbs_set_participant_device_added(LinphoneChatRoomCbs *cbs,
                                                         LinphoneChatRoomCbsParticipantDeviceAddedCb cb) {
	cbs->participantDeviceAddedCb = cb;
}

LinphoneChatRoomCbsParticipantDeviceRemovedCb
linphone_chat_room_cbs_get_participant_device_removed(const LinphoneChatRoomCbs *cbs) {
	return cbs->participantDeviceRemovedCb;
}

void linphone_chat_room_cbs_set_participant_device_removed(LinphoneChatRoomCbs *cbs,
                                                           LinphoneChatRoomCbsParticipantDeviceRemovedCb cb) {
	cbs->participantDeviceRemovedCb = cb;
}

LinphoneChatRoomCbsParticipantDeviceMediaAvailabilityChangedCb
linphone_chat_room_cbs_get_participant_device_media_availability_changed(const LinphoneChatRoomCbs *cbs) {
	return cbs->participantDeviceMediaAvailabilityChangedCb;
}

void linphone_chat_room_cbs_set_participant_device_media_availability_changed(
    LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsParticipantDeviceMediaAvailabilityChangedCb cb) {
	cbs->participantDeviceMediaAvailabilityChangedCb = cb;
}

LinphoneChatRoomCbsParticipantDeviceStateChangedCb
linphone_chat_room_cbs_get_participant_device_state_changed(const LinphoneChatRoomCbs *cbs) {
	return cbs->participantDeviceStateChangedCb;
}

void linphone_chat_room_cbs_set_participant_device_state_changed(
    LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsParticipantDeviceStateChangedCb cb) {
	cbs->participantDeviceStateChangedCb = cb;
}

LinphoneChatRoomCbsConferenceJoinedCb linphone_chat_room_cbs_get_conference_joined(const LinphoneChatRoomCbs *cbs) {
	return cbs->conferenceJoinedCb;
}

void linphone_chat_room_cbs_set_conference_joined(LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsConferenceJoinedCb cb) {
	cbs->conferenceJoinedCb = cb;
}

LinphoneChatRoomCbsConferenceLeftCb linphone_chat_room_cbs_get_conference_left(const LinphoneChatRoomCbs *cbs) {
	return cbs->conferenceLeftCb;
}

void linphone_chat_room_cbs_set_conference_left(LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsConferenceLeftCb cb) {
	cbs->conferenceLeftCb = cb;
}

LinphoneChatRoomCbsEphemeralEventCb linphone_chat_room_cbs_get_ephemeral_event(const LinphoneChatRoomCbs *cbs) {
	return cbs->ephemeralEventCb;
}

void linphone_chat_room_cbs_set_ephemeral_event(LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsEphemeralEventCb cb) {
	cbs->ephemeralEventCb = cb;
}

LinphoneChatRoomCbsEphemeralMessageTimerStartedCb
linphone_chat_room_cbs_get_ephemeral_message_timer_started(const LinphoneChatRoomCbs *cbs) {
	return cbs->EphemeralMessageTimerStartedCb;
}

void linphone_chat_room_cbs_set_ephemeral_message_timer_started(LinphoneChatRoomCbs *cbs,
                                                                LinphoneChatRoomCbsEphemeralMessageTimerStartedCb cb) {
	cbs->EphemeralMessageTimerStartedCb = cb;
}

LinphoneChatRoomCbsEphemeralMessageDeletedCb
linphone_chat_room_cbs_get_ephemeral_message_deleted(const LinphoneChatRoomCbs *cbs) {
	return cbs->ephemeralMessageDeletedCb;
}

void linphone_chat_room_cbs_set_ephemeral_message_deleted(LinphoneChatRoomCbs *cbs,
                                                          LinphoneChatRoomCbsEphemeralMessageDeletedCb cb) {
	cbs->ephemeralMessageDeletedCb = cb;
}

LinphoneChatRoomCbsConferenceAddressGenerationCb
linphone_chat_room_cbs_get_conference_address_generation(const LinphoneChatRoomCbs *cbs) {
	return cbs->conferenceAddressGenerationCb;
}

void linphone_chat_room_cbs_set_conference_address_generation(LinphoneChatRoomCbs *cbs,
                                                              LinphoneChatRoomCbsConferenceAddressGenerationCb cb) {
	cbs->conferenceAddressGenerationCb = cb;
}

LinphoneChatRoomCbsParticipantRegistrationSubscriptionRequestedCb
linphone_chat_room_cbs_get_participant_registration_subscription_requested(const LinphoneChatRoomCbs *cbs) {
	return cbs->participantRegistrationSubscriptionRequestedCb;
}

void linphone_chat_room_cbs_set_participant_registration_subscription_requested(
    LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsParticipantRegistrationSubscriptionRequestedCb cb) {
	cbs->participantRegistrationSubscriptionRequestedCb = cb;
}

LinphoneChatRoomCbsParticipantRegistrationUnsubscriptionRequestedCb
linphone_chat_room_cbs_get_participant_registration_unsubscription_requested(const LinphoneChatRoomCbs *cbs) {
	return cbs->participantRegistrationUnsubscriptionRequestedCb;
}

void linphone_chat_room_cbs_set_participant_registration_unsubscription_requested(
    LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsParticipantRegistrationUnsubscriptionRequestedCb cb) {
	cbs->participantRegistrationUnsubscriptionRequestedCb = cb;
}

LinphoneChatRoomCbsShouldChatMessageBeStoredCb
linphone_chat_room_cbs_get_chat_message_should_be_stored(LinphoneChatRoomCbs *cbs) {
	return cbs->shouldMessageBeStoredCb;
}

void linphone_chat_room_cbs_set_chat_message_should_be_stored(LinphoneChatRoomCbs *cbs,
                                                              LinphoneChatRoomCbsShouldChatMessageBeStoredCb cb) {
	cbs->shouldMessageBeStoredCb = cb;
}

LinphoneChatRoomCbsChatMessageParticipantImdnStateChangedCb
linphone_chat_room_cbs_get_chat_message_participant_imdn_state_changed(LinphoneChatRoomCbs *cbs) {
	return cbs->chatMessageParticipantImdnStateChangedCb;
}

void linphone_chat_room_cbs_set_chat_message_participant_imdn_state_changed(
    LinphoneChatRoomCbs *cbs, LinphoneChatRoomCbsChatMessageParticipantImdnStateChangedCb cb) {
	cbs->chatMessageParticipantImdnStateChangedCb = cb;
}
