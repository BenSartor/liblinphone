/*
 * Copyright (c) 2010-2023 Belledonne Communications SARL.
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

#ifndef _L_REMOTE_CONFERENCE_LIST_EVENT_HANDLER_H_
#define _L_REMOTE_CONFERENCE_LIST_EVENT_HANDLER_H_

#include <map>
#include <unordered_map>

#include "linphone/types.h"
#include "linphone/utils/general.h"

#include "conference/conference-id.h"
#include "core/core-accessor.h"
#include "core/core-listener.h"
#include "remote-conference-event-handler-base.h"

// =============================================================================

LINPHONE_BEGIN_NAMESPACE

class Address;
class Content;
class EventSubscribe;
class RemoteConferenceEventHandler;

class RemoteConferenceListEventHandler : public RemoteConferenceEventHandlerBase,
                                         public CoreAccessor,
                                         public CoreListener {
public:
	RemoteConferenceListEventHandler(const std::shared_ptr<Core> &core);
	~RemoteConferenceListEventHandler();

	void subscribe() override;
	void subscribe(const std::shared_ptr<Account> &account);
	void unsubscribe() override;
	void unsubscribe(const std::shared_ptr<Account> &account);
	void invalidateSubscription() override;
	void notifyReceived(std::string from, const Content *notifyContent);
	void addHandler(RemoteConferenceEventHandler *handler);
	void removeHandler(RemoteConferenceEventHandler *handler);
	void clearHandlers();
	RemoteConferenceEventHandler *findHandler(const ConferenceId &conferenceId) const;
	bool getInitialSubscriptionUnderWayFlag(const ConferenceId &conferenceId) const;

private:
	bool isHandlerInSameDomainAsCore(const ConferenceId &conferenceId) const;
	std::unordered_map<ConferenceId, RemoteConferenceEventHandler *> handlers;
	std::list<std::shared_ptr<EventSubscribe>> levs;

	std::map<std::string, std::shared_ptr<Address>> parseRlmi(const std::string &xmlBody) const;

	// CoreListener
	void onNetworkReachable(bool sipNetworkReachable, bool mediaNetworkReachable) override;
	void onRegistrationStateChanged(LinphoneProxyConfig *cfg,
	                                LinphoneRegistrationState state,
	                                const std::string &message) override;
	void onEnteringBackground() override;
	void onEnteringForeground() override;
};

LINPHONE_END_NAMESPACE

#endif // ifndef _L_REMOTE_CONFERENCE_LIST_EVENT_HANDLER_H_
