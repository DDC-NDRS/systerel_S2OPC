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

#include "sopc_pubsub_helpers.h"

#include <string.h>

#include "sopc_assert.h"
#include "sopc_helper_string.h"
#include "sopc_helper_uri.h"
#include "sopc_logger.h"
#include "sopc_mem_alloc.h"
#include "sopc_pubsub_protocol.h"

#define TCP_PREFIX "opc.tcp://"

static bool SOPC_Internal_PubSubHelpers_ParseAddressUDP(const char* address,
                                                        SOPC_Socket_AddressInfo** multicastAddr,
                                                        char** port)
{
    SOPC_ASSERT(NULL != port);
    SOPC_ASSERT(NULL != multicastAddr);

    size_t hostLen, portIdx, portLen;
    bool result = SOPC_Helper_URI_ParseUri_WithPrefix(UADP_PREFIX, address, &hostLen, &portIdx, &portLen);
    if (result)
    {
        char* ip = SOPC_Calloc(hostLen + 1, sizeof(*ip));
        char* ipRes = NULL;
        *port = SOPC_Calloc(portLen + 1, sizeof(**port));
        char* portRes = NULL;
        if (NULL != ip && NULL != *port)
        {
            ipRes = strncpy(ip, address + strlen(UADP_PREFIX), hostLen);
            portRes = strncpy(*port, address + portIdx, portLen);
        }

        if (NULL != ip && NULL != *port && ip == ipRes && *port == portRes)
        {
            ipRes[hostLen] = '\0';
            portRes[portLen] = '\0';
            // TODO: detect IPv4 / IPv6 addresses ? => force IPv4
            *multicastAddr = SOPC_UDP_SocketAddress_Create(false, ip, *port);
        }

        SOPC_Free(ip);
    }

    return (NULL != *multicastAddr);
}

bool SOPC_PubSubHelpers_ParseAddressUDP(const char* address, SOPC_Socket_AddressInfo** multicastAddr)
{
    char* port = NULL;
    bool result = SOPC_Internal_PubSubHelpers_ParseAddressUDP(address, multicastAddr, &port);
    SOPC_Free(port);
    return result;
}

bool SOPC_Helper_URI_ParseUri_WithPrefix(const char* prefix,
                                         const char* uri,
                                         size_t* hostnameLength,
                                         size_t* portIdx,
                                         size_t* portLength)
{
    bool result = false;
    size_t idx = 0;
    bool isPort = false;
    bool endOfPort = false;
    bool hasPort = false;
    bool hasName = false;
    bool invalid = false;
    bool startIPv6 = false;

    if (NULL == prefix || NULL == uri || NULL == hostnameLength || NULL == portIdx || NULL == portLength)
    {
        return false;
    }
    // We only accept UADP or MQTT prefix in this function
    if (0 != strncmp(prefix, UADP_PREFIX, strlen(UADP_PREFIX)) &&
        0 != strncmp(prefix, MQTT_PREFIX, strlen(MQTT_PREFIX)) && 0 != strncmp(prefix, TCP_PREFIX, strlen(TCP_PREFIX)))
    {
        SOPC_Logger_TraceError(SOPC_LOG_MODULE_PUBSUB, "Failed to determine the protocol from prefix '%s'", prefix);
        return false;
    }

    const size_t PREFIX_LENGTH = strlen(prefix);
    *hostnameLength = 0;
    *portIdx = 0;
    *portLength = 0;
    if (strlen(uri) + 4 > TCP_UA_MAX_URL_LENGTH)
    {
        // Encoded value shall be less than 4096 bytes
        return false;
    }
    else if (strlen(uri) <= PREFIX_LENGTH || SOPC_strncmp_ignore_case(uri, prefix, PREFIX_LENGTH) != 0)
    {
        return false;
    }

    // search for a ':' defining port for given IP
    // search for a '/' defining endpoint name for given IP => at least 1 char after it (len - 1)
    for (idx = PREFIX_LENGTH; idx < strlen(uri) - 1 && !invalid; idx++)
    {
        if (false != isPort && false == endOfPort)
        {
            if (uri[idx] >= '0' && uri[idx] <= '9')
            {
                if (false == hasPort)
                {
                    // port definition
                    hasPort = true;
                    *portIdx = idx;
                }
            }
            else if (uri[idx] == '/' && false == invalid)
            {
                // Name of the endpoint after port, invalid otherwise
                if (false == hasPort)
                {
                    invalid = true;
                }
                else
                {
                    *portLength = idx - *portIdx;
                    hasName = true;
                    endOfPort = true; // End of port definition
                }
            }
            else
            {
                if (false == hasPort || false == hasName)
                {
                    // unexpected character: we do not expect a endpoint name
                    invalid = true;
                }
            }
        }
        else
        {
            if (false == endOfPort)
            {
                // Treatment before the port parsing
                if (uri[idx] == ':' && false == startIPv6)
                {
                    *hostnameLength = idx - PREFIX_LENGTH;
                    isPort = true;
                }
                else if (uri[idx] == '[')
                {
                    startIPv6 = true;
                }
                else if (uri[idx] == ']')
                {
                    if (false == startIPv6)
                    {
                        invalid = true;
                    }
                    else
                    {
                        startIPv6 = false;
                    }
                }
            }
            else if (hasPort)
            {
                // Treatment after the port parsing
                // TODO: check absence of forbidden characters
            }
        }
    }

    if (hasPort != false && false == invalid)
    {
        result = true;
        if (*portLength == 0)
        {
            // No endpoint name after port provided
            *portLength = idx - *portIdx + 1;
        }
    }

    return result;
}

/* Check if array dimension of src_variant respect limitations of dest_arrayDimensions.*/
static bool SOPC_ArrayDimensions_isCompatible(const SOPC_PubSub_ArrayDimension* dest_arrayDimensions,
                                              const SOPC_Variant* src_variant)
{
    if (NULL == dest_arrayDimensions || NULL == src_variant)
    {
        return false;
    }
    bool res = true;
    int32_t src_valueRank = SOPC_Variant_Get_ValueRank(src_variant);

    // If variant is a scalar nothing to check
    if (SOPC_VariantArrayType_SingleValue != src_variant->ArrayType)
    {
        if (NULL == dest_arrayDimensions->arrayDimensions)
        {
            res = false;
        }
        else if (dest_arrayDimensions->valueRank == src_valueRank)
        {
            if (SOPC_VariantArrayType_Array == src_variant->ArrayType)
            {
                res = ((dest_arrayDimensions->arrayDimensions[0] == 0) ||
                       (src_variant->Value.Array.Length <= (int32_t) dest_arrayDimensions->arrayDimensions[0]));
            }
            else
            {
                for (int i = 0; res && i < src_valueRank; i++)
                {
                    // Array dimension equal to zero means no up boundary
                    const uint32_t dimI = dest_arrayDimensions->arrayDimensions[i];
                    res = ((dimI == 0) || (src_variant->Value.Matrix.ArrayDimensions[i] <= (int32_t) dimI));
                }
            }
        }
        else
        {
            res = false;
        }
    }
    return res;
}

bool SOPC_PubSubHelpers_IsCompatibleVariant(const SOPC_FieldMetaData* fieldMetaData,
                                            const SOPC_Variant* variant,
                                            bool* out_isBad)
{
    SOPC_ASSERT(NULL != fieldMetaData);
    SOPC_ASSERT(NULL != variant);
    bool res = true;
    if (NULL != out_isBad)
    {
        *out_isBad = false;
    }

    SOPC_BuiltinId expBuiltInType = SOPC_FieldMetaData_Get_BuiltinType(fieldMetaData);
    if (SOPC_Null_Id == expBuiltInType)
    {
        return true;
    }

    if (expBuiltInType == variant->BuiltInTypeId)
    {
        int32_t expValueRank = SOPC_FieldMetaData_Get_ValueRank(fieldMetaData);
        int32_t actualValueRank = SOPC_Variant_Get_ValueRank(variant);

        res = SOPC_ValueRank_IsAssignableInto(expValueRank, actualValueRank);
        if (res)
        {
            const SOPC_PubSub_ArrayDimension* expectedArrayDimensions =
                SOPC_FieldMetaData_Get_ArrayDimension(fieldMetaData);
            SOPC_ASSERT(NULL != expectedArrayDimensions);
            res = SOPC_ArrayDimensions_isCompatible(expectedArrayDimensions, variant);
        }
        return res;
    }
    else if (SOPC_Null_Id == variant->BuiltInTypeId)
    {
        // Consider NULL variant compatible with any type:
        // it might be used when status is Bad or misused (NULL usage unclear in OpcUa spec part 6)
        return true;
    }
    else if (SOPC_StatusCode_Id == variant->BuiltInTypeId && (0 != (variant->Value.Status & SOPC_BadStatusMask)))
    {
        // Consider Bad status variant compatible with any value
        // (see spec part 14 - 1.05: Table 26: DataSetMessage field representation options)
        if (NULL != out_isBad)
        {
            *out_isBad = true;
        }
        // It shall be a single value that indicate variant status
        return (SOPC_VariantArrayType_SingleValue == variant->ArrayType);
    }
    return false;
}

bool SOPC_PubSubHelpers_IsPreencodeCompatibleVariant(const SOPC_FieldMetaData* fieldMetaData,
                                                     const SOPC_Variant* variant)
{
    SOPC_ASSERT(NULL != fieldMetaData);
    SOPC_ASSERT(NULL != variant);

    const SOPC_BuiltinId expBuiltInType = SOPC_FieldMetaData_Get_BuiltinType(fieldMetaData);
    bool res = false;
    if (expBuiltInType == variant->BuiltInTypeId)
    {
        const SOPC_PubSub_ArrayDimension* arrDimension = SOPC_FieldMetaData_Get_ArrayDimension(fieldMetaData);
        const int32_t actualValueRank = SOPC_Variant_Get_ValueRank(variant);
        if (NULL != arrDimension)
        {
            if (arrDimension->valueRank == actualValueRank)
            {
                if (-1 == actualValueRank)
                {
                    res = true;
                }
                else if (1 == actualValueRank)
                {
                    SOPC_ASSERT(NULL != arrDimension->arrayDimensions);
                    res = ((int32_t) *arrDimension->arrayDimensions == variant->Value.Array.Length);
                }
            }
        }
    }
    return res;
}

SOPC_ReturnStatus SOPC_Helper_PublisherId_Compare(const SOPC_Conf_PublisherId* pubIdLeft,
                                                  const SOPC_Conf_PublisherId* pubIdRight,
                                                  bool* comp)
{
    if (pubIdLeft == NULL || NULL == pubIdRight || NULL == comp)
    {
        return SOPC_STATUS_INVALID_PARAMETERS;
    }
    SOPC_ReturnStatus status = SOPC_STATUS_OK;
    *comp = false;
    if (pubIdLeft->type == pubIdRight->type)
    {
        int32_t match = -1;
        switch (pubIdLeft->type)
        {
        case SOPC_UInteger_PublisherId:
            if (pubIdLeft->data.uint == pubIdRight->data.uint)
            {
                *comp = true;
            }
            break;
        case SOPC_String_PublisherId:
            status = SOPC_String_Compare(&pubIdLeft->data.string, &pubIdRight->data.string, false, &match);
            if (SOPC_STATUS_OK == status)
            {
                *comp = (0 == match);
            }
            break;
        case SOPC_Null_PublisherId:
        default:
            *comp = true;
            break;
        }
    }
    return status;
}
