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

#include <bctoolbox/tester.h>

#include "c-wrapper/internal/c-tools.h"
#include "call/call.h"
#include "conference/conference-info.h"
#include "conference/params/media-session-params.h"
#include "conference/participant-device.h"
#include "conference/session/media-session.h"
#include "liblinphone_tester.h"
#include "mediastreamer2/msmire.h"
#include "sal/call-op.h"
#include "sal/sal_media_description.h"
#include "shared_tester_functions.h"
#include "tester_utils.h"

using namespace std;
#include <sstream>

using namespace LinphonePrivate;

static void check_ice_from_rtp(LinphoneCall *c1, LinphoneCall *c2, LinphoneStreamType stream_type) {
	MediaStream *ms;
	LinphoneCallStats *stats;
	switch (stream_type) {
		case LinphoneStreamTypeAudio:
			ms = linphone_call_get_stream(c1, LinphoneStreamTypeAudio);
			break;
		case LinphoneStreamTypeVideo:
			ms = linphone_call_get_stream(c1, LinphoneStreamTypeVideo);
			break;
		case LinphoneStreamTypeText:
			ms = linphone_call_get_stream(c1, LinphoneStreamTypeText);
			break;
		default:
			lError() << "Unknown stream type [" << linphone_stream_type_to_string(stream_type) << "]";
			BC_ASSERT_FALSE(stream_type >= LinphoneStreamTypeUnknown);
			return;
	}

	stats = linphone_call_get_audio_stats(c1);
	if (linphone_call_stats_get_ice_state(stats) == LinphoneIceStateHostConnection && media_stream_started(ms)) {
		struct sockaddr_storage remaddr;
		socklen_t remaddrlen = sizeof(remaddr);
		char ip[NI_MAXHOST] = {0};
		int port = 0;
		std::string expected_addr;
		AudioStream *astream;

		const LinphoneCallParams *cp1 = linphone_call_get_current_params(c1);
		const LinphoneCallParams *cp2 = linphone_call_get_current_params(c2);
		if (linphone_call_params_get_update_call_when_ice_completed(cp1) &&
		    linphone_call_params_get_update_call_when_ice_completed(cp2)) {
			memset(&remaddr, 0, remaddrlen);

			LinphonePrivate::SalCallOp *op = LinphonePrivate::Call::toCpp(c2)->getOp();
			const std::shared_ptr<SalMediaDescription> &result_desc = op->getFinalMediaDescription();
			const auto &result_stream = result_desc->getStreamIdx(0);
			if (result_stream != Utils::getEmptyConstRefObject<SalStreamDescription>()) {
				expected_addr = result_stream.getRtpAddress();
			}
			if (expected_addr.empty()) {
				expected_addr = result_desc->getConnectionAddress();
			}
			astream = (AudioStream *)linphone_call_get_stream(c1, LinphoneStreamTypeAudio);
			if ((expected_addr.find(':') == std::string::npos) &&
			    (astream->ms.sessions.rtp_session->rtp.gs.rem_addr.ss_family == AF_INET6)) {
				bctbx_sockaddr_ipv6_to_ipv4((struct sockaddr *)&astream->ms.sessions.rtp_session->rtp.gs.rem_addr,
				                            (struct sockaddr *)&remaddr, &remaddrlen);
			} else {
				memcpy(&remaddr, &astream->ms.sessions.rtp_session->rtp.gs.rem_addr,
				       astream->ms.sessions.rtp_session->rtp.gs.rem_addrlen);
			}
			bctbx_sockaddr_to_ip_address((struct sockaddr *)&remaddr, remaddrlen, ip, sizeof(ip), &port);

			BC_ASSERT_STRING_EQUAL(ip, expected_addr.c_str());
		}
	}
	linphone_call_stats_unref(stats);
}

bool_t check_ice(LinphoneCoreManager *caller, LinphoneCoreManager *callee, LinphoneIceState state) {
	LinphoneCall *c1, *c2;
	bool_t global_success = TRUE;
	bool_t audio_success = FALSE;
	bool_t video_success = FALSE;
	bool_t text_success = FALSE;
	bool_t audio_enabled, video_enabled, realtime_text_enabled;
	MSTimeSpec ts;

	c1 = linphone_core_get_call_by_remote_address2(caller->lc, callee->identity);
	c2 = linphone_core_get_call_by_remote_address2(callee->lc, caller->identity);

	BC_ASSERT_PTR_NOT_NULL(c1);
	BC_ASSERT_PTR_NOT_NULL(c2);
	if (!c1 || !c2) return FALSE;
	linphone_call_ref(c1);
	linphone_call_ref(c2);

	BC_ASSERT_EQUAL(linphone_call_params_video_enabled(linphone_call_get_current_params(c1)),
	                linphone_call_params_video_enabled(linphone_call_get_current_params(c2)), int, "%d");
	BC_ASSERT_EQUAL(linphone_call_params_realtime_text_enabled(linphone_call_get_current_params(c1)),
	                linphone_call_params_realtime_text_enabled(linphone_call_get_current_params(c2)), int, "%d");
	audio_enabled = linphone_call_params_audio_enabled(linphone_call_get_current_params(c1));
	video_enabled = linphone_call_params_video_enabled(linphone_call_get_current_params(c1));
	realtime_text_enabled = linphone_call_params_realtime_text_enabled(linphone_call_get_current_params(c1));
	if (audio_enabled) {
		liblinphone_tester_clock_start(&ts);
		LinphoneCallStats *stats1 = NULL;
		LinphoneCallStats *stats2 = NULL;
		do {
			if ((c1 != NULL) && (c2 != NULL)) {
				stats1 = linphone_call_get_audio_stats(c1);
				stats2 = linphone_call_get_audio_stats(c2);
				if (linphone_call_stats_get_ice_state(stats1) == state &&
				    linphone_call_stats_get_ice_state(stats2) == state) {
					audio_success = TRUE;
					check_ice_from_rtp(c1, c2, LinphoneStreamTypeAudio);
					check_ice_from_rtp(c2, c1, LinphoneStreamTypeAudio);
					break;
				}
				linphone_core_iterate(caller->lc);
				linphone_core_iterate(callee->lc);
				linphone_call_stats_unref(stats1);
				linphone_call_stats_unref(stats2);
				stats1 = stats2 = NULL;
			}
			ms_usleep(20000);
		} while (!liblinphone_tester_clock_elapsed(&ts, 10000));
		if (stats1) linphone_call_stats_unref(stats1);
		if (stats2) linphone_call_stats_unref(stats2);
	}

	if (video_enabled) {
		liblinphone_tester_clock_start(&ts);
		LinphoneCallStats *stats1 = NULL;
		LinphoneCallStats *stats2 = NULL;
		do {
			if ((c1 != NULL) && (c2 != NULL)) {
				stats1 = linphone_call_get_video_stats(c1);
				stats2 = linphone_call_get_video_stats(c2);
				if (linphone_call_stats_get_ice_state(stats1) == state &&
				    linphone_call_stats_get_ice_state(stats2) == state) {
					video_success = TRUE;
					check_ice_from_rtp(c1, c2, LinphoneStreamTypeVideo);
					check_ice_from_rtp(c2, c1, LinphoneStreamTypeVideo);
					break;
				}
				linphone_core_iterate(caller->lc);
				linphone_core_iterate(callee->lc);
				linphone_call_stats_unref(stats1);
				linphone_call_stats_unref(stats2);
				stats1 = stats2 = NULL;
			}
			ms_usleep(20000);
		} while (!liblinphone_tester_clock_elapsed(&ts, 10000));
		if (stats1) linphone_call_stats_unref(stats1);
		if (stats2) linphone_call_stats_unref(stats2);
	}

	if (realtime_text_enabled) {
		liblinphone_tester_clock_start(&ts);
		LinphoneCallStats *stats1 = NULL;
		LinphoneCallStats *stats2 = NULL;
		do {
			if ((c1 != NULL) && (c2 != NULL)) {
				stats1 = linphone_call_get_text_stats(c1);
				stats2 = linphone_call_get_text_stats(c2);
				if (linphone_call_stats_get_ice_state(stats1) == state &&
				    linphone_call_stats_get_ice_state(stats2) == state) {
					text_success = TRUE;
					check_ice_from_rtp(c1, c2, LinphoneStreamTypeText);
					check_ice_from_rtp(c2, c1, LinphoneStreamTypeText);
					break;
				}
				linphone_core_iterate(caller->lc);
				linphone_core_iterate(callee->lc);
				linphone_call_stats_unref(stats1);
				linphone_call_stats_unref(stats2);
				stats1 = stats2 = NULL;
			}
			ms_usleep(20000);
		} while (!liblinphone_tester_clock_elapsed(&ts, 10000));
		if (stats1) linphone_call_stats_unref(stats1);
		if (stats2) linphone_call_stats_unref(stats2);
	}
	linphone_call_unref(c1);
	linphone_call_unref(c2);
	if (audio_enabled) global_success = global_success && audio_success;
	if (video_enabled) global_success = global_success && video_success;
	if (realtime_text_enabled) global_success = global_success && text_success;
	return global_success;
}

bool_t check_ice_sdp(LinphoneCall *call) {
	SalMediaDescription *desc = _linphone_call_get_local_desc(call);
	belle_sdp_session_description_t *sdp = desc->toSdp();
	const char *value = belle_sdp_session_description_get_attribute_value(sdp, "ice-ufrag");
	if (value) return TRUE;
	return FALSE;
}

bool_t is_srtp_secured(LinphoneCall *call, LinphoneStreamType ctype) {
	SalStreamType type = SalOther;
	switch (ctype) {
		case LinphoneStreamTypeAudio:
			type = SalAudio;
			break;
		case LinphoneStreamTypeVideo:
			type = SalVideo;
			break;
		case LinphoneStreamTypeText:
			type = SalText;
			break;
		default:
			type = SalOther;
			break;
	}
	SalMediaDescription *desc = _linphone_call_get_result_desc(call);
	const SalStreamDescription &stream = desc->findBestStream(type);
	if (stream == Utils::getEmptyConstRefObject<SalStreamDescription>()) return FALSE;
	if (stream.hasSrtp()) {
		const auto &streamCryptos = stream.getCryptos();
		for (const auto &crypto : streamCryptos) {
			const auto &algo = crypto.algo;
			// return (!ms_crypto_suite_is_unencrypted(algo) && !ms_crypto_suite_is_unauthenticated(algo));
			return (!ms_crypto_suite_is_unencrypted(algo));
		}
	}
	return FALSE;
}

void check_media_stream(LinphoneCall *call, bool_t is_null) {

	LinphonePrivate::Call *c = LinphonePrivate::Call::toCpp(call);
	const auto &params = linphone_call_get_current_params(call);

	if (is_null || (params && !linphone_call_params_audio_enabled(params))) {
		BC_ASSERT_PTR_NULL(c->getMediaStream(LinphoneStreamTypeAudio));
	} else {
		BC_ASSERT_PTR_NOT_NULL(c->getMediaStream(LinphoneStreamTypeAudio));
	}

	if (is_null || (params && !linphone_call_params_video_enabled(params))) {
		BC_ASSERT_PTR_NULL(c->getMediaStream(LinphoneStreamTypeVideo));
	} else {
		BC_ASSERT_PTR_NOT_NULL(c->getMediaStream(LinphoneStreamTypeVideo));
	}

	if (is_null || (params && !linphone_call_params_realtime_text_enabled(params))) {
		BC_ASSERT_PTR_NULL(c->getMediaStream(LinphoneStreamTypeText));
	} else {
		BC_ASSERT_PTR_NOT_NULL(c->getMediaStream(LinphoneStreamTypeText));
	}
}

void check_result_desc_rtp_rtcp_ports(LinphoneCall *call, int rtp_port, int rtcp_port) {
	SalMediaDescription *desc = _linphone_call_get_result_desc(call);
	for (auto &stream : desc->streams) {
		BC_ASSERT_EQUAL(stream.rtp_port, rtp_port, int, "%d");
		BC_ASSERT_EQUAL(stream.rtcp_port, rtcp_port, int, "%d");
	}
}

void check_local_desc_stream(LinphoneCall *call) {
	const auto &desc = _linphone_call_get_local_desc(call);
	const auto &core = linphone_call_get_core(call);
	const auto &params = linphone_call_get_params(call);

	const auto &audioStream = desc->findBestStream(SalAudio);
	if (audioStream != Utils::getEmptyConstRefObject<SalStreamDescription>()) {
		const auto &streamDir = audioStream.getDirection();
		const auto &callParamsAudioDir = sal_dir_from_call_params_dir(linphone_call_params_get_audio_direction(params));
		if (params && linphone_call_params_audio_enabled(params)) {
			BC_ASSERT_EQUAL(streamDir, callParamsAudioDir, int, "%d");
		} else {
			BC_ASSERT_EQUAL(streamDir,
			                linphone_core_get_keep_stream_direction_for_rejected_stream(core) ? callParamsAudioDir
			                                                                                  : SalStreamInactive,
			                int, "%d");
			BC_ASSERT_EQUAL(audioStream.rtp_port, 0, int, "%d");
			BC_ASSERT_EQUAL(audioStream.rtcp_port, 0, int, "%d");
		}
	}

	const auto &videoStream = desc->findBestStream(SalVideo);
	if (videoStream != Utils::getEmptyConstRefObject<SalStreamDescription>()) {
		const auto &streamDir = videoStream.getDirection();
		const auto &callParamsVideoDir = sal_dir_from_call_params_dir(linphone_call_params_get_video_direction(params));
		if (params && linphone_call_params_video_enabled(params)) {
			BC_ASSERT_EQUAL(streamDir, callParamsVideoDir, int, "%d");
		} else {
			BC_ASSERT_EQUAL(streamDir,
			                linphone_core_get_keep_stream_direction_for_rejected_stream(core) ? callParamsVideoDir
			                                                                                  : SalStreamInactive,
			                int, "%d");
			BC_ASSERT_EQUAL(videoStream.rtp_port, 0, int, "%d");
			BC_ASSERT_EQUAL(videoStream.rtcp_port, 0, int, "%d");
		}
	}

	const auto &textStream = desc->findBestStream(SalText);
	if (textStream != Utils::getEmptyConstRefObject<SalStreamDescription>()) {
		const auto &streamDir = textStream.getDirection();
		if (params && linphone_call_params_realtime_text_enabled(params)) {
			BC_ASSERT_EQUAL(streamDir, SalStreamSendRecv, int, "%d");
		} else {
			BC_ASSERT_EQUAL(streamDir,
			                linphone_core_get_keep_stream_direction_for_rejected_stream(core) ? SalStreamSendRecv
			                                                                                  : SalStreamInactive,
			                int, "%d");
			BC_ASSERT_EQUAL(textStream.rtp_port, 0, int, "%d");
			BC_ASSERT_EQUAL(textStream.rtcp_port, 0, int, "%d");
		}
	}
}

void _linphone_call_check_nb_streams(const LinphoneCall *call,
                                     const int nb_audio_streams,
                                     const int nb_video_streams,
                                     const int nb_text_streams) {
	const SalMediaDescription *call_result_desc = _linphone_call_get_result_desc(call);
	BC_ASSERT_PTR_NOT_NULL(call_result_desc);
	if (call_result_desc) {
		BC_ASSERT_EQUAL((int)call_result_desc->getNbStreams(), nb_audio_streams + nb_video_streams + nb_text_streams,
		                int, "%i");
		BC_ASSERT_EQUAL((int)call_result_desc->nbStreamsOfType(SalAudio), nb_audio_streams, int, "%i");
		BC_ASSERT_EQUAL((int)call_result_desc->nbStreamsOfType(SalVideo), nb_video_streams, int, "%i");
		BC_ASSERT_EQUAL((int)call_result_desc->nbStreamsOfType(SalText), nb_text_streams, int, "%i");
	}
}

int _linphone_call_get_nb_audio_steams(const LinphoneCall *call) {
	const SalMediaDescription *call_result_desc = _linphone_call_get_result_desc(call);
	return (int)call_result_desc->nbStreamsOfType(SalAudio);
}

int _linphone_call_get_nb_video_steams(const LinphoneCall *call) {
	const SalMediaDescription *call_result_desc = _linphone_call_get_result_desc(call);
	return (int)call_result_desc->nbStreamsOfType(SalVideo);
}

int _linphone_call_get_nb_text_steams(const LinphoneCall *call) {
	const SalMediaDescription *call_result_desc = _linphone_call_get_result_desc(call);
	return (int)call_result_desc->nbStreamsOfType(SalText);
}

LinphoneConferenceLayout _linphone_participant_device_get_layout(const LinphoneParticipantDevice *participant_device) {
	const auto &session =
	    static_pointer_cast<MediaSession>(LinphonePrivate::ParticipantDevice::toCpp(participant_device)->getSession());
	if (session) {
		return (LinphoneConferenceLayout)session->getRemoteParams()->getConferenceVideoLayout();
	}
	return LinphoneConferenceLayoutGrid;
}

bool_t _linphone_participant_device_get_audio_enabled(const LinphoneParticipantDevice *participant_device) {
	const auto &session =
	    static_pointer_cast<MediaSession>(LinphonePrivate::ParticipantDevice::toCpp(participant_device)->getSession());
	if (session) {
		return (session->getCurrentParams()->audioEnabled()) ? TRUE : FALSE;
	}
	return FALSE;
}

bool_t _linphone_participant_device_get_video_enabled(const LinphoneParticipantDevice *participant_device) {
	const auto &session =
	    static_pointer_cast<MediaSession>(LinphonePrivate::ParticipantDevice::toCpp(participant_device)->getSession());
	if (session) {
		return (session->getCurrentParams()->videoEnabled()) ? TRUE : FALSE;
	}
	return FALSE;
}

bool_t _linphone_participant_device_get_real_time_text_enabled(const LinphoneParticipantDevice *participant_device) {
	const auto &session =
	    static_pointer_cast<MediaSession>(LinphonePrivate::ParticipantDevice::toCpp(participant_device)->getSession());
	if (session) {
		return (session->getCurrentParams()->realtimeTextEnabled()) ? TRUE : FALSE;
	}
	return FALSE;
}

void _linphone_call_check_nb_active_streams(const LinphoneCall *call,
                                            const int nb_audio_streams,
                                            const int nb_video_streams,
                                            const int nb_text_streams) {
	BC_ASSERT_EQUAL(Call::toCpp(call)->getMediaStreamsNb(LinphoneStreamTypeAudio), nb_audio_streams, int, "%d");
	BC_ASSERT_EQUAL(Call::toCpp(call)->getMediaStreamsNb(LinphoneStreamTypeVideo), nb_video_streams, int, "%d");
	BC_ASSERT_EQUAL(Call::toCpp(call)->getMediaStreamsNb(LinphoneStreamTypeText), nb_text_streams, int, "%d");
}

void check_video_conference(bctbx_list_t *lcs,
                            LinphoneCoreManager *lc1,
                            LinphoneCoreManager *lc2,
                            LinphoneConferenceLayout layout) {
	LinphoneCall *call1 = linphone_core_get_current_call(lc1->lc);
	LinphoneCall *call2 = linphone_core_get_current_call(lc2->lc);
	BC_ASSERT_PTR_NOT_NULL(call1);
	BC_ASSERT_PTR_NOT_NULL(call2);
	if (call1 && call2) {
		VideoStream *vstream1s = (VideoStream *)linphone_call_get_stream(call1, LinphoneStreamTypeVideo);
		BC_ASSERT_PTR_NOT_NULL(vstream1s);
		VideoStream *vstream2s = (VideoStream *)linphone_call_get_stream(call2, LinphoneStreamTypeVideo);
		BC_ASSERT_PTR_NOT_NULL(vstream2s);
		BC_ASSERT_TRUE(vstream1s && vstream1s->source && ms_filter_get_id(vstream1s->source) == MS_MIRE_ID);
		BC_ASSERT_TRUE(vstream2s && vstream2s->source && ms_filter_get_id(vstream2s->source) == MS_MIRE_ID);
		MSMireControl c1 = {{0, 5, 10, 15, 20, 25}};
		MSMireControl c2 = {{100, 120, 140, 160, 180, 200}};

		if (vstream1s && vstream1s->source && ms_filter_get_id(vstream1s->source) == MS_MIRE_ID) {
			ms_filter_call_method(vstream1s->source, MS_MIRE_SET_COLOR, &c1);
		}
		if (vstream2s && vstream2s->source && ms_filter_get_id(vstream2s->source) == MS_MIRE_ID) {
			ms_filter_call_method(vstream2s->source, MS_MIRE_SET_COLOR, &c2);
		}

		wait_for_list(lcs, NULL, 5, liblinphone_tester_sip_timeout);

		LinphoneConference *conference1 = linphone_call_get_conference(call1);
		BC_ASSERT_PTR_NOT_NULL(conference1);
		if (conference1) {
			int nb = (linphone_conference_get_participant_count(conference1) + 2);
			BC_ASSERT_EQUAL(Call::toCpp(call1)->getMediaStreamsNb(LinphoneStreamTypeVideo), nb, int, "%d");
		}

		LinphoneConference *conference2 = linphone_call_get_conference(call1);
		BC_ASSERT_PTR_NOT_NULL(conference2);
		if (conference2) {
			int nb = (linphone_conference_get_participant_count(conference2) + 2);
			BC_ASSERT_EQUAL(Call::toCpp(call2)->getMediaStreamsNb(LinphoneStreamTypeVideo), nb, int, "%d");
		}

		linphone_call_check_rtp_sessions(call1);
		linphone_call_check_rtp_sessions(call2);
		BC_ASSERT_TRUE(linphone_call_compare_video_color(call1, c2, MediaStreamRecvOnly, vstream2s->label));
		BC_ASSERT_TRUE(linphone_call_compare_video_color(call2, c1, MediaStreamRecvOnly, vstream1s->label));
		if (layout != LinphoneConferenceLayoutGrid) {
			BC_ASSERT_TRUE(linphone_call_compare_video_color(call2, c1, MediaStreamSendRecv, ""));
		}
	}
}

void check_video_conference_with_local_participant(bctbx_list_t *participants,
                                                   LinphoneConferenceLayout layout,
                                                   bool_t local_participant) {
	for (bctbx_list_t *it = participants; it; it = bctbx_list_next(it)) {
		LinphoneCoreManager *m = (LinphoneCoreManager *)bctbx_list_get_data(it);
		LinphoneCall *call = linphone_core_get_current_call(m->lc);
		BC_ASSERT_PTR_NOT_NULL(call);
		if (call) {
			const LinphoneCallParams *call_params = linphone_call_get_current_params(call);
			const bool_t video_enabled = linphone_call_params_video_enabled(call_params);

			int nb = (static_cast<int>((bctbx_list_size(participants) + (local_participant ? 2 : 1))));
			if (!video_enabled) {
				if (layout == LinphoneConferenceLayoutActiveSpeaker) {
					// Only thumbnail corresponding to the participant is going to be inactivated
					nb--;
				} else if (layout == LinphoneConferenceLayoutGrid) {
					// main and thumbnail corresponding to the participant are going to be inactivated
					nb -= 2;
				}
			}
			BC_ASSERT_EQUAL(Call::toCpp(call)->getMediaStreamsNb(LinphoneStreamTypeVideo), nb, int, "%d");
			if (video_enabled) {
				linphone_call_check_rtp_sessions(call);
			}
		}
	}
}

void _linphone_conference_video_change(bctbx_list_t *lcs,
                                       LinphoneCoreManager *mgr1,
                                       LinphoneCoreManager *mgr2,
                                       LinphoneCoreManager *mgr3) {
	MSMireControl c1 = {{0, 5, 10, 15, 20, 25}};
	MSMireControl c3 = {{100, 120, 140, 160, 180, 200}};

	for (LinphoneCoreManager *mgr : {mgr1, mgr2, mgr3}) {
		LinphoneCall *call = linphone_core_get_current_call(mgr->lc);
		BC_ASSERT_PTR_NOT_NULL(call);
		if (!call) return;
		VideoStream *vstream = (VideoStream *)linphone_call_get_stream(call, LinphoneStreamTypeVideo);
		if (mgr != mgr2) { // mgr2 is audio only
			BC_ASSERT_TRUE(vstream && vstream->source && ms_filter_get_id(vstream->source) == MS_MIRE_ID);
			if (vstream && vstream->source && ms_filter_get_id(vstream->source) == MS_MIRE_ID) {
				if (mgr == mgr1) ms_filter_call_method(vstream->source, MS_MIRE_SET_COLOR, &c1);
				else ms_filter_call_method(vstream->source, MS_MIRE_SET_COLOR, &c3);
			}
		} else {
			BC_ASSERT_PTR_NULL(vstream);
		}
	}

	LinphoneCall *call1 = linphone_core_get_current_call(mgr1->lc);
	LinphoneConference *confMgr1 = linphone_call_get_conference(call1);
	LinphoneConference *confMgr3 = linphone_call_get_conference(linphone_core_get_current_call(mgr3->lc));

	// mgr3 speaks and mgr1's video change
	linphone_core_enable_mic(mgr1->lc, FALSE);
	linphone_core_enable_mic(mgr2->lc, FALSE);
	lInfo() << __func__ << ": mgr3 speaks";
	wait_for_list(lcs, NULL, 0, 5000);
	BC_ASSERT_TRUE(linphone_call_compare_video_color(call1, c3, MediaStreamSendRecv, ""));

	// mgr1 should see mgr3 as active speaker
	LinphoneParticipantDevice *device = linphone_conference_get_active_speaker_participant_device(confMgr1);
	if (BC_ASSERT_PTR_NOT_NULL(device)) {
		const LinphoneAddress *addrMgr1 = linphone_participant_device_get_address(device);

		LinphoneParticipant *participant = linphone_conference_get_me(confMgr3);
		bctbx_list_t *devices = linphone_participant_get_devices(participant);
		const LinphoneAddress *addrMgr3 =
		    linphone_participant_device_get_address((LinphoneParticipantDevice *)devices->data);

		BC_ASSERT_TRUE(linphone_address_equal(addrMgr1, addrMgr3));

		bctbx_list_free_with_data(devices, (bctbx_list_free_func)linphone_participant_device_unref);
	}

	// mgr2 does not see any active speaker as it has no video
	LinphoneConference *confMgr2 = linphone_call_get_conference(linphone_core_get_current_call(mgr2->lc));
	device = linphone_conference_get_active_speaker_participant_device(confMgr2);
	BC_ASSERT_PTR_NULL(device);

	// mgr2 speaks until mgr1's video change
	linphone_core_enable_mic(mgr2->lc, TRUE);
	linphone_core_enable_mic(mgr3->lc, FALSE);
	lInfo() << __func__ << ": mgr2 speaks";
	/* mg2 is audio-only, so this shall not affect video on other participants */
	wait_for_list(lcs, NULL, 0, 5000);
	BC_ASSERT_FALSE(linphone_call_compare_video_color(call1, c3, MediaStreamSendRecv, ""));
	BC_ASSERT_FALSE(linphone_call_compare_video_color(call1, c1, MediaStreamSendRecv, ""));

	// mgr1 should see mgr2 as active speaker even though it has no video as it's speaking
	device = linphone_conference_get_active_speaker_participant_device(confMgr1);
	if (BC_ASSERT_PTR_NOT_NULL(device)) {
		const LinphoneAddress *addrMgr1 = linphone_participant_device_get_address(device);

		LinphoneParticipant *participant = linphone_conference_get_me(confMgr2);
		bctbx_list_t *devices = linphone_participant_get_devices(participant);
		const LinphoneAddress *addrMgr2 =
		    linphone_participant_device_get_address((LinphoneParticipantDevice *)devices->data);

		BC_ASSERT_TRUE(linphone_address_equal(addrMgr1, addrMgr2));

		bctbx_list_free_with_data(devices, (bctbx_list_free_func)linphone_participant_device_unref);
	}

	// mgr1 speaks and mgr1's video not change
	lInfo() << __func__ << ": mgr1 speaks";
	linphone_core_enable_mic(mgr2->lc, FALSE);
	linphone_core_enable_mic(mgr1->lc, TRUE);
	wait_for_list(lcs, NULL, 0, 5000);
	BC_ASSERT_TRUE(linphone_call_compare_video_color(call1, c3, MediaStreamSendRecv, ""));
	BC_ASSERT_FALSE(linphone_call_compare_video_color(call1, c1, MediaStreamSendRecv, ""));

	// mgr3 should see mgr1 as active speaker
	device = linphone_conference_get_active_speaker_participant_device(confMgr3);
	if (BC_ASSERT_PTR_NOT_NULL(device)) {
		const LinphoneAddress *addrMgr3 = linphone_participant_device_get_address(device);

		LinphoneParticipant *participant = linphone_conference_get_me(confMgr1);
		bctbx_list_t *devices = linphone_participant_get_devices(participant);
		const LinphoneAddress *addrMgr1 =
		    linphone_participant_device_get_address((LinphoneParticipantDevice *)devices->data);

		BC_ASSERT_TRUE(linphone_address_equal(addrMgr3, addrMgr1));

		bctbx_list_free_with_data(devices, (bctbx_list_free_func)linphone_participant_device_unref);
	}
}

const char *_linphone_call_get_subject(LinphoneCall *call) {
	SalCallOp *op = Call::toCpp(call)->getOp();
	return L_STRING_TO_C(op->getSubject());
}

static std::string get_ice_default_candidate(LinphoneCoreManager *m) {
	std::string rtpAddress;
	rtpAddress = _linphone_call_get_local_desc(linphone_core_get_current_call(m->lc))->getStreamIdx(0).getRtpAddress();
	if (!rtpAddress.empty()) {
		return rtpAddress;
	} else {
		std::string cAddress =
		    _linphone_call_get_local_desc(linphone_core_get_current_call(m->lc))->getConnectionAddress();
		return cAddress;
	}
}

static bool address_in_list(const std::string &ip, const bctbx_list_t *addresses) {
	if (ip.empty()) return FALSE;
	for (; addresses != NULL; addresses = addresses->next) {
		if (ip.compare((const char *)addresses->data) == 0) return true;
	}
	return false;
}

static void check_expected_candidate_type(LinphoneCoreManager *m,
                                          TesterIceCandidateType expected_type,
                                          const bctbx_list_t *local_addresses) {
	std::string ip = get_ice_default_candidate(m);
	const struct addrinfo *ai = linphone_core_get_stun_server_addrinfo(m->lc);
	std::string relayIP;

	// bctbx_message("default-candidate=%s", ip.c_str());
	if (ai) {
		char rawip[64] = {0};
		bctbx_addrinfo_to_ip_address(ai, rawip, sizeof(rawip), NULL);
		relayIP = rawip;
	}
	switch (expected_type) {
		case TesterIceCandidateHost:
			BC_ASSERT_TRUE(address_in_list(ip, local_addresses));
			break;
		case TesterIceCandidateSflrx:
			BC_ASSERT_FALSE(address_in_list(ip, local_addresses));
			BC_ASSERT_TRUE(ip != relayIP);
			break;
		case TesterIceCandidateRelay:
			BC_ASSERT_TRUE(ip == relayIP);
			break;
	}
}

void liblinphone_tester_check_ice_default_candidates(LinphoneCoreManager *marie,
                                                     TesterIceCandidateType marie_expected_type,
                                                     LinphoneCoreManager *pauline,
                                                     TesterIceCandidateType pauline_expected_type) {
	bctbx_list_t *local_addresses = linphone_fetch_local_addresses();

	check_expected_candidate_type(marie, marie_expected_type, local_addresses);
	check_expected_candidate_type(pauline, pauline_expected_type, local_addresses);
	bctbx_list_free_with_data(local_addresses, bctbx_free);
}

int liblinphone_tester_send_data(const void *buffer, size_t length, const char *dest_ip, int dest_port, int sock_type) {
	struct addrinfo hints;
	struct addrinfo *res = NULL;
	int err = 0;
	std::ostringstream service;
	bctbx_socket_t sock;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = sock_type;
	service << dest_port;

	err = bctbx_getaddrinfo(dest_ip, service.str().c_str(), &hints, &res);
	if (err != 0) {
		bctbx_error("liblinphone_tester_send_data: bctbx_getaddrinfo() failed: %s", gai_strerror(err));
		return -1;
	}
	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock == (bctbx_socket_t)-1) {
		bctbx_error("liblinphone_tester_send_data: socket creation failed.");
		return -1;
	}
	err = bctbx_connect(sock, (struct sockaddr *)res->ai_addr, (socklen_t)res->ai_addrlen);
	if (err != 0) {
		bctbx_error("liblinphone_tester_send_data: connection failed: %s", getSocketError());
		goto end;
	}
	err = bctbx_send(sock, buffer, length, 0);
	if (err == -1) {
		bctbx_error("liblinphone_tester_send_data: send() failed: %s", getSocketError());
		goto end;
	}
end:
	bctbx_freeaddrinfo(res);
	bctbx_socket_close(sock);
	return err;
}

bool_t linphone_conference_type_is_full_state(const char *text) {
	std::string data(text);
	return ((data.find("state=\"full\"") != std::string::npos) &&
	        (data.find("state=\"partial\"") == std::string::npos) &&
	        (data.find("state=\"deleted\"") == std::string::npos))
	           ? TRUE
	           : FALSE;
}

void linphone_conference_info_check_participant(const LinphoneConferenceInfo *conference_info,
                                                LinphoneAddress *address,
                                                int sequence_number) {
	const auto &sequence = LinphonePrivate::ConferenceInfo::toCpp(conference_info)
	                           ->getParticipantParam(Address::toCpp(address)->getSharedFromThis(), "X-SEQ");
	BC_ASSERT_TRUE(!sequence.empty());
	if (!sequence.empty()) {
		const int sequenceNumber = std::atoi(sequence.c_str());
		BC_ASSERT_EQUAL(sequenceNumber, sequence_number, int, "%d");
	}
}

void linphone_conference_info_check_organizer(const LinphoneConferenceInfo *conference_info, int sequence_number) {
	const auto &sequence = LinphonePrivate::ConferenceInfo::toCpp(conference_info)->getOrganizerParam("X-SEQ");
	BC_ASSERT_TRUE(!sequence.empty());
	if (!sequence.empty()) {
		const int sequenceNumber = std::atoi(sequence.c_str());
		BC_ASSERT_EQUAL(sequenceNumber, sequence_number, int, "%d");
	}
}
