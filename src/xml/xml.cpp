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

#include "xml.h"

namespace namespace_ {
// Lang
//

Lang::Lang(const char *s) : ::LinphonePrivate::Xsd::XmlSchema::String(s) {
}

Lang::Lang(const ::std::string &s) : ::LinphonePrivate::Xsd::XmlSchema::String(s) {
}

Lang::Lang(const Lang &o, ::LinphonePrivate::Xsd::XmlSchema::Flags f, ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(o, f, c) {
}

// Space
//

Space::Space(Value v) : ::LinphonePrivate::Xsd::XmlSchema::Ncname(_xsd_Space_literals_[v]) {
}

Space::Space(const char *v) : ::LinphonePrivate::Xsd::XmlSchema::Ncname(v) {
}

Space::Space(const ::std::string &v) : ::LinphonePrivate::Xsd::XmlSchema::Ncname(v) {
}

Space::Space(const ::LinphonePrivate::Xsd::XmlSchema::Ncname &v) : ::LinphonePrivate::Xsd::XmlSchema::Ncname(v) {
}

Space::Space(const Space &v,
             ::LinphonePrivate::Xsd::XmlSchema::Flags f,
             ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Ncname(v, f, c) {
}

Space &Space::operator=(Value v) {
	static_cast<::LinphonePrivate::Xsd::XmlSchema::Ncname &>(*this) =
	    ::LinphonePrivate::Xsd::XmlSchema::Ncname(_xsd_Space_literals_[v]);

	return *this;
}

// Lang_member
//

Lang_member::Lang_member(Value v) : ::LinphonePrivate::Xsd::XmlSchema::String(_xsd_Lang_member_literals_[v]) {
}

Lang_member::Lang_member(const char *v) : ::LinphonePrivate::Xsd::XmlSchema::String(v) {
}

Lang_member::Lang_member(const ::std::string &v) : ::LinphonePrivate::Xsd::XmlSchema::String(v) {
}

Lang_member::Lang_member(const ::LinphonePrivate::Xsd::XmlSchema::String &v)
    : ::LinphonePrivate::Xsd::XmlSchema::String(v) {
}

Lang_member::Lang_member(const Lang_member &v,
                         ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                         ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(v, f, c) {
}

Lang_member &Lang_member::operator=(Value v) {
	static_cast<::LinphonePrivate::Xsd::XmlSchema::String &>(*this) =
	    ::LinphonePrivate::Xsd::XmlSchema::String(_xsd_Lang_member_literals_[v]);

	return *this;
}
} // namespace namespace_

#include <xsd/cxx/xml/dom/wildcard-source.hxx>

#include <xsd/cxx/xml/dom/parsing-source.hxx>

#include <xsd/cxx/tree/type-factory-map.hxx>

namespace _xsd {
static const ::xsd::cxx::tree::type_factory_plate<0, char> type_factory_plate_init;
}

namespace namespace_ {
// Lang
//

Lang::Lang(const ::xercesc::DOMElement &e,
           ::LinphonePrivate::Xsd::XmlSchema::Flags f,
           ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(e, f, c) {
}

Lang::Lang(const ::xercesc::DOMAttr &a,
           ::LinphonePrivate::Xsd::XmlSchema::Flags f,
           ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(a, f, c) {
}

Lang::Lang(const ::std::string &s,
           const ::xercesc::DOMElement *e,
           ::LinphonePrivate::Xsd::XmlSchema::Flags f,
           ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(s, e, f, c) {
}

Lang *Lang::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f, ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class Lang(*this, f, c);
}

// Space
//

Space::Space(const ::xercesc::DOMElement &e,
             ::LinphonePrivate::Xsd::XmlSchema::Flags f,
             ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Ncname(e, f, c) {
	_xsd_Space_convert();
}

Space::Space(const ::xercesc::DOMAttr &a,
             ::LinphonePrivate::Xsd::XmlSchema::Flags f,
             ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Ncname(a, f, c) {
	_xsd_Space_convert();
}

Space::Space(const ::std::string &s,
             const ::xercesc::DOMElement *e,
             ::LinphonePrivate::Xsd::XmlSchema::Flags f,
             ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Ncname(s, e, f, c) {
	_xsd_Space_convert();
}

Space *Space::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f,
                     ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class Space(*this, f, c);
}

Space::Value Space::_xsd_Space_convert() const {
	::xsd::cxx::tree::enum_comparator<char> c(_xsd_Space_literals_);
	const Value *i(::std::lower_bound(_xsd_Space_indexes_, _xsd_Space_indexes_ + 2, *this, c));

	if (i == _xsd_Space_indexes_ + 2 || _xsd_Space_literals_[*i] != *this) {
		throw ::xsd::cxx::tree::unexpected_enumerator<char>(*this);
	}

	return *i;
}

const char *const Space::_xsd_Space_literals_[2] = {"default", "preserve"};

const Space::Value Space::_xsd_Space_indexes_[2] = {::namespace_::Space::default_, ::namespace_::Space::preserve};

// Lang_member
//

Lang_member::Lang_member(const ::xercesc::DOMElement &e,
                         ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                         ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(e, f, c) {
	_xsd_Lang_member_convert();
}

Lang_member::Lang_member(const ::xercesc::DOMAttr &a,
                         ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                         ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(a, f, c) {
	_xsd_Lang_member_convert();
}

Lang_member::Lang_member(const ::std::string &s,
                         const ::xercesc::DOMElement *e,
                         ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                         ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(s, e, f, c) {
	_xsd_Lang_member_convert();
}

Lang_member *Lang_member::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f,
                                 ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class Lang_member(*this, f, c);
}

Lang_member::Value Lang_member::_xsd_Lang_member_convert() const {
	::xsd::cxx::tree::enum_comparator<char> c(_xsd_Lang_member_literals_);
	const Value *i(::std::lower_bound(_xsd_Lang_member_indexes_, _xsd_Lang_member_indexes_ + 1, *this, c));

	if (i == _xsd_Lang_member_indexes_ + 1 || _xsd_Lang_member_literals_[*i] != *this) {
		throw ::xsd::cxx::tree::unexpected_enumerator<char>(*this);
	}

	return *i;
}

const char *const Lang_member::_xsd_Lang_member_literals_[1] = {""};

const Lang_member::Value Lang_member::_xsd_Lang_member_indexes_[1] = {::namespace_::Lang_member::empty};
} // namespace namespace_

#include <ostream>

#include <xsd/cxx/tree/std-ostream-map.hxx>

namespace _xsd {
static const ::xsd::cxx::tree::std_ostream_plate<0, char> std_ostream_plate_init;
}

namespace namespace_ {
::std::ostream &operator<<(::std::ostream &o, const Lang &i) {
	return o << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);
}

::std::ostream &operator<<(::std::ostream &o, Space::Value i) {
	return o << Space::_xsd_Space_literals_[i];
}

::std::ostream &operator<<(::std::ostream &o, const Space &i) {
	return o << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::Ncname &>(i);
}

::std::ostream &operator<<(::std::ostream &o, Lang_member::Value i) {
	return o << Lang_member::_xsd_Lang_member_literals_[i];
}

::std::ostream &operator<<(::std::ostream &o, const Lang_member &i) {
	return o << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);
}
} // namespace namespace_

#include <istream>
#include <xsd/cxx/tree/error-handler.hxx>
#include <xsd/cxx/xml/sax/std-input-source.hxx>

namespace namespace_ {}

#include <ostream>
#include <xsd/cxx/tree/error-handler.hxx>
#include <xsd/cxx/xml/dom/serialization-source.hxx>

#include <xsd/cxx/tree/type-serializer-map.hxx>

namespace _xsd {
static const ::xsd::cxx::tree::type_serializer_plate<0, char> type_serializer_plate_init;
}

namespace namespace_ {
void operator<<(::xercesc::DOMElement &e, const Lang &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);
}

void operator<<(::xercesc::DOMAttr &a, const Lang &i) {
	a << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);
}

void operator<<(::LinphonePrivate::Xsd::XmlSchema::ListStream &l, const Lang &i) {
	l << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);
}

void operator<<(::xercesc::DOMElement &e, const Space &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::Ncname &>(i);
}

void operator<<(::xercesc::DOMAttr &a, const Space &i) {
	a << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::Ncname &>(i);
}

void operator<<(::LinphonePrivate::Xsd::XmlSchema::ListStream &l, const Space &i) {
	l << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::Ncname &>(i);
}

void operator<<(::xercesc::DOMElement &e, const Lang_member &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);
}

void operator<<(::xercesc::DOMAttr &a, const Lang_member &i) {
	a << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);
}

void operator<<(::LinphonePrivate::Xsd::XmlSchema::ListStream &l, const Lang_member &i) {
	l << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);
}
} // namespace namespace_

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
