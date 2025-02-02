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

#include "resource-lists.h"

namespace LinphonePrivate {
namespace Xsd {
namespace ResourceLists {
// ListType
//

const ListType::DisplayNameOptional &ListType::getDisplayName() const {
	return this->display_name_;
}

ListType::DisplayNameOptional &ListType::getDisplayName() {
	return this->display_name_;
}

void ListType::setDisplayName(const DisplayNameType &x) {
	this->display_name_.set(x);
}

void ListType::setDisplayName(const DisplayNameOptional &x) {
	this->display_name_ = x;
}

void ListType::setDisplayName(::std::unique_ptr<DisplayNameType> x) {
	this->display_name_.set(std::move(x));
}

const ListType::ListSequence &ListType::getList() const {
	return this->list_;
}

ListType::ListSequence &ListType::getList() {
	return this->list_;
}

void ListType::setList(const ListSequence &s) {
	this->list_ = s;
}

const ListType::ExternalSequence &ListType::getExternal() const {
	return this->external_;
}

ListType::ExternalSequence &ListType::getExternal() {
	return this->external_;
}

void ListType::setExternal(const ExternalSequence &s) {
	this->external_ = s;
}

const ListType::EntrySequence &ListType::getEntry() const {
	return this->entry_;
}

ListType::EntrySequence &ListType::getEntry() {
	return this->entry_;
}

void ListType::setEntry(const EntrySequence &s) {
	this->entry_ = s;
}

const ListType::EntryRefSequence &ListType::getEntryRef() const {
	return this->entry_ref_;
}

ListType::EntryRefSequence &ListType::getEntryRef() {
	return this->entry_ref_;
}

void ListType::setEntryRef(const EntryRefSequence &s) {
	this->entry_ref_ = s;
}

const ListType::AnySequence &ListType::getAny() const {
	return this->any_;
}

ListType::AnySequence &ListType::getAny() {
	return this->any_;
}

void ListType::setAny(const AnySequence &s) {
	this->any_ = s;
}

const ListType::NameOptional &ListType::getName() const {
	return this->name_;
}

ListType::NameOptional &ListType::getName() {
	return this->name_;
}

void ListType::setName(const NameType &x) {
	this->name_.set(x);
}

void ListType::setName(const NameOptional &x) {
	this->name_ = x;
}

void ListType::setName(::std::unique_ptr<NameType> x) {
	this->name_.set(std::move(x));
}

const ListType::AnyAttributeSet &ListType::getAnyAttribute() const {
	return this->any_attribute_;
}

ListType::AnyAttributeSet &ListType::getAnyAttribute() {
	return this->any_attribute_;
}

void ListType::setAnyAttribute(const AnyAttributeSet &s) {
	this->any_attribute_ = s;
}

const ::xercesc::DOMDocument &ListType::getDomDocument() const {
	return *this->dom_document_;
}

::xercesc::DOMDocument &ListType::getDomDocument() {
	return *this->dom_document_;
}

// EntryType
//

const EntryType::DisplayNameOptional &EntryType::getDisplayName() const {
	return this->display_name_;
}

EntryType::DisplayNameOptional &EntryType::getDisplayName() {
	return this->display_name_;
}

void EntryType::setDisplayName(const DisplayNameType &x) {
	this->display_name_.set(x);
}

void EntryType::setDisplayName(const DisplayNameOptional &x) {
	this->display_name_ = x;
}

void EntryType::setDisplayName(::std::unique_ptr<DisplayNameType> x) {
	this->display_name_.set(std::move(x));
}

const EntryType::AnySequence &EntryType::getAny() const {
	return this->any_;
}

EntryType::AnySequence &EntryType::getAny() {
	return this->any_;
}

void EntryType::setAny(const AnySequence &s) {
	this->any_ = s;
}

const EntryType::UriType &EntryType::getUri() const {
	return this->uri_.get();
}

EntryType::UriType &EntryType::getUri() {
	return this->uri_.get();
}

void EntryType::setUri(const UriType &x) {
	this->uri_.set(x);
}

void EntryType::setUri(::std::unique_ptr<UriType> x) {
	this->uri_.set(std::move(x));
}

::std::unique_ptr<EntryType::UriType> EntryType::setDetachUri() {
	return this->uri_.detach();
}

const EntryType::AnyAttributeSet &EntryType::getAnyAttribute() const {
	return this->any_attribute_;
}

EntryType::AnyAttributeSet &EntryType::getAnyAttribute() {
	return this->any_attribute_;
}

void EntryType::setAnyAttribute(const AnyAttributeSet &s) {
	this->any_attribute_ = s;
}

const ::xercesc::DOMDocument &EntryType::getDomDocument() const {
	return *this->dom_document_;
}

::xercesc::DOMDocument &EntryType::getDomDocument() {
	return *this->dom_document_;
}

// EntryRefType
//

const EntryRefType::DisplayNameOptional &EntryRefType::getDisplayName() const {
	return this->display_name_;
}

EntryRefType::DisplayNameOptional &EntryRefType::getDisplayName() {
	return this->display_name_;
}

void EntryRefType::setDisplayName(const DisplayNameType &x) {
	this->display_name_.set(x);
}

void EntryRefType::setDisplayName(const DisplayNameOptional &x) {
	this->display_name_ = x;
}

void EntryRefType::setDisplayName(::std::unique_ptr<DisplayNameType> x) {
	this->display_name_.set(std::move(x));
}

const EntryRefType::AnySequence &EntryRefType::getAny() const {
	return this->any_;
}

EntryRefType::AnySequence &EntryRefType::getAny() {
	return this->any_;
}

void EntryRefType::setAny(const AnySequence &s) {
	this->any_ = s;
}

const EntryRefType::RefType &EntryRefType::getRef() const {
	return this->ref_.get();
}

EntryRefType::RefType &EntryRefType::getRef() {
	return this->ref_.get();
}

void EntryRefType::setRef(const RefType &x) {
	this->ref_.set(x);
}

void EntryRefType::setRef(::std::unique_ptr<RefType> x) {
	this->ref_.set(std::move(x));
}

::std::unique_ptr<EntryRefType::RefType> EntryRefType::setDetachRef() {
	return this->ref_.detach();
}

const EntryRefType::AnyAttributeSet &EntryRefType::getAnyAttribute() const {
	return this->any_attribute_;
}

EntryRefType::AnyAttributeSet &EntryRefType::getAnyAttribute() {
	return this->any_attribute_;
}

void EntryRefType::setAnyAttribute(const AnyAttributeSet &s) {
	this->any_attribute_ = s;
}

const ::xercesc::DOMDocument &EntryRefType::getDomDocument() const {
	return *this->dom_document_;
}

::xercesc::DOMDocument &EntryRefType::getDomDocument() {
	return *this->dom_document_;
}

// ExternalType
//

const ExternalType::DisplayNameOptional &ExternalType::getDisplayName() const {
	return this->display_name_;
}

ExternalType::DisplayNameOptional &ExternalType::getDisplayName() {
	return this->display_name_;
}

void ExternalType::setDisplayName(const DisplayNameType &x) {
	this->display_name_.set(x);
}

void ExternalType::setDisplayName(const DisplayNameOptional &x) {
	this->display_name_ = x;
}

void ExternalType::setDisplayName(::std::unique_ptr<DisplayNameType> x) {
	this->display_name_.set(std::move(x));
}

const ExternalType::AnySequence &ExternalType::getAny() const {
	return this->any_;
}

ExternalType::AnySequence &ExternalType::getAny() {
	return this->any_;
}

void ExternalType::setAny(const AnySequence &s) {
	this->any_ = s;
}

const ExternalType::AnchorOptional &ExternalType::getAnchor() const {
	return this->anchor_;
}

ExternalType::AnchorOptional &ExternalType::getAnchor() {
	return this->anchor_;
}

void ExternalType::setAnchor(const AnchorType &x) {
	this->anchor_.set(x);
}

void ExternalType::setAnchor(const AnchorOptional &x) {
	this->anchor_ = x;
}

void ExternalType::setAnchor(::std::unique_ptr<AnchorType> x) {
	this->anchor_.set(std::move(x));
}

const ExternalType::AnyAttributeSet &ExternalType::getAnyAttribute() const {
	return this->any_attribute_;
}

ExternalType::AnyAttributeSet &ExternalType::getAnyAttribute() {
	return this->any_attribute_;
}

void ExternalType::setAnyAttribute(const AnyAttributeSet &s) {
	this->any_attribute_ = s;
}

const ::xercesc::DOMDocument &ExternalType::getDomDocument() const {
	return *this->dom_document_;
}

::xercesc::DOMDocument &ExternalType::getDomDocument() {
	return *this->dom_document_;
}

// DisplayNameType
//

const DisplayNameType::LangOptional &DisplayNameType::getLang() const {
	return this->lang_;
}

DisplayNameType::LangOptional &DisplayNameType::getLang() {
	return this->lang_;
}

void DisplayNameType::setLang(const LangType &x) {
	this->lang_.set(x);
}

void DisplayNameType::setLang(const LangOptional &x) {
	this->lang_ = x;
}

void DisplayNameType::setLang(::std::unique_ptr<LangType> x) {
	this->lang_.set(std::move(x));
}

// List
//

// DisplayName
//

// ResourceLists
//

const ResourceLists::ListSequence &ResourceLists::getList() const {
	return this->list_;
}

ResourceLists::ListSequence &ResourceLists::getList() {
	return this->list_;
}

void ResourceLists::setList(const ListSequence &s) {
	this->list_ = s;
}
} // namespace ResourceLists
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
namespace ResourceLists {
// ListType
//

ListType::ListType()
    : ::LinphonePrivate::Xsd::XmlSchema::Type(), dom_document_(::xsd::cxx::xml::dom::create_document<char>()),
      display_name_(this), list_(this), external_(this), entry_(this), entry_ref_(this), any_(this->getDomDocument()),
      name_(this), any_attribute_(this->getDomDocument()) {
}

ListType::ListType(const ListType &x,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(x, f, c), dom_document_(::xsd::cxx::xml::dom::create_document<char>()),
      display_name_(x.display_name_, f, this), list_(x.list_, f, this), external_(x.external_, f, this),
      entry_(x.entry_, f, this), entry_ref_(x.entry_ref_, f, this), any_(x.any_, this->getDomDocument()),
      name_(x.name_, f, this), any_attribute_(x.any_attribute_, this->getDomDocument()) {
}

ListType::ListType(const ::xercesc::DOMElement &e,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(e, f | ::LinphonePrivate::Xsd::XmlSchema::Flags::base, c),
      dom_document_(::xsd::cxx::xml::dom::create_document<char>()), display_name_(this), list_(this), external_(this),
      entry_(this), entry_ref_(this), any_(this->getDomDocument()), name_(this),
      any_attribute_(this->getDomDocument()) {
	if ((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::base) == 0) {
		::xsd::cxx::xml::dom::parser<char> p(e, true, false, true);
		this->parse(p, f);
	}
}

void ListType::parse(::xsd::cxx::xml::dom::parser<char> &p, ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	for (; p.more_content(); p.next_content(false)) {
		const ::xercesc::DOMElement &i(p.cur_element());
		const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(i));

		// display-name
		//
		if (n.name() == "display-name" && n.namespace_() == "urn:ietf:params:xml:ns:resource-lists") {
			::std::unique_ptr<DisplayNameType> r(DisplayNameTraits::create(i, f, this));

			if (!this->display_name_) {
				this->display_name_.set(::std::move(r));
				continue;
			}
		}

		// list
		//
		if (n.name() == "list" && n.namespace_() == "urn:ietf:params:xml:ns:resource-lists") {
			::std::unique_ptr<ListType1> r(ListTraits::create(i, f, this));

			this->list_.push_back(::std::move(r));
			continue;
		}

		// external
		//
		if (n.name() == "external" && n.namespace_() == "urn:ietf:params:xml:ns:resource-lists") {
			::std::unique_ptr<ExternalType> r(ExternalTraits::create(i, f, this));

			this->external_.push_back(::std::move(r));
			continue;
		}

		// entry
		//
		if (n.name() == "entry" && n.namespace_() == "urn:ietf:params:xml:ns:resource-lists") {
			::std::unique_ptr<EntryType> r(EntryTraits::create(i, f, this));

			this->entry_.push_back(::std::move(r));
			continue;
		}

		// entry-ref
		//
		if (n.name() == "entry-ref" && n.namespace_() == "urn:ietf:params:xml:ns:resource-lists") {
			::std::unique_ptr<EntryRefType> r(EntryRefTraits::create(i, f, this));

			this->entry_ref_.push_back(::std::move(r));
			continue;
		}

		// any
		//
		if ((!n.namespace_().empty() && n.namespace_() != "urn:ietf:params:xml:ns:resource-lists")) {
			::xercesc::DOMElement *r(static_cast<::xercesc::DOMElement *>(
			    this->getDomDocument().importNode(const_cast<::xercesc::DOMElement *>(&i), true)));
			this->any_.push_back(r);
			continue;
		}

		break;
	}

	while (p.more_attributes()) {
		const ::xercesc::DOMAttr &i(p.next_attribute());
		const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(i));

		if (n.name() == "name" && n.namespace_().empty()) {
			this->name_.set(NameTraits::create(i, f, this));
			continue;
		}

		// any_attribute
		//
		if ((!n.namespace_().empty() && n.namespace_() != "urn:ietf:params:xml:ns:resource-lists" &&
		     n.namespace_() != ::xsd::cxx::xml::bits::xmlns_namespace<char>() &&
		     n.namespace_() != ::xsd::cxx::xml::bits::xsi_namespace<char>())) {
			::xercesc::DOMAttr *r(static_cast<::xercesc::DOMAttr *>(
			    this->getDomDocument().importNode(const_cast<::xercesc::DOMAttr *>(&i), true)));
			this->any_attribute_.insert(r);
			continue;
		}
	}
}

ListType *ListType::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f,
                           ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class ListType(*this, f, c);
}

ListType &ListType::operator=(const ListType &x) {
	if (this != &x) {
		static_cast<::LinphonePrivate::Xsd::XmlSchema::Type &>(*this) = x;
		this->display_name_ = x.display_name_;
		this->list_ = x.list_;
		this->external_ = x.external_;
		this->entry_ = x.entry_;
		this->entry_ref_ = x.entry_ref_;
		this->any_ = x.any_;
		this->name_ = x.name_;
		this->any_attribute_ = x.any_attribute_;
	}

	return *this;
}

ListType::~ListType() {
}

// EntryType
//

EntryType::EntryType(const UriType &uri)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(), dom_document_(::xsd::cxx::xml::dom::create_document<char>()),
      display_name_(this), any_(this->getDomDocument()), uri_(uri, this), any_attribute_(this->getDomDocument()) {
}

EntryType::EntryType(const EntryType &x,
                     ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                     ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(x, f, c), dom_document_(::xsd::cxx::xml::dom::create_document<char>()),
      display_name_(x.display_name_, f, this), any_(x.any_, this->getDomDocument()), uri_(x.uri_, f, this),
      any_attribute_(x.any_attribute_, this->getDomDocument()) {
}

EntryType::EntryType(const ::xercesc::DOMElement &e,
                     ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                     ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(e, f | ::LinphonePrivate::Xsd::XmlSchema::Flags::base, c),
      dom_document_(::xsd::cxx::xml::dom::create_document<char>()), display_name_(this), any_(this->getDomDocument()),
      uri_(this), any_attribute_(this->getDomDocument()) {
	if ((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::base) == 0) {
		::xsd::cxx::xml::dom::parser<char> p(e, true, false, true);
		this->parse(p, f);
	}
}

void EntryType::parse(::xsd::cxx::xml::dom::parser<char> &p, ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	for (; p.more_content(); p.next_content(false)) {
		const ::xercesc::DOMElement &i(p.cur_element());
		const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(i));

		// display-name
		//
		if (n.name() == "display-name" && n.namespace_() == "urn:ietf:params:xml:ns:resource-lists") {
			::std::unique_ptr<DisplayNameType> r(DisplayNameTraits::create(i, f, this));

			if (!this->display_name_) {
				this->display_name_.set(::std::move(r));
				continue;
			}
		}

		// any
		//
		if ((!n.namespace_().empty() && n.namespace_() != "urn:ietf:params:xml:ns:resource-lists")) {
			::xercesc::DOMElement *r(static_cast<::xercesc::DOMElement *>(
			    this->getDomDocument().importNode(const_cast<::xercesc::DOMElement *>(&i), true)));
			this->any_.push_back(r);
			continue;
		}

		break;
	}

	while (p.more_attributes()) {
		const ::xercesc::DOMAttr &i(p.next_attribute());
		const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(i));

		if (n.name() == "uri" && n.namespace_().empty()) {
			this->uri_.set(UriTraits::create(i, f, this));
			continue;
		}

		// any_attribute
		//
		if ((!n.namespace_().empty() && n.namespace_() != "urn:ietf:params:xml:ns:resource-lists" &&
		     n.namespace_() != ::xsd::cxx::xml::bits::xmlns_namespace<char>() &&
		     n.namespace_() != ::xsd::cxx::xml::bits::xsi_namespace<char>())) {
			::xercesc::DOMAttr *r(static_cast<::xercesc::DOMAttr *>(
			    this->getDomDocument().importNode(const_cast<::xercesc::DOMAttr *>(&i), true)));
			this->any_attribute_.insert(r);
			continue;
		}
	}

	if (!uri_.present()) {
		throw ::xsd::cxx::tree::expected_attribute<char>("uri", "");
	}
}

EntryType *EntryType::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f,
                             ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class EntryType(*this, f, c);
}

EntryType &EntryType::operator=(const EntryType &x) {
	if (this != &x) {
		static_cast<::LinphonePrivate::Xsd::XmlSchema::Type &>(*this) = x;
		this->display_name_ = x.display_name_;
		this->any_ = x.any_;
		this->uri_ = x.uri_;
		this->any_attribute_ = x.any_attribute_;
	}

	return *this;
}

EntryType::~EntryType() {
}

// EntryRefType
//

EntryRefType::EntryRefType(const RefType &ref)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(), dom_document_(::xsd::cxx::xml::dom::create_document<char>()),
      display_name_(this), any_(this->getDomDocument()), ref_(ref, this), any_attribute_(this->getDomDocument()) {
}

EntryRefType::EntryRefType(const EntryRefType &x,
                           ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                           ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(x, f, c), dom_document_(::xsd::cxx::xml::dom::create_document<char>()),
      display_name_(x.display_name_, f, this), any_(x.any_, this->getDomDocument()), ref_(x.ref_, f, this),
      any_attribute_(x.any_attribute_, this->getDomDocument()) {
}

EntryRefType::EntryRefType(const ::xercesc::DOMElement &e,
                           ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                           ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(e, f | ::LinphonePrivate::Xsd::XmlSchema::Flags::base, c),
      dom_document_(::xsd::cxx::xml::dom::create_document<char>()), display_name_(this), any_(this->getDomDocument()),
      ref_(this), any_attribute_(this->getDomDocument()) {
	if ((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::base) == 0) {
		::xsd::cxx::xml::dom::parser<char> p(e, true, false, true);
		this->parse(p, f);
	}
}

void EntryRefType::parse(::xsd::cxx::xml::dom::parser<char> &p, ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	for (; p.more_content(); p.next_content(false)) {
		const ::xercesc::DOMElement &i(p.cur_element());
		const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(i));

		// display-name
		//
		if (n.name() == "display-name" && n.namespace_() == "urn:ietf:params:xml:ns:resource-lists") {
			::std::unique_ptr<DisplayNameType> r(DisplayNameTraits::create(i, f, this));

			if (!this->display_name_) {
				this->display_name_.set(::std::move(r));
				continue;
			}
		}

		// any
		//
		if ((!n.namespace_().empty() && n.namespace_() != "urn:ietf:params:xml:ns:resource-lists")) {
			::xercesc::DOMElement *r(static_cast<::xercesc::DOMElement *>(
			    this->getDomDocument().importNode(const_cast<::xercesc::DOMElement *>(&i), true)));
			this->any_.push_back(r);
			continue;
		}

		break;
	}

	while (p.more_attributes()) {
		const ::xercesc::DOMAttr &i(p.next_attribute());
		const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(i));

		if (n.name() == "ref" && n.namespace_().empty()) {
			this->ref_.set(RefTraits::create(i, f, this));
			continue;
		}

		// any_attribute
		//
		if ((!n.namespace_().empty() && n.namespace_() != "urn:ietf:params:xml:ns:resource-lists" &&
		     n.namespace_() != ::xsd::cxx::xml::bits::xmlns_namespace<char>() &&
		     n.namespace_() != ::xsd::cxx::xml::bits::xsi_namespace<char>())) {
			::xercesc::DOMAttr *r(static_cast<::xercesc::DOMAttr *>(
			    this->getDomDocument().importNode(const_cast<::xercesc::DOMAttr *>(&i), true)));
			this->any_attribute_.insert(r);
			continue;
		}
	}

	if (!ref_.present()) {
		throw ::xsd::cxx::tree::expected_attribute<char>("ref", "");
	}
}

EntryRefType *EntryRefType::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f,
                                   ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class EntryRefType(*this, f, c);
}

EntryRefType &EntryRefType::operator=(const EntryRefType &x) {
	if (this != &x) {
		static_cast<::LinphonePrivate::Xsd::XmlSchema::Type &>(*this) = x;
		this->display_name_ = x.display_name_;
		this->any_ = x.any_;
		this->ref_ = x.ref_;
		this->any_attribute_ = x.any_attribute_;
	}

	return *this;
}

EntryRefType::~EntryRefType() {
}

// ExternalType
//

ExternalType::ExternalType()
    : ::LinphonePrivate::Xsd::XmlSchema::Type(), dom_document_(::xsd::cxx::xml::dom::create_document<char>()),
      display_name_(this), any_(this->getDomDocument()), anchor_(this), any_attribute_(this->getDomDocument()) {
}

ExternalType::ExternalType(const ExternalType &x,
                           ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                           ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(x, f, c), dom_document_(::xsd::cxx::xml::dom::create_document<char>()),
      display_name_(x.display_name_, f, this), any_(x.any_, this->getDomDocument()), anchor_(x.anchor_, f, this),
      any_attribute_(x.any_attribute_, this->getDomDocument()) {
}

ExternalType::ExternalType(const ::xercesc::DOMElement &e,
                           ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                           ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(e, f | ::LinphonePrivate::Xsd::XmlSchema::Flags::base, c),
      dom_document_(::xsd::cxx::xml::dom::create_document<char>()), display_name_(this), any_(this->getDomDocument()),
      anchor_(this), any_attribute_(this->getDomDocument()) {
	if ((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::base) == 0) {
		::xsd::cxx::xml::dom::parser<char> p(e, true, false, true);
		this->parse(p, f);
	}
}

void ExternalType::parse(::xsd::cxx::xml::dom::parser<char> &p, ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	for (; p.more_content(); p.next_content(false)) {
		const ::xercesc::DOMElement &i(p.cur_element());
		const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(i));

		// display-name
		//
		if (n.name() == "display-name" && n.namespace_() == "urn:ietf:params:xml:ns:resource-lists") {
			::std::unique_ptr<DisplayNameType> r(DisplayNameTraits::create(i, f, this));

			if (!this->display_name_) {
				this->display_name_.set(::std::move(r));
				continue;
			}
		}

		// any
		//
		if ((!n.namespace_().empty() && n.namespace_() != "urn:ietf:params:xml:ns:resource-lists")) {
			::xercesc::DOMElement *r(static_cast<::xercesc::DOMElement *>(
			    this->getDomDocument().importNode(const_cast<::xercesc::DOMElement *>(&i), true)));
			this->any_.push_back(r);
			continue;
		}

		break;
	}

	while (p.more_attributes()) {
		const ::xercesc::DOMAttr &i(p.next_attribute());
		const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(i));

		if (n.name() == "anchor" && n.namespace_().empty()) {
			this->anchor_.set(AnchorTraits::create(i, f, this));
			continue;
		}

		// any_attribute
		//
		if ((!n.namespace_().empty() && n.namespace_() != "urn:ietf:params:xml:ns:resource-lists" &&
		     n.namespace_() != ::xsd::cxx::xml::bits::xmlns_namespace<char>() &&
		     n.namespace_() != ::xsd::cxx::xml::bits::xsi_namespace<char>())) {
			::xercesc::DOMAttr *r(static_cast<::xercesc::DOMAttr *>(
			    this->getDomDocument().importNode(const_cast<::xercesc::DOMAttr *>(&i), true)));
			this->any_attribute_.insert(r);
			continue;
		}
	}
}

ExternalType *ExternalType::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f,
                                   ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class ExternalType(*this, f, c);
}

ExternalType &ExternalType::operator=(const ExternalType &x) {
	if (this != &x) {
		static_cast<::LinphonePrivate::Xsd::XmlSchema::Type &>(*this) = x;
		this->display_name_ = x.display_name_;
		this->any_ = x.any_;
		this->anchor_ = x.anchor_;
		this->any_attribute_ = x.any_attribute_;
	}

	return *this;
}

ExternalType::~ExternalType() {
}

// DisplayNameType
//

DisplayNameType::DisplayNameType() : ::LinphonePrivate::Xsd::XmlSchema::String(), lang_(this) {
}

DisplayNameType::DisplayNameType(const char *_xsd_String_base)
    : ::LinphonePrivate::Xsd::XmlSchema::String(_xsd_String_base), lang_(this) {
}

DisplayNameType::DisplayNameType(const ::std::string &_xsd_String_base)
    : ::LinphonePrivate::Xsd::XmlSchema::String(_xsd_String_base), lang_(this) {
}

DisplayNameType::DisplayNameType(const ::LinphonePrivate::Xsd::XmlSchema::String &_xsd_String_base)
    : ::LinphonePrivate::Xsd::XmlSchema::String(_xsd_String_base), lang_(this) {
}

DisplayNameType::DisplayNameType(const DisplayNameType &x,
                                 ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                                 ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(x, f, c), lang_(x.lang_, f, this) {
}

DisplayNameType::DisplayNameType(const ::xercesc::DOMElement &e,
                                 ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                                 ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(e, f | ::LinphonePrivate::Xsd::XmlSchema::Flags::base, c), lang_(this) {
	if ((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::base) == 0) {
		::xsd::cxx::xml::dom::parser<char> p(e, false, false, true);
		this->parse(p, f);
	}
}

void DisplayNameType::parse(::xsd::cxx::xml::dom::parser<char> &p, ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	while (p.more_attributes()) {
		const ::xercesc::DOMAttr &i(p.next_attribute());
		const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(i));

		if (n.name() == "lang" && n.namespace_() == "http://www.w3.org/XML/1998/namespace") {
			this->lang_.set(LangTraits::create(i, f, this));
			continue;
		}
	}
}

DisplayNameType *DisplayNameType::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f,
                                         ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class DisplayNameType(*this, f, c);
}

DisplayNameType &DisplayNameType::operator=(const DisplayNameType &x) {
	if (this != &x) {
		static_cast<::LinphonePrivate::Xsd::XmlSchema::String &>(*this) = x;
		this->lang_ = x.lang_;
	}

	return *this;
}

DisplayNameType::~DisplayNameType() {
}

// List
//

List::List() : ::LinphonePrivate::Xsd::ResourceLists::ListType() {
}

List::List(const List &x, ::LinphonePrivate::Xsd::XmlSchema::Flags f, ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::ResourceLists::ListType(x, f, c) {
}

List::List(const ::xercesc::DOMElement &e,
           ::LinphonePrivate::Xsd::XmlSchema::Flags f,
           ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::ResourceLists::ListType(e, f, c) {
}

List *List::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f, ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class List(*this, f, c);
}

List::~List() {
}

// DisplayName
//

DisplayName::DisplayName() : ::LinphonePrivate::Xsd::ResourceLists::DisplayNameType() {
}

DisplayName::DisplayName(const char *_xsd_String_base)
    : ::LinphonePrivate::Xsd::ResourceLists::DisplayNameType(_xsd_String_base) {
}

DisplayName::DisplayName(const ::std::string &_xsd_String_base)
    : ::LinphonePrivate::Xsd::ResourceLists::DisplayNameType(_xsd_String_base) {
}

DisplayName::DisplayName(const ::LinphonePrivate::Xsd::XmlSchema::String &_xsd_String_base)
    : ::LinphonePrivate::Xsd::ResourceLists::DisplayNameType(_xsd_String_base) {
}

DisplayName::DisplayName(const DisplayName &x,
                         ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                         ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::ResourceLists::DisplayNameType(x, f, c) {
}

DisplayName::DisplayName(const ::xercesc::DOMElement &e,
                         ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                         ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::ResourceLists::DisplayNameType(e, f, c) {
}

DisplayName *DisplayName::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f,
                                 ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class DisplayName(*this, f, c);
}

DisplayName::~DisplayName() {
}

// ResourceLists
//

ResourceLists::ResourceLists() : ::LinphonePrivate::Xsd::XmlSchema::Type(), list_(this) {
}

ResourceLists::ResourceLists(const ResourceLists &x,
                             ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                             ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(x, f, c), list_(x.list_, f, this) {
}

ResourceLists::ResourceLists(const ::xercesc::DOMElement &e,
                             ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                             ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(e, f | ::LinphonePrivate::Xsd::XmlSchema::Flags::base, c), list_(this) {
	if ((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::base) == 0) {
		::xsd::cxx::xml::dom::parser<char> p(e, true, false, false);
		this->parse(p, f);
	}
}

void ResourceLists::parse(::xsd::cxx::xml::dom::parser<char> &p, ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	for (; p.more_content(); p.next_content(false)) {
		const ::xercesc::DOMElement &i(p.cur_element());
		const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(i));

		// list
		//
		if (n.name() == "list" && n.namespace_() == "urn:ietf:params:xml:ns:resource-lists") {
			::std::unique_ptr<ListType> r(ListTraits::create(i, f, this));

			this->list_.push_back(::std::move(r));
			continue;
		}

		break;
	}
}

ResourceLists *ResourceLists::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f,
                                     ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class ResourceLists(*this, f, c);
}

ResourceLists &ResourceLists::operator=(const ResourceLists &x) {
	if (this != &x) {
		static_cast<::LinphonePrivate::Xsd::XmlSchema::Type &>(*this) = x;
		this->list_ = x.list_;
	}

	return *this;
}

ResourceLists::~ResourceLists() {
}
} // namespace ResourceLists
} // namespace Xsd
} // namespace LinphonePrivate

#include <ostream>

#include <xsd/cxx/tree/std-ostream-map.hxx>

namespace _xsd {
static const ::xsd::cxx::tree::std_ostream_plate<0, char> std_ostream_plate_init;
}

namespace LinphonePrivate {
namespace Xsd {
namespace ResourceLists {
::std::ostream &operator<<(::std::ostream &o, const ListType &i) {
	if (i.getDisplayName()) {
		o << ::std::endl << "display-name: " << *i.getDisplayName();
	}

	for (ListType::ListConstIterator b(i.getList().begin()), e(i.getList().end()); b != e; ++b) {
		o << ::std::endl << "list: " << *b;
	}

	for (ListType::ExternalConstIterator b(i.getExternal().begin()), e(i.getExternal().end()); b != e; ++b) {
		o << ::std::endl << "external: " << *b;
	}

	for (ListType::EntryConstIterator b(i.getEntry().begin()), e(i.getEntry().end()); b != e; ++b) {
		o << ::std::endl << "entry: " << *b;
	}

	for (ListType::EntryRefConstIterator b(i.getEntryRef().begin()), e(i.getEntryRef().end()); b != e; ++b) {
		o << ::std::endl << "entry-ref: " << *b;
	}

	if (i.getName()) {
		o << ::std::endl << "name: " << *i.getName();
	}

	return o;
}

::std::ostream &operator<<(::std::ostream &o, const EntryType &i) {
	if (i.getDisplayName()) {
		o << ::std::endl << "display-name: " << *i.getDisplayName();
	}

	o << ::std::endl << "uri: " << i.getUri();
	return o;
}

::std::ostream &operator<<(::std::ostream &o, const EntryRefType &i) {
	if (i.getDisplayName()) {
		o << ::std::endl << "display-name: " << *i.getDisplayName();
	}

	o << ::std::endl << "ref: " << i.getRef();
	return o;
}

::std::ostream &operator<<(::std::ostream &o, const ExternalType &i) {
	if (i.getDisplayName()) {
		o << ::std::endl << "display-name: " << *i.getDisplayName();
	}

	if (i.getAnchor()) {
		o << ::std::endl << "anchor: " << *i.getAnchor();
	}

	return o;
}

::std::ostream &operator<<(::std::ostream &o, const DisplayNameType &i) {
	o << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);

	if (i.getLang()) {
		o << ::std::endl << "lang: " << *i.getLang();
	}

	return o;
}

::std::ostream &operator<<(::std::ostream &o, const List &i) {
	o << static_cast<const ::LinphonePrivate::Xsd::ResourceLists::ListType &>(i);

	return o;
}

::std::ostream &operator<<(::std::ostream &o, const DisplayName &i) {
	o << static_cast<const ::LinphonePrivate::Xsd::ResourceLists::DisplayNameType &>(i);

	return o;
}

::std::ostream &operator<<(::std::ostream &o, const ResourceLists &i) {
	for (ResourceLists::ListConstIterator b(i.getList().begin()), e(i.getList().end()); b != e; ++b) {
		o << ::std::endl << "list: " << *b;
	}

	return o;
}
} // namespace ResourceLists
} // namespace Xsd
} // namespace LinphonePrivate

#include <istream>
#include <xsd/cxx/tree/error-handler.hxx>
#include <xsd/cxx/xml/sax/std-input-source.hxx>

namespace LinphonePrivate {
namespace Xsd {
namespace ResourceLists {
::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>
parseResourceLists(const ::std::string &u,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::tree::error_handler<char> h;

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(u, h, p, f));

	h.throw_if_failed<::xsd::cxx::tree::parsing<char>>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>(
	    ::LinphonePrivate::Xsd::ResourceLists::parseResourceLists(
	        std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>
parseResourceLists(const ::std::string &u,
                   ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(u, h, p, f));

	if (!d.get()) throw ::xsd::cxx::tree::parsing<char>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>(
	    ::LinphonePrivate::Xsd::ResourceLists::parseResourceLists(
	        std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>
parseResourceLists(const ::std::string &u,
                   ::xercesc::DOMErrorHandler &h,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(u, h, p, f));

	if (!d.get()) throw ::xsd::cxx::tree::parsing<char>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>(
	    ::LinphonePrivate::Xsd::ResourceLists::parseResourceLists(
	        std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>
parseResourceLists(::std::istream &is,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::xml::sax::std_input_source isrc(is);
	return ::LinphonePrivate::Xsd::ResourceLists::parseResourceLists(isrc, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>
parseResourceLists(::std::istream &is,
                   ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::xml::sax::std_input_source isrc(is);
	return ::LinphonePrivate::Xsd::ResourceLists::parseResourceLists(isrc, h, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>
parseResourceLists(::std::istream &is,
                   ::xercesc::DOMErrorHandler &h,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::sax::std_input_source isrc(is);
	return ::LinphonePrivate::Xsd::ResourceLists::parseResourceLists(isrc, h, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>
parseResourceLists(::std::istream &is,
                   const ::std::string &sid,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::xml::sax::std_input_source isrc(is, sid);
	return ::LinphonePrivate::Xsd::ResourceLists::parseResourceLists(isrc, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>
parseResourceLists(::std::istream &is,
                   const ::std::string &sid,
                   ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::xml::sax::std_input_source isrc(is, sid);
	return ::LinphonePrivate::Xsd::ResourceLists::parseResourceLists(isrc, h, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>
parseResourceLists(::std::istream &is,
                   const ::std::string &sid,
                   ::xercesc::DOMErrorHandler &h,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::sax::std_input_source isrc(is, sid);
	return ::LinphonePrivate::Xsd::ResourceLists::parseResourceLists(isrc, h, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>
parseResourceLists(::xercesc::InputSource &i,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::tree::error_handler<char> h;

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(i, h, p, f));

	h.throw_if_failed<::xsd::cxx::tree::parsing<char>>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>(
	    ::LinphonePrivate::Xsd::ResourceLists::parseResourceLists(
	        std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>
parseResourceLists(::xercesc::InputSource &i,
                   ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(i, h, p, f));

	if (!d.get()) throw ::xsd::cxx::tree::parsing<char>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>(
	    ::LinphonePrivate::Xsd::ResourceLists::parseResourceLists(
	        std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>
parseResourceLists(::xercesc::InputSource &i,
                   ::xercesc::DOMErrorHandler &h,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(i, h, p, f));

	if (!d.get()) throw ::xsd::cxx::tree::parsing<char>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>(
	    ::LinphonePrivate::Xsd::ResourceLists::parseResourceLists(
	        std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>
parseResourceLists(const ::xercesc::DOMDocument &doc,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	if (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) {
		::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
		    static_cast<::xercesc::DOMDocument *>(doc.cloneNode(true)));

		return ::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>(
		    ::LinphonePrivate::Xsd::ResourceLists::parseResourceLists(
		        std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
	}

	const ::xercesc::DOMElement &e(*doc.getDocumentElement());
	const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(e));

	if (n.name() == "resource-lists" && n.namespace_() == "urn:ietf:params:xml:ns:resource-lists") {
		::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists> r(
		    ::xsd::cxx::tree::traits<::LinphonePrivate::Xsd::ResourceLists::ResourceLists, char>::create(e, f, 0));
		return r;
	}

	throw ::xsd::cxx::tree::unexpected_element<char>(n.name(), n.namespace_(), "resource-lists",
	                                                 "urn:ietf:params:xml:ns:resource-lists");
}

::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists>
parseResourceLists(::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d,
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

	if (n.name() == "resource-lists" && n.namespace_() == "urn:ietf:params:xml:ns:resource-lists") {
		::std::unique_ptr<::LinphonePrivate::Xsd::ResourceLists::ResourceLists> r(
		    ::xsd::cxx::tree::traits<::LinphonePrivate::Xsd::ResourceLists::ResourceLists, char>::create(e, f, 0));
		return r;
	}

	throw ::xsd::cxx::tree::unexpected_element<char>(n.name(), n.namespace_(), "resource-lists",
	                                                 "urn:ietf:params:xml:ns:resource-lists");
}
} // namespace ResourceLists
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
namespace ResourceLists {
void operator<<(::xercesc::DOMElement &e, const ListType &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::Type &>(i);

	// any_attribute
	//
	for (ListType::AnyAttributeConstIterator b(i.getAnyAttribute().begin()), n(i.getAnyAttribute().end()); b != n;
	     ++b) {
		::xercesc::DOMAttr *a(static_cast<::xercesc::DOMAttr *>(
		    e.getOwnerDocument()->importNode(const_cast<::xercesc::DOMAttr *>(&(*b)), true)));

		if (a->getLocalName() == 0) e.setAttributeNode(a);
		else e.setAttributeNodeNS(a);
	}

	// display-name
	//
	if (i.getDisplayName()) {
		::xercesc::DOMElement &s(
		    ::xsd::cxx::xml::dom::create_element("display-name", "urn:ietf:params:xml:ns:resource-lists", e));

		s << *i.getDisplayName();
	}

	// list
	//
	for (ListType::ListConstIterator b(i.getList().begin()), n(i.getList().end()); b != n; ++b) {
		::xercesc::DOMElement &s(
		    ::xsd::cxx::xml::dom::create_element("list", "urn:ietf:params:xml:ns:resource-lists", e));

		s << *b;
	}

	// external
	//
	for (ListType::ExternalConstIterator b(i.getExternal().begin()), n(i.getExternal().end()); b != n; ++b) {
		::xercesc::DOMElement &s(
		    ::xsd::cxx::xml::dom::create_element("external", "urn:ietf:params:xml:ns:resource-lists", e));

		s << *b;
	}

	// entry
	//
	for (ListType::EntryConstIterator b(i.getEntry().begin()), n(i.getEntry().end()); b != n; ++b) {
		::xercesc::DOMElement &s(
		    ::xsd::cxx::xml::dom::create_element("entry", "urn:ietf:params:xml:ns:resource-lists", e));

		s << *b;
	}

	// entry-ref
	//
	for (ListType::EntryRefConstIterator b(i.getEntryRef().begin()), n(i.getEntryRef().end()); b != n; ++b) {
		::xercesc::DOMElement &s(
		    ::xsd::cxx::xml::dom::create_element("entry-ref", "urn:ietf:params:xml:ns:resource-lists", e));

		s << *b;
	}

	// any
	//
	for (ListType::AnyConstIterator b(i.getAny().begin()), n(i.getAny().end()); b != n; ++b) {
		e.appendChild(e.getOwnerDocument()->importNode(const_cast<::xercesc::DOMElement *>(&(*b)), true));
	}

	// name
	//
	if (i.getName()) {
		::xercesc::DOMAttr &a(::xsd::cxx::xml::dom::create_attribute("name", e));

		a << *i.getName();
	}
}

void operator<<(::xercesc::DOMElement &e, const EntryType &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::Type &>(i);

	// any_attribute
	//
	for (EntryType::AnyAttributeConstIterator b(i.getAnyAttribute().begin()), n(i.getAnyAttribute().end()); b != n;
	     ++b) {
		::xercesc::DOMAttr *a(static_cast<::xercesc::DOMAttr *>(
		    e.getOwnerDocument()->importNode(const_cast<::xercesc::DOMAttr *>(&(*b)), true)));

		if (a->getLocalName() == 0) e.setAttributeNode(a);
		else e.setAttributeNodeNS(a);
	}

	// display-name
	//
	if (i.getDisplayName()) {
		::xercesc::DOMElement &s(
		    ::xsd::cxx::xml::dom::create_element("display-name", "urn:ietf:params:xml:ns:resource-lists", e));

		s << *i.getDisplayName();
	}

	// any
	//
	for (EntryType::AnyConstIterator b(i.getAny().begin()), n(i.getAny().end()); b != n; ++b) {
		e.appendChild(e.getOwnerDocument()->importNode(const_cast<::xercesc::DOMElement *>(&(*b)), true));
	}

	// uri
	//
	{
		::xercesc::DOMAttr &a(::xsd::cxx::xml::dom::create_attribute("uri", e));

		a << i.getUri();
	}
}

void operator<<(::xercesc::DOMElement &e, const EntryRefType &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::Type &>(i);

	// any_attribute
	//
	for (EntryRefType::AnyAttributeConstIterator b(i.getAnyAttribute().begin()), n(i.getAnyAttribute().end()); b != n;
	     ++b) {
		::xercesc::DOMAttr *a(static_cast<::xercesc::DOMAttr *>(
		    e.getOwnerDocument()->importNode(const_cast<::xercesc::DOMAttr *>(&(*b)), true)));

		if (a->getLocalName() == 0) e.setAttributeNode(a);
		else e.setAttributeNodeNS(a);
	}

	// display-name
	//
	if (i.getDisplayName()) {
		::xercesc::DOMElement &s(
		    ::xsd::cxx::xml::dom::create_element("display-name", "urn:ietf:params:xml:ns:resource-lists", e));

		s << *i.getDisplayName();
	}

	// any
	//
	for (EntryRefType::AnyConstIterator b(i.getAny().begin()), n(i.getAny().end()); b != n; ++b) {
		e.appendChild(e.getOwnerDocument()->importNode(const_cast<::xercesc::DOMElement *>(&(*b)), true));
	}

	// ref
	//
	{
		::xercesc::DOMAttr &a(::xsd::cxx::xml::dom::create_attribute("ref", e));

		a << i.getRef();
	}
}

void operator<<(::xercesc::DOMElement &e, const ExternalType &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::Type &>(i);

	// any_attribute
	//
	for (ExternalType::AnyAttributeConstIterator b(i.getAnyAttribute().begin()), n(i.getAnyAttribute().end()); b != n;
	     ++b) {
		::xercesc::DOMAttr *a(static_cast<::xercesc::DOMAttr *>(
		    e.getOwnerDocument()->importNode(const_cast<::xercesc::DOMAttr *>(&(*b)), true)));

		if (a->getLocalName() == 0) e.setAttributeNode(a);
		else e.setAttributeNodeNS(a);
	}

	// display-name
	//
	if (i.getDisplayName()) {
		::xercesc::DOMElement &s(
		    ::xsd::cxx::xml::dom::create_element("display-name", "urn:ietf:params:xml:ns:resource-lists", e));

		s << *i.getDisplayName();
	}

	// any
	//
	for (ExternalType::AnyConstIterator b(i.getAny().begin()), n(i.getAny().end()); b != n; ++b) {
		e.appendChild(e.getOwnerDocument()->importNode(const_cast<::xercesc::DOMElement *>(&(*b)), true));
	}

	// anchor
	//
	if (i.getAnchor()) {
		::xercesc::DOMAttr &a(::xsd::cxx::xml::dom::create_attribute("anchor", e));

		a << *i.getAnchor();
	}
}

void serializeResourceLists(::std::ostream &o,
                            const ::LinphonePrivate::Xsd::ResourceLists::ResourceLists &s,
                            const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                            const ::std::string &e,
                            ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0);

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::ResourceLists::serializeResourceLists(s, m, f));

	::xsd::cxx::tree::error_handler<char> h;

	::xsd::cxx::xml::dom::ostream_format_target t(o);
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		h.throw_if_failed<::xsd::cxx::tree::serialization<char>>();
	}
}

void serializeResourceLists(::std::ostream &o,
                            const ::LinphonePrivate::Xsd::ResourceLists::ResourceLists &s,
                            ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
                            const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                            const ::std::string &e,
                            ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0);

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::ResourceLists::serializeResourceLists(s, m, f));
	::xsd::cxx::xml::dom::ostream_format_target t(o);
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		throw ::xsd::cxx::tree::serialization<char>();
	}
}

void serializeResourceLists(::std::ostream &o,
                            const ::LinphonePrivate::Xsd::ResourceLists::ResourceLists &s,
                            ::xercesc::DOMErrorHandler &h,
                            const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                            const ::std::string &e,
                            ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::ResourceLists::serializeResourceLists(s, m, f));
	::xsd::cxx::xml::dom::ostream_format_target t(o);
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		throw ::xsd::cxx::tree::serialization<char>();
	}
}

void serializeResourceLists(::xercesc::XMLFormatTarget &t,
                            const ::LinphonePrivate::Xsd::ResourceLists::ResourceLists &s,
                            const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                            const ::std::string &e,
                            ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::ResourceLists::serializeResourceLists(s, m, f));

	::xsd::cxx::tree::error_handler<char> h;

	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		h.throw_if_failed<::xsd::cxx::tree::serialization<char>>();
	}
}

void serializeResourceLists(::xercesc::XMLFormatTarget &t,
                            const ::LinphonePrivate::Xsd::ResourceLists::ResourceLists &s,
                            ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
                            const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                            const ::std::string &e,
                            ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::ResourceLists::serializeResourceLists(s, m, f));
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		throw ::xsd::cxx::tree::serialization<char>();
	}
}

void serializeResourceLists(::xercesc::XMLFormatTarget &t,
                            const ::LinphonePrivate::Xsd::ResourceLists::ResourceLists &s,
                            ::xercesc::DOMErrorHandler &h,
                            const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                            const ::std::string &e,
                            ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::ResourceLists::serializeResourceLists(s, m, f));
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		throw ::xsd::cxx::tree::serialization<char>();
	}
}

void serializeResourceLists(::xercesc::DOMDocument &d,
                            const ::LinphonePrivate::Xsd::ResourceLists::ResourceLists &s,
                            ::LinphonePrivate::Xsd::XmlSchema::Flags) {
	::xercesc::DOMElement &e(*d.getDocumentElement());
	const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(e));

	if (n.name() == "resource-lists" && n.namespace_() == "urn:ietf:params:xml:ns:resource-lists") {
		e << s;
	} else {
		throw ::xsd::cxx::tree::unexpected_element<char>(n.name(), n.namespace_(), "resource-lists",
		                                                 "urn:ietf:params:xml:ns:resource-lists");
	}
}

::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument>
serializeResourceLists(const ::LinphonePrivate::Xsd::ResourceLists::ResourceLists &s,
                       const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                       ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::serialize<char>("resource-lists", "urn:ietf:params:xml:ns:resource-lists", m, f));

	::LinphonePrivate::Xsd::ResourceLists::serializeResourceLists(*d, s, f);
	return d;
}

void operator<<(::xercesc::DOMElement &e, const DisplayNameType &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);

	// lang
	//
	if (i.getLang()) {
		::xercesc::DOMAttr &a(
		    ::xsd::cxx::xml::dom::create_attribute("lang", "http://www.w3.org/XML/1998/namespace", e));

		a << *i.getLang();
	}
}

void operator<<(::xercesc::DOMElement &e, const List &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::ResourceLists::ListType &>(i);
}

void operator<<(::xercesc::DOMElement &e, const DisplayName &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::ResourceLists::DisplayNameType &>(i);
}

void operator<<(::xercesc::DOMElement &e, const ResourceLists &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::Type &>(i);

	// list
	//
	for (ResourceLists::ListConstIterator b(i.getList().begin()), n(i.getList().end()); b != n; ++b) {
		::xercesc::DOMElement &s(
		    ::xsd::cxx::xml::dom::create_element("list", "urn:ietf:params:xml:ns:resource-lists", e));

		s << *b;
	}
}
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
