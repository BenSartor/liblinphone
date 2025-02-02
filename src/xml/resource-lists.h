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

#ifndef RESOURCE_LISTS_H
#define RESOURCE_LISTS_H

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
namespace LinphonePrivate {
namespace Xsd {
namespace ResourceLists {
class ListType;
class EntryType;
class EntryRefType;
class ExternalType;
class DisplayNameType;
class List;
class DisplayName;
class ResourceLists;
} // namespace ResourceLists
} // namespace Xsd
} // namespace LinphonePrivate

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

#include "xml.h"

namespace LinphonePrivate {
namespace Xsd {
namespace ResourceLists {
class ListType : public ::LinphonePrivate::Xsd::XmlSchema::Type {
public:
	// display-name
	//
	typedef ::LinphonePrivate::Xsd::ResourceLists::DisplayNameType DisplayNameType;
	typedef ::xsd::cxx::tree::optional<DisplayNameType> DisplayNameOptional;
	typedef ::xsd::cxx::tree::traits<DisplayNameType, char> DisplayNameTraits;

	const DisplayNameOptional &getDisplayName() const;

	DisplayNameOptional &getDisplayName();

	void setDisplayName(const DisplayNameType &x);

	void setDisplayName(const DisplayNameOptional &x);

	void setDisplayName(::std::unique_ptr<DisplayNameType> p);

	// list
	//
	typedef ::LinphonePrivate::Xsd::ResourceLists::List ListType1;
	typedef ::xsd::cxx::tree::sequence<ListType1> ListSequence;
	typedef ListSequence::iterator ListIterator;
	typedef ListSequence::const_iterator ListConstIterator;
	typedef ::xsd::cxx::tree::traits<ListType1, char> ListTraits;

	const ListSequence &getList() const;

	ListSequence &getList();

	void setList(const ListSequence &s);

	// external
	//
	typedef ::LinphonePrivate::Xsd::ResourceLists::ExternalType ExternalType;
	typedef ::xsd::cxx::tree::sequence<ExternalType> ExternalSequence;
	typedef ExternalSequence::iterator ExternalIterator;
	typedef ExternalSequence::const_iterator ExternalConstIterator;
	typedef ::xsd::cxx::tree::traits<ExternalType, char> ExternalTraits;

	const ExternalSequence &getExternal() const;

	ExternalSequence &getExternal();

	void setExternal(const ExternalSequence &s);

	// entry
	//
	typedef ::LinphonePrivate::Xsd::ResourceLists::EntryType EntryType;
	typedef ::xsd::cxx::tree::sequence<EntryType> EntrySequence;
	typedef EntrySequence::iterator EntryIterator;
	typedef EntrySequence::const_iterator EntryConstIterator;
	typedef ::xsd::cxx::tree::traits<EntryType, char> EntryTraits;

	const EntrySequence &getEntry() const;

	EntrySequence &getEntry();

	void setEntry(const EntrySequence &s);

	// entry-ref
	//
	typedef ::LinphonePrivate::Xsd::ResourceLists::EntryRefType EntryRefType;
	typedef ::xsd::cxx::tree::sequence<EntryRefType> EntryRefSequence;
	typedef EntryRefSequence::iterator EntryRefIterator;
	typedef EntryRefSequence::const_iterator EntryRefConstIterator;
	typedef ::xsd::cxx::tree::traits<EntryRefType, char> EntryRefTraits;

	const EntryRefSequence &getEntryRef() const;

	EntryRefSequence &getEntryRef();

	void setEntryRef(const EntryRefSequence &s);

	// any
	//
	typedef ::xsd::cxx::tree::element_sequence AnySequence;
	typedef AnySequence::iterator AnyIterator;
	typedef AnySequence::const_iterator AnyConstIterator;

	const AnySequence &getAny() const;

	AnySequence &getAny();

	void setAny(const AnySequence &s);

	// name
	//
	typedef ::LinphonePrivate::Xsd::XmlSchema::String NameType;
	typedef ::xsd::cxx::tree::optional<NameType> NameOptional;
	typedef ::xsd::cxx::tree::traits<NameType, char> NameTraits;

	const NameOptional &getName() const;

	NameOptional &getName();

	void setName(const NameType &x);

	void setName(const NameOptional &x);

	void setName(::std::unique_ptr<NameType> p);

	// any_attribute
	//
	typedef ::xsd::cxx::tree::attribute_set<char> AnyAttributeSet;
	typedef AnyAttributeSet::iterator AnyAttributeIterator;
	typedef AnyAttributeSet::const_iterator AnyAttributeConstIterator;

	const AnyAttributeSet &getAnyAttribute() const;

	AnyAttributeSet &getAnyAttribute();

	void setAnyAttribute(const AnyAttributeSet &s);

	// DOMDocument for wildcard content.
	//
	const ::xercesc::DOMDocument &getDomDocument() const;

	::xercesc::DOMDocument &getDomDocument();

	// Constructors.
	//
	ListType();

	ListType(const ::xercesc::DOMElement &e,
	         ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	         ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	ListType(const ListType &x,
	         ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	         ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	virtual ListType *_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	                         ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0) const;

	ListType &operator=(const ListType &x);

	virtual ~ListType();

	// Implementation.
	//
protected:
	void parse(::xsd::cxx::xml::dom::parser<char> &, ::LinphonePrivate::Xsd::XmlSchema::Flags);

protected:
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> dom_document_;

	DisplayNameOptional display_name_;
	ListSequence list_;
	ExternalSequence external_;
	EntrySequence entry_;
	EntryRefSequence entry_ref_;
	AnySequence any_;
	NameOptional name_;
	AnyAttributeSet any_attribute_;
};

class EntryType : public ::LinphonePrivate::Xsd::XmlSchema::Type {
public:
	// display-name
	//
	typedef ::LinphonePrivate::Xsd::ResourceLists::DisplayName DisplayNameType;
	typedef ::xsd::cxx::tree::optional<DisplayNameType> DisplayNameOptional;
	typedef ::xsd::cxx::tree::traits<DisplayNameType, char> DisplayNameTraits;

	const DisplayNameOptional &getDisplayName() const;

	DisplayNameOptional &getDisplayName();

	void setDisplayName(const DisplayNameType &x);

	void setDisplayName(const DisplayNameOptional &x);

	void setDisplayName(::std::unique_ptr<DisplayNameType> p);

	// any
	//
	typedef ::xsd::cxx::tree::element_sequence AnySequence;
	typedef AnySequence::iterator AnyIterator;
	typedef AnySequence::const_iterator AnyConstIterator;

	const AnySequence &getAny() const;

	AnySequence &getAny();

	void setAny(const AnySequence &s);

	// uri
	//
	typedef ::LinphonePrivate::Xsd::XmlSchema::Uri UriType;
	typedef ::xsd::cxx::tree::traits<UriType, char> UriTraits;

	const UriType &getUri() const;

	UriType &getUri();

	void setUri(const UriType &x);

	void setUri(::std::unique_ptr<UriType> p);

	::std::unique_ptr<UriType> setDetachUri();

	// any_attribute
	//
	typedef ::xsd::cxx::tree::attribute_set<char> AnyAttributeSet;
	typedef AnyAttributeSet::iterator AnyAttributeIterator;
	typedef AnyAttributeSet::const_iterator AnyAttributeConstIterator;

	const AnyAttributeSet &getAnyAttribute() const;

	AnyAttributeSet &getAnyAttribute();

	void setAnyAttribute(const AnyAttributeSet &s);

	// DOMDocument for wildcard content.
	//
	const ::xercesc::DOMDocument &getDomDocument() const;

	::xercesc::DOMDocument &getDomDocument();

	// Constructors.
	//
	EntryType(const UriType &);

	EntryType(const ::xercesc::DOMElement &e,
	          ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	          ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	EntryType(const EntryType &x,
	          ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	          ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	virtual EntryType *_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	                          ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0) const;

	EntryType &operator=(const EntryType &x);

	virtual ~EntryType();

	// Implementation.
	//
protected:
	void parse(::xsd::cxx::xml::dom::parser<char> &, ::LinphonePrivate::Xsd::XmlSchema::Flags);

protected:
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> dom_document_;

	DisplayNameOptional display_name_;
	AnySequence any_;
	::xsd::cxx::tree::one<UriType> uri_;
	AnyAttributeSet any_attribute_;
};

class EntryRefType : public ::LinphonePrivate::Xsd::XmlSchema::Type {
public:
	// display-name
	//
	typedef ::LinphonePrivate::Xsd::ResourceLists::DisplayNameType DisplayNameType;
	typedef ::xsd::cxx::tree::optional<DisplayNameType> DisplayNameOptional;
	typedef ::xsd::cxx::tree::traits<DisplayNameType, char> DisplayNameTraits;

	const DisplayNameOptional &getDisplayName() const;

	DisplayNameOptional &getDisplayName();

	void setDisplayName(const DisplayNameType &x);

	void setDisplayName(const DisplayNameOptional &x);

	void setDisplayName(::std::unique_ptr<DisplayNameType> p);

	// any
	//
	typedef ::xsd::cxx::tree::element_sequence AnySequence;
	typedef AnySequence::iterator AnyIterator;
	typedef AnySequence::const_iterator AnyConstIterator;

	const AnySequence &getAny() const;

	AnySequence &getAny();

	void setAny(const AnySequence &s);

	// ref
	//
	typedef ::LinphonePrivate::Xsd::XmlSchema::Uri RefType;
	typedef ::xsd::cxx::tree::traits<RefType, char> RefTraits;

	const RefType &getRef() const;

	RefType &getRef();

	void setRef(const RefType &x);

	void setRef(::std::unique_ptr<RefType> p);

	::std::unique_ptr<RefType> setDetachRef();

	// any_attribute
	//
	typedef ::xsd::cxx::tree::attribute_set<char> AnyAttributeSet;
	typedef AnyAttributeSet::iterator AnyAttributeIterator;
	typedef AnyAttributeSet::const_iterator AnyAttributeConstIterator;

	const AnyAttributeSet &getAnyAttribute() const;

	AnyAttributeSet &getAnyAttribute();

	void setAnyAttribute(const AnyAttributeSet &s);

	// DOMDocument for wildcard content.
	//
	const ::xercesc::DOMDocument &getDomDocument() const;

	::xercesc::DOMDocument &getDomDocument();

	// Constructors.
	//
	EntryRefType(const RefType &);

	EntryRefType(const ::xercesc::DOMElement &e,
	             ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	             ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	EntryRefType(const EntryRefType &x,
	             ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	             ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	virtual EntryRefType *_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	                             ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0) const;

	EntryRefType &operator=(const EntryRefType &x);

	virtual ~EntryRefType();

	// Implementation.
	//
protected:
	void parse(::xsd::cxx::xml::dom::parser<char> &, ::LinphonePrivate::Xsd::XmlSchema::Flags);

protected:
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> dom_document_;

	DisplayNameOptional display_name_;
	AnySequence any_;
	::xsd::cxx::tree::one<RefType> ref_;
	AnyAttributeSet any_attribute_;
};

class ExternalType : public ::LinphonePrivate::Xsd::XmlSchema::Type {
public:
	// display-name
	//
	typedef ::LinphonePrivate::Xsd::ResourceLists::DisplayNameType DisplayNameType;
	typedef ::xsd::cxx::tree::optional<DisplayNameType> DisplayNameOptional;
	typedef ::xsd::cxx::tree::traits<DisplayNameType, char> DisplayNameTraits;

	const DisplayNameOptional &getDisplayName() const;

	DisplayNameOptional &getDisplayName();

	void setDisplayName(const DisplayNameType &x);

	void setDisplayName(const DisplayNameOptional &x);

	void setDisplayName(::std::unique_ptr<DisplayNameType> p);

	// any
	//
	typedef ::xsd::cxx::tree::element_sequence AnySequence;
	typedef AnySequence::iterator AnyIterator;
	typedef AnySequence::const_iterator AnyConstIterator;

	const AnySequence &getAny() const;

	AnySequence &getAny();

	void setAny(const AnySequence &s);

	// anchor
	//
	typedef ::LinphonePrivate::Xsd::XmlSchema::Uri AnchorType;
	typedef ::xsd::cxx::tree::optional<AnchorType> AnchorOptional;
	typedef ::xsd::cxx::tree::traits<AnchorType, char> AnchorTraits;

	const AnchorOptional &getAnchor() const;

	AnchorOptional &getAnchor();

	void setAnchor(const AnchorType &x);

	void setAnchor(const AnchorOptional &x);

	void setAnchor(::std::unique_ptr<AnchorType> p);

	// any_attribute
	//
	typedef ::xsd::cxx::tree::attribute_set<char> AnyAttributeSet;
	typedef AnyAttributeSet::iterator AnyAttributeIterator;
	typedef AnyAttributeSet::const_iterator AnyAttributeConstIterator;

	const AnyAttributeSet &getAnyAttribute() const;

	AnyAttributeSet &getAnyAttribute();

	void setAnyAttribute(const AnyAttributeSet &s);

	// DOMDocument for wildcard content.
	//
	const ::xercesc::DOMDocument &getDomDocument() const;

	::xercesc::DOMDocument &getDomDocument();

	// Constructors.
	//
	ExternalType();

	ExternalType(const ::xercesc::DOMElement &e,
	             ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	             ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	ExternalType(const ExternalType &x,
	             ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	             ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	virtual ExternalType *_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	                             ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0) const;

	ExternalType &operator=(const ExternalType &x);

	virtual ~ExternalType();

	// Implementation.
	//
protected:
	void parse(::xsd::cxx::xml::dom::parser<char> &, ::LinphonePrivate::Xsd::XmlSchema::Flags);

protected:
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> dom_document_;

	DisplayNameOptional display_name_;
	AnySequence any_;
	AnchorOptional anchor_;
	AnyAttributeSet any_attribute_;
};

class DisplayNameType : public ::LinphonePrivate::Xsd::XmlSchema::String {
public:
	// lang
	//
	typedef ::namespace_::Lang LangType;
	typedef ::xsd::cxx::tree::optional<LangType> LangOptional;
	typedef ::xsd::cxx::tree::traits<LangType, char> LangTraits;

	const LangOptional &getLang() const;

	LangOptional &getLang();

	void setLang(const LangType &x);

	void setLang(const LangOptional &x);

	void setLang(::std::unique_ptr<LangType> p);

	// Constructors.
	//
	DisplayNameType();

	DisplayNameType(const char *);

	DisplayNameType(const ::std::string &);

	DisplayNameType(const ::LinphonePrivate::Xsd::XmlSchema::String &);

	DisplayNameType(const ::xercesc::DOMElement &e,
	                ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	                ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	DisplayNameType(const DisplayNameType &x,
	                ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	                ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	virtual DisplayNameType *_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	                                ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0) const;

	DisplayNameType &operator=(const DisplayNameType &x);

	virtual ~DisplayNameType();

	// Implementation.
	//
protected:
	void parse(::xsd::cxx::xml::dom::parser<char> &, ::LinphonePrivate::Xsd::XmlSchema::Flags);

protected:
	LangOptional lang_;
};

class List : public ::LinphonePrivate::Xsd::ResourceLists::ListType {
public:
	// Constructors.
	//
	List();

	List(const ::xercesc::DOMElement &e,
	     ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	     ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	List(const List &x,
	     ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	     ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	virtual List *_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	                     ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0) const;

	virtual ~List();
};

class DisplayName : public ::LinphonePrivate::Xsd::ResourceLists::DisplayNameType {
public:
	// Constructors.
	//
	DisplayName();

	DisplayName(const char *);

	DisplayName(const ::std::string &);

	DisplayName(const ::LinphonePrivate::Xsd::XmlSchema::String &);

	DisplayName(const ::xercesc::DOMElement &e,
	            ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	            ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	DisplayName(const DisplayName &x,
	            ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	            ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	virtual DisplayName *_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	                            ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0) const;

	virtual ~DisplayName();
};

class ResourceLists : public ::LinphonePrivate::Xsd::XmlSchema::Type {
public:
	// list
	//
	typedef ::LinphonePrivate::Xsd::ResourceLists::ListType ListType;
	typedef ::xsd::cxx::tree::sequence<ListType> ListSequence;
	typedef ListSequence::iterator ListIterator;
	typedef ListSequence::const_iterator ListConstIterator;
	typedef ::xsd::cxx::tree::traits<ListType, char> ListTraits;

	const ListSequence &getList() const;

	ListSequence &getList();

	void setList(const ListSequence &s);

	// Constructors.
	//
	ResourceLists();

	ResourceLists(const ::xercesc::DOMElement &e,
	              ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	              ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	ResourceLists(const ResourceLists &x,
	              ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	              ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0);

	virtual ResourceLists *_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
	                              ::LinphonePrivate::Xsd::XmlSchema::Container *c = 0) const;

	ResourceLists &operator=(const ResourceLists &x);

	virtual ~ResourceLists();

	// Implementation.
	//
protected:
	void parse(::xsd::cxx::xml::dom::parser<char> &, ::LinphonePrivate::Xsd::XmlSchema::Flags);

protected:
	ListSequence list_;
};
} // namespace ResourceLists
} // namespace Xsd
} // namespace LinphonePrivate

#include <iosfwd>

namespace LinphonePrivate {
namespace Xsd {
namespace ResourceLists {
::std::ostream &operator<<(::std::ostream &, const ListType &);

::std::ostream &operator<<(::std::ostream &, const EntryType &);

::std::ostream &operator<<(::std::ostream &, const EntryRefType &);

::std::ostream &operator<<(::std::ostream &, const ExternalType &);

::std::ostream &operator<<(::std::ostream &, const DisplayNameType &);

::std::ostream &operator<<(::std::ostream &, const List &);

::std::ostream &operator<<(::std::ostream &, const DisplayName &);

::std::ostream &operator<<(::std::ostream &, const ResourceLists &);
} // namespace ResourceLists
} // namespace Xsd
} // namespace LinphonePrivate

#include <iosfwd>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/sax/InputSource.hpp>

namespace LinphonePrivate {
namespace Xsd {
namespace ResourceLists {
// Parse a URI or a local file.
//

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists> parseResourceLists(
    const ::std::string &uri,
    ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
    const ::LinphonePrivate::Xsd::XmlSchema::Properties &p = ::LinphonePrivate::Xsd::XmlSchema::Properties());

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists> parseResourceLists(
    const ::std::string &uri,
    ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &eh,
    ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
    const ::LinphonePrivate::Xsd::XmlSchema::Properties &p = ::LinphonePrivate::Xsd::XmlSchema::Properties());

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists> parseResourceLists(
    const ::std::string &uri,
    ::xercesc::DOMErrorHandler &eh,
    ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
    const ::LinphonePrivate::Xsd::XmlSchema::Properties &p = ::LinphonePrivate::Xsd::XmlSchema::Properties());

// Parse std::istream.
//

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists> parseResourceLists(
    ::std::istream &is,
    ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
    const ::LinphonePrivate::Xsd::XmlSchema::Properties &p = ::LinphonePrivate::Xsd::XmlSchema::Properties());

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists> parseResourceLists(
    ::std::istream &is,
    ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &eh,
    ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
    const ::LinphonePrivate::Xsd::XmlSchema::Properties &p = ::LinphonePrivate::Xsd::XmlSchema::Properties());

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists> parseResourceLists(
    ::std::istream &is,
    ::xercesc::DOMErrorHandler &eh,
    ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
    const ::LinphonePrivate::Xsd::XmlSchema::Properties &p = ::LinphonePrivate::Xsd::XmlSchema::Properties());

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists> parseResourceLists(
    ::std::istream &is,
    const ::std::string &id,
    ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
    const ::LinphonePrivate::Xsd::XmlSchema::Properties &p = ::LinphonePrivate::Xsd::XmlSchema::Properties());

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists> parseResourceLists(
    ::std::istream &is,
    const ::std::string &id,
    ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &eh,
    ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
    const ::LinphonePrivate::Xsd::XmlSchema::Properties &p = ::LinphonePrivate::Xsd::XmlSchema::Properties());

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists> parseResourceLists(
    ::std::istream &is,
    const ::std::string &id,
    ::xercesc::DOMErrorHandler &eh,
    ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
    const ::LinphonePrivate::Xsd::XmlSchema::Properties &p = ::LinphonePrivate::Xsd::XmlSchema::Properties());

// Parse xercesc::InputSource.
//

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists> parseResourceLists(
    ::xercesc::InputSource &is,
    ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
    const ::LinphonePrivate::Xsd::XmlSchema::Properties &p = ::LinphonePrivate::Xsd::XmlSchema::Properties());

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists> parseResourceLists(
    ::xercesc::InputSource &is,
    ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &eh,
    ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
    const ::LinphonePrivate::Xsd::XmlSchema::Properties &p = ::LinphonePrivate::Xsd::XmlSchema::Properties());

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists> parseResourceLists(
    ::xercesc::InputSource &is,
    ::xercesc::DOMErrorHandler &eh,
    ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
    const ::LinphonePrivate::Xsd::XmlSchema::Properties &p = ::LinphonePrivate::Xsd::XmlSchema::Properties());

// Parse xercesc::DOMDocument.
//

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists> parseResourceLists(
    const ::xercesc::DOMDocument &d,
    ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
    const ::LinphonePrivate::Xsd::XmlSchema::Properties &p = ::LinphonePrivate::Xsd::XmlSchema::Properties());

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists> parseResourceLists(
    ::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d,
    ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0,
    const ::LinphonePrivate::Xsd::XmlSchema::Properties &p = ::LinphonePrivate::Xsd::XmlSchema::Properties());
} // namespace ResourceLists
} // namespace Xsd
} // namespace LinphonePrivate

#include <iosfwd>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/framework/XMLFormatter.hpp>

#include <xsd/cxx/xml/dom/auto-ptr.hxx>

namespace LinphonePrivate {
namespace Xsd {
namespace ResourceLists {
void operator<<(::xercesc::DOMElement &, const ListType &);

void operator<<(::xercesc::DOMElement &, const EntryType &);

void operator<<(::xercesc::DOMElement &, const EntryRefType &);

void operator<<(::xercesc::DOMElement &, const ExternalType &);

// Serialize to std::ostream.
//

void serializeResourceLists(::std::ostream &os,
                            const ::LinphonePrivate::Xsd::ResourceLists::ResourceLists &x,
                            const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m =
                                ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap(),
                            const ::std::string &e = "UTF-8",
                            ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0);

void serializeResourceLists(::std::ostream &os,
                            const ::LinphonePrivate::Xsd::ResourceLists::ResourceLists &x,
                            ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &eh,
                            const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m =
                                ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap(),
                            const ::std::string &e = "UTF-8",
                            ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0);

void serializeResourceLists(::std::ostream &os,
                            const ::LinphonePrivate::Xsd::ResourceLists::ResourceLists &x,
                            ::xercesc::DOMErrorHandler &eh,
                            const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m =
                                ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap(),
                            const ::std::string &e = "UTF-8",
                            ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0);

// Serialize to xercesc::XMLFormatTarget.
//

void serializeResourceLists(::xercesc::XMLFormatTarget &ft,
                            const ::LinphonePrivate::Xsd::ResourceLists::ResourceLists &x,
                            const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m =
                                ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap(),
                            const ::std::string &e = "UTF-8",
                            ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0);

void serializeResourceLists(::xercesc::XMLFormatTarget &ft,
                            const ::LinphonePrivate::Xsd::ResourceLists::ResourceLists &x,
                            ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &eh,
                            const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m =
                                ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap(),
                            const ::std::string &e = "UTF-8",
                            ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0);

void serializeResourceLists(::xercesc::XMLFormatTarget &ft,
                            const ::LinphonePrivate::Xsd::ResourceLists::ResourceLists &x,
                            ::xercesc::DOMErrorHandler &eh,
                            const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m =
                                ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap(),
                            const ::std::string &e = "UTF-8",
                            ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0);

// Serialize to an existing xercesc::DOMDocument.
//

void serializeResourceLists(::xercesc::DOMDocument &d,
                            const ::LinphonePrivate::Xsd::ResourceLists::ResourceLists &x,
                            ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0);

// Serialize to a new xercesc::DOMDocument.
//

::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument>
serializeResourceLists(const ::LinphonePrivate::Xsd::ResourceLists::ResourceLists &x,
                       const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m =
                           ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap(),
                       ::LinphonePrivate::Xsd::XmlSchema::Flags f = 0);

void operator<<(::xercesc::DOMElement &, const DisplayNameType &);

void operator<<(::xercesc::DOMElement &, const List &);

void operator<<(::xercesc::DOMElement &, const DisplayName &);

void operator<<(::xercesc::DOMElement &, const ResourceLists &);
} // namespace ResourceLists
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

#endif // RESOURCE_LISTS_H
