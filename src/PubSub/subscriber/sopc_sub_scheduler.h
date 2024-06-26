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

#ifndef SOPC_SUB_SCHEDULER_H_
#define SOPC_SUB_SCHEDULER_H_

#include "sopc_pubsub_conf.h"
#include "sopc_sub_target_variable.h"

/* TODO: Move me */
typedef enum SOPC_PubSubState
{
    SOPC_PubSubState_Disabled = 0,
    SOPC_PubSubState_Paused = 1,
    SOPC_PubSubState_Operational = 2,
    SOPC_PubSubState_Error = 3
} SOPC_PubSubState;

/* Notification of DSM state change
 * \param pubId    the publisher Id of changed DSM. NULL for a global state change.
 * \param groupId  the writer group Id of the changed DSM. 0 for a global state change.
 * \param writerId the writer Id of changed DSM. 0 for a global state change.
 * \param state    the new DSM subscriber state
 *  */
typedef void SOPC_SubscriberStateChanged_Func(const SOPC_Conf_PublisherId* pubId,
                                              uint16_t groupId,
                                              uint16_t writerId,
                                              SOPC_PubSubState state);

/**
 * @brief Callback to notify gaps in received DataSetMessage sequence number.
 * DataSetMessage is identified by tuple [pubId, groupId, writerId] describe below.
 *
 * @param pubId Publisher Id of the networkMessage
 * @param groupId WriterGroupId to filter dataSetMessage
 * @param writerId dataSetWriterId to filter dataSetMessage
 * @param prevSN previous DataSetMessage sequence number received
 * @param receivedSN current DataSetMessage sequence number received
 */
typedef void SOPC_SubscriberDataSetMessageSNGap_Func(SOPC_Conf_PublisherId pubId,
                                                     uint16_t groupId,
                                                     uint16_t writerId,
                                                     uint16_t prevSN,
                                                     uint16_t receivedSN);

/* Only ::pStateChangedCb callback and ::pSubDisconnectedCb can be NULL */
bool SOPC_SubScheduler_Start(SOPC_PubSubConfiguration* config,
                             SOPC_SubTargetVariableConfig* targetConfig,
                             SOPC_SubscriberStateChanged_Func* pStateChangedCb,
                             SOPC_SubscriberDataSetMessageSNGap_Func sdmSnGapCb,
                             SOPC_PubSub_OnFatalError* pSubDisconnectedCb,
                             int threadPriority);

void SOPC_SubScheduler_Stop(void);

#endif /* SOPC_SUB_SCHEDULER_H_ */
