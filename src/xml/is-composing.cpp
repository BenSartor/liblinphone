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

#include "is-composing.h"

namespace LinphonePrivate {
namespace Xsd {
namespace IsComposing {
// IsComposing
//

const IsComposing::StateType &IsComposing::getState() const {
	return this->state_.get();
}

IsComposing::StateType &IsComposing::getState() {
	return this->state_.get();
}

void IsComposing::setState(const StateType &x) {
	this->state_.set(x);
}

void IsComposing::setState(::std::unique_ptr<StateType> x) {
	this->state_.set(std::move(x));
}

::std::unique_ptr<IsComposing::StateType> IsComposing::setDetachState() {
	return this->state_.detach();
}

const IsComposing::LastactiveOptional &IsComposing::getLastactive() const {
	return this->lastactive_;
}

IsComposing::LastactiveOptional &IsComposing::getLastactive() {
	return this->lastactive_;
}

void IsComposing::setLastactive(const LastactiveType &x) {
	this->lastactive_.set(x);
}

void IsComposing::setLastactive(const LastactiveOptional &x) {
	this->lastactive_ = x;
}

void IsComposing::setLastactive(::std::unique_ptr<LastactiveType> x) {
	this->lastactive_.set(std::move(x));
}

const IsComposing::ContenttypeOptional &IsComposing::getContenttype() const {
	return this->contenttype_;
}

IsComposing::ContenttypeOptional &IsComposing::getContenttype() {
	return this->contenttype_;
}

void IsComposing::setContenttype(const ContenttypeType &x) {
	this->contenttype_.set(x);
}

void IsComposing::setContenttype(const ContenttypeOptional &x) {
	this->contenttype_ = x;
}

void IsComposing::setContenttype(::std::unique_ptr<ContenttypeType> x) {
	this->contenttype_.set(std::move(x));
}

const IsComposing::RefreshOptional &IsComposing::getRefresh() const {
	return this->refresh_;
}

IsComposing::RefreshOptional &IsComposing::getRefresh() {
	return this->refresh_;
}

void IsComposing::setRefresh(const RefreshType &x) {
	this->refresh_.set(x);
}

void IsComposing::setRefresh(const RefreshOptional &x) {
	this->refresh_ = x;
}

const IsComposing::AnySequence &IsComposing::getAny() const {
	return this->any_;
}

IsComposing::AnySequence &IsComposing::getAny() {
	return this->any_;
}

void IsComposing::setAny(const AnySequence &s) {
	this->any_ = s;
}

const ::xercesc::DOMDocument &IsComposing::getDomDocument() const {
	return *this->dom_document_;
}

::xercesc::DOMDocument &IsComposing::getDomDocument() {
	return *this->dom_document_;
}
} // namespace IsComposing
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
namespace IsComposing {
// IsComposing
//

IsComposing::IsComposing(const StateType &state)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(), dom_document_(::xsd::cxx::xml::dom::create_document<char>()),
      state_(state, this), lastactive_(this), contenttype_(this), refresh_(this), any_(this->getDomDocument()) {
}

IsComposing::IsComposing(const IsComposing &x,
                         ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                         ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(x, f, c), dom_document_(::xsd::cxx::xml::dom::create_document<char>()),
      state_(x.state_, f, this), lastactive_(x.lastactive_, f, this), contenttype_(x.contenttype_, f, this),
      refresh_(x.refresh_, f, this), any_(x.any_, this->getDomDocument()) {
}

IsComposing::IsComposing(const ::xercesc::DOMElement &e,
                         ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                         ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(e, f | ::LinphonePrivate::Xsd::XmlSchema::Flags::base, c),
      dom_document_(::xsd::cxx::xml::dom::create_document<char>()), state_(this), lastactive_(this), contenttype_(this),
      refresh_(this), any_(this->getDomDocument()) {
	if ((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::base) == 0) {
		::xsd::cxx::xml::dom::parser<char> p(e, true, false, false);
		this->parse(p, f);
	}
}

void IsComposing::parse(::xsd::cxx::xml::dom::parser<char> &p, ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	for (; p.more_content(); p.next_content(false)) {
		const ::xercesc::DOMElement &i(p.cur_element());
		const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(i));

		// state
		//
		if (n.name() == "state" && n.namespace_() == "urn:ietf:params:xml:ns:im-iscomposing") {
			::std::unique_ptr<StateType> r(StateTraits::create(i, f, this));

			if (!state_.present()) {
				this->state_.set(::std::move(r));
				continue;
			}
		}

		// lastactive
		//
		if (n.name() == "lastactive" && n.namespace_() == "urn:ietf:params:xml:ns:im-iscomposing") {
			::std::unique_ptr<LastactiveType> r(LastactiveTraits::create(i, f, this));

			if (!this->lastactive_) {
				this->lastactive_.set(::std::move(r));
				continue;
			}
		}

		// contenttype
		//
		if (n.name() == "contenttype" && n.namespace_() == "urn:ietf:params:xml:ns:im-iscomposing") {
			::std::unique_ptr<ContenttypeType> r(ContenttypeTraits::create(i, f, this));

			if (!this->contenttype_) {
				this->contenttype_.set(::std::move(r));
				continue;
			}
		}

		// refresh
		//
		if (n.name() == "refresh" && n.namespace_() == "urn:ietf:params:xml:ns:im-iscomposing") {
			if (!this->refresh_) {
				this->refresh_.set(RefreshTraits::create(i, f, this));
				continue;
			}
		}

		// any
		//
		if ((!n.namespace_().empty() && n.namespace_() != "urn:ietf:params:xml:ns:im-iscomposing")) {
			::xercesc::DOMElement *r(static_cast<::xercesc::DOMElement *>(
			    this->getDomDocument().importNode(const_cast<::xercesc::DOMElement *>(&i), true)));
			this->any_.push_back(r);
			continue;
		}

		break;
	}

	if (!state_.present()) {
		throw ::xsd::cxx::tree::expected_element<char>("state", "urn:ietf:params:xml:ns:im-iscomposing");
	}
}

IsComposing *IsComposing::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f,
                                 ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class IsComposing(*this, f, c);
}

IsComposing &IsComposing::operator=(const IsComposing &x) {
	if (this != &x) {
		static_cast<::LinphonePrivate::Xsd::XmlSchema::Type &>(*this) = x;
		this->state_ = x.state_;
		this->lastactive_ = x.lastactive_;
		this->contenttype_ = x.contenttype_;
		this->refresh_ = x.refresh_;
		this->any_ = x.any_;
	}

	return *this;
}

IsComposing::~IsComposing() {
}
} // namespace IsComposing
} // namespace Xsd
} // namespace LinphonePrivate

#include <ostream>

#include <xsd/cxx/tree/std-ostream-map.hxx>

namespace _xsd {
static const ::xsd::cxx::tree::std_ostream_plate<0, char> std_ostream_plate_init;
}

namespace LinphonePrivate {
namespace Xsd {
namespace IsComposing {
::std::ostream &operator<<(::std::ostream &o, const IsComposing &i) {
	o << ::std::endl << "state: " << i.getState();
	if (i.getLastactive()) {
		o << ::std::endl << "lastactive: " << *i.getLastactive();
	}

	if (i.getContenttype()) {
		o << ::std::endl << "contenttype: " << *i.getContenttype();
	}

	if (i.getRefresh()) {
		o << ::std::endl << "refresh: " << *i.getRefresh();
	}

	return o;
}
} // namespace IsComposing
} // namespace Xsd
} // namespace LinphonePrivate

#include <istream>
#include <xsd/cxx/tree/error-handler.hxx>
#include <xsd/cxx/xml/sax/std-input-source.hxx>

namespace LinphonePrivate {
namespace Xsd {
namespace IsComposing {
::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>
parseIsComposing(const ::std::string &u,
                 ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                 const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::tree::error_handler<char> h;

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(u, h, p, f));

	h.throw_if_failed<::xsd::cxx::tree::parsing<char>>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>(
	    ::LinphonePrivate::Xsd::IsComposing::parseIsComposing(
	        std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>
parseIsComposing(const ::std::string &u,
                 ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
                 ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                 const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(u, h, p, f));

	if (!d.get()) throw ::xsd::cxx::tree::parsing<char>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>(
	    ::LinphonePrivate::Xsd::IsComposing::parseIsComposing(
	        std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>
parseIsComposing(const ::std::string &u,
                 ::xercesc::DOMErrorHandler &h,
                 ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                 const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(u, h, p, f));

	if (!d.get()) throw ::xsd::cxx::tree::parsing<char>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>(
	    ::LinphonePrivate::Xsd::IsComposing::parseIsComposing(
	        std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>
parseIsComposing(::std::istream &is,
                 ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                 const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::xml::sax::std_input_source isrc(is);
	return ::LinphonePrivate::Xsd::IsComposing::parseIsComposing(isrc, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>
parseIsComposing(::std::istream &is,
                 ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
                 ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                 const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::xml::sax::std_input_source isrc(is);
	return ::LinphonePrivate::Xsd::IsComposing::parseIsComposing(isrc, h, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>
parseIsComposing(::std::istream &is,
                 ::xercesc::DOMErrorHandler &h,
                 ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                 const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::sax::std_input_source isrc(is);
	return ::LinphonePrivate::Xsd::IsComposing::parseIsComposing(isrc, h, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>
parseIsComposing(::std::istream &is,
                 const ::std::string &sid,
                 ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                 const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::xml::sax::std_input_source isrc(is, sid);
	return ::LinphonePrivate::Xsd::IsComposing::parseIsComposing(isrc, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>
parseIsComposing(::std::istream &is,
                 const ::std::string &sid,
                 ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
                 ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                 const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::xml::sax::std_input_source isrc(is, sid);
	return ::LinphonePrivate::Xsd::IsComposing::parseIsComposing(isrc, h, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>
parseIsComposing(::std::istream &is,
                 const ::std::string &sid,
                 ::xercesc::DOMErrorHandler &h,
                 ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                 const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::sax::std_input_source isrc(is, sid);
	return ::LinphonePrivate::Xsd::IsComposing::parseIsComposing(isrc, h, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>
parseIsComposing(::xercesc::InputSource &i,
                 ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                 const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::tree::error_handler<char> h;

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(i, h, p, f));

	h.throw_if_failed<::xsd::cxx::tree::parsing<char>>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>(
	    ::LinphonePrivate::Xsd::IsComposing::parseIsComposing(
	        std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>
parseIsComposing(::xercesc::InputSource &i,
                 ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
                 ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                 const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(i, h, p, f));

	if (!d.get()) throw ::xsd::cxx::tree::parsing<char>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>(
	    ::LinphonePrivate::Xsd::IsComposing::parseIsComposing(
	        std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>
parseIsComposing(::xercesc::InputSource &i,
                 ::xercesc::DOMErrorHandler &h,
                 ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                 const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(i, h, p, f));

	if (!d.get()) throw ::xsd::cxx::tree::parsing<char>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>(
	    ::LinphonePrivate::Xsd::IsComposing::parseIsComposing(
	        std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>
parseIsComposing(const ::xercesc::DOMDocument &doc,
                 ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                 const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	if (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) {
		::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
		    static_cast<::xercesc::DOMDocument *>(doc.cloneNode(true)));

		return ::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>(
		    ::LinphonePrivate::Xsd::IsComposing::parseIsComposing(
		        std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
	}

	const ::xercesc::DOMElement &e(*doc.getDocumentElement());
	const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(e));

	if (n.name() == "isComposing" && n.namespace_() == "urn:ietf:params:xml:ns:im-iscomposing") {
		::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing> r(
		    ::xsd::cxx::tree::traits<::LinphonePrivate::Xsd::IsComposing::IsComposing, char>::create(e, f, 0));
		return r;
	}

	throw ::xsd::cxx::tree::unexpected_element<char>(n.name(), n.namespace_(), "isComposing",
	                                                 "urn:ietf:params:xml:ns:im-iscomposing");
}

::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing>
parseIsComposing(::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d,
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

	if (n.name() == "isComposing" && n.namespace_() == "urn:ietf:params:xml:ns:im-iscomposing") {
		::std::unique_ptr<::LinphonePrivate::Xsd::IsComposing::IsComposing> r(
		    ::xsd::cxx::tree::traits<::LinphonePrivate::Xsd::IsComposing::IsComposing, char>::create(e, f, 0));
		return r;
	}

	throw ::xsd::cxx::tree::unexpected_element<char>(n.name(), n.namespace_(), "isComposing",
	                                                 "urn:ietf:params:xml:ns:im-iscomposing");
}
} // namespace IsComposing
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
namespace IsComposing {
void serializeIsComposing(::std::ostream &o,
                          const ::LinphonePrivate::Xsd::IsComposing::IsComposing &s,
                          const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                          const ::std::string &e,
                          ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0);

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::IsComposing::serializeIsComposing(s, m, f));

	::xsd::cxx::tree::error_handler<char> h;

	::xsd::cxx::xml::dom::ostream_format_target t(o);
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		h.throw_if_failed<::xsd::cxx::tree::serialization<char>>();
	}
}

void serializeIsComposing(::std::ostream &o,
                          const ::LinphonePrivate::Xsd::IsComposing::IsComposing &s,
                          ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
                          const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                          const ::std::string &e,
                          ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0);

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::IsComposing::serializeIsComposing(s, m, f));
	::xsd::cxx::xml::dom::ostream_format_target t(o);
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		throw ::xsd::cxx::tree::serialization<char>();
	}
}

void serializeIsComposing(::std::ostream &o,
                          const ::LinphonePrivate::Xsd::IsComposing::IsComposing &s,
                          ::xercesc::DOMErrorHandler &h,
                          const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                          const ::std::string &e,
                          ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::IsComposing::serializeIsComposing(s, m, f));
	::xsd::cxx::xml::dom::ostream_format_target t(o);
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		throw ::xsd::cxx::tree::serialization<char>();
	}
}

void serializeIsComposing(::xercesc::XMLFormatTarget &t,
                          const ::LinphonePrivate::Xsd::IsComposing::IsComposing &s,
                          const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                          const ::std::string &e,
                          ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::IsComposing::serializeIsComposing(s, m, f));

	::xsd::cxx::tree::error_handler<char> h;

	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		h.throw_if_failed<::xsd::cxx::tree::serialization<char>>();
	}
}

void serializeIsComposing(::xercesc::XMLFormatTarget &t,
                          const ::LinphonePrivate::Xsd::IsComposing::IsComposing &s,
                          ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
                          const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                          const ::std::string &e,
                          ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::IsComposing::serializeIsComposing(s, m, f));
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		throw ::xsd::cxx::tree::serialization<char>();
	}
}

void serializeIsComposing(::xercesc::XMLFormatTarget &t,
                          const ::LinphonePrivate::Xsd::IsComposing::IsComposing &s,
                          ::xercesc::DOMErrorHandler &h,
                          const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                          const ::std::string &e,
                          ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::IsComposing::serializeIsComposing(s, m, f));
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		throw ::xsd::cxx::tree::serialization<char>();
	}
}

void serializeIsComposing(::xercesc::DOMDocument &d,
                          const ::LinphonePrivate::Xsd::IsComposing::IsComposing &s,
                          ::LinphonePrivate::Xsd::XmlSchema::Flags) {
	::xercesc::DOMElement &e(*d.getDocumentElement());
	const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(e));

	if (n.name() == "isComposing" && n.namespace_() == "urn:ietf:params:xml:ns:im-iscomposing") {
		e << s;
	} else {
		throw ::xsd::cxx::tree::unexpected_element<char>(n.name(), n.namespace_(), "isComposing",
		                                                 "urn:ietf:params:xml:ns:im-iscomposing");
	}
}

::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument>
serializeIsComposing(const ::LinphonePrivate::Xsd::IsComposing::IsComposing &s,
                     const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                     ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::serialize<char>("isComposing", "urn:ietf:params:xml:ns:im-iscomposing", m, f));

	::LinphonePrivate::Xsd::IsComposing::serializeIsComposing(*d, s, f);
	return d;
}

void operator<<(::xercesc::DOMElement &e, const IsComposing &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::Type &>(i);

	// state
	//
	{
		::xercesc::DOMElement &s(
		    ::xsd::cxx::xml::dom::create_element("state", "urn:ietf:params:xml:ns:im-iscomposing", e));

		s << i.getState();
	}

	// lastactive
	//
	if (i.getLastactive()) {
		::xercesc::DOMElement &s(
		    ::xsd::cxx::xml::dom::create_element("lastactive", "urn:ietf:params:xml:ns:im-iscomposing", e));

		s << *i.getLastactive();
	}

	// contenttype
	//
	if (i.getContenttype()) {
		::xercesc::DOMElement &s(
		    ::xsd::cxx::xml::dom::create_element("contenttype", "urn:ietf:params:xml:ns:im-iscomposing", e));

		s << *i.getContenttype();
	}

	// refresh
	//
	if (i.getRefresh()) {
		::xercesc::DOMElement &s(
		    ::xsd::cxx::xml::dom::create_element("refresh", "urn:ietf:params:xml:ns:im-iscomposing", e));

		s << *i.getRefresh();
	}

	// any
	//
	for (IsComposing::AnyConstIterator b(i.getAny().begin()), n(i.getAny().end()); b != n; ++b) {
		e.appendChild(e.getOwnerDocument()->importNode(const_cast<::xercesc::DOMElement *>(&(*b)), true));
	}
}
} // namespace IsComposing
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
