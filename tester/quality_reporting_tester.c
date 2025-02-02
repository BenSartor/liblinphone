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

#include "liblinphone_tester.h"
#include "linphone/core.h"
#include "quality_reporting.h"
#include "tester_utils.h"

/* Avoid crash if x is NULL on libc versions <4.5.26 */
#define __strstr(x, y) ((x == NULL) ? NULL : strstr(x, y))

static void on_report_send_mandatory(BCTBX_UNUSED(const LinphoneCall *call),
                                     BCTBX_UNUSED(SalStreamType stream_type),
                                     const LinphoneContent *content) {
	const char *body = linphone_content_get_utf8_text(content);
	char *remote_metrics_start = __strstr(body, "RemoteMetrics:");
	BC_ASSERT_TRUE((__strstr(body, "VQIntervalReport\r\n") == body) ||
	               (__strstr(body, "VQSessionReport\r\n") == body) ||
	               (__strstr(body, "VQSessionReport: CallTerm\r\n") == body));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "CallID:"));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "LocalID:"));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "RemoteID:"));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "OrigID:"));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "LocalGroup:"));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "RemoteGroup:"));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "LocalAddr:"));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "IP="));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "PORT="));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "SSRC="));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "RemoteAddr:"));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "IP="));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "PORT="));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "SSRC="));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "LocalMetrics:"));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "Timestamps:"));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "START="));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "STOP="));

	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "SessionDesc:"));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "PT="));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "PD="));
	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "SR="));

	/* We should have not reached RemoteMetrics section yet */
	BC_ASSERT_TRUE(!remote_metrics_start || body < remote_metrics_start);

	BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "DialogID:"));
}

static const char *on_report_send_verify_metrics(const reporting_content_metrics_t *metrics, const char *body) {
	if (metrics->rtcp_xr_count) {
		BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "SessionDesc:"));
		BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "JitterBuffer:"));
		BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "PacketLoss:"));
	}
	if (metrics->rtcp_sr_count + metrics->rtcp_xr_count > 0) BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "Delay:"));
	if (metrics->rtcp_xr_count) BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "QualityEst:"));

	return body;
}

#if VIDEO_ENABLED
static void
on_report_send_with_rtcp_xr_local(const LinphoneCall *call, SalStreamType stream_type, const LinphoneContent *content) {
	const char *body = NULL;
	const char *local_metrics_start = NULL;
	const char *remote_metrics_start = NULL;
	reporting_session_report_t *report = linphone_quality_reporting_get_reports(
	    linphone_call_log_get_quality_reporting(linphone_call_get_call_log(call)))[stream_type];
	on_report_send_mandatory(call, stream_type, content);

	body = linphone_content_get_utf8_text(content);
	local_metrics_start = __strstr(body, "LocalMetrics:");
	remote_metrics_start = __strstr(body, "RemoteMetrics:");
	BC_ASSERT_PTR_NOT_NULL(local_metrics_start);
	BC_ASSERT_TRUE(!remote_metrics_start ||
	               on_report_send_verify_metrics(&report->local_metrics, body) < remote_metrics_start);
}
#endif

static void on_report_send_with_rtcp_xr_remote(const LinphoneCall *call,
                                               SalStreamType stream_type,
                                               const LinphoneContent *content) {
	char *body = (char *)linphone_content_get_buffer(content);
	reporting_session_report_t *report = linphone_quality_reporting_get_reports(
	    linphone_call_log_get_quality_reporting(linphone_call_get_call_log(call)))[stream_type];

	on_report_send_mandatory(call, stream_type, content);
	if ((report->remote_metrics.rtcp_sr_count + report->remote_metrics.rtcp_xr_count) > 0) {
		BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "RemoteMetrics:"));
		BC_ASSERT_PTR_NOT_NULL(body = __strstr(body, "Timestamps:"));
		on_report_send_verify_metrics(&report->remote_metrics, body);
	}
}

/*
static void on_report_send_with_rtcp_xr_both (const LinphoneCall *call, SalStreamType stream_type, const LinphoneContent
*content) { on_report_send_with_rtcp_xr_local(call, stream_type, content); on_report_send_with_rtcp_xr_remote(call,
stream_type, content);
}
*/

static bool_t create_call_for_quality_reporting_tests(LinphoneCoreManager *marie,
                                                      LinphoneCoreManager *pauline,
                                                      LinphoneCall **call_marie,
                                                      LinphoneCall **call_pauline,
                                                      LinphoneCallParams *params_marie,
                                                      LinphoneCallParams *params_pauline) {
	bool_t call_succeeded = call_with_params(marie, pauline, params_marie, params_pauline);
	BC_ASSERT_TRUE(call_succeeded);
	if (call_succeeded) {
		if (call_marie) {
			*call_marie = linphone_core_get_current_call(marie->lc);
			BC_ASSERT_PTR_NOT_NULL(*call_marie);
		}
		if (call_pauline) {
			*call_pauline = linphone_core_get_current_call(pauline->lc);
			BC_ASSERT_PTR_NOT_NULL(*call_pauline);
		}
	}
	return call_succeeded;
}

static void quality_reporting_not_used_without_config(void) {
	LinphoneCoreManager *marie = linphone_core_manager_new("marie_quality_reporting_rc");
	LinphoneCoreManager *pauline = linphone_core_manager_new("pauline_tcp_rc");
	LinphoneCall *call_marie = NULL;
	LinphoneCall *call_pauline = NULL;
	reporting_session_report_t **quality_reports = NULL;

	if (create_call_for_quality_reporting_tests(marie, pauline, &call_marie, &call_pauline, NULL, NULL)) {
		// Marie has stats collection enabled but Pauline has not

		const LinphoneAccount *marie_account = linphone_call_get_dest_account(call_marie);
		const LinphoneAccountParams *marie_account_params = linphone_account_get_params(marie_account);
		BC_ASSERT_TRUE(linphone_account_params_quality_reporting_enabled(marie_account_params));

		const LinphoneAccount *pauline_account = linphone_call_get_dest_account(call_pauline);
		const LinphoneAccountParams *pauline_account_params = linphone_account_get_params(pauline_account);
		BC_ASSERT_FALSE(linphone_account_params_quality_reporting_enabled(pauline_account_params));

		// This field should be already filled
		quality_reports = linphone_quality_reporting_get_reports(
		    linphone_call_log_get_quality_reporting(linphone_call_get_call_log(call_marie)));
		BC_ASSERT_PTR_NOT_NULL(quality_reports[0]->info.local_addr.ip);

		// But not this one since it is updated at the end of call
		BC_ASSERT_PTR_NULL(quality_reports[0]->dialog_id);
		end_call(marie, pauline);
	}

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void quality_reporting_not_sent_if_call_not_started(void) {
	LinphoneCoreManager *marie = linphone_core_manager_new("marie_quality_reporting_rc");
	LinphoneCoreManager *pauline = linphone_core_manager_new("pauline_tcp_rc");
	LinphoneCallLog *out_call_log;
	LinphoneCall *out_call;

	linphone_core_set_max_calls(pauline->lc, 0);
	out_call = linphone_core_invite(marie->lc, "pauline");
	BC_ASSERT_PTR_NOT_NULL(out_call);
	if (!out_call) goto end;
	linphone_call_ref(out_call);

	BC_ASSERT_TRUE(wait_for_until(marie->lc, pauline->lc, &marie->stat.number_of_LinphoneCallError, 1, 10000));
	BC_ASSERT_EQUAL(marie->stat.number_of_LinphoneCallError, 1, int, "%d");

	if (bctbx_list_size(linphone_core_get_call_logs(marie->lc)) > 0) {
		out_call_log = (LinphoneCallLog *)(linphone_core_get_call_logs(marie->lc)->data);
		BC_ASSERT_PTR_NOT_NULL(out_call_log);
		BC_ASSERT_EQUAL(linphone_call_log_get_status(out_call_log), LinphoneCallAborted, int, "%d");
	}
	linphone_call_unref(out_call);

	// Wait a few time...
	wait_for_until(marie->lc, NULL, NULL, 0, 1000);

	// Since the callee was busy, there should be no publish to do
	BC_ASSERT_EQUAL(marie->stat.number_of_LinphonePublishProgress, 0, int, "%d");
	BC_ASSERT_EQUAL(marie->stat.number_of_LinphonePublishOk, 0, int, "%d");

end:
	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void quality_reporting_not_sent_if_low_bandwidth(void) {
	LinphoneCoreManager *marie = linphone_core_manager_new("marie_quality_reporting_rc");
	LinphoneCoreManager *pauline = linphone_core_manager_new("pauline_tcp_rc");
	LinphoneCallParams *marie_params = linphone_core_create_call_params(marie->lc, NULL);
	linphone_call_params_enable_low_bandwidth(marie_params, TRUE);

	if (create_call_for_quality_reporting_tests(marie, pauline, NULL, NULL, marie_params, NULL)) {
		end_call(marie, pauline);

		BC_ASSERT_EQUAL(marie->stat.number_of_LinphonePublishProgress, 0, int, "%d");
		BC_ASSERT_EQUAL(marie->stat.number_of_LinphonePublishOk, 0, int, "%d");
	}
	linphone_call_params_unref(marie_params);
	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void on_report_send_remove_fields(BCTBX_UNUSED(const LinphoneCall *call),
                                         BCTBX_UNUSED(SalStreamType stream_type),
                                         const LinphoneContent *content) {
	char *body = bctbx_strdup(linphone_content_get_utf8_text(content));
	/* Corrupt start of the report */
	const char *corrupted_str = "corrupted report is corrupted";
	size_t corrupted_len = strlen(corrupted_str);
	strncpy(body, corrupted_str, corrupted_len + 1);
	linphone_content_set_utf8_text((LinphoneContent *)content, body);
	bctbx_free(body);
}

static void quality_reporting_invalid_report(void) {
	LinphoneCoreManager *marie = linphone_core_manager_new("marie_quality_reporting_rc");
	LinphoneCoreManager *pauline = linphone_core_manager_new("pauline_tcp_rc");
	LinphoneCall *call_marie = NULL;
	LinphoneCall *call_pauline = NULL;

	if (create_call_for_quality_reporting_tests(marie, pauline, &call_marie, &call_pauline, NULL, NULL)) {
		linphone_reporting_set_on_report_send(call_marie, on_report_send_remove_fields);

		end_call(marie, pauline);

		BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &marie->stat.number_of_LinphonePublishProgress, 1));
		BC_ASSERT_TRUE(wait_for_until(marie->lc, pauline->lc, &marie->stat.number_of_LinphonePublishError, 1, 3000));
		BC_ASSERT_EQUAL(marie->stat.number_of_LinphonePublishError, 1, int, "%d");
		BC_ASSERT_EQUAL(marie->stat.number_of_LinphonePublishOk, 0, int, "%d");
	}

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void quality_reporting_at_call_termination(void) {
	LinphoneCoreManager *marie = linphone_core_manager_new("marie_quality_reporting_rc");
	LinphoneCoreManager *pauline = linphone_core_manager_new("pauline_rc_rtcp_xr");
	LinphoneCall *call_marie = NULL;
	LinphoneCall *call_pauline = NULL;
	reporting_session_report_t **quality_reports = NULL;

	if (create_call_for_quality_reporting_tests(marie, pauline, &call_marie, &call_pauline, NULL, NULL)) {
		linphone_reporting_set_on_report_send(call_marie, on_report_send_with_rtcp_xr_remote);

		linphone_core_terminate_all_calls(marie->lc);

		// Now dialog id should be filled
		quality_reports = linphone_quality_reporting_get_reports(
		    linphone_call_log_get_quality_reporting(linphone_call_get_call_log(call_marie)));
		BC_ASSERT_PTR_NOT_NULL(quality_reports[0]->dialog_id);

		BC_ASSERT_TRUE(wait_for_until(marie->lc, pauline->lc, &marie->stat.number_of_LinphoneCallReleased, 1, 10000));
		BC_ASSERT_TRUE(wait_for_until(pauline->lc, NULL, &pauline->stat.number_of_LinphoneCallReleased, 1, 10000));

		BC_ASSERT_PTR_NULL(linphone_core_get_current_call(marie->lc));
		BC_ASSERT_PTR_NULL(linphone_core_get_current_call(pauline->lc));

		// PUBLISH submission to the collector should be ok
		BC_ASSERT_TRUE(wait_for(marie->lc, NULL, &marie->stat.number_of_LinphonePublishProgress, 1));
		BC_ASSERT_EQUAL(marie->stat.number_of_LinphonePublishProgress, 1, int, "%d");
		BC_ASSERT_TRUE(wait_for(marie->lc, NULL, &marie->stat.number_of_LinphonePublishOk, 1));
	}

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void quality_reporting_interval_report(void) {
	LinphoneCoreManager *marie = linphone_core_manager_new("marie_rc_rtcp_xr");
	LinphoneCoreManager *pauline = linphone_core_manager_new("pauline_rc_rtcp_xr");
	LinphoneCall *call_marie = NULL;
	LinphoneCall *call_pauline = NULL;

	if (create_call_for_quality_reporting_tests(marie, pauline, &call_marie, &call_pauline, NULL, NULL)) {
		linphone_reporting_set_on_report_send(call_marie, on_report_send_mandatory);
		LinphoneAccount *account = linphone_call_get_dest_account(call_marie);
		LinphoneAccountParams *account_params = linphone_account_params_clone(linphone_account_get_params(account));
		linphone_account_params_set_quality_reporting_interval(account_params, 1);
		linphone_account_set_params(account, account_params);
		linphone_account_params_unref(account_params);

		BC_ASSERT_PTR_NOT_NULL(linphone_core_get_current_call(marie->lc));
		BC_ASSERT_PTR_NOT_NULL(linphone_core_get_current_call(pauline->lc));

		// PUBLISH submission to the collector should be ok
		BC_ASSERT_TRUE(
		    wait_for_until(marie->lc, pauline->lc, &marie->stat.number_of_LinphonePublishProgress, 1, 60000));
		BC_ASSERT_TRUE(wait_for_until(marie->lc, pauline->lc, &marie->stat.number_of_LinphonePublishOk, 1, 60000));
		end_call(marie, pauline);
		BC_ASSERT_TRUE(wait_for(marie->lc, pauline->lc, &marie->stat.number_of_LinphonePublishOk, 2));
	}

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

#ifdef VIDEO_ENABLED
static void quality_reporting_session_report_if_video_stopped(void) {
	LinphoneCoreManager *marie = linphone_core_manager_new("marie_rc_rtcp_xr");
	LinphoneCoreManager *pauline = linphone_core_manager_new("pauline_tcp_rc");
	LinphoneCall *call_pauline = NULL;
	LinphoneCall *call_marie = NULL;
	LinphoneCallParams *pauline_params;
	LinphoneCallParams *marie_params;

	linphone_core_enable_video_capture(marie->lc, TRUE);
	linphone_core_enable_video_display(marie->lc, FALSE);
	linphone_core_enable_video_capture(pauline->lc, TRUE);
	linphone_core_enable_video_display(pauline->lc, FALSE);
	marie_params = linphone_core_create_call_params(marie->lc, NULL);
	linphone_call_params_enable_video(marie_params, TRUE);
	pauline_params = linphone_core_create_call_params(pauline->lc, NULL);
	linphone_call_params_enable_video(pauline_params, TRUE);

	if (create_call_for_quality_reporting_tests(marie, pauline, &call_marie, &call_pauline, marie_params,
	                                            pauline_params)) {
		linphone_reporting_set_on_report_send(call_marie, on_report_send_with_rtcp_xr_local);

		BC_ASSERT_EQUAL(marie->stat.number_of_LinphonePublishProgress, 0, int, "%d");
		BC_ASSERT_EQUAL(marie->stat.number_of_LinphonePublishOk, 0, int, "%d");

		BC_ASSERT_TRUE(wait_for_until(marie->lc, pauline->lc, NULL, 0, 10000));
		BC_ASSERT_TRUE(linphone_call_params_video_enabled(linphone_call_get_current_params(call_pauline)));

		/* Remove video */
		linphone_call_params_unref(pauline_params);
		pauline_params = linphone_core_create_call_params(pauline->lc, call_pauline);
		linphone_call_params_enable_video(pauline_params, FALSE);
		linphone_call_update(call_pauline, pauline_params);
		linphone_call_params_unref(pauline_params);
		BC_ASSERT_TRUE(
		    wait_for_until(marie->lc, pauline->lc, &marie->stat.number_of_LinphoneCallStreamsRunning, 2, 10000));

		BC_ASSERT_TRUE(
		    wait_for_until(marie->lc, pauline->lc, &marie->stat.number_of_LinphonePublishProgress, 1, 10000));
		BC_ASSERT_TRUE(wait_for_until(marie->lc, pauline->lc, &marie->stat.number_of_LinphonePublishOk, 1, 10000));

		BC_ASSERT_FALSE(linphone_call_params_video_enabled(linphone_call_get_current_params(call_pauline)));

		end_call(marie, pauline);

		BC_ASSERT_TRUE(wait_for_until(marie->lc, pauline->lc, &marie->stat.number_of_LinphonePublishProgress, 2, 5000));
		BC_ASSERT_TRUE(wait_for_until(marie->lc, pauline->lc, &marie->stat.number_of_LinphonePublishOk, 2, 5000));
	}
	linphone_call_params_unref(marie_params);

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}
#endif

static void publish_report_with_route_state_changed(LinphoneCore *lc, LinphoneEvent *ev, LinphonePublishState state) {
	if (state == LinphonePublishProgress) {
		char *uri = linphone_address_as_string(linphone_event_get_resource(ev));
		BC_ASSERT_STRING_EQUAL(
		    uri, linphone_proxy_config_get_quality_reporting_collector(linphone_core_get_default_proxy_config(lc)));
		bctbx_free(uri);
	}
}

static void quality_reporting_sent_using_custom_route(void) {
	LinphoneCoreManager *marie = linphone_core_manager_new("marie_quality_reporting_rc");
	LinphoneCoreManager *pauline = linphone_core_manager_new("pauline_tcp_rc");
	LinphoneCall *call_marie = NULL;
	LinphoneCall *call_pauline = NULL;

	LinphoneCoreCbs *cbs = linphone_factory_create_core_cbs(linphone_factory_get());
	linphone_core_cbs_set_publish_state_changed(cbs, publish_report_with_route_state_changed);
	linphone_core_add_callbacks(marie->lc, cbs);
	linphone_core_cbs_unref(cbs);

	// INVALID collector: sip.linphone.org do not collect reports, so it will throw a 404 Not Found error
	LinphoneProxyConfig *config = linphone_core_get_default_proxy_config(marie->lc);
	linphone_proxy_config_edit(config);
	linphone_proxy_config_set_quality_reporting_collector(config, "sip:invalid.uri");
	linphone_proxy_config_done(config);

	if (create_call_for_quality_reporting_tests(marie, pauline, &call_marie, &call_pauline, NULL, NULL)) {
		end_call(marie, pauline);

		// PUBLISH submission to the collector should be ERROR since route is not valid
		BC_ASSERT_TRUE(wait_for(marie->lc, NULL, &marie->stat.number_of_LinphonePublishProgress, 1));
		BC_ASSERT_EQUAL(marie->stat.number_of_LinphonePublishProgress, 1, int, "%d");
		BC_ASSERT_TRUE(wait_for_until(marie->lc, NULL, &marie->stat.number_of_LinphonePublishError, 1, 10000));
		BC_ASSERT_EQUAL(marie->stat.number_of_LinphonePublishOk, 0, int, "%d");
	}

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void quality_reporting_interval_report_video_and_rtt_base(bool_t enable_video) {
	LinphoneCoreManager *marie = linphone_core_manager_new("marie_rc_rtcp_xr");
	LinphoneCoreManager *pauline = linphone_core_manager_new("pauline_rc_rtcp_xr");
	LinphoneCall *call_marie = NULL;
	LinphoneCall *call_pauline = NULL;
	LinphoneCallParams *pauline_params;
	LinphoneCallParams *marie_params;
	LinphoneChatRoom *pauline_chat_room;

	linphone_core_enable_video_capture(marie->lc, TRUE);
	linphone_core_enable_video_display(marie->lc, FALSE);
	linphone_core_enable_video_capture(pauline->lc, TRUE);
	linphone_core_enable_video_display(pauline->lc, FALSE);
	marie_params = linphone_core_create_call_params(marie->lc, NULL);
	linphone_call_params_enable_video(marie_params, enable_video);
	linphone_call_params_enable_realtime_text(marie_params, TRUE);
	pauline_params = linphone_core_create_call_params(pauline->lc, NULL);
	linphone_call_params_enable_video(pauline_params, enable_video);
	linphone_call_params_enable_realtime_text(pauline_params, TRUE);

	if (create_call_for_quality_reporting_tests(marie, pauline, &call_marie, &call_pauline, marie_params,
	                                            pauline_params)) {
		linphone_reporting_set_on_report_send(call_marie, on_report_send_mandatory);
		LinphoneAccount *account = linphone_call_get_dest_account(call_marie);
		LinphoneAccountParams *account_params = linphone_account_params_clone(linphone_account_get_params(account));
		linphone_account_params_set_quality_reporting_interval(account_params, 3);
		linphone_account_set_params(account, account_params);
		linphone_account_params_unref(account_params);

		BC_ASSERT_TRUE(wait_for_until(marie->lc, pauline->lc, NULL, 0, 3000));
		BC_ASSERT_TRUE(linphone_call_params_video_enabled(linphone_call_get_current_params(call_pauline)) ==
		               enable_video);
		BC_ASSERT_TRUE(linphone_call_params_realtime_text_enabled(linphone_call_get_current_params(call_pauline)));

		BC_ASSERT_PTR_NOT_NULL(linphone_core_get_current_call(marie->lc));
		BC_ASSERT_PTR_NOT_NULL(linphone_core_get_current_call(pauline->lc));

		// PUBLISH submission to the collector should be ok
		BC_ASSERT_TRUE(wait_for_until(marie->lc, pauline->lc, &marie->stat.number_of_LinphonePublishProgress, 1, 5000));
		BC_ASSERT_TRUE(wait_for_until(marie->lc, pauline->lc, &marie->stat.number_of_LinphonePublishOk, 1, 10000));

		pauline_chat_room = linphone_call_get_chat_room(call_pauline);
		BC_ASSERT_PTR_NOT_NULL(pauline_chat_room);
		LinphoneChatMessage *rtt_message = NULL;
		if (pauline_chat_room) {
			const char *message = "Lorem Ipsum Belledonnum Communicatum";
			size_t i;
			rtt_message = linphone_chat_room_create_message_from_utf8(pauline_chat_room, NULL);
			LinphoneChatRoom *marie_chat_room = linphone_call_get_chat_room(call_marie);

			for (i = 0; i < strlen(message); i++) {
				linphone_chat_message_put_char(rtt_message, message[i]);
				BC_ASSERT_TRUE(wait_for_until(pauline->lc, marie->lc,
				                              &marie->stat.number_of_LinphoneIsComposingActiveReceived, (int)(i + 1),
				                              1000));
				BC_ASSERT_EQUAL(linphone_chat_room_get_char(marie_chat_room), message[i], char, "%c");
			}
			linphone_chat_message_send(rtt_message);
		}

		end_call(marie, pauline);
		/* Wait that all publish complete */
		BC_ASSERT_TRUE(wait_for_until(marie->lc, pauline->lc, &marie->stat.number_of_LinphonePublishOk,
		                              marie->stat.number_of_LinphonePublishProgress, 60000));

		if (rtt_message) linphone_chat_message_unref(rtt_message);
	}

	linphone_call_params_unref(marie_params);
	linphone_call_params_unref(pauline_params);

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void quality_reporting_interval_report_rtt(void) {
	quality_reporting_interval_report_video_and_rtt_base(FALSE);
}

#ifdef VIDEO_ENABLED
static void quality_reporting_interval_report_video_and_rtt(void) {
	quality_reporting_interval_report_video_and_rtt_base(TRUE);
}
#endif

static void video_bandwidth_estimation(void) {
	LinphoneCoreManager *marie = linphone_core_manager_new("marie_rc");
	LinphoneCoreManager *pauline = linphone_core_manager_new("pauline_rc");
	LinphoneVideoPolicy pol = {0};
	OrtpNetworkSimulatorParams simparams = {0};

	linphone_core_set_video_device(marie->lc, "Mire: Mire (synthetic moving picture)");
	linphone_core_enable_video_capture(marie->lc, TRUE);
	linphone_core_enable_video_display(marie->lc, TRUE);
	linphone_core_enable_video_capture(pauline->lc, TRUE);
	linphone_core_enable_video_display(pauline->lc, TRUE);

	pol.automatically_accept = TRUE;
	pol.automatically_initiate = TRUE;
	linphone_core_set_video_policy(marie->lc, &pol);
	linphone_core_set_video_policy(pauline->lc, &pol);

	linphone_core_set_preferred_video_definition_by_name(marie->lc, "vga");
	simparams.mode = OrtpNetworkSimulatorOutbound;
	simparams.enabled = TRUE;
	simparams.max_bandwidth = 300000;
	linphone_core_set_network_simulator_params(marie->lc, &simparams);

	if (BC_ASSERT_TRUE(call(marie, pauline))) {
		/* Wait for the first TMMBR */
		BC_ASSERT_TRUE(wait_for_until(marie->lc, pauline->lc, &marie->stat.number_of_tmmbr_received, 1, 50000));
		BC_ASSERT_GREATER((float)marie->stat.last_tmmbr_value_received, 270000.f, float, "%f");
		BC_ASSERT_LOWER((float)marie->stat.last_tmmbr_value_received, 330000.f, float, "%f");

		end_call(marie, pauline);
	}

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

test_t quality_reporting_tests[] = {
    TEST_NO_TAG("Not used if no config", quality_reporting_not_used_without_config),
    TEST_NO_TAG("Call term session report not sent if call did not start",
                quality_reporting_not_sent_if_call_not_started),
    TEST_NO_TAG("Call term session report not sent if low bandwidth", quality_reporting_not_sent_if_low_bandwidth),
    TEST_NO_TAG("Call term session report invalid if missing mandatory fields", quality_reporting_invalid_report),
    TEST_NO_TAG("Call term session report sent if call ended normally", quality_reporting_at_call_termination),
    TEST_NO_TAG("Interval report if interval is configured", quality_reporting_interval_report),
    TEST_NO_TAG("Interval report if interval is configured with realtime text", quality_reporting_interval_report_rtt),
#ifdef VIDEO_ENABLED
    TEST_NO_TAG("Interval report if interval is configured with video and realtime text",
                quality_reporting_interval_report_video_and_rtt),
    TEST_NO_TAG("Session report sent if video stopped during call", quality_reporting_session_report_if_video_stopped),
#endif // ifdef VIDEO_ENABLED
    TEST_NO_TAG("Sent using custom route", quality_reporting_sent_using_custom_route),
    TEST_NO_TAG("Video bandwidth estimation", video_bandwidth_estimation)};

test_suite_t quality_reporting_test_suite = {"QualityReporting",
                                             NULL,
                                             NULL,
                                             liblinphone_tester_before_each,
                                             liblinphone_tester_after_each,
                                             sizeof(quality_reporting_tests) / sizeof(quality_reporting_tests[0]),
                                             quality_reporting_tests};
