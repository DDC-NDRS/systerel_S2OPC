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

#ifndef SOPC_MONITORED_ITEM_POINTER_IMPL_H_
#define SOPC_MONITORED_ITEM_POINTER_IMPL_H_

#include "constants.h"
#include "sopc_numeric_range.h"

typedef struct SOPC_InternalMonitoredItemFilterCtx
{
    bool isDataFilter;
    union
    {
        struct
        {
            OpcUa_DataChangeFilter dataFilter;
            double filterAbsoluteDeadbandContext;
            SOPC_Variant* lastCachedValueForFilter;
        } Data;
        struct
        {
            OpcUa_EventFilter* eventFilter;
            char** qnPathStrSelectClauses;               // eventFilter->NoOfSelectClauses clauses
            SOPC_NumericRange** indexRangeSelectClauses; // eventFilter->NoOfSelectClauses clauses
            SOPC_NodeId whereClauseTypeId; // Set to non-NULL id if eventFilter->WhereClause.Element[0] was a valid of
                                           // OfType expression
        } Event;
    } Filter;
} SOPC_InternalMonitoredItemFilterCtx;

typedef struct SOPC_InternalMonitoredItem
{
    uint32_t monitoredItemId;
    constants__t_subscription_i subId;
    constants__t_NodeId_i nid;
    constants__t_AttributeId_i aid;
    constants__t_IndexRange_i indexRangeString;
    constants__t_TimestampsToReturn_i timestampToReturn;
    constants__t_monitoringMode_i monitoringMode;
    constants__t_client_handle_i clientHandle;
    SOPC_NumericRange* indexRange;
    SOPC_InternalMonitoredItemFilterCtx* filterCtx;
    bool discardOldest;
    int32_t queueSize;
    SOPC_SLinkedList* notifQueue;
    bool queueOverflowEventTriggered;
} SOPC_InternalMonitoredItem;

#endif /* SOPC_MONITORED_ITEM_POINTER_IMPL_H_ */
