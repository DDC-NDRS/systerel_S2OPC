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

 File Name            : address_space_authorization.h

 Date                 : 26/07/2024 08:42:55

 C Translator Version : tradc Java V1.2 (06/02/2022)

******************************************************************************/

#ifndef _address_space_authorization_h
#define _address_space_authorization_h

/*--------------------------
   Added by the Translator
  --------------------------*/
#include "b2c.h"

/*-----------------
   IMPORTS Clause
  -----------------*/
#include "user_authorization_bs.h"

/*-----------------
   EXTENDS Clause
  -----------------*/
#include "address_space_bs.h"
#include "address_space_local.h"

/*--------------
   SEES Clause
  --------------*/
#include "constants.h"
#include "constants_statuscodes_bs.h"
#include "data_value_pointer_bs.h"

/*------------------------
   INITIALISATION Clause
  ------------------------*/
extern void address_space_authorization__INITIALISATION(void);

/*-------------------------------
   PROMOTES and EXTENDS Clauses
  -------------------------------*/
#define address_space_authorization__addNode_AddressSpace_Variable address_space_bs__addNode_AddressSpace_Variable
#define address_space_authorization__addNode_check_valid_node_attributes_type address_space_bs__addNode_check_valid_node_attributes_type
#define address_space_authorization__address_space_bs_UNINITIALISATION address_space_bs__address_space_bs_UNINITIALISATION
#define address_space_authorization__exec_callMethod address_space_bs__exec_callMethod
#define address_space_authorization__gen_addNode_event address_space_bs__gen_addNode_event
#define address_space_authorization__get_AccessLevel address_space_bs__get_AccessLevel
#define address_space_authorization__get_BrowseName address_space_bs__get_BrowseName
#define address_space_authorization__get_DataType address_space_bs__get_DataType
#define address_space_authorization__get_DisplayName address_space_bs__get_DisplayName
#define address_space_authorization__get_EventNotifier address_space_bs__get_EventNotifier
#define address_space_authorization__get_Executable address_space_bs__get_Executable
#define address_space_authorization__get_InputArguments address_space_bs__get_InputArguments
#define address_space_authorization__get_NodeClass address_space_bs__get_NodeClass
#define address_space_authorization__get_Node_RefIndexEnd address_space_bs__get_Node_RefIndexEnd
#define address_space_authorization__get_RefIndex_Reference address_space_bs__get_RefIndex_Reference
#define address_space_authorization__get_Reference_IsForward address_space_bs__get_Reference_IsForward
#define address_space_authorization__get_Reference_ReferenceType address_space_bs__get_Reference_ReferenceType
#define address_space_authorization__get_Reference_TargetNode address_space_bs__get_Reference_TargetNode
#define address_space_authorization__get_TypeDefinition address_space_bs__get_TypeDefinition
#define address_space_authorization__get_ValueRank address_space_bs__get_ValueRank
#define address_space_authorization__get_Value_StatusCode address_space_bs__get_Value_StatusCode
#define address_space_authorization__get_conv_Variant_Type address_space_bs__get_conv_Variant_Type
#define address_space_authorization__get_conv_Variant_ValueRank address_space_bs__get_conv_Variant_ValueRank
#define address_space_authorization__is_AddressSpace_constant address_space_bs__is_AddressSpace_constant
#define address_space_authorization__is_IndexRangeDefined address_space_bs__is_IndexRangeDefined
#define address_space_authorization__is_NodeId_equal address_space_bs__is_NodeId_equal
#define address_space_authorization__is_local_service_treatment address_space_local__is_local_service_treatment
#define address_space_authorization__read_AddressSpace_AccessLevelEx_value address_space_bs__read_AddressSpace_AccessLevelEx_value
#define address_space_authorization__read_AddressSpace_AccessLevel_value address_space_bs__read_AddressSpace_AccessLevel_value
#define address_space_authorization__read_AddressSpace_ArrayDimensions_value address_space_bs__read_AddressSpace_ArrayDimensions_value
#define address_space_authorization__read_AddressSpace_BrowseName_value address_space_bs__read_AddressSpace_BrowseName_value
#define address_space_authorization__read_AddressSpace_ContainsNoLoops_value address_space_bs__read_AddressSpace_ContainsNoLoops_value
#define address_space_authorization__read_AddressSpace_DataTypeDefinition_value address_space_bs__read_AddressSpace_DataTypeDefinition_value
#define address_space_authorization__read_AddressSpace_DataType_value address_space_bs__read_AddressSpace_DataType_value
#define address_space_authorization__read_AddressSpace_DisplayName_value address_space_bs__read_AddressSpace_DisplayName_value
#define address_space_authorization__read_AddressSpace_EventNotifier_value address_space_bs__read_AddressSpace_EventNotifier_value
#define address_space_authorization__read_AddressSpace_Executable_value address_space_bs__read_AddressSpace_Executable_value
#define address_space_authorization__read_AddressSpace_Historizing_value address_space_bs__read_AddressSpace_Historizing_value
#define address_space_authorization__read_AddressSpace_IsAbstract_value address_space_bs__read_AddressSpace_IsAbstract_value
#define address_space_authorization__read_AddressSpace_NodeClass_value address_space_bs__read_AddressSpace_NodeClass_value
#define address_space_authorization__read_AddressSpace_NodeId_value address_space_bs__read_AddressSpace_NodeId_value
#define address_space_authorization__read_AddressSpace_Raw_Node_Value_value address_space_bs__read_AddressSpace_Raw_Node_Value_value
#define address_space_authorization__read_AddressSpace_Symmetric_value address_space_bs__read_AddressSpace_Symmetric_value
#define address_space_authorization__read_AddressSpace_UserAccessLevel_value address_space_bs__read_AddressSpace_UserAccessLevel_value
#define address_space_authorization__read_AddressSpace_UserExecutable_value address_space_bs__read_AddressSpace_UserExecutable_value
#define address_space_authorization__read_AddressSpace_ValueRank_value address_space_bs__read_AddressSpace_ValueRank_value
#define address_space_authorization__read_AddressSpace_Value_value address_space_bs__read_AddressSpace_Value_value
#define address_space_authorization__read_AddressSpace_clear_value address_space_bs__read_AddressSpace_clear_value
#define address_space_authorization__read_AddressSpace_free_variant address_space_bs__read_AddressSpace_free_variant
#define address_space_authorization__readall_AddressSpace_Node address_space_bs__readall_AddressSpace_Node
#define address_space_authorization__set_Value address_space_bs__set_Value
#define address_space_authorization__set_Value_SourceTimestamp address_space_bs__set_Value_SourceTimestamp
#define address_space_authorization__set_Value_StatusCode address_space_bs__set_Value_StatusCode
#define address_space_authorization__set_local_service_treatment address_space_local__set_local_service_treatment
#define address_space_authorization__write_AddressSpace_free_dataValue address_space_bs__write_AddressSpace_free_dataValue

/*--------------------
   OPERATIONS Clause
  --------------------*/
extern void address_space_authorization__get_user_authorization(
   const constants__t_operation_type_i address_space_authorization__p_operation_type,
   const constants__t_NodeId_i address_space_authorization__p_node_id,
   const constants__t_AttributeId_i address_space_authorization__p_attribute_id,
   const constants__t_user_i address_space_authorization__p_user,
   t_bool * const address_space_authorization__p_authorized);
extern void address_space_authorization__has_access_level_executable(
   const constants__t_Node_i address_space_authorization__node,
   t_bool * const address_space_authorization__bres);
extern void address_space_authorization__has_access_level_read(
   const constants__t_Node_i address_space_authorization__node,
   t_bool * const address_space_authorization__bres);
extern void address_space_authorization__has_access_level_write(
   const constants__t_Node_i address_space_authorization__node,
   const constants__t_RawStatusCode address_space_authorization__raw_sc,
   const constants__t_Timestamp address_space_authorization__source_ts,
   t_bool * const address_space_authorization__bres);

#endif