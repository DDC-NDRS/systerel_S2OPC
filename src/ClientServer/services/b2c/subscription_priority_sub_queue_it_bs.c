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

#include "subscription_priority_sub_queue_it_bs.h"

#include "sopc_assert.h"
#include "sopc_macros.h"
#include "sopc_mem_alloc.h"

/*--------------
   SEES Clause
  --------------*/
#include "constants.h"
#include "sopc_singly_linked_list.h"

/*------------------------
   INITIALISATION Clause
  ------------------------*/
void subscription_priority_sub_queue_it_bs__INITIALISATION(void)
{ /*Translated from B but an initialisation is not needed in this module.*/
}

/*--------------------
   OPERATIONS Clause
  --------------------*/
void subscription_priority_sub_queue_it_bs__continue_iter_subscription_priority_queue(
    const constants__t_prioritySubQueueIterator_i subscription_priority_sub_queue_it_bs__p_iterator,
    t_bool* const subscription_priority_sub_queue_it_bs__p_continue,
    constants__t_subscription_i* const subscription_priority_sub_queue_it_bs__p_subscription,
    constants__t_prioritySubQueueIterator_i* const subscription_priority_sub_queue_it_bs__p_iterator_updated)
{
    SOPC_SLinkedListIterator it = subscription_priority_sub_queue_it_bs__p_iterator;
    *subscription_priority_sub_queue_it_bs__p_subscription = (constants__t_subscription_i) SOPC_SLinkedList_Next(&it);
    *subscription_priority_sub_queue_it_bs__p_continue = SOPC_SLinkedList_HasNext(&it);
    *subscription_priority_sub_queue_it_bs__p_iterator_updated = it;
}

void subscription_priority_sub_queue_it_bs__init_prio_subscriptions_queue_iterator(
    const constants__t_prioritySubQueue_i subscription_priority_sub_queue_it_bs__p_queue,
    constants__t_prioritySubQueueIterator_i* const subscription_priority_sub_queue_it_bs__iterator)
{
    *subscription_priority_sub_queue_it_bs__iterator =
        SOPC_SLinkedList_GetIterator(subscription_priority_sub_queue_it_bs__p_queue);
}
