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

#include <check.h>

#include "check_helpers.h"
#include "libs2opc_common_config.h"
#include "sopc_common.h"

static SOPC_CustomThreadProperties validProps = {.priority = 10, .cpuAffinity = 0};

START_TEST(test_set_thread_configuration_null)
{
    ck_assert_int_eq(SOPC_STATUS_INVALID_PARAMETERS,
                     SOPC_CommonHelper_SetThreadConfiguration(SOPC_THREAD_COMPONENT_SOCKETS, NULL));
}
END_TEST

START_TEST(test_set_thread_configuration_invalid_component_negative)
{
    ck_assert_int_eq(SOPC_STATUS_INVALID_PARAMETERS,
                     SOPC_CommonHelper_SetThreadConfiguration((SOPC_CommonHelper_ThreadComponentEnum) -1, &validProps));
}
END_TEST

START_TEST(test_set_thread_configuration_invalid_component_max)
{
    ck_assert_int_eq(SOPC_STATUS_INVALID_PARAMETERS,
                     SOPC_CommonHelper_SetThreadConfiguration(SOPC_THREAD_COMPONENT_MAX, &validProps));
}
END_TEST

START_TEST(test_set_thread_configuration_ok)
{
    SOPC_Log_Configuration logConfig = SOPC_Common_GetDefaultLogConfiguration();
    SOPC_CustomThreadProperties props = {.priority = 0, .cpuAffinity = -1};
    SOPC_ReturnStatus status = SOPC_STATUS_NOK;

    status = SOPC_CommonHelper_SetThreadConfiguration(SOPC_THREAD_COMPONENT_SOCKETS, &props);
    ck_assert_int_eq(SOPC_STATUS_OK, status);

    status = SOPC_CommonHelper_Initialize(&logConfig, NULL);
    ck_assert_int_eq(SOPC_STATUS_OK, status);

    SOPC_CommonHelper_Clear();
}
END_TEST

START_TEST(test_set_thread_configuration_double_set)
{
    ck_assert_int_eq(SOPC_STATUS_OK,
                     SOPC_CommonHelper_SetThreadConfiguration(SOPC_THREAD_COMPONENT_SERVICES, &validProps));
    ck_assert_int_eq(SOPC_STATUS_INVALID_STATE,
                     SOPC_CommonHelper_SetThreadConfiguration(SOPC_THREAD_COMPONENT_SERVICES, &validProps));
}
END_TEST

Suite* tests_make_suite_common_helper_thread_config(void)
{
    Suite* s = suite_create("Common helper thread configuration");
    TCase* c = tcase_create("Common helper thread configuration");

    tcase_add_test(c, test_set_thread_configuration_null);
    tcase_add_test(c, test_set_thread_configuration_invalid_component_negative);
    tcase_add_test(c, test_set_thread_configuration_invalid_component_max);
    tcase_add_test(c, test_set_thread_configuration_ok);
    tcase_add_test(c, test_set_thread_configuration_double_set);
    suite_add_tcase(s, c);

    return s;
}
