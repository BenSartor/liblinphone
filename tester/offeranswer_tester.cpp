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

#include "liblinphone_tester.h"
#include "linphone/core.h"
#include "linphone/lpconfig.h"
#include "linphone/utils/utils.h"
#include "sal/sal_media_description.h"
#include "sal/sal_stream_description.h"
#include "tester_utils.h"
#include <sys/stat.h>
#include <sys/types.h>

using namespace LinphonePrivate;

static int get_codec_position(const MSList *l, const char *mime_type, int rate) {
	const MSList *elem;
	int i;
	for (elem = l, i = 0; elem != NULL; elem = elem->next, i++) {
		PayloadType *pt = (PayloadType *)elem->data;
		if (strcasecmp(pt->mime_type, mime_type) == 0 && pt->clock_rate == rate) return i;
	}
	return -1;
}

/*check basic things about codecs at startup: order and enablement*/
static void start_with_no_config(void) {
	LinphoneCore *lc =
	    linphone_factory_create_core_3(linphone_factory_get(), NULL, liblinphone_tester_get_empty_rc(), system_context);
	const MSList *codecs = linphone_core_get_audio_codecs(lc);
	int opus_codec_pos;
	int speex_codec_pos = get_codec_position(codecs, "speex", 8000);
	int speex16_codec_pos = get_codec_position(codecs, "speex", 16000);
	PayloadType *pt;
	opus_codec_pos = get_codec_position(codecs, "opus", 48000);
	if (opus_codec_pos != -1) BC_ASSERT_EQUAL(opus_codec_pos, 0, int, "%d");
	BC_ASSERT_LOWER(speex16_codec_pos, speex_codec_pos, int, "%d");

	pt = linphone_core_find_payload_type(lc, "speex", 16000, 1);
	BC_ASSERT_PTR_NOT_NULL(pt);
	if (pt) {
		BC_ASSERT_TRUE(linphone_core_payload_type_enabled(lc, pt));
	}
	linphone_core_unref(lc);
}

static void check_payload_type_numbers(LinphoneCall *call1, LinphoneCall *call2, int expected_number) {
	const LinphoneCallParams *params = linphone_call_get_current_params(call1);
	if (!BC_ASSERT_PTR_NOT_NULL(params)) return;
	const PayloadType *pt = linphone_call_params_get_used_audio_codec(params);
	BC_ASSERT_PTR_NOT_NULL(pt);
	if (pt) {
		BC_ASSERT_EQUAL(payload_type_get_number(pt), expected_number, int, "%d");
	}
	params = linphone_call_get_current_params(call2);
	pt = linphone_call_params_get_used_audio_codec(params);
	BC_ASSERT_PTR_NOT_NULL(pt);
	if (pt) {
		BC_ASSERT_EQUAL(payload_type_get_number(pt), expected_number, int, "%d");
	}
}

static void simple_call_with_different_codec_mappings(void) {
	LinphoneCoreManager *marie;
	LinphoneCoreManager *pauline;
	LinphoneCall *pauline_call;

	marie = linphone_core_manager_new("marie_rc");
	pauline = linphone_core_manager_new("pauline_tcp_rc");

	disable_all_audio_codecs_except_one(marie->lc, "pcmu", -1);
	disable_all_audio_codecs_except_one(pauline->lc, "pcmu", -1);

	/*marie set a fantasy number to PCMU*/
	payload_type_set_number(linphone_core_find_payload_type(marie->lc, "PCMU", 8000, -1), 104);

	BC_ASSERT_TRUE(call(marie, pauline));
	pauline_call = linphone_core_get_current_call(pauline->lc);
	BC_ASSERT_PTR_NOT_NULL(pauline_call);
	if (pauline_call) {
		LinphoneCallParams *params;
		check_payload_type_numbers(linphone_core_get_current_call(marie->lc), pauline_call, 104);
		/*make a reinvite in the other direction*/
		linphone_call_update(pauline_call, params = linphone_core_create_call_params(pauline->lc, pauline_call));
		linphone_call_params_unref(params);
		BC_ASSERT_TRUE(wait_for(pauline->lc, marie->lc, &pauline->stat.number_of_LinphoneCallUpdating, 1));
		BC_ASSERT_TRUE(wait_for(pauline->lc, marie->lc, &marie->stat.number_of_LinphoneCallUpdatedByRemote, 1));
		BC_ASSERT_TRUE(wait_for(pauline->lc, marie->lc, &pauline->stat.number_of_LinphoneCallStreamsRunning, 2));
		BC_ASSERT_TRUE(wait_for(pauline->lc, marie->lc, &marie->stat.number_of_LinphoneCallStreamsRunning, 2));
		/*payload type numbers shall remain the same*/
		check_payload_type_numbers(linphone_core_get_current_call(marie->lc), pauline_call, 104);
	}

	end_call(marie, pauline);
	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void simple_call_with_fmtps(void) {
	LinphoneCoreManager *marie;
	LinphoneCoreManager *pauline;
	LinphoneCall *pauline_call;

	marie = linphone_core_manager_new("marie_rc");
	pauline = linphone_core_manager_new("pauline_tcp_rc");

	disable_all_audio_codecs_except_one(marie->lc, "pcmu", -1);
	disable_all_audio_codecs_except_one(pauline->lc, "pcmu", -1);

	/*marie set a fantasy fmtp to PCMU*/
	LinphonePayloadType *marie_pt = linphone_core_get_payload_type(marie->lc, "PCMU", 8000, -1);
	linphone_payload_type_set_recv_fmtp(marie_pt, "parles-plus-fort=1");
	linphone_payload_type_unref(marie_pt);

	BC_ASSERT_TRUE(call(marie, pauline));
	pauline_call = linphone_core_get_current_call(pauline->lc);
	BC_ASSERT_PTR_NOT_NULL(pauline_call);
	if (pauline_call) {
		LinphonePayloadType *pt =
		    linphone_call_params_get_used_audio_payload_type(linphone_call_get_current_params(pauline_call));
		BC_ASSERT_PTR_NOT_NULL(pt);
		if (pt) {
			BC_ASSERT_STRING_EQUAL(linphone_payload_type_get_send_fmtp(pt), "parles-plus-fort=1");
		}
		linphone_payload_type_unref(pt);
		pt = linphone_call_params_get_used_audio_payload_type(
		    linphone_call_get_current_params(linphone_core_get_current_call(marie->lc)));
		BC_ASSERT_PTR_NOT_NULL(pt);
		if (pt) {
			ms_message("send_fmtp=%s, recv_fmtp=%s", linphone_payload_type_get_send_fmtp(pt),
			           linphone_payload_type_get_recv_fmtp(pt));
			BC_ASSERT_STRING_EQUAL(linphone_payload_type_get_recv_fmtp(pt), "parles-plus-fort=1");
		}
		linphone_payload_type_unref(pt);
	}

	end_call(marie, pauline);
	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

#ifdef VIDEO_ENABLED
static void h264_call_with_fmtps(void) {
	LinphoneCoreManager *marie;
	LinphoneCoreManager *pauline;
	LinphoneCall *pauline_call;
	OrtpPayloadType *pt_1 = NULL, *pt_2 = NULL;
	marie = linphone_core_manager_new("marie_rc");
	pauline = linphone_core_manager_new("pauline_tcp_rc");
	bctbx_list_t *video_codecs = NULL;
	OrtpPayloadType *origin_h264_pt = NULL;

	linphone_core_set_video_device(marie->lc, "Mire: Mire (synthetic moving picture)");
	linphone_core_enable_video_capture(marie->lc, TRUE);
	linphone_core_enable_video_display(marie->lc, TRUE);
	linphone_core_enable_video_capture(pauline->lc, TRUE);
	linphone_core_enable_video_display(pauline->lc, TRUE);

	LinphoneVideoActivationPolicy *pol = linphone_factory_create_video_activation_policy(linphone_factory_get());
	linphone_video_activation_policy_set_automatically_accept(pol, TRUE);
	linphone_video_activation_policy_set_automatically_initiate(pol, TRUE);

	linphone_core_set_video_activation_policy(marie->lc, pol);
	linphone_core_set_video_activation_policy(pauline->lc, pol);
	linphone_video_activation_policy_unref(pol);

	LinphonePayloadType *marie_pt = linphone_core_get_payload_type(marie->lc, "h264", 90000, -1);
	if (!marie_pt) {
		ms_warning("H264 not available on this platform, skeeping");
		goto end;
	}
	linphone_payload_type_unref(marie_pt);

	disable_all_video_codecs_except_one(marie->lc, "H264");
	disable_all_video_codecs_except_one(pauline->lc, "H264");
	origin_h264_pt = linphone_core_find_payload_type(pauline->lc, "H264", 90000, -1);
	pt_1 = payload_type_clone(origin_h264_pt);
	pt_2 = payload_type_clone(pt_1);

	payload_type_set_recv_fmtp(pt_1, "profile-level-id=42801F; packetization-mode=0");
	payload_type_set_recv_fmtp(pt_2, "profile-level-id=42801F; packetization-mode=1");

	video_codecs = bctbx_list_copy(linphone_core_get_video_codecs(marie->lc));
	for (bctbx_list_t *it = video_codecs; it != NULL; it = bctbx_list_next(it)) {
		OrtpPayloadType *pt = (OrtpPayloadType *)bctbx_list_get_data(it);
		if (strcasecmp(payload_type_get_mime(pt), "H264") == 0) {
			video_codecs = bctbx_list_remove(video_codecs, pt);
			break; // one
		}
	}
	video_codecs = bctbx_list_append(video_codecs, pt_1);
	video_codecs = bctbx_list_append(video_codecs, pt_2);
	linphone_core_set_video_codecs(marie->lc, video_codecs);

	/*marie set packetization-mode=1*/
	// linphone_payload_type_set_recv_fmtp(linphone_core_get_payload_type(marie->lc, "h264", 90000, -1),
	// "profile-level-id=42801F ");

	BC_ASSERT_TRUE(call(marie, pauline));
	pauline_call = linphone_core_get_current_call(pauline->lc);
	BC_ASSERT_PTR_NOT_NULL(pauline_call);
	if (pauline_call) {
		LinphonePayloadType *pt =
		    linphone_call_params_get_used_video_payload_type(linphone_call_get_current_params(pauline_call));
		BC_ASSERT_PTR_NOT_NULL(pt);
		if (pt) {
			BC_ASSERT_PTR_NOT_NULL(strstr(linphone_payload_type_get_recv_fmtp(pt), "packetization-mode=1"));
		}
		linphone_payload_type_unref(pt);
		pt = linphone_call_params_get_used_video_payload_type(
		    linphone_call_get_current_params(linphone_core_get_current_call(marie->lc)));
		BC_ASSERT_PTR_NOT_NULL(pt);
		if (pt) {
			ms_message("send_fmtp=%s, recv_fmtp=%s", linphone_payload_type_get_send_fmtp(pt),
			           linphone_payload_type_get_recv_fmtp(pt));
			BC_ASSERT_PTR_NOT_NULL(strstr(linphone_payload_type_get_recv_fmtp(pt), "packetization-mode=1"));
			BC_ASSERT_PTR_NOT_NULL(strstr(linphone_payload_type_get_recv_fmtp(pt), "packetization-mode=1"));
		}
		linphone_payload_type_unref(pt);
	}

	end_call(marie, pauline);

end:
	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
	if (pt_1) payload_type_destroy(pt_1);
	if (pt_2) payload_type_destroy(pt_2);
}
static void enable_rtp_bundle(LinphoneCore *lc, bool_t enable) {

	if (enable == false) {
		linphone_config_set_bool(linphone_core_get_config(lc), "rtp", "accept_bundle", FALSE);
	}
	LinphoneAccount *account = linphone_core_get_default_account(lc);
	LinphoneAccountParams *account_params = linphone_account_params_clone(linphone_account_get_params(account));
	linphone_account_params_enable_rtp_bundle(account_params, enable);
	linphone_account_set_params(account, account_params);
	linphone_account_params_unref(account_params);
}
static void enable_video_stream(LinphoneCore *lc, LinphoneVideoActivationPolicy *policy) {

	linphone_core_set_video_device(lc, "Mire: Mire (synthetic moving picture)");
	linphone_core_enable_video_capture(lc, TRUE);
	linphone_core_enable_video_display(lc, TRUE);
	linphone_core_set_video_activation_policy(lc, policy);
}
typedef struct flexfec_scenarios_t {
	bool_t bundle_proposed;
	bool_t fec_proposed;
	bool_t bundle_accepted;
	bool_t fec_accepted;
} flexfec_scenarios;

static void _base_video_call_for_fec(flexfec_scenarios params) {
	LinphoneCoreManager *marie;
	LinphoneCoreManager *pauline;
	LinphoneCall *pauline_call;

	marie = linphone_core_manager_new("marie_rc");
	pauline = linphone_core_manager_new("pauline_rc");

	LinphoneVideoActivationPolicy *pol = linphone_factory_create_video_activation_policy(linphone_factory_get());
	linphone_video_activation_policy_set_automatically_accept(pol, TRUE);
	linphone_video_activation_policy_set_automatically_initiate(pol, TRUE);

	enable_rtp_bundle(marie->lc, params.bundle_proposed);
	enable_rtp_bundle(pauline->lc, params.bundle_accepted);

	linphone_core_enable_fec(marie->lc, params.fec_proposed);
	linphone_core_enable_fec(pauline->lc, params.fec_accepted);

	enable_video_stream(marie->lc, pol);
	enable_video_stream(pauline->lc, pol);

	linphone_video_activation_policy_unref(pol);

	BC_ASSERT_TRUE(call(marie, pauline));
	pauline_call = linphone_core_get_current_call(pauline->lc);
	if (pauline_call) {

		const LinphoneCallParams *call_params = linphone_call_get_current_params(pauline_call);
		bool_t bundle_enabled = linphone_call_params_rtp_bundle_enabled(call_params);

		if (params.bundle_proposed && params.bundle_accepted) BC_ASSERT_TRUE(bundle_enabled);
		else {
			BC_ASSERT_FALSE(bundle_enabled);
		}
	}

	end_call(marie, pauline);

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

void flexfec_test_both_proposed_both_accepted(void) {
	flexfec_scenarios params = {TRUE, TRUE, TRUE, TRUE};
	_base_video_call_for_fec(params);
}
void flexfec_test_both_proposed_bundle_accepted(void) {
	flexfec_scenarios params = {TRUE, TRUE, TRUE, FALSE};
	_base_video_call_for_fec(params);
}
void flexfec_test_both_proposed_nothing_accepted(void) {
	flexfec_scenarios params = {TRUE, TRUE, FALSE, FALSE};
	_base_video_call_for_fec(params);
}
void flexfec_test_both_poposed_only_fec_accepted(void) {
	flexfec_scenarios params = {TRUE, TRUE, FALSE, TRUE};
	_base_video_call_for_fec(params);
}
void flexfec_test_only_fec_proposed_and_accepted(void) {
	flexfec_scenarios params = {FALSE, TRUE, FALSE, TRUE};
	_base_video_call_for_fec(params);
}
static void h264_call_receiver_with_no_h264_support(void) {
	LinphoneCoreManager *marie;
	LinphoneCoreManager *pauline;
	LinphoneCall *pauline_call;
	marie = linphone_core_manager_new("marie_rc");
	pauline = linphone_core_manager_new("pauline_tcp_rc");

	linphone_core_set_video_device(marie->lc, "Mire: Mire (synthetic moving picture)");
	linphone_core_enable_video_capture(marie->lc, TRUE);
	linphone_core_enable_video_display(marie->lc, TRUE);
	linphone_core_enable_video_capture(pauline->lc, TRUE);
	linphone_core_enable_video_display(pauline->lc, TRUE);

	LinphoneVideoActivationPolicy *pol = linphone_factory_create_video_activation_policy(linphone_factory_get());
	linphone_video_activation_policy_set_automatically_accept(pol, TRUE);
	linphone_video_activation_policy_set_automatically_initiate(pol, TRUE);

	linphone_core_set_video_activation_policy(marie->lc, pol);
	linphone_core_set_video_activation_policy(pauline->lc, pol);
	linphone_video_activation_policy_unref(pol);

	LinphonePayloadType *marie_pt = linphone_core_get_payload_type(marie->lc, "h264", 90000, -1);
	if (!marie_pt) {
		ms_warning("H264 not available on this platform, skeeping");
		goto end;
	}
	linphone_payload_type_unref(marie_pt);

	disable_all_video_codecs_except_one(marie->lc, "H264");
	disable_all_video_codecs_except_one(pauline->lc, "VP8");

	BC_ASSERT_TRUE(call(marie, pauline));
	pauline_call = linphone_core_get_current_call(pauline->lc);
	BC_ASSERT_PTR_NOT_NULL(pauline_call);
	if (pauline_call) {
		LinphonePayloadType *pt =
		    linphone_call_params_get_used_video_payload_type(linphone_call_get_current_params(pauline_call));
		BC_ASSERT_PTR_NULL(pt);
		pt = linphone_call_params_get_used_video_payload_type(
		    linphone_call_get_current_params(linphone_core_get_current_call(marie->lc)));
		BC_ASSERT_PTR_NULL(pt);
	}

	end_call(marie, pauline);
end:
	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void h264_call_without_packetization_mode(void) {
	LinphoneCoreManager *marie;
	LinphoneCoreManager *pauline;
	LinphoneCall *pauline_call;
	OrtpPayloadType *pt_1 = NULL, *pt_2 = NULL;
	marie = linphone_core_manager_new("marie_rc");
	pauline = linphone_core_manager_new("pauline_tcp_rc");

	linphone_core_set_video_device(marie->lc, "Mire: Mire (synthetic moving picture)");
	linphone_core_enable_video_capture(marie->lc, TRUE);
	linphone_core_enable_video_display(marie->lc, TRUE);
	linphone_core_enable_video_capture(pauline->lc, TRUE);
	linphone_core_enable_video_display(pauline->lc, TRUE);

	LinphoneVideoActivationPolicy *pol = linphone_factory_create_video_activation_policy(linphone_factory_get());
	linphone_video_activation_policy_set_automatically_accept(pol, TRUE);
	linphone_video_activation_policy_set_automatically_initiate(pol, TRUE);

	linphone_core_set_video_activation_policy(marie->lc, pol);
	linphone_core_set_video_activation_policy(pauline->lc, pol);
	linphone_video_activation_policy_unref(pol);

	LinphonePayloadType *marie_pt = linphone_core_get_payload_type(marie->lc, "h264", 90000, -1);
	if (!marie_pt) {
		ms_warning("H264 not available on this platform, skeeping");
		goto end;
	}
	linphone_payload_type_unref(marie_pt);

	disable_all_video_codecs_except_one(marie->lc, "H264");
	disable_all_video_codecs_except_one(pauline->lc, "H264");

	BC_ASSERT_TRUE(call(marie, pauline));
	pauline_call = linphone_core_get_current_call(pauline->lc);
	BC_ASSERT_PTR_NOT_NULL(pauline_call);
	if (pauline_call) {
		LinphonePayloadType *pt =
		    linphone_call_params_get_used_video_payload_type(linphone_call_get_current_params(pauline_call));
		BC_ASSERT_PTR_NOT_NULL(pt);
		if (pt) {
			BC_ASSERT_PTR_NULL(strstr(linphone_payload_type_get_recv_fmtp(pt), "packetization-mode"));
		}
		linphone_payload_type_unref(pt);
		pt = linphone_call_params_get_used_video_payload_type(
		    linphone_call_get_current_params(linphone_core_get_current_call(marie->lc)));
		BC_ASSERT_PTR_NOT_NULL(pt);
		if (pt) {
			ms_message("send_fmtp=%s, recv_fmtp=%s", linphone_payload_type_get_send_fmtp(pt),
			           linphone_payload_type_get_recv_fmtp(pt));
			BC_ASSERT_PTR_NULL(strstr(linphone_payload_type_get_recv_fmtp(pt), "packetization-mode"));
			BC_ASSERT_PTR_NULL(strstr(linphone_payload_type_get_recv_fmtp(pt), "packetization-mode"));
		}
		linphone_payload_type_unref(pt);
	}

	end_call(marie, pauline);

end:
	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
	if (pt_1) payload_type_destroy(pt_1);
	if (pt_2) payload_type_destroy(pt_2);
}
#endif

static void call_failed_because_of_codecs(void) {
	LinphoneCoreManager *marie = linphone_core_manager_new("marie_rc");
	LinphoneCoreManager *pauline = linphone_core_manager_new("pauline_tcp_rc");
	LinphoneCall *out_call;

	disable_all_audio_codecs_except_one(marie->lc, "pcmu", -1);
	disable_all_audio_codecs_except_one(pauline->lc, "pcma", -1);
	out_call = linphone_core_invite_address(pauline->lc, marie->identity);
	linphone_call_ref(out_call);
	BC_ASSERT_TRUE(wait_for(pauline->lc, marie->lc, &pauline->stat.number_of_LinphoneCallOutgoingInit, 1));

	/*flexisip will retain the 488 until the "urgent reply" timeout (I.E 5s) arrives.*/
	BC_ASSERT_TRUE(wait_for_until(pauline->lc, marie->lc, &pauline->stat.number_of_LinphoneCallError, 1, 7000));
	BC_ASSERT_EQUAL(linphone_call_get_reason(out_call), LinphoneReasonNotAcceptable, int, "%d");
	BC_ASSERT_EQUAL(marie->stat.number_of_LinphoneCallIncomingReceived, 0, int, "%d");
	BC_ASSERT_EQUAL(marie->stat.number_of_LinphoneCallReleased, 0, int, "%d");

	linphone_call_unref(out_call);
	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void profile_call_base(bool_t avpf1,
                              LinphoneMediaEncryption srtp1,
                              bool_t avpf2,
                              LinphoneMediaEncryption srtp2,
                              bool_t encryption_mandatory,
                              const char *expected_profile,
                              bool_t enable_video) {
	LinphoneCoreManager *marie = linphone_core_manager_new("marie_rc");
	LinphoneCoreManager *pauline = linphone_core_manager_new("pauline_tcp_rc");
	LinphoneProxyConfig *lpc;
	const LinphoneCallParams *params;

	if (avpf1) {
		lpc = linphone_core_get_default_proxy_config(marie->lc);
		linphone_proxy_config_edit(lpc);
		linphone_proxy_config_set_avpf_mode(lpc, LinphoneAVPFEnabled);
		linphone_proxy_config_set_avpf_rr_interval(lpc, 3);
		linphone_proxy_config_done(lpc);
	}
	if (avpf2) {
		lpc = linphone_core_get_default_proxy_config(pauline->lc);
		linphone_proxy_config_edit(lpc);
		linphone_proxy_config_set_avpf_mode(lpc, LinphoneAVPFEnabled);
		linphone_proxy_config_set_avpf_rr_interval(lpc, 3);
		linphone_proxy_config_done(lpc);
	}

	if (encryption_mandatory) {
		linphone_core_set_media_encryption_mandatory(marie->lc, TRUE);
		linphone_core_set_media_encryption_mandatory(pauline->lc, TRUE);
	}

	if (enable_video && linphone_core_video_supported(marie->lc)) {
		LinphoneVideoPolicy policy;
		policy.automatically_accept = TRUE;
		policy.automatically_initiate = TRUE;
		linphone_core_enable_video_capture(marie->lc, TRUE);
		linphone_core_enable_video_display(marie->lc, TRUE);
		linphone_core_set_video_policy(marie->lc, &policy);
		linphone_core_enable_video_capture(pauline->lc, TRUE);
		linphone_core_enable_video_display(pauline->lc, TRUE);
		linphone_core_set_video_policy(pauline->lc, &policy);
	}

	if (linphone_core_media_encryption_supported(marie->lc, srtp1)) {
		linphone_core_set_media_encryption(marie->lc, srtp1);
	} else {
		ms_message("Unsupported [%s] encryption type, cannot test", linphone_media_encryption_to_string(srtp1));
		goto end;
	}
	if (linphone_core_media_encryption_supported(pauline->lc, srtp2)) {
		linphone_core_set_media_encryption(pauline->lc, srtp2);
	} else {
		ms_message("Unsupported [%s] encryption type, cannot test", linphone_media_encryption_to_string(srtp2));
		goto end;
	}

	if (!BC_ASSERT_TRUE(call(marie, pauline))) goto end;
	BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &marie->stat.number_of_LinphoneCallStreamsRunning, 1));
	BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &pauline->stat.number_of_LinphoneCallStreamsRunning, 1));
	wait_for_until(marie->lc, pauline->lc, NULL, 0, 1000); /*wait 1 second for streams to start flowing*/
	if (linphone_core_get_current_call(marie->lc)) {
		params = linphone_call_get_current_params(linphone_core_get_current_call(marie->lc));
		BC_ASSERT_STRING_EQUAL(linphone_call_params_get_rtp_profile(params), expected_profile);
	}
	if (linphone_core_get_current_call(pauline->lc)) {
		params = linphone_call_get_current_params(linphone_core_get_current_call(pauline->lc));
		BC_ASSERT_STRING_EQUAL(linphone_call_params_get_rtp_profile(params), expected_profile);
	}

	linphone_core_terminate_all_calls(marie->lc);
	BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &marie->stat.number_of_LinphoneCallEnd, 1));
	BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &pauline->stat.number_of_LinphoneCallEnd, 1));
	BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &marie->stat.number_of_LinphoneCallReleased, 1));
	BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &pauline->stat.number_of_LinphoneCallReleased, 1));
	BC_ASSERT_EQUAL(marie->stat.number_of_LinphoneCallConnected, 1, int, "%d");
	BC_ASSERT_EQUAL(pauline->stat.number_of_LinphoneCallConnected, 1, int, "%d");
end:
	linphone_core_manager_destroy(pauline);
	linphone_core_manager_destroy(marie);
}

#ifdef VIDEO_ENABLED
/*This test simulates a case where an audio/video call is sent with AVPF only for video, not audio. */
static void avp_avpf_video_call(void) {
	LinphoneCoreManager *marie = linphone_core_manager_new("marie_rc");
	LinphoneCoreManager *pauline = linphone_core_manager_new("pauline_tcp_rc");
	LinphoneProxyConfig *lpc;
	const LinphoneCallParams *params;
	const char *expected_profile = "RTP/AVPF";

	linphone_config_set_bool(linphone_core_get_config(marie->lc), "misc", "no_avpf_for_audio", TRUE);

	lpc = linphone_core_get_default_proxy_config(marie->lc);
	linphone_proxy_config_edit(lpc);
	linphone_proxy_config_set_avpf_mode(lpc, LinphoneAVPFEnabled);
	linphone_proxy_config_set_avpf_rr_interval(lpc, 3);
	linphone_proxy_config_done(lpc);

	lpc = linphone_core_get_default_proxy_config(pauline->lc);
	linphone_proxy_config_edit(lpc);
	linphone_proxy_config_set_avpf_mode(lpc, LinphoneAVPFEnabled);
	linphone_proxy_config_set_avpf_rr_interval(lpc, 3);
	linphone_proxy_config_done(lpc);

	if (linphone_core_video_supported(marie->lc)) {
		LinphoneVideoPolicy policy;
		policy.automatically_accept = TRUE;
		policy.automatically_initiate = TRUE;
		linphone_core_enable_video_capture(marie->lc, TRUE);
		linphone_core_enable_video_display(marie->lc, TRUE);
		linphone_core_set_video_policy(marie->lc, &policy);
		linphone_core_enable_video_capture(pauline->lc, TRUE);
		linphone_core_enable_video_display(pauline->lc, TRUE);
		linphone_core_set_video_policy(pauline->lc, &policy);
	}

	if (!BC_ASSERT_TRUE(call(marie, pauline))) goto end;
	BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &marie->stat.number_of_LinphoneCallStreamsRunning, 1));
	BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &pauline->stat.number_of_LinphoneCallStreamsRunning, 1));
	wait_for_until(marie->lc, pauline->lc, NULL, 0, 1000); /*wait 1 second for streams to start flowing*/
	if (linphone_core_get_current_call(marie->lc)) {
		params = linphone_call_get_current_params(linphone_core_get_current_call(marie->lc));
		BC_ASSERT_STRING_EQUAL(linphone_call_params_get_rtp_profile(params), expected_profile);
	}
	if (linphone_core_get_current_call(pauline->lc)) {
		params = linphone_call_get_current_params(linphone_core_get_current_call(pauline->lc));
		BC_ASSERT_STRING_EQUAL(linphone_call_params_get_rtp_profile(params), expected_profile);
	}
	BC_ASSERT_TRUE(linphone_call_params_audio_enabled(
	    linphone_call_get_current_params(linphone_core_get_current_call(marie->lc))));
	BC_ASSERT_TRUE(linphone_call_params_audio_enabled(
	    linphone_call_get_current_params(linphone_core_get_current_call(pauline->lc))));

	linphone_core_terminate_all_calls(marie->lc);
	BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &marie->stat.number_of_LinphoneCallEnd, 1));
	BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &pauline->stat.number_of_LinphoneCallEnd, 1));
	BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &marie->stat.number_of_LinphoneCallReleased, 1));
	BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &pauline->stat.number_of_LinphoneCallReleased, 1));
	BC_ASSERT_EQUAL(marie->stat.number_of_LinphoneCallConnected, 1, int, "%d");
	BC_ASSERT_EQUAL(pauline->stat.number_of_LinphoneCallConnected, 1, int, "%d");
end:
	linphone_core_manager_destroy(pauline);
	linphone_core_manager_destroy(marie);
}

#endif

static void profile_call(bool_t avpf1,
                         LinphoneMediaEncryption srtp1,
                         bool_t avpf2,
                         LinphoneMediaEncryption srtp2,
                         const char *expected_profile,
                         bool_t enable_video) {
	profile_call_base(avpf1, srtp1, avpf2, srtp2, FALSE, expected_profile, enable_video);
}

static void avp_to_avp_call(void) {
	profile_call(FALSE, LinphoneMediaEncryptionNone, FALSE, LinphoneMediaEncryptionNone, "RTP/AVP", FALSE);
}
#ifdef VIDEO_ENABLED
static void avp_to_avp_video_call(void) {
	profile_call(FALSE, LinphoneMediaEncryptionNone, FALSE, LinphoneMediaEncryptionNone, "RTP/AVP", TRUE);
}
#endif

static void avp_to_avpf_call(void) {
	profile_call(FALSE, LinphoneMediaEncryptionNone, TRUE, LinphoneMediaEncryptionNone, "RTP/AVP", FALSE);
}
#ifdef VIDEO_ENABLED
static void avp_to_avpf_video_call(void) {
	profile_call(FALSE, LinphoneMediaEncryptionNone, TRUE, LinphoneMediaEncryptionNone, "RTP/AVP", TRUE);
}
#endif

static void avp_to_savp_call(void) {
	profile_call(FALSE, LinphoneMediaEncryptionNone, FALSE, LinphoneMediaEncryptionSRTP, "RTP/AVP", FALSE);
}
#ifdef VIDEO_ENABLED
static void avp_to_savp_video_call(void) {
	profile_call(FALSE, LinphoneMediaEncryptionNone, FALSE, LinphoneMediaEncryptionSRTP, "RTP/AVP", TRUE);
}
#endif

static void avp_to_savpf_call(void) {
	profile_call(FALSE, LinphoneMediaEncryptionNone, TRUE, LinphoneMediaEncryptionSRTP, "RTP/AVP", FALSE);
}
#ifdef VIDEO_ENABLED
static void avp_to_savpf_video_call(void) {
	profile_call(FALSE, LinphoneMediaEncryptionNone, TRUE, LinphoneMediaEncryptionSRTP, "RTP/AVP", TRUE);
}
#endif

static void avpf_to_avp_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionNone, FALSE, LinphoneMediaEncryptionNone, "RTP/AVPF", FALSE);
}
#ifdef VIDEO_ENABLED
static void avpf_to_avp_video_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionNone, FALSE, LinphoneMediaEncryptionNone, "RTP/AVPF", TRUE);
}
#endif

static void avpf_to_avpf_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionNone, TRUE, LinphoneMediaEncryptionNone, "RTP/AVPF", FALSE);
}
#ifdef VIDEO_ENABLED
static void avpf_to_avpf_video_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionNone, TRUE, LinphoneMediaEncryptionNone, "RTP/AVPF", TRUE);
}
#endif

static void avpf_to_savp_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionNone, FALSE, LinphoneMediaEncryptionSRTP, "RTP/AVPF", FALSE);
}
#ifdef VIDEO_ENABLED
static void avpf_to_savp_video_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionNone, FALSE, LinphoneMediaEncryptionSRTP, "RTP/AVPF", TRUE);
}
#endif

static void avpf_to_savpf_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionNone, TRUE, LinphoneMediaEncryptionSRTP, "RTP/AVPF", FALSE);
}
#ifdef VIDEO_ENABLED
static void avpf_to_savpf_video_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionNone, TRUE, LinphoneMediaEncryptionSRTP, "RTP/AVPF", TRUE);
}
#endif

static void savp_to_avp_call(void) {
	profile_call(FALSE, LinphoneMediaEncryptionSRTP, FALSE, LinphoneMediaEncryptionNone, "RTP/SAVP", FALSE);
}
#ifdef VIDEO_ENABLED
static void savp_to_avp_video_call(void) {
	profile_call(FALSE, LinphoneMediaEncryptionSRTP, FALSE, LinphoneMediaEncryptionNone, "RTP/SAVP", TRUE);
}
#endif

static void savp_to_avpf_call(void) {
	profile_call(FALSE, LinphoneMediaEncryptionSRTP, TRUE, LinphoneMediaEncryptionNone, "RTP/SAVP", FALSE);
}
#ifdef VIDEO_ENABLED
static void savp_to_avpf_video_call(void) {
	profile_call(FALSE, LinphoneMediaEncryptionSRTP, TRUE, LinphoneMediaEncryptionNone, "RTP/SAVP", TRUE);
}
#endif

static void savp_to_savp_call(void) {
	profile_call(FALSE, LinphoneMediaEncryptionSRTP, FALSE, LinphoneMediaEncryptionSRTP, "RTP/SAVP", FALSE);
}
#ifdef VIDEO_ENABLED
static void savp_to_savp_video_call(void) {
	profile_call(FALSE, LinphoneMediaEncryptionSRTP, FALSE, LinphoneMediaEncryptionSRTP, "RTP/SAVP", TRUE);
}
#endif

static void savp_to_savpf_call(void) {
	profile_call(FALSE, LinphoneMediaEncryptionSRTP, TRUE, LinphoneMediaEncryptionSRTP, "RTP/SAVP", FALSE);
}
#ifdef VIDEO_ENABLED
static void savp_to_savpf_video_call(void) {
	profile_call(FALSE, LinphoneMediaEncryptionSRTP, TRUE, LinphoneMediaEncryptionSRTP, "RTP/SAVP", TRUE);
}
#endif

static void savpf_to_avp_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionSRTP, FALSE, LinphoneMediaEncryptionNone, "RTP/SAVPF", FALSE);
}
#ifdef VIDEO_ENABLED
static void savpf_to_avp_video_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionSRTP, FALSE, LinphoneMediaEncryptionNone, "RTP/SAVPF", TRUE);
}
#endif

static void savpf_to_avpf_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionSRTP, TRUE, LinphoneMediaEncryptionNone, "RTP/SAVPF", FALSE);
}
#ifdef VIDEO_ENABLED
static void savpf_to_avpf_video_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionSRTP, TRUE, LinphoneMediaEncryptionNone, "RTP/SAVPF", TRUE);
}
#endif

static void savpf_to_savp_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionSRTP, FALSE, LinphoneMediaEncryptionSRTP, "RTP/SAVPF", FALSE);
}
#ifdef VIDEO_ENABLED
static void savpf_to_savp_video_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionSRTP, FALSE, LinphoneMediaEncryptionSRTP, "RTP/SAVPF", TRUE);
}
#endif
static void savpf_to_savpf_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionSRTP, TRUE, LinphoneMediaEncryptionSRTP, "RTP/SAVPF", FALSE);
}
#ifdef VIDEO_ENABLED
static void savpf_to_savpf_video_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionSRTP, TRUE, LinphoneMediaEncryptionSRTP, "RTP/SAVPF", TRUE);
}
#endif

static void savpf_dtls_to_savpf_dtls_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionDTLS, TRUE, LinphoneMediaEncryptionDTLS, "UDP/TLS/RTP/SAVPF", FALSE);
}
#ifdef VIDEO_ENABLED
static void savpf_dtls_to_savpf_dtls_video_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionDTLS, TRUE, LinphoneMediaEncryptionDTLS, "UDP/TLS/RTP/SAVPF", TRUE);
}
#endif

static void savpf_dtls_to_savpf_dtls_encryption_mandatory_call(void) {
	profile_call_base(TRUE, LinphoneMediaEncryptionDTLS, TRUE, LinphoneMediaEncryptionDTLS, TRUE, "UDP/TLS/RTP/SAVPF",
	                  FALSE);
}
#ifdef VIDEO_ENABLED
static void savpf_dtls_to_savpf_dtls_encryption_mandatory_video_call(void) {
	profile_call_base(TRUE, LinphoneMediaEncryptionDTLS, TRUE, LinphoneMediaEncryptionDTLS, TRUE, "UDP/TLS/RTP/SAVPF",
	                  TRUE);
}
#endif

static void savpf_dtls_to_savpf_encryption_mandatory_call(void) {
	/*profile_call_base(TRUE, LinphoneMediaEncryptionDTLS, TRUE, LinphoneMediaEncryptionSRTP, TRUE,
	 * "UDP/TLS/RTP/SAVPF",FALSE); not sure of result*/
}
#ifdef VIDEO_ENABLED
static void savpf_dtls_to_savpf_encryption_mandatory_video_call(void) {
	/*profile_call_base(TRUE, LinphoneMediaEncryptionDTLS, TRUE, LinphoneMediaEncryptionSRTP, TRUE,
	 * "UDP/TLS/RTP/SAVPF",TRUE); not sure of result*/
}
#endif

static void savpf_dtls_to_savpf_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionDTLS, TRUE, LinphoneMediaEncryptionSRTP, "UDP/TLS/RTP/SAVPF", FALSE);
}
#ifdef VIDEO_ENABLED
static void savpf_dtls_to_savpf_video_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionDTLS, TRUE, LinphoneMediaEncryptionSRTP, "UDP/TLS/RTP/SAVPF", TRUE);
}
#endif

static void savpf_dtls_to_avpf_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionDTLS, TRUE, LinphoneMediaEncryptionNone, "UDP/TLS/RTP/SAVPF", FALSE);
}
#ifdef VIDEO_ENABLED
static void savpf_dtls_to_avpf_video_call(void) {
	profile_call(TRUE, LinphoneMediaEncryptionDTLS, TRUE, LinphoneMediaEncryptionNone, "UDP/TLS/RTP/SAVPF", TRUE);
}
#endif

#ifdef VIDEO_ENABLED
static OrtpPayloadType *configure_core_for_avpf_and_video(LinphoneCore *lc) {
	LinphoneProxyConfig *lpc;
	OrtpPayloadType *pt;
	LinphoneVideoPolicy policy = {0};

	policy.automatically_initiate = TRUE;
	policy.automatically_accept = TRUE;
	lpc = linphone_core_get_default_proxy_config(lc);
	linphone_proxy_config_edit(lpc);
	linphone_proxy_config_set_avpf_mode(lpc, LinphoneAVPFEnabled);
	linphone_proxy_config_set_avpf_rr_interval(lpc, 3);
	linphone_proxy_config_done(lpc);
	linphone_core_set_video_device(lc, "StaticImage: Static picture");
	linphone_core_enable_video_capture(lc, TRUE);
	linphone_core_enable_video_display(lc, TRUE);
	linphone_core_set_video_policy(lc, &policy);
	pt = linphone_core_find_payload_type(lc, "VP8", 90000, -1);
	if (pt == NULL) {
		ms_warning("VP8 codec not available.");
	} else {
		disable_all_video_codecs_except_one(lc, "VP8");
	}
	return pt;
}

static void check_avpf_features(LinphoneCore *lc, unsigned char expected_features) {
	LinphoneCall *lcall = linphone_core_get_current_call(lc);
	BC_ASSERT_PTR_NOT_NULL(lcall);
	if (lcall != NULL) {
		SalMediaDescription *resultDesc = _linphone_call_get_result_desc(lcall);
		const auto &desc = resultDesc->findStream(SalProtoRtpAvpf, SalVideo);
		BC_ASSERT_TRUE(desc != Utils::getEmptyConstRefObject<SalStreamDescription>());
		if (desc != Utils::getEmptyConstRefObject<SalStreamDescription>()) {
			const auto &payloads = desc.getPayloads();
			BC_ASSERT_FALSE(payloads.empty());
			if (!payloads.empty()) {
				PayloadType *pt = payloads.front();
				BC_ASSERT_STRING_EQUAL(pt->mime_type, "VP8");
				BC_ASSERT_EQUAL(pt->avpf.features, expected_features, int, "%d");
			}
		}
	}
}

static void compatible_avpf_features(void) {
	LinphoneCoreManager *marie = linphone_core_manager_new("marie_rc");
	LinphoneCoreManager *pauline = linphone_core_manager_new("pauline_tcp_rc");
	OrtpPayloadType *pt;
	bool_t call_ok;

	if (configure_core_for_avpf_and_video(marie->lc) == NULL) goto end;

	pt = configure_core_for_avpf_and_video(pauline->lc);

	BC_ASSERT_TRUE((call_ok = call(marie, pauline)));
	if (!call_ok) goto end;

	BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &marie->stat.number_of_LinphoneCallStreamsRunning, 1));
	BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &pauline->stat.number_of_LinphoneCallStreamsRunning, 1));
	wait_for_until(marie->lc, pauline->lc, NULL, 0, 1000); /*wait 1 second for streams to start flowing*/
	check_avpf_features(marie->lc, pt->avpf.features);
	check_avpf_features(pauline->lc, pt->avpf.features);

	end_call(marie, pauline);
end:
	linphone_core_manager_destroy(pauline);
	linphone_core_manager_destroy(marie);
}

static void incompatible_avpf_features(void) {
	LinphoneCoreManager *marie = linphone_core_manager_new("marie_rc");
	LinphoneCoreManager *pauline = linphone_core_manager_new("pauline_tcp_rc");
	OrtpPayloadType *pt;
	bool_t call_ok;

	if (configure_core_for_avpf_and_video(marie->lc) == NULL) goto end;

	pt = configure_core_for_avpf_and_video(pauline->lc);
	pt->avpf.features = PAYLOAD_TYPE_AVPF_NONE;

	BC_ASSERT_TRUE(call_ok = call(marie, pauline));
	if (!call_ok) goto end;
	BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &marie->stat.number_of_LinphoneCallStreamsRunning, 1));
	BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &pauline->stat.number_of_LinphoneCallStreamsRunning, 1));
	wait_for_until(marie->lc, pauline->lc, NULL, 0, 1000); /*wait 1 second for streams to start flowing*/
	check_avpf_features(marie->lc, PAYLOAD_TYPE_AVPF_NONE);
	check_avpf_features(pauline->lc, PAYLOAD_TYPE_AVPF_NONE);

	end_call(marie, pauline);
end:
	linphone_core_manager_destroy(pauline);
	linphone_core_manager_destroy(marie);
}
#endif

static test_t offeranswer_tests[] = {
    TEST_NO_TAG("Start with no config", start_with_no_config),
    TEST_NO_TAG("Call failed because of codecs", call_failed_because_of_codecs),
    TEST_NO_TAG("Simple call with different codec mappings", simple_call_with_different_codec_mappings),
    TEST_NO_TAG("Simple call with fmtps", simple_call_with_fmtps),
    TEST_NO_TAG("AVP to AVP call", avp_to_avp_call),
    TEST_NO_TAG("AVP to AVPF call", avp_to_avpf_call),
    TEST_NO_TAG("AVP to SAVP call", avp_to_savp_call),
    TEST_NO_TAG("AVP to SAVPF call", avp_to_savpf_call),
    TEST_NO_TAG("AVPF to AVP call", avpf_to_avp_call),
    TEST_NO_TAG("AVPF to AVPF call", avpf_to_avpf_call),
    TEST_NO_TAG("AVPF to SAVP call", avpf_to_savp_call),
    TEST_NO_TAG("AVPF to SAVPF call", avpf_to_savpf_call),
    TEST_NO_TAG("SAVP to AVP call", savp_to_avp_call),
    TEST_NO_TAG("SAVP to AVPF call", savp_to_avpf_call),
    TEST_NO_TAG("SAVP to SAVP call", savp_to_savp_call),
    TEST_NO_TAG("SAVP to SAVPF call", savp_to_savpf_call),
    TEST_NO_TAG("SAVPF to AVP call", savpf_to_avp_call),
    TEST_NO_TAG("SAVPF to AVPF call", savpf_to_avpf_call),
    TEST_NO_TAG("SAVPF to SAVP call", savpf_to_savp_call),
    TEST_NO_TAG("SAVPF to SAVPF call", savpf_to_savpf_call),
    TEST_ONE_TAG("SAVPF/DTLS to SAVPF/DTLS call", savpf_dtls_to_savpf_dtls_call, "DTLS"),
    TEST_ONE_TAG("SAVPF/DTLS to SAVPF/DTLS encryption mandatory call",
                 savpf_dtls_to_savpf_dtls_encryption_mandatory_call,
                 "DTLS"),
    TEST_ONE_TAG("SAVPF/DTLS to SAVPF call", savpf_dtls_to_savpf_call, "DTLS"),
    TEST_ONE_TAG(
        "SAVPF/DTLS to SAVPF encryption mandatory call", savpf_dtls_to_savpf_encryption_mandatory_call, "DTLS"),
    TEST_ONE_TAG("SAVPF/DTLS to AVPF call", savpf_dtls_to_avpf_call, "DTLS"),
#ifdef VIDEO_ENABLED
    TEST_NO_TAG("AVP to AVP video call", avp_to_avp_video_call),
    TEST_NO_TAG("AVP to AVPF video call", avp_to_avpf_video_call),
    TEST_NO_TAG("AVP to SAVP video call", avp_to_savp_video_call),
    TEST_NO_TAG("AVP to SAVPF video call", avp_to_savpf_video_call),
    TEST_NO_TAG("AVPF to AVP video call", avpf_to_avp_video_call),
    TEST_NO_TAG("AVPF to AVPF video call", avpf_to_avpf_video_call),
    TEST_NO_TAG("AVPF to SAVP video call", avpf_to_savp_video_call),
    TEST_NO_TAG("AVPF to SAVPF video call", avpf_to_savpf_video_call),
    TEST_NO_TAG("SAVP to AVP video call", savp_to_avp_video_call),
    TEST_NO_TAG("SAVP to AVPF video call", savp_to_avpf_video_call),
    TEST_NO_TAG("SAVP to SAVP video call", savp_to_savp_video_call),
    TEST_NO_TAG("SAVP to SAVPF video call", savp_to_savpf_video_call),
    TEST_NO_TAG("SAVPF to AVP video call", savpf_to_avp_video_call),
    TEST_NO_TAG("SAVPF to AVPF video call", savpf_to_avpf_video_call),
    TEST_NO_TAG("SAVPF to SAVP video call", savpf_to_savp_video_call),
    TEST_NO_TAG("SAVPF to SAVPF video call", savpf_to_savpf_video_call),
    TEST_ONE_TAG("SAVPF/DTLS to SAVPF/DTLS video call", savpf_dtls_to_savpf_dtls_video_call, "DTLS"),
    TEST_ONE_TAG("SAVPF/DTLS to SAVPF/DTLS encryption mandatory video call",
                 savpf_dtls_to_savpf_dtls_encryption_mandatory_video_call,
                 "DTLS"),
    TEST_ONE_TAG("SAVPF/DTLS to SAVPF video call", savpf_dtls_to_savpf_video_call, "DTLS"),
    TEST_ONE_TAG("SAVPF/DTLS to SAVPF encryption mandatory video call",
                 savpf_dtls_to_savpf_encryption_mandatory_video_call,
                 "DTLS"),
    TEST_ONE_TAG("SAVPF/DTLS to AVPF video call", savpf_dtls_to_avpf_video_call, "DTLS"),

    TEST_NO_TAG("Compatible AVPF features", compatible_avpf_features),
    TEST_NO_TAG("Incompatible AVPF features", incompatible_avpf_features),
    TEST_NO_TAG("H264 packetization-mode set to 1 on sender", h264_call_with_fmtps),
    TEST_NO_TAG("H264 on sender, but not on receiver", h264_call_receiver_with_no_h264_support),
    TEST_NO_TAG("H264 packetization-mode not set", h264_call_without_packetization_mode),
    TEST_NO_TAG("Mixed AVP+AVPF video call", avp_avpf_video_call),
    TEST_NO_TAG("flexfec both proposed both accepted", flexfec_test_both_proposed_both_accepted),
    TEST_NO_TAG("flexfec both proposed bundle accepted", flexfec_test_both_proposed_bundle_accepted),
    TEST_NO_TAG("flexfec both proposed nothing accepted", flexfec_test_both_proposed_nothing_accepted),
    TEST_NO_TAG("flexfec both proposed only fec accepted", flexfec_test_both_poposed_only_fec_accepted),
    TEST_NO_TAG("flexfec only fec proposed and accepted", flexfec_test_only_fec_proposed_and_accepted),

#endif
};

test_suite_t offeranswer_test_suite = {"Offer-answer",
                                       NULL,
                                       NULL,
                                       liblinphone_tester_before_each,
                                       liblinphone_tester_after_each,
                                       sizeof(offeranswer_tests) / sizeof(offeranswer_tests[0]),
                                       offeranswer_tests};
