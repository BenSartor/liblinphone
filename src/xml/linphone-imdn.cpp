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

// Begin prologue.
//
#if __clang__ || __GNUC__ >= 4
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#endif
#if __GNUC__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif
#if __GNUC__ >= 7
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif
//
// End prologue.

#include <xsd/cxx/pre.hxx>

#include "linphone-imdn.h"

namespace LinphonePrivate {
namespace Xsd {
namespace LinphoneImdn {
// ImdnReason
//

const ImdnReason::CodeType &ImdnReason::getCode() const {
	return this->code_.get();
}

ImdnReason::CodeType &ImdnReason::getCode() {
	return this->code_.get();
}

void ImdnReason::setCode(const CodeType &x) {
	this->code_.set(x);
}

ImdnReason::CodeType ImdnReason::getCodeDefaultValue() {
	return CodeType(200);
}
} // namespace LinphoneImdn
} // namespace Xsd
} // namespace LinphonePrivate

#include <xsd/cxx/xml/dom/wildcard-source.hxx>

#include <xsd/cxx/xml/dom/parsing-source.hxx>

#include <xsd/cxx/tree/type-factory-map.hxx>

namespace _xsd {
static const ::xsd::cxx::tree::type_factory_plate<0, char> type_factory_plate_init;
}

namespace LinphonePrivate {
namespace Xsd {
namespace LinphoneImdn {
// ImdnReason
//

ImdnReason::ImdnReason() : ::LinphonePrivate::Xsd::XmlSchema::String(), code_(getCodeDefaultValue(), this) {
}

ImdnReason::ImdnReason(const char *_xsd_String_base)
    : ::LinphonePrivate::Xsd::XmlSchema::String(_xsd_String_base), code_(getCodeDefaultValue(), this) {
}

ImdnReason::ImdnReason(const ::std::string &_xsd_String_base)
    : ::LinphonePrivate::Xsd::XmlSchema::String(_xsd_String_base), code_(getCodeDefaultValue(), this) {
}

ImdnReason::ImdnReason(const ::LinphonePrivate::Xsd::XmlSchema::String &_xsd_String_base)
    : ::LinphonePrivate::Xsd::XmlSchema::String(_xsd_String_base), code_(getCodeDefaultValue(), this) {
}

ImdnReason::ImdnReason(const ImdnReason &x,
                       ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                       ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(x, f, c), code_(x.code_, f, this) {
}

ImdnReason::ImdnReason(const ::xercesc::DOMElement &e,
                       ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                       ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(e, f | ::LinphonePrivate::Xsd::XmlSchema::Flags::base, c), code_(this) {
	if ((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::base) == 0) {
		::xsd::cxx::xml::dom::parser<char> p(e, false, false, true);
		this->parse(p, f);
	}
}

void ImdnReason::parse(::xsd::cxx::xml::dom::parser<char> &p, ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	while (p.more_attributes()) {
		const ::xercesc::DOMAttr &i(p.next_attribute());
		const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(i));

		if (n.name() == "code" && n.namespace_().empty()) {
			this->code_.set(CodeTraits::create(i, f, this));
			continue;
		}
	}

	if (!code_.present()) {
		this->code_.set(getCodeDefaultValue());
	}
}

ImdnReason *ImdnReason::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f,
                               ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class ImdnReason(*this, f, c);
}

ImdnReason &ImdnReason::operator=(const ImdnReason &x) {
	if (this != &x) {
		static_cast<::LinphonePrivate::Xsd::XmlSchema::String &>(*this) = x;
		this->code_ = x.code_;
	}

	return *this;
}

ImdnReason::~ImdnReason() {
}
} // namespace LinphoneImdn
} // namespace Xsd
} // namespace LinphonePrivate

#include <ostream>

#include <xsd/cxx/tree/std-ostream-map.hxx>

namespace _xsd {
static const ::xsd::cxx::tree::std_ostream_plate<0, char> std_ostream_plate_init;
}

namespace LinphonePrivate {
namespace Xsd {
namespace LinphoneImdn {
::std::ostream &operator<<(::std::ostream &o, const ImdnReason &i) {
	o << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);

	o << ::std::endl << "code: " << i.getCode();
	return o;
}
} // namespace LinphoneImdn
} // namespace Xsd
} // namespace LinphonePrivate

#include <istream>
#include <xsd/cxx/tree/error-handler.hxx>
#include <xsd/cxx/xml/sax/std-input-source.hxx>

namespace LinphonePrivate {
namespace Xsd {
namespace LinphoneImdn {
::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>
parseReason(const ::std::string &u,
            ::LinphonePrivate::Xsd::XmlSchema::Flags f,
            const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::tree::error_handler<char> h;

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(u, h, p, f));

	h.throw_if_failed<::xsd::cxx::tree::parsing<char>>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>(
	    ::LinphonePrivate::Xsd::LinphoneImdn::parseReason(std::move(d),
	                                                      f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>
parseReason(const ::std::string &u,
            ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
            ::LinphonePrivate::Xsd::XmlSchema::Flags f,
            const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(u, h, p, f));

	if (!d.get()) throw ::xsd::cxx::tree::parsing<char>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>(
	    ::LinphonePrivate::Xsd::LinphoneImdn::parseReason(std::move(d),
	                                                      f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>
parseReason(const ::std::string &u,
            ::xercesc::DOMErrorHandler &h,
            ::LinphonePrivate::Xsd::XmlSchema::Flags f,
            const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(u, h, p, f));

	if (!d.get()) throw ::xsd::cxx::tree::parsing<char>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>(
	    ::LinphonePrivate::Xsd::LinphoneImdn::parseReason(std::move(d),
	                                                      f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>
parseReason(::std::istream &is,
            ::LinphonePrivate::Xsd::XmlSchema::Flags f,
            const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::xml::sax::std_input_source isrc(is);
	return ::LinphonePrivate::Xsd::LinphoneImdn::parseReason(isrc, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>
parseReason(::std::istream &is,
            ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
            ::LinphonePrivate::Xsd::XmlSchema::Flags f,
            const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::xml::sax::std_input_source isrc(is);
	return ::LinphonePrivate::Xsd::LinphoneImdn::parseReason(isrc, h, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>
parseReason(::std::istream &is,
            ::xercesc::DOMErrorHandler &h,
            ::LinphonePrivate::Xsd::XmlSchema::Flags f,
            const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::sax::std_input_source isrc(is);
	return ::LinphonePrivate::Xsd::LinphoneImdn::parseReason(isrc, h, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>
parseReason(::std::istream &is,
            const ::std::string &sid,
            ::LinphonePrivate::Xsd::XmlSchema::Flags f,
            const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::xml::sax::std_input_source isrc(is, sid);
	return ::LinphonePrivate::Xsd::LinphoneImdn::parseReason(isrc, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>
parseReason(::std::istream &is,
            const ::std::string &sid,
            ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
            ::LinphonePrivate::Xsd::XmlSchema::Flags f,
            const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::xml::sax::std_input_source isrc(is, sid);
	return ::LinphonePrivate::Xsd::LinphoneImdn::parseReason(isrc, h, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>
parseReason(::std::istream &is,
            const ::std::string &sid,
            ::xercesc::DOMErrorHandler &h,
            ::LinphonePrivate::Xsd::XmlSchema::Flags f,
            const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::sax::std_input_source isrc(is, sid);
	return ::LinphonePrivate::Xsd::LinphoneImdn::parseReason(isrc, h, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>
parseReason(::xercesc::InputSource &i,
            ::LinphonePrivate::Xsd::XmlSchema::Flags f,
            const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::tree::error_handler<char> h;

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(i, h, p, f));

	h.throw_if_failed<::xsd::cxx::tree::parsing<char>>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>(
	    ::LinphonePrivate::Xsd::LinphoneImdn::parseReason(std::move(d),
	                                                      f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>
parseReason(::xercesc::InputSource &i,
            ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
            ::LinphonePrivate::Xsd::XmlSchema::Flags f,
            const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(i, h, p, f));

	if (!d.get()) throw ::xsd::cxx::tree::parsing<char>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>(
	    ::LinphonePrivate::Xsd::LinphoneImdn::parseReason(std::move(d),
	                                                      f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>
parseReason(::xercesc::InputSource &i,
            ::xercesc::DOMErrorHandler &h,
            ::LinphonePrivate::Xsd::XmlSchema::Flags f,
            const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(i, h, p, f));

	if (!d.get()) throw ::xsd::cxx::tree::parsing<char>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>(
	    ::LinphonePrivate::Xsd::LinphoneImdn::parseReason(std::move(d),
	                                                      f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>
parseReason(const ::xercesc::DOMDocument &doc,
            ::LinphonePrivate::Xsd::XmlSchema::Flags f,
            const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	if (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) {
		::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
		    static_cast<::xercesc::DOMDocument *>(doc.cloneNode(true)));

		return ::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>(
		    ::LinphonePrivate::Xsd::LinphoneImdn::parseReason(
		        std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
	}

	const ::xercesc::DOMElement &e(*doc.getDocumentElement());
	const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(e));

	if (n.name() == "reason" && n.namespace_() == "http://www.linphone.org/xsds/imdn.xsd") {
		::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason> r(
		    ::xsd::cxx::tree::traits<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason, char>::create(e, f, 0));
		return r;
	}

	throw ::xsd::cxx::tree::unexpected_element<char>(n.name(), n.namespace_(), "reason",
	                                                 "http://www.linphone.org/xsds/imdn.xsd");
}

::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason>
parseReason(::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d,
            ::LinphonePrivate::Xsd::XmlSchema::Flags f,
            const ::LinphonePrivate::Xsd::XmlSchema::Properties &) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> c(
	    ((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) &&
	     !(f & ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom))
	        ? static_cast<::xercesc::DOMDocument *>(d->cloneNode(true))
	        : 0);

	::xercesc::DOMDocument &doc(c.get() ? *c : *d);
	const ::xercesc::DOMElement &e(*doc.getDocumentElement());

	const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(e));

	if (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom)
		doc.setUserData(::LinphonePrivate::Xsd::XmlSchema::dom::treeNodeKey, (c.get() ? &c : &d), 0);

	if (n.name() == "reason" && n.namespace_() == "http://www.linphone.org/xsds/imdn.xsd") {
		::std::unique_ptr<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason> r(
		    ::xsd::cxx::tree::traits<::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason, char>::create(e, f, 0));
		return r;
	}

	throw ::xsd::cxx::tree::unexpected_element<char>(n.name(), n.namespace_(), "reason",
	                                                 "http://www.linphone.org/xsds/imdn.xsd");
}
} // namespace LinphoneImdn
} // namespace Xsd
} // namespace LinphonePrivate

#include <ostream>
#include <xsd/cxx/tree/error-handler.hxx>
#include <xsd/cxx/xml/dom/serialization-source.hxx>

#include <xsd/cxx/tree/type-serializer-map.hxx>

namespace _xsd {
static const ::xsd::cxx::tree::type_serializer_plate<0, char> type_serializer_plate_init;
}

namespace LinphonePrivate {
namespace Xsd {
namespace LinphoneImdn {
void serializeReason(::std::ostream &o,
                     const ::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason &s,
                     const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                     const ::std::string &e,
                     ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0);

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::LinphoneImdn::serializeReason(s, m, f));

	::xsd::cxx::tree::error_handler<char> h;

	::xsd::cxx::xml::dom::ostream_format_target t(o);
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		h.throw_if_failed<::xsd::cxx::tree::serialization<char>>();
	}
}

void serializeReason(::std::ostream &o,
                     const ::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason &s,
                     ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
                     const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                     const ::std::string &e,
                     ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0);

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::LinphoneImdn::serializeReason(s, m, f));
	::xsd::cxx::xml::dom::ostream_format_target t(o);
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		throw ::xsd::cxx::tree::serialization<char>();
	}
}

void serializeReason(::std::ostream &o,
                     const ::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason &s,
                     ::xercesc::DOMErrorHandler &h,
                     const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                     const ::std::string &e,
                     ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::LinphoneImdn::serializeReason(s, m, f));
	::xsd::cxx::xml::dom::ostream_format_target t(o);
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		throw ::xsd::cxx::tree::serialization<char>();
	}
}

void serializeReason(::xercesc::XMLFormatTarget &t,
                     const ::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason &s,
                     const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                     const ::std::string &e,
                     ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::LinphoneImdn::serializeReason(s, m, f));

	::xsd::cxx::tree::error_handler<char> h;

	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		h.throw_if_failed<::xsd::cxx::tree::serialization<char>>();
	}
}

void serializeReason(::xercesc::XMLFormatTarget &t,
                     const ::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason &s,
                     ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
                     const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                     const ::std::string &e,
                     ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::LinphoneImdn::serializeReason(s, m, f));
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		throw ::xsd::cxx::tree::serialization<char>();
	}
}

void serializeReason(::xercesc::XMLFormatTarget &t,
                     const ::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason &s,
                     ::xercesc::DOMErrorHandler &h,
                     const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                     const ::std::string &e,
                     ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::LinphoneImdn::serializeReason(s, m, f));
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		throw ::xsd::cxx::tree::serialization<char>();
	}
}

void serializeReason(::xercesc::DOMDocument &d,
                     const ::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason &s,
                     ::LinphonePrivate::Xsd::XmlSchema::Flags) {
	::xercesc::DOMElement &e(*d.getDocumentElement());
	const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(e));

	if (n.name() == "reason" && n.namespace_() == "http://www.linphone.org/xsds/imdn.xsd") {
		e << s;
	} else {
		throw ::xsd::cxx::tree::unexpected_element<char>(n.name(), n.namespace_(), "reason",
		                                                 "http://www.linphone.org/xsds/imdn.xsd");
	}
}

::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument>
serializeReason(const ::LinphonePrivate::Xsd::LinphoneImdn::ImdnReason &s,
                const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::serialize<char>("reason", "http://www.linphone.org/xsds/imdn.xsd", m, f));

	::LinphonePrivate::Xsd::LinphoneImdn::serializeReason(*d, s, f);
	return d;
}

void operator<<(::xercesc::DOMElement &e, const ImdnReason &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);

	// code
	//
	{
		::xercesc::DOMAttr &a(::xsd::cxx::xml::dom::create_attribute("code", e));

		a << i.getCode();
	}
}
} // namespace LinphoneImdn
} // namespace Xsd
} // namespace LinphonePrivate

#include <xsd/cxx/post.hxx>

// Begin epilogue.
//
#if __GNUC__ >= 7
#pragma GCC diagnostic pop
#endif
#if __GNUC__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1)
#pragma GCC diagnostic pop
#endif
#if __clang__ || __GNUC__ >= 4
#pragma GCC diagnostic pop
#endif
//
// End epilogue.
