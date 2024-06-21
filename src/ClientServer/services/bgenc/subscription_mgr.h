/*
 * Licensed to Systerel under one or more contributor license
 * agreements. See the NOTICE file distributed with this work
 * for additional information regarding copyright ownership.
 * Systerel licenses this file to you under the Apache
 * License, Version 2.0 (the "License"); you may not use this
 * file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/******************************************************************************

 File Name            : subscription_mgr.h

 Date                 : 08/04/2024 12:58:59

 C Translator Version : tradc Java V1.2 (06/02/2022)

******************************************************************************/

#ifndef _subscription_mgr_h
#define _subscription_mgr_h

/*--------------------------
   Added by the Translator
  --------------------------*/
#include "b2c.h"

/*-----------------
   IMPORTS Clause
  -----------------*/
#include "msg_subscription_create_bs.h"
#include "msg_subscription_delete_bs.h"
#include "msg_subscription_monitored_item.h"
#include "msg_subscription_publish_ack_bs.h"
#include "msg_subscription_set_publishing_mode_bs.h"
#include "subscription_absent_node.h"
#include "subscription_core.h"
#include "subscription_create_monitored_item_it.h"

/*--------------
   SEES Clause
  --------------*/
#include "address_space_itf.h"
#include "channel_mgr.h"
#include "constants.h"
#include "constants_statuscodes_bs.h"
#include "message_in_bs.h"
#include "message_out_bs.h"
#include "request_handle_bs.h"
#include "session_mgr.h"
#include "write_value_pointer_bs.h"

/*------------------------
   INITIALISATION Clause
  ------------------------*/
extern void subscription_mgr__INITIALISATION(void);

/*-------------------------------
   PROMOTES and EXTENDS Clauses
  -------------------------------*/
#define subscription_mgr__is_valid_subscription subscription_core__is_valid_subscription

/*--------------------------
   LOCAL_OPERATIONS Clause
  --------------------------*/
extern void subscription_mgr__create_notification_on_monitored_items_if_data_changed(
   const constants__t_monitoredItemQueue_i subscription_mgr__p_monitoredItemQueue,
   const constants__t_WriteValuePointer_i subscription_mgr__p_old_wv_pointer,
   const constants__t_WriteValuePointer_i subscription_mgr__p_new_wv_pointer);
extern void subscription_mgr__create_notification_on_monitored_items_if_node_changed(
   const constants__t_monitoredItemQueue_i subscription_mgr__p_monitoredItemQueue,
   const t_bool subscription_mgr__p_added);
extern void subscription_mgr__fill_delete_subscriptions_response(
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   const t_bool subscription_mgr__p_has_sub,
   const constants__t_subscription_i subscription_mgr__p_session_sub,
   const t_entier4 subscription_mgr__p_nb_reqs);
extern void subscription_mgr__fill_publish_response_msg(
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   const constants__t_subscription_i subscription_mgr__p_subscription,
   const t_bool subscription_mgr__p_moreNotifs);
extern void subscription_mgr__fill_publish_response_msg_ack_results(
   const constants__t_session_i subscription_mgr__p_session,
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   const t_entier4 subscription_mgr__p_nb_acks);
extern void subscription_mgr__fill_publish_response_msg_available_seq_nums(
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   const constants__t_notifRepublishQueue_i subscription_mgr__republishQueue,
   const t_entier4 subscription_mgr__nb_seq_nums);
extern void subscription_mgr__fill_response_subscription_delete_monitored_items(
   const constants__t_subscription_i subscription_mgr__p_subscription,
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   const t_entier4 subscription_mgr__p_nb_monitored_items);
extern void subscription_mgr__fill_response_subscription_modify_monitored_items(
   const constants__t_endpoint_config_idx_i subscription_mgr__p_endpoint_idx,
   const constants__t_TimestampsToReturn_i subscription_mgr__p_tsToReturn,
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   const t_entier4 subscription_mgr__p_nb_monitored_items);
extern void subscription_mgr__fill_response_subscription_set_monit_mode_monitored_items(
   const constants__t_monitoringMode_i subscription_mgr__p_monitoring_mode,
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   const t_entier4 subscription_mgr__p_nb_monitored_items);
extern void subscription_mgr__fill_set_publishing_mode_response(
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   const t_bool subscription_mgr__p_has_sub,
   const constants__t_subscription_i subscription_mgr__p_session_sub,
   const t_bool subscription_mgr__p_pub_enabled,
   const t_entier4 subscription_mgr__p_nb_reqs);
extern void subscription_mgr__local_check_filtered_subscription_and_monitored_item(
   const constants__t_subscription_i subscription_mgr__p_opt_sub_to_filter,
   const constants__t_monitoredItemId_i subscription_mgr__p_opt_mi_to_fitler,
   const constants__t_subscription_i subscription_mgr__p_sub,
   const constants__t_monitoredItemId_i subscription_mgr__p_mi,
   t_bool * const subscription_mgr__bres);
extern void subscription_mgr__local_create_delete_notification_on_set_monit_mode_change(
   const constants__t_monitoredItemPointer_i subscription_mgr__p_monitoredItemPointer,
   const constants__t_monitoringMode_i subscription_mgr__p_prevMonitMode,
   const constants__t_monitoringMode_i subscription_mgr__p_newMonitMode);
extern void subscription_mgr__local_create_notification_if_node_changed_or_monitMode_enabling(
   const constants__t_monitoredItemPointer_i subscription_mgr__p_monitoredItemPointer,
   const t_bool subscription_mgr__p_added_or_monitMode,
   const t_bool subscription_mgr__p_monitMode_enabling);
extern void subscription_mgr__local_create_notification_on_monitored_item_if_event_selected(
   const constants__t_monitoredItemPointer_i subscription_mgr__p_monitoredItemPointer,
   const constants__t_Event_i subscription_mgr__p_event,
   const constants__t_subscription_i subscription_mgr__p_sub_id,
   const constants__t_monitoredItemId_i subscription_mgr__p_mi_id);
extern void subscription_mgr__local_create_notification_on_monitored_items_if_data_changed(
   const constants__t_monitoredItemPointer_i subscription_mgr__p_monitoredItemPointer,
   const constants__t_WriteValuePointer_i subscription_mgr__p_old_wv_pointer,
   const constants__t_WriteValuePointer_i subscription_mgr__p_new_wv_pointer);
extern void subscription_mgr__local_create_notification_on_monitored_items_if_event_selected(
   const constants__t_monitoredItemQueue_i subscription_mgr__p_monitoredItemQueue,
   const constants__t_Event_i subscription_mgr__p_event,
   const constants__t_subscription_i subscription_mgr__p_sub_id,
   const constants__t_monitoredItemId_i subscription_mgr__p_mi_id);
extern void subscription_mgr__local_treat_create_monitored_item_index(
   const constants__t_session_i subscription_mgr__p_session,
   const constants__t_subscription_i subscription_mgr__p_subscription,
   const constants__t_TimestampsToReturn_i subscription_mgr__p_tsToReturn,
   const constants__t_user_i subscription_mgr__p_user,
   const constants__t_LocaleIds_i subscription_mgr__p_locales,
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   const t_entier4 subscription_mgr__p_index);
extern void subscription_mgr__local_treat_create_monitored_items(
   const constants__t_session_i subscription_mgr__p_session,
   const constants__t_subscription_i subscription_mgr__p_subscription,
   const constants__t_TimestampsToReturn_i subscription_mgr__p_tsToReturn,
   const constants__t_user_i subscription_mgr__p_user,
   const constants__t_LocaleIds_i subscription_mgr__p_locales,
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   const t_entier4 subscription_mgr__p_nb_monitored_items);

/*--------------------
   OPERATIONS Clause
  --------------------*/
extern void subscription_mgr__server_subscription_data_changed(
   const constants__t_WriteValuePointer_i subscription_mgr__p_old_write_value_pointer,
   const constants__t_WriteValuePointer_i subscription_mgr__p_new_write_value_pointer);
extern void subscription_mgr__server_subscription_event_triggered(
   const constants__t_NodeId_i subscription_mgr__p_notifierId,
   const constants__t_Event_i subscription_mgr__p_event,
   const constants__t_subscription_i subscription_mgr__p_sub_id,
   const constants__t_monitoredItemId_i subscription_mgr__p_mi_id,
   t_bool * const subscription_mgr__bres);
extern void subscription_mgr__server_subscription_node_changed(
   const t_bool subscription_mgr__p_node_added,
   const constants__t_NodeId_i subscription_mgr__p_nid);
extern void subscription_mgr__server_subscription_publish_timeout(
   const constants__t_subscription_i subscription_mgr__p_subscription);
extern void subscription_mgr__server_subscription_session_inactive(
   const constants__t_session_i subscription_mgr__p_session,
   const constants__t_sessionState subscription_mgr__p_newSessionState);
extern void subscription_mgr__subscription_mgr_UNINITIALISATION(void);
extern void subscription_mgr__treat_create_subscription_request(
   const constants__t_session_i subscription_mgr__p_session,
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   constants_statuscodes_bs__t_StatusCode_i * const subscription_mgr__StatusCode_service);
extern void subscription_mgr__treat_delete_subscriptions_request(
   const constants__t_session_i subscription_mgr__p_session,
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   constants_statuscodes_bs__t_StatusCode_i * const subscription_mgr__StatusCode_service);
extern void subscription_mgr__treat_modify_subscription_request(
   const constants__t_session_i subscription_mgr__p_session,
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   constants_statuscodes_bs__t_StatusCode_i * const subscription_mgr__StatusCode_service);
extern void subscription_mgr__treat_publishing_mode_request(
   const constants__t_session_i subscription_mgr__p_session,
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   constants_statuscodes_bs__t_StatusCode_i * const subscription_mgr__StatusCode_service);
extern void subscription_mgr__treat_subscription_create_monitored_items_req(
   const constants__t_session_i subscription_mgr__p_session,
   const constants__t_user_i subscription_mgr__p_user,
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   constants_statuscodes_bs__t_StatusCode_i * const subscription_mgr__StatusCode_service);
extern void subscription_mgr__treat_subscription_delete_monitored_items_req(
   const constants__t_session_i subscription_mgr__p_session,
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   constants_statuscodes_bs__t_StatusCode_i * const subscription_mgr__StatusCode_service);
extern void subscription_mgr__treat_subscription_modify_monitored_items_req(
   const constants__t_session_i subscription_mgr__p_session,
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   constants_statuscodes_bs__t_StatusCode_i * const subscription_mgr__StatusCode_service);
extern void subscription_mgr__treat_subscription_publish_request(
   const constants__t_session_i subscription_mgr__p_session,
   const constants__t_msg_header_i subscription_mgr__p_req_header,
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_server_request_handle_i subscription_mgr__p_req_handle,
   const constants__t_request_context_i subscription_mgr__p_req_ctx,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   constants_statuscodes_bs__t_StatusCode_i * const subscription_mgr__StatusCode_service,
   t_bool * const subscription_mgr__async_resp_msg);
extern void subscription_mgr__treat_subscription_republish_request(
   const constants__t_session_i subscription_mgr__p_session,
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   constants_statuscodes_bs__t_StatusCode_i * const subscription_mgr__StatusCode_service);
extern void subscription_mgr__treat_subscription_set_monit_mode_monitored_items_req(
   const constants__t_session_i subscription_mgr__p_session,
   const constants__t_msg_i subscription_mgr__p_req_msg,
   const constants__t_msg_i subscription_mgr__p_resp_msg,
   constants_statuscodes_bs__t_StatusCode_i * const subscription_mgr__StatusCode_service);

#endif
