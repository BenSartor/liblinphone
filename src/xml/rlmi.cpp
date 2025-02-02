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

#include "rlmi.h"

namespace LinphonePrivate {
namespace Xsd {
namespace Rlmi {
// List
//

const List::NameSequence &List::getName() const {
	return this->name_;
}

List::NameSequence &List::getName() {
	return this->name_;
}

void List::setName(const NameSequence &s) {
	this->name_ = s;
}

const List::ResourceSequence &List::getResource() const {
	return this->resource_;
}

List::ResourceSequence &List::getResource() {
	return this->resource_;
}

void List::setResource(const ResourceSequence &s) {
	this->resource_ = s;
}

const List::UriType &List::getUri() const {
	return this->uri_.get();
}

List::UriType &List::getUri() {
	return this->uri_.get();
}

void List::setUri(const UriType &x) {
	this->uri_.set(x);
}

void List::setUri(::std::unique_ptr<UriType> x) {
	this->uri_.set(std::move(x));
}

::std::unique_ptr<List::UriType> List::setDetachUri() {
	return this->uri_.detach();
}

const List::VersionType &List::getVersion() const {
	return this->version_.get();
}

List::VersionType &List::getVersion() {
	return this->version_.get();
}

void List::setVersion(const VersionType &x) {
	this->version_.set(x);
}

const List::FullStateType &List::getFullState() const {
	return this->fullState_.get();
}

List::FullStateType &List::getFullState() {
	return this->fullState_.get();
}

void List::setFullState(const FullStateType &x) {
	this->fullState_.set(x);
}

const List::CidOptional &List::getCid() const {
	return this->cid_;
}

List::CidOptional &List::getCid() {
	return this->cid_;
}

void List::setCid(const CidType &x) {
	this->cid_.set(x);
}

void List::setCid(const CidOptional &x) {
	this->cid_ = x;
}

void List::setCid(::std::unique_ptr<CidType> x) {
	this->cid_.set(std::move(x));
}

const List::AnyAttributeSet &List::getAnyAttribute() const {
	return this->any_attribute_;
}

List::AnyAttributeSet &List::getAnyAttribute() {
	return this->any_attribute_;
}

void List::setAnyAttribute(const AnyAttributeSet &s) {
	this->any_attribute_ = s;
}

const ::xercesc::DOMDocument &List::getDomDocument() const {
	return *this->dom_document_;
}

::xercesc::DOMDocument &List::getDomDocument() {
	return *this->dom_document_;
}

// Resource
//

const Resource::NameSequence &Resource::getName() const {
	return this->name_;
}

Resource::NameSequence &Resource::getName() {
	return this->name_;
}

void Resource::setName(const NameSequence &s) {
	this->name_ = s;
}

const Resource::InstanceSequence &Resource::getInstance() const {
	return this->instance_;
}

Resource::InstanceSequence &Resource::getInstance() {
	return this->instance_;
}

void Resource::setInstance(const InstanceSequence &s) {
	this->instance_ = s;
}

const Resource::UriType &Resource::getUri() const {
	return this->uri_.get();
}

Resource::UriType &Resource::getUri() {
	return this->uri_.get();
}

void Resource::setUri(const UriType &x) {
	this->uri_.set(x);
}

void Resource::setUri(::std::unique_ptr<UriType> x) {
	this->uri_.set(std::move(x));
}

::std::unique_ptr<Resource::UriType> Resource::setDetachUri() {
	return this->uri_.detach();
}

const Resource::AnyAttributeSet &Resource::getAnyAttribute() const {
	return this->any_attribute_;
}

Resource::AnyAttributeSet &Resource::getAnyAttribute() {
	return this->any_attribute_;
}

void Resource::setAnyAttribute(const AnyAttributeSet &s) {
	this->any_attribute_ = s;
}

const ::xercesc::DOMDocument &Resource::getDomDocument() const {
	return *this->dom_document_;
}

::xercesc::DOMDocument &Resource::getDomDocument() {
	return *this->dom_document_;
}

// Instance
//

const Instance::AnySequence &Instance::getAny() const {
	return this->any_;
}

Instance::AnySequence &Instance::getAny() {
	return this->any_;
}

void Instance::setAny(const AnySequence &s) {
	this->any_ = s;
}

const Instance::IdType &Instance::getId() const {
	return this->id_.get();
}

Instance::IdType &Instance::getId() {
	return this->id_.get();
}

void Instance::setId(const IdType &x) {
	this->id_.set(x);
}

void Instance::setId(::std::unique_ptr<IdType> x) {
	this->id_.set(std::move(x));
}

::std::unique_ptr<Instance::IdType> Instance::setDetachId() {
	return this->id_.detach();
}

const Instance::StateType &Instance::getState() const {
	return this->state_.get();
}

Instance::StateType &Instance::getState() {
	return this->state_.get();
}

void Instance::setState(const StateType &x) {
	this->state_.set(x);
}

void Instance::setState(::std::unique_ptr<StateType> x) {
	this->state_.set(std::move(x));
}

::std::unique_ptr<Instance::StateType> Instance::setDetachState() {
	return this->state_.detach();
}

const Instance::ReasonOptional &Instance::getReason() const {
	return this->reason_;
}

Instance::ReasonOptional &Instance::getReason() {
	return this->reason_;
}

void Instance::setReason(const ReasonType &x) {
	this->reason_.set(x);
}

void Instance::setReason(const ReasonOptional &x) {
	this->reason_ = x;
}

void Instance::setReason(::std::unique_ptr<ReasonType> x) {
	this->reason_.set(std::move(x));
}

const Instance::CidOptional &Instance::getCid() const {
	return this->cid_;
}

Instance::CidOptional &Instance::getCid() {
	return this->cid_;
}

void Instance::setCid(const CidType &x) {
	this->cid_.set(x);
}

void Instance::setCid(const CidOptional &x) {
	this->cid_ = x;
}

void Instance::setCid(::std::unique_ptr<CidType> x) {
	this->cid_.set(std::move(x));
}

const Instance::AnyAttributeSet &Instance::getAnyAttribute() const {
	return this->any_attribute_;
}

Instance::AnyAttributeSet &Instance::getAnyAttribute() {
	return this->any_attribute_;
}

void Instance::setAnyAttribute(const AnyAttributeSet &s) {
	this->any_attribute_ = s;
}

const ::xercesc::DOMDocument &Instance::getDomDocument() const {
	return *this->dom_document_;
}

::xercesc::DOMDocument &Instance::getDomDocument() {
	return *this->dom_document_;
}

// Name
//

const Name::LangOptional &Name::getLang() const {
	return this->lang_;
}

Name::LangOptional &Name::getLang() {
	return this->lang_;
}

void Name::setLang(const LangType &x) {
	this->lang_.set(x);
}

void Name::setLang(const LangOptional &x) {
	this->lang_ = x;
}

void Name::setLang(::std::unique_ptr<LangType> x) {
	this->lang_.set(std::move(x));
}

// State
//

State::State(Value v) : ::LinphonePrivate::Xsd::XmlSchema::String(_xsd_State_literals_[v]) {
}

State::State(const char *v) : ::LinphonePrivate::Xsd::XmlSchema::String(v) {
}

State::State(const ::std::string &v) : ::LinphonePrivate::Xsd::XmlSchema::String(v) {
}

State::State(const ::LinphonePrivate::Xsd::XmlSchema::String &v) : ::LinphonePrivate::Xsd::XmlSchema::String(v) {
}

State::State(const State &v,
             ::LinphonePrivate::Xsd::XmlSchema::Flags f,
             ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(v, f, c) {
}

State &State::operator=(Value v) {
	static_cast<::LinphonePrivate::Xsd::XmlSchema::String &>(*this) =
	    ::LinphonePrivate::Xsd::XmlSchema::String(_xsd_State_literals_[v]);

	return *this;
}
} // namespace Rlmi
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
namespace Rlmi {
// List
//

List::List(const UriType &uri, const VersionType &version, const FullStateType &fullState)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(), dom_document_(::xsd::cxx::xml::dom::create_document<char>()),
      name_(this), resource_(this), uri_(uri, this), version_(version, this), fullState_(fullState, this), cid_(this),
      any_attribute_(this->getDomDocument()) {
}

List::List(const List &x, ::LinphonePrivate::Xsd::XmlSchema::Flags f, ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(x, f, c), dom_document_(::xsd::cxx::xml::dom::create_document<char>()),
      name_(x.name_, f, this), resource_(x.resource_, f, this), uri_(x.uri_, f, this), version_(x.version_, f, this),
      fullState_(x.fullState_, f, this), cid_(x.cid_, f, this),
      any_attribute_(x.any_attribute_, this->getDomDocument()) {
}

List::List(const ::xercesc::DOMElement &e,
           ::LinphonePrivate::Xsd::XmlSchema::Flags f,
           ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(e, f | ::LinphonePrivate::Xsd::XmlSchema::Flags::base, c),
      dom_document_(::xsd::cxx::xml::dom::create_document<char>()), name_(this), resource_(this), uri_(this),
      version_(this), fullState_(this), cid_(this), any_attribute_(this->getDomDocument()) {
	if ((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::base) == 0) {
		::xsd::cxx::xml::dom::parser<char> p(e, true, false, true);
		this->parse(p, f);
	}
}

void List::parse(::xsd::cxx::xml::dom::parser<char> &p, ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	for (; p.more_content(); p.next_content(false)) {
		const ::xercesc::DOMElement &i(p.cur_element());
		const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(i));

		// name
		//
		if (n.name() == "name" && n.namespace_() == "urn:ietf:params:xml:ns:rlmi") {
			::std::unique_ptr<NameType> r(NameTraits::create(i, f, this));

			this->name_.push_back(::std::move(r));
			continue;
		}

		// resource
		//
		if (n.name() == "resource" && n.namespace_() == "urn:ietf:params:xml:ns:rlmi") {
			::std::unique_ptr<ResourceType> r(ResourceTraits::create(i, f, this));

			this->resource_.push_back(::std::move(r));
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

		if (n.name() == "version" && n.namespace_().empty()) {
			this->version_.set(VersionTraits::create(i, f, this));
			continue;
		}

		if (n.name() == "fullState" && n.namespace_().empty()) {
			this->fullState_.set(FullStateTraits::create(i, f, this));
			continue;
		}

		if (n.name() == "cid" && n.namespace_().empty()) {
			this->cid_.set(CidTraits::create(i, f, this));
			continue;
		}

		// any_attribute
		//
		if ((n.namespace_() != ::xsd::cxx::xml::bits::xmlns_namespace<char>() &&
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

	if (!version_.present()) {
		throw ::xsd::cxx::tree::expected_attribute<char>("version", "");
	}

	if (!fullState_.present()) {
		throw ::xsd::cxx::tree::expected_attribute<char>("fullState", "");
	}
}

List *List::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f, ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class List(*this, f, c);
}

List &List::operator=(const List &x) {
	if (this != &x) {
		static_cast<::LinphonePrivate::Xsd::XmlSchema::Type &>(*this) = x;
		this->name_ = x.name_;
		this->resource_ = x.resource_;
		this->uri_ = x.uri_;
		this->version_ = x.version_;
		this->fullState_ = x.fullState_;
		this->cid_ = x.cid_;
		this->any_attribute_ = x.any_attribute_;
	}

	return *this;
}

List::~List() {
}

// Resource
//

Resource::Resource(const UriType &uri)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(), dom_document_(::xsd::cxx::xml::dom::create_document<char>()),
      name_(this), instance_(this), uri_(uri, this), any_attribute_(this->getDomDocument()) {
}

Resource::Resource(const Resource &x,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(x, f, c), dom_document_(::xsd::cxx::xml::dom::create_document<char>()),
      name_(x.name_, f, this), instance_(x.instance_, f, this), uri_(x.uri_, f, this),
      any_attribute_(x.any_attribute_, this->getDomDocument()) {
}

Resource::Resource(const ::xercesc::DOMElement &e,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(e, f | ::LinphonePrivate::Xsd::XmlSchema::Flags::base, c),
      dom_document_(::xsd::cxx::xml::dom::create_document<char>()), name_(this), instance_(this), uri_(this),
      any_attribute_(this->getDomDocument()) {
	if ((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::base) == 0) {
		::xsd::cxx::xml::dom::parser<char> p(e, true, false, true);
		this->parse(p, f);
	}
}

void Resource::parse(::xsd::cxx::xml::dom::parser<char> &p, ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	for (; p.more_content(); p.next_content(false)) {
		const ::xercesc::DOMElement &i(p.cur_element());
		const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(i));

		// name
		//
		if (n.name() == "name" && n.namespace_() == "urn:ietf:params:xml:ns:rlmi") {
			::std::unique_ptr<NameType> r(NameTraits::create(i, f, this));

			this->name_.push_back(::std::move(r));
			continue;
		}

		// instance
		//
		if (n.name() == "instance" && n.namespace_() == "urn:ietf:params:xml:ns:rlmi") {
			::std::unique_ptr<InstanceType> r(InstanceTraits::create(i, f, this));

			this->instance_.push_back(::std::move(r));
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
		if ((n.namespace_() != ::xsd::cxx::xml::bits::xmlns_namespace<char>() &&
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

Resource *Resource::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f,
                           ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class Resource(*this, f, c);
}

Resource &Resource::operator=(const Resource &x) {
	if (this != &x) {
		static_cast<::LinphonePrivate::Xsd::XmlSchema::Type &>(*this) = x;
		this->name_ = x.name_;
		this->instance_ = x.instance_;
		this->uri_ = x.uri_;
		this->any_attribute_ = x.any_attribute_;
	}

	return *this;
}

Resource::~Resource() {
}

// Instance
//

Instance::Instance(const IdType &id, const StateType &state)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(), dom_document_(::xsd::cxx::xml::dom::create_document<char>()),
      any_(this->getDomDocument()), id_(id, this), state_(state, this), reason_(this), cid_(this),
      any_attribute_(this->getDomDocument()) {
}

Instance::Instance(const Instance &x,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(x, f, c), dom_document_(::xsd::cxx::xml::dom::create_document<char>()),
      any_(x.any_, this->getDomDocument()), id_(x.id_, f, this), state_(x.state_, f, this), reason_(x.reason_, f, this),
      cid_(x.cid_, f, this), any_attribute_(x.any_attribute_, this->getDomDocument()) {
}

Instance::Instance(const ::xercesc::DOMElement &e,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f,
                   ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::Type(e, f | ::LinphonePrivate::Xsd::XmlSchema::Flags::base, c),
      dom_document_(::xsd::cxx::xml::dom::create_document<char>()), any_(this->getDomDocument()), id_(this),
      state_(this), reason_(this), cid_(this), any_attribute_(this->getDomDocument()) {
	if ((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::base) == 0) {
		::xsd::cxx::xml::dom::parser<char> p(e, true, false, true);
		this->parse(p, f);
	}
}

void Instance::parse(::xsd::cxx::xml::dom::parser<char> &p, ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	for (; p.more_content(); p.next_content(false)) {
		const ::xercesc::DOMElement &i(p.cur_element());
		const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(i));

		// any
		//
		if (true) {
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

		if (n.name() == "id" && n.namespace_().empty()) {
			this->id_.set(IdTraits::create(i, f, this));
			continue;
		}

		if (n.name() == "state" && n.namespace_().empty()) {
			this->state_.set(StateTraits::create(i, f, this));
			continue;
		}

		if (n.name() == "reason" && n.namespace_().empty()) {
			this->reason_.set(ReasonTraits::create(i, f, this));
			continue;
		}

		if (n.name() == "cid" && n.namespace_().empty()) {
			this->cid_.set(CidTraits::create(i, f, this));
			continue;
		}

		// any_attribute
		//
		if ((n.namespace_() != ::xsd::cxx::xml::bits::xmlns_namespace<char>() &&
		     n.namespace_() != ::xsd::cxx::xml::bits::xsi_namespace<char>())) {
			::xercesc::DOMAttr *r(static_cast<::xercesc::DOMAttr *>(
			    this->getDomDocument().importNode(const_cast<::xercesc::DOMAttr *>(&i), true)));
			this->any_attribute_.insert(r);
			continue;
		}
	}

	if (!id_.present()) {
		throw ::xsd::cxx::tree::expected_attribute<char>("id", "");
	}

	if (!state_.present()) {
		throw ::xsd::cxx::tree::expected_attribute<char>("state", "");
	}
}

Instance *Instance::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f,
                           ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class Instance(*this, f, c);
}

Instance &Instance::operator=(const Instance &x) {
	if (this != &x) {
		static_cast<::LinphonePrivate::Xsd::XmlSchema::Type &>(*this) = x;
		this->any_ = x.any_;
		this->id_ = x.id_;
		this->state_ = x.state_;
		this->reason_ = x.reason_;
		this->cid_ = x.cid_;
		this->any_attribute_ = x.any_attribute_;
	}

	return *this;
}

Instance::~Instance() {
}

// Name
//

Name::Name() : ::LinphonePrivate::Xsd::XmlSchema::String(), lang_(this) {
}

Name::Name(const char *_xsd_String_base) : ::LinphonePrivate::Xsd::XmlSchema::String(_xsd_String_base), lang_(this) {
}

Name::Name(const ::std::string &_xsd_String_base)
    : ::LinphonePrivate::Xsd::XmlSchema::String(_xsd_String_base), lang_(this) {
}

Name::Name(const ::LinphonePrivate::Xsd::XmlSchema::String &_xsd_String_base)
    : ::LinphonePrivate::Xsd::XmlSchema::String(_xsd_String_base), lang_(this) {
}

Name::Name(const Name &x, ::LinphonePrivate::Xsd::XmlSchema::Flags f, ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(x, f, c), lang_(x.lang_, f, this) {
}

Name::Name(const ::xercesc::DOMElement &e,
           ::LinphonePrivate::Xsd::XmlSchema::Flags f,
           ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(e, f | ::LinphonePrivate::Xsd::XmlSchema::Flags::base, c), lang_(this) {
	if ((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::base) == 0) {
		::xsd::cxx::xml::dom::parser<char> p(e, false, false, true);
		this->parse(p, f);
	}
}

void Name::parse(::xsd::cxx::xml::dom::parser<char> &p, ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	while (p.more_attributes()) {
		const ::xercesc::DOMAttr &i(p.next_attribute());
		const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(i));

		if (n.name() == "lang" && n.namespace_() == "http://www.w3.org/XML/1998/namespace") {
			this->lang_.set(LangTraits::create(i, f, this));
			continue;
		}
	}
}

Name *Name::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f, ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class Name(*this, f, c);
}

Name &Name::operator=(const Name &x) {
	if (this != &x) {
		static_cast<::LinphonePrivate::Xsd::XmlSchema::String &>(*this) = x;
		this->lang_ = x.lang_;
	}

	return *this;
}

Name::~Name() {
}

// State
//

State::State(const ::xercesc::DOMElement &e,
             ::LinphonePrivate::Xsd::XmlSchema::Flags f,
             ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(e, f, c) {
	_xsd_State_convert();
}

State::State(const ::xercesc::DOMAttr &a,
             ::LinphonePrivate::Xsd::XmlSchema::Flags f,
             ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(a, f, c) {
	_xsd_State_convert();
}

State::State(const ::std::string &s,
             const ::xercesc::DOMElement *e,
             ::LinphonePrivate::Xsd::XmlSchema::Flags f,
             ::LinphonePrivate::Xsd::XmlSchema::Container *c)
    : ::LinphonePrivate::Xsd::XmlSchema::String(s, e, f, c) {
	_xsd_State_convert();
}

State *State::_clone(::LinphonePrivate::Xsd::XmlSchema::Flags f,
                     ::LinphonePrivate::Xsd::XmlSchema::Container *c) const {
	return new class State(*this, f, c);
}

State::Value State::_xsd_State_convert() const {
	::xsd::cxx::tree::enum_comparator<char> c(_xsd_State_literals_);
	const Value *i(::std::lower_bound(_xsd_State_indexes_, _xsd_State_indexes_ + 3, *this, c));

	if (i == _xsd_State_indexes_ + 3 || _xsd_State_literals_[*i] != *this) {
		throw ::xsd::cxx::tree::unexpected_enumerator<char>(*this);
	}

	return *i;
}

const char *const State::_xsd_State_literals_[3] = {"active", "pending", "terminated"};

const State::Value State::_xsd_State_indexes_[3] = {::LinphonePrivate::Xsd::Rlmi::State::active,
                                                    ::LinphonePrivate::Xsd::Rlmi::State::pending,
                                                    ::LinphonePrivate::Xsd::Rlmi::State::terminated};
} // namespace Rlmi
} // namespace Xsd
} // namespace LinphonePrivate

#include <ostream>

#include <xsd/cxx/tree/std-ostream-map.hxx>

namespace _xsd {
static const ::xsd::cxx::tree::std_ostream_plate<0, char> std_ostream_plate_init;
}

namespace LinphonePrivate {
namespace Xsd {
namespace Rlmi {
::std::ostream &operator<<(::std::ostream &o, const List &i) {
	for (List::NameConstIterator b(i.getName().begin()), e(i.getName().end()); b != e; ++b) {
		o << ::std::endl << "name: " << *b;
	}

	for (List::ResourceConstIterator b(i.getResource().begin()), e(i.getResource().end()); b != e; ++b) {
		o << ::std::endl << "resource: " << *b;
	}

	o << ::std::endl << "uri: " << i.getUri();
	o << ::std::endl << "version: " << i.getVersion();
	o << ::std::endl << "fullState: " << i.getFullState();
	if (i.getCid()) {
		o << ::std::endl << "cid: " << *i.getCid();
	}

	return o;
}

::std::ostream &operator<<(::std::ostream &o, const Resource &i) {
	for (Resource::NameConstIterator b(i.getName().begin()), e(i.getName().end()); b != e; ++b) {
		o << ::std::endl << "name: " << *b;
	}

	for (Resource::InstanceConstIterator b(i.getInstance().begin()), e(i.getInstance().end()); b != e; ++b) {
		o << ::std::endl << "instance: " << *b;
	}

	o << ::std::endl << "uri: " << i.getUri();
	return o;
}

::std::ostream &operator<<(::std::ostream &o, const Instance &i) {
	o << ::std::endl << "id: " << i.getId();
	o << ::std::endl << "state: " << i.getState();
	if (i.getReason()) {
		o << ::std::endl << "reason: " << *i.getReason();
	}

	if (i.getCid()) {
		o << ::std::endl << "cid: " << *i.getCid();
	}

	return o;
}

::std::ostream &operator<<(::std::ostream &o, const Name &i) {
	o << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);

	if (i.getLang()) {
		o << ::std::endl << "lang: " << *i.getLang();
	}

	return o;
}

::std::ostream &operator<<(::std::ostream &o, State::Value i) {
	return o << State::_xsd_State_literals_[i];
}

::std::ostream &operator<<(::std::ostream &o, const State &i) {
	return o << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);
}
} // namespace Rlmi
} // namespace Xsd
} // namespace LinphonePrivate

#include <istream>
#include <xsd/cxx/tree/error-handler.hxx>
#include <xsd/cxx/xml/sax/std-input-source.hxx>

namespace LinphonePrivate {
namespace Xsd {
namespace Rlmi {
::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>
parseList(const ::std::string &u,
          ::LinphonePrivate::Xsd::XmlSchema::Flags f,
          const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::tree::error_handler<char> h;

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(u, h, p, f));

	h.throw_if_failed<::xsd::cxx::tree::parsing<char>>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>(::LinphonePrivate::Xsd::Rlmi::parseList(
	    std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>
parseList(const ::std::string &u,
          ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
          ::LinphonePrivate::Xsd::XmlSchema::Flags f,
          const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(u, h, p, f));

	if (!d.get()) throw ::xsd::cxx::tree::parsing<char>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>(::LinphonePrivate::Xsd::Rlmi::parseList(
	    std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>
parseList(const ::std::string &u,
          ::xercesc::DOMErrorHandler &h,
          ::LinphonePrivate::Xsd::XmlSchema::Flags f,
          const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(u, h, p, f));

	if (!d.get()) throw ::xsd::cxx::tree::parsing<char>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>(::LinphonePrivate::Xsd::Rlmi::parseList(
	    std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>
parseList(::std::istream &is,
          ::LinphonePrivate::Xsd::XmlSchema::Flags f,
          const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::xml::sax::std_input_source isrc(is);
	return ::LinphonePrivate::Xsd::Rlmi::parseList(isrc, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>
parseList(::std::istream &is,
          ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
          ::LinphonePrivate::Xsd::XmlSchema::Flags f,
          const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::xml::sax::std_input_source isrc(is);
	return ::LinphonePrivate::Xsd::Rlmi::parseList(isrc, h, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>
parseList(::std::istream &is,
          ::xercesc::DOMErrorHandler &h,
          ::LinphonePrivate::Xsd::XmlSchema::Flags f,
          const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::sax::std_input_source isrc(is);
	return ::LinphonePrivate::Xsd::Rlmi::parseList(isrc, h, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>
parseList(::std::istream &is,
          const ::std::string &sid,
          ::LinphonePrivate::Xsd::XmlSchema::Flags f,
          const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::xml::sax::std_input_source isrc(is, sid);
	return ::LinphonePrivate::Xsd::Rlmi::parseList(isrc, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>
parseList(::std::istream &is,
          const ::std::string &sid,
          ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
          ::LinphonePrivate::Xsd::XmlSchema::Flags f,
          const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0,
	                                    (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) == 0);

	::xsd::cxx::xml::sax::std_input_source isrc(is, sid);
	return ::LinphonePrivate::Xsd::Rlmi::parseList(isrc, h, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>
parseList(::std::istream &is,
          const ::std::string &sid,
          ::xercesc::DOMErrorHandler &h,
          ::LinphonePrivate::Xsd::XmlSchema::Flags f,
          const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::xml::sax::std_input_source isrc(is, sid);
	return ::LinphonePrivate::Xsd::Rlmi::parseList(isrc, h, f, p);
}

::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>
parseList(::xercesc::InputSource &i,
          ::LinphonePrivate::Xsd::XmlSchema::Flags f,
          const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::xsd::cxx::tree::error_handler<char> h;

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(i, h, p, f));

	h.throw_if_failed<::xsd::cxx::tree::parsing<char>>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>(::LinphonePrivate::Xsd::Rlmi::parseList(
	    std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>
parseList(::xercesc::InputSource &i,
          ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
          ::LinphonePrivate::Xsd::XmlSchema::Flags f,
          const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(i, h, p, f));

	if (!d.get()) throw ::xsd::cxx::tree::parsing<char>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>(::LinphonePrivate::Xsd::Rlmi::parseList(
	    std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>
parseList(::xercesc::InputSource &i,
          ::xercesc::DOMErrorHandler &h,
          ::LinphonePrivate::Xsd::XmlSchema::Flags f,
          const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::parse<char>(i, h, p, f));

	if (!d.get()) throw ::xsd::cxx::tree::parsing<char>();

	return ::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>(::LinphonePrivate::Xsd::Rlmi::parseList(
	    std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
}

::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>
parseList(const ::xercesc::DOMDocument &doc,
          ::LinphonePrivate::Xsd::XmlSchema::Flags f,
          const ::LinphonePrivate::Xsd::XmlSchema::Properties &p) {
	if (f & ::LinphonePrivate::Xsd::XmlSchema::Flags::keep_dom) {
		::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
		    static_cast<::xercesc::DOMDocument *>(doc.cloneNode(true)));

		return ::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>(::LinphonePrivate::Xsd::Rlmi::parseList(
		    std::move(d), f | ::LinphonePrivate::Xsd::XmlSchema::Flags::own_dom, p));
	}

	const ::xercesc::DOMElement &e(*doc.getDocumentElement());
	const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(e));

	if (n.name() == "list" && n.namespace_() == "urn:ietf:params:xml:ns:rlmi") {
		::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List> r(
		    ::xsd::cxx::tree::traits<::LinphonePrivate::Xsd::Rlmi::List, char>::create(e, f, 0));
		return r;
	}

	throw ::xsd::cxx::tree::unexpected_element<char>(n.name(), n.namespace_(), "list", "urn:ietf:params:xml:ns:rlmi");
}

::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List>
parseList(::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d,
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

	if (n.name() == "list" && n.namespace_() == "urn:ietf:params:xml:ns:rlmi") {
		::std::unique_ptr<::LinphonePrivate::Xsd::Rlmi::List> r(
		    ::xsd::cxx::tree::traits<::LinphonePrivate::Xsd::Rlmi::List, char>::create(e, f, 0));
		return r;
	}

	throw ::xsd::cxx::tree::unexpected_element<char>(n.name(), n.namespace_(), "list", "urn:ietf:params:xml:ns:rlmi");
}
} // namespace Rlmi
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
namespace Rlmi {
void serializeList(::std::ostream &o,
                   const ::LinphonePrivate::Xsd::Rlmi::List &s,
                   const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                   const ::std::string &e,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0);

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::Rlmi::serializeList(s, m, f));

	::xsd::cxx::tree::error_handler<char> h;

	::xsd::cxx::xml::dom::ostream_format_target t(o);
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		h.throw_if_failed<::xsd::cxx::tree::serialization<char>>();
	}
}

void serializeList(::std::ostream &o,
                   const ::LinphonePrivate::Xsd::Rlmi::List &s,
                   ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
                   const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                   const ::std::string &e,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::xsd::cxx::xml::auto_initializer i((f & ::LinphonePrivate::Xsd::XmlSchema::Flags::dont_initialize) == 0);

	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::Rlmi::serializeList(s, m, f));
	::xsd::cxx::xml::dom::ostream_format_target t(o);
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		throw ::xsd::cxx::tree::serialization<char>();
	}
}

void serializeList(::std::ostream &o,
                   const ::LinphonePrivate::Xsd::Rlmi::List &s,
                   ::xercesc::DOMErrorHandler &h,
                   const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                   const ::std::string &e,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::Rlmi::serializeList(s, m, f));
	::xsd::cxx::xml::dom::ostream_format_target t(o);
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		throw ::xsd::cxx::tree::serialization<char>();
	}
}

void serializeList(::xercesc::XMLFormatTarget &t,
                   const ::LinphonePrivate::Xsd::Rlmi::List &s,
                   const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                   const ::std::string &e,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::Rlmi::serializeList(s, m, f));

	::xsd::cxx::tree::error_handler<char> h;

	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		h.throw_if_failed<::xsd::cxx::tree::serialization<char>>();
	}
}

void serializeList(::xercesc::XMLFormatTarget &t,
                   const ::LinphonePrivate::Xsd::Rlmi::List &s,
                   ::LinphonePrivate::Xsd::XmlSchema::ErrorHandler &h,
                   const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                   const ::std::string &e,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::Rlmi::serializeList(s, m, f));
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		throw ::xsd::cxx::tree::serialization<char>();
	}
}

void serializeList(::xercesc::XMLFormatTarget &t,
                   const ::LinphonePrivate::Xsd::Rlmi::List &s,
                   ::xercesc::DOMErrorHandler &h,
                   const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
                   const ::std::string &e,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::LinphonePrivate::Xsd::Rlmi::serializeList(s, m, f));
	if (!::xsd::cxx::xml::dom::serialize(t, *d, e, h, f)) {
		throw ::xsd::cxx::tree::serialization<char>();
	}
}

void serializeList(::xercesc::DOMDocument &d,
                   const ::LinphonePrivate::Xsd::Rlmi::List &s,
                   ::LinphonePrivate::Xsd::XmlSchema::Flags) {
	::xercesc::DOMElement &e(*d.getDocumentElement());
	const ::xsd::cxx::xml::qualified_name<char> n(::xsd::cxx::xml::dom::name<char>(e));

	if (n.name() == "list" && n.namespace_() == "urn:ietf:params:xml:ns:rlmi") {
		e << s;
	} else {
		throw ::xsd::cxx::tree::unexpected_element<char>(n.name(), n.namespace_(), "list",
		                                                 "urn:ietf:params:xml:ns:rlmi");
	}
}

::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument>
serializeList(const ::LinphonePrivate::Xsd::Rlmi::List &s,
              const ::LinphonePrivate::Xsd::XmlSchema::NamespaceInfomap &m,
              ::LinphonePrivate::Xsd::XmlSchema::Flags f) {
	::LinphonePrivate::Xsd::XmlSchema::dom::unique_ptr<::xercesc::DOMDocument> d(
	    ::xsd::cxx::xml::dom::serialize<char>("list", "urn:ietf:params:xml:ns:rlmi", m, f));

	::LinphonePrivate::Xsd::Rlmi::serializeList(*d, s, f);
	return d;
}

void operator<<(::xercesc::DOMElement &e, const List &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::Type &>(i);

	// any_attribute
	//
	for (List::AnyAttributeConstIterator b(i.getAnyAttribute().begin()), n(i.getAnyAttribute().end()); b != n; ++b) {
		::xercesc::DOMAttr *a(static_cast<::xercesc::DOMAttr *>(
		    e.getOwnerDocument()->importNode(const_cast<::xercesc::DOMAttr *>(&(*b)), true)));

		if (a->getLocalName() == 0) e.setAttributeNode(a);
		else e.setAttributeNodeNS(a);
	}

	// name
	//
	for (List::NameConstIterator b(i.getName().begin()), n(i.getName().end()); b != n; ++b) {
		::xercesc::DOMElement &s(::xsd::cxx::xml::dom::create_element("name", "urn:ietf:params:xml:ns:rlmi", e));

		s << *b;
	}

	// resource
	//
	for (List::ResourceConstIterator b(i.getResource().begin()), n(i.getResource().end()); b != n; ++b) {
		::xercesc::DOMElement &s(::xsd::cxx::xml::dom::create_element("resource", "urn:ietf:params:xml:ns:rlmi", e));

		s << *b;
	}

	// uri
	//
	{
		::xercesc::DOMAttr &a(::xsd::cxx::xml::dom::create_attribute("uri", e));

		a << i.getUri();
	}

	// version
	//
	{
		::xercesc::DOMAttr &a(::xsd::cxx::xml::dom::create_attribute("version", e));

		a << i.getVersion();
	}

	// fullState
	//
	{
		::xercesc::DOMAttr &a(::xsd::cxx::xml::dom::create_attribute("fullState", e));

		a << i.getFullState();
	}

	// cid
	//
	if (i.getCid()) {
		::xercesc::DOMAttr &a(::xsd::cxx::xml::dom::create_attribute("cid", e));

		a << *i.getCid();
	}
}

void operator<<(::xercesc::DOMElement &e, const Resource &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::Type &>(i);

	// any_attribute
	//
	for (Resource::AnyAttributeConstIterator b(i.getAnyAttribute().begin()), n(i.getAnyAttribute().end()); b != n;
	     ++b) {
		::xercesc::DOMAttr *a(static_cast<::xercesc::DOMAttr *>(
		    e.getOwnerDocument()->importNode(const_cast<::xercesc::DOMAttr *>(&(*b)), true)));

		if (a->getLocalName() == 0) e.setAttributeNode(a);
		else e.setAttributeNodeNS(a);
	}

	// name
	//
	for (Resource::NameConstIterator b(i.getName().begin()), n(i.getName().end()); b != n; ++b) {
		::xercesc::DOMElement &s(::xsd::cxx::xml::dom::create_element("name", "urn:ietf:params:xml:ns:rlmi", e));

		s << *b;
	}

	// instance
	//
	for (Resource::InstanceConstIterator b(i.getInstance().begin()), n(i.getInstance().end()); b != n; ++b) {
		::xercesc::DOMElement &s(::xsd::cxx::xml::dom::create_element("instance", "urn:ietf:params:xml:ns:rlmi", e));

		s << *b;
	}

	// uri
	//
	{
		::xercesc::DOMAttr &a(::xsd::cxx::xml::dom::create_attribute("uri", e));

		a << i.getUri();
	}
}

void operator<<(::xercesc::DOMElement &e, const Instance &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::Type &>(i);

	// any_attribute
	//
	for (Instance::AnyAttributeConstIterator b(i.getAnyAttribute().begin()), n(i.getAnyAttribute().end()); b != n;
	     ++b) {
		::xercesc::DOMAttr *a(static_cast<::xercesc::DOMAttr *>(
		    e.getOwnerDocument()->importNode(const_cast<::xercesc::DOMAttr *>(&(*b)), true)));

		if (a->getLocalName() == 0) e.setAttributeNode(a);
		else e.setAttributeNodeNS(a);
	}

	// any
	//
	for (Instance::AnyConstIterator b(i.getAny().begin()), n(i.getAny().end()); b != n; ++b) {
		e.appendChild(e.getOwnerDocument()->importNode(const_cast<::xercesc::DOMElement *>(&(*b)), true));
	}

	// id
	//
	{
		::xercesc::DOMAttr &a(::xsd::cxx::xml::dom::create_attribute("id", e));

		a << i.getId();
	}

	// state
	//
	{
		::xercesc::DOMAttr &a(::xsd::cxx::xml::dom::create_attribute("state", e));

		a << i.getState();
	}

	// reason
	//
	if (i.getReason()) {
		::xercesc::DOMAttr &a(::xsd::cxx::xml::dom::create_attribute("reason", e));

		a << *i.getReason();
	}

	// cid
	//
	if (i.getCid()) {
		::xercesc::DOMAttr &a(::xsd::cxx::xml::dom::create_attribute("cid", e));

		a << *i.getCid();
	}
}

void operator<<(::xercesc::DOMElement &e, const Name &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);

	// lang
	//
	if (i.getLang()) {
		::xercesc::DOMAttr &a(
		    ::xsd::cxx::xml::dom::create_attribute("lang", "http://www.w3.org/XML/1998/namespace", e));

		a << *i.getLang();
	}
}

void operator<<(::xercesc::DOMElement &e, const State &i) {
	e << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);
}

void operator<<(::xercesc::DOMAttr &a, const State &i) {
	a << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);
}

void operator<<(::LinphonePrivate::Xsd::XmlSchema::ListStream &l, const State &i) {
	l << static_cast<const ::LinphonePrivate::Xsd::XmlSchema::String &>(i);
}
} // namespace Rlmi
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
