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

#include "service_register_server2_set_bs.h"

#include "service_discovery_servers_internal.h"
#include "sopc_assert.h"
#include "sopc_date_time.h"
#include "sopc_macros.h"
#include "sopc_mem_alloc.h"
#include "sopc_singly_linked_list.h"

static SOPC_SLinkedList* registeredServer2List = NULL;
static SOPC_SLinkedListIterator registeredServer2ListIt;
static SOPC_Value_Timestamp resetTime = {0, 0};

/*------------------------
   INITIALISATION Clause
  ------------------------*/
void service_register_server2_set_bs__INITIALISATION(void)
{
    registeredServer2List = SOPC_SLinkedList_Create(0);
    SOPC_ASSERT(NULL != registeredServer2List);
    registeredServer2ListIt = NULL;
    resetTime.timestamp = SOPC_Time_GetCurrentTimeUTC();
}

/*--------------------
   OPERATIONS Clause
  --------------------*/
void service_register_server2_set_bs__add_to_registered_server2_set(
    const constants__t_RegisteredServer_i service_register_server2_set_bs__p_registered_server,
    const constants__t_MdnsDiscoveryConfig_i service_register_server2_set_bs__p_mdns_config,
    const t_entier4 service_register_server2_set_bs__p_recordId,
    t_bool* const service_register_server2_set_bs__alloc_success)
{
    SOPC_ASSERT(service_register_server2_set_bs__p_recordId >= 0);
    *service_register_server2_set_bs__alloc_success = false;
    SOPC_RegisterServer2Record_Internal* newRecord = SOPC_Calloc(1, sizeof(*newRecord));
    if (NULL == newRecord)
    {
        return;
    }
    void* added = (void*) SOPC_SLinkedList_Append(
        registeredServer2List, (uint32_t) service_register_server2_set_bs__p_recordId, (uintptr_t) newRecord);
    if (added == newRecord)
    {
        // Move RegisteredServer
        OpcUa_RegisteredServer_Initialize(&newRecord->registeredServer);
        SOPC_ReturnStatus status =
            SOPC_EncodeableObject_Copy(&OpcUa_RegisteredServer_EncodeableType, &newRecord->registeredServer,
                                       service_register_server2_set_bs__p_registered_server);
        if (SOPC_STATUS_OK == status)
        {
            OpcUa_MdnsDiscoveryConfiguration_Initialize(&newRecord->mDNSconfig);
            status = SOPC_EncodeableObject_Copy(&OpcUa_MdnsDiscoveryConfiguration_EncodeableType,
                                                &newRecord->mDNSconfig, service_register_server2_set_bs__p_mdns_config);
        }

        if (SOPC_STATUS_OK == status)
        {
            // Move mDNSconfig
            newRecord->recordId = service_register_server2_set_bs__p_recordId;

            *service_register_server2_set_bs__alloc_success = true;
        }
        else
        {
            OpcUa_RegisteredServer_Clear(&newRecord->registeredServer);
            OpcUa_MdnsDiscoveryConfiguration_Clear(&newRecord->mDNSconfig);

            void* removed = (void*) SOPC_SLinkedList_RemoveFromId(
                registeredServer2List, (uint32_t) service_register_server2_set_bs__p_recordId);
            SOPC_ASSERT(added == removed);
            SOPC_Free(newRecord);
        }
    }
    else
    {
        SOPC_Free(newRecord);
    }
}

void service_register_server2_set_bs__init_iter_registered_server2_set(
    t_bool* const service_register_server2_set_bs__continue)
{
    registeredServer2ListIt = SOPC_SLinkedList_GetIterator(registeredServer2List);
    if (NULL != registeredServer2ListIt)
    {
        *service_register_server2_set_bs__continue = SOPC_SLinkedList_HasNext(&registeredServer2ListIt);
    }
    else
    {
        *service_register_server2_set_bs__continue = false;
    }
}

void service_register_server2_set_bs__clear_iter_registered_server2_set(void)
{
    registeredServer2ListIt = NULL;
}

void service_register_server2_set_bs__continue_iter_registered_server2_set(
    t_bool* const service_register_server2_set_bs__continue,
    constants__t_RegisteredServer2Info_i* const service_register_server2_set_bs__p_registeredServerInfo)
{
    SOPC_ASSERT(NULL != registeredServer2ListIt);
    *service_register_server2_set_bs__p_registeredServerInfo =
        (constants__t_RegisteredServer2Info_i) SOPC_SLinkedList_Next(&registeredServer2ListIt);
    *service_register_server2_set_bs__continue = SOPC_SLinkedList_HasNext(&registeredServer2ListIt);
}

void service_register_server2_set_bs__get_registered_server2_mdns_config(
    const constants__t_RegisteredServer2Info_i service_register_server2_set_bs__p_registeredServerInfo,
    constants__t_MdnsDiscoveryConfig_i* const service_register_server2_set_bs__p_mdns_config)
{
    *service_register_server2_set_bs__p_mdns_config =
        &service_register_server2_set_bs__p_registeredServerInfo->mDNSconfig;
}

void service_register_server2_set_bs__get_registered_server2_recordId(
    const constants__t_RegisteredServer2Info_i service_register_server2_set_bs__p_registeredServerInfo,
    t_entier4* const service_register_server2_set_bs__p_recordId)
{
    *service_register_server2_set_bs__p_recordId = service_register_server2_set_bs__p_registeredServerInfo->recordId;
}

void service_register_server2_set_bs__get_registered_server2_registered_server(
    const constants__t_RegisteredServer2Info_i service_register_server2_set_bs__p_registeredServerInfo,
    constants__t_RegisteredServer_i* const service_register_server2_set_bs__p_registered_server)
{
    *service_register_server2_set_bs__p_registered_server =
        &service_register_server2_set_bs__p_registeredServerInfo->registeredServer;
}

static void freeRecord(uint32_t id, uintptr_t val)
{
    SOPC_UNUSED_ARG(id);
    SOPC_RegisterServer2Record_Internal* record = (SOPC_RegisterServer2Record_Internal*) val;

    if (NULL != record)
    {
        OpcUa_RegisteredServer_Clear(&record->registeredServer);
        OpcUa_MdnsDiscoveryConfiguration_Clear(&record->mDNSconfig);
    }
    SOPC_Free(record);
}

void service_register_server2_set_bs__remove_from_registered_server2_set(
    const t_entier4 service_register_server2_set_bs__p_recordId)
{
    SOPC_ASSERT(service_register_server2_set_bs__p_recordId >= 0);
    SOPC_RegisterServer2Record_Internal* record = (SOPC_RegisterServer2Record_Internal*) SOPC_SLinkedList_RemoveFromId(
        registeredServer2List, (uint32_t) service_register_server2_set_bs__p_recordId);
    freeRecord(0, (uintptr_t) record);
}

void service_register_server2_set_bs__reset_registered_server2_set(void)
{
    SOPC_SLinkedList_Apply(registeredServer2List, freeRecord);
    SOPC_SLinkedList_Clear(registeredServer2List);
    resetTime.timestamp = SOPC_Time_GetCurrentTimeUTC();
}

void service_register_server2_set_bs__get_card_register2_set(t_entier4* const service_register_server2_set_bs__card_set)
{
    uint32_t length = SOPC_SLinkedList_GetLength(registeredServer2List);
    if (length > INT32_MAX)
    {
        *service_register_server2_set_bs__card_set = INT32_MAX;
    }
    else
    {
        *service_register_server2_set_bs__card_set = (int32_t) length;
    }
}

void service_register_server2_set_bs__get_registered_server2_counter_reset_time(
    constants__t_Timestamp* const service_register_server2_set_bs__p_timestamp)
{
    *service_register_server2_set_bs__p_timestamp = resetTime;
}

void service_register_server2_set_bs__service_register_server2_set_bs_UNINITIALISATION(void)
{
    if (NULL != registeredServer2List)
    {
        SOPC_SLinkedList_Apply(registeredServer2List, freeRecord);
    }
    SOPC_SLinkedList_Delete(registeredServer2List);
    registeredServer2List = NULL;
}
