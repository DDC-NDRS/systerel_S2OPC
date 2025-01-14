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

/**
 * @brief
 *  Implementation of SOPC sockets in scope of ZEPHYR OS
 */
#ifndef SOPC_ZEPHYR_P_SOCKETS_HEADER_
#define SOPC_ZEPHYR_P_SOCKETS_HEADER_

#include <stdbool.h>
#include <stdint.h>

#include <zephyr/net/socket.h>

#include "sopc_enums.h"

/**
 *  \brief Socket base type
 */
struct SOPC_Socket_Impl
{
    int sock;
};

/**
 *  \brief Socket addressing information for listening or connecting operation type
 *  \note Internal treatment use the fact it is the first field as property
 */
struct SOPC_Socket_AddressInfo
{
    struct zsock_addrinfo addrInfo;
};

/**
 *  \brief Socket address information on a connected socket
 *  \note Internal treatment use the fact it is the first field as property
 */
struct SOPC_Socket_Address
{
    struct zsock_addrinfo address;
};

/**
 *  \brief Set of sockets type
 */
struct SOPC_SocketSet
{
    int32_t fdmax;    /**< max of the set */
    zsock_fd_set set; /**< set */
};

#endif /* SOPC_ZEPHYR_P_SOCKETS_HEADER_ */
