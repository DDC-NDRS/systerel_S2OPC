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

#include "sopc_assert.h"
#include "sopc_common_constants.h"
#include "sopc_macros.h"
#include "sopc_mem_alloc.h"
#include "sopc_threads.h"

#include "p_sopc_sockets.h"

#include "lwipopts.h"

/****** CONFIGURATION CHECK *******/
#if !LWIP_NETIF_LOOPBACK
#error "LwIP 'LWIP_NETIF_LOOPBACK' is required for this sample!"
#endif
#if !SO_REUSE
#error "LwIP 'SO_REUSE' is required for this sample!"
#endif

SOPC_ReturnStatus SOPC_Socket_Network_Enable_Keepalive(SOPC_Socket sock,
                                                       unsigned int time,
                                                       unsigned int interval,
                                                       unsigned int counter)
{
    (void) sock;
    (void) time;
    (void) interval;
    (void) counter;
    return SOPC_STATUS_NOT_SUPPORTED;
}

SOPC_ReturnStatus SOPC_Socket_Network_Disable_Keepalive(SOPC_Socket sock)
{
    (void) sock;
    return SOPC_STATUS_NOT_SUPPORTED;
}

bool SOPC_Socket_Network_Initialize()
{
    return true;
}

bool SOPC_Socket_Network_Clear()
{
    return true;
}

SOPC_ReturnStatus SOPC_Socket_AddrInfo_Get(const char* hostname, const char* port, SOPC_Socket_AddressInfo** addrs)
{
    SOPC_ReturnStatus status = SOPC_STATUS_INVALID_PARAMETERS;
    SOPC_Socket_AddressInfo hints;
    memset(&hints, 0, sizeof(SOPC_Socket_AddressInfo));
    hints.addrInfo.ai_family = AF_UNSPEC; // AF_INET or AF_INET6  can be use to force IPV4 or IPV6
    hints.addrInfo.ai_socktype = SOCK_STREAM;
    hints.addrInfo.ai_flags = AI_PASSIVE;

    struct addrinfo* getAddrInfoRes = NULL;

    if ((hostname != NULL || port != NULL) && addrs != NULL)
    {
        if (getaddrinfo(hostname, port, &hints.addrInfo, &getAddrInfoRes) != 0)
        {
            status = SOPC_STATUS_NOK;
        }
        else
        {
            *addrs = (SOPC_Socket_AddressInfo*) getAddrInfoRes;
            status = SOPC_STATUS_OK;
        }
    }
    return status;
}

SOPC_Socket_AddressInfo* SOPC_Socket_AddrInfo_IterNext(SOPC_Socket_AddressInfo* addr)
{
    SOPC_Socket_AddressInfo* res = NULL;
    if (NULL != addr)
    {
        res = (SOPC_Socket_AddressInfo*) addr->addrInfo.ai_next;
    }
    return res;
}

uint8_t SOPC_Socket_AddrInfo_IsIPV6(const SOPC_Socket_AddressInfo* addr)
{
    return addr->addrInfo.ai_family == PF_INET6;
}

void SOPC_Socket_AddrInfoDelete(SOPC_Socket_AddressInfo** addrs)
{
    if (NULL != addrs)
    {
        freeaddrinfo(&(*addrs)->addrInfo);
        *addrs = NULL;
    }
}

void SOPC_SocketAddress_Delete(SOPC_Socket_Address** addr)
{
    if (NULL == addr)
    {
        return;
    }
    if (NULL != *addr)
    {
        SOPC_Free((*addr)->address.ai_addr);
    }
    SOPC_Free(*addr);
    *addr = NULL;
}

SOPC_Socket_Address* SOPC_Socket_CopyAddress(SOPC_Socket_AddressInfo* addr)
{
    SOPC_Socket_Address* result = SOPC_Calloc(1, sizeof(*result));
    if (NULL != result)
    {
        result->address.ai_addr = SOPC_Calloc(1, addr->addrInfo.ai_addrlen);
        result->address.ai_addrlen = addr->addrInfo.ai_addrlen;
        result->address.ai_family = addr->addrInfo.ai_family;
        if (NULL != result->address.ai_addr)
        {
            result->address.ai_addr =
                memcpy(result->address.ai_addr, addr->addrInfo.ai_addr, addr->addrInfo.ai_addrlen);
        }
        else
        {
            SOPC_Free(result);
            result = NULL;
        }
    }
    return result;
}

SOPC_Socket_Address* SOPC_Socket_GetPeerAddress(SOPC_Socket sock)
{
    if (!SOPC_FREERTOS_SOCKET_IS_VALID(sock))
    {
        return NULL;
    }
    SOPC_Socket_Address* result = SOPC_Calloc(1, sizeof(*result));
    struct sockaddr_storage* sockAddrStorage = SOPC_Calloc(1, sizeof(*sockAddrStorage));
    socklen_t sockAddrStorageLen = sizeof(*sockAddrStorage);
    int res = -1;
    if (NULL != result && NULL != sockAddrStorage)
    {
        res = getpeername(sock->sock, (struct sockaddr*) sockAddrStorage, &sockAddrStorageLen);
        if (0 == res)
        {
            result->address.ai_family = sockAddrStorage->ss_family;
            result->address.ai_addrlen = sockAddrStorageLen;
            result->address.ai_addr = (struct sockaddr*) sockAddrStorage;
        }
    }
    if (res != 0)
    {
        SOPC_Free(sockAddrStorage);
        SOPC_Free(result);
        result = NULL;
    }
    return result;
}

SOPC_ReturnStatus SOPC_SocketAddress_GetNameInfo(const SOPC_Socket_Address* addr, char** host, char** service)
{
    /* TODO: getnameinfo is not available in lwip */
    SOPC_UNUSED_ARG(addr);
    SOPC_UNUSED_ARG(host);
    SOPC_UNUSED_ARG(service);
    return SOPC_STATUS_NOT_SUPPORTED;
}

void SOPC_Socket_Clear(SOPC_Socket* sock)
{
    if (NULL != sock && SOPC_INVALID_SOCKET != *sock)
    {
        (*sock)->sock = SOPC_FREERTOS_INVALID_SOCKET_ID;
        SOPC_Free((*sock)->membership);
        (*sock)->membership = NULL;
    }
}

static SOPC_ReturnStatus Socket_Configure(SOPC_Socket sock, bool setNonBlocking)
{
    if (!SOPC_FREERTOS_SOCKET_IS_VALID(sock))
    {
        return SOPC_STATUS_INVALID_PARAMETERS;
    }

    SOPC_ReturnStatus status = SOPC_STATUS_OK;
    const int trueInt = true;

    // Deactivate Nagle's algorithm since we always write a TCP UA binary message (and not just few bytes)
    int setOptStatus = setsockopt(sock->sock, IPPROTO_TCP, TCP_NODELAY, (const void*) &trueInt, sizeof(int));
    if (0 != setOptStatus)
    {
        status = SOPC_STATUS_NOK;
    }

    if (SOPC_STATUS_OK == status && setNonBlocking != false)
    {
        setOptStatus = fcntl(sock->sock, F_SETFL, O_NONBLOCK);
        if (0 != setOptStatus)
        {
            status = SOPC_STATUS_NOK;
        }
    }

    return status;
}

SOPC_ReturnStatus SOPC_Socket_CreateNew(SOPC_Socket_AddressInfo* addr,
                                        bool setReuseAddr,
                                        bool setNonBlocking,
                                        SOPC_Socket* sock)
{
    if (NULL == addr || NULL == sock)
    {
        return SOPC_STATUS_INVALID_PARAMETERS;
    }

    SOPC_ReturnStatus status = SOPC_STATUS_OK;
    int setOptStatus = 0;

    SOPC_Socket result = SOPC_Malloc(sizeof(*result));
    SOPC_ASSERT(NULL != result);
    result->sock = socket(addr->addrInfo.ai_family, addr->addrInfo.ai_socktype, addr->addrInfo.ai_protocol);
    result->membership = NULL;

    if (!SOPC_FREERTOS_SOCKET_IS_VALID(result))
    {
        status = SOPC_STATUS_NOK;
    }
    else
    {
        status = Socket_Configure(result, setNonBlocking);
    }

    if (SOPC_STATUS_OK == status && setReuseAddr != false)
    {
        const int trueInt = true;
        setOptStatus = setsockopt(result->sock, SOL_SOCKET, SO_REUSEADDR, (const void*) &trueInt, sizeof(int));
        if (0 != setOptStatus)
        {
            status = SOPC_STATUS_NOK;
        }
    }

    // Enforce IPV6 sockets can be used for IPV4 connections (if socket is IPV6)
    if (SOPC_STATUS_OK == status && AF_INET6 == addr->addrInfo.ai_family)
    {
#ifdef IPPROTO_IPV6
        const int falseInt = false;
        setOptStatus = setsockopt(result->sock, IPPROTO_IPV6, IPV6_V6ONLY, (const void*) &falseInt, sizeof(int));
        if (0 != setOptStatus)
        {
            status = SOPC_STATUS_NOK;
        }
#else
        status = SOPC_STATUS_NOT_SUPPORTED;
#endif
    }

    if (status != SOPC_STATUS_OK)
    {
        SOPC_Socket_Close(&result);
    }
    else
    {
        *sock = result;
    }

    return status;
}

SOPC_ReturnStatus SOPC_Socket_Listen(SOPC_Socket sock, SOPC_Socket_AddressInfo* addr)
{
    SOPC_ReturnStatus status = SOPC_STATUS_INVALID_PARAMETERS;
    int bindListenStatus = -1;
    if (NULL != addr && SOPC_FREERTOS_SOCKET_IS_VALID(sock))
    {
        bindListenStatus = bind(sock->sock, addr->addrInfo.ai_addr, addr->addrInfo.ai_addrlen);
        if (-1 != bindListenStatus)
        {
            bindListenStatus = listen(sock->sock, SOPC_MAX_PENDING_CONNECTIONS);

            if (-1 == bindListenStatus)
            {
                SOPC_CONSOLE_PRINTF("Listen socket failed with code %d \r\n", (int) errno);
            }
        }
        else
        {
            SOPC_CONSOLE_PRINTF("Bind socket failed with code %d \r\n", (int) errno);
        }
    }
    else
    {
        SOPC_CONSOLE_PRINTF("Null address!\r\n");
    }
    if (-1 != bindListenStatus)
    {
        if (ERR_RTE == errno)
        {
            SOPC_CONSOLE_PRINTF("Error is : Route unreacheable\r\n");
        }
        status = SOPC_STATUS_OK;
    }
    return status;
}

SOPC_ReturnStatus SOPC_Socket_Accept(SOPC_Socket listeningSock, bool setNonBlocking, SOPC_Socket* acceptedSock)
{
    SOPC_ASSERT(NULL != acceptedSock);

    SOPC_ReturnStatus status = SOPC_STATUS_INVALID_PARAMETERS;
    struct sockaddr remoteAddr;
    socklen_t addrLen = sizeof(remoteAddr);
    (*acceptedSock) = NULL;

    if (SOPC_FREERTOS_SOCKET_IS_VALID(listeningSock))
    {
        int sock = accept(listeningSock->sock, &remoteAddr, &addrLen);
        if (SOPC_FREERTOS_INVALID_SOCKET_ID != sock)
        {
            // A new socket must be created.
            *acceptedSock = SOPC_Calloc(1, sizeof(**acceptedSock));
            status = (NULL == *acceptedSock ? SOPC_STATUS_OUT_OF_MEMORY : SOPC_STATUS_OK);
            if (SOPC_STATUS_OK == status)
            {
                (*acceptedSock)->sock = sock;
                status = Socket_Configure(*acceptedSock, setNonBlocking);
                if (SOPC_STATUS_OK != status)
                {
                    close(sock);
                }
            }
            if (SOPC_STATUS_OK != status)
            {
                SOPC_Free(*acceptedSock);
                *acceptedSock = NULL;
            }
        }
    }
    return status;
}

static bool check_socket_connection(int socket_fd)
{
    if (socket_fd < 0)
    {
        return false; // Invalid socket
    }

    // Use select to check if the socket is ready for writing
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(socket_fd, &writefds);

    struct timeval timeout;
    timeout.tv_sec = 5; // 5 seconds timeout
    timeout.tv_usec = 0;

    int result = select(socket_fd + 1, NULL, &writefds, NULL, &timeout);

    if (result > 0 && FD_ISSET(socket_fd, &writefds))
    {
        int error = 0;
        socklen_t error_len = sizeof(error);
        if (getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, &error_len) < 0 || error != 0)
        {
            // Connection failed
            return false;
        }
        else
        {
            // Connection established
            return true;
        }
    }
    else if (result == 0)
    {
        // Timeout occurred
        return false;
    }
    else
    {
        // Error occurred
        return false;
    }
}

SOPC_ReturnStatus SOPC_Socket_Connect(SOPC_Socket sock, SOPC_Socket_AddressInfo* addr)
{
    if (NULL == addr || !SOPC_FREERTOS_SOCKET_IS_VALID(sock))
    {
        return SOPC_STATUS_INVALID_PARAMETERS;
    }

    int connectStatus = -1;
    connectStatus = connect(sock->sock, addr->addrInfo.ai_addr, addr->addrInfo.ai_addrlen);
    if (connectStatus < 0)
    {
        if (EINPROGRESS == errno)
        {
            // Non blocking connection started - check if it is established
            if (check_socket_connection((int) sock->sock))
            {
                connectStatus = 0; // Connection established
            }
            else
            {
                connectStatus = -1; // Connection still in progress
            }
        }
    }
    if (connectStatus == 0)
    {
        return SOPC_STATUS_OK;
    }

    return SOPC_STATUS_NOK;
}

SOPC_ReturnStatus SOPC_Socket_ConnectToLocal(SOPC_Socket from, SOPC_Socket to)
{
    SOPC_ASSERT(SOPC_FREERTOS_SOCKET_IS_VALID(to));
    SOPC_ReturnStatus status = SOPC_STATUS_INVALID_PARAMETERS;
    SOPC_Socket_AddressInfo addr;
    struct sockaddr saddr;
    memset(&addr, 0, sizeof(SOPC_Socket_AddressInfo));
    memset(&saddr, 0, sizeof(struct sockaddr));
    addr.addrInfo.ai_addr = &saddr;
    addr.addrInfo.ai_addrlen = sizeof(struct sockaddr);
    int ret = getsockname(to->sock, addr.addrInfo.ai_addr, &addr.addrInfo.ai_addrlen);
    if (0 == ret)
    {
        status = SOPC_Socket_Connect(from, &addr);
    }

    return status;
}

SOPC_ReturnStatus SOPC_Socket_CheckAckConnect(SOPC_Socket sock)
{
    if (!SOPC_FREERTOS_SOCKET_IS_VALID(sock))
    {
        return SOPC_STATUS_INVALID_PARAMETERS;
    }

    int error = 0;
    socklen_t len = sizeof(int);
    int ret = getsockopt(sock->sock, SOL_SOCKET, SO_ERROR, &error, &len);
    if (ret < 0 || 0 != error)
    {
        return SOPC_STATUS_NOK;
    }
    return SOPC_STATUS_OK;
}

SOPC_SocketSet* SOPC_SocketSet_Create(void)
{
    SOPC_SocketSet* res = SOPC_Calloc(1, sizeof(SOPC_SocketSet));
    if (NULL != res)
    {
        SOPC_SocketSet_Clear(res);
    }
    return res;
}

void SOPC_SocketSet_Delete(SOPC_SocketSet** set)
{
    if (NULL != set)
    {
        SOPC_Free(*set);
        *set = NULL;
    }
}

void SOPC_SocketSet_Add(SOPC_Socket sock, SOPC_SocketSet* sockSet)
{
    if (SOPC_FREERTOS_SOCKET_IS_VALID(sock) && NULL != sockSet)
    {
        FD_SET(sock->sock, &sockSet->set);
        if (sock->sock > sockSet->fdmax)
        {
            sockSet->fdmax = sock->sock;
        }
    }
}

bool SOPC_SocketSet_IsPresent(SOPC_Socket sock, SOPC_SocketSet* sockSet)
{
    if (SOPC_FREERTOS_SOCKET_IS_VALID(sock) && NULL != sockSet)
    {
        if (false == FD_ISSET(sock->sock, &sockSet->set))
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    return false;
}

void SOPC_SocketSet_Clear(SOPC_SocketSet* sockSet)
{
    if (NULL != sockSet)
    {
        FD_ZERO(&sockSet->set);
        sockSet->fdmax = 0;
    }
}

int32_t SOPC_Socket_WaitSocketEvents(SOPC_SocketSet* readSet,
                                     SOPC_SocketSet* writeSet,
                                     SOPC_SocketSet* exceptSet,
                                     uint32_t waitMs)
{
    int32_t nbReady = 0;
    struct timeval timeout;
    struct timeval* val;
    int fdmax = 0;
    if (readSet->fdmax > writeSet->fdmax)
    {
        fdmax = readSet->fdmax;
    }
    else
    {
        fdmax = writeSet->fdmax;
    }
    if (exceptSet->fdmax > fdmax)
    {
        fdmax = exceptSet->fdmax;
    }

    if (waitMs == 0)
    {
        val = NULL;
    }
    else
    {
        timeout.tv_sec = (time_t)(waitMs / 1000);
        timeout.tv_usec = (suseconds_t)(1000 * (waitMs % 1000));
        val = &timeout;
    }
    nbReady = select(fdmax + 1, &readSet->set, &writeSet->set, &exceptSet->set, val);
    return (int32_t) nbReady;
}

SOPC_ReturnStatus SOPC_Socket_Write(SOPC_Socket sock, const uint8_t* data, uint32_t count, uint32_t* sentBytes)
{
    if (!SOPC_FREERTOS_SOCKET_IS_VALID(sock) || NULL == data || count > INT32_MAX || sentBytes == NULL)
    {
        return SOPC_STATUS_INVALID_PARAMETERS;
    }

    ssize_t res = 0;
    res = send(sock->sock, data, count, 0);

    if (res >= 0)
    {
        *sentBytes = (uint32_t) res;
        return SOPC_STATUS_OK;
    }

    *sentBytes = 0;
    if (EAGAIN == errno || EWOULDBLOCK == errno)
    {
        return SOPC_STATUS_WOULD_BLOCK;
    }

    return SOPC_STATUS_NOK;
}

SOPC_ReturnStatus SOPC_Socket_Read(SOPC_Socket sock, uint8_t* data, uint32_t dataSize, uint32_t* readCount)
{
    if (!SOPC_FREERTOS_SOCKET_IS_VALID(sock) || NULL == data || 0 >= dataSize || NULL == readCount)
    {
        return SOPC_STATUS_INVALID_PARAMETERS;
    }

    ssize_t sReadCount = 0;
    sReadCount = recv(sock->sock, data, dataSize, 0);

    if (sReadCount > 0)
    {
        *readCount = (uint32_t) sReadCount;
        return SOPC_STATUS_OK;
    }

    if (sReadCount == 0)
    {
        *readCount = 0;
        return SOPC_STATUS_CLOSED;
    }

    *readCount = 0;
    if (EAGAIN == errno || EWOULDBLOCK == errno)
    {
        return SOPC_STATUS_WOULD_BLOCK;
    }

    return SOPC_STATUS_NOK;
}

SOPC_ReturnStatus SOPC_Socket_BytesToRead(SOPC_Socket sock, uint32_t* bytesToRead)
{
    // TODO: to be implemented
    return SOPC_STATUS_NOK;
}

void SOPC_Socket_Close(SOPC_Socket* pSock)
{
    if (NULL != pSock)
    {
        SOPC_Socket sock = *pSock;
        if (SOPC_FREERTOS_SOCKET_IS_VALID(sock))
        {
            close(sock->sock);
            sock->sock = SOPC_FREERTOS_INVALID_SOCKET_ID;
            SOPC_ASSERT(sock->membership == NULL);
        }
        SOPC_Free(sock);
        *pSock = SOPC_INVALID_SOCKET;
    }
}
