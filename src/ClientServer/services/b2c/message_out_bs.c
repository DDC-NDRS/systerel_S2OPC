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
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "message_out_bs.h"
#include "util_b2c.h"

#include "sopc_assert.h"
#include "sopc_date_time.h"
#include "sopc_encoder.h"
#include "sopc_logger.h"
#include "sopc_macros.h"
#include "sopc_mem_alloc.h"
#include "sopc_protocol_constants.h"
#include "sopc_toolkit_config_internal.h"

/*------------------------
   INITIALISATION Clause
  ------------------------*/
void message_out_bs__INITIALISATION(void)
{ /*Translated from B but an intialisation is not needed from this module.*/
}

/*--------------------
   OPERATIONS Clause
  --------------------*/
static void* internal_alloc_msg_header(const t_bool is_request)
{
    SOPC_EncodeableType* encType = NULL;
    SOPC_ReturnStatus status = SOPC_STATUS_NOK;
    void* header = NULL;

    if (is_request)
    {
        encType = &OpcUa_RequestHeader_EncodeableType;
    }
    else
    {
        encType = &OpcUa_ResponseHeader_EncodeableType;
    }

    status = SOPC_EncodeableObject_Create(encType, &header);
    if (SOPC_STATUS_OK != status)
    {
        header = NULL;
    }
    return header;
}

static void util_message_out_bs__alloc_msg(const constants__t_msg_type_i message_out_bs__msg_type,
                                           constants__t_msg_header_i* const message_out_bs__nmsg_header,
                                           constants__t_msg_i* const message_out_bs__nmsg)
{
    void* header = NULL;
    void* msg = NULL;
    SOPC_ReturnStatus status = SOPC_STATUS_NOK;

    SOPC_EncodeableType* encTyp = NULL;
    SOPC_EncodeableType* reqEncTyp = NULL;
    SOPC_EncodeableType* respEncTyp = NULL;
    t_bool isReq = false;

    util_message__get_encodeable_type(message_out_bs__msg_type, &reqEncTyp, &respEncTyp, &isReq);
    if (isReq != false)
    {
        encTyp = reqEncTyp;
    }
    else
    {
        encTyp = respEncTyp;
    }

    if (NULL != encTyp)
    {
        status = SOPC_EncodeableObject_Create(encTyp, &msg);
    }

    if (SOPC_STATUS_OK == status)
    {
        header = internal_alloc_msg_header(isReq);
    }

    if (NULL != header)
    {
        *message_out_bs__nmsg = (constants__t_msg_i) msg;
        *message_out_bs__nmsg_header = (constants__t_msg_header_i) header;
    }
    else
    {
        SOPC_EncodeableObject_Delete(encTyp, &msg);

        *message_out_bs__nmsg = constants__c_msg_indet;
        *message_out_bs__nmsg_header = constants__c_msg_header_indet;
    }
}

void message_out_bs__alloc_msg_header(const t_bool message_out_bs__p_is_request,
                                      constants__t_msg_header_i* const message_out_bs__nmsg_header)
{
    *message_out_bs__nmsg_header = internal_alloc_msg_header(message_out_bs__p_is_request);
}

void message_out_bs__alloc_req_msg(const constants__t_msg_type_i message_out_bs__msg_type,
                                   constants__t_msg_header_i* const message_out_bs__nmsg_header,
                                   constants__t_msg_i* const message_out_bs__nmsg)
{
    util_message_out_bs__alloc_msg(message_out_bs__msg_type, message_out_bs__nmsg_header, message_out_bs__nmsg);
}

void message_out_bs__alloc_resp_msg(const constants__t_msg_type_i message_out_bs__msg_type,
                                    constants__t_msg_header_i* const message_out_bs__nmsg_header,
                                    constants__t_msg_i* const message_out_bs__nmsg)
{
    util_message_out_bs__alloc_msg(message_out_bs__msg_type, message_out_bs__nmsg_header, message_out_bs__nmsg);
}

void message_out_bs__bless_msg_out(const constants__t_msg_i message_out_bs__msg,
                                   constants__t_msg_type_i* const message_out_bs__msg_typ)
{
    /*
     * We check that the message looks good: non-NULL pointer and contains a
     * valid encodeable type.  This is therefore not exactly what is specified in
     * the B model.  We work on a best effort basis and mostly trust the
     * the piece of code which passed the message.
     */
    SOPC_EncodeableType* encType = NULL;
    *message_out_bs__msg_typ = constants__c_msg_type_indet;
    if (constants__c_msg_indet == message_out_bs__msg)
    {
        return;
    }
    encType = *(SOPC_EncodeableType**) message_out_bs__msg;
    if (NULL == encType)
    {
        return;
    }
    util_message__get_message_type(encType, message_out_bs__msg_typ);
    if (NULL != encType && constants__c_msg_type_indet == *message_out_bs__msg_typ)
    {
        // In case of unrecognized type, deallocate here since B calling operation cannot manage it (msg not set)
        SOPC_EncodeableObject_Delete(encType, message_out_bs__msg);
    }
}

void message_out_bs__dealloc_msg_header_out(const constants__t_msg_header_i message_out_bs__msg_header)
{
    // Generated header, parameter not really a const.
    SOPC_GCC_DIAGNOSTIC_IGNORE_CAST_CONST
    if ((*(SOPC_EncodeableType**) message_out_bs__msg_header) == &OpcUa_ResponseHeader_EncodeableType)
    {
        SOPC_EncodeableObject_Delete(&OpcUa_ResponseHeader_EncodeableType, (void**) &message_out_bs__msg_header);
    }
    else if ((*(SOPC_EncodeableType**) message_out_bs__msg_header) == &OpcUa_RequestHeader_EncodeableType)
    {
        SOPC_EncodeableObject_Delete(&OpcUa_RequestHeader_EncodeableType, (void**) &message_out_bs__msg_header);
    }
    else
    {
        SOPC_ASSERT(false);
    }
    SOPC_GCC_DIAGNOSTIC_RESTORE
}

void message_out_bs__dealloc_msg_out(const constants__t_msg_i message_out_bs__msg)
{
    // First field of each message structure is an encodeable type
    SOPC_EncodeableType* encType = NULL;
    if (message_out_bs__msg != constants__c_msg_indet)
    {
        encType = *(SOPC_EncodeableType**) message_out_bs__msg;

        // To could keep generated prototype
        // Generated header, parameter not really a const.
        SOPC_GCC_DIAGNOSTIC_IGNORE_CAST_CONST
        SOPC_EncodeableObject_Delete(encType, (void**) &message_out_bs__msg);
        SOPC_GCC_DIAGNOSTIC_RESTORE
    }
}

static void internal__message_out_bs__encode_msg(const constants__t_channel_config_idx_i message_out_bs__channel_cfg,
                                                 const constants__t_msg_header_type_i message_out_bs__header_type,
                                                 const constants__t_msg_type_i message_out_bs__msg_type,
                                                 const constants__t_msg_header_i message_out_bs__msg_header,
                                                 const constants__t_msg_i message_out_bs__msg,
                                                 constants_statuscodes_bs__t_StatusCode_i* const message_out_bs__sc,
                                                 constants__t_byte_buffer_i* const message_out_bs__buffer)
{
    *message_out_bs__sc = constants_statuscodes_bs__e_sc_bad_out_of_memory;
    *message_out_bs__buffer = constants__c_byte_buffer_indet;
    OpcUa_RequestHeader* reqHeader = NULL;
    OpcUa_ResponseHeader* respHeader = NULL;
    SOPC_ReturnStatus status = SOPC_STATUS_NOK;
    SOPC_EncodeableType* encType = *(SOPC_EncodeableType**) message_out_bs__msg;
    SOPC_EncodeableType* headerType = *(SOPC_EncodeableType**) message_out_bs__msg_header;
    SOPC_SecureChannel_Config* chConfig = NULL;
    if (&OpcUa_RequestHeader_EncodeableType == headerType)
    {
        chConfig = SOPC_ToolkitClient_GetSecureChannelConfig(message_out_bs__channel_cfg);
    }
    else if (&OpcUa_ResponseHeader_EncodeableType == headerType)
    {
        chConfig = SOPC_ToolkitServer_GetSecureChannelConfig(message_out_bs__channel_cfg);
    }
    else
    {
        SOPC_ASSERT(false);
    }

    if (NULL == chConfig)
    {
        *message_out_bs__sc = constants_statuscodes_bs__e_sc_bad_encoding_error;
        return;
    }

    uint32_t sendMessageMaxSize = (uint32_t) chConfig->internalProtocolData;
    SOPC_Buffer* buffer = SOPC_Buffer_CreateResizable(
        SOPC_TCP_UA_MIN_BUFFER_SIZE, sendMessageMaxSize + SOPC_UA_SYMMETRIC_SECURE_MESSAGE_HEADERS_LENGTH);
    if (NULL != buffer)
    {
        status = SOPC_STATUS_OK;
    }
    if (SOPC_STATUS_OK == status)
    {
        status = SOPC_Buffer_SetDataLength(buffer, SOPC_UA_SYMMETRIC_SECURE_MESSAGE_HEADERS_LENGTH);
    }
    if (SOPC_STATUS_OK == status)
    {
        // Encodeable type: either msg_type = service fault type or it is the type provided by the msg
        if (message_out_bs__msg_type == constants__e_msg_service_fault_resp)
        {
            encType = &OpcUa_ServiceFault_EncodeableType;
        }

        status = SOPC_Buffer_SetPosition(buffer, SOPC_UA_SYMMETRIC_SECURE_MESSAGE_HEADERS_LENGTH);
    }
    if (SOPC_STATUS_OK == status)
    {
        // Encode req/resp header content not dependent on message content
        if (&OpcUa_RequestHeader_EncodeableType == headerType)
        {
            reqHeader = (OpcUa_RequestHeader*) message_out_bs__msg_header;
            reqHeader->Timestamp = SOPC_Time_GetCurrentTimeUTC();
            // TODO: reqHeader->AuditEntryId ?
            reqHeader->TimeoutHint = SOPC_REQUEST_TIMEOUT_MS / 2; // TODO: to be configured by each service ?
        }
        else if (&OpcUa_ResponseHeader_EncodeableType == headerType)
        {
            respHeader = (OpcUa_ResponseHeader*) message_out_bs__msg_header;
            respHeader->Timestamp = SOPC_Time_GetCurrentTimeUTC();
        }
        else
        {
            SOPC_ASSERT(false);
        }
    }
    if (SOPC_STATUS_OK == status)
    {
        status = SOPC_EncodeMsg_Type_Header_Body(buffer, encType, headerType, message_out_bs__msg_header,
                                                 message_out_bs__msg);
    }
    if (SOPC_STATUS_OK == status)
    {
        *message_out_bs__sc = constants_statuscodes_bs__e_sc_ok;
        *message_out_bs__buffer = (constants__t_byte_buffer_i) buffer;

        if (message_out_bs__msg_type == constants__e_msg_service_fault_resp)
        {
            SOPC_Logger_TraceDebug(SOPC_LOG_MODULE_CLIENTSERVER,
                                   "Services: encoded output message type = '%s' with statusCode= '%" PRIX32 "'",
                                   SOPC_EncodeableType_GetName(encType),
                                   ((OpcUa_ResponseHeader*) message_out_bs__msg_header)->ServiceResult);
        }
        else
        {
            // Note: no status in case of request and good status mandatory for not faulty response
            SOPC_Logger_TraceDebug(SOPC_LOG_MODULE_CLIENTSERVER, "Services: encoded output message type = '%s'",
                                   SOPC_EncodeableType_GetName(encType));
        }
    }
    else
    {
        switch (status)
        {
        case SOPC_STATUS_WOULD_BLOCK:
            SOPC_Logger_TraceWarning(
                SOPC_LOG_MODULE_CLIENTSERVER,
                "Services: encoding of message failed (type = '%s') because it is too large: max size %" PRIu32
                " reached",
                SOPC_EncodeableType_GetName(encType), buffer->maximum_size);

            // Limit of buffer reached
            switch (message_out_bs__header_type)
            {
            case constants__e_msg_request_type:
                *message_out_bs__sc = constants_statuscodes_bs__e_sc_bad_request_too_large;
                break;
            case constants__e_msg_response_type:
                if (SOPC_Internal_Common_GetEncodingConstants()->send_max_msg_size == sendMessageMaxSize)
                {
                    /* Internal limit */
                    *message_out_bs__sc = constants_statuscodes_bs__e_sc_bad_encoding_limits_exceeded;
                }
                else
                {
                    /* Client limit */
                    *message_out_bs__sc = constants_statuscodes_bs__e_sc_bad_response_too_large;
                }
                break;
            default:
                SOPC_ASSERT(false);
            }
            break;
        // TODO: add a SOPC_STATUS_ENCODING_LIMIT for errors due to limits
        case SOPC_STATUS_ENCODING_ERROR:
        default:
            SOPC_Logger_TraceWarning(SOPC_LOG_MODULE_CLIENTSERVER, "Services: encoding of message failed (type = '%s')",
                                     SOPC_EncodeableType_GetName(encType));

            *message_out_bs__sc = constants_statuscodes_bs__e_sc_bad_encoding_error;
        }

        // if buffer is not used, it must be freed
        SOPC_Buffer_Delete(buffer);
        buffer = NULL;
    }
}

void message_out_bs__encode_msg(const constants__t_channel_config_idx_i message_out_bs__channel_cfg,
                                const constants__t_msg_header_type_i message_out_bs__header_type,
                                const constants__t_msg_type_i message_out_bs__msg_type,
                                const constants__t_msg_header_i message_out_bs__msg_header,
                                const constants__t_msg_i message_out_bs__msg,
                                constants_statuscodes_bs__t_StatusCode_i* const message_out_bs__sc,
                                constants__t_byte_buffer_i* const message_out_bs__buffer)
{
    internal__message_out_bs__encode_msg(message_out_bs__channel_cfg, message_out_bs__header_type,
                                         message_out_bs__msg_type, message_out_bs__msg_header, message_out_bs__msg,
                                         message_out_bs__sc, message_out_bs__buffer);

    if (constants_statuscodes_bs__e_sc_ok != *message_out_bs__sc &&
        constants__e_msg_response_type == message_out_bs__header_type)
    {
        // In this case we should send a ServiceFault instead of failing
        OpcUa_ResponseHeader* respHeader = (OpcUa_ResponseHeader*) message_out_bs__msg_header;
        switch (*message_out_bs__sc)
        {
        case constants_statuscodes_bs__e_sc_bad_response_too_large:
            respHeader->ServiceResult = OpcUa_BadResponseTooLarge;
            break;
        case constants_statuscodes_bs__e_sc_bad_encoding_limits_exceeded:
            respHeader->ServiceResult = OpcUa_BadEncodingLimitsExceeded;
            break;
        case constants_statuscodes_bs__e_sc_bad_encoding_error:
        default:
            respHeader->ServiceResult = OpcUa_BadEncodingError;
        }
        internal__message_out_bs__encode_msg(message_out_bs__channel_cfg, message_out_bs__header_type,
                                             constants__e_msg_service_fault_resp, message_out_bs__msg_header,
                                             message_out_bs__msg, message_out_bs__sc, message_out_bs__buffer);
    }
}

void message_out_bs__forget_msg_out(const constants__t_msg_i message_out_bs__msg)
{
    // The message is now owned by another part of the code
    SOPC_UNUSED_ARG(message_out_bs__msg);
}

void message_out_bs__copy_msg_resp_header_into_msg_out(const constants__t_msg_header_i message_out_bs__msg_header,
                                                       const constants__t_msg_i message_out_bs__msg)
{
    util_message__copy_resp_header_into_msg(message_out_bs__msg_header, message_out_bs__msg);
}

void message_out_bs__server_write_msg_out_header_req_handle(
    const constants__t_msg_header_i message_out_bs__msg_header,
    const constants__t_server_request_handle_i message_out_bs__req_handle)
{
    if ((*(SOPC_EncodeableType**) message_out_bs__msg_header) == &OpcUa_ResponseHeader_EncodeableType)
    {
        ((OpcUa_ResponseHeader*) message_out_bs__msg_header)->RequestHandle = message_out_bs__req_handle;
    }
    else if ((*(SOPC_EncodeableType**) message_out_bs__msg_header) == &OpcUa_RequestHeader_EncodeableType)
    {
        ((OpcUa_RequestHeader*) message_out_bs__msg_header)->RequestHandle = message_out_bs__req_handle;
    }
    else
    {
        SOPC_ASSERT(false);
    }
}

void message_out_bs__client_write_msg_out_header_req_handle(
    const constants__t_msg_header_i message_out_bs__msg_header,
    const constants__t_client_request_handle_i message_out_bs__req_handle)
{
    message_out_bs__server_write_msg_out_header_req_handle(message_out_bs__msg_header, message_out_bs__req_handle);
}

void message_out_bs__write_msg_out_header_session_token(
    const constants__t_msg_header_i message_out_bs__msg_header,
    const constants__t_session_token_i message_out_bs__session_token)
{
    SOPC_NodeId* authToken = message_out_bs__session_token;

    SOPC_ReturnStatus status =
        SOPC_NodeId_Copy(&((OpcUa_RequestHeader*) message_out_bs__msg_header)->AuthenticationToken, authToken);
    SOPC_ASSERT(SOPC_STATUS_OK == status);
}

void message_out_bs__write_msg_resp_header_service_status(
    const constants__t_msg_header_i message_out_bs__msg_header,
    const constants_statuscodes_bs__t_StatusCode_i message_out_bs__status_code)
{
    SOPC_StatusCode status = OpcUa_BadInternalError;
    util_status_code__B_to_C(message_out_bs__status_code, &status);
    ((OpcUa_ResponseHeader*) message_out_bs__msg_header)->ServiceResult = status;
}
