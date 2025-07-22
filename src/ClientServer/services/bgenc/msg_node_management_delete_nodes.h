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

 File Name            : msg_node_management_delete_nodes.h

 Date                 : 11/02/2025 09:51:26

 C Translator Version : tradc Java V1.2 (06/02/2022)

******************************************************************************/

#ifndef _msg_node_management_delete_nodes_h
#define _msg_node_management_delete_nodes_h

/*--------------------------
   Added by the Translator
  --------------------------*/
#include "b2c.h"

/*-----------------
   IMPORTS Clause
  -----------------*/
#include "msg_node_management_delete_nodes_bs.h"

/*--------------
   SEES Clause
  --------------*/
#include "constants.h"
#include "constants_statuscodes_bs.h"
#include "message_in_bs.h"
#include "message_out_bs.h"

/*----------------------------
   CONCRETE_VARIABLES Clause
  ----------------------------*/
extern t_entier4 msg_node_management_delete_nodes__nb_nodes_to_delete;

/*------------------------
   INITIALISATION Clause
  ------------------------*/
extern void msg_node_management_delete_nodes__INITIALISATION(void);

/*-------------------------------
   PROMOTES and EXTENDS Clauses
  -------------------------------*/
#define msg_node_management_delete_nodes__alloc_msg_delete_nodes_resp_results msg_node_management_delete_nodes_bs__alloc_msg_delete_nodes_resp_results
#define msg_node_management_delete_nodes__getall_delete_node_item_req_params msg_node_management_delete_nodes_bs__getall_delete_node_item_req_params
#define msg_node_management_delete_nodes__setall_msg_delete_nodes_item_resp_params msg_node_management_delete_nodes_bs__setall_msg_delete_nodes_item_resp_params

/*--------------------
   OPERATIONS Clause
  --------------------*/
extern void msg_node_management_delete_nodes__getall_msg_delete_nodes_req_params(
   const constants__t_msg_i msg_node_management_delete_nodes__p_req_msg,
   constants_statuscodes_bs__t_StatusCode_i * const msg_node_management_delete_nodes__p_sc,
   t_entier4 * const msg_node_management_delete_nodes__p_nb_nodes_to_delete);

#endif
