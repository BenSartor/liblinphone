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

#include "linphone/api/c-conference-info.h"
#include "c-wrapper/c-wrapper.h"
#include "c-wrapper/internal/c-tools.h"
#include "conference/conference-info.h"
#include "linphone/api/c-address.h"

// =============================================================================

using namespace LinphonePrivate;

LinphoneConferenceInfo *linphone_conference_info_new() {
	return ConferenceInfo::createCObject();
}

LinphoneConferenceInfo *linphone_conference_info_ref(LinphoneConferenceInfo *conference_info) {
	ConferenceInfo::toCpp(conference_info)->ref();
	return conference_info;
}

LinphoneConferenceInfo *linphone_conference_info_clone(const LinphoneConferenceInfo *info) {
	return static_cast<ConferenceInfo *>(ConferenceInfo::toCpp(info)->clone())->toC();
}

void linphone_conference_info_unref(LinphoneConferenceInfo *conference_info) {
	ConferenceInfo::toCpp(conference_info)->unref();
}

const LinphoneAddress *linphone_conference_info_get_organizer(const LinphoneConferenceInfo *conference_info) {
	const auto &address = ConferenceInfo::toCpp(conference_info)->getOrganizerAddress();
	return address && address->isValid() ? address->toC() : nullptr;
}

void linphone_conference_info_set_organizer(LinphoneConferenceInfo *conference_info, LinphoneAddress *organizer) {
	ConferenceInfo::toCpp(conference_info)->setOrganizer(Address::toCpp(organizer)->getSharedFromThis());
}

bctbx_list_t *linphone_conference_info_get_participants(const LinphoneConferenceInfo *conference_info) {
	const auto &participants = ConferenceInfo::toCpp(conference_info)->getParticipants();
	bctbx_list_t *participant_addresses = NULL;
	for (const auto &participant : participants) {
		const auto &address = participant.first;
		participant_addresses = bctbx_list_append(participant_addresses, address->toC());
	}
	return participant_addresses;
}

void linphone_conference_info_set_participants(LinphoneConferenceInfo *conference_info, bctbx_list_t *participants) {
	const std::list<std::shared_ptr<LinphonePrivate::Address>> participantsList =
	    LinphonePrivate::Utils::bctbxListToCppSharedPtrList<LinphoneAddress, LinphonePrivate::Address>(participants);
	ConferenceInfo::participant_list_t participantsMap;
	ConferenceInfo::participant_params_t participantsParams;
	for (const auto &p : participantsList) {
		participantsMap[p] = participantsParams;
	}
	ConferenceInfo::toCpp(conference_info)->setParticipants(participantsMap);
}

void linphone_conference_info_add_participant(LinphoneConferenceInfo *conference_info, LinphoneAddress *participant) {
	ConferenceInfo::toCpp(conference_info)->addParticipant(Address::toCpp(participant)->getSharedFromThis());
}

void linphone_conference_info_remove_participant(LinphoneConferenceInfo *conference_info,
                                                 LinphoneAddress *participant) {
	ConferenceInfo::toCpp(conference_info)->removeParticipant(Address::toCpp(participant)->getSharedFromThis());
}

const LinphoneAddress *linphone_conference_info_get_uri(const LinphoneConferenceInfo *conference_info) {
	const auto &address = ConferenceInfo::toCpp(conference_info)->getUri();
	return address && address->isValid() ? address->toC() : nullptr;
}

void linphone_conference_info_set_uri(LinphoneConferenceInfo *conference_info, LinphoneAddress *uri) {
	ConferenceInfo::toCpp(conference_info)->setUri(Address::toCpp(uri)->getSharedFromThis());
}

time_t linphone_conference_info_get_date_time(const LinphoneConferenceInfo *conference_info) {
	return ConferenceInfo::toCpp(conference_info)->getDateTime();
}

void linphone_conference_info_set_date_time(LinphoneConferenceInfo *conference_info, time_t datetime) {
	ConferenceInfo::toCpp(conference_info)->setDateTime(datetime);
}

unsigned int linphone_conference_info_get_duration(const LinphoneConferenceInfo *conference_info) {
	return ConferenceInfo::toCpp(conference_info)->getDuration();
}

void linphone_conference_info_set_duration(LinphoneConferenceInfo *conference_info, unsigned int duration) {
	ConferenceInfo::toCpp(conference_info)->setDuration(duration);
}

const char *linphone_conference_info_get_subject(const LinphoneConferenceInfo *conference_info) {
	return L_STRING_TO_C(ConferenceInfo::toCpp(conference_info)->getSubject());
}

void linphone_conference_info_set_subject(LinphoneConferenceInfo *conference_info, const char *subject) {
	ConferenceInfo::toCpp(conference_info)->setSubject(L_C_TO_STRING(subject));
}

unsigned int linphone_conference_info_get_ics_sequence(const LinphoneConferenceInfo *conference_info) {
	return ConferenceInfo::toCpp(conference_info)->getIcsSequence();
}

const char *linphone_conference_info_get_ics_uid(const LinphoneConferenceInfo *conference_info) {
	return L_STRING_TO_C(ConferenceInfo::toCpp(conference_info)->getIcsUid());
}

const char *linphone_conference_info_get_description(const LinphoneConferenceInfo *conference_info) {
	return L_STRING_TO_C(ConferenceInfo::toCpp(conference_info)->getDescription());
}

void linphone_conference_info_set_description(LinphoneConferenceInfo *conference_info, const char *description) {
	ConferenceInfo::toCpp(conference_info)->setDescription(L_C_TO_STRING(description));
}

char *linphone_conference_info_get_icalendar_string(const LinphoneConferenceInfo *conference_info) {
	std::string tmp = ConferenceInfo::toCpp(conference_info)->toIcsString();
	if (!tmp.empty()) {
		return bctbx_strdup(L_STRING_TO_C(tmp));
	}

	return NULL;
}

LinphoneConferenceInfoState linphone_conference_info_get_state(const LinphoneConferenceInfo *conference_info) {
	return (LinphoneConferenceInfoState)ConferenceInfo::toCpp(conference_info)->getState();
}
