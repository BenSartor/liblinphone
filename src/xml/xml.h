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

#ifndef XML_H
#define XML_H

#ifndef XSD_CXX11
#define XSD_CXX11
#endif

#ifndef XSD_USE_CHAR
#define XSD_USE_CHAR
#endif

#ifndef XSD_CXX_TREE_USE_CHAR
#define XSD_CXX_TREE_USE_CHAR
#endif

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

#include <xsd/cxx/config.hxx>

#if (XSD_INT_VERSION != 4000000L)
#error XSD runtime version mismatch
#endif

#include <xsd/cxx/pre.hxx>

#include <xsd/cxx/xml/char-utf8.hxx>

#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/exceptions.hxx>
#include <xsd/cxx/tree/types.hxx>

#include <xsd/cxx/xml/error-handler.hxx>

#include <xsd/cxx/xml/dom/auto-ptr.hxx>

#include <xsd/cxx/tree/parsing.hxx>
#include <xsd/cxx/tree/parsing/boolean.hxx>
#include <xsd/cxx/tree/parsing/byte.hxx>
#include <xsd/cxx/tree/parsing/decimal.hxx>
#include <xsd/cxx/tree/parsing/double.hxx>
#include <xsd/cxx/tree/parsing/float.hxx>
#include <xsd/cxx/tree/parsing/int.hxx>
#include <xsd/cxx/tree/parsing/long.hxx>
#include <xsd/cxx/tree/parsing/short.hxx>
#include <xsd/cxx/tree/parsing/unsigned-byte.hxx>
#include <xsd/cxx/tree/parsing/unsigned-int.hxx>
#include <xsd/cxx/tree/parsing/unsigned-long.hxx>
#include <xsd/cxx/tree/parsing/unsigned-short.hxx>

#include <xsd/cxx/tree/serialization.hxx>
#include <xsd/cxx/tree/serialization/boolean.hxx>
#include <xsd/cxx/tree/serialization/byte.hxx>
#include <xsd/cxx/tree/serialization/decimal.hxx>
#include <xsd/cxx/tree/serialization/double.hxx>
#include <xsd/cxx/tree/serialization/float.hxx>
#include <xsd/cxx/tree/serialization/int.hxx>
#include <xsd/cxx/tree/serialization/long.hxx>
#include <xsd/cxx/tree/serialization/short.hxx>
#include <xsd/cxx/tree/serialization/unsigned-byte.hxx>
#include <xsd/cxx/tree/serialization/unsigned-int.hxx>
#include <xsd/cxx/tree/serialization/unsigned-long.hxx>
#include <xsd/cxx/tree/serialization/unsigned-short.hxx>
#include <xsd/cxx/xml/dom/serialization-header.hxx>

#include <xsd/cxx/tree/std-ostream-operators.hxx>

namespace LinphonePrivate {
namespace Xsd {
namespace XmlSchema {
// anyType and anySimpleType.
//
typedef ::xsd::cxx::tree::type Type;
typedef ::xsd::cxx::tree::simple_type<char, Type> SimpleType;
typedef ::xsd::cxx::tree::type Container;

// 8-bit
//
typedef signed char Byte;
typedef unsigned char UnsignedByte;

// 16-bit
//
typedef short Short;
typedef unsigned short UnsignedShort;

// 32-bit
//
typedef int Int;
typedef unsigned int UnsignedInt;

// 64-bit
//
typedef long long Long;
typedef unsigned long long UnsignedLong;

// Supposed to be arbitrary-length integral types.
//
typedef long long Integer;
typedef long long NonPositiveInteger;
typedef unsigned long long NonNegativeInteger;
typedef unsigned long long PositiveInteger;
typedef long long NegativeInteger;

// Boolean.
//
typedef bool Boolean;

// Floating-point types.
//
typedef float Float;
typedef double Double;
typedef double Decimal;

// String types.
//
typedef ::xsd::cxx::tree::string<char, SimpleType> String;
typedef ::xsd::cxx::tree::normalized_string<char, String> NormalizedString;
typedef ::xsd::cxx::tree::token<char, NormalizedString> Token;
typedef ::xsd::cxx::tree::name<char, Token> Name;
typedef ::xsd::cxx::tree::nmtoken<char, Token> Nmtoken;
typedef ::xsd::cxx::tree::nmtokens<char, SimpleType, Nmtoken> Nmtokens;
typedef ::xsd::cxx::tree::ncname<char, Name> Ncname;
typedef ::xsd::cxx::tree::language<char, Token> Language;

// ID/IDREF.
//
typedef ::xsd::cxx::tree::id<char, Ncname> Id;
typedef ::xsd::cxx::tree::idref<char, Ncname, Type> Idref;
typedef ::xsd::cxx::tree::idrefs<char, SimpleType, Idref> Idrefs;

// URI.
//
typedef ::xsd::cxx::tree::uri<char, SimpleType> Uri;

// Qualified name.
//
typedef ::xsd::cxx::tree::qname<char, SimpleType, Uri, Ncname> Qname;

// Binary.
//
typedef ::xsd::cxx::tree::buffer<char> Buffer;
typedef ::xsd::cxx::tree::base64_binary<char, SimpleType> Base64Binary;
typedef ::xsd::cxx::tree::hex_binary<char, SimpleType> HexBinary;

// Date/time.
//
typedef ::xsd::cxx::tree::time_zone TimeZone;
typedef ::xsd::cxx::tree::date<char, SimpleType> Date;
typedef ::xsd::cxx::tree::date_time<char, SimpleType> DateTime;
typedef ::xsd::cxx::tree::duration<char, SimpleType> Duration;
typedef ::xsd::cxx::tree::gday<char, SimpleType> Gday;
typedef ::xsd::cxx::tree::gmonth<char, SimpleType> Gmonth;
typedef ::xsd::cxx::tree::gmonth_day<char, SimpleType> GmonthDay;
typedef ::xsd::cxx::tree::gyear<char, SimpleType> Gyear;
typedef ::xsd::cxx::tree::gyear_month<char, SimpleType> GyearMonth;
typedef ::xsd::cxx::tree::time<char, SimpleType> Time;

// Entity.
//
typedef ::xsd::cxx::tree::entity<char, Ncname> Entity;
typedef ::xsd::cxx::tree::entities<char, SimpleType, Entity> Entities;

typedef ::xsd::cxx::tree::content_order ContentOrder;
// Namespace information and list stream. Used in
// serialization functions.
//
typedef ::xsd::cxx::xml::dom::namespace_info<char> NamespaceInfo;
typedef ::xsd::cxx::xml::dom::namespace_infomap<char> NamespaceInfomap;
typedef ::xsd::cxx::tree::list_stream<char> ListStream;
typedef ::xsd::cxx::tree::as_double<Double> AsDouble;
typedef ::xsd::cxx::tree::as_decimal<Decimal> AsDecimal;
typedef ::xsd::cxx::tree::facet Facet;

// Flags and properties.
//
typedef ::xsd::cxx::tree::flags Flags;
typedef ::xsd::cxx::tree::properties<char> Properties;

// Parsing/serialization diagnostics.
//
typedef ::xsd::cxx::tree::severity Severity;
typedef ::xsd::cxx::tree::error<char> Error;
typedef ::xsd::cxx::tree::diagnostics<char> Diagnostics;

// Exceptions.
//
typedef ::xsd::cxx::tree::exception<char> Exception;
typedef ::xsd::cxx::tree::bounds<char> Bounds;
typedef ::xsd::cxx::tree::duplicate_id<char> DuplicateId;
typedef ::xsd::cxx::tree::parsing<char> Parsing;
typedef ::xsd::cxx::tree::expected_element<char> ExpectedElement;
typedef ::xsd::cxx::tree::unexpected_element<char> UnexpectedElement;
typedef ::xsd::cxx::tree::expected_attribute<char> ExpectedAttribute;
typedef ::xsd::cxx::tree::unexpected_enumerator<char> UnexpectedEnumerator;
typedef ::xsd::cxx::tree::expected_text_content<char> ExpectedTextContent;
typedef ::xsd::cxx::tree::no_prefix_mapping<char> NoPrefixMapping;
typedef ::xsd::cxx::tree::no_type_info<char> NoTypeInfo;
typedef ::xsd::cxx::tree::not_derived<char> NotDerived;
typedef ::xsd::cxx::tree::serialization<char> Serialization;

// Error handler callback interface.
//
typedef ::xsd::cxx::xml::error_handler<char> ErrorHandler;

// DOM interaction.
//
namespace dom {
// Automatic pointer for DOMDocument.
//
using ::xsd::cxx::xml::dom::unique_ptr;

#ifndef XSD_CXX_TREE_TREE_NODE_KEY__LINPHONEPRIVATE__XSD__XMLSCHEMA
#define XSD_CXX_TREE_TREE_NODE_KEY__LINPHONEPRIVATE__XSD__XMLSCHEMA
// DOM user data key for back pointers to tree nodes.
//
const XMLCh *const treeNodeKey = ::xsd::cxx::tree::user_data_keys::node;
#endif
} // namespace dom
} // namespace XmlSchema
} // namespace Xsd
} // namespace LinphonePrivate

// Forward declarations.
//
namespace namespace_ {
class Lang;
class Space;
class Lang_member;
} // namespace namespace_

#include <algorithm> // std::binary_search
#include <limits>    // std::numeric_limits
#include <memory>    // ::std::unique_ptr
#include <utility>   // std::move

#include <xsd/cxx/xml/char-utf8.hxx>

#include <xsd/cxx/tree/containers.hxx>
#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/exceptions.hxx>
#include <xsd/cxx/tree/list.hxx>

#include <xsd/cxx/xml/dom/parsing-header.hxx>

#include <xsd/cxx/tree/containers-wildcard.hxx>

namespace namespace_ {
class Lang : public ::LinphonePrivate::Xsd::XmlSchema::String {
public:
	Lang(const char *v);

	Lang(const ::std::string &v);

	Lang(const ::xercesc::DOMElement &e,
	     ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	     ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	Lang(const ::xercesc::DOMAttr &a,
	     ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	     ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	Lang(const ::std::string &s,
	     const ::xercesc::DOMElement *e,
	     ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	     ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	Lang(const Lang &x,
	     ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	     ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	virtual Lang *_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	                     ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0) const;
};

class Space : public ::LinphonePrivate::Xsd::XmlSchema::Ncname {
public:
	enum Value { default_, preserve };

	Space(Value v);

	Space(const char *v);

	Space(const ::std::string &v);

	Space(const ::LinphonePrivate::Xsd::XmlSchema::Ncname &v);

	Space(const ::xercesc::DOMElement &e,
	      ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	      ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	Space(const ::xercesc::DOMAttr &a,
	      ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	      ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	Space(const ::std::string &s,
	      const ::xercesc::DOMElement *e,
	      ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	      ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	Space(const Space &x,
	      ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	      ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	virtual Space *_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	                      ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0) const;

	Space &operator=(Value v);

	virtual operator Value() const {
		return _xsd_Space_convert();
	}

protected:
	Value _xsd_Space_convert() const;

public:
	static const char *const _xsd_Space_literals_[2];
	static const Value _xsd_Space_indexes_[2];
};

class Lang_member : public ::LinphonePrivate::Xsd::XmlSchema::String {
public:
	enum Value { empty };

	Lang_member(Value v);

	Lang_member(const char *v);

	Lang_member(const ::std::string &v);

	Lang_member(const ::LinphonePrivate::Xsd::XmlSchema::String &v);

	Lang_member(const ::xercesc::DOMElement &e,
	            ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	            ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	Lang_member(const ::xercesc::DOMAttr &a,
	            ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	            ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	Lang_member(const ::std::string &s,
	            const ::xercesc::DOMElement *e,
	            ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	            ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	Lang_member(const Lang_member &x,
	            ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	            ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	virtual Lang_member *_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	                            ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0) const;

	Lang_member &operator=(Value v);

	virtual operator Value() const {
		return _xsd_Lang_member_convert();
	}

protected:
	Value _xsd_Lang_member_convert() const;

public:
	static const char *const _xsd_Lang_member_literals_[1];
	static const Value _xsd_Lang_member_indexes_[1];
};
} // namespace namespace_

#include <iosfwd>

namespace namespace_ {
::std::ostream &operator<<(::std::ostream &, const Lang &);

::std::ostream &operator<<(::std::ostream &, Space::Value);

::std::ostream &operator<<(::std::ostream &, const Space &);

::std::ostream &operator<<(::std::ostream &, Lang_member::Value);

::std::ostream &operator<<(::std::ostream &, const Lang_member &);
} // namespace namespace_

#include <iosfwd>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/sax/InputSource.hpp>

namespace namespace_ {}

#include <iosfwd>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/framework/XMLFormatter.hpp>

#include <xsd/cxx/xml/dom/auto-ptr.hxx>

namespace namespace_ {
void operator<<(::xercesc::DOMElement &, const Lang &);

void operator<<(::xercesc::DOMAttr &, const Lang &);

void operator<<(::LinphonePrivate::Xsd::XmlSchema::ListStream &, const Lang &);

void operator<<(::xercesc::DOMElement &, const Space &);

void operator<<(::xercesc::DOMAttr &, const Space &);

void operator<<(::LinphonePrivate::Xsd::XmlSchema::ListStream &, const Space &);

void operator<<(::xercesc::DOMElement &, const Lang_member &);

void operator<<(::xercesc::DOMAttr &, const Lang_member &);

void operator<<(::LinphonePrivate::Xsd::XmlSchema::ListStream &, const Lang_member &);
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

#endif // XML_H
