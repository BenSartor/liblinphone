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

#include "linphone/api/c-content.h"
#include "linphone/wrapper_utils.h"

#include "c-wrapper/c-wrapper.h"
#include "content/content-type.h"
#include "content/file-content.h"
#include "content/file-transfer-content.h"
#include "content/header/header-param.h"

// =============================================================================

using namespace std;

static void _linphone_content_constructor(LinphoneContent *content);
static void _linphone_content_destructor(LinphoneContent *content);
static void _linphone_content_c_clone(LinphoneContent *dest, const LinphoneContent *src);

L_DECLARE_C_CLONABLE_OBJECT_IMPL_WITH_XTORS(
    Content,
    _linphone_content_constructor,
    _linphone_content_destructor,
    _linphone_content_c_clone,

    void *cryptoContext; // Used to encrypt file for RCS file transfer.

    mutable size_t size;
    bool_t is_dirty = FALSE;
    SalBodyHandler * body_handler;

    struct Cache {
	    string name;
	    string type;
	    string subtype;
	    string buffer;
	    string file_path;
	    string header_value;
    } mutable cache;)

static void _linphone_content_constructor(LinphoneContent *content) {
	new (&content->cache) LinphoneContent::Cache();
}

static void _linphone_content_destructor(LinphoneContent *content) {
	content->cache.~Cache();
	if (content->body_handler) sal_body_handler_unref(content->body_handler);
}

static void _linphone_content_c_clone(LinphoneContent *dest, const LinphoneContent *src) {
	new (&dest->cache) LinphoneContent::Cache();
	dest->size = src->size;
	dest->cache = src->cache;
	if (!src->is_dirty && src->body_handler) dest->body_handler = sal_body_handler_ref(src->body_handler);
}

// =============================================================================
// Reference and user data handling functions.
// =============================================================================

LinphoneContent *linphone_content_ref(LinphoneContent *content) {
	belle_sip_object_ref(content);
	return content;
}

void linphone_content_unref(LinphoneContent *content) {
	belle_sip_object_unref(content);
}

void *linphone_content_get_user_data(const LinphoneContent *content) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(content)->getUserData().getValue<void *>();
}

void linphone_content_set_user_data(LinphoneContent *content, void *user_data) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(content)->setUserData(user_data);
}

// =============================================================================

const char *linphone_content_get_type(const LinphoneContent *content) {
	content->cache.type = L_GET_CPP_PTR_FROM_C_OBJECT(content)->getContentType().getType();
	return content->cache.type.c_str();
}

void linphone_content_set_type(LinphoneContent *content, const char *type) {
	LinphonePrivate::ContentType contentType = L_GET_CPP_PTR_FROM_C_OBJECT(content)->getContentType();
	contentType.setType(L_C_TO_STRING(type));
	L_GET_CPP_PTR_FROM_C_OBJECT(content)->setContentType(contentType);
}

const char *linphone_content_get_subtype(const LinphoneContent *content) {
	content->cache.subtype = L_GET_CPP_PTR_FROM_C_OBJECT(content)->getContentType().getSubType();
	return content->cache.subtype.c_str();
}

void linphone_content_set_subtype(LinphoneContent *content, const char *subtype) {
	LinphonePrivate::ContentType contentType = L_GET_CPP_PTR_FROM_C_OBJECT(content)->getContentType();
	contentType.setSubType(L_C_TO_STRING(subtype));
	L_GET_CPP_PTR_FROM_C_OBJECT(content)->setContentType(contentType);
}

void linphone_content_add_content_type_parameter(LinphoneContent *content, const char *name, const char *value) {
	LinphonePrivate::ContentType contentType = L_GET_CPP_PTR_FROM_C_OBJECT(content)->getContentType();
	contentType.addParameter(L_C_TO_STRING(name), L_C_TO_STRING(value));
	L_GET_CPP_PTR_FROM_C_OBJECT(content)->setContentType(contentType);
}

const uint8_t *linphone_content_get_buffer(const LinphoneContent *content) {
	return reinterpret_cast<const uint8_t *>(linphone_content_get_utf8_text(content));
}

void linphone_content_set_buffer(LinphoneContent *content, const uint8_t *buffer, size_t size) {
	content->is_dirty = TRUE;
	L_GET_CPP_PTR_FROM_C_OBJECT(content)->setBody(buffer, size);
}

const char *linphone_content_get_string_buffer(const LinphoneContent *content) {
	content->cache.buffer = L_GET_CPP_PTR_FROM_C_OBJECT(content)->getBodyAsUtf8String();
	return content->cache.buffer.c_str();
}

const char *linphone_content_get_utf8_text(const LinphoneContent *content) {
	content->cache.buffer = L_GET_CPP_PTR_FROM_C_OBJECT(content)->getBodyAsUtf8String();
	return content->cache.buffer.c_str();
}

void linphone_content_set_utf8_text(LinphoneContent *content, const char *buffer) {
	content->is_dirty = TRUE;
	L_GET_CPP_PTR_FROM_C_OBJECT(content)->setBodyFromUtf8(L_C_TO_STRING(buffer));
}

void linphone_content_set_string_buffer(LinphoneContent *content, const char *buffer) {
	content->is_dirty = TRUE;
	L_GET_CPP_PTR_FROM_C_OBJECT(content)->setBodyFromUtf8(L_C_TO_STRING(buffer));
}

size_t linphone_content_get_file_size(const LinphoneContent *content) {
	const LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	size_t size = 0;
	if (c->isFile()) size = static_cast<const LinphonePrivate::FileContent *>(c)->getFileSize();
	else if (c->isFileTransfer()) size = static_cast<const LinphonePrivate::FileTransferContent *>(c)->getFileSize();
	return size;
}

size_t linphone_content_get_size(const LinphoneContent *content) {
	const LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	size_t size = c->getSize();
	if (size == 0) {
		size = content->size;
	}
	return size;
}

void linphone_content_set_size(LinphoneContent *content, size_t size) {
	content->size = size;
}

const char *linphone_content_get_encoding(const LinphoneContent *content) {
	return L_STRING_TO_C(L_GET_CPP_PTR_FROM_C_OBJECT(content)->getContentEncoding());
}

void linphone_content_set_encoding(LinphoneContent *content, const char *encoding) {
	L_GET_CPP_PTR_FROM_C_OBJECT(content)->setContentEncoding(L_C_TO_STRING(encoding));
}

const char *linphone_content_get_disposition(const LinphoneContent *content) {
	LinphonePrivate::ContentDisposition contentDisposition =
	    L_GET_CPP_PTR_FROM_C_OBJECT(content)->getContentDisposition();
	return L_STRING_TO_C(contentDisposition.asString());
}

void linphone_content_set_disposition(LinphoneContent *content, const char *disposition) {
	if (disposition != nullptr) {
		string strDisposition = L_C_TO_STRING(disposition);
		if (!strDisposition.empty()) {
			LinphonePrivate::ContentDisposition contentDisposition =
			    LinphonePrivate::ContentDisposition(strDisposition);
			L_GET_CPP_PTR_FROM_C_OBJECT(content)->setContentDisposition(contentDisposition);
		}
	}
}

const char *linphone_content_get_name(const LinphoneContent *content) {
	const LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	if (c->isFile()) return static_cast<const LinphonePrivate::FileContent *>(c)->getFileName().c_str();
	else if (c->isFileTransfer())
		return static_cast<const LinphonePrivate::FileTransferContent *>(c)->getFileName().c_str();
	return content->cache.name.c_str();
}

void linphone_content_set_name(LinphoneContent *content, const char *name) {
	LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	if (c->isFile()) static_cast<LinphonePrivate::FileContent *>(c)->setFileName(L_C_TO_STRING(name));
	else if (c->isFileTransfer())
		static_cast<LinphonePrivate::FileTransferContent *>(c)->setFileName(L_C_TO_STRING(name));
	else content->cache.name = L_C_TO_STRING(name);
}

bool_t linphone_content_is_multipart(const LinphoneContent *content) {
	return L_GET_CPP_PTR_FROM_C_OBJECT(content)->getContentType().isMultipart();
}

bctbx_list_t *linphone_content_get_parts(const LinphoneContent *content) {
	bctbx_list_t *parts = nullptr;
	SalBodyHandler *bodyHandler;
	if (!content->is_dirty && content->body_handler) {
		bodyHandler = sal_body_handler_ref(content->body_handler);
	} else {
		bodyHandler = sal_body_handler_from_content(content);
	}

	if (!sal_body_handler_is_multipart(bodyHandler)) {
		sal_body_handler_unref(bodyHandler);
		return parts;
	}

	const bctbx_list_t *sal_parts = sal_body_handler_get_parts(bodyHandler);
	bctbx_list_t *it = (bctbx_list_t *)sal_parts;
	while (it != nullptr) {
		SalBodyHandler *bh = (SalBodyHandler *)it->data;
		LinphoneContent *part = linphone_content_from_sal_body_handler(bh);
		parts = bctbx_list_append(parts, linphone_content_ref(part));
		linphone_content_unref(part);
		it = bctbx_list_next(it);
	}

	sal_body_handler_unref(bodyHandler);
	return parts;
}

LinphoneContent *linphone_content_get_part(const LinphoneContent *content, int idx) {
	SalBodyHandler *bodyHandler;
	if (!content->is_dirty && content->body_handler) {
		bodyHandler = sal_body_handler_ref(content->body_handler);
	} else {
		bodyHandler = sal_body_handler_from_content(content);
	}

	if (!sal_body_handler_is_multipart(bodyHandler)) {
		sal_body_handler_unref(bodyHandler);
		return nullptr;
	}

	SalBodyHandler *partBodyHandler = sal_body_handler_get_part(bodyHandler, idx);
	LinphoneContent *result = linphone_content_from_sal_body_handler(partBodyHandler);
	sal_body_handler_unref(bodyHandler);
	return result;
}

LinphoneContent *
linphone_content_find_part_by_header(const LinphoneContent *content, const char *headerName, const char *headerValue) {
	SalBodyHandler *bodyHandler;
	if (!content->is_dirty && content->body_handler) {
		bodyHandler = sal_body_handler_ref(content->body_handler);
	} else {
		bodyHandler = sal_body_handler_from_content(content);
	}

	if (!sal_body_handler_is_multipart(bodyHandler)) {
		sal_body_handler_unref(bodyHandler);
		return nullptr;
	}

	SalBodyHandler *partBodyHandler = sal_body_handler_find_part_by_header(bodyHandler, headerName, headerValue);
	LinphoneContent *result = linphone_content_from_sal_body_handler(partBodyHandler);
	sal_body_handler_unref(bodyHandler);
	return result;
}

const char *linphone_content_get_custom_header(const LinphoneContent *content, const char *headerName) {
	SalBodyHandler *bodyHandler;
	if (!content->is_dirty && content->body_handler) {
		bodyHandler = sal_body_handler_ref(content->body_handler);
	} else {
		bodyHandler = sal_body_handler_from_content(content);
	}

	content->cache.header_value = L_C_TO_STRING(sal_body_handler_get_header(bodyHandler, headerName));
	sal_body_handler_unref(bodyHandler);
	return content->cache.header_value.c_str();
}

void linphone_content_add_custom_header(LinphoneContent *content, const char *header_name, const char *header_value) {
	LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	c->addHeader(L_C_TO_STRING(header_name), L_C_TO_STRING(header_value));
}

const char *linphone_content_get_key(const LinphoneContent *content) {
	const LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	if (c->isFileTransfer()) {
		const LinphonePrivate::FileTransferContent *ftc = static_cast<const LinphonePrivate::FileTransferContent *>(c);
		return ftc->getFileKey().data();
	}
	return nullptr;
}

size_t linphone_content_get_key_size(const LinphoneContent *content) {
	const LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	if (c->isFileTransfer()) {
		const LinphonePrivate::FileTransferContent *ftc = static_cast<const LinphonePrivate::FileTransferContent *>(c);
		return ftc->getFileKeySize();
	}
	return 0;
}

void linphone_content_set_key(LinphoneContent *content, const char *key, const size_t keyLength) {
	LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	if (c->isFileTransfer()) {
		LinphonePrivate::FileTransferContent *ftc = static_cast<LinphonePrivate::FileTransferContent *>(c);
		ftc->setFileKey(key, keyLength);
	}
}

const char *linphone_content_get_authTag(const LinphoneContent *content) {
	const LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	if (c->isFileTransfer()) {
		const LinphonePrivate::FileTransferContent *ftc = static_cast<const LinphonePrivate::FileTransferContent *>(c);
		return ftc->getFileAuthTag().data();
	}
	return nullptr;
}

size_t linphone_content_get_authTag_size(const LinphoneContent *content) {
	const LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	if (c->isFileTransfer()) {
		const LinphonePrivate::FileTransferContent *ftc = static_cast<const LinphonePrivate::FileTransferContent *>(c);
		return ftc->getFileAuthTagSize();
	}
	return 0;
}

void linphone_content_set_authTag(LinphoneContent *content, const char *tag, const size_t tagLength) {
	LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	if (c->isFileTransfer()) {
		LinphonePrivate::FileTransferContent *ftc = static_cast<LinphonePrivate::FileTransferContent *>(c);
		ftc->setFileAuthTag(tag, tagLength);
	}
}

const char *linphone_content_get_file_path(const LinphoneContent *content) {
	const LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	if (c->isFile()) {
		return L_STRING_TO_C(static_cast<const LinphonePrivate::FileContent *>(c)->getFilePath());
	} else if (c->isFileTransfer()) {
		return L_STRING_TO_C(static_cast<const LinphonePrivate::FileTransferContent *>(c)->getFilePath());
	}
	return L_STRING_TO_C(content->cache.file_path);
}

/* function deprecated on 07/01/2022 export_plain_file is more explicit */
char *linphone_content_get_plain_file_path(const LinphoneContent *content) {
	return linphone_content_export_plain_file(content);
}

char *linphone_content_export_plain_file(const LinphoneContent *content) {
	const LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	if (c->isFile()) {
		auto filePath = static_cast<const LinphonePrivate::FileContent *>(c)->exportPlainFile();
		return bctbx_strdup(L_STRING_TO_C(filePath));
	} else if (c->isFileTransfer()) {
		auto filePath = static_cast<const LinphonePrivate::FileTransferContent *>(c)->exportPlainFile();
		return bctbx_strdup(L_STRING_TO_C(filePath));
	}
	return NULL;
}

void linphone_content_set_file_path(LinphoneContent *content, const char *file_path) {
	LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	if (c->isFile()) {
		static_cast<LinphonePrivate::FileContent *>(c)->setFilePath(L_C_TO_STRING(file_path));
	} else if (c->isFileTransfer()) {
		static_cast<LinphonePrivate::FileTransferContent *>(c)->setFilePath(L_C_TO_STRING(file_path));
	}
	content->cache.file_path = L_C_TO_STRING(file_path);
}

int linphone_content_get_file_duration(LinphoneContent *content) {
	LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	if (c->isFile()) return static_cast<LinphonePrivate::FileContent *>(c)->getFileDuration();
	else if (c->isFileTransfer()) return static_cast<LinphonePrivate::FileTransferContent *>(c)->getFileDuration();
	return -1;
}

bool_t linphone_content_is_text(const LinphoneContent *content) {
	const LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	return c->getContentType() == LinphonePrivate::ContentType::PlainText;
}

bool_t linphone_content_is_voice_recording(const LinphoneContent *content) {
	const LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	if (c->isFile()) {
		return c->getContentType().strongEqual(LinphonePrivate::ContentType::VoiceRecording);
	} else if (c->isFileTransfer()) {
		return static_cast<const LinphonePrivate::FileTransferContent *>(c)->getFileContentType().strongEqual(
		    LinphonePrivate::ContentType::VoiceRecording);
	}
	return false;
}

bool_t linphone_content_is_icalendar(const LinphoneContent *content) {
	const LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	if (c->isFileTransfer()) {
		return static_cast<const LinphonePrivate::FileTransferContent *>(c)->getFileContentType().strongEqual(
		    LinphonePrivate::ContentType::Icalendar);
	} else {
		return c->getContentType().strongEqual(LinphonePrivate::ContentType::Icalendar);
	}
	return false;
}

bool_t linphone_content_is_file(const LinphoneContent *content) {
	const LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	return c->isFile();
}

bool_t linphone_content_is_file_transfer(const LinphoneContent *content) {
	const LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	return c->isFileTransfer();
}

bool_t linphone_content_is_file_encrypted(const LinphoneContent *content) {
	const LinphonePrivate::Content *c = L_GET_CPP_PTR_FROM_C_OBJECT(content);
	if (c->isFile()) {
		return static_cast<const LinphonePrivate::FileContent *>(c)->isEncrypted();
	} else if (c->isFileTransfer()) {
		return static_cast<const LinphonePrivate::FileTransferContent *>(c)->isEncrypted();
	}
	return FALSE;
}

// =============================================================================
// Private functions.
// =============================================================================

static LinphoneContent *linphone_content_new_with_body_handler(const SalBodyHandler *bodyHandler, bool parseMultipart) {
	LinphoneContent *content = L_INIT(Content);
	content->cryptoContext = nullptr;
	LinphonePrivate::Content *c = new LinphonePrivate::Content();
	L_SET_CPP_PTR_FROM_C_OBJECT(content, c);
	content->body_handler = nullptr;

	if (!bodyHandler) return content;

	content->body_handler = sal_body_handler_ref((SalBodyHandler *)bodyHandler);

	linphone_content_set_type(content, sal_body_handler_get_type(bodyHandler));
	linphone_content_set_subtype(content, sal_body_handler_get_subtype(bodyHandler));
	for (const belle_sip_list_t *params = sal_body_handler_get_content_type_parameters_names(bodyHandler); params;
	     params = params->next) {
		const char *paramName = reinterpret_cast<const char *>(params->data);
		const char *paramValue = sal_body_handler_get_content_type_parameter(bodyHandler, paramName);
		linphone_content_add_content_type_parameter(content, paramName, paramValue);
	}

	if (linphone_content_is_multipart(content) && parseMultipart) {
		belle_sip_multipart_body_handler_t *mpbh = BELLE_SIP_MULTIPART_BODY_HANDLER(bodyHandler);
		char *body = belle_sip_object_to_string(mpbh);
		linphone_content_set_utf8_text(content, body);
		belle_sip_free(body);
	} else {
		linphone_content_set_utf8_text(content, reinterpret_cast<char *>(sal_body_handler_get_data(bodyHandler)));
	}

	const belle_sip_list_t *headers =
	    reinterpret_cast<const belle_sip_list_t *>(sal_body_handler_get_headers(bodyHandler));
	while (headers) {
		belle_sip_header_t *cHeader = BELLE_SIP_HEADER(headers->data);
		LinphonePrivate::Header header =
		    LinphonePrivate::Header(belle_sip_header_get_name(cHeader), belle_sip_header_get_unparsed_value(cHeader));
		L_GET_CPP_PTR_FROM_C_OBJECT(content)->addHeader(header);
		headers = headers->next;
	}

	if (sal_body_handler_get_encoding(bodyHandler))
		linphone_content_set_encoding(content, sal_body_handler_get_encoding(bodyHandler));

	const char *disposition = sal_body_handler_get_content_disposition(bodyHandler);
	if (disposition) {
		LinphonePrivate::ContentDisposition contentDisposition = LinphonePrivate::ContentDisposition(disposition);
		L_GET_CPP_PTR_FROM_C_OBJECT(content)->setContentDisposition(contentDisposition);
	}

	return content;
}

LinphoneContent *linphone_content_new(void) {
	return linphone_content_new_with_body_handler(nullptr, true);
}

LinphoneContent *linphone_content_copy(const LinphoneContent *ref) {
	return reinterpret_cast<LinphoneContent *>(belle_sip_object_clone(BELLE_SIP_OBJECT(ref)));
}

LinphoneContent *linphone_core_create_content(BCTBX_UNUSED(LinphoneCore *lc)) {
	return linphone_content_new();
}

// Crypto context is managed(allocated/freed) by the encryption function,
// so provide the address of field in the private structure.
void **linphone_content_get_cryptoContext_address(LinphoneContent *content) {
	return &content->cryptoContext;
}

LinphoneContent *linphone_content_from_sal_body_handler(const SalBodyHandler *bodyHandler, bool parseMultipart) {
	if (!bodyHandler) return nullptr;
	return linphone_content_new_with_body_handler(bodyHandler, parseMultipart);
}

SalBodyHandler *sal_body_handler_from_content(const LinphoneContent *content, bool parseMultipart) {
	if (!content) return nullptr;

	if (!content->is_dirty && content->body_handler) return sal_body_handler_ref(content->body_handler);

	auto cppContent = L_GET_CPP_PTR_FROM_C_OBJECT(content);

	SalBodyHandler *bodyHandler;
	LinphonePrivate::ContentType contentType = cppContent->getContentType();
	if (contentType.isMultipart() && parseMultipart) {
		size_t size = linphone_content_get_size(content);
		char *buffer = bctbx_strdup(cppContent->getBodyAsUtf8String().c_str());
		const char *boundary = L_STRING_TO_C(contentType.getParameter("boundary").getValue());
		belle_sip_multipart_body_handler_t *bh = NULL;
		if (boundary) bh = belle_sip_multipart_body_handler_new_from_buffer(buffer, size, boundary);
		else if (size > 2) {
			size_t startIndex = 2, index;
			while (startIndex < size &&
			       (buffer[startIndex] != '-' || buffer[startIndex - 1] != '-' // Take accout of first "--"
			        || (startIndex > 2 && buffer[startIndex - 2] != '\n')))    // Must be at the beginning of the line
				++startIndex;
			index = startIndex;
			while (index < size && buffer[index] != '\n' && buffer[index] != '\r')
				++index;
			if (startIndex != index) {
				char *boundaryStr = bctbx_strndup(buffer + startIndex, (int)(index - startIndex));
				bh = belle_sip_multipart_body_handler_new_from_buffer(buffer, size, boundaryStr);
				bctbx_free(boundaryStr);
			}
		}

		bodyHandler = reinterpret_cast<SalBodyHandler *>(BELLE_SIP_BODY_HANDLER(bh));
		bctbx_free(buffer);
	} else {
		bodyHandler = sal_body_handler_new();
		sal_body_handler_set_data(bodyHandler, belle_sip_strdup(cppContent->getBodyAsUtf8String().c_str()));
	}

	for (const auto &header : cppContent->getHeaders()) {
		sal_body_handler_add_header(bodyHandler, header.getName().c_str(), header.getValueWithParams().c_str());
	}

	sal_body_handler_set_type(bodyHandler, contentType.getType().c_str());
	sal_body_handler_set_subtype(bodyHandler, contentType.getSubType().c_str());
	sal_body_handler_set_size(bodyHandler, linphone_content_get_size(content));
	for (const auto &param : contentType.getParameters())
		sal_body_handler_set_content_type_parameter(bodyHandler, param.getName().c_str(), param.getValue().c_str());

	if (linphone_content_get_encoding(content))
		sal_body_handler_set_encoding(bodyHandler, linphone_content_get_encoding(content));

	const LinphonePrivate::ContentDisposition &disposition = cppContent->getContentDisposition();
	if (disposition.isValid()) sal_body_handler_set_content_disposition(bodyHandler, disposition.asString().c_str());

	return bodyHandler;
}
