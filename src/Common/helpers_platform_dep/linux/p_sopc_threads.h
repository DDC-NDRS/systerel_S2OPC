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

#ifndef SOPC_P_THREADS_H_
#define SOPC_P_THREADS_H_

#include <pthread.h>

#include "sopc_mutexes.h"
#include "sopc_threads.h"

/** The linux actual implementation of Mutex */
struct SOPC_Mutex_Impl
{
    pthread_mutex_t mutex;
};

/** The linux actual implementation of Condition variables */
struct SOPC_Condition_Impl
{
    pthread_cond_t cond;
};

typedef pthread_t SOPC_Thread;

#endif /* SOPC_P_THREADS_H_ */
