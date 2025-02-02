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

#ifndef _L_CONFERENCE_ID_H_
#define _L_CONFERENCE_ID_H_

#include "address/address.h"

// =============================================================================

LINPHONE_BEGIN_NAMESPACE

class LINPHONE_PUBLIC ConferenceId {
public:
	ConferenceId();
	ConferenceId(const std::shared_ptr<Address> &peerAddress, const std::shared_ptr<const Address> &localAddress);
	ConferenceId(const std::shared_ptr<Address> &peerAddress, const std::shared_ptr<Address> &localAddress);
	ConferenceId(const ConferenceId &other);

	virtual ~ConferenceId() = default;

	ConferenceId *clone() const {
		return new ConferenceId(*this);
	}

	ConferenceId &operator=(const ConferenceId &other);

	bool operator==(const ConferenceId &other) const;
	bool operator!=(const ConferenceId &other) const;

	bool operator<(const ConferenceId &other) const;

	const std::shared_ptr<Address> &getPeerAddress() const;
	const std::shared_ptr<Address> &getLocalAddress() const;

	void setPeerAddress(const std::shared_ptr<Address> &addr);
	void setLocalAddress(const std::shared_ptr<Address> &addr);

	bool isValid() const;

private:
	std::shared_ptr<Address> peerAddress;
	std::shared_ptr<Address> localAddress;
};

inline std::ostream &operator<<(std::ostream &os, const ConferenceId &conferenceId) {
	auto peerAddress =
	    (conferenceId.getPeerAddress()) ? conferenceId.getPeerAddress()->asStringUriOnly() : std::string("<undefined>");
	auto localAddress = (conferenceId.getLocalAddress()) ? conferenceId.getLocalAddress()->asStringUriOnly()
	                                                     : std::string("<undefined>");
	os << "ConferenceId(peer=" << peerAddress << ", local=" << localAddress << ")";
	return os;
}

LINPHONE_END_NAMESPACE

// Add map key support.
namespace std {
template <>
struct hash<LinphonePrivate::ConferenceId> {
	std::size_t operator()(const LinphonePrivate::ConferenceId &conferenceId) const {
		const auto peerAddress =
		    conferenceId.getPeerAddress() ? conferenceId.getPeerAddress()->toStringOrdered() : "sip:";
		const auto localAddress =
		    conferenceId.getLocalAddress() ? conferenceId.getLocalAddress()->toStringOrdered() : "sip:";
		return hash<string>()(peerAddress) ^ (hash<string>()(localAddress) << 1);
	}
};
} // namespace std

#endif // ifndef _L_CONFERENCE_ID_H_
