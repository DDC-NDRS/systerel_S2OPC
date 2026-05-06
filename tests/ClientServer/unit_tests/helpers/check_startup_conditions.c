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

/* Description :
 * This file contains the startup conditions tests for the SOPC_PKIProvider_CreateFromStore.
 *
 * The goal is to check the behavior of the function when it is called with a PKI store containing valid and bad
 * certificates, or only bad certificates, and with or without the compilation option
 * S2OPC_START_APP_PKI_WITH_ONE_VALID_CERT.
 *
 * The following tests are implemented :
 *
 *      - test_pki_create_from_store_without_option_with_valid_certs: Start when the compilation option
 * S2OPC_START_APP_PKI_WITH_ONE_VALID_CERT is not set, with a PKI store containing valid and bad certificates. The
 * function PKIProvider_CreateFromStore should return a bad status.
 *
 *      - test_pki_create_from_store_with_option_with_valid_certs_and_bad_certs: Start when the compilation option
 * S2OPC_START_APP_PKI_WITH_ONE_VALID_CERT is set, with a PKI store containing valid and bad certificates. The
 * function PKIProvider_CreateFromStore should return a good status and create the PKI provider every certificates.
 *
 *      - test_pki_create_from_store_with_option_with_only_bad_certs: Start when the compilation option
 * S2OPC_START_APP_PKI_WITH_ONE_VALID_CERT is set, with a PKI store containing only bad certificates. The function
 * PKIProvider_CreateFromStore should return a bad status and not create the PKI provider.
 */

#include "check_helpers.h"

#include <check.h>
#include <stdbool.h>

#include "sopc_enums.h"
#include "sopc_pki_stack.h"

/* DEFINE PKIs */
#define PKI_VALID_CERTS_PLUS_BAD_CERTS TEST_CLIENTSERVER_PKI_DIR "/valid_certs_plus_bad_certs"

#define PKI_ONLY_BAD_CERT TEST_CLIENTSERVER_PKI_DIR "/only_bad_certs"

/* TESTS */
#ifndef S2OPC_START_APP_PKI_WITH_ONE_VALID_CERT
START_TEST(test_pki_create_from_store_without_option_with_valid_certs)
{
    SOPC_PKIProvider* pki = NULL;

    SOPC_ReturnStatus status = SOPC_PKIProvider_CreateFromStore(PKI_VALID_CERTS_PLUS_BAD_CERTS, &pki);

    ck_assert_int_ne(SOPC_STATUS_OK, status);
    ck_assert_ptr_null(pki);
}
END_TEST

#else
START_TEST(test_pki_create_from_store_with_option_with_valid_certs_and_bad_certs)
{
    SOPC_PKIProvider* pki = NULL;

    SOPC_ReturnStatus status = SOPC_PKIProvider_CreateFromStore(PKI_VALID_CERTS_PLUS_BAD_CERTS, &pki);

    ck_assert_int_eq(SOPC_STATUS_OK, status);
    ck_assert_ptr_nonnull(pki);

    SOPC_PKIProvider_Free(&pki);
    ck_assert_ptr_null(pki);
}
END_TEST

START_TEST(test_pki_create_from_store_with_option_with_only_bad_certs)
{
    SOPC_PKIProvider* pki = NULL;

    SOPC_ReturnStatus status = SOPC_PKIProvider_CreateFromStore(PKI_ONLY_BAD_CERT, &pki);

    ck_assert_int_ne(SOPC_STATUS_OK, status);
    ck_assert_ptr_null(pki);
}
END_TEST
#endif

Suite* tests_make_suite_pki_create_from_store(void)
{
    Suite* s = suite_create("PKI CreateFromStore tests");
    TCase* tc_pki = tcase_create("PKI CreateFromStore");

#ifndef S2OPC_START_APP_PKI_WITH_ONE_VALID_CERT
    tcase_add_test(tc_pki, test_pki_create_from_store_without_option_with_valid_certs);
#else
    tcase_add_test(tc_pki, test_pki_create_from_store_with_option_with_valid_certs_and_bad_certs);
    tcase_add_test(tc_pki, test_pki_create_from_store_with_option_with_only_bad_certs);
#endif
    suite_add_tcase(s, tc_pki);

    return s;
}
