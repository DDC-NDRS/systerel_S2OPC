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

/** \file
 *
 * \brief Unit tests for SOPC_Event_GetNbVariables and SOPC_Event_ForEachVar consistency.
 */

#include "check_helpers.h"

#include <check.h>

#include "sopc_event.h"
#include "sopc_macros.h"

typedef struct
{
    size_t count;
} EventVarCountCtx;

static void count_event_var_cb(const char* qnPath,
                               SOPC_Variant* var,
                               const SOPC_NodeId* dataType,
                               int32_t valueRank,
                               uintptr_t user_data)
{
    SOPC_UNUSED_ARG(qnPath);
    SOPC_UNUSED_ARG(var);
    SOPC_UNUSED_ARG(dataType);
    SOPC_UNUSED_ARG(valueRank);

    EventVarCountCtx* ctx = (EventVarCountCtx*) user_data;
    ctx->count += 1;
}

START_TEST(test_get_nb_variables_matches_foreach_on_alarm_template)
{
    SOPC_Event* alarmEvent = SOPC_Event_GetInstanceAlarmConditionType();
    EventVarCountCtx ctx = {0};

    ck_assert(NULL != alarmEvent);
    ck_assert(0 < SOPC_Event_GetNbVariables(alarmEvent));
    SOPC_Event_ForEachVar(alarmEvent, &count_event_var_cb, (uintptr_t) &ctx);
    ck_assert_uint_eq(SOPC_Event_GetNbVariables(alarmEvent), ctx.count);
    SOPC_Event_Delete(&alarmEvent);
}
END_TEST

START_TEST(test_create_copy_preserves_nb_variables)
{
    SOPC_Event* alarmEvent = SOPC_Event_GetInstanceAlarmConditionType();
    SOPC_Event* alarmEventCopy = NULL;
    EventVarCountCtx ctx = {0};

    ck_assert(NULL != alarmEvent);
    alarmEventCopy = SOPC_Event_CreateCopy(alarmEvent, false);
    ck_assert(NULL != alarmEventCopy);
    ck_assert_uint_eq(SOPC_Event_GetNbVariables(alarmEvent), SOPC_Event_GetNbVariables(alarmEventCopy));
    SOPC_Event_ForEachVar(alarmEventCopy, &count_event_var_cb, (uintptr_t) &ctx);
    ck_assert_uint_eq(SOPC_Event_GetNbVariables(alarmEventCopy), ctx.count);
    SOPC_Event_Delete(&alarmEventCopy);
    SOPC_Event_Delete(&alarmEvent);
}
END_TEST

Suite* tests_make_suite_event_variables(void)
{
    Suite* s = suite_create("EventVariables");
    TCase* tc = tcase_create("EventVariables");

    tcase_add_test(tc, test_get_nb_variables_matches_foreach_on_alarm_template);
    tcase_add_test(tc, test_create_copy_preserves_nb_variables);
    suite_add_tcase(s, tc);

    return s;
}
