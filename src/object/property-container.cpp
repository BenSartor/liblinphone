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

#include <unordered_map>

#include "bctoolbox/utils.hh"
#include "property-container.h"

// =============================================================================

using namespace std;

LINPHONE_BEGIN_NAMESPACE

// -----------------------------------------------------------------------------

class PropertyContainerPrivate {
public:
	unordered_map<string, Variant> properties;
};

// -----------------------------------------------------------------------------

PropertyContainer::PropertyContainer() : mPrivate(nullptr) {
}

/*
 * Empty copy constructor. Don't change this pattern.
 * PropertyContainer is an Entity component, not a simple structure.
 * An Entity is UNIQUE.
 */
PropertyContainer::PropertyContainer(const PropertyContainer &) : mPrivate(nullptr) {
}

PropertyContainer::~PropertyContainer() {
	delete mPrivate;
}

PropertyContainer &PropertyContainer::operator=(const PropertyContainer &) {
	return *this;
}

const Variant &PropertyContainer::getProperty(const string &name) const {
	if (!mPrivate) return bctoolbox::Utils::getEmptyConstRefObject<Variant>();
	auto &properties = mPrivate->properties;
	auto it = properties.find(name);
	return it == properties.cend() ? bctoolbox::Utils::getEmptyConstRefObject<Variant>() : it->second;
}

void PropertyContainer::setProperty(const string &name, const Variant &value) {
	if (!mPrivate) mPrivate = new PropertyContainerPrivate();
	mPrivate->properties[name] = value;
}

void PropertyContainer::setProperty(const string &name, Variant &&value) {
	if (!mPrivate) mPrivate = new PropertyContainerPrivate();
	mPrivate->properties[name] = std::move(value);
}

int PropertyContainer::remove(const std::string &name) const {
	if (mPrivate) {
		auto it = mPrivate->properties.find(name);
		if (it == mPrivate->properties.end()) return -1;
		mPrivate->properties.erase(it);
		return 0;
	}
	return -2;
}

void PropertyContainer::clear() {
	if (mPrivate) {
		mPrivate->properties.clear();
	}
}

bool PropertyContainer::hasKey(const std::string &name) const {
	if (mPrivate) {
		return mPrivate->properties.find(name) != mPrivate->properties.end();
	}
	return false;
}

LINPHONE_END_NAMESPACE
