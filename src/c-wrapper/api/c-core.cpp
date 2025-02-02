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

#include "c-wrapper/c-wrapper.h"
#include "core/core.h"
#include "linphone/utils/utils.h"
#include "linphone/wrapper_utils.h"

#include "private_structs.h"

#include "../../ldap/ldap.h"
#include "call/audio-device/audio-device.h"
#include "chat/encryption/encryption-engine.h"
#include "chat/encryption/legacy-encryption-engine.h"
#include "linphone/api/c-types.h"
#include "push-notification-message/push-notification-message.h"

// =============================================================================

using namespace std;

using namespace LinphonePrivate;

static void _linphone_core_constructor(LinphoneCore *lc);
static void _linphone_core_destructor(LinphoneCore *lc);

L_DECLARE_C_OBJECT_IMPL_WITH_XTORS(
    Core, _linphone_core_constructor, _linphone_core_destructor, LINPHONE_CORE_STRUCT_FIELDS;

    struct Cache {
	    ~Cache() {
	    }

	    string lime_server_url;
    } mutable cache;)

static void _linphone_core_constructor(LinphoneCore *lc) {
	lc->state = LinphoneGlobalOff;
	new (&lc->cache) LinphoneCore::Cache();
}

static void _linphone_core_destructor(LinphoneCore *lc) {
	lc->cache.~Cache();
	_linphone_core_uninit(lc);
}

void linphone_core_set_im_encryption_engine(LinphoneCore *lc, LinphoneImEncryptionEngine *imee) {
	if (lc->im_encryption_engine) {
		linphone_im_encryption_engine_unref(lc->im_encryption_engine);
		lc->im_encryption_engine = NULL;
	}
	if (imee) {
		imee->lc = lc;
		lc->im_encryption_engine = linphone_im_encryption_engine_ref(imee);
		L_GET_CPP_PTR_FROM_C_OBJECT(lc)->setEncryptionEngine(
		    new LegacyEncryptionEngine(L_GET_CPP_PTR_FROM_C_OBJECT(lc)));
	}
}

void linphone_core_enable_lime_x3dh(LinphoneCore *lc, bool_t enable) {
	linphone_config_set_bool(linphone_core_get_config(lc), "lime", "enabled", enable);
	L_GET_CPP_PTR_FROM_C_OBJECT(lc)->enableLimeX3dh(enable ? true : false);
}

bool_t linphone_core_lime_x3dh_enabled(const LinphoneCore *lc) {
	bool isEnabled = L_GET_CPP_PTR_FROM_C_OBJECT(lc)->limeX3dhEnabled();
	return isEnabled ? TRUE : FALSE;
}

bool_t linphone_core_lime_x3dh_available(const LinphoneCore *lc) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(lc)->limeX3dhAvailable();
}

void linphone_core_set_lime_x3dh_server_url(LinphoneCore *lc, const char *url) {
	L_GET_CPP_PTR_FROM_C_OBJECT(lc)->setX3dhServerUrl(L_C_TO_STRING(url));
}

const char *linphone_core_get_lime_x3dh_server_url(LinphoneCore *lc) {
	lc->cache.lime_server_url = L_GET_CPP_PTR_FROM_C_OBJECT(lc)->getX3dhServerUrl();
	return L_STRING_TO_C(lc->cache.lime_server_url);
}

// Deprecated
const char *linphone_core_get_linphone_specs(const LinphoneCore *lc) {
	return linphone_config_get_string(linphone_core_get_config(lc), "sip", "linphone_specs", NULL);
}

// Deprecated
void linphone_core_set_linphone_specs(LinphoneCore *lc, const char *specs) {
	L_GET_CPP_PTR_FROM_C_OBJECT(lc)->setSpecs(Utils::cStringToCppString(specs));
}

void linphone_core_set_linphone_specs_list(LinphoneCore *lc, const bctbx_list_t *specs) {
	L_GET_CPP_PTR_FROM_C_OBJECT(lc)->setSpecs(L_GET_CPP_LIST_FROM_C_LIST(specs, const char *, string));
}

void linphone_core_add_linphone_spec(LinphoneCore *lc, const char *spec) {
	L_GET_CPP_PTR_FROM_C_OBJECT(lc)->addSpec(Utils::cStringToCppString(spec));
}

void linphone_core_remove_linphone_spec(LinphoneCore *lc, const char *spec) {
	L_GET_CPP_PTR_FROM_C_OBJECT(lc)->removeSpec(Utils::cStringToCppString(spec));
}

const bctbx_list_t *linphone_core_get_linphone_specs_list(LinphoneCore *lc) {
	return L_GET_C_LIST_FROM_CPP_LIST(L_GET_CPP_PTR_FROM_C_OBJECT(lc)->getSpecsList());
}

void linphone_core_enable_friend_list_subscription(LinphoneCore *lc, bool_t enable) {
	L_GET_CPP_PTR_FROM_C_OBJECT(lc)->enableFriendListSubscription(enable == TRUE ? true : false);
}

bool_t linphone_core_is_friend_list_subscription_enabled(LinphoneCore *lc) {
	return linphone_core_friend_list_subscription_enabled(lc);
}

bool_t linphone_core_friend_list_subscription_enabled(LinphoneCore *core) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(core)->isFriendListSubscriptionEnabled() ? TRUE : FALSE;
}

void linphone_core_ensure_registered(LinphoneCore *lc) {
	L_GET_CPP_PTR_FROM_C_OBJECT(lc)->pushNotificationReceived("", "", false);
}

void linphone_core_process_push_notification(LinphoneCore *lc, const char *call_id) {
	L_GET_CPP_PTR_FROM_C_OBJECT(lc)->pushNotificationReceived(call_id, "", false);
}

void linphone_core_push_notification_received(LinphoneCore *lc, const char *payload, const char *call_id) {
	L_GET_CPP_PTR_FROM_C_OBJECT(lc)->pushNotificationReceived(call_id, payload, false);
}

void linphone_core_push_notification_received_2(LinphoneCore *lc,
                                                const char *payload,
                                                const char *call_id,
                                                bool_t is_core_starting) {
	L_GET_CPP_PTR_FROM_C_OBJECT(lc)->pushNotificationReceived(call_id, payload, is_core_starting);
}

LinphonePushNotificationMessage *linphone_core_get_new_message_from_callid(LinphoneCore *lc, const char *call_id) {
	std::shared_ptr<PushNotificationMessage> cppMsg =
	    L_GET_CPP_PTR_FROM_C_OBJECT(lc)->getPushNotificationMessage(Utils::cStringToCppString(call_id));
	if (!cppMsg) return NULL;

	LinphonePushNotificationMessage *msg = (LinphonePushNotificationMessage *)cppMsg->toC();
	if (msg) {
		// We need to take a ref on the object because this function is called from outside linphone-sdk.
		belle_sip_object_ref(msg);
	}
	return msg;
}

/* Uses the chat_room_addr instead of the call_id like linphone_core_get_new_message_from_callid to get the chatroom.
Using the call_id to get the chat room require to add a new param to chat room objects where the conference address is
already here */
LinphoneChatRoom *linphone_core_get_new_chat_room_from_conf_addr(LinphoneCore *lc, const char *chat_room_addr) {
	std::shared_ptr<ChatRoom> cppChatRoom =
	    L_GET_CPP_PTR_FROM_C_OBJECT(lc)->getPushNotificationChatRoom(Utils::cStringToCppString(chat_room_addr));
	LinphoneChatRoom *chatRoom = L_GET_C_BACK_PTR(cppChatRoom);

	return chatRoom;
}

bctbx_list_t *linphone_core_get_audio_devices(const LinphoneCore *lc) {
	return LinphonePrivate::AudioDevice::getCListFromCppList(L_GET_CPP_PTR_FROM_C_OBJECT(lc)->getAudioDevices());
}

bctbx_list_t *linphone_core_get_extended_audio_devices(const LinphoneCore *lc) {
	return LinphonePrivate::AudioDevice::getCListFromCppList(
	    L_GET_CPP_PTR_FROM_C_OBJECT(lc)->getExtendedAudioDevices());
}

void linphone_core_set_input_audio_device(LinphoneCore *lc, LinphoneAudioDevice *audio_device) {
	L_GET_CPP_PTR_FROM_C_OBJECT(lc)->setInputAudioDevice(
	    (audio_device ? LinphonePrivate::AudioDevice::getSharedFromThis(audio_device) : NULL));
}

void linphone_core_set_output_audio_device(LinphoneCore *lc, LinphoneAudioDevice *audio_device) {
	L_GET_CPP_PTR_FROM_C_OBJECT(lc)->setOutputAudioDevice(
	    (audio_device ? LinphonePrivate::AudioDevice::getSharedFromThis(audio_device) : NULL));
}

const LinphoneAudioDevice *linphone_core_get_input_audio_device(const LinphoneCore *lc) {
	auto audioDevice = L_GET_CPP_PTR_FROM_C_OBJECT(lc)->getInputAudioDevice();
	if (audioDevice) {
		return audioDevice->toC();
	}
	return NULL;
}

const LinphoneAudioDevice *linphone_core_get_output_audio_device(const LinphoneCore *lc) {
	auto audioDevice = L_GET_CPP_PTR_FROM_C_OBJECT(lc)->getOutputAudioDevice();
	if (audioDevice) {
		return audioDevice->toC();
	}
	return NULL;
}

void linphone_core_set_default_input_audio_device(LinphoneCore *lc, LinphoneAudioDevice *audio_device) {
	if (audio_device) {
		L_GET_CPP_PTR_FROM_C_OBJECT(lc)->setDefaultInputAudioDevice(
		    LinphonePrivate::AudioDevice::getSharedFromThis(audio_device));
	}
}

void linphone_core_set_default_output_audio_device(LinphoneCore *lc, LinphoneAudioDevice *audio_device) {
	if (audio_device) {
		L_GET_CPP_PTR_FROM_C_OBJECT(lc)->setDefaultOutputAudioDevice(
		    LinphonePrivate::AudioDevice::getSharedFromThis(audio_device));
	}
}

const LinphoneAudioDevice *linphone_core_get_default_input_audio_device(const LinphoneCore *lc) {
	auto audioDevice = L_GET_CPP_PTR_FROM_C_OBJECT(lc)->getDefaultInputAudioDevice();
	if (audioDevice) {
		return audioDevice->toC();
	}
	return NULL;
}

const LinphoneAudioDevice *linphone_core_get_default_output_audio_device(const LinphoneCore *lc) {
	auto audioDevice = L_GET_CPP_PTR_FROM_C_OBJECT(lc)->getDefaultOutputAudioDevice();
	if (audioDevice) {
		return audioDevice->toC();
	}
	return NULL;
}

VideoStream *linphone_core_get_preview_stream(LinphoneCore *lc) {
	return lc->previewstream;
}

const char *linphone_core_get_conference_version(const LinphoneCore *lc) {
	return lc->conference_version;
}

const char *linphone_core_get_groupchat_version(const LinphoneCore *lc) {
	return lc->groupchat_version;
}

const char *linphone_core_get_ephemeral_version(const LinphoneCore *lc) {
	return lc->ephemeral_version;
}

bool_t linphone_core_is_in_background(const LinphoneCore *lc) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(lc)->isInBackground();
}

void linphone_core_enable_conference_ics_in_message_body(LinphoneCore *core, bool_t enable) {
	linphone_config_set_bool(linphone_core_get_config(core), "misc", "send_conference_ics_in_message_body", enable);
}

bool_t linphone_core_conference_ics_in_message_body_enabled(const LinphoneCore *core) {
	return linphone_config_get_bool(linphone_core_get_config(core), "misc", "send_conference_ics_in_message_body",
	                                TRUE);
}

LinphoneLdapParams *linphone_core_create_ldap_params(LinphoneCore *core) {
	return linphone_ldap_params_new(core);
}

LinphoneLdap *linphone_core_create_ldap(LinphoneCore *core) {
	return linphone_ldap_new(core);
}

LinphoneLdap *linphone_core_create_ldap_with_params(LinphoneCore *core, LinphoneLdapParams *params) {
	LinphoneLdap *ldap = linphone_ldap_new_with_params(core, params);
	linphone_core_add_ldap(core, ldap);
	return ldap;
}

void linphone_core_clear_ldaps(LinphoneCore *core) {
	auto list = L_GET_CPP_PTR_FROM_C_OBJECT(core)->getLdapList();
	for (auto ldap : list) {
		L_GET_CPP_PTR_FROM_C_OBJECT(core)->removeLdap(ldap);
	}
}

void linphone_core_add_ldap(LinphoneCore *core, LinphoneLdap *ldap) {
	L_GET_CPP_PTR_FROM_C_OBJECT(core)->addLdap(Ldap::toCpp(ldap)->getSharedFromThis());
}

void linphone_core_remove_ldap(LinphoneCore *core, LinphoneLdap *ldap) {
	L_GET_CPP_PTR_FROM_C_OBJECT(core)->removeLdap(Ldap::toCpp(ldap)->getSharedFromThis());
}

bctbx_list_t *linphone_core_get_ldap_list(LinphoneCore *lc) {
	return Ldap::getCListFromCppList(L_GET_CPP_PTR_FROM_C_OBJECT(lc)->getLdapList());
}

bool_t linphone_core_get_chat_messages_aggregation_enabled(LinphoneCore *core) {
	return linphone_config_get_bool(linphone_core_get_config(core), "sip", "chat_messages_aggregation", FALSE);
}

void linphone_core_set_chat_messages_aggregation_enabled(LinphoneCore *core, bool_t enabled) {
	linphone_config_set_bool(linphone_core_get_config(core), "sip", "chat_messages_aggregation", enabled);
}
