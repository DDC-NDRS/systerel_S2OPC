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
 * \brief Helpers to validate OPC UA service response treatment
 *
 * E.g. Used to detect malformed server responses before correlating request and response
 * arrays by index (see client wrapper security guidelines).
 */

#ifndef LIBS2OPC_RESPONSE_HELPER_H_
#define LIBS2OPC_RESPONSE_HELPER_H_

#include "sopc_types.h"

/**
 * \brief Opaque snapshot of request count fields used after the request is sent.
 *
 * Created by ::SOPC_ServiceResponse_CreateRequestContext before handing the request to the
 * toolkit (which takes ownership and frees it). Freed by ::SOPC_ServiceResponse_ValidateResultCount
 * after validation, or by ::SOPC_ServiceResponse_DeleteRequestContext when validation is skipped.
 */
typedef struct SOPC_ServiceResponse_ReqContext SOPC_ServiceResponse_ReqContext;

/**
 * \brief Returns true when \p responseCount equals \p expectedCount.
 *
 * Logs a trace error on mismatch. Both counts shall be non-negative for a match.
 *
 * \param responseCount   Number of elements in the service response array
 * \param expectedCount   Expected number of elements from the request
 * \param contextLabel    Short label used in log messages (service or field name)
 *
 * \return true if counts match, false otherwise
 */
bool SOPC_ServiceResponse_CheckResultCountMatches(int32_t responseCount,
                                                  int32_t expectedCount,
                                                  const char* contextLabel);

/**
 * \brief Captures request count fields needed for post-send response validation.
 *
 * Shall be called on the request before it is passed to a service call (the toolkit frees
 * the request once sent).
 *
 * \param request   The service request (encodeable object)
 *
 * \return A new context, or NULL if \p request is NULL, the request type is not supported,
 *         or memory allocation failed
 */
SOPC_ServiceResponse_ReqContext* SOPC_ServiceResponse_CreateRequestContext(const void* request);

/**
 * \brief Frees a request context created by ::SOPC_ServiceResponse_CreateRequestContext.
 *
 * Use when the context is no longer needed and ::SOPC_ServiceResponse_ValidateResultCount shall
 * not be called (e.g. send failure before a response is received).
 *
 * \param[in, out] ppReqCtx  In: context to free. Out: NULL.
 */
void SOPC_ServiceResponse_DeleteRequestContext(SOPC_ServiceResponse_ReqContext** ppReqCtx);

/**
 * \brief Validates that a service response has one result entry per request operation.
 *
 * Supported request/response pairs are the OPC UA services exposing a \c NoOfResults
 * array aligned with the request operation count (Read, Write, Browse, Call, etc.).
 * Some services may also run additional sub-result count checks (e.g. Browse references,
 * Call InputArgumentResults / OutputArguments).
 *
 * Takes ownership of \p *ppReqCtx: the context is always freed before return and
 * \p *ppReqCtx is set to NULL.
 *
 * \param[in, out] ppReqCtx  In: context from ::SOPC_ServiceResponse_CreateRequestContext.
 *                           Out: always NULL after the call.
 * \param response  The matching response for the request (encodeable object)
 *
 * \return SOPC_STATUS_OK if counts match,
 *         SOPC_STATUS_NOK if counts differ or are negative,
 *         SOPC_STATUS_INVALID_STATE if a sub-result count exceeds configured limits (e.g. Browse references,
 *         Call method argument lists),
 *         SOPC_STATUS_WOULD_BLOCK if the global service result is not good,
 *         SOPC_STATUS_INVALID_PARAMETERS if \p ppReqCtx, \p *ppReqCtx or \p response is NULL, or types mismatch
 */
SOPC_ReturnStatus SOPC_ServiceResponse_ValidateResultCount(SOPC_ServiceResponse_ReqContext** ppReqCtx,
                                                           const void* response);

#endif /* LIBS2OPC_RESPONSE_HELPER_H_ */
