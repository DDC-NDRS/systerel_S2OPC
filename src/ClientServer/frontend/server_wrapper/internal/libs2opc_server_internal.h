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
 * \privatesection
 *
 * \brief Internal module used to manage the wrapper for server config. It should not be used outside of the server
 * wraper implementation.
 *
 */

#ifndef LIBS2OPC_SERVER_CONFIG_INTERNAL_H_
#define LIBS2OPC_SERVER_CONFIG_INTERNAL_H_

#include <inttypes.h>
#include <stdbool.h>

#include "sopc_address_space.h"
#include "sopc_event_manager.h"
#include "sopc_mutexes.h"
#include "sopc_toolkit_config.h"
#include "sopc_toolkit_config_constants.h"
#include "sopc_user_app_itf.h"

#include "libs2opc_server.h"
#include "libs2opc_server_config.h"
#include "libs2opc_server_runtime_variables.h"

#ifndef SOPC_HELPER_LOCAL_RESPONSE_TIMEOUT_MS
#define SOPC_HELPER_LOCAL_RESPONSE_TIMEOUT_MS 5000
#endif

#ifndef SOPC_DEFAULT_SHUTDOWN_PHASE_IN_SECONDS
#define SOPC_DEFAULT_SHUTDOWN_PHASE_IN_SECONDS 5
#endif

#ifndef SOPC_DEFAULT_CURRENT_TIME_REFERSH_FREQ_MS
#define SOPC_DEFAULT_CURRENT_TIME_REFERSH_FREQ_MS 1000
#endif

typedef enum
{
    SOPC_SERVER_STATE_INITIALIZING = 0,
    SOPC_SERVER_STATE_CONFIGURING = 1,
    SOPC_SERVER_STATE_CONFIGURED = 2,
    SOPC_SERVER_STATE_STARTED = 3,
    SOPC_SERVER_STATE_SHUTDOWN_PHASE = 4,
    SOPC_SERVER_STATE_STOPPING = 5,
    SOPC_SERVER_STATE_STOPPED = 6,
} SOPC_HelperServer_State;

// The server helper dedicated configuration in addition to configuration ::SOPC_S2OPC_Config
typedef struct SOPC_ServerHelper_Config
{
    // Flag atomically set when the structure is initialized during call to SOPC_ServerConfigHelper_Initialize
    // and singleton config is initialized
    int32_t initialized;
    // Server state
    SOPC_Mutex stateMutex;
    SOPC_HelperServer_State state;

    // Address space instance
    SOPC_AddressSpace* addressSpace;

    // Application write notification callback record
    SOPC_WriteNotif_Fct* writeNotifCb;
    // Application node availability for CreateMonitoreItem callback
    SOPC_CreateMI_NodeAvail_Fct* nodeAvailCb;
    // Application asynchronous local service response callback record
    SOPC_LocalServiceAsyncResp_Fct* asyncRespCb;
    // Application server private key password callback
    SOPC_GetServerKeyPassword_Fct*
        getServerKeyPassword; /* If it is defined and if the serverKey is encrypted,
                                 then the callback is called during call to
                                 ::SOPC_ServerConfigHelper_SetKeyCertPairFromPath
                                 or ::SOPC_ServerConfigHelper_ConfigureFromXML.
                                 The callback allows to retrieve the password for decryption */

    // Synchronous local service response management
    SOPC_Condition syncLocalServiceCond;
    SOPC_Mutex syncLocalServiceMutex;
    uint32_t syncLocalServiceId;
    bool syncCalled;
    void* syncResp;

    // Stop server management:

    // Manage server stopping when server is running synchronously using SOPC_ServerHelper_Serve.
    struct
    {
        SOPC_Condition serverStoppedCond;
        SOPC_Mutex serverStoppedMutex;
        int32_t serverRequestedToStop;
        bool serverAllEndpointsClosed;
    } syncServeStopData;

    // Server stopped notification callback record
    SOPC_ServerStopped_Fct* stoppedCb;
    // Server stopped notification callback data
    SOPC_ReturnStatus serverStoppedStatus;

    // Server shutdown phase duration configuration
    uint16_t configuredSecondsTillShutdown;
    // Server status current time refresh interval
    uint16_t configuredCurrentTimeRefreshIntervalMs;
    uint32_t currentTimeRefreshTimerId;

    // Server build info
    OpcUa_BuildInfo* buildInfo;

    // Configured endpoint indexes and opened state arrays
    uint8_t nbEndpoints;
    SOPC_Endpoint_Config* endpoints[SOPC_MAX_ENDPOINT_DESCRIPTION_CONFIGURATIONS]; // we do not use config.endpoints to
                                                                                   // avoid pre-allocating structure
    SOPC_EndpointConfigIdx* endpointIndexes; // array of endpoint indexes provided by toolkit
    bool* endpointClosed; // array of closed endpoint to keep track of endpoints notified closed by toolkit

    // Runtime variables
    SOPC_Server_RuntimeVariables runtimeVariables;

} SOPC_ServerHelper_Config;

// Define the structure used as context for asynchronous calls
typedef struct SOPC_HelperConfigInternal_Ctx
{
    // actual context for helper user (user application)
    uintptr_t userContext;

    // Discriminant
    SOPC_App_Com_Event event;
    union
    {
        struct LocalServiceCtx
        {
            // sync call management
            bool isSyncCall;
            uint32_t syncId;
            // custom async response callback
            SOPC_LocalServiceAsyncResp_Fct* customAsyncRespCb;
            // internal use of local services + local service type callback (e.g runtime variables)
            bool isHelperInternal;
            // message to display in case of internal local service failure (response NOK)
            const char* internalErrorMsg;
        } localService;
    } eventCtx;
} SOPC_HelperConfigInternal_Ctx;

// The singleton configuration structure
extern SOPC_ServerHelper_Config sopc_server_helper_config;

// Returns true if the server is in configuring state, false otherwise
bool SOPC_ServerInternal_IsConfiguring(void);

// Returns true if the server is in started state, false otherwise
bool SOPC_ServerInternal_IsStarted(void);

// Returns true if the server is in stopped state, false otherwise
bool SOPC_ServerInternal_IsStopped(void);

// Returns true if the server is in stopped state or in a state previous to started state, false otherwise
// Note: server configuration is not clearable in shutdown or stopping states
bool SOPC_ServerInternal_IsConfigClearable(void);

// Check for configuration issues and set server state as configured in case of success
bool SOPC_ServerInternal_CheckConfigAndSetConfiguredState(void);

// Check current state and set server state as started in case of success
bool SOPC_ServerInternal_SetStartedState(void);

// Check current state and set server state as stopping in case of success
bool SOPC_ServerInternal_SetStoppingState(void);

// Set server state as stopped
void SOPC_ServerInternal_SetStoppedState(void);

// Get password to decrypt server private key from internal callback
bool SOPC_ServerInternal_GetKeyPassword(char** outPassword);

// Local service synchronous internal callback
void SOPC_ServerInternal_SyncLocalServiceCb(SOPC_EncodeableType* encType,
                                            void* response,
                                            SOPC_HelperConfigInternal_Ctx* helperCtx);

// Local service asynchronous internal callback
void SOPC_ServerInternal_AsyncLocalServiceCb(SOPC_EncodeableType* encType,
                                             void* response,
                                             SOPC_HelperConfigInternal_Ctx* helperCtx);

// Endpoint closed asynchronous callback
void SOPC_ServerInternal_ClosedEndpoint(uint32_t epConfigIdx, SOPC_ReturnStatus status);

// Clear low level endpoint config (clear strings, do not clear user managers)
void SOPC_ServerInternal_ClearEndpoint(SOPC_Endpoint_Config* epConfig);

// Callback instance to be used on client application key / certificate pair update
void SOPC_ServerInternal_KeyCertPairUpdateCb(uintptr_t updateParam);

// Callback instance to be used on server PKI update
void SOPC_ServerInternal_PKIProviderUpdateCb(uintptr_t updateParam);

// Local service asynchronous internal version:
// it differs from ::SOPC_ServerHelper_LocalServiceAsyncCustom
// in that the provided context in \p asyncRespCb call is internal ::SOPC_HelperConfigInternal_Ctx instead of \p userCtx
bool SOPC_ServerInternal_LocalServiceAsync(SOPC_LocalServiceAsyncResp_Fct* asyncRespCb,
                                           void* request,
                                           uintptr_t userCtx,
                                           const char* errorMsg);

#endif
