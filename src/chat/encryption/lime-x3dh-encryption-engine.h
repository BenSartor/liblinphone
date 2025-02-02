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

#ifndef _L_LIME_X3DH_ENCRYPTION_ENGINE_H_
#define _L_LIME_X3DH_ENCRYPTION_ENGINE_H_

#include "belle-sip/belle-sip.h"
#include "belle-sip/http-listener.h"
#include "carddav.h"
#include "core/core-listener.h"
#include "encryption-engine.h"
#include "lime-x3dh-server-engine.h"
#include "lime/lime.hpp"

// =============================================================================

LINPHONE_BEGIN_NAMESPACE

inline std::string encodeBase64(const std::vector<uint8_t> &input) {
	const unsigned char *inputBuffer = input.data();
	size_t inputLength = input.size();
	size_t encodedLength = 0;
	bctbx_base64_encode(NULL, &encodedLength, inputBuffer, inputLength); // set encodedLength to the correct value
	unsigned char *encodedBuffer = new unsigned char[encodedLength];     // allocate encoded buffer with correct length
	bctbx_base64_encode(encodedBuffer, &encodedLength, inputBuffer, inputLength); // real encoding
	std::string output((char *)encodedBuffer);
	delete[] encodedBuffer;
	return output;
}

inline std::vector<uint8_t> decodeBase64(const std::string &input) {
	const unsigned char *inputBuffer = (const unsigned char *)input.data();
	size_t inputLength = input.size();
	size_t decodedLength = 0;
	bctbx_base64_decode(NULL, &decodedLength, inputBuffer, inputLength); // set decodedLength to the correct value
	unsigned char *decodedBuffer = new unsigned char[decodedLength];     // allocate decoded buffer with correct length
	bctbx_base64_decode(decodedBuffer, &decodedLength, inputBuffer, inputLength); // real decoding
	std::vector<uint8_t> output(decodedBuffer, decodedBuffer + decodedLength);
	delete[] decodedBuffer;
	return output;
}

class LimeManager : public lime::LimeManager {
public:
	LimeManager(const std::string &db_access,
	            belle_http_provider_t *prov,
	            std::shared_ptr<Core> core); // LinphoneCore *lc

private:
	static void processIoError(void *data, const belle_sip_io_error_event_t *event) noexcept;
	static void processResponse(void *data, const belle_http_response_event_t *event) noexcept;
	static void processAuthRequested(void *data, belle_sip_auth_event_t *event) noexcept;
};

class LimeX3dhEncryptionEngine : public EncryptionEngine, public CoreListener, private LimeX3dhUtils {
public:
	LimeX3dhEncryptionEngine(const std::string &db_access,
	                         belle_http_provider_t *prov,
	                         const std::shared_ptr<Core> core);

	~LimeX3dhEncryptionEngine();

	std::shared_ptr<LimeManager> getLimeManager();
	lime::limeCallback setLimeCallback(std::string operation);
	lime::limeCallback setLimeUserCreationCallback(LinphoneCore *lc, const std::string localDeviceId);
	lime::CurveId getCurveId() const;

	// EncryptionEngine overrides

	ChatMessageModifier::Result processIncomingMessage(const std::shared_ptr<ChatMessage> &message,
	                                                   int &errorCode) override;

	ChatMessageModifier::Result processOutgoingMessage(const std::shared_ptr<ChatMessage> &message,
	                                                   int &errorCode) override;

	void generateFileTransferKey(const std::shared_ptr<AbstractChatRoom> &ChatRoom,
	                             const std::shared_ptr<ChatMessage> &message,
	                             FileTransferContent *fileTransferContent) override;

	int downloadingFile(const std::shared_ptr<ChatMessage> &message,
	                    size_t offset,
	                    const uint8_t *buffer,
	                    size_t size,
	                    uint8_t *decrypted_buffer,
	                    FileTransferContent *fileTransferContent) override;

	int uploadingFile(const std::shared_ptr<ChatMessage> &message,
	                  size_t offset,
	                  const uint8_t *buffer,
	                  size_t *size,
	                  uint8_t *encrypted_buffer,
	                  FileTransferContent *fileTransferContent) override;

	int cancelFileTransfer(FileTransferContent *fileTransferContent) override;

	void mutualAuthentication(MSZrtpContext *zrtpContext,
	                          const std::shared_ptr<SalMediaDescription> &localMediaDescription,
	                          const std::shared_ptr<SalMediaDescription> &remoteMediaDescription,
	                          LinphoneCallDir direction) override;

	void authenticationVerified(MSZrtpContext *zrtpContext,
	                            const std::shared_ptr<SalMediaDescription> &remoteMediaDescription,
	                            const char *peerDeviceId) override;

	void authenticationRejected(const char *peerDeviceId) override;

	void addSecurityEventInChatrooms(const std::shared_ptr<Address> &peerDeviceAddr,
	                                 ConferenceSecurityEvent::SecurityEventType securityEventType) override;

	std::shared_ptr<ConferenceSecurityEvent> onDeviceAdded(const std::shared_ptr<Address> &newDeviceAddr,
	                                                       std::shared_ptr<Participant> participant,
	                                                       const std::shared_ptr<AbstractChatRoom> &chatRoom,
	                                                       ChatRoom::SecurityLevel currentSecurityLevel) override;

	bool isEncryptionEnabledForFileTransfer(const std::shared_ptr<AbstractChatRoom> &ChatRoom) override;
	AbstractChatRoom::SecurityLevel getSecurityLevel(const std::string &deviceId) const override;
	AbstractChatRoom::SecurityLevel getSecurityLevel(const std::list<std::string> &deviceIds) const override;
	EncryptionEngine::EngineType getEngineType() override;
	std::list<EncryptionParameter> getEncryptionParameters() override;
	void cleanDb() override;

	// CoreListener overrides

	void onNetworkReachable(bool sipNetworkReachable, bool mediaNetworkReachable) override;

	void onRegistrationStateChanged(LinphoneProxyConfig *cfg,
	                                LinphoneRegistrationState state,
	                                const std::string &message) override;

	void onServerUrlChanged(const std::shared_ptr<Account> &account, const std::string &limeServerUrl) override;

	void staleSession(const std::string localDeviceId, const std::string peerDeviceId) override;

	void setTestForceDecryptionFailureFlag(bool flag) override;

private:
	void update(const std::string localDeviceId);
	std::shared_ptr<LimeManager> limeManager;
	std::string _dbAccess;
	lime::CurveId curve;
	bool forceFailure = false;
};

LINPHONE_END_NAMESPACE

#endif // _L_LIME_X3DH_ENCRYPTION_ENGINE_H_
