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

#include <bctoolbox/defs.h>

#include <bctoolbox/crypto.h>

#include "linphone/api/c-content.h"
#include "linphone/core.h"

#include "c-wrapper/c-wrapper.h"

// TODO: From coreapi. Remove me later.
#include "private.h"

BELLE_SIP_DECLARE_NO_IMPLEMENTED_INTERFACES(LinphoneFriendListCbs);

BELLE_SIP_INSTANCIATE_VPTR(LinphoneFriendListCbs,
                           belle_sip_object_t,
                           NULL, // destroy
                           NULL, // clone
                           NULL, // Marshall
                           FALSE);

LinphoneFriendListCbs *linphone_friend_list_cbs_new(void) {
	return belle_sip_object_new(LinphoneFriendListCbs);
}

LinphoneFriendListCbs *linphone_friend_list_get_callbacks(const LinphoneFriendList *friend_list) {
	return friend_list->cbs;
}

void linphone_friend_list_add_callbacks(LinphoneFriendList *friend_list, LinphoneFriendListCbs *cbs) {
	friend_list->callbacks = bctbx_list_append(friend_list->callbacks, linphone_friend_list_cbs_ref(cbs));
}

void linphone_friend_list_remove_callbacks(LinphoneFriendList *friend_list, LinphoneFriendListCbs *cbs) {
	friend_list->callbacks = bctbx_list_remove(friend_list->callbacks, cbs);
	linphone_friend_list_cbs_unref(cbs);
}

LinphoneFriendListCbs *linphone_friend_list_get_current_callbacks(const LinphoneFriendList *friend_list) {
	return friend_list->currentCbs;
}

void linphone_friend_list_set_current_callbacks(LinphoneFriendList *friend_list, LinphoneFriendListCbs *cbs) {
	friend_list->currentCbs = cbs;
}

const bctbx_list_t *linphone_friend_list_get_callbacks_list(const LinphoneFriendList *friend_list) {
	return friend_list->callbacks;
}

#define NOTIFY_IF_EXIST(cbName, functionName, ...)                                                                     \
	bctbx_list_t *callbacksCopy = bctbx_list_copy(linphone_friend_list_get_callbacks_list(list));                      \
	for (bctbx_list_t *it = callbacksCopy; it; it = bctbx_list_next(it)) {                                             \
		linphone_friend_list_set_current_callbacks(                                                                    \
		    list, reinterpret_cast<LinphoneFriendListCbs *>(bctbx_list_get_data(it)));                                 \
		LinphoneFriendListCbs##cbName##Cb cb =                                                                         \
		    linphone_friend_list_cbs_get_##functionName(linphone_friend_list_get_current_callbacks(list));             \
		if (cb) cb(__VA_ARGS__);                                                                                       \
	}                                                                                                                  \
	linphone_friend_list_set_current_callbacks(list, nullptr);                                                         \
	bctbx_list_free(callbacksCopy);

LinphoneFriendListCbs *linphone_friend_list_cbs_ref(LinphoneFriendListCbs *cbs) {
	belle_sip_object_ref(cbs);
	return cbs;
}

void linphone_friend_list_cbs_unref(LinphoneFriendListCbs *cbs) {
	belle_sip_object_unref(cbs);
}

void *linphone_friend_list_cbs_get_user_data(const LinphoneFriendListCbs *cbs) {
	return cbs->user_data;
}

void linphone_friend_list_cbs_set_user_data(LinphoneFriendListCbs *cbs, void *ud) {
	cbs->user_data = ud;
}

LinphoneFriendListCbsContactCreatedCb linphone_friend_list_cbs_get_contact_created(const LinphoneFriendListCbs *cbs) {
	return cbs->contact_created_cb;
}

void linphone_friend_list_cbs_set_contact_created(LinphoneFriendListCbs *cbs,
                                                  LinphoneFriendListCbsContactCreatedCb cb) {
	cbs->contact_created_cb = cb;
}

LinphoneFriendListCbsContactDeletedCb linphone_friend_list_cbs_get_contact_deleted(const LinphoneFriendListCbs *cbs) {
	return cbs->contact_deleted_cb;
}

void linphone_friend_list_cbs_set_contact_deleted(LinphoneFriendListCbs *cbs,
                                                  LinphoneFriendListCbsContactDeletedCb cb) {
	cbs->contact_deleted_cb = cb;
}

LinphoneFriendListCbsContactUpdatedCb linphone_friend_list_cbs_get_contact_updated(const LinphoneFriendListCbs *cbs) {
	return cbs->contact_updated_cb;
}

void linphone_friend_list_cbs_set_contact_updated(LinphoneFriendListCbs *cbs,
                                                  LinphoneFriendListCbsContactUpdatedCb cb) {
	cbs->contact_updated_cb = cb;
}

LinphoneFriendListCbsSyncStateChangedCb
linphone_friend_list_cbs_get_sync_status_changed(const LinphoneFriendListCbs *cbs) {
	return cbs->sync_state_changed_cb;
}

void linphone_friend_list_cbs_set_sync_status_changed(LinphoneFriendListCbs *cbs,
                                                      LinphoneFriendListCbsSyncStateChangedCb cb) {
	cbs->sync_state_changed_cb = cb;
}

LinphoneFriendListCbsPresenceReceivedCb
linphone_friend_list_cbs_get_presence_received(const LinphoneFriendListCbs *cbs) {
	return cbs->presence_received_cb;
}

void linphone_friend_list_cbs_set_presence_received(LinphoneFriendListCbs *cbs,
                                                    LinphoneFriendListCbsPresenceReceivedCb cb) {
	cbs->presence_received_cb = cb;
}

#ifdef HAVE_XML2

#if defined(HAVE_SQLITE) && defined(VCARD_ENABLED)
static void carddav_done(LinphoneCardDavContext *cdc, bool_t success, const char *msg);
#endif // defined(HAVE_SQLITE) && defined(VCARD_ENABLED)

static int add_uri_entry(xmlTextWriterPtr writer, int err, const char *uri) {
	if (err >= 0) {
		err = xmlTextWriterStartElement(writer, (const xmlChar *)"entry");
	}
	if (err >= 0) {
		err = xmlTextWriterWriteAttribute(writer, (const xmlChar *)"uri", (const xmlChar *)uri);
	}
	if (err >= 0) {
		/* Close the "entry" element. */
		err = xmlTextWriterEndElement(writer);
	}
	return err;
}

static char *create_resource_list_xml(const LinphoneFriendList *list) {
	char *xml_content = NULL;
	xmlBufferPtr buf;
	xmlTextWriterPtr writer;
	int err;

	if (list->friends_map_uri == NULL) return NULL;

	bctbx_iterator_t *it = bctbx_map_cchar_begin(list->friends_map_uri);
	bctbx_iterator_t *end = bctbx_map_cchar_end(list->friends_map_uri);
	if (bctbx_iterator_cchar_equals(it, end)) {
		ms_warning("%s: Empty list in subscription, ignored.", __FUNCTION__);
		return NULL;
	}

	buf = xmlBufferCreate();
	if (buf == NULL) {
		ms_error("%s: Error creating the XML buffer", __FUNCTION__);
		return NULL;
	}
	writer = xmlNewTextWriterMemory(buf, 0);
	if (writer == NULL) {
		ms_error("%s: Error creating the XML writer", __FUNCTION__);
		return NULL;
	}

	xmlTextWriterSetIndent(writer, 1);
	err = xmlTextWriterStartDocument(writer, "1.0", "UTF-8", NULL);
	if (err >= 0) {
		err = xmlTextWriterStartElementNS(writer, NULL, (const xmlChar *)"resource-lists",
		                                  (const xmlChar *)"urn:ietf:params:xml:ns:resource-lists");
	}
	if (err >= 0) {
		err = xmlTextWriterWriteAttributeNS(writer, (const xmlChar *)"xmlns", (const xmlChar *)"xsi", NULL,
		                                    (const xmlChar *)"http://www.w3.org/2001/XMLSchema-instance");
	}

	if (err >= 0) {
		err = xmlTextWriterStartElement(writer, (const xmlChar *)"list");
	}

	const char *prev = NULL;
	while (!bctbx_iterator_cchar_equals(it, end)) {
		const bctbx_pair_t *pair = bctbx_iterator_cchar_get_pair(it);
		const char *uri = bctbx_pair_cchar_get_first(reinterpret_cast<const bctbx_pair_cchar_t *>(pair));
		if (prev == NULL || strcmp(prev, uri) != 0) {
			// Map is sorted, prevent duplicates
			err = add_uri_entry(writer, err, uri);
		}
		prev = uri;
		it = bctbx_iterator_cchar_get_next(it);
	}
	bctbx_iterator_cchar_delete(it);
	bctbx_iterator_cchar_delete(end);

	if (err >= 0) {
		/* Close the "list" element. */
		err = xmlTextWriterEndElement(writer);
	}

	if (err >= 0) {
		/* Close the "resource-lists" element. */
		err = xmlTextWriterEndElement(writer);
	}
	if (err >= 0) {
		err = xmlTextWriterEndDocument(writer);
	}
	if (err > 0) {
		/* xmlTextWriterEndDocument returns the size of the content. */
		xml_content = ms_strdup((char *)buf->content);
	}
	xmlFreeTextWriter(writer);
	xmlBufferFree(buf);

	return xml_content;
}

static void linphone_friend_presence_received(LinphoneFriendList *list,
                                              LinphoneFriend *lf,
                                              const char *uri,
                                              LinphonePresenceModel *presence) {
	lf->presence_received = TRUE;
	const char *phone_number = linphone_friend_sip_uri_to_phone_number(lf, uri);
	if (phone_number) {
		char *presence_address = linphone_presence_model_get_contact(presence);
		bctbx_iterator_t *it = bctbx_map_cchar_find_key(list->friends_map_uri, presence_address);
		bctbx_iterator_t *end = bctbx_map_cchar_end(list->friends_map_uri);
		bool_t found_friend_with_phone = FALSE;

		// Map is sorted, check if next entry matches key otherwise stop
		while (!found_friend_with_phone && !bctbx_iterator_cchar_equals(it, end)) {
			bctbx_pair_t *pair = bctbx_iterator_cchar_get_pair(it);
			const char *key = bctbx_pair_cchar_get_first(reinterpret_cast<bctbx_pair_cchar_t *>(pair));
			if (!key || strcmp(presence_address, key) != 0) break;
			LinphoneFriend *lf2 = (LinphoneFriend *)bctbx_pair_cchar_get_second(pair);
			if (lf2 == lf) {
				found_friend_with_phone = TRUE;
			}
			it = bctbx_iterator_cchar_get_next(it);
		}
		bctbx_iterator_cchar_delete(it);
		bctbx_iterator_cchar_delete(end);

		if (!found_friend_with_phone) {
			bctbx_pair_t *pair = (bctbx_pair_t *)bctbx_pair_cchar_new(presence_address, linphone_friend_ref(lf));
			bctbx_map_cchar_insert_and_delete(list->friends_map_uri, pair);
		}
		linphone_friend_set_presence_model_for_uri_or_tel(lf, phone_number, presence);
		linphone_core_notify_notify_presence_received_for_uri_or_tel(list->lc, lf, phone_number, presence);
	} else {
		linphone_friend_set_presence_model_for_uri_or_tel(lf, uri, presence);
		linphone_core_notify_notify_presence_received_for_uri_or_tel(list->lc, lf, uri, presence);
	}

	linphone_friend_notify_presence_received(lf);

	// Deprecated
	linphone_core_notify_notify_presence_received(list->lc, lf);
}

static void linphone_friend_list_parse_multipart_related_body(LinphoneFriendList *list,
                                                              const LinphoneContent *body,
                                                              const char *first_part_body) {
	xmlparsing_context_t *xml_ctx = linphone_xmlparsing_context_new();
	xmlSetGenericErrorFunc(xml_ctx, linphone_xmlparsing_genericxml_error);
	xml_ctx->doc = xmlReadDoc((const unsigned char *)first_part_body, 0, NULL, 0);
	if (xml_ctx->doc) {
		LinphoneFriend *lf;
		LinphoneContent *presence_part;
		xmlXPathObjectPtr resource_object;
		xmlXPathObjectPtr name_object;
		char *version_str = NULL;
		char *full_state_str = NULL;
		char *uri = NULL;
		bool_t full_state = FALSE;
		int version;
		int i;
		bctbx_list_t *list_friends_presence_received = NULL;
		LinphoneFriendListCbs *list_cbs = linphone_friend_list_get_callbacks(list);

		if (linphone_create_xml_xpath_context(xml_ctx) < 0) goto end;
		xmlXPathRegisterNs(xml_ctx->xpath_ctx, (const xmlChar *)"rlmi", (const xmlChar *)"urn:ietf:params:xml:ns:rlmi");

		version_str = linphone_get_xml_attribute_text_content(xml_ctx, "/rlmi:list", "version");
		if (!version_str) {
			ms_warning("rlmi+xml: No version attribute in list");
			goto end;
		}
		version = atoi(version_str);
		linphone_free_xml_text_content(version_str);
		if (version < list->expected_notification_version) { /*no longuer an error as dialog may be silently restarting
			                                                    by the refresher*/
			ms_warning("rlmi+xml: Received notification with version %d expected was %d, dialog may have been reseted",
			           version, list->expected_notification_version);
		}

		full_state_str = linphone_get_xml_attribute_text_content(xml_ctx, "/rlmi:list", "fullState");
		if (!full_state_str) {
			ms_warning("rlmi+xml: No fullState attribute in list");
			goto end;
		}
		if ((strcmp(full_state_str, "true") == 0) || (strcmp(full_state_str, "1") == 0)) {
			bctbx_list_t *l = list->friends;
			for (; l != NULL; l = bctbx_list_next(l)) {
				lf = (LinphoneFriend *)bctbx_list_get_data(l);
				linphone_friend_clear_presence_models(lf);
			}
			full_state = TRUE;
		}
		linphone_free_xml_text_content(full_state_str);
		if ((list->expected_notification_version == 0) && !full_state) {
			ms_warning("rlmi+xml: Notification with version 0 is not full state, this is not valid");
			goto end;
		}
		list->expected_notification_version = version + 1;

		name_object = linphone_get_xml_xpath_object_for_node_list(xml_ctx, "/rlmi:list/rlmi:resource/rlmi:name/..");
		if (name_object && name_object->nodesetval) {
			for (i = 1; i <= name_object->nodesetval->nodeNr; i++) {
				char *name = NULL;
				LinphoneAddress *addr;
				linphone_xml_xpath_context_set_node(xml_ctx, xmlXPathNodeSetItem(name_object->nodesetval, i - 1));
				name = linphone_get_xml_text_content(xml_ctx, "./rlmi:name");
				uri = linphone_get_xml_text_content(xml_ctx, "./@uri");
				if (!uri) continue;
				addr = linphone_address_new(uri);
				if (!addr) continue;
				lf = linphone_friend_list_find_friend_by_address(list, addr);
				linphone_address_unref(addr);
				if (!lf && list->bodyless_subscription) {
					lf = linphone_core_create_friend_with_address(list->lc, uri);
					linphone_friend_list_add_friend(list, lf);
					linphone_friend_unref(lf);
				}
				if (name) {
					linphone_friend_set_name(lf, name);
					linphone_free_xml_text_content(name);
				}
			}
		}
		if (name_object) xmlXPathFreeObject(name_object);

		bctbx_list_t *parts = linphone_content_get_parts(body);

		resource_object = linphone_get_xml_xpath_object_for_node_list(
		    xml_ctx, "/rlmi:list/rlmi:resource/rlmi:instance[@state=\"active\"]/..");
		if (resource_object && resource_object->nodesetval) {
			for (i = 1; i <= resource_object->nodesetval->nodeNr; i++) {
				char *cid = NULL;
				linphone_xml_xpath_context_set_node(xml_ctx, xmlXPathNodeSetItem(resource_object->nodesetval, i - 1));
				cid = linphone_get_xml_text_content(xml_ctx, "./rlmi:instance/@cid");
				if (cid) {
					presence_part = nullptr;
					bctbx_list_t *it = parts;
					while (it != nullptr) {
						LinphoneContent *content = (LinphoneContent *)it->data;
						const char *header = linphone_content_get_custom_header(content, "Content-Id");
						if (header && strcmp(header, cid) == 0) {
							presence_part = linphone_content_ref(content);
							break;
						}
						it = bctbx_list_next(it);
					}

					if (!presence_part) {
						ms_warning("rlmi+xml: Cannot find part with Content-Id: %s", cid);
					} else {
						SalPresenceModel *presence = NULL;
						linphone_notify_parse_presence(linphone_content_get_type(presence_part),
						                               linphone_content_get_subtype(presence_part),
						                               linphone_content_get_utf8_text(presence_part), &presence);
						if (presence) {
							// Try to reduce CPU cost of linphone_address_new and find_friend_by_address by only doing
							// it when we know for sure we have a presence to notify
							LinphoneAddress *addr;
							uri = linphone_get_xml_text_content(xml_ctx, "./@uri");
							if (!uri) continue;
							addr = linphone_address_new(uri);
							if (!addr) continue;
							linphone_free_xml_text_content(uri);

							// Clean the URI
							if (linphone_address_has_uri_param(addr, "gr")) {
								linphone_address_remove_uri_param(addr, "gr");
							}
							uri = linphone_address_as_string_uri_only(addr);
							linphone_address_unref(addr);

							bctbx_iterator_t *it = bctbx_map_cchar_find_key(list->friends_map_uri, uri);
							bctbx_iterator_t *end = bctbx_map_cchar_end(list->friends_map_uri);
							if (bctbx_iterator_cchar_equals(it, end)) {
								if (list->bodyless_subscription) {
									lf = linphone_core_create_friend_with_address(list->lc, uri);
									linphone_friend_list_add_friend(list, lf);
									linphone_friend_unref(lf);

									linphone_friend_presence_received(list, lf, uri, (LinphonePresenceModel *)presence);
									list_friends_presence_received =
									    bctbx_list_prepend(list_friends_presence_received, lf);
								}
							} else {
								// Map is sorted, check if next entry matches key otherwise stop
								while (!bctbx_iterator_cchar_equals(it, end)) {
									bctbx_pair_t *pair = bctbx_iterator_cchar_get_pair(it);
									const char *key =
									    bctbx_pair_cchar_get_first(reinterpret_cast<bctbx_pair_cchar_t *>(pair));
									if (!key || strcmp(uri, key) != 0) break;
									lf = (LinphoneFriend *)bctbx_pair_cchar_get_second(pair);
									if (lf) {
										linphone_friend_presence_received(list, lf, uri,
										                                  (LinphonePresenceModel *)presence);
										list_friends_presence_received =
										    bctbx_list_prepend(list_friends_presence_received, lf);
									}
									it = bctbx_iterator_cchar_get_next(it);
								}
							}
							bctbx_iterator_cchar_delete(it);
							bctbx_iterator_cchar_delete(end);

							linphone_content_unref(presence_part);
							linphone_presence_model_unref((LinphonePresenceModel *)presence);
							ms_free(uri);
						}
					}
					linphone_free_xml_text_content(cid);
				}
			}
			// Notify list with all friends for which we received presence information
			if (bctbx_list_size(list_friends_presence_received) > 0) {
				if (list_cbs && linphone_friend_list_cbs_get_presence_received(list_cbs)) {
					linphone_friend_list_cbs_get_presence_received(list_cbs)(list, list_friends_presence_received);
				}

				NOTIFY_IF_EXIST(PresenceReceived, presence_received, list, list_friends_presence_received)
			}
			bctbx_list_free(list_friends_presence_received);
		}

		bctbx_list_free_with_data(parts, (void (*)(void *))linphone_content_unref);
		if (resource_object) xmlXPathFreeObject(resource_object);
	} else {
		ms_warning("Wrongly formatted rlmi+xml body: %s", xml_ctx->errorBuffer);
	}

end:
	linphone_xmlparsing_context_destroy(xml_ctx);
}

#else /* HAVE_XML2 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* stubs */
static void linphone_friend_list_parse_multipart_related_body(LinphoneFriendList *list,
                                                              const LinphoneContent *body,
                                                              const char *first_part_body) {
	ms_warning("linphone_friend_list_parse_multipart_related_body() is stubbed.");
}

static char *create_resource_list_xml(const LinphoneFriendList *list) {
	ms_warning("create_resource_list_xml() is stubbed.");
	return NULL;
}

#pragma GCC diagnostic pop

#endif

static bool_t linphone_friend_list_has_subscribe_inactive(const LinphoneFriendList *list) {
	if (list->bodyless_subscription) return TRUE;

	bctbx_list_t *l = list->friends;
	bool_t has_subscribe_inactive = FALSE;
	for (; l != NULL; l = bctbx_list_next(l)) {
		LinphoneFriend *lf = (LinphoneFriend *)bctbx_list_get_data(l);
		if (lf->subscribe_active != TRUE) {
			has_subscribe_inactive = TRUE;
			break;
		}
	}
	return has_subscribe_inactive;
}

static LinphoneFriendList *linphone_friend_list_new(void) {
	LinphoneFriendList *list = belle_sip_object_new(LinphoneFriendList);
	list->cbs = linphone_friend_list_cbs_new();
	list->enable_subscriptions = FALSE;
	list->friends_map = bctbx_mmap_cchar_new();
	list->friends_map_uri = bctbx_mmap_cchar_new();
	list->bodyless_subscription = FALSE;
	list->type = LinphoneFriendListTypeDefault;
	list->revision = 0;
	return list;
}

static void linphone_friend_list_destroy(LinphoneFriendList *list) {
	if (list->display_name != NULL) ms_free(list->display_name);
	if (list->rls_addr) linphone_address_unref(list->rls_addr);
	if (list->rls_uri != NULL) ms_free(list->rls_uri);
	if (list->content_digest != NULL) ms_free(list->content_digest);
	if (list->event != NULL) {
		linphone_event_terminate(list->event);
		linphone_event_unref(list->event);
		list->event = NULL;
	}
	if (list->uri != NULL) ms_free(list->uri);
	if (list->cbs) linphone_friend_list_cbs_unref(list->cbs);
	bctbx_list_free_with_data(list->callbacks, (bctbx_list_free_func)linphone_friend_list_cbs_unref);
	list->callbacks = nullptr;
	if (list->dirty_friends_to_update)
		list->dirty_friends_to_update =
		    bctbx_list_free_with_data(list->dirty_friends_to_update, (void (*)(void *))linphone_friend_unref);
	if (list->friends)
		list->friends = bctbx_list_free_with_data(list->friends, (void (*)(void *))_linphone_friend_release);
	if (list->friends_map)
		bctbx_mmap_cchar_delete_with_data(list->friends_map, (void (*)(void *))linphone_friend_unref);
	if (list->friends_map_uri)
		bctbx_mmap_cchar_delete_with_data(list->friends_map_uri, (void (*)(void *))linphone_friend_unref);
}

BELLE_SIP_DECLARE_NO_IMPLEMENTED_INTERFACES(LinphoneFriendList);

BELLE_SIP_INSTANCIATE_VPTR(LinphoneFriendList,
                           belle_sip_object_t,
                           (belle_sip_object_destroy_t)linphone_friend_list_destroy,
                           NULL, // clone
                           NULL, // marshal
                           FALSE);

LinphoneFriendList *linphone_core_create_friend_list(LinphoneCore *lc) {
	LinphoneFriendList *list = linphone_friend_list_new();
	list->lc = lc;
	if (lc) { // Will be NULL if created from database
		list->enable_subscriptions = linphone_core_is_friend_list_subscription_enabled(lc);
	}
	return list;
}

LinphoneFriendList *linphone_friend_list_ref(LinphoneFriendList *list) {
	belle_sip_object_ref(list);
	return list;
}

void _linphone_friend_list_release(LinphoneFriendList *list) {
	/*drops all references to core and unref*/
	list->lc = NULL;
	if (list->event != NULL) {
		linphone_event_unref(list->event);
		list->event = NULL;
	}
	if (list->cbs) {
		linphone_friend_list_cbs_unref(list->cbs);
		list->cbs = NULL;
	}
	if (list->dirty_friends_to_update) {
		list->dirty_friends_to_update =
		    bctbx_list_free_with_data(list->dirty_friends_to_update, (void (*)(void *))linphone_friend_unref);
	}
	if (list->friends) {
		list->friends = bctbx_list_free_with_data(list->friends, (void (*)(void *))_linphone_friend_release);
	}
	linphone_friend_list_unref(list);
}

void linphone_friend_list_unref(LinphoneFriendList *list) {
	belle_sip_object_unref(list);
}

void *linphone_friend_list_get_user_data(const LinphoneFriendList *list) {
	return list->user_data;
}

void linphone_friend_list_set_user_data(LinphoneFriendList *list, void *ud) {
	list->user_data = ud;
}

void linphone_friend_list_set_type(LinphoneFriendList *list, LinphoneFriendListType type) {
	list->type = type;
}

LinphoneFriendListType linphone_friend_list_get_type(LinphoneFriendList *list) {
	return list->type;
}

const char *linphone_friend_list_get_display_name(const LinphoneFriendList *list) {
	return list->display_name;
}

void linphone_friend_list_set_display_name(LinphoneFriendList *list, const char *display_name) {
	if (list->display_name != NULL) {
		ms_free(list->display_name);
		list->display_name = NULL;
	}
	if (display_name != NULL) {
		list->display_name = ms_strdup(display_name);
		linphone_core_store_friends_list_in_db(list->lc, list);
	}
}

LinphoneAddress *linphone_friend_list_get_rls_address(const LinphoneFriendList *list) {
	return list->rls_addr;
}
LinphoneAddress *_linphone_friend_list_get_rls_address(const LinphoneFriendList *list) {
	if (list->rls_addr) return list->rls_addr;
	else if (list->lc) {
		const char *rls_uri = linphone_config_get_string(list->lc->config, "sip", "rls_uri", NULL);
		if (list->lc->default_rls_addr) linphone_address_unref(list->lc->default_rls_addr);

		list->lc->default_rls_addr = NULL;

		if (rls_uri) {
			/*to make sure changes in config are used if any*/
			list->lc->default_rls_addr = linphone_address_new(rls_uri);
		}

		return list->lc->default_rls_addr;
	} else return NULL;
}
void linphone_friend_list_set_rls_address(LinphoneFriendList *list, const LinphoneAddress *rls_addr) {
	LinphoneAddress *new_rls_addr = rls_addr ? linphone_address_clone(rls_addr) : NULL;

	if (list->rls_addr) {
		linphone_address_unref(list->rls_addr);
	}
	list->rls_addr = new_rls_addr;
	if (list->rls_uri != NULL) {
		ms_free(list->rls_uri);
		list->rls_uri = NULL;
	}
	if (list->rls_addr) {
		list->rls_uri = linphone_address_as_string(list->rls_addr);
		linphone_core_store_friends_list_in_db(list->lc, list);
	}
}

const char *linphone_friend_list_get_rls_uri(const LinphoneFriendList *list) {
	return list->rls_uri;
}

void linphone_friend_list_set_rls_uri(LinphoneFriendList *list, const char *rls_uri) {
	LinphoneAddress *addr = rls_uri ? linphone_core_create_address(list->lc, rls_uri) : NULL;
	linphone_friend_list_set_rls_address(list, addr);
	if (addr) linphone_address_unref(addr);
}

static LinphoneFriendListStatus
_linphone_friend_list_add_friend(LinphoneFriendList *list, LinphoneFriend *lf, bool_t synchronize) {
	LinphoneFriendListStatus status = LinphoneFriendListInvalidFriend;
	const LinphoneAddress *addr;

	if (!list || lf->friend_list) {
		if (!list) ms_error("linphone_friend_list_add_friend(): invalid list, null");
		if (lf->friend_list) ms_error("linphone_friend_list_add_friend(): invalid friend, already in list");
		return status;
	}

	addr = linphone_friend_get_address(lf);
	if (addr == NULL && linphone_friend_get_vcard(lf) == NULL && linphone_friend_get_phone_numbers(lf) == NULL) {
		ms_error("linphone_friend_list_add_friend(): invalid friend, no vCard, SIP URI or phone number");
		return status;
	}

	bool_t present = FALSE;
	if (lf->refkey) {
		present = linphone_friend_list_find_friend_by_ref_key(list, lf->refkey) != NULL;
	} else {
		present = bctbx_list_find(list->friends, lf) != NULL;
	}
	if (present) {
		char *tmp = NULL;
		if (addr) tmp = linphone_address_as_string(addr);
		ms_warning("Friend %s already in list [%s], ignored.", tmp ? tmp : "unknown", list->display_name);
		if (tmp) ms_free(tmp);
	} else {
		status = linphone_friend_list_import_friend(list, lf, synchronize);
		linphone_friend_save(lf, lf->lc);
	}

	if (!list->rls_uri) // Mimic the behaviour of linphone_core_add_friend() when a resource list server is not in use
		linphone_friend_apply(lf, lf->lc);

	return status;
}

LinphoneFriendListStatus linphone_friend_list_add_friend(LinphoneFriendList *list, LinphoneFriend *lf) {
	return _linphone_friend_list_add_friend(list, lf, TRUE);
}

LinphoneFriendListStatus linphone_friend_list_add_local_friend(LinphoneFriendList *list, LinphoneFriend *lf) {
	return _linphone_friend_list_add_friend(list, lf, FALSE);
}

void linphone_friend_list_invalidate_friends_maps(LinphoneFriendList *list) {
	if (list->friends_map)
		bctbx_mmap_cchar_delete_with_data(list->friends_map, (void (*)(void *))linphone_friend_unref);
	list->friends_map = bctbx_mmap_cchar_new();
	if (list->friends_map_uri)
		bctbx_mmap_cchar_delete_with_data(list->friends_map_uri, (void (*)(void *))linphone_friend_unref);
	list->friends_map_uri = bctbx_mmap_cchar_new();

	const bctbx_list_t *elem;
	for (elem = list->friends; elem != NULL; elem = bctbx_list_next(elem)) {
		LinphoneFriend *lf = (LinphoneFriend *)bctbx_list_get_data(elem);
		linphone_friend_add_addresses_and_numbers_into_maps(lf, list);
	}
}

LinphoneFriendListStatus
linphone_friend_list_import_friend(LinphoneFriendList *list, LinphoneFriend *lf, bool_t synchronize) {
	if (lf->friend_list) {
		ms_error("linphone_friend_list_add_friend(): invalid friend, already in list");
		return LinphoneFriendListInvalidFriend;
	}

	lf->friend_list = list;
	lf->lc = list->lc;
	list->friends = bctbx_list_prepend(list->friends, linphone_friend_ref(lf));
	linphone_friend_add_addresses_and_numbers_into_maps(lf, list);

	if (synchronize) {
		list->dirty_friends_to_update = bctbx_list_prepend(list->dirty_friends_to_update, linphone_friend_ref(lf));
	}
	return LinphoneFriendListOK;
}

#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif // _MSC_VER
static LinphoneFriendListStatus
_linphone_friend_list_remove_friend(LinphoneFriendList *list, LinphoneFriend *lf, bool_t remove_from_server) {
	bctbx_list_t *iterator;
	bctbx_list_t *phone_numbers;
	const bctbx_list_t *addresses;
	bctbx_list_t *elem = bctbx_list_find(list->friends, lf);
	if (elem == NULL) return LinphoneFriendListNonExistentFriend;

#if defined(HAVE_SQLITE) && defined(VCARD_ENABLED)

	if (lf && lf->lc && lf->lc->friends_db) {
		linphone_core_remove_friend_from_db(lf->lc, lf);
	}

	if (remove_from_server) {
		LinphoneVcard *lvc = linphone_friend_get_vcard(lf);
		if (lvc && linphone_vcard_get_uid(lvc)) {
			LinphoneCardDavContext *cdc = linphone_carddav_context_new(list);
			if (cdc) {
				cdc->sync_done_cb = carddav_done;
				if (cdc->friend_list->cbs->sync_state_changed_cb) {
					cdc->friend_list->cbs->sync_state_changed_cb(cdc->friend_list, LinphoneFriendListSyncStarted, NULL);
				}
				NOTIFY_IF_EXIST(SyncStateChanged, sync_status_changed, list, LinphoneFriendListSyncStarted, NULL)
				linphone_carddav_delete_vcard(cdc, lf);
			}
		}
	}
#endif
	list->friends = bctbx_list_erase_link(list->friends, elem);
	if (lf->refkey) {
		bctbx_iterator_t *it = bctbx_map_cchar_find_key(list->friends_map, lf->refkey);
		bctbx_iterator_t *end = bctbx_map_cchar_end(list->friends_map);
		if (!bctbx_iterator_cchar_equals(it, end)) {
			linphone_friend_unref((LinphoneFriend *)bctbx_pair_cchar_get_second(bctbx_iterator_cchar_get_pair(it)));
			bctbx_map_cchar_erase(list->friends_map, it);
		}
		if (it) bctbx_iterator_cchar_delete(it);
		if (end) bctbx_iterator_cchar_delete(end);
	}

	phone_numbers = linphone_friend_get_phone_numbers(lf);
	iterator = phone_numbers;
	while (iterator) {
		const char *number = (const char *)bctbx_list_get_data(iterator);
		const char *uri = linphone_friend_phone_number_to_sip_uri(lf, number);
		if (uri) {
			bctbx_iterator_t *it = bctbx_map_cchar_find_key(list->friends_map_uri, uri);
			bctbx_iterator_t *end = bctbx_map_cchar_end(list->friends_map_uri);
			if (!bctbx_iterator_cchar_equals(it, end)) {
				linphone_friend_unref((LinphoneFriend *)bctbx_pair_cchar_get_second(bctbx_iterator_cchar_get_pair(it)));
				bctbx_map_cchar_erase(list->friends_map_uri, it);
			}
			if (it) bctbx_iterator_cchar_delete(it);
			if (end) bctbx_iterator_cchar_delete(end);
		}
		iterator = bctbx_list_next(iterator);
	}
	if (phone_numbers) bctbx_list_free(phone_numbers);

	addresses = linphone_friend_get_addresses(lf);
	iterator = (bctbx_list_t *)addresses;
	while (iterator) {
		LinphoneAddress *lfaddr = (LinphoneAddress *)bctbx_list_get_data(iterator);
		char *uri = linphone_address_as_string_uri_only(lfaddr);
		if (uri) {
			bctbx_iterator_t *it = bctbx_map_cchar_find_key(list->friends_map_uri, uri);
			bctbx_iterator_t *end = bctbx_map_cchar_end(list->friends_map_uri);
			if (!bctbx_iterator_cchar_equals(it, end)) {
				linphone_friend_unref((LinphoneFriend *)bctbx_pair_cchar_get_second(bctbx_iterator_cchar_get_pair(it)));
				bctbx_map_cchar_erase(list->friends_map_uri, it);
			}
			if (it) bctbx_iterator_cchar_delete(it);
			if (end) bctbx_iterator_cchar_delete(end);
			ms_free(uri);
		}

		iterator = bctbx_list_next(iterator);
	}

	lf->friend_list = NULL;
	linphone_friend_unref(lf);
	return LinphoneFriendListOK;
}
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif // _MSC_VER

LinphoneFriendListStatus linphone_friend_list_remove_friend(LinphoneFriendList *list, LinphoneFriend *lf) {
	return _linphone_friend_list_remove_friend(list, lf, TRUE);
}

const bctbx_list_t *linphone_friend_list_get_friends(const LinphoneFriendList *list) {
	return list->friends;
}

#if defined(HAVE_SQLITE) && defined(VCARD_ENABLED)

static void carddav_done(LinphoneCardDavContext *cdc, bool_t success, const char *msg) {
	LinphoneFriendList *list = cdc->friend_list;
	if (cdc && cdc->friend_list->cbs && cdc->friend_list->cbs->sync_state_changed_cb) {
		cdc->friend_list->cbs->sync_state_changed_cb(
		    cdc->friend_list, success ? LinphoneFriendListSyncSuccessful : LinphoneFriendListSyncFailure, msg);
	}
	NOTIFY_IF_EXIST(SyncStateChanged, sync_status_changed, list,
	                success ? LinphoneFriendListSyncSuccessful : LinphoneFriendListSyncFailure, msg)
	linphone_carddav_context_destroy(cdc);
}

void linphone_friend_list_update_dirty_friends(LinphoneFriendList *list) {
	bctbx_list_t *dirty_friends = list->dirty_friends_to_update;

	while (dirty_friends) {
		LinphoneCardDavContext *cdc = linphone_carddav_context_new(list);
		if (cdc) {
			LinphoneFriend *lf = (LinphoneFriend *)bctbx_list_get_data(dirty_friends);
			cdc->sync_done_cb = carddav_done;
			if (lf) {
				if (cdc->friend_list->cbs->sync_state_changed_cb) {
					cdc->friend_list->cbs->sync_state_changed_cb(cdc->friend_list, LinphoneFriendListSyncStarted, NULL);
				}
				NOTIFY_IF_EXIST(SyncStateChanged, sync_status_changed, list, LinphoneFriendListSyncStarted, NULL)
				linphone_carddav_put_vcard(cdc, lf);
			}
		}
		dirty_friends = bctbx_list_next(dirty_friends);
	}
	list->dirty_friends_to_update =
	    bctbx_list_free_with_data(list->dirty_friends_to_update, (void (*)(void *))linphone_friend_unref);
}

static void carddav_created(LinphoneCardDavContext *cdc, LinphoneFriend *lf) {
	if (cdc) {
		LinphoneFriendList *list = cdc->friend_list;
		linphone_friend_list_import_friend(list, lf, FALSE);
		if (cdc->friend_list->cbs->contact_created_cb) {
			cdc->friend_list->cbs->contact_created_cb(list, lf);
		}
		NOTIFY_IF_EXIST(ContactCreated, contact_created, list, lf)
	}
}

static void carddav_removed(LinphoneCardDavContext *cdc, LinphoneFriend *lf) {
	if (cdc) {
		LinphoneFriendList *list = cdc->friend_list;
		_linphone_friend_list_remove_friend(list, lf, FALSE);
		if (cdc->friend_list->cbs->contact_deleted_cb) {
			cdc->friend_list->cbs->contact_deleted_cb(list, lf);
		}
		NOTIFY_IF_EXIST(ContactDeleted, contact_deleted, list, lf)
	}
}

static void carddav_updated(LinphoneCardDavContext *cdc, LinphoneFriend *lf_new, LinphoneFriend *lf_old) {
	if (cdc) {
		LinphoneFriendList *list = cdc->friend_list;
		bctbx_list_t *elem = bctbx_list_find(list->friends, lf_old);
		if (elem) {
			elem->data = linphone_friend_ref(lf_new);
		}
		linphone_core_store_friend_in_db(lf_new->lc, lf_new);

		if (cdc->friend_list->cbs->contact_updated_cb) {
			cdc->friend_list->cbs->contact_updated_cb(list, lf_new, lf_old);
		}
		NOTIFY_IF_EXIST(ContactUpdated, contact_updated, list, lf_new, lf_old)
		linphone_friend_unref(lf_old);
	}
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

void linphone_friend_list_synchronize_friends_from_server(LinphoneFriendList *list) {
	if (!list || !list->lc) {
		ms_error("Either list or list's Core pointer is null, this is not expected!");
		return;
	}

	// Vcard4.0 list synchronisation
	if (list->type == LinphoneFriendListTypeVCard4) {
		const char *contacts_vcard_list_uri =
		    linphone_config_get_string(list->lc->config, "misc", "contacts-vcard-list", NULL);

		belle_http_request_listener_callbacks_t belle_request_listener = {0};
		belle_generic_uri_t *uri = belle_generic_uri_parse(contacts_vcard_list_uri);

		belle_request_listener.process_auth_requested = [](void *ctx, belle_sip_auth_event_t *event) {
			LinphoneFriendList *list = (LinphoneFriendList *)ctx;

			linphone_auth_info_fill_belle_sip_event(
			    _linphone_core_find_auth_info(
			        list->lc, belle_sip_auth_event_get_realm(event), belle_sip_auth_event_get_username(event),
			        belle_sip_auth_event_get_domain(event), belle_sip_auth_event_get_algorithm(event), TRUE),
			    event);
		};

		belle_request_listener.process_response = [](void *ctx, const belle_http_response_event_t *event) {
			LinphoneFriendList *list = (LinphoneFriendList *)ctx;
			const char *body = belle_sip_message_get_body(BELLE_SIP_MESSAGE(event->response));

			if (body) {
				const char *url = linphone_config_get_string(list->lc->config, "misc", "contacts-vcard-list", NULL);

				char *buf;
				/**
				 * We directly remove from the SQLite database the friends, then the friends_lists
				 * - Because we doesn't have a foreign key between the two tables
				 * - Because removing friends can only be done using linphone_friend_list_remove_friend and requires a
				 * loop
				 * - Because the primary key is id (autoincrement) we can have several friends_lists that have the same
				 * display_name
				 * - Because doing the following lines using the current C API would require to load the full
				 * friends_lists table in memory and do the where manually, then delete one by one each linked friends
				 */

				buf = sqlite3_mprintf("delete from friends where friend_list_id in (select id from friends_lists where "
				                      "display_name = %Q)",
				                      url);
				linphone_sql_request_generic(list->lc->friends_db, buf);
				sqlite3_free(buf);

				buf = sqlite3_mprintf("delete from friends_lists where display_name = %Q", url);
				linphone_sql_request_generic(list->lc->friends_db, buf);
				sqlite3_free(buf);

				/**
				 * And then we clean, clear and resync the complete database in memory
				 */
				linphone_core_friends_storage_resync_friends_lists(list->lc);

				/**
				 * And then we save the received friendlist
				 * Each of the following lines is calling linphone_core_store_friends_list_in_db
				 * So we do 4 SQL requests
				 */

				linphone_friend_list_set_uri(list, url);
				linphone_friend_list_set_display_name(list, url);
				linphone_friend_list_import_friends_from_vcard4_buffer(list, body);

				linphone_core_add_friend_list(list->lc, list);

				NOTIFY_IF_EXIST(SyncStateChanged, sync_status_changed, list, LinphoneFriendListSyncSuccessful, NULL)
			}
		};

		belle_request_listener.process_io_error = [](void *ctx, BCTBX_UNUSED(const belle_sip_io_error_event_t *event)) {
			LinphoneFriendList *list = (LinphoneFriendList *)ctx;
			NOTIFY_IF_EXIST(SyncStateChanged, sync_status_changed, list, LinphoneFriendListSyncFailure, "IO error")
		};

		belle_request_listener.process_timeout = [](void *ctx, BCTBX_UNUSED(const belle_sip_timeout_event_t *event)) {
			LinphoneFriendList *list = (LinphoneFriendList *)ctx;
			NOTIFY_IF_EXIST(SyncStateChanged, sync_status_changed, list, LinphoneFriendListSyncFailure,
			                "Timeout reached")
		};

		/**
		 * We free-up the existing listeners if the previous request was not cancelled properly
		 */
		if (list->lc->base_contacts_list_http_listener) {
			belle_sip_object_unref(list->lc->base_contacts_list_http_listener);
			list->lc->base_contacts_list_http_listener = NULL;
		}

		list->lc->base_contacts_list_http_listener =
		    belle_http_request_listener_create_from_callbacks(&belle_request_listener, list);

		belle_http_request_t *request = belle_http_request_create(
		    "GET", uri, belle_sip_header_create("User-Agent", linphone_core_get_user_agent(list->lc)), NULL);

		LinphoneProxyConfig *cfg = linphone_core_get_default_proxy_config(list->lc);
		if (cfg != nullptr) {
			char *addr = linphone_address_as_string_uri_only(linphone_proxy_config_get_identity_address(cfg));
			belle_sip_message_add_header(BELLE_SIP_MESSAGE(request), belle_http_header_create("From", addr));
			ms_free(addr);
		}

		belle_http_provider_send_request(list->lc->http_provider, request, list->lc->base_contacts_list_http_listener);
	} else if (list->type == LinphoneFriendListTypeCardDAV) {
		if (!list->uri) {
			ms_error("Can't synchronize CardDAV list [%p](%s) without an URI", list,
			         linphone_friend_list_get_display_name(list));
			return;
		}

		// CardDav synchronisation
		LinphoneCardDavContext *cdc = NULL;
		cdc = linphone_carddav_context_new(list);
		if (cdc) {
			cdc->contact_created_cb = carddav_created;
			cdc->contact_removed_cb = carddav_removed;
			cdc->contact_updated_cb = carddav_updated;
			cdc->sync_done_cb = carddav_done;
			if (cdc && cdc->friend_list->cbs->sync_state_changed_cb) {
				cdc->friend_list->cbs->sync_state_changed_cb(cdc->friend_list, LinphoneFriendListSyncStarted, NULL);
			}
			NOTIFY_IF_EXIST(SyncStateChanged, sync_status_changed, list, LinphoneFriendListSyncStarted, NULL)
			linphone_carddav_synchronize(cdc);
		} else {
			ms_error("Failed to create a CardDAV context for friend list [%p] with URI [%s]", list, list->uri);
		}
	}
}

#else

void linphone_friend_list_update_dirty_friends(BCTBX_UNUSED(LinphoneFriendList *list)) {
	ms_warning("linphone_friend_list_update_dirty_friends(): stubbed.");
}

void linphone_friend_list_synchronize_friends_from_server(BCTBX_UNUSED(LinphoneFriendList *list)) {
	ms_warning("linphone_friend_list_synchronize_friends_from_server(): stubbed.");
}

#endif

LinphoneFriend *linphone_friend_list_find_friend_by_address(const LinphoneFriendList *list,
                                                            const LinphoneAddress *address) {
	LinphoneAddress *clean_addr = linphone_address_clone(address);
	LinphoneFriend *lf;
	if (linphone_address_has_uri_param(clean_addr, "gr")) {
		linphone_address_remove_uri_param(clean_addr, "gr");
	}
	char *uri = linphone_address_as_string_uri_only(clean_addr);
	lf = linphone_friend_list_find_friend_by_uri(list, uri);
	bctbx_free(uri);
	linphone_address_unref(clean_addr);
	return lf;
}

LinphoneFriend *linphone_friend_list_find_friend_by_phone_number(const LinphoneFriendList *list,
                                                                 const char *phoneNumber) {
	LinphoneFriend *result = NULL;

	const bctbx_list_t *elem;
	for (elem = list->friends; elem != NULL; elem = bctbx_list_next(elem)) {
		LinphoneFriend *lf = (LinphoneFriend *)bctbx_list_get_data(elem);
		if (linphone_friend_has_phone_number(lf, phoneNumber)) {
			result = lf;
			break;
		}
	}

	return result;
}

bctbx_list_t *linphone_friend_list_find_friends_by_address(const LinphoneFriendList *list,
                                                           const LinphoneAddress *address) {
	LinphoneAddress *clean_addr = linphone_address_clone(address);
	bctbx_list_t *result = NULL;
	if (linphone_address_has_uri_param(clean_addr, "gr")) {
		linphone_address_remove_uri_param(clean_addr, "gr");
	}
	char *uri = linphone_address_as_string_uri_only(clean_addr);
	result = linphone_friend_list_find_friends_by_uri(list, uri);
	bctbx_free(uri);
	linphone_address_unref(clean_addr);
	return result;
}

LinphoneFriend *linphone_friend_list_find_friend_by_uri(const LinphoneFriendList *list, const char *uri) {
	LinphoneFriend *result = NULL;
	bctbx_iterator_t *it = bctbx_map_cchar_find_key(list->friends_map_uri, uri);
	bctbx_iterator_t *end = bctbx_map_cchar_end(list->friends_map_uri);
	if (!bctbx_iterator_cchar_equals(it, end)) {
		bctbx_pair_t *pair = bctbx_iterator_cchar_get_pair(it);
		result = (LinphoneFriend *)bctbx_pair_cchar_get_second(pair);
	}
	bctbx_iterator_cchar_delete(end);
	bctbx_iterator_cchar_delete(it);
	return result;
}

bctbx_list_t *linphone_friend_list_find_friends_by_uri(const LinphoneFriendList *list, const char *uri) {
	bctbx_list_t *result = NULL;
	bctbx_iterator_t *it = bctbx_map_cchar_find_key(list->friends_map_uri, uri);
	bctbx_iterator_t *end = bctbx_map_cchar_end(list->friends_map_uri);
	while (!bctbx_iterator_cchar_equals(it, end)) {
		const bctbx_pair_t *pair = bctbx_iterator_cchar_get_pair(it);
		const char *friend_uri = bctbx_pair_cchar_get_first(reinterpret_cast<const bctbx_pair_cchar_t *>(pair));
		if (uri && friend_uri && strcmp(friend_uri, uri) == 0) {
			LinphoneFriend *lf = (LinphoneFriend *)bctbx_pair_cchar_get_second(pair);
			result = bctbx_list_prepend(result, linphone_friend_ref(lf));
		} else {
			break;
		}
		it = bctbx_iterator_cchar_get_next(it);
	}
	bctbx_iterator_cchar_delete(end);
	bctbx_iterator_cchar_delete(it);
	return result;
}

LinphoneFriend *linphone_friend_list_find_friend_by_ref_key(const LinphoneFriendList *list, const char *ref_key) {
	LinphoneFriend *result = NULL;
	if (list) {
		bctbx_iterator_t *it = bctbx_map_cchar_find_key(list->friends_map, ref_key);
		bctbx_iterator_t *end = bctbx_map_cchar_end(list->friends_map);
		if (!bctbx_iterator_cchar_equals(it, end)) {
			bctbx_pair_t *pair = bctbx_iterator_cchar_get_pair(it);
			result = (LinphoneFriend *)bctbx_pair_cchar_get_second(pair);
		}
		bctbx_iterator_cchar_delete(end);
		bctbx_iterator_cchar_delete(it);
	}
	return result;
}

LinphoneFriend *linphone_friend_list_find_friend_by_inc_subscribe(const LinphoneFriendList *list,
                                                                  LinphonePrivate::SalOp *op) {
	const bctbx_list_t *elem;
	for (elem = list->friends; elem != NULL; elem = bctbx_list_next(elem)) {
		LinphoneFriend *lf = (LinphoneFriend *)bctbx_list_get_data(elem);
		if (bctbx_list_find(lf->insubs, op)) return lf;
	}
	return NULL;
}

LinphoneFriend *linphone_friend_list_find_friend_by_out_subscribe(const LinphoneFriendList *list,
                                                                  LinphonePrivate::SalOp *op) {
	const bctbx_list_t *elem;
	for (elem = list->friends; elem != NULL; elem = bctbx_list_next(elem)) {
		LinphoneFriend *lf = (LinphoneFriend *)bctbx_list_get_data(elem);
		if (lf->outsub && ((lf->outsub == op) || lf->outsub->isForkedOf(op))) return lf;
	}
	return NULL;
}

static void linphone_friend_list_close_subscriptions(LinphoneFriendList *list) {
	/* FIXME we should wait until subscription to complete. */
	if (list->event) {
		linphone_event_terminate(list->event);
		linphone_event_unref(list->event);
		list->event = NULL;
	}
	bctbx_list_for_each(list->friends, (void (*)(void *))linphone_friend_close_subscriptions);
}

static void _linphone_friend_list_send_list_subscription_with_body(LinphoneFriendList *list, LinphoneAddress *address) {
	char *xml_content = create_resource_list_xml(list);
	if (!xml_content) return;

	unsigned char digest[16];
	bctbx_md5((unsigned char *)xml_content, strlen(xml_content), digest);
	if (list->event && list->content_digest && (memcmp(list->content_digest, digest, sizeof(digest)) == 0)) {
		/* The content has not changed, only refresh the event. */
		linphone_event_refresh_subscribe(list->event);
	} else {
		LinphoneContent *content;
		bctbx_list_t *elem = NULL;
		int expires = linphone_config_get_int(list->lc->config, "sip", "rls_presence_expires", 3600);
		list->expected_notification_version = 0;
		if (list->content_digest) ms_free(list->content_digest);

		list->content_digest = reinterpret_cast<unsigned char *>(ms_malloc(sizeof(digest)));
		memcpy(list->content_digest, digest, sizeof(digest));
		if (list->event) {
			linphone_event_terminate(list->event);
			linphone_event_unref(list->event);
		}
		list->event = linphone_core_create_subscribe(list->lc, address, "presence", expires);
		linphone_event_set_internal(list->event, TRUE);
		linphone_event_add_custom_header(list->event, "Require", "recipient-list-subscribe");
		linphone_event_add_custom_header(list->event, "Supported", "eventlist");
		linphone_event_add_custom_header(list->event, "Accept",
		                                 "multipart/related, application/pidf+xml, application/rlmi+xml");
		linphone_event_add_custom_header(list->event, "Content-Disposition", "recipient-list");
		content = linphone_core_create_content(list->lc);
		linphone_content_set_type(content, "application");
		linphone_content_set_subtype(content, "resource-lists+xml");
		linphone_content_set_utf8_text(content, xml_content);
		if (linphone_core_content_encoding_supported(list->lc, "deflate")) {
			linphone_content_set_encoding(content, "deflate");
			linphone_event_add_custom_header(list->event, "Accept-Encoding", "deflate");
		}
		for (elem = list->friends; elem != NULL; elem = bctbx_list_next(elem)) {
			LinphoneFriend *lf = (LinphoneFriend *)elem->data;
			lf->subscribe_active = TRUE;
		}
		linphone_event_send_subscribe(list->event, content);
		linphone_content_unref(content);
		linphone_event_set_user_data(list->event, list);
	}
	ms_free(xml_content);
}

static void _linphone_friend_list_send_list_subscription_without_body(LinphoneFriendList *list,
                                                                      LinphoneAddress *address) {
	bctbx_list_t *elem = NULL;
	int expires = linphone_config_get_int(list->lc->config, "sip", "rls_presence_expires", 3600);
	list->expected_notification_version = 0;
	if (list->content_digest) ms_free(list->content_digest);

	if (list->event) {
		linphone_event_terminate(list->event);
		linphone_event_unref(list->event);
	}
	list->event = linphone_core_create_subscribe(list->lc, address, "presence", expires);
	linphone_event_set_internal(list->event, TRUE);
	linphone_event_add_custom_header(list->event, "Supported", "eventlist");
	linphone_event_add_custom_header(list->event, "Accept",
	                                 "multipart/related, application/pidf+xml, application/rlmi+xml");
	if (linphone_core_content_encoding_supported(list->lc, "deflate"))
		linphone_event_add_custom_header(list->event, "Accept-Encoding", "deflate");

	for (elem = list->friends; elem != NULL; elem = bctbx_list_next(elem)) {
		LinphoneFriend *lf = (LinphoneFriend *)elem->data;
		lf->subscribe_active = TRUE;
	}
	linphone_event_send_subscribe(list->event, NULL);
	linphone_event_set_user_data(list->event, list);
}

static void linphone_friend_list_send_list_subscription(LinphoneFriendList *list) {
	LinphoneAddress *address = _linphone_friend_list_get_rls_address(list);
	if (!address) {
		ms_warning("Friend list's [%p] has no RLS address, can't send subscription", list);
		return;
	}

	if (!linphone_friend_list_has_subscribe_inactive(list)) {
		ms_warning("Friend list's [%p] subscribe is inactive, can't send subscription", list);
		return;
	}

	if (list->bodyless_subscription) _linphone_friend_list_send_list_subscription_without_body(list, address);
	else _linphone_friend_list_send_list_subscription_with_body(list, address);
}

void linphone_friend_list_update_subscriptions(LinphoneFriendList *list) {
	LinphoneProxyConfig *cfg = NULL;
	LinphoneAddress *address = _linphone_friend_list_get_rls_address(list);
	bool_t only_when_registered = FALSE;
	bool_t should_send_list_subscribe = FALSE;

	ms_message("Updating friend list [%p](%s) subscriptions", list, linphone_friend_list_get_display_name(list));
	if (list->lc) {
		if (address) cfg = linphone_core_lookup_known_proxy(list->lc, address);
		only_when_registered = linphone_core_should_subscribe_friends_only_when_registered(list->lc);
		// in case of only_when_registered, proxy config is mandatory to send subscribes. Otherwise, unexpected
		// subscribtion can be issued using default contact address even if no account is configured yet.
		should_send_list_subscribe =
		    (!only_when_registered || (cfg && linphone_proxy_config_get_state(cfg) == LinphoneRegistrationOk));
	}

	if (address != NULL) {
		if (list->enable_subscriptions) {
			if (should_send_list_subscribe) {
				linphone_friend_list_send_list_subscription(list);
			} else {
				if (list->event) {
					linphone_event_terminate(list->event);
					linphone_event_unref(list->event);
					list->event = NULL;
					ms_message("Friend list [%p] subscription terminated because proxy config lost connection", list);
				} else {
					ms_message("Friend list [%p] subscription update skipped since dependant proxy config is not yet "
					           "registered",
					           list);
				}
			}
		} else {
			ms_message("Friend list [%p] subscription update skipped since subscriptions not enabled yet", list);
		}
	} else if (list->enable_subscriptions) {
		ms_message("Updating friend list's [%p] friends subscribes", list);
		const bctbx_list_t *elem;
		for (elem = list->friends; elem != NULL; elem = bctbx_list_next(elem)) {
			LinphoneFriend *lf = (LinphoneFriend *)bctbx_list_get_data(elem);
			linphone_friend_update_subscribes(lf, only_when_registered);
		}
	}
}

void linphone_friend_list_invalidate_subscriptions(LinphoneFriendList *list) {
	ms_message("Invalidating friend list's [%p] subscriptions", list);
	// Terminate subscription event
	if (list->event) {
		linphone_event_terminate(list->event);
		linphone_event_unref(list->event);
		list->event = NULL;
	}

	const bctbx_list_t *elem;
	for (elem = list->friends; elem != NULL; elem = bctbx_list_next(elem)) {
		LinphoneFriend *lf = (LinphoneFriend *)bctbx_list_get_data(elem);
		linphone_friend_invalidate_subscription(lf);
	}
}

void linphone_friend_list_notify_presence(LinphoneFriendList *list, LinphonePresenceModel *presence) {
	const bctbx_list_t *elem;
	for (elem = list->friends; elem != NULL; elem = bctbx_list_next(elem)) {
		LinphoneFriend *lf = (LinphoneFriend *)bctbx_list_get_data(elem);
		linphone_friend_notify(lf, presence);
	}
}

void linphone_friend_list_notify_presence_received(LinphoneFriendList *list,
                                                   BCTBX_UNUSED(LinphoneEvent *lev),
                                                   const LinphoneContent *body) {
	if (!linphone_content_is_multipart(body)) return;

	LinphoneContent *first_part;
	const char *type = linphone_content_get_type(body);
	const char *subtype = linphone_content_get_subtype(body);

	if ((strcmp(type, "multipart") != 0) || (strcmp(subtype, "related") != 0)) {
		ms_warning("multipart presence notified but it is not 'multipart/related', instead is '%s/%s'", type, subtype);
		return;
	}

	first_part = linphone_content_get_part(body, 0);
	if (first_part == NULL) {
		ms_warning("'multipart/related' presence notified but it doesn't contain any part");
		return;
	}

	type = linphone_content_get_type(first_part);
	subtype = linphone_content_get_subtype(first_part);
	if ((strcmp(type, "application") != 0) || (strcmp(subtype, "rlmi+xml") != 0)) {
		ms_warning("multipart presence notified but first part is not 'application/rlmi+xml'");
		linphone_content_unref(first_part);
		return;
	}

	linphone_friend_list_parse_multipart_related_body(list, body, linphone_content_get_utf8_text(first_part));
	linphone_content_unref(first_part);
}

const char *linphone_friend_list_get_uri(const LinphoneFriendList *list) {
	return list->uri;
}

void linphone_friend_list_set_uri(LinphoneFriendList *list, const char *uri) {
	if (list->uri != NULL) {
		ms_free(list->uri);
		list->uri = NULL;
	}
	if (uri != NULL) {
		list->uri = ms_strdup(uri);
		linphone_core_store_friends_list_in_db(list->lc, list);
	}
}

bool_t linphone_friend_list_is_subscription_bodyless(LinphoneFriendList *list) {
	return list->bodyless_subscription;
}

void linphone_friend_list_set_subscription_bodyless(LinphoneFriendList *list, bool_t bodyless) {
	list->bodyless_subscription = bodyless;
}

void linphone_friend_list_update_revision(LinphoneFriendList *list, int rev) {
	list->revision = rev;
	linphone_core_store_friends_list_in_db(list->lc, list);
}

void linphone_friend_list_subscription_state_changed(LinphoneCore *lc,
                                                     LinphoneEvent *lev,
                                                     LinphoneSubscriptionState state) {
	LinphoneFriendList *list = (LinphoneFriendList *)linphone_event_get_user_data(lev);
	if (!list) {
		ms_warning("core [%p] Receiving unexpected state [%s] for event [%p], no associated friend list", lc,
		           linphone_subscription_state_to_string(state), lev);
	} else {
		ms_message("Receiving new state [%s] for event [%p] for friend list [%p]",
		           linphone_subscription_state_to_string(state), lev, list);

		if (state == LinphoneSubscriptionOutgoingProgress && linphone_event_get_reason(lev) == LinphoneReasonNoMatch) {
			ms_message("Reseting version count for friend list [%p]", list);
			list->expected_notification_version = 0;
		}
	}
}

LinphoneCore *linphone_friend_list_get_core(const LinphoneFriendList *list) {
	return list->lc;
}

static LinphoneStatus linphone_friend_list_import_friends_from_vcard4(LinphoneFriendList *list, bctbx_list_t *vcards) {
	bctbx_list_t *vcards_iterator = NULL;
	int count = 0;

	if (!linphone_core_vcard_supported()) {
		ms_error("vCard support wasn't enabled at compilation time");
		return -1;
	}
	if (!list) {
		ms_error("Can't import into a NULL list");
		return -1;
	}

	vcards_iterator = vcards;

	while (vcards_iterator != NULL && bctbx_list_get_data(vcards_iterator) != NULL) {
		LinphoneVcard *vcard = (LinphoneVcard *)bctbx_list_get_data(vcards_iterator);
		LinphoneFriend *lf = linphone_friend_new_from_vcard(vcard);
		linphone_vcard_unref(vcard);
		if (lf) {
			if (LinphoneFriendListOK == linphone_friend_list_import_friend(list, lf, TRUE)) {
				linphone_friend_save(lf, lf->lc);
				count++;
			}
			linphone_friend_unref(lf);
		}
		vcards_iterator = bctbx_list_next(vcards_iterator);
	}
	bctbx_list_free(vcards);
	linphone_core_store_friends_list_in_db(list->lc, list);
	return count;
}
LinphoneStatus linphone_friend_list_import_friends_from_vcard4_file(LinphoneFriendList *list, const char *vcard_file) {
	bctbx_list_t *vcards = NULL;

	if (!linphone_core_vcard_supported()) {
		ms_error("vCard support wasn't enabled at compilation time");
		return -1;
	}
	if (!list) {
		ms_error("Can't import into a NULL list");
		return -1;
	}

	vcards = linphone_vcard_context_get_vcard_list_from_file(list->lc->vcard_context, vcard_file);
	if (!vcards) {
		ms_error("Failed to parse the file %s", vcard_file);
		return -1;
	}
	return linphone_friend_list_import_friends_from_vcard4(list, vcards);
}

LinphoneStatus linphone_friend_list_import_friends_from_vcard4_buffer(LinphoneFriendList *list,
                                                                      const char *vcard_buffer) {
	bctbx_list_t *vcards = NULL;

	if (!linphone_core_vcard_supported()) {
		ms_error("vCard support wasn't enabled at compilation time");
		return -1;
	}
	if (!list) {
		ms_error("Can't import into a NULL list");
		return -1;
	}

	vcards = linphone_vcard_context_get_vcard_list_from_buffer(list->lc->vcard_context, vcard_buffer);
	if (!vcards) {
		ms_error("Failed to parse the buffer");
		return -1;
	}

	return linphone_friend_list_import_friends_from_vcard4(list, vcards);
}

void linphone_friend_list_export_friends_as_vcard4_file(LinphoneFriendList *list, const char *vcard_file) {
	FILE *file = NULL;
	const bctbx_list_t *friends;

	if (!linphone_core_vcard_supported()) {
		ms_error("vCard support wasn't enabled at compilation time");
		return;
	}

	file = fopen(vcard_file, "wb");
	if (file == NULL) {
		ms_warning("Could not write %s ! Maybe it is read-only. Contacts will not be saved.", vcard_file);
		return;
	}

	friends = linphone_friend_list_get_friends(list);
	while (friends != NULL && bctbx_list_get_data(friends) != NULL) {
		LinphoneFriend *lf = (LinphoneFriend *)bctbx_list_get_data(friends);
		LinphoneVcard *vcard = linphone_friend_get_vcard(lf);
		if (vcard) {
			const char *vcard_text = linphone_vcard_as_vcard4_string(vcard);
			fprintf(file, "%s", vcard_text);
		}
		friends = bctbx_list_next(friends);
	}

	fclose(file);
}

void linphone_friend_list_enable_subscriptions(LinphoneFriendList *list, bool_t enabled) {
	if (list->enable_subscriptions != enabled) {
		list->enable_subscriptions = enabled;
		if (enabled) {
			ms_message("Updating friend list [%p] subscriptions", list);
			linphone_friend_list_update_subscriptions(list);
		} else {
			ms_message("Closing friend list [%p] subscriptions", list);
			linphone_friend_list_close_subscriptions(list);
		}
	}
}

bool_t linphone_friend_list_subscriptions_enabled(LinphoneFriendList *list) {
	return list->enable_subscriptions;
}
