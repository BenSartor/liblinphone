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

#include "push-notification-message.h"

using namespace std;

LINPHONE_BEGIN_NAMESPACE

PushNotificationMessage::PushNotificationMessage(const std::string &callId,
                                                 bool isText,
                                                 const std::string &textContent,
                                                 const std::string &subject,
                                                 const std::string &fromAddr,
                                                 const std::string &localAddr,
                                                 const std::string &peerAddr,
                                                 bool isIcalendar,
                                                 bool isConferenceInvitationNew,
                                                 bool isConferenceInvitationUpdate,
                                                 bool isConferenceInvitationCancellation) {
	PushNotificationMessage::init(callId, isText, textContent, subject, fromAddr, localAddr, peerAddr, isIcalendar,
	                              isConferenceInvitationNew, isConferenceInvitationUpdate,
	                              isConferenceInvitationCancellation);
}

void PushNotificationMessage::init(const std::string &callId,
                                   bool isText,
                                   const std::string &textContent,
                                   const std::string &subject,
                                   const std::string &fromAddr,
                                   const std::string &localAddr,
                                   const std::string &peerAddr,
                                   bool isIcalendar,
                                   bool isConferenceInvitationNew,
                                   bool isConferenceInvitationUpdate,
                                   bool isConferenceInvitationCancellation) {
	mCallId = callId;
	mIsText = isText;
	mTextContent = textContent;
	mSubject = subject;
	mFromAddr = fromAddr;
	mLocalAddr = localAddr;
	mPeerAddr = peerAddr;
	mIsIcalendar = isIcalendar;
	mIsConferenceInvitationNew = isConferenceInvitationNew;
	mIsConferenceInvitationUpdate = isConferenceInvitationUpdate;
	mIsConferenceInvitationCancellation = isConferenceInvitationCancellation;
}

const std::string &PushNotificationMessage::getCallId() const {
	return mCallId;
}

bool PushNotificationMessage::isText() const {
	return mIsText;
}

const string &PushNotificationMessage::getTextContent() const {
	return mTextContent;
}

const string &PushNotificationMessage::getSubject() const {
	return mSubject;
}

shared_ptr<Address> PushNotificationMessage::getFromAddr() const {
	return make_shared<Address>(mFromAddr);
}
shared_ptr<Address> PushNotificationMessage::getLocalAddr() const {
	return make_shared<Address>(mLocalAddr);
}

shared_ptr<Address> PushNotificationMessage::getPeerAddr() const {
	return make_shared<Address>(mPeerAddr);
}

bool PushNotificationMessage::isIcalendar() const {
	return mIsIcalendar;
}

bool PushNotificationMessage::isConferenceInvitationNew() const {
	return mIsConferenceInvitationNew;
}

bool PushNotificationMessage::isConferenceInvitationUpdate() const {
	return mIsConferenceInvitationUpdate;
}

bool PushNotificationMessage::isConferenceInvitationCancellation() const {
	return mIsConferenceInvitationCancellation;
}

std::string PushNotificationMessage::toString() const {
	std::ostringstream ss;

	ss << "callId[" << mCallId << "] ";
	ss << "isText[" << mIsText << "] ";
	ss << "text[" << mTextContent << "] ";
	ss << "subject[" << mSubject << "] ";
	ss << "fromAddr[" << mFromAddr << "] ";
	ss << "localAddr[" << mLocalAddr << "] ";
	ss << "peerAddr[" << mPeerAddr << "] ";
	ss << "isIcalendar[" << mIsIcalendar << "] ";
	ss << "isConferenceInvitationUpdate[" << mIsConferenceInvitationUpdate << "] ";
	ss << "isConferenceInvitationCancellation[" << mIsConferenceInvitationCancellation << "] ";

	return ss.str();
}

LINPHONE_END_NAMESPACE
