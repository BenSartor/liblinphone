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

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <bctoolbox/defs.h>

#include "linphone/core.h"
#include "linphone/lpconfig.h"

#if !defined(_WIN32) && !defined(__ANDROID__) && !defined(__QNXNTO__)
#include <iconv.h>
#include <langinfo.h>
#include <string.h>
#endif // if !defined(_WIN32) && !defined(__ANDROID__) && !defined(__QNXNTO__)

#define MAX_PATH_SIZE 1024

#include "c-wrapper/c-wrapper.h"
#include "core/core-p.h"
#include "db/main-db.h"

// TODO: From coreapi. Remove me later.
#include "private.h"

using namespace std;

using namespace LinphonePrivate;

BELLE_SIP_DECLARE_NO_IMPLEMENTED_INTERFACES(LinphoneFriendCbs);

BELLE_SIP_INSTANCIATE_VPTR(LinphoneFriendCbs,
                           belle_sip_object_t,
                           NULL, // destroy
                           NULL, // clone
                           NULL, // Marshall
                           FALSE);

LinphoneFriendCbs *linphone_friend_cbs_new(void) {
	return belle_sip_object_new(LinphoneFriendCbs);
}

void linphone_friend_add_callbacks(LinphoneFriend *linphone_friend, LinphoneFriendCbs *cbs) {
	linphone_friend->callbacks = bctbx_list_append(linphone_friend->callbacks, linphone_friend_cbs_ref(cbs));
}

void linphone_friend_remove_callbacks(LinphoneFriend *linphone_friend, LinphoneFriendCbs *cbs) {
	linphone_friend->callbacks = bctbx_list_remove(linphone_friend->callbacks, cbs);
	linphone_friend_cbs_unref(cbs);
}

LinphoneFriendCbs *linphone_friend_get_current_callbacks(const LinphoneFriend *linphone_friend) {
	return linphone_friend->currentCbs;
}

void linphone_friend_set_current_callbacks(LinphoneFriend *linphone_friend, LinphoneFriendCbs *cbs) {
	linphone_friend->currentCbs = cbs;
}

const bctbx_list_t *linphone_friend_get_callbacks_list(const LinphoneFriend *linphone_friend) {
	return linphone_friend->callbacks;
}

#define NOTIFY_IF_EXIST(cbName, functionName, ...)                                                                     \
	bctbx_list_t *callbacksCopy = bctbx_list_copy(linphone_friend_get_callbacks_list(linphone_friend));                \
	for (bctbx_list_t *it = callbacksCopy; it; it = bctbx_list_next(it)) {                                             \
		linphone_friend_set_current_callbacks(linphone_friend,                                                         \
		                                      reinterpret_cast<LinphoneFriendCbs *>(bctbx_list_get_data(it)));         \
		LinphoneFriendCbs##cbName##Cb cb =                                                                             \
		    linphone_friend_cbs_get_##functionName(linphone_friend_get_current_callbacks(linphone_friend));            \
		if (cb) cb(__VA_ARGS__);                                                                                       \
	}                                                                                                                  \
	linphone_friend_set_current_callbacks(linphone_friend, nullptr);                                                   \
	bctbx_list_free(callbacksCopy);

LinphoneFriendCbs *linphone_friend_cbs_ref(LinphoneFriendCbs *cbs) {
	belle_sip_object_ref(cbs);
	return cbs;
}

void linphone_friend_cbs_unref(LinphoneFriendCbs *cbs) {
	belle_sip_object_unref(cbs);
}

void *linphone_friend_cbs_get_user_data(const LinphoneFriendCbs *cbs) {
	return cbs->user_data;
}

void linphone_friend_cbs_set_user_data(LinphoneFriendCbs *cbs, void *ud) {
	cbs->user_data = ud;
}

LinphoneFriendCbsPresenceReceivedCb linphone_friend_cbs_get_presence_received(const LinphoneFriendCbs *cbs) {
	return cbs->presence_received_cb;
}

void linphone_friend_cbs_set_presence_received(LinphoneFriendCbs *cbs, LinphoneFriendCbsPresenceReceivedCb cb) {
	cbs->presence_received_cb = cb;
}

void linphone_friend_notify_presence_received(LinphoneFriend *linphone_friend) {
	NOTIFY_IF_EXIST(PresenceReceived, presence_received, linphone_friend)
}

const char *linphone_online_status_to_string(LinphoneOnlineStatus ss) {
	const char *str = NULL;
	switch (ss) {
		case LinphoneStatusOnline:
			str = "Online";
			break;
		case LinphoneStatusBusy:
			str = "Busy";
			break;
		case LinphoneStatusBeRightBack:
			str = "Be right back";
			break;
		case LinphoneStatusAway:
			str = "Away";
			break;
		case LinphoneStatusOnThePhone:
			str = "On the phone";
			break;
		case LinphoneStatusOutToLunch:
			str = "Out to lunch";
			break;
		case LinphoneStatusDoNotDisturb:
			str = "Do not disturb";
			break;
		case LinphoneStatusMoved:
			str = "Moved";
			break;
		case LinphoneStatusAltService:
			str = "Using another messaging service";
			break;
		case LinphoneStatusOffline:
			str = "Offline";
			break;
		case LinphoneStatusPending:
			str = "Pending";
			break;
		case LinphoneStatusVacation:
			str = "Vacation";
			break;
		default:
			str = "Unknown status";
	}
	return str;
}

static int friend_compare(const void *a, const void *b) {
	LinphoneFriend *lfa = (LinphoneFriend *)a;
	LinphoneFriend *lfb = (LinphoneFriend *)b;
	const bctbx_list_t *addressesa = linphone_friend_get_addresses(lfa);
	const bctbx_list_t *addressesb = linphone_friend_get_addresses(lfb);
	bctbx_list_t *iteratora = (bctbx_list_t *)addressesa;
	bctbx_list_t *iteratorb = (bctbx_list_t *)addressesb;
	int ret = 1;

	while (iteratora && (ret == 1)) {
		LinphoneAddress *fa = (LinphoneAddress *)bctbx_list_get_data(iteratora);
		while (iteratorb && (ret == 1)) {
			LinphoneAddress *fb = (LinphoneAddress *)bctbx_list_get_data(iteratorb);
			if (linphone_address_weak_equal(fa, fb)) ret = 0;
			iteratorb = bctbx_list_next(iteratorb);
		}
		iteratora = bctbx_list_next(iteratora);
	}

	return ret;
}

static LinphoneFriendPresence *find_presence_model_for_uri_or_tel(const LinphoneFriend *lf, const char *uri_or_tel) {
	bctbx_list_t *iterator = NULL;
	LinphoneAddress *uri_or_tel_addr = NULL;
	LinphoneFriendPresence *result = NULL;
	if (!lf->lc) {
		ms_warning("Cannot find uri of tel [%s] from friend [%p] because not associated to any Linphone core object",
		           uri_or_tel, lf);
		return NULL;
	}
	if ((iterator = lf->presence_models) == NULL) {
		/*no need to move forward, just reutn to avoid useless uri parsing*/
		return NULL;
	};

	uri_or_tel_addr = linphone_core_interpret_url(lf->lc, uri_or_tel);

	while (uri_or_tel_addr && iterator) {
		LinphoneFriendPresence *lfp = (LinphoneFriendPresence *)bctbx_list_get_data(iterator);
		LinphoneAddress *lfp_addr = linphone_core_interpret_url(lf->lc, lfp->uri_or_tel);
		if (lfp_addr && linphone_address_weak_equal(uri_or_tel_addr, lfp_addr)) {
			result = lfp;
		}
		if (lfp_addr) linphone_address_unref(lfp_addr);
		if (result == NULL) iterator = bctbx_list_next(iterator);
		else break;
	}
	if (uri_or_tel_addr) linphone_address_unref(uri_or_tel_addr);
	return result;
}

static void
add_presence_model_for_uri_or_tel(LinphoneFriend *lf, const char *uri_or_tel, LinphonePresenceModel *presence) {
	LinphoneFriendPresence *lfp = ms_new0(LinphoneFriendPresence, 1);
	lfp->uri_or_tel = ms_strdup(uri_or_tel);
	if (presence) lfp->presence = linphone_presence_model_ref(presence);
	lf->presence_models = bctbx_list_append(lf->presence_models, lfp);
}

static void free_friend_presence(LinphoneFriendPresence *lfp) {
	ms_free(lfp->uri_or_tel);
	if (lfp->presence) linphone_presence_model_unref(lfp->presence);
	ms_free(lfp);
}

static void free_phone_number_sip_uri(LinphoneFriendPhoneNumberSipUri *lfpnsu) {
	ms_free(lfpnsu->number);
	ms_free(lfpnsu->uri);
	ms_free(lfpnsu);
}

bctbx_list_t *linphone_find_friend_by_address(bctbx_list_t *fl, const LinphoneAddress *addr, LinphoneFriend **lf) {
	bctbx_list_t *res = NULL;
	LinphoneFriend dummy;
	if (lf != NULL) *lf = NULL;
	memset(&dummy, 0, sizeof(LinphoneFriend));
	dummy.uri = (LinphoneAddress *)addr;
	res = bctbx_list_find_custom(fl, friend_compare, &dummy);
	if (lf != NULL && res != NULL) *lf = (LinphoneFriend *)bctbx_list_get_data(res);
	return res;
}

void __linphone_friend_do_subscribe(LinphoneFriend *fr) {
	LinphoneCore *lc = fr->lc;
	const LinphoneAddress *addr = linphone_friend_get_address(fr);

	if (addr != NULL) {
		if (fr->outsub == NULL) {
			/* people for which we don't have yet an answer should appear as offline */
			fr->presence_models =
			    bctbx_list_free_with_data(fr->presence_models, (bctbx_list_free_func)free_friend_presence);
			/*
			if (fr->lc->vtable.notify_recv)
			    fr->lc->vtable.notify_recv(fr->lc,(LinphoneFriend*)fr);
			*/
		} else {
			fr->outsub->release();
			fr->outsub = NULL;
		}
		fr->outsub = new SalPresenceOp(lc->sal.get());
		linphone_configure_op(lc, fr->outsub, addr, NULL, TRUE);
		fr->outsub->subscribe(linphone_config_get_int(lc->config, "sip", "subscribe_expires", 600));
		fr->subscribe_active = TRUE;
	} else {
		ms_error("Can't send a SUBSCRIBE for friend [%p] without an address!", fr);
	}
}

LinphoneFriend *linphone_friend_new(void) {
	LinphoneFriend *obj = belle_sip_object_new(LinphoneFriend);
	obj->pol = LinphoneSPAccept;
	obj->subscribe = TRUE;
	obj->vcard = NULL;
	obj->storage_id = 0;
	obj->rc_index = -1;
	obj->is_starred = FALSE;
	obj->native_uri = NULL;
	return obj;
}

#if __clang__ || ((__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __GNUC__ > 4)
#pragma GCC diagnostic push
#endif
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#else
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
LinphoneFriend *linphone_friend_new_with_address(const char *addr) {
	LinphoneAddress *linphone_address = linphone_address_new(addr);
	LinphoneFriend *fr;

	if (linphone_address == NULL) {
		ms_error("Cannot create friend for address [%s]", addr ? addr : "null");
		return NULL;
	}
	fr = linphone_friend_new();
	linphone_friend_set_address(fr, linphone_address);
	linphone_address_unref(linphone_address);
	return fr;
}
#if __clang__ || ((__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __GNUC__ > 4)
#pragma GCC diagnostic pop
#endif

void linphone_friend_set_user_data(LinphoneFriend *lf, void *data) {
	lf->user_data = data;
}

void *linphone_friend_get_user_data(const LinphoneFriend *lf) {
	return lf->user_data;
}

bool_t linphone_friend_in_list(const LinphoneFriend *lf) {
	return lf->friend_list != NULL;
}

void linphone_core_interpret_friend_uri(LinphoneCore *lc, const char *uri, char **result) {
	LinphoneAddress *fr = NULL;
	*result = NULL;
	fr = linphone_address_new(uri);
	if (fr == NULL) {
		char *tmp = NULL;
		if (strchr(uri, '@') != NULL) {
			LinphoneAddress *u;
			/*try adding sip:*/
			tmp = ms_strdup_printf("sip:%s", uri);
			u = linphone_address_new(tmp);
			if (u != NULL) {
				*result = tmp;
			}
		} else if (lc->default_proxy != NULL) {
			/*try adding domain part from default current proxy*/
			LinphoneAddress *id = linphone_address_new(linphone_core_get_identity(lc));
			if ((id != NULL) && (uri[0] != '\0')) {
				linphone_address_set_display_name(id, NULL);
				linphone_address_set_username(id, uri);
				*result = linphone_address_as_string(id);
				linphone_address_unref(id);
			}
		}
		if (*result) {
			/*looks good */
			ms_message("%s interpreted as %s", uri, *result);
		} else {
			ms_warning("Fail to interpret friend uri %s", uri);
		}
	} else {
		*result = linphone_address_as_string(fr);
		linphone_address_unref(fr);
	}
}

const LinphoneAddress *linphone_friend_get_address(const LinphoneFriend *lf) {
	if (linphone_core_vcard_supported()) {
		if (lf->vcard) {
			const bctbx_list_t *sip_addresses = linphone_vcard_get_sip_addresses(lf->vcard);
			if (sip_addresses) {
				LinphoneAddress *addr = (LinphoneAddress *)bctbx_list_nth_data(sip_addresses, 0);
				return addr;
			}
		}
		return NULL;
	}
	if (lf->uri) return lf->uri;
	return NULL;
}

static void add_friend_to_list_map_if_not_in_it_yet(LinphoneFriend *lf, const char *uri) {
	if (!lf || !lf->friend_list || !uri || strlen(uri) == 0) return;

	bctbx_iterator_t *it = bctbx_map_cchar_find_key(lf->friend_list->friends_map_uri, uri);
	bctbx_iterator_t *end = bctbx_map_cchar_end(lf->friend_list->friends_map_uri);
	bool_t found = FALSE;

	// Map is sorted, check if next entry matches key otherwise stop
	while (!found && !bctbx_iterator_cchar_equals(it, end)) {
		bctbx_pair_t *pair = bctbx_iterator_cchar_get_pair(it);
		const char *key = bctbx_pair_cchar_get_first(reinterpret_cast<bctbx_pair_cchar_t *>(pair));
		if (!key || strcmp(uri, key) != 0) break;
		LinphoneFriend *lf2 = (LinphoneFriend *)bctbx_pair_cchar_get_second(pair);
		if (lf2 == lf) {
			found = TRUE;
		}
		it = bctbx_iterator_cchar_get_next(it);
	}
	bctbx_iterator_cchar_delete(it);
	bctbx_iterator_cchar_delete(end);

	if (!found) {
		bctbx_pair_t *pair = (bctbx_pair_t *)bctbx_pair_cchar_new(uri, linphone_friend_ref(lf));
		bctbx_map_cchar_insert_and_delete(lf->friend_list->friends_map_uri, pair);
	}
}

static void remove_friend_from_list_map_if_already_in_it(LinphoneFriend *lf, const char *uri) {
	if (!lf || !lf->friend_list || !uri || strlen(uri) == 0) return;

	bctbx_iterator_t *it = bctbx_map_cchar_find_key(lf->friend_list->friends_map_uri, uri);
	bctbx_iterator_t *end = bctbx_map_cchar_end(lf->friend_list->friends_map_uri);

	// Map is sorted, check if next entry matches key otherwise stop
	while (!bctbx_iterator_cchar_equals(it, end)) {
		bctbx_pair_t *pair = bctbx_iterator_cchar_get_pair(it);
		const char *key = bctbx_pair_cchar_get_first(reinterpret_cast<bctbx_pair_cchar_t *>(pair));
		if (!key || strcmp(uri, key) != 0) break;
		LinphoneFriend *lf2 = (LinphoneFriend *)bctbx_pair_cchar_get_second(pair);
		if (lf2 == lf) {
			linphone_friend_unref(lf2);
			bctbx_map_cchar_erase(lf->friend_list->friends_map_uri, it);
			break;
		}
		it = bctbx_iterator_cchar_get_next(it);
	}
	bctbx_iterator_cchar_delete(it);
	bctbx_iterator_cchar_delete(end);
}

LinphoneStatus linphone_friend_set_address(LinphoneFriend *lf, const LinphoneAddress *addr) {
	if (!addr) return -1;
	LinphoneAddress *fr = linphone_address_clone(addr);
	char *address;
	const LinphoneAddress *mAddr = linphone_friend_get_address(lf);
	if (mAddr && lf->friend_list) {
		char *mainAddress = linphone_address_as_string_uri_only(mAddr);
		remove_friend_from_list_map_if_already_in_it(lf, mainAddress);
		ms_free(mainAddress);
	}
	linphone_address_clean(fr);

	address = linphone_address_as_string_uri_only(fr);
	if (lf->friend_list) {
		add_friend_to_list_map_if_not_in_it_yet(lf, address);
	}

	if (linphone_core_vcard_supported()) {
		if (!lf->vcard) {
			const char *dpname = linphone_address_get_display_name(fr) ? linphone_address_get_display_name(fr)
			                                                           : linphone_address_get_username(fr);
			linphone_friend_create_vcard(lf, dpname);
		}
		linphone_vcard_edit_main_sip_address(lf->vcard, address);
		linphone_address_unref(fr);
	} else {
		if (lf->uri != NULL) linphone_address_unref(lf->uri);
		lf->uri = fr;
	}

	ms_free(address);
	return 0;
}

void linphone_friend_add_address(LinphoneFriend *lf, const LinphoneAddress *addr) {
	LinphoneAddress *fr;
	char *uri;
	if (!lf || !addr) return;

	fr = linphone_address_clone(addr);
	linphone_address_clean(fr);

	uri = linphone_address_as_string_uri_only(fr);
	if (lf->friend_list) {
		add_friend_to_list_map_if_not_in_it_yet(lf, uri);
	}

	if (linphone_core_vcard_supported()) {
		if (lf->vcard) {
			linphone_vcard_add_sip_address(lf->vcard, uri);
			linphone_address_unref(fr);
		}
	} else {
		if (lf->uri == NULL) lf->uri = fr;
		else linphone_address_unref(fr);
	}
	ms_free(uri);
}

const bctbx_list_t *linphone_friend_get_addresses(const LinphoneFriend *lf) {
	if (!lf) return NULL;

	if (linphone_core_vcard_supported()) {
		const bctbx_list_t *addresses = linphone_vcard_get_sip_addresses(lf->vcard);
		return addresses;
	} else {
		bctbx_list_t *addresses = NULL;
		return lf->uri ? bctbx_list_append(addresses, lf->uri) : NULL;
	}
}

void linphone_friend_remove_address(LinphoneFriend *lf, const LinphoneAddress *addr) {
	char *address;
	if (!lf || !addr || !lf->vcard) return;

	address = linphone_address_as_string_uri_only(addr);
	if (lf->friend_list) {
		remove_friend_from_list_map_if_already_in_it(lf, address);
	}

	if (linphone_core_vcard_supported()) {
		linphone_vcard_remove_sip_address(lf->vcard, address);
	}
	ms_free(address);
}

void linphone_friend_add_phone_number(LinphoneFriend *lf, const char *phone) {
	if (!lf || !phone) return;

	if (lf->friend_list) {
		const char *uri = linphone_friend_phone_number_to_sip_uri(lf, phone);
		add_friend_to_list_map_if_not_in_it_yet(lf, uri);
	}

	if (linphone_core_vcard_supported()) {
		if (!lf->vcard) {
			linphone_friend_create_vcard(lf, phone);
		}
		linphone_vcard_add_phone_number(lf->vcard, phone);
	}
}

void linphone_friend_add_phone_number_with_label(LinphoneFriend *lf, LinphoneFriendPhoneNumber *phoneNumber) {
	if (!lf || !phoneNumber) return;

	const char *phone = linphone_friend_phone_number_get_phone_number(phoneNumber);
	if (!phone) return;

	if (lf->friend_list) {
		const char *uri = linphone_friend_phone_number_to_sip_uri(lf, phone);
		add_friend_to_list_map_if_not_in_it_yet(lf, uri);
	}

	if (linphone_core_vcard_supported()) {
		if (!lf->vcard) {
			linphone_friend_create_vcard(lf, phone);
		}
		linphone_vcard_add_phone_number_with_label(lf->vcard, phoneNumber);
	}
}

bctbx_list_t *linphone_friend_get_phone_numbers(const LinphoneFriend *lf) {
	if (!lf || !lf->vcard) return NULL;

	if (linphone_core_vcard_supported()) {
		return linphone_vcard_get_phone_numbers(lf->vcard);
	}
	return NULL;
}

bctbx_list_t *linphone_friend_get_phone_numbers_with_label(const LinphoneFriend *lf) {
	if (!lf || !lf->vcard) return NULL;

	if (linphone_core_vcard_supported()) {
		return linphone_vcard_get_phone_numbers_with_label(lf->vcard);
	}
	return NULL;
}

bool_t linphone_friend_has_phone_number(const LinphoneFriend *lf, const char *phoneNumber) {
	if (!lf || !phoneNumber) return FALSE;

	LinphoneAccount *account = linphone_core_get_default_account(lf->lc);
	// Account can be null, both linphone_account_is_phone_number and linphone_account_normalize_phone_number can handle
	// it
	if (phoneNumber == NULL || !linphone_account_is_phone_number(account, phoneNumber)) {
		ms_warning("Phone number [%s] isn't valid", phoneNumber);
		return FALSE;
	}

	bool_t found = FALSE;
	const bctbx_list_t *elem;
	const bctbx_list_t *accounts = linphone_core_get_account_list(lf->lc);
	for (elem = accounts; elem != NULL; elem = bctbx_list_next(elem)) {
		account = (LinphoneAccount *)bctbx_list_get_data(elem);

		char *normalized_phone_number = linphone_account_normalize_phone_number(account, phoneNumber);
		if (linphone_core_vcard_supported()) {
			bctbx_list_t *numbers = linphone_friend_get_phone_numbers(lf);
			bctbx_list_t *it = NULL;
			for (it = numbers; it != NULL; it = bctbx_list_next(it)) {
				const char *value = (const char *)bctbx_list_get_data(it);
				char *normalized_value = linphone_account_normalize_phone_number(account, value);
				if (normalized_value && strcmp(normalized_value, normalized_phone_number) == 0) {
					found = TRUE;
					ms_free(normalized_value);
					break;
				}
				if (normalized_value) ms_free(normalized_value);
			}
			bctbx_list_free(numbers);
		}
		if (normalized_phone_number) ms_free(normalized_phone_number);

		if (found) break;
	}

	return found;
}

void linphone_friend_remove_phone_number(LinphoneFriend *lf, const char *phone) {
	if (!lf || !phone || !lf->vcard) return;

	if (lf->friend_list) {
		const char *uri = linphone_friend_phone_number_to_sip_uri(lf, phone);
		if (uri) {
			remove_friend_from_list_map_if_already_in_it(lf, uri);
		}
	}

	if (linphone_core_vcard_supported()) {
		linphone_vcard_remove_phone_number(lf->vcard, phone);
	}
}

void linphone_friend_remove_phone_number_with_label(LinphoneFriend *lf, const LinphoneFriendPhoneNumber *phoneNumber) {
	if (!lf || !phoneNumber || !lf->vcard) return;

	const char *phone = linphone_friend_phone_number_get_phone_number(phoneNumber);
	if (!phone) return;

	if (lf->friend_list) {
		const char *uri = linphone_friend_phone_number_to_sip_uri(lf, phone);
		if (uri) {
			remove_friend_from_list_map_if_already_in_it(lf, uri);
		}
	}

	if (linphone_core_vcard_supported()) {
		linphone_vcard_remove_phone_number_with_label(lf->vcard, phoneNumber);
	}
}

LinphoneStatus linphone_friend_set_name(LinphoneFriend *lf, const char *name) {
	if (linphone_core_vcard_supported()) {
		if (!lf->vcard) linphone_friend_create_vcard(lf, name);
		linphone_vcard_set_full_name(lf->vcard, name);
	} else {
		if (!lf->uri) {
			ms_warning("linphone_friend_set_address() must be called before linphone_friend_set_name() to be able to "
			           "set display name.");
			return -1;
		}
		linphone_address_set_display_name(lf->uri, name);
	}
	return 0;
}

LinphoneStatus linphone_friend_enable_subscribes(LinphoneFriend *fr, bool_t val) {
	fr->subscribe = val;
	return 0;
}

LinphoneStatus linphone_friend_set_inc_subscribe_policy(LinphoneFriend *fr, LinphoneSubscribePolicy pol) {
	fr->pol = pol;
	return 0;
}

void linphone_friend_notify(LinphoneFriend *lf, LinphonePresenceModel *presence) {
	bctbx_list_t *elem;
	if (lf->insubs) {
		const LinphoneAddress *addr = linphone_friend_get_address(lf);
		if (addr) {
			char *addr_str = linphone_address_as_string(addr);
			ms_message("Want to notify %s", addr_str);
			ms_free(addr_str);
		}
	}
	for (elem = lf->insubs; elem != NULL; elem = bctbx_list_next(elem)) {
		auto op = static_cast<SalPresenceOp *>(bctbx_list_get_data(elem));
		op->notifyPresence((SalPresenceModel *)presence);
	}
}

void linphone_friend_add_incoming_subscription(LinphoneFriend *lf, SalOp *op) {
	/*ownership of the op is transfered from sal to the LinphoneFriend*/
	lf->insubs = bctbx_list_append(lf->insubs, op);
}

void linphone_friend_remove_incoming_subscription(LinphoneFriend *lf, SalOp *op) {
	if (bctbx_list_find(lf->insubs, op)) {
		op->release();
		lf->insubs = bctbx_list_remove(lf->insubs, op);
	}
}

static void linphone_friend_unsubscribe(LinphoneFriend *lf) {
	if (lf->outsub != NULL) {
		lf->outsub->unsubscribe();
	}
	/* for friend list there is no necessary outsub*/
	lf->subscribe_active = FALSE;
}

void linphone_friend_invalidate_subscription(LinphoneFriend *lf) {
	bctbx_list_t *iterator;
	LinphoneCore *lc = lf->lc;

	if (lf->outsub != NULL) {
		lf->outsub->release();
		lf->outsub = NULL;
	}

	// To resend a subscribe on the next network_reachable(TRUE)
	lf->subscribe_active = FALSE;

	/* Notify application that we no longer know the presence activity */
	iterator = lf->presence_models;
	while (iterator) {
		LinphoneFriendPresence *lfp = (LinphoneFriendPresence *)bctbx_list_get_data(iterator);
		linphone_presence_model_unref(lfp->presence);
		lfp->presence = linphone_presence_model_new();
		linphone_presence_model_set_basic_status(lfp->presence, LinphonePresenceBasicStatusClosed);
		linphone_core_notify_notify_presence_received_for_uri_or_tel(lc, lf, lfp->uri_or_tel, lfp->presence);
		iterator = bctbx_list_next(iterator);
	}
	if (bctbx_list_size(lf->presence_models) > 0) {
		// Deprecated
		linphone_core_notify_notify_presence_received(lc, lf);
	}
	lf->initial_subscribes_sent = FALSE;
}

static void close_presence_notification(SalPresenceOp *op) {
	op->notifyPresenceClose();
}

static void release_sal_op(SalOp *op) {
	op->release();
}

static void linphone_friend_close_incoming_subscriptions(LinphoneFriend *lf) {
	bctbx_list_for_each(lf->insubs, (MSIterateFunc)close_presence_notification);
	lf->insubs = bctbx_list_free_with_data(lf->insubs, (MSIterateFunc)release_sal_op);
}

void linphone_friend_close_subscriptions(LinphoneFriend *lf) {
	linphone_friend_unsubscribe(lf);
	linphone_friend_close_incoming_subscriptions(lf);
}

static void _linphone_friend_release_ops(LinphoneFriend *lf) {
	lf->insubs = bctbx_list_free_with_data(lf->insubs, (MSIterateFunc)release_sal_op);
	if (lf->outsub) {
		lf->outsub->release();
		lf->outsub = NULL;
	}
}

static void _linphone_friend_destroy(LinphoneFriend *lf) {
	_linphone_friend_release_ops(lf);
	if (lf->presence_models) bctbx_list_free_with_data(lf->presence_models, (bctbx_list_free_func)free_friend_presence);
	if (lf->phone_number_sip_uri_map)
		bctbx_list_free_with_data(lf->phone_number_sip_uri_map, (bctbx_list_free_func)free_phone_number_sip_uri);
	if (lf->uri != NULL) linphone_address_unref(lf->uri);
	if (lf->info != NULL) buddy_info_free(lf->info);
	if (lf->vcard != NULL) linphone_vcard_unref(lf->vcard);
	if (lf->refkey != NULL) ms_free(lf->refkey);
	if (lf->native_uri != NULL) ms_free(lf->native_uri);

	bctbx_list_free_with_data(lf->callbacks, (bctbx_list_free_func)linphone_friend_cbs_unref);
	lf->callbacks = nullptr;
}

static belle_sip_error_code
_linphone_friend_marshall(belle_sip_object_t *obj, char *buff, size_t buff_size, size_t *offset) {
	LinphoneFriend *lf = (LinphoneFriend *)obj;
	belle_sip_error_code err = BELLE_SIP_OK;
	if (lf->uri) {
		char *tmp = linphone_address_as_string(lf->uri);
		err = belle_sip_snprintf(buff, buff_size, offset, "%s", tmp);
		ms_free(tmp);
	}
	return err;
}

const char *linphone_friend_get_name(const LinphoneFriend *lf) {
	if (!lf) return NULL;

	const char *fullname = NULL;
	if (linphone_core_vcard_supported()) {
		if (lf->vcard) {
			fullname = linphone_vcard_get_full_name(lf->vcard);
		}
	}
	if (!fullname && lf->uri) {
		fullname = linphone_address_get_display_name(lf->uri);
	}
	return fullname;
}

bool_t linphone_friend_get_send_subscribe(const LinphoneFriend *lf) {
	return lf->subscribe;
}

LinphoneSubscribePolicy linphone_friend_get_inc_subscribe_policy(const LinphoneFriend *lf) {
	return lf->pol;
}

LinphoneOnlineStatus linphone_friend_get_status(const LinphoneFriend *lf) {
	const LinphonePresenceModel *presence = linphone_friend_get_presence_model(lf);
	LinphoneOnlineStatus online_status = LinphoneStatusOffline;
	LinphonePresenceBasicStatus basic_status = LinphonePresenceBasicStatusClosed;
	LinphonePresenceActivity *activity = NULL;
	const char *description = NULL;
	unsigned int nb_activities = 0;

	if (presence != NULL) {
		basic_status = linphone_presence_model_get_basic_status(presence);
		nb_activities = linphone_presence_model_get_nb_activities(presence);
		online_status =
		    (basic_status == LinphonePresenceBasicStatusOpen) ? LinphoneStatusOnline : LinphoneStatusOffline;
		if (nb_activities > 1) {
			char *tmp = NULL;
			const LinphoneAddress *addr = linphone_friend_get_address(lf);
			if (addr) tmp = linphone_address_as_string(addr);
			ms_warning("Friend %s has several activities, get status from the first one", tmp ? tmp : "unknown");
			if (tmp) {
				ms_free(tmp);
			}
			nb_activities = 1;
		}
		if (nb_activities == 1) {
			activity = linphone_presence_model_get_activity(presence);
			description = linphone_presence_activity_get_description(activity);
			switch (linphone_presence_activity_get_type(activity)) {
				case LinphonePresenceActivityBreakfast:
				case LinphonePresenceActivityDinner:
				case LinphonePresenceActivityLunch:
				case LinphonePresenceActivityMeal:
					online_status = LinphoneStatusOutToLunch;
					break;
				case LinphonePresenceActivityAppointment:
				case LinphonePresenceActivityMeeting:
				case LinphonePresenceActivityPerformance:
				case LinphonePresenceActivityPresentation:
				case LinphonePresenceActivitySpectator:
				case LinphonePresenceActivityWorking:
				case LinphonePresenceActivityWorship:
					online_status = LinphoneStatusDoNotDisturb;
					break;
				case LinphonePresenceActivityAway:
				case LinphonePresenceActivitySleeping:
					online_status = LinphoneStatusAway;
					break;
				case LinphonePresenceActivityHoliday:
				case LinphonePresenceActivityTravel:
				case LinphonePresenceActivityVacation:
					online_status = LinphoneStatusVacation;
					break;
				case LinphonePresenceActivityBusy:
					if (description && strcmp(description, "Do not disturb") ==
					                       0) { // See linphonecore.c linphone_core_set_presence_info() method
						online_status = LinphoneStatusDoNotDisturb;
					} else {
						online_status = LinphoneStatusBusy;
					}
					break;
				case LinphonePresenceActivityLookingForWork:
				case LinphonePresenceActivityPlaying:
				case LinphonePresenceActivityShopping:
				case LinphonePresenceActivityTV:
					online_status = LinphoneStatusBusy;
					break;
				case LinphonePresenceActivityInTransit:
				case LinphonePresenceActivitySteering:
					online_status = LinphoneStatusBeRightBack;
					break;
				case LinphonePresenceActivityOnThePhone:
					online_status = LinphoneStatusOnThePhone;
					break;
				case LinphonePresenceActivityOther:
				case LinphonePresenceActivityPermanentAbsence:
					online_status = LinphoneStatusMoved;
					break;
				case LinphonePresenceActivityUnknown:
					/* Rely on the basic status information. */
					break;
			}
		}
	}

	return online_status;
}

const LinphonePresenceModel *linphone_friend_get_presence_model(const LinphoneFriend *lf) {
	const LinphonePresenceModel *presence = NULL;
	LinphoneFriend *const_lf = (LinphoneFriend *)lf;
	const bctbx_list_t *addrs = linphone_friend_get_addresses(const_lf);
	bctbx_list_t *phones = NULL;
	bctbx_list_t *it;

	for (it = (bctbx_list_t *)addrs; it != NULL; it = it->next) {
		LinphoneAddress *addr = (LinphoneAddress *)it->data;
		char *uri = linphone_address_as_string_uri_only(addr);
		presence = linphone_friend_get_presence_model_for_uri_or_tel(const_lf, uri);
		ms_free(uri);
		if (presence) break;
	}
	if (presence) return presence;

	phones = linphone_friend_get_phone_numbers(const_lf);
	for (it = phones; it != NULL; it = it->next) {
		presence = linphone_friend_get_presence_model_for_uri_or_tel(const_lf, static_cast<const char *>(it->data));
		if (presence) break;
	}
	bctbx_list_free(phones);
	return presence;
}

LinphoneConsolidatedPresence linphone_friend_get_consolidated_presence(const LinphoneFriend *lf) {
	const LinphonePresenceModel *model = linphone_friend_get_presence_model(lf);
	if (!model) return LinphoneConsolidatedPresenceOffline;
	return linphone_presence_model_get_consolidated_presence(model);
}

const LinphonePresenceModel *linphone_friend_get_presence_model_for_uri_or_tel(const LinphoneFriend *lf,
                                                                               const char *uri_or_tel) {
	LinphoneFriendPresence *lfp = find_presence_model_for_uri_or_tel(lf, uri_or_tel);
	if (lfp) return lfp->presence;
	return NULL;
}

void linphone_friend_set_presence_model(LinphoneFriend *lf, LinphonePresenceModel *presence) {
	const LinphoneAddress *addr = linphone_friend_get_address(lf);
	if (addr) {
		char *uri = linphone_address_as_string_uri_only(addr);
		linphone_friend_set_presence_model_for_uri_or_tel(lf, uri, presence);
		ms_free(uri);
	}
}

void linphone_friend_set_presence_model_for_uri_or_tel(LinphoneFriend *lf,
                                                       const char *uri_or_tel,
                                                       LinphonePresenceModel *presence) {
	LinphoneFriendPresence *lfp = find_presence_model_for_uri_or_tel(lf, uri_or_tel);
	if (lfp) {
		if (lfp->presence) {
			linphone_presence_model_unref(lfp->presence);
			lfp->presence = NULL;
		}
		if (presence) lfp->presence = linphone_presence_model_ref(presence);
	} else {
		add_presence_model_for_uri_or_tel(lf, uri_or_tel, presence);
	}
}

bool_t linphone_friend_is_presence_received(const LinphoneFriend *lf) {
	return lf->presence_received;
}

BuddyInfo *linphone_friend_get_info(const LinphoneFriend *lf) {
	return lf->info;
}

/*
 * updates the p2p subscriptions.
 * If only_when_registered is TRUE, subscribe will be sent only if the friend's corresponding proxy config is in
 *registered. Otherwise if the proxy config goes to unregistered state, the subscription refresh will be suspended. An
 *optional proxy whose state has changed can be passed to optimize the processing.
 **/
void linphone_friend_update_subscribes(LinphoneFriend *fr, bool_t only_when_registered) {
	int can_subscribe = 1;

	if (only_when_registered && (fr->subscribe || fr->subscribe_active)) {
		const LinphoneAddress *addr = linphone_friend_get_address(fr);
		if (addr != NULL) {
			LinphoneProxyConfig *cfg = linphone_core_lookup_known_proxy(fr->lc, addr);
			if (cfg && linphone_proxy_config_get_state(cfg) != LinphoneRegistrationOk) {
				char *tmp = linphone_address_as_string(addr);
				ms_debug("Friend [%s] belongs to proxy config with identity [%s], but this one isn't registered. "
				         "Subscription is suspended.",
				         tmp, linphone_proxy_config_get_identity(cfg));
				ms_free(tmp);
				can_subscribe = 0;
			}
		}
	}
	if (can_subscribe && fr->subscribe && fr->subscribe_active == FALSE) {
		ms_message("Sending a new SUBSCRIBE for friend [%p]", fr);
		__linphone_friend_do_subscribe(fr);
	} else if (can_subscribe && fr->subscribe_active && !fr->subscribe) {
		linphone_friend_unsubscribe(fr);
	} else if (!can_subscribe && fr->outsub) {
		fr->subscribe_active = FALSE;
		fr->outsub->stopRefreshing();
	}
}

void linphone_friend_save(LinphoneFriend *fr, LinphoneCore *lc) {
	if (lc && lc->friends_db_file) linphone_core_store_friend_in_db(lc, fr);
}

void linphone_friend_apply(LinphoneFriend *fr, LinphoneCore *lc) {
	LinphonePresenceModel *model;
	const LinphoneAddress *addr = linphone_friend_get_address(fr);

	if (!addr) {
		ms_debug("No sip url defined in friend %s", linphone_friend_get_name(fr));
		return;
	}
	if (!linphone_core_ready(lc)) {
		/* lc not ready, deffering subscription */
		fr->commit = TRUE;
		return;
	}

	if (fr->inc_subscribe_pending) {
		switch (fr->pol) {
			case LinphoneSPWait:
				model = linphone_presence_model_new_with_activity(LinphonePresenceActivityOther,
				                                                  "Waiting for user acceptance");
				linphone_friend_notify(fr, model);
				linphone_presence_model_unref(model);
				break;
			case LinphoneSPAccept:
				if (fr->lc) linphone_friend_notify(fr, fr->lc->presence_model);
				break;
			case LinphoneSPDeny:
				linphone_friend_notify(fr, NULL);
				break;
		}
		fr->inc_subscribe_pending = FALSE;
	}

	if (fr->pol == LinphoneSPDeny && fr->insubs) {
		linphone_friend_close_incoming_subscriptions(fr);
	}

	linphone_friend_update_subscribes(fr, linphone_core_should_subscribe_friends_only_when_registered(lc));

	ms_debug("linphone_friend_apply() done.");
	lc->bl_refresh = TRUE;
	fr->commit = FALSE;
}

void linphone_friend_edit(LinphoneFriend *fr) {
	if (fr && linphone_core_vcard_supported() && fr->vcard) {
		linphone_vcard_compute_md5_hash(fr->vcard);
	}
}

void linphone_friend_done(LinphoneFriend *fr) {
	ms_return_if_fail(fr);
	if (!fr->lc) return;

	if (fr && linphone_core_vcard_supported() && fr->vcard) {
		if (linphone_vcard_compare_md5_hash(fr->vcard) != 0) {
			ms_debug("vCard's md5 has changed, mark friend as dirty and clear sip addresses list cache");
			linphone_vcard_clean_cache(fr->vcard);
			if (fr->friend_list) {
				fr->friend_list->dirty_friends_to_update =
				    bctbx_list_append(fr->friend_list->dirty_friends_to_update, linphone_friend_ref(fr));
			}
		}
	}
	linphone_friend_apply(fr, fr->lc);
	linphone_friend_save(fr, fr->lc);
}

#if __clang__ || ((__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __GNUC__ > 4)
#pragma GCC diagnostic push
#endif
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#else
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
LinphoneFriend *linphone_core_create_friend(LinphoneCore *lc) {
	LinphoneFriend *lf = linphone_friend_new();
	lf->lc = lc;
	return lf;
}

LinphoneFriend *linphone_core_create_friend_with_address(LinphoneCore *lc, const char *address) {
	LinphoneFriend *lf = linphone_friend_new_with_address(address);
	if (lf) lf->lc = lc;
	return lf;
}
#if __clang__ || ((__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __GNUC__ > 4)
#pragma GCC diagnostic pop
#endif

void linphone_core_add_friend(LinphoneCore *lc, LinphoneFriend *lf) {
	LinphoneFriendList *friendList = linphone_core_get_default_friend_list(lc);
	if (!friendList) {
		friendList = linphone_core_create_friend_list(lc);
		linphone_core_add_friend_list(lc, friendList);
		linphone_friend_list_unref(friendList);
	}
	if (linphone_friend_list_add_friend(friendList, lf) != LinphoneFriendListOK) return;
	if (bctbx_list_find(lc->subscribers, lf)) {
		/*if this friend was in the pending subscriber list, now remove it from this list*/
		lc->subscribers = bctbx_list_remove(lc->subscribers, lf);
		linphone_friend_unref(lf);
	}
}

void linphone_core_remove_friend(BCTBX_UNUSED(LinphoneCore *lc), LinphoneFriend *lf) {
	if (lf && lf->friend_list) {
		if (linphone_friend_list_remove_friend(lf->friend_list, lf) == LinphoneFriendListNonExistentFriend) {
			ms_error("linphone_core_remove_friend(): friend [%p] is not part of core's list.", lf);
		}
	}
}

void linphone_core_update_friends_subscriptions(LinphoneCore *lc) {
	bctbx_list_t *lists = lc->friends_lists;
	while (lists) {
		LinphoneFriendList *list = (LinphoneFriendList *)bctbx_list_get_data(lists);
		linphone_friend_list_update_subscriptions(list);
		lists = bctbx_list_next(lists);
	}
	// done here to avoid double initial subscribtion if triggered by proxy or from the app.
	lc->initial_subscribes_sent = TRUE;
}

bool_t linphone_core_should_subscribe_friends_only_when_registered(const LinphoneCore *lc) {
	return !!linphone_config_get_int(lc->config, "sip", "subscribe_presence_only_when_registered", 1);
}

void linphone_core_send_initial_subscribes(LinphoneCore *lc) {
	if (lc->initial_subscribes_sent) return;
	linphone_core_update_friends_subscriptions(lc);
	ms_message("Initial friend lists subscribes has been sent");
}

void linphone_core_invalidate_friend_subscriptions(LinphoneCore *lc) {
	bctbx_list_t *lists = lc->friends_lists;
	while (lists) {
		LinphoneFriendList *list = (LinphoneFriendList *)bctbx_list_get_data(lists);
		linphone_friend_list_invalidate_subscriptions(list);
		lists = bctbx_list_next(lists);
	}
	lc->initial_subscribes_sent = FALSE;
}

void linphone_friend_set_ref_key(LinphoneFriend *lf, const char *key) {
	if (lf->refkey != NULL) {
		ms_free(lf->refkey);
		lf->refkey = NULL;
	}
	if (key) {
		lf->refkey = ms_strdup(key);
	}
	if (lf->lc) {
		linphone_friend_save(lf, lf->lc);
	}
}

const char *linphone_friend_get_ref_key(const LinphoneFriend *lf) {
	return lf->refkey;
}

LinphoneFriend *linphone_core_find_friend(const LinphoneCore *lc, const LinphoneAddress *addr) {
	bctbx_list_t *lists = lc->friends_lists;
	LinphoneFriend *lf = NULL;
	while (lists && !lf) {
		LinphoneFriendList *list = (LinphoneFriendList *)bctbx_list_get_data(lists);
		lf = linphone_friend_list_find_friend_by_address(list, addr);
		lists = bctbx_list_next(lists);
	}
	return lf;
}

LinphoneFriend *linphone_core_find_friend_by_phone_number(const LinphoneCore *lc, const char *phoneNumber) {
	bctbx_list_t *lists = lc->friends_lists;
	LinphoneFriend *lf = NULL;
	while (lists && !lf) {
		LinphoneFriendList *list = (LinphoneFriendList *)bctbx_list_get_data(lists);
		lf = linphone_friend_list_find_friend_by_phone_number(list, phoneNumber);
		lists = bctbx_list_next(lists);
	}
	return lf;
}

bctbx_list_t *linphone_core_find_friends(const LinphoneCore *lc, const LinphoneAddress *addr) {
	bctbx_list_t *result = NULL;
	bctbx_list_t *lists = lc->friends_lists;
	while (lists) {
		LinphoneFriendList *list = (LinphoneFriendList *)bctbx_list_get_data(lists);
		bctbx_list_t *list_result = linphone_friend_list_find_friends_by_address(list, addr);
		if (list_result) {
			bctbx_list_t *elem;
			for (elem = list_result; elem != NULL; elem = bctbx_list_next(elem)) {
				LinphoneFriend *lf = (LinphoneFriend *)bctbx_list_get_data(elem);
				if (lf) {
					result = bctbx_list_append(result, linphone_friend_ref(lf));
				}
			}
			bctbx_list_free_with_data(list_result, (void (*)(void *))linphone_friend_unref);
		}
		lists = bctbx_list_next(lists);
	}
	return result;
}

LinphoneFriend *linphone_core_get_friend_by_address(const LinphoneCore *lc, const char *uri) {
	bctbx_list_t *lists = lc->friends_lists;
	LinphoneFriend *lf = NULL;
	while (lists && !lf) {
		LinphoneFriendList *list = (LinphoneFriendList *)bctbx_list_get_data(lists);
		lf = linphone_friend_list_find_friend_by_uri(list, uri);
		lists = bctbx_list_next(lists);
	}
	return lf;
}

LinphoneFriend *linphone_core_get_friend_by_ref_key(const LinphoneCore *lc, const char *key) {
	bctbx_list_t *lists = lc->friends_lists;
	LinphoneFriend *lf = NULL;
	while (lists && !lf) {
		LinphoneFriendList *list = (LinphoneFriendList *)bctbx_list_get_data(lists);
		lf = linphone_friend_list_find_friend_by_ref_key(list, key);
		lists = bctbx_list_next(lists);
	}
	return lf;
}

LinphoneFriend *linphone_core_find_friend_by_out_subscribe(const LinphoneCore *lc, SalOp *op) {
	bctbx_list_t *lists = lc->friends_lists;
	LinphoneFriend *lf = NULL;
	while (lists && !lf) {
		LinphoneFriendList *list = (LinphoneFriendList *)bctbx_list_get_data(lists);
		lf = linphone_friend_list_find_friend_by_out_subscribe(list, op);
		lists = bctbx_list_next(lists);
	}
	return lf;
}

LinphoneFriend *linphone_core_find_friend_by_inc_subscribe(const LinphoneCore *lc, SalOp *op) {
	bctbx_list_t *lists = lc->friends_lists;
	LinphoneFriend *lf = NULL;
	while (lists && !lf) {
		LinphoneFriendList *list = (LinphoneFriendList *)bctbx_list_get_data(lists);
		lf = linphone_friend_list_find_friend_by_inc_subscribe(list, op);
		lists = bctbx_list_next(lists);
	}
	return lf;
}

#define key_compare(s1, s2) strcmp(s1, s2)

LinphoneSubscribePolicy __policy_str_to_enum(const char *pol) {
	if (key_compare("accept", pol) == 0) {
		return LinphoneSPAccept;
	}
	if (key_compare("deny", pol) == 0) {
		return LinphoneSPDeny;
	}
	if (key_compare("wait", pol) == 0) {
		return LinphoneSPWait;
	}
	ms_warning("Unrecognized subscribe policy: %s", pol);
	return LinphoneSPWait;
}

LinphoneFriend *linphone_friend_new_from_config_file(LinphoneCore *lc, int index) {
	const char *tmp;
	char item[50];
	int a;
	LinphoneFriend *lf;
	LpConfig *config = lc->config;

	sprintf(item, "friend_%i", index);

	if (!linphone_config_has_section(config, item)) {
		return NULL;
	}

	tmp = linphone_config_get_string(config, item, "url", NULL);
	if (tmp == NULL) {
		return NULL;
	}
	lf = linphone_core_create_friend_with_address(lc, tmp);
	if (lf == NULL) {
		return NULL;
	}
	tmp = linphone_config_get_string(config, item, "pol", NULL);
	if (tmp == NULL) linphone_friend_set_inc_subscribe_policy(lf, LinphoneSPWait);
	else {
		linphone_friend_set_inc_subscribe_policy(lf, __policy_str_to_enum(tmp));
	}
	a = linphone_config_get_int(config, item, "subscribe", 0);
	linphone_friend_send_subscribe(lf, !!a);
	a = linphone_config_get_int(config, item, "presence_received", 0);
	lf->presence_received = (bool_t)a;
	lf->rc_index = index;

	linphone_friend_set_ref_key(lf, linphone_config_get_string(config, item, "refkey", NULL));
	linphone_friend_set_starred(lf, linphone_config_get_bool(config, item, "starred", FALSE));
	return lf;
}

int linphone_friend_get_rc_index(const LinphoneFriend *lf) {
	return lf->rc_index;
}

void linphone_friend_remove(LinphoneFriend *lf) {
	if (!lf) return;

	if (lf->friend_list) {
		linphone_friend_list_remove_friend(lf->friend_list, lf);
	}

	if (lf->rc_index >= 0) {
		LinphoneCore *lc = lf->lc;
		if (lc) {
			LinphoneConfig *config = linphone_core_get_config(lc);
			if (config) {
				char section[50];
				sprintf(section, "friend_%i", lf->rc_index);
				linphone_config_clean_section(config, section);
				linphone_core_config_sync(lc);
				lf->rc_index = -1;
			}
		}
	}
}

LinphoneCore *linphone_friend_get_core(const LinphoneFriend *fr) {
	return fr->lc;
}

LinphoneFriend *linphone_friend_ref(LinphoneFriend *lf) {
	belle_sip_object_ref(lf);
	return lf;
}

void linphone_friend_unref(LinphoneFriend *lf) {
	belle_sip_object_unref(lf);
}

/* DEPRECATED */
void linphone_friend_destroy(LinphoneFriend *lf) {
	linphone_friend_unref(lf);
}

LinphoneVcard *linphone_friend_get_vcard(const LinphoneFriend *fr) {
	if (fr && linphone_core_vcard_supported()) return fr->vcard;
	return NULL;
}

void linphone_friend_set_vcard(LinphoneFriend *fr, LinphoneVcard *vcard) {
	if (!fr || !linphone_core_vcard_supported()) return;

	const char *fullname = linphone_vcard_get_full_name(vcard);
	if (!fullname || strlen(fullname) == 0) {
		ms_warning("Trying to set an invalid vCard (no fullname) to friend, aborting");
		return;
	}

	if (fr->vcard) linphone_vcard_unref(fr->vcard);
	if (vcard) fr->vcard = linphone_vcard_ref(vcard);
	linphone_friend_save(fr, fr->lc);
}

bool_t linphone_friend_create_vcard(LinphoneFriend *fr, const char *name) {
	LinphoneVcard *vcard = NULL;
	LinphoneCore *lc = NULL;
	bool_t skip = FALSE;

	if (!fr || !name) {
		ms_error("Can't create vCard for friend [%p] with name [%s]", fr, name);
		return FALSE;
	}
	if (!linphone_core_vcard_supported()) {
		ms_warning("VCard support is not builtin");
		return FALSE;
	}
	if (fr->vcard) {
		ms_error("Friend already has a VCard");
		return FALSE;
	}

	vcard = linphone_factory_create_vcard(linphone_factory_get());

	lc = fr->lc;
	if (!lc && fr->friend_list) {
		lc = fr->friend_list->lc;
	}
	if (lc) {
		skip = !linphone_config_get_int(fr->lc->config, "misc", "store_friends", 1);
		linphone_vcard_set_skip_validation(vcard, skip);
	}
	linphone_vcard_set_full_name(vcard, name);
	linphone_friend_set_vcard(fr, vcard);
	linphone_vcard_unref(vcard);
	ms_debug("VCard created for friend [%p]", fr);
	return TRUE;
}

#if __clang__ || ((__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __GNUC__ > 4)
#pragma GCC diagnostic push
#endif
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#else
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
LinphoneFriend *linphone_friend_new_from_vcard(LinphoneVcard *vcard) {
	LinphoneFriend *fr;

	if (!linphone_core_vcard_supported()) {
		ms_error("VCard support is not builtin");
		return NULL;
	}
	if (vcard == NULL) {
		ms_error("Cannot create friend from null vcard");
		return NULL;
	}

	fr = linphone_friend_new();
	// Currently presence takes too much time when dealing with hundreds of friends, so I disabled it for now
	fr->pol = LinphoneSPDeny;
	fr->subscribe = FALSE;
	linphone_friend_set_vcard(fr, vcard);
	return fr;
}
#if __clang__ || ((__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __GNUC__ > 4)
#pragma GCC diagnostic pop
#endif

/*drops all references to the core and unref*/
void _linphone_friend_release(LinphoneFriend *lf) {
	lf->lc = NULL;
	_linphone_friend_release_ops(lf);
	linphone_friend_unref(lf);
}

BELLE_SIP_DECLARE_NO_IMPLEMENTED_INTERFACES(LinphoneFriend);

BELLE_SIP_INSTANCIATE_VPTR(LinphoneFriend,
                           belle_sip_object_t,
                           (belle_sip_object_destroy_t)_linphone_friend_destroy,
                           NULL, // clone
                           _linphone_friend_marshall,
                           FALSE);

void linphone_friend_add_addresses_and_numbers_into_maps(LinphoneFriend *lf, LinphoneFriendList *list) {
	bctbx_list_t *iterator;
	bctbx_list_t *phone_numbers;
	const bctbx_list_t *addresses;

	if (lf->refkey) {
		bctbx_pair_t *pair = (bctbx_pair_t *)bctbx_pair_cchar_new(lf->refkey, linphone_friend_ref(lf));
		bctbx_map_cchar_insert_and_delete(list->friends_map, pair);
	}

	phone_numbers = linphone_friend_get_phone_numbers(lf);
	iterator = phone_numbers;
	while (iterator) {
		const char *number = (const char *)bctbx_list_get_data(iterator);
		const char *uri = linphone_friend_phone_number_to_sip_uri(lf, number);
		if (uri) {
			add_friend_to_list_map_if_not_in_it_yet(lf, uri);
		}
		iterator = bctbx_list_next(iterator);
	}
	bctbx_list_free(phone_numbers);

	addresses = linphone_friend_get_addresses(lf);
	iterator = (bctbx_list_t *)addresses;
	while (iterator) {
		LinphoneAddress *lfaddr = (LinphoneAddress *)bctbx_list_get_data(iterator);
		char *uri = linphone_address_as_string_uri_only(lfaddr);
		if (uri) {
			add_friend_to_list_map_if_not_in_it_yet(lf, uri);
			ms_free(uri);
		}
		iterator = bctbx_list_next(iterator);
	}
}

/*******************************************************************************
 * SQL storage related functions                                               *
 ******************************************************************************/

#ifdef HAVE_SQLITE

static void linphone_create_friends_table(sqlite3 *db) {
	char *errmsg = NULL;
	int ret;
	ret = sqlite3_exec(db,
	                   "CREATE TABLE IF NOT EXISTS friends ("
	                   "id                INTEGER PRIMARY KEY AUTOINCREMENT,"
	                   "friend_list_id    INTEGER,"
	                   "sip_uri           TEXT,"
	                   "subscribe_policy  INTEGER,"
	                   "send_subscribe    INTEGER,"
	                   "ref_key           TEXT,"
	                   "vCard             TEXT,"
	                   "vCard_etag        TEXT,"
	                   "vCard_url         TEXT,"
	                   "presence_received INTEGER"
	                   ");",
	                   0, 0, &errmsg);
	if (ret != SQLITE_OK) {
		ms_error("Error in creation: %s.", errmsg);
		sqlite3_free(errmsg);
	}

	ret = sqlite3_exec(db,
	                   "CREATE TABLE IF NOT EXISTS friends_lists ("
	                   "id                INTEGER PRIMARY KEY AUTOINCREMENT,"
	                   "display_name      TEXT,"
	                   "rls_uri           TEXT,"
	                   "uri               TEXT,"
	                   "revision          INTEGER"
	                   ");",
	                   0, 0, &errmsg);
	if (ret != SQLITE_OK) {
		ms_error("Error in creation: %s.", errmsg);
		sqlite3_free(errmsg);
	}
}

static bool_t linphone_update_friends_table(sqlite3 *db) {
	static sqlite3_stmt *stmt_version;
	int database_user_version = -1;
	char *errmsg = NULL;

	if (sqlite3_prepare_v2(db, "PRAGMA user_version;", -1, &stmt_version, NULL) == SQLITE_OK) {
		while (sqlite3_step(stmt_version) == SQLITE_ROW) {
			database_user_version = sqlite3_column_int(stmt_version, 0);
			ms_debug("friends database user version = %i", database_user_version);
		}
	}
	sqlite3_finalize(stmt_version);

	if (database_user_version != 3100) { // Linphone 3.10.0
		int ret =
		    sqlite3_exec(db,
		                 "BEGIN TRANSACTION;\n"
		                 "ALTER TABLE friends RENAME TO temp_friends;\n"
		                 "CREATE TABLE IF NOT EXISTS friends ("
		                 "id                INTEGER PRIMARY KEY AUTOINCREMENT,"
		                 "friend_list_id    INTEGER,"
		                 "sip_uri           TEXT,"
		                 "subscribe_policy  INTEGER,"
		                 "send_subscribe    INTEGER,"
		                 "ref_key           TEXT,"
		                 "vCard             TEXT,"
		                 "vCard_etag        TEXT,"
		                 "vCard_url         TEXT,"
		                 "presence_received INTEGER"
		                 ");\n"
		                 "INSERT INTO friends SELECT id, friend_list_id, sip_uri, subscribe_policy, send_subscribe, "
		                 "ref_key, vCard, vCard_etag, vCard_url, presence_received FROM temp_friends;\n"
		                 "DROP TABLE temp_friends;\n"
		                 "PRAGMA user_version = 3100;\n"
		                 "COMMIT;",
		                 0, 0, &errmsg);
		if (ret != SQLITE_OK) {
			ms_error("Error altering table friends: %s.", errmsg);
			sqlite3_free(errmsg);
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

void linphone_core_friends_storage_init(LinphoneCore *lc) {
	int ret;
	const char *errmsg;
	sqlite3 *db;

	linphone_core_friends_storage_close(lc);

	ret = _linphone_sqlite3_open(lc->friends_db_file, &db);
	if (ret != SQLITE_OK) {
		errmsg = sqlite3_errmsg(db);
		ms_error("Error in the opening: %s.", errmsg);
		sqlite3_close(db);
		return;
	}

	linphone_create_friends_table(db);
	if (linphone_update_friends_table(db)) {
		// After updating schema, database need to be closed/reopenned
		sqlite3_close(db);
		_linphone_sqlite3_open(lc->friends_db_file, &db);
	}

	lc->friends_db = db;

	linphone_core_friends_storage_resync_friends_lists(lc);
}

static int linphone_sql_request_generic(sqlite3 *db, const char *stmt) {
	char *errmsg = NULL;
	int ret;
	ret = sqlite3_exec(db, stmt, NULL, NULL, &errmsg);
	if (ret != SQLITE_OK) {
		ms_error("linphone_sql_request: statement %s -> error sqlite3_exec(): %s.", stmt, errmsg);
		sqlite3_free(errmsg);
	}
	return ret;
}

int linphone_core_friends_storage_resync_friends_lists(LinphoneCore *lc) {
	bctbx_list_t *friends_lists = NULL;
	int synced_friends_lists = 0;

	/**
	 * First lets remove all the orphan friends from the DB
	 */
	char *buf = sqlite3_mprintf("delete from friends where friend_list_id not in (select id from friends_lists)");
	linphone_sql_request_generic(lc->friends_db, buf);
	sqlite3_free(buf);

	friends_lists = linphone_core_fetch_friends_lists_from_db(lc);
	if (friends_lists) {
		const bctbx_list_t *it;
		ms_warning("Replacing current default friend list by the one(s) from the database");
		lc->friends_lists =
		    bctbx_list_free_with_data(lc->friends_lists, (bctbx_list_free_func)linphone_friend_list_unref);

		const char *url = linphone_config_get_string(lc->config, "misc", "contacts-vcard-list", NULL);

		for (it = friends_lists; it != NULL; it = bctbx_list_next(it)) {
			LinphoneFriendList *list = (LinphoneFriendList *)bctbx_list_get_data(it);

			const char *list_name = linphone_friend_list_get_display_name(list);
			if (list_name && url && strcmp(url, list_name) == 0) {
				linphone_friend_list_set_type(list, LinphoneFriendListTypeVCard4);
			}
			linphone_core_add_friend_list(lc, list);
			synced_friends_lists++;
		}
		friends_lists = bctbx_list_free_with_data(friends_lists, (bctbx_list_free_func)linphone_friend_list_unref);
	}

	return synced_friends_lists;
}

void linphone_core_friends_storage_close(LinphoneCore *lc) {
	if (lc->friends_db) {
		sqlite3_close(lc->friends_db);
		lc->friends_db = NULL;
	}
}

/* DB layout:
 * | 0  | storage_id
 * | 1  | display_name
 * | 2  | rls_uri
 * | 3  | uri
 * | 4  | revision
 */
static int create_friend_list(void *data, BCTBX_UNUSED(int argc), char **argv, BCTBX_UNUSED(char **colName)) {
	bctbx_list_t **list = (bctbx_list_t **)data;
	unsigned int storage_id = (unsigned int)atoi(argv[0]);
	LinphoneFriendList *lfl = linphone_core_create_friend_list(NULL);

	lfl->storage_id = storage_id;
	linphone_friend_list_set_display_name(lfl, argv[1]);
	linphone_friend_list_set_rls_uri(lfl, argv[2]);
	linphone_friend_list_set_uri(lfl, argv[3]);
	lfl->revision = atoi(argv[4]);

	*list = bctbx_list_append(*list, linphone_friend_list_ref(lfl));
	linphone_friend_list_unref(lfl);
	return 0;
}

#if __clang__ || ((__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __GNUC__ > 4)
#pragma GCC diagnostic push
#endif
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#else
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
/* DB layout:
 * | 0  | storage_id
 * | 1  | friend_list_id
 * | 2  | sip_uri
 * | 3  | subscribe_policy
 * | 4  | send_subscribe
 * | 5  | ref_key
 * | 6  | vCard
 * | 7  | vCard eTag
 * | 8  | vCard URL
 * | 9  | presence_received
 */
static int create_friend(void *data, BCTBX_UNUSED(int argc), char **argv, BCTBX_UNUSED(char **colName)) {
	LinphoneVcardContext *context = (LinphoneVcardContext *)data;
	bctbx_list_t **list = (bctbx_list_t **)linphone_vcard_context_get_user_data(context);
	LinphoneFriend *lf = NULL;
	LinphoneVcard *vcard = NULL;
	unsigned int storage_id = (unsigned int)atoi(argv[0]);

	vcard = linphone_vcard_context_get_vcard_from_buffer(context, argv[6]);
	if (vcard) {
		linphone_vcard_set_etag(vcard, argv[7]);
		linphone_vcard_set_url(vcard, argv[8]);
		lf = linphone_friend_new_from_vcard(vcard);
		linphone_vcard_unref(vcard);
	}
	if (!lf) {
		lf = linphone_friend_new();
		if (argv[2] != NULL) {
			LinphoneAddress *addr = linphone_address_new(argv[2]);
			if (addr) {
				linphone_friend_set_address(lf, addr);
				linphone_address_unref(addr);
			}
		}
	}
	linphone_friend_set_inc_subscribe_policy(lf, static_cast<LinphoneSubscribePolicy>(atoi(argv[3])));
	linphone_friend_send_subscribe(lf, !!atoi(argv[4]));
	linphone_friend_set_ref_key(lf, ms_strdup(argv[5]));
	lf->presence_received = !!atoi(argv[9]);
	lf->storage_id = storage_id;

	*list = bctbx_list_append(*list, linphone_friend_ref(lf));
	linphone_friend_unref(lf);
	return 0;
}
#if __clang__ || ((__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __GNUC__ > 4)
#pragma GCC diagnostic pop
#endif

static int linphone_sql_request_friend(sqlite3 *db, const char *stmt, LinphoneVcardContext *context) {
	char *errmsg = NULL;
	int ret;
	ret = sqlite3_exec(db, stmt, create_friend, context, &errmsg);
	if (ret != SQLITE_OK) {
		ms_error("linphone_sql_request: statement %s -> error sqlite3_exec(): %s.", stmt, errmsg);
		sqlite3_free(errmsg);
	}
	return ret;
}

static int linphone_sql_request_friends_list(sqlite3 *db, const char *stmt, bctbx_list_t **list) {
	char *errmsg = NULL;
	int ret;
	ret = sqlite3_exec(db, stmt, create_friend_list, list, &errmsg);
	if (ret != SQLITE_OK) {
		ms_error("linphone_sql_request: statement %s -> error sqlite3_exec(): %s.", stmt, errmsg);
		sqlite3_free(errmsg);
	}
	return ret;
}

void linphone_core_store_friend_in_db(LinphoneCore *lc, LinphoneFriend *lf) {
	if (lc && lc->friends_db) {
		char *buf;
		int store_friends = linphone_config_get_int(lc->config, "misc", "store_friends", 1);
		LinphoneVcard *vcard = NULL;
		const LinphoneAddress *addr;
		char *addr_str = NULL;

		if (!store_friends) {
			return;
		}

		if (!lf || !lf->friend_list) {
			ms_warning("Either the friend or the friend list is null, skipping...");
			return;
		}

		if (linphone_friend_list_is_subscription_bodyless(
		        lf->friend_list)) // Add friend in DB only if its list bodyless subscription is not enabled.
			return;

		/**
		 * The friends_list store logic is hidden into the friend store logic
		 */
		if (lf->friend_list->storage_id == 0) {
			ms_warning("Trying to add a friend in db, but friend list isn't, let's do that first");
			linphone_core_store_friends_list_in_db(lc, lf->friend_list);
		}

		if (linphone_core_vcard_supported()) vcard = linphone_friend_get_vcard(lf);
		addr = linphone_friend_get_address(lf);
		if (addr != NULL) addr_str = linphone_address_as_string(addr);
		if (lf->storage_id > 0) {
			buf = sqlite3_mprintf("UPDATE friends SET "
			                      "friend_list_id=%u,sip_uri=%Q,subscribe_policy=%i,send_subscribe=%i,ref_key=%Q,vCard="
			                      "%Q,vCard_etag=%Q,vCard_url=%Q,presence_received=%i WHERE (id = %u);",
			                      lf->friend_list->storage_id, addr_str, lf->pol, lf->subscribe, lf->refkey,
			                      vcard ? linphone_vcard_as_vcard4_string(vcard) : NULL,
			                      vcard ? linphone_vcard_get_etag(vcard) : NULL,
			                      vcard ? linphone_vcard_get_url(vcard) : NULL, lf->presence_received, lf->storage_id);
		} else {
			buf = sqlite3_mprintf("INSERT INTO friends VALUES(NULL,%u,%Q,%i,%i,%Q,%Q,%Q,%Q,%i);",
			                      lf->friend_list->storage_id, addr_str, lf->pol, lf->subscribe, lf->refkey,
			                      vcard ? linphone_vcard_as_vcard4_string(vcard) : NULL,
			                      vcard ? linphone_vcard_get_etag(vcard) : NULL,
			                      vcard ? linphone_vcard_get_url(vcard) : NULL, lf->presence_received);
		}
		if (addr_str != NULL) ms_free(addr_str);

		linphone_sql_request_generic(lc->friends_db, buf);
		sqlite3_free(buf);

		if (lf->storage_id == 0) {
			lf->storage_id = (unsigned int)sqlite3_last_insert_rowid(lc->friends_db);
		}
	}
}

void linphone_core_store_friends_list_in_db(LinphoneCore *lc, LinphoneFriendList *list) {
	if (lc && lc->friends_db &&
	    !linphone_friend_list_is_subscription_bodyless(list)) { // Do not store list if bodyless subscription is enabled
		char *buf;
		int store_friends = linphone_config_get_int(lc->config, "misc", "store_friends", 1);

		if (!store_friends) {
			return;
		}

		if (list->storage_id > 0) {
			buf = sqlite3_mprintf(
			    "UPDATE friends_lists SET display_name=%Q,rls_uri=%Q,uri=%Q,revision=%i WHERE (id = %u);",
			    list->display_name, list->rls_uri, list->uri, list->revision, list->storage_id);
		} else {
			buf = sqlite3_mprintf("INSERT INTO friends_lists VALUES(NULL,%Q,%Q,%Q,%i);", list->display_name,
			                      list->rls_uri, list->uri, list->revision);
		}
		linphone_sql_request_generic(lc->friends_db, buf);
		sqlite3_free(buf);

		if (list->storage_id == 0) {
			list->storage_id = (unsigned int)sqlite3_last_insert_rowid(lc->friends_db);
		}
	}
}

void linphone_core_remove_friend_from_db(LinphoneCore *lc, LinphoneFriend *lf) {
	if (lc && lc->friends_db) {
		char *buf;
		if (lf->storage_id == 0) {
			ms_error("Friend doesn't have a storage_id !");
			return;
		}

		buf = sqlite3_mprintf("DELETE FROM friends WHERE id = %u", lf->storage_id);
		linphone_sql_request_generic(lc->friends_db, buf);
		sqlite3_free(buf);

		lf->storage_id = 0;
	}
}

void linphone_core_remove_friends_list_from_db(LinphoneCore *lc, LinphoneFriendList *list) {
	if (lc && lc->friends_db) {
		char *buf;
		if (list->storage_id == 0) {
			ms_error("Friend list doesn't have a storage_id !");
			return;
		}

		buf =
		    sqlite3_mprintf("DELETE FROM friends WHERE friend_list_id in (select id from friends_lists where id = %u)",
		                    list->storage_id);
		linphone_sql_request_generic(lc->friends_db, buf);
		sqlite3_free(buf);

		buf = sqlite3_mprintf("DELETE FROM friends_lists WHERE id = %u", list->storage_id);
		linphone_sql_request_generic(lc->friends_db, buf);
		sqlite3_free(buf);

		list->storage_id = 0;
	}
}

bctbx_list_t *linphone_core_fetch_friends_from_db(LinphoneCore *lc, LinphoneFriendList *list) {
	char *buf;
	uint64_t begin, end;
	bctbx_list_t *result = NULL;
	bctbx_list_t *elem = NULL;

	if (!lc) {
		ms_warning("lc is NULL");
		return NULL;
	}

	if (lc->friends_db == NULL) {
		ms_warning("Friends database wasn't initialized with linphone_core_friends_storage_init() yet");
		return NULL;
	}

	linphone_vcard_context_set_user_data(lc->vcard_context, &result);

	buf = sqlite3_mprintf("SELECT * FROM friends WHERE friend_list_id = %u ORDER BY id", list->storage_id);

	begin = bctbx_get_cur_time_ms();
	linphone_sql_request_friend(lc->friends_db, buf, lc->vcard_context);
	end = bctbx_get_cur_time_ms();
	ms_message("%s(): %u results fetched, completed in %i ms", __FUNCTION__, (unsigned int)bctbx_list_size(result),
	           (int)(end - begin));
	sqlite3_free(buf);

	for (elem = result; elem != NULL; elem = bctbx_list_next(elem)) {
		LinphoneFriend *lf = (LinphoneFriend *)bctbx_list_get_data(elem);
		lf->lc = lc;
		lf->friend_list = list;
		linphone_friend_add_addresses_and_numbers_into_maps(lf, list);
	}
	linphone_vcard_context_set_user_data(lc->vcard_context, NULL);

	return result;
}

bctbx_list_t *linphone_core_fetch_friends_lists_from_db(LinphoneCore *lc) {
	char *buf;
	uint64_t begin, end;
	bctbx_list_t *result = NULL;
	bctbx_list_t *elem = NULL;

	if (!lc) {
		ms_warning("lc is NULL");
		return NULL;
	}

	if (lc->friends_db == NULL) {
		ms_warning("Friends database wasn't initialized with linphone_core_friends_storage_init() yet");
		return NULL;
	}

	buf = sqlite3_mprintf("SELECT * FROM friends_lists ORDER BY id");

	begin = bctbx_get_cur_time_ms();
	linphone_sql_request_friends_list(lc->friends_db, buf, &result);
	end = bctbx_get_cur_time_ms();
	ms_message("%s(): %u results fetched, completed in %i ms", __FUNCTION__, (unsigned int)bctbx_list_size(result),
	           (int)(end - begin));
	sqlite3_free(buf);

	for (elem = result; elem != NULL; elem = bctbx_list_next(elem)) {
		LinphoneFriendList *lfl = (LinphoneFriendList *)bctbx_list_get_data(elem);
		lfl->lc = lc;
		lfl->friends = linphone_core_fetch_friends_from_db(lc, lfl);
	}

	return result;
}

#else

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void linphone_core_store_friends_list_in_db(LinphoneCore *lc, LinphoneFriendList *list) {
	ms_warning("linphone_core_store_friends_list_in_db(): stubbed");
}

void linphone_core_friends_storage_init(LinphoneCore *lc) {
}

void linphone_core_friends_storage_close(LinphoneCore *lc) {
}

void linphone_core_store_friend_in_db(LinphoneCore *lc, LinphoneFriend *lf) {
	ms_warning("linphone_core_store_friend_in_db(): stubbed");
}

void linphone_core_remove_friends_list_from_db(LinphoneCore *lc, LinphoneFriendList *list) {
	ms_warning("linphone_core_store_friend_in_db(): stubbed");
}

int linphone_core_friends_storage_resync_friends_lists(LinphoneCore *lc) {
	ms_warning("linphone_core_friends_storage_resync_friends_lists(): stubbed");
	return -1;
}

#pragma GCC diagnostic pop

#endif /* HAVE_SQLITE */

void linphone_core_set_friends_database_path(LinphoneCore *lc, const char *path) {
	if (!linphone_core_conference_server_enabled(lc) && L_GET_PRIVATE(lc->cppPtr)->mainDb)
		L_GET_PRIVATE(lc->cppPtr)->mainDb->import(LinphonePrivate::MainDb::Sqlite3, path);

	// TODO: Remove me later.
	if (lc->friends_db_file) {
		ms_free(lc->friends_db_file);
		lc->friends_db_file = NULL;
	}
	if (path) {
		lc->friends_db_file = ms_strdup(path);
		linphone_core_friends_storage_init(lc);
	}
}

const char *linphone_core_get_friends_database_path(LinphoneCore *lc) {
	return lc->friends_db_file;
}

LinphoneSubscriptionState linphone_friend_get_subscription_state(const LinphoneFriend *lf) {
	return lf->out_sub_state;
}

const char *linphone_friend_phone_number_to_sip_uri(LinphoneFriend *lf, const char *phone_number) {
	LinphoneFriendPhoneNumberSipUri *lfpnsu;
	char *normalized_number;
	char *full_uri;
	LinphoneProxyConfig *proxy_config;
	bctbx_list_t *iterator = lf->phone_number_sip_uri_map;

	while (iterator) {
		lfpnsu = (LinphoneFriendPhoneNumberSipUri *)bctbx_list_get_data(iterator);
		if (strcmp(lfpnsu->number, phone_number) == 0) {
			/*force sip uri computation because proxy config may have changed, specially, ccc could have been added
			 * since last computation*/
			// free_phone_number_sip_uri(lfpnsu);
			if (lf->phone_number_sip_uri_map == iterator) {
				/*change list head if head is removed*/
				iterator = lf->phone_number_sip_uri_map = bctbx_list_erase_link(lf->phone_number_sip_uri_map, iterator);
			} else {
				iterator = bctbx_list_erase_link(lf->phone_number_sip_uri_map, iterator);
			}
		} else {
			iterator = bctbx_list_next(iterator);
		}
	}

	proxy_config = linphone_core_get_default_proxy_config(linphone_friend_get_core(lf));
	if (!proxy_config) return NULL;
	if (strstr(phone_number, "tel:") == phone_number)
		phone_number += 4; /* Remove the "tel:" prefix if it is present. */
	normalized_number = linphone_proxy_config_normalize_phone_number(proxy_config, phone_number);
	if (!normalized_number) return NULL;
	full_uri =
	    ms_strdup_printf("sip:%s@%s;user=phone", normalized_number, linphone_proxy_config_get_domain(proxy_config));
	ms_free(normalized_number);

	lfpnsu = ms_new0(LinphoneFriendPhoneNumberSipUri, 1);
	lfpnsu->number = ms_strdup(phone_number);
	lfpnsu->uri = full_uri;
	lf->phone_number_sip_uri_map = bctbx_list_append(lf->phone_number_sip_uri_map, lfpnsu);
	return full_uri;
}

const char *linphone_friend_sip_uri_to_phone_number(LinphoneFriend *lf, const char *uri) {
	bctbx_list_t *iterator = lf->phone_number_sip_uri_map;

	while (iterator) {
		LinphoneFriendPhoneNumberSipUri *lfpnsu = (LinphoneFriendPhoneNumberSipUri *)bctbx_list_get_data(iterator);
		if (strcmp(lfpnsu->uri, uri) == 0) return lfpnsu->number;
		iterator = bctbx_list_next(iterator);
	}
	return NULL;
}

void linphone_friend_clear_presence_models(LinphoneFriend *lf) {
	lf->presence_models = bctbx_list_free_with_data(lf->presence_models, (bctbx_list_free_func)free_friend_presence);
}

int linphone_friend_get_capabilities(const LinphoneFriend *lf) {
	int capabilities = 0;
	const LinphonePresenceModel *presence = NULL;
	const bctbx_list_t *addrs = linphone_friend_get_addresses(lf);
	bctbx_list_t *phones = linphone_friend_get_phone_numbers(lf);
	bctbx_list_t *it;

	for (it = (bctbx_list_t *)addrs; it != NULL; it = it->next) {
		LinphoneAddress *addr = (LinphoneAddress *)it->data;
		char *uri = linphone_address_as_string_uri_only(addr);
		presence = linphone_friend_get_presence_model_for_uri_or_tel(lf, uri);
		ms_free(uri);

		if (!presence) continue;
		capabilities |= linphone_presence_model_get_capabilities(presence);
	}
	for (it = phones; it != NULL; it = it->next) {
		presence = linphone_friend_get_presence_model_for_uri_or_tel(lf, static_cast<const char *>(it->data));

		if (!presence) continue;
		capabilities |= linphone_presence_model_get_capabilities(presence);
	}
	bctbx_list_free(phones);

	return capabilities;
}

bool_t linphone_friend_has_capability(const LinphoneFriend *lf, const LinphoneFriendCapability capability) {
	return !!(linphone_friend_get_capabilities(lf) & capability);
}

bool_t linphone_friend_has_capability_with_version(const LinphoneFriend *lf,
                                                   const LinphoneFriendCapability capability,
                                                   float version) {
	const LinphonePresenceModel *presence = NULL;
	const bctbx_list_t *addrs = linphone_friend_get_addresses(lf);
	bctbx_list_t *phones = linphone_friend_get_phone_numbers(lf);
	bctbx_list_t *it;
	bool_t result = FALSE;

	for (it = (bctbx_list_t *)addrs; it != NULL; it = it->next) {
		LinphoneAddress *addr = (LinphoneAddress *)it->data;
		char *uri = linphone_address_as_string_uri_only(addr);
		presence = linphone_friend_get_presence_model_for_uri_or_tel(lf, uri);
		ms_free(uri);

		if (!presence) continue;
		if (linphone_presence_model_has_capability_with_version(presence, capability, version)) result = TRUE;
	}
	for (it = phones; it != NULL; it = it->next) {
		presence = linphone_friend_get_presence_model_for_uri_or_tel(lf, static_cast<const char *>(it->data));

		if (!presence) continue;
		if (linphone_presence_model_has_capability_with_version(presence, capability, version)) result = TRUE;
	}
	bctbx_list_free(phones);

	return result;
}

bool_t linphone_friend_has_capability_with_version_or_more(const LinphoneFriend *lf,
                                                           const LinphoneFriendCapability capability,
                                                           float version) {
	const LinphonePresenceModel *presence = NULL;
	const bctbx_list_t *addrs = linphone_friend_get_addresses(lf);
	bctbx_list_t *phones = linphone_friend_get_phone_numbers(lf);
	bctbx_list_t *it;
	bool_t result = FALSE;

	for (it = (bctbx_list_t *)addrs; it != NULL; it = it->next) {
		LinphoneAddress *addr = (LinphoneAddress *)it->data;
		char *uri = linphone_address_as_string_uri_only(addr);
		presence = linphone_friend_get_presence_model_for_uri_or_tel(lf, uri);
		ms_free(uri);

		if (!presence) continue;
		if (linphone_presence_model_has_capability_with_version_or_more(presence, capability, version)) result = TRUE;
	}
	for (it = phones; it != NULL; it = it->next) {
		presence = linphone_friend_get_presence_model_for_uri_or_tel(lf, static_cast<const char *>(it->data));

		if (!presence) continue;
		if (linphone_presence_model_has_capability_with_version_or_more(presence, capability, version)) result = TRUE;
	}
	bctbx_list_free(phones);

	return result;
}

float linphone_friend_get_capability_version(const LinphoneFriend *lf, const LinphoneFriendCapability capability) {
	const LinphonePresenceModel *presence = NULL;
	const bctbx_list_t *addrs = linphone_friend_get_addresses(lf);
	bctbx_list_t *phones = linphone_friend_get_phone_numbers(lf);
	bctbx_list_t *it;
	float version = -1.0;

	for (it = (bctbx_list_t *)addrs; it != NULL; it = it->next) {
		LinphoneAddress *addr = (LinphoneAddress *)it->data;
		char *uri = linphone_address_as_string_uri_only(addr);
		presence = linphone_friend_get_presence_model_for_uri_or_tel(lf, uri);
		ms_free(uri);

		if (!presence) continue;
		float presence_version = linphone_presence_model_get_capability_version(presence, capability);
		if (presence_version > version) version = presence_version;
	}
	for (it = phones; it != NULL; it = it->next) {
		presence = linphone_friend_get_presence_model_for_uri_or_tel(lf, static_cast<const char *>(it->data));

		if (!presence) continue;
		float presence_version = linphone_presence_model_get_capability_version(presence, capability);
		if (presence_version > version) version = presence_version;
	}
	bctbx_list_free(phones);

	return version;
}

void linphone_friend_set_photo(LinphoneFriend *lf, const char *picture_uri) {
	if (!lf) return;

	if (linphone_core_vcard_supported()) {
		linphone_vcard_set_photo(lf->vcard, picture_uri);
	}
}

const char *linphone_friend_get_photo(const LinphoneFriend *lf) {
	if (!lf) return NULL;

	if (linphone_core_vcard_supported()) {
		return linphone_vcard_get_photo(lf->vcard);
	}

	return NULL;
}

void linphone_friend_set_starred(LinphoneFriend *lf, bool_t is_starred) {
	if (lf) lf->is_starred = is_starred;
}

bool_t linphone_friend_get_starred(const LinphoneFriend *lf) {
	if (!lf) return FALSE;
	return lf->is_starred;
}

void linphone_friend_set_native_uri(LinphoneFriend *lf, const char *native_uri) {
	if (!lf) return;
	if (lf->native_uri) {
		ms_free(lf->native_uri);
		lf->native_uri = NULL;
	}
	if (native_uri) {
		lf->native_uri = ms_strdup(native_uri);
	}
}

const char *linphone_friend_get_native_uri(const LinphoneFriend *lf) {
	if (!lf) return NULL;
	return lf->native_uri;
}

void linphone_friend_set_organization(LinphoneFriend *lf, const char *organization) {
	if (!lf) return;
	if (linphone_core_vcard_supported() && lf->vcard) {
		linphone_vcard_set_organization(lf->vcard, organization);
	}
}

const char *linphone_friend_get_organization(const LinphoneFriend *lf) {
	if (lf && linphone_core_vcard_supported() && lf->vcard) {
		return linphone_vcard_get_organization(lf->vcard);
	}
	return NULL;
}
