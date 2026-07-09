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
 * \brief Unit tests for libs2opc_response_helper service response validation.
 */

#include "check_helpers.h"

#include <check.h>

#include "libs2opc_response_helper.h"

#include "opcua_statuscodes.h"
#include "sopc_builtintypes.h"

START_TEST(test_check_result_count_matches)
{
    ck_assert(SOPC_ServiceResponse_CheckResultCountMatches(2, 2, "test"));
    ck_assert(!SOPC_ServiceResponse_CheckResultCountMatches(3, 2, "test"));
    ck_assert(!SOPC_ServiceResponse_CheckResultCountMatches(-1, 2, "test"));
}
END_TEST

START_TEST(test_validate_read_result_count)
{
    OpcUa_ReadRequest req;
    OpcUa_ReadResponse resp;
    OpcUa_CreateSessionRequest unsupportedReq;
    SOPC_ServiceResponse_ReqContext* reqCtx = NULL;

    OpcUa_ReadRequest_Initialize(&req);
    OpcUa_ReadResponse_Initialize(&resp);
    OpcUa_CreateSessionRequest_Initialize(&unsupportedReq);

    // Request and response number of elements matching (2)
    req.NoOfNodesToRead = 2;
    resp.NoOfResults = 2;
    reqCtx = SOPC_ServiceResponse_CreateRequestContext(&req);
    ck_assert(NULL != reqCtx);
    ck_assert_int_eq(SOPC_STATUS_OK, SOPC_ServiceResponse_ValidateResultCount(&reqCtx, &resp));
    ck_assert(NULL == reqCtx);

    // Response number of elements not matching (1 instead of 2)
    resp.NoOfResults = 1;
    reqCtx = SOPC_ServiceResponse_CreateRequestContext(&req);
    ck_assert(NULL != reqCtx);
    ck_assert_int_eq(SOPC_STATUS_NOK, SOPC_ServiceResponse_ValidateResultCount(&reqCtx, &resp));
    ck_assert(NULL == reqCtx);

    // Tests for invalid parameters
    ck_assert_int_eq(SOPC_STATUS_INVALID_PARAMETERS, SOPC_ServiceResponse_ValidateResultCount(NULL, &resp));
    ck_assert(NULL == SOPC_ServiceResponse_CreateRequestContext(&unsupportedReq));

    // Tests for context deletion
    reqCtx = SOPC_ServiceResponse_CreateRequestContext(&req);
    ck_assert(NULL != reqCtx);
    SOPC_ServiceResponse_DeleteRequestContext(&reqCtx);
    ck_assert(NULL == reqCtx);

    OpcUa_ReadRequest_Clear(&req);
    OpcUa_ReadResponse_Clear(&resp);
    OpcUa_CreateSessionRequest_Clear(&unsupportedReq);
}
END_TEST

START_TEST(test_validate_browse_reference_count)
{
    OpcUa_BrowseRequest req;
    OpcUa_BrowseResponse resp;
    OpcUa_BrowseResult results[1];
    SOPC_ServiceResponse_ReqContext* reqCtx = NULL;

    OpcUa_BrowseRequest_Initialize(&req);
    OpcUa_BrowseResponse_Initialize(&resp);
    OpcUa_BrowseResult_Initialize(&results[0]);

    // Test specific behavior for browse services which checks the max references constraint compliance
    req.NoOfNodesToBrowse = 1;
    req.RequestedMaxReferencesPerNode = 10;
    resp.NoOfResults = 1;
    resp.Results = results;
    results[0].NoOfReferences = 10;
    reqCtx = SOPC_ServiceResponse_CreateRequestContext(&req);
    ck_assert(NULL != reqCtx);
    ck_assert_int_eq(SOPC_STATUS_OK, SOPC_ServiceResponse_ValidateResultCount(&reqCtx, &resp));
    ck_assert(NULL == reqCtx);

    // Test with non-compliant response (11 references instead of 10 maximum requested)
    results[0].NoOfReferences = 11;
    reqCtx = SOPC_ServiceResponse_CreateRequestContext(&req);
    ck_assert(NULL != reqCtx);
    ck_assert_int_eq(SOPC_STATUS_INVALID_STATE, SOPC_ServiceResponse_ValidateResultCount(&reqCtx, &resp));
    ck_assert(NULL == reqCtx);

    // No maximum number of references requested => do not check the number of references
    req.RequestedMaxReferencesPerNode = 0;
    results[0].NoOfReferences = 1000;
    reqCtx = SOPC_ServiceResponse_CreateRequestContext(&req);
    ck_assert(NULL != reqCtx);
    ck_assert_int_eq(SOPC_STATUS_OK, SOPC_ServiceResponse_ValidateResultCount(&reqCtx, &resp));
    ck_assert(NULL == reqCtx);

    OpcUa_BrowseResult_Clear(&results[0]);
    resp.Results = NULL;
    resp.NoOfResults = 0;
    OpcUa_BrowseRequest_Clear(&req);
    OpcUa_BrowseResponse_Clear(&resp);
}
END_TEST

START_TEST(test_validate_call_method_argument_counts)
{
    OpcUa_CallRequest req;
    OpcUa_CallResponse resp;
    OpcUa_CallMethodRequest methodsToCall[1];
    OpcUa_CallMethodResult results[1];
    SOPC_ServiceResponse_ReqContext* reqCtx = NULL;

    OpcUa_CallRequest_Initialize(&req);
    OpcUa_CallResponse_Initialize(&resp);
    OpcUa_CallMethodRequest_Initialize(&methodsToCall[0]);
    OpcUa_CallMethodResult_Initialize(&results[0]);

    req.NoOfMethodsToCall = 1;
    req.MethodsToCall = methodsToCall;
    methodsToCall[0].NoOfInputArguments = 2;
    resp.NoOfResults = 1;
    resp.Results = results;

    // Test specific behavior for method call results
    results[0].StatusCode = SOPC_GoodGenericStatus;
    results[0].NoOfInputArgumentResults = 0;
    results[0].NoOfOutputArguments = 1;
    reqCtx = SOPC_ServiceResponse_CreateRequestContext(&req);
    ck_assert(NULL != reqCtx);
    ck_assert_int_eq(SOPC_STATUS_OK, SOPC_ServiceResponse_ValidateResultCount(&reqCtx, &resp));
    ck_assert(NULL == reqCtx);

    // BadInvalidArgument => 1 result per input argument
    results[0].StatusCode = OpcUa_BadInvalidArgument;
    results[0].NoOfInputArgumentResults = 2;
    results[0].NoOfOutputArguments = 0;
    reqCtx = SOPC_ServiceResponse_CreateRequestContext(&req);
    ck_assert(NULL != reqCtx);
    ck_assert_int_eq(SOPC_STATUS_OK, SOPC_ServiceResponse_ValidateResultCount(&reqCtx, &resp));
    ck_assert(NULL == reqCtx);

    // Test when number of input arg results is invalid
    results[0].NoOfInputArgumentResults = 1;
    reqCtx = SOPC_ServiceResponse_CreateRequestContext(&req);
    ck_assert(NULL != reqCtx);
    ck_assert_int_eq(SOPC_STATUS_INVALID_STATE, SOPC_ServiceResponse_ValidateResultCount(&reqCtx, &resp));
    ck_assert(NULL == reqCtx);

    // Not BadInvalidArgument => no input argument result expected
    results[0].StatusCode = OpcUa_BadNodeIdUnknown;
    results[0].NoOfInputArgumentResults = 0;
    results[0].NoOfOutputArguments = 0;
    reqCtx = SOPC_ServiceResponse_CreateRequestContext(&req);
    ck_assert(NULL != reqCtx);
    ck_assert_int_eq(SOPC_STATUS_OK, SOPC_ServiceResponse_ValidateResultCount(&reqCtx, &resp));
    ck_assert(NULL == reqCtx);

    // Test with unexpected input argument result for BadNodeId
    results[0].NoOfInputArgumentResults = 1;
    reqCtx = SOPC_ServiceResponse_CreateRequestContext(&req);
    ck_assert(NULL != reqCtx);
    ck_assert_int_eq(SOPC_STATUS_INVALID_STATE, SOPC_ServiceResponse_ValidateResultCount(&reqCtx, &resp));
    ck_assert(NULL == reqCtx);

    // Test with unexpected number of output argument for BadNodeId (0 expected)
    results[0].NoOfInputArgumentResults = 0;
    results[0].NoOfOutputArguments = 1;
    reqCtx = SOPC_ServiceResponse_CreateRequestContext(&req);
    ck_assert(NULL != reqCtx);
    ck_assert_int_eq(SOPC_STATUS_INVALID_STATE, SOPC_ServiceResponse_ValidateResultCount(&reqCtx, &resp));
    ck_assert(NULL == reqCtx);

    // Test unexpected input argument results when return status is good
    results[0].StatusCode = SOPC_GoodGenericStatus;
    results[0].NoOfInputArgumentResults = 3;
    results[0].NoOfOutputArguments = 0;
    reqCtx = SOPC_ServiceResponse_CreateRequestContext(&req);
    ck_assert(NULL != reqCtx);
    ck_assert_int_eq(SOPC_STATUS_INVALID_STATE, SOPC_ServiceResponse_ValidateResultCount(&reqCtx, &resp));
    ck_assert(NULL == reqCtx);

    OpcUa_CallMethodResult_Clear(&results[0]);
    resp.Results = NULL;
    resp.NoOfResults = 0;
    req.MethodsToCall = NULL;
    req.NoOfMethodsToCall = 0;
    OpcUa_CallRequest_Clear(&req);
    OpcUa_CallResponse_Clear(&resp);
}
END_TEST

Suite* tests_make_suite_response_helper(void)
{
    Suite* s = suite_create("ResponseHelper");
    TCase* tc = tcase_create("ResponseHelper");

    tcase_add_test(tc, test_check_result_count_matches);
    tcase_add_test(tc, test_validate_read_result_count);
    tcase_add_test(tc, test_validate_browse_reference_count);
    tcase_add_test(tc, test_validate_call_method_argument_counts);
    suite_add_tcase(s, tc);

    return s;
}
