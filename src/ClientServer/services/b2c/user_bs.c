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

#include "user_bs.h"
#include "sopc_assert.h"
#include "sopc_builtintypes.h"
#include "sopc_enum_types.h"
#include "sopc_macros.h"

/*--------------
   SEES Clause
  --------------*/
#include "constants.h"
#include "sopc_service_call_context.h"
#include "sopc_user.h"

#include "app_cb_call_context_internal.h"

/*------------------------
   INITIALISATION Clause
  ------------------------*/
void user_bs__INITIALISATION(void)
{ /*Translated from B but an initialization is not needed from this module.*/
}

/*--------------------
   OPERATIONS Clause
  --------------------*/

void user_bs__are_application_uri_equal(const constants__t_user_i user_bs__p_user,
                                        const constants__t_Criteria_i user_bs__p_cli_app_uri,
                                        t_bool* const user_bs__p_bres)
{
    /* Spec: This criteria type is used if a Role should be granted to a Session
       for Application Authentication with Anonymous UserIdentityToken.*/
    SOPC_UNUSED_ARG(user_bs__p_user); // B model checks for anonymous constraint
    *user_bs__p_bres = false;
    const SOPC_CallContext* callCtx = SOPC_CallContext_GetCurrent();
    OpcUa_MessageSecurityMode secuMode = SOPC_CallContext_GetSecurityMode(callCtx);

    /* Spec: The Client Certificate shall be trusted by the Server and the Session shall
       use at least a signed communication channel. */
    if (OpcUa_MessageSecurityMode_Sign == secuMode || OpcUa_MessageSecurityMode_SignAndEncrypt == secuMode)
    {
        /* Spec: The criteria is the ApplicationUri from the Client Certificate used for the Session.
         * Note: equality of ApplicationUri is checked server_create_session_req_do_crypto */
        const OpcUa_ApplicationDescription* cliDesc = SOPC_CallContext_GetClientApplicationDesc(callCtx);
        SOPC_ASSERT(NULL != cliDesc);
        int32_t comparison = -1;
        SOPC_ReturnStatus status =
            SOPC_String_Compare(&cliDesc->ApplicationUri, user_bs__p_cli_app_uri, false, &comparison);
        *user_bs__p_bres = (0 == comparison && SOPC_STATUS_OK == status);
    }
}

void user_bs__are_username_equal(const constants__t_user_i user_bs__p_user,
                                 const constants__t_Criteria_i user_bs__p_username,
                                 t_bool* const user_bs__b_res)
{
    const SOPC_User* user = SOPC_UserWithAuthorization_GetUser(user_bs__p_user);
    SOPC_ASSERT(SOPC_User_IsUsername(user));
    const SOPC_String* username = SOPC_User_GetUsername(user);
    int32_t comparison = -1;
    SOPC_ReturnStatus status = SOPC_String_Compare(username, user_bs__p_username, false, &comparison);
    *user_bs__b_res = (0 == comparison && SOPC_STATUS_OK == status);
}

void user_bs__are_certificate_tb_equal(const constants__t_user_i user_bs__p_user,
                                       const constants__t_Criteria_i user_bs__p_thumbprint,
                                       t_bool* const user_bs__b_res)
{
    const SOPC_User* user = SOPC_UserWithAuthorization_GetUser(user_bs__p_user);
    SOPC_ASSERT(SOPC_User_IsCertificate(user));
    const SOPC_String* certTb = SOPC_User_GetCertificate_Thumbprint(user);
    int32_t comparison = -1;
    SOPC_ReturnStatus status = SOPC_String_Compare(certTb, user_bs__p_thumbprint, false, &comparison);
    *user_bs__b_res = (0 == comparison && SOPC_STATUS_OK == status);
}

void user_bs__is_anonymous(const constants__t_user_i user_bs__p_user, t_bool* const user_bs__b_res)
{
    SOPC_ASSERT(NULL != user_bs__b_res);
    const SOPC_User* user = SOPC_UserWithAuthorization_GetUser(user_bs__p_user);
    *user_bs__b_res = SOPC_User_IsAnonymous(user);
}

void user_bs__is_certificate(const constants__t_user_i user_bs__p_user, t_bool* const user_bs__b_res)
{
    SOPC_ASSERT(NULL != user_bs__b_res);
    const SOPC_User* user = SOPC_UserWithAuthorization_GetUser(user_bs__p_user);
    *user_bs__b_res = SOPC_User_IsCertificate(user);
}

void user_bs__is_username(const constants__t_user_i user_bs__p_user, t_bool* const user_bs__b_res)
{
    SOPC_ASSERT(NULL != user_bs__b_res);
    const SOPC_User* user = SOPC_UserWithAuthorization_GetUser(user_bs__p_user);
    *user_bs__b_res = SOPC_User_IsUsername(user);
}
