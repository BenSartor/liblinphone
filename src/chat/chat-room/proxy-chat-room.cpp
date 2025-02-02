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

#include <bctoolbox/defs.h>

#include "basic-to-client-group-chat-room.h"
#include "c-wrapper/c-wrapper.h"
#include "chat-room-p.h"
#include "proxy-chat-room-p.h"

// =============================================================================

using namespace std;

LINPHONE_BEGIN_NAMESPACE

void ProxyChatRoomPrivate::setupProxy() {
	L_Q();
	static_pointer_cast<ChatRoom>(chatRoom)->getPrivate()->setProxyChatRoom(q);
}

void ProxyChatRoomPrivate::teardownProxy() {
	static_pointer_cast<ChatRoom>(chatRoom)->getPrivate()->setProxyChatRoom(nullptr);
}

// -----------------------------------------------------------------------------

ProxyChatRoom::ProxyChatRoom(ProxyChatRoomPrivate &p, const shared_ptr<ChatRoom> &chatRoom)
    : AbstractChatRoom(p, chatRoom->getCore()) {
	L_D();

	d->chatRoom = chatRoom;
	d->setupProxy();
}

// -----------------------------------------------------------------------------

std::shared_ptr<Conference> ProxyChatRoom::getConference() const {
	L_D();
	return d->chatRoom->getConference();
}

const ConferenceId &ProxyChatRoom::getConferenceId() const {
	L_D();
	return d->chatRoom->getConferenceId();
}

const std::shared_ptr<Address> &ProxyChatRoom::getPeerAddress() const {
	L_D();
	return d->chatRoom->getPeerAddress();
}

const std::shared_ptr<Address> &ProxyChatRoom::getLocalAddress() const {
	L_D();
	return d->chatRoom->getLocalAddress();
}

// -----------------------------------------------------------------------------

time_t ProxyChatRoom::getCreationTime() const {
	L_D();
	return d->chatRoom->getCreationTime();
}

time_t ProxyChatRoom::getLastUpdateTime() const {
	L_D();
	return d->chatRoom->getLastUpdateTime();
}

// -----------------------------------------------------------------------------

ProxyChatRoom::CapabilitiesMask ProxyChatRoom::getCapabilities() const {
	L_D();
	return d->chatRoom->getCapabilities() | ProxyChatRoom::Capabilities::Proxy;
}

ProxyChatRoom::SecurityLevel ProxyChatRoom::getSecurityLevel() const {
	L_D();
	return d->chatRoom->getSecurityLevel();
}

ConferenceInterface::State ProxyChatRoom::getState() const {
	L_D();
	return d->chatRoom->getState();
}

bool ProxyChatRoom::hasBeenLeft() const {
	L_D();
	return d->chatRoom->hasBeenLeft();
}

bool ProxyChatRoom::isReadOnly() const {
	L_D();
	return d->chatRoom->isReadOnly();
}

// -----------------------------------------------------------------------------

list<shared_ptr<EventLog>> ProxyChatRoom::getMessageHistory(int nLast) const {
	L_D();
	return d->chatRoom->getMessageHistory(nLast);
}

list<shared_ptr<EventLog>> ProxyChatRoom::getMessageHistoryRange(int begin, int end) const {
	L_D();
	return d->chatRoom->getMessageHistoryRange(begin, end);
}

list<shared_ptr<ChatMessage>> ProxyChatRoom::getUnreadChatMessages() const {
	L_D();
	return d->chatRoom->getUnreadChatMessages();
}

int ProxyChatRoom::getMessageHistorySize() const {
	L_D();
	return d->chatRoom->getMessageHistorySize();
}

list<shared_ptr<EventLog>> ProxyChatRoom::getHistory(int nLast) const {
	L_D();
	return d->chatRoom->getHistory(nLast);
}

list<shared_ptr<EventLog>> ProxyChatRoom::getHistoryRange(int begin, int end) const {
	L_D();
	return d->chatRoom->getHistoryRange(begin, end);
}

int ProxyChatRoom::getHistorySize() const {
	L_D();
	return d->chatRoom->getHistorySize();
}

void ProxyChatRoom::deleteFromDb() {
	L_D();
	d->chatRoom->deleteFromDb();
}

void ProxyChatRoom::deleteHistory() {
	L_D();
	d->chatRoom->deleteHistory();
}

void ProxyChatRoom::deleteMessageFromHistory(const shared_ptr<ChatMessage> &message) {
	L_D();
	d->chatRoom->deleteMessageFromHistory(message);
}

shared_ptr<ChatMessage> ProxyChatRoom::getLastChatMessageInHistory() const {
	L_D();
	return d->chatRoom->getLastChatMessageInHistory();
}

bool ProxyChatRoom::isEmpty() const {
	L_D();
	return d->chatRoom->isEmpty();
}

int ProxyChatRoom::getChatMessageCount() const {
	L_D();
	return d->chatRoom->getChatMessageCount();
}

int ProxyChatRoom::getUnreadChatMessageCount() const {
	L_D();
	return d->chatRoom->getUnreadChatMessageCount();
}

// -----------------------------------------------------------------------------

void ProxyChatRoom::compose() {
	L_D();
	return d->chatRoom->compose();
}

bool ProxyChatRoom::isRemoteComposing() const {
	L_D();
	return d->chatRoom->isRemoteComposing();
}

list<std::shared_ptr<Address>> ProxyChatRoom::getComposingAddresses() const {
	L_D();
	return d->chatRoom->getComposingAddresses();
}

// -----------------------------------------------------------------------------

shared_ptr<ChatMessage> ProxyChatRoom::createChatMessage() {
	L_D();
	return d->chatRoom->createChatMessage();
}

// Deprecated
shared_ptr<ChatMessage> ProxyChatRoom::createChatMessage(const string &text) {
	L_D();
	return d->chatRoom->createChatMessage(text);
}
shared_ptr<ChatMessage> ProxyChatRoom::createChatMessageFromUtf8(const string &text) {
	L_D();
	return d->chatRoom->createChatMessageFromUtf8(text);
}
shared_ptr<ChatMessage> ProxyChatRoom::createFileTransferMessage(FileContent *content) {
	L_D();
	return d->chatRoom->createFileTransferMessage(content);
}

shared_ptr<ChatMessage> ProxyChatRoom::createForwardMessage(const shared_ptr<ChatMessage> &msg) {
	L_D();
	return d->chatRoom->createForwardMessage(msg);
}

shared_ptr<ChatMessage> ProxyChatRoom::createReplyMessage(const shared_ptr<ChatMessage> &msg) {
	L_D();
	return d->chatRoom->createReplyMessage(msg);
}
// -----------------------------------------------------------------------------

list<shared_ptr<ChatMessage>> ProxyChatRoom::findChatMessages(const list<string> &messageIds) const {
	L_D();
	return d->chatRoom->findChatMessages(messageIds);
}

shared_ptr<ChatMessage> ProxyChatRoom::findChatMessage(const string &messageId) const {
	L_D();
	return d->chatRoom->findChatMessage(messageId);
}

shared_ptr<ChatMessage> ProxyChatRoom::findChatMessage(const string &messageId,
                                                       ChatMessage::Direction direction) const {
	L_D();
	return d->chatRoom->findChatMessage(messageId, direction);
}

void ProxyChatRoom::markAsRead() {
	L_D();
	d->chatRoom->markAsRead();
}

void ProxyChatRoom::enableEphemeral(bool ephem, bool updateDb) {
	L_D();
	d->chatRoom->enableEphemeral(ephem, updateDb);
}

bool ProxyChatRoom::ephemeralEnabled() const {
	L_D();
	return d->chatRoom->ephemeralEnabled();
}

void ProxyChatRoom::setEphemeralLifetime(long lifetime, bool updateDb) {
	L_D();
	d->chatRoom->setEphemeralLifetime(lifetime, updateDb);
}

long ProxyChatRoom::getEphemeralLifetime() const {
	L_D();
	return d->chatRoom->getEphemeralLifetime();
}

void ProxyChatRoom::setEphemeralMode(AbstractChatRoom::EphemeralMode mode, bool updateDb) {
	L_D();
	d->chatRoom->setEphemeralMode(mode, updateDb);
}

AbstractChatRoom::EphemeralMode ProxyChatRoom::getEphemeralMode() const {
	L_D();
	return d->chatRoom->getEphemeralMode();
}

bool ProxyChatRoom::ephemeralSupportedByAllParticipants() const {
	L_D();
	return d->chatRoom->ephemeralSupportedByAllParticipants();
}

// -----------------------------------------------------------------------------

const std::shared_ptr<Address> &ProxyChatRoom::getConferenceAddress() const {
	L_D();
	return d->chatRoom->getConferenceAddress();
}

// -----------------------------------------------------------------------------
void ProxyChatRoom::allowCpim(BCTBX_UNUSED(bool value)) {
}

void ProxyChatRoom::allowMultipart(BCTBX_UNUSED(bool value)) {
}

bool ProxyChatRoom::canHandleCpim() const {
	L_D();
	return d->chatRoom->canHandleCpim();
}

bool ProxyChatRoom::canHandleMultipart() const {
	L_D();
	return d->chatRoom->canHandleMultipart();
}

bool ProxyChatRoom::addParticipant(std::shared_ptr<Call> call) {
	L_D();
	return d->chatRoom->addParticipant(call);
}

bool ProxyChatRoom::addParticipant(const std::shared_ptr<Address> &participantAddress) {
	L_D();
	return d->chatRoom->addParticipant(participantAddress);
}

bool ProxyChatRoom::addParticipants(const list<std::shared_ptr<Address>> &addresses) {
	L_D();
	return d->chatRoom->addParticipants(addresses);
}

bool ProxyChatRoom::removeParticipant(const shared_ptr<Participant> &participant) {
	L_D();
	return d->chatRoom->removeParticipant(participant);
}

bool ProxyChatRoom::removeParticipants(const list<shared_ptr<Participant>> &participants) {
	L_D();
	return d->chatRoom->removeParticipants(participants);
}

shared_ptr<Participant> ProxyChatRoom::findParticipant(const std::shared_ptr<Address> &participantAddress) const {
	L_D();
	return d->chatRoom->findParticipant(participantAddress);
}

shared_ptr<Participant> ProxyChatRoom::getMe() const {
	L_D();
	return d->chatRoom->getMe();
}

int ProxyChatRoom::getParticipantCount() const {
	L_D();
	return d->chatRoom->getParticipantCount();
}

const list<shared_ptr<Participant>> &ProxyChatRoom::getParticipants() const {
	L_D();
	return d->chatRoom->getParticipants();
}

const list<shared_ptr<ParticipantDevice>> ProxyChatRoom::getParticipantDevices() const {
	L_D();
	return d->chatRoom->getParticipantDevices();
}

void ProxyChatRoom::setParticipantAdminStatus(const shared_ptr<Participant> &participant, bool isAdmin) {
	L_D();
	d->chatRoom->setParticipantAdminStatus(participant, isAdmin);
}

// -----------------------------------------------------------------------------

const string &ProxyChatRoom::getSubject() const {
	L_D();
	return d->chatRoom->getSubject();
}

void ProxyChatRoom::setSubject(const string &subject) {
	L_D();
	d->chatRoom->setSubject(subject);
}

// -----------------------------------------------------------------------------

void ProxyChatRoom::join() {
	L_D();
	d->chatRoom->join();
}

void ProxyChatRoom::join(const std::shared_ptr<Address> &participantAddress) {
	L_D();
	d->chatRoom->join(participantAddress);
}

void ProxyChatRoom::leave() {
	L_D();
	d->chatRoom->leave();
}

// -----------------------------------------------------------------------------

bool ProxyChatRoom::update(const ConferenceParamsInterface &newParameters) {
	L_D();
	return d->chatRoom->update(newParameters);
}

const shared_ptr<AbstractChatRoom> &ProxyChatRoom::getProxiedChatRoom() const {
	L_D();
	return d->chatRoom;
}

const std::shared_ptr<ChatRoomParams> &ProxyChatRoom::getCurrentParams() const {
	L_D();

	return d->chatRoom->getCurrentParams();
}

void ProxyChatRoom::setState(ConferenceInterface::State state) {
	L_D();
	d->chatRoom->setState(state);
}

bool ProxyChatRoom::canHandleParticipants() const {
	L_D();
	return d->chatRoom->canHandleParticipants();
}

uint32_t ProxyChatRoom::getChar() {
	L_D();
	return d->chatRoom->getChar();
}

std::shared_ptr<Call> ProxyChatRoom::getCall() const {
	L_D();
	return d->chatRoom->getCall();
}

LINPHONE_END_NAMESPACE
