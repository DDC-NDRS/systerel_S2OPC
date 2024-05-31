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

/*------------------------
   Exported Declarations
  ------------------------*/
#include <stdbool.h>
#include <stdio.h>

#include "sopc_secure_channels_api.h"

#include "sopc_assert.h"
#include "sopc_crypto_profiles.h"
#include "sopc_macros.h"
#include "sopc_types.h"

#include "channel_mgr_bs.h"

#include "util_b2c.h"

#include "sopc_services_api_internal.h"
#include "sopc_toolkit_config_internal.h"

#include "channel_mgr_1.h"

/*------------------------
   INITIALISATION Clause
  ------------------------*/
void channel_mgr_bs__INITIALISATION(void) {}

/*--------------------
   OPERATIONS Clause
  --------------------*/
void channel_mgr_bs__is_valid_channel_config_idx(const constants__t_channel_config_idx_i channel_mgr_bs__p_config_idx,
                                                 t_bool* const channel_mgr_bs__bres)
{
    SOPC_SecureChannel_Config* res = NULL;

    if (channel_mgr_bs__p_config_idx > 0 && channel_mgr_bs__p_config_idx <= constants__t_channel_config_idx_i_max)
    {
        res = SOPC_ToolkitClient_GetSecureChannelConfig(channel_mgr_bs__p_config_idx);
        if (NULL == res)
        {
            res = SOPC_ToolkitServer_GetSecureChannelConfig(channel_mgr_bs__p_config_idx);
        }
    }
    else
    {
        SOPC_ASSERT(false);
    }

    if (NULL == res)
    {
        *channel_mgr_bs__bres = false;
    }
    else
    {
        *channel_mgr_bs__bres = true;
    }
}

void channel_mgr_bs__is_valid_endpoint_config_idx(const constants__t_endpoint_config_idx_i channel_mgr_bs__p_config_idx,
                                                  t_bool* const channel_mgr_bs__bres)
{
    SOPC_Endpoint_Config* res = NULL;

    if (channel_mgr_bs__p_config_idx > 0 && channel_mgr_bs__p_config_idx <= constants__t_endpoint_config_idx_i_max)
    {
        res = SOPC_ToolkitServer_GetEndpointConfig(channel_mgr_bs__p_config_idx);
    }
    else
    {
        SOPC_ASSERT(false);
    }

    if (NULL == res)
    {
        *channel_mgr_bs__bres = false;
    }
    else
    {
        *channel_mgr_bs__bres = true;
    }
}

void channel_mgr_bs__prepare_cli_open_secure_channel(
    const constants__t_channel_config_idx_i channel_mgr_bs__p_config_idx,
    const constants__t_reverse_endpoint_config_idx_i channel_mgr_bs__p_reverse_endpoint_config_idx)
{
    SOPC_SecureChannel_Config* config = SOPC_ToolkitClient_GetSecureChannelConfig(channel_mgr_bs__p_config_idx);
    if (NULL != config)
    {
        if (constants__c_reverse_endpoint_config_idx_indet == channel_mgr_bs__p_reverse_endpoint_config_idx)
        {
            SOPC_SecureChannels_EnqueueEvent(SC_CONNECT, channel_mgr_bs__p_config_idx, (uintptr_t) NULL,
                                             (uintptr_t) NULL);
        }
        else
        {
            SOPC_SecureChannels_EnqueueEvent(SC_REVERSE_CONNECT, channel_mgr_bs__p_reverse_endpoint_config_idx,
                                             (uintptr_t) channel_mgr_bs__p_config_idx, (uintptr_t) NULL);
        }
    }
    // else: will be checked in B model in next instruction and open avoided
}

void channel_mgr_bs__finalize_close_secure_channel(
    const constants__t_channel_i channel_mgr_bs__p_channel,
    const constants_statuscodes_bs__t_StatusCode_i channel_mgr_bs__p_statusCode)
{
    SOPC_StatusCode statusCode = SOPC_GoodGenericStatus;
    util_status_code__B_to_C(channel_mgr_bs__p_statusCode, &statusCode);
    SOPC_SecureChannels_EnqueueEvent(SC_DISCONNECT, channel_mgr_bs__p_channel, (uintptr_t) statusCode, 0);
}

void channel_mgr_bs__last_connected_channel_lost(const t_bool channel_mgr_bs__p_clientOnly)
{
    SOPC_EventHandler_Post(SOPC_Services_GetEventHandler(), SE_TO_SE_SC_ALL_DISCONNECTED, 0,
                           (uintptr_t) channel_mgr_bs__p_clientOnly, 0);
}

extern void channel_mgr_bs__define_SecurityPolicy(const constants__t_channel_i channel_mgr_bs__p_channel)
{
    // Nothing to do since config is static, it is only for B model
    SOPC_UNUSED_ARG(channel_mgr_bs__p_channel);
}

extern void channel_mgr_bs__reset_SecurityPolicy(const constants__t_channel_i channel_mgr_bs__channel)
{
    // Nothing to do since config is static, it is only for B model
    SOPC_UNUSED_ARG(channel_mgr_bs__channel);
}

void channel_mgr_bs__get_SecurityPolicy(const constants__t_channel_i channel_mgr_bs__channel,
                                        constants__t_SecurityPolicy* const channel_mgr_bs__secpol)
{
    SOPC_SecureChannel_Config* pSCCfg = NULL;
    uint32_t scConfigIdx = 0;

    channel_mgr_1__get_channel_info(channel_mgr_bs__channel, &scConfigIdx);

    pSCCfg = SOPC_ToolkitServer_GetSecureChannelConfig(scConfigIdx);
    if (pSCCfg == NULL)
    {
        pSCCfg = SOPC_ToolkitClient_GetSecureChannelConfig(scConfigIdx);
    }
    SOPC_ASSERT(pSCCfg != NULL);

    // Note: this conditional branch is only needed for mingw compiler that did not take assert into account
    if (pSCCfg != NULL)
    {
        util_channel__SecurityPolicy_C_to_B(pSCCfg->reqSecuPolicyUri, channel_mgr_bs__secpol);
    }
}

void channel_mgr_bs__channel_do_nothing(const constants__t_channel_i channel_mgr_bs__channel)
{
    SOPC_UNUSED_ARG(channel_mgr_bs__channel);
}
