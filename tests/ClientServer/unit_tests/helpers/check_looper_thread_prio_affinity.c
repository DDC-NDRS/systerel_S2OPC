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
 * \brief Looper thread priority and affinity readback tests (libcheck).
 *
 * Dedicated binary (check_looper_thread_prio_affinity) that checks SOPC
 * looper thread configuration set by SOPC_Looper_CreatePrioritized.
 *
 * Test flow (Linux):
 * 1. Create a looper with a known priority and CPU index.
 * 2. Post an event handled on the looper thread.
 * 3. In the callback, read back scheduling policy, priority, and CPU affinity
 *    with pthread/sched APIs and compare to the values passed at creation.
 * 4. Signal the main test thread and assert readback status codes are zero.
 *
 * Priority 0 (default SCHED_OTHER) is used so CI runs without CAP_SYS_NICE.
 * Glibc exposes cpu_set_t helpers and pthread_getaffinity_np when _GNU_SOURCE
 * is defined (see CMakeLists.txt for this target). On non-Linux platforms a
 * placeholder test is registered so TAP output remains valid (plan 1..1).
 */

#include <check.h>
#include <stdbool.h>
#include <stdlib.h> /* EXIT_* */

#include "sopc_atomic.h"
#include "sopc_event_handler.h"
#include "sopc_macros.h"
#include "sopc_threads.h"

Suite* tests_make_suite_looper_thread_prio_affinity(void);

#ifdef __linux__

#include <pthread.h>
#include <sched.h>

/* Handoff from looper callback to main test thread (see wait_value). */
static int32_t g_affinityAndPrioDone = 0;
/* Affinity readback: 0 = OK, -1 = pthread_getaffinity_np failed, -2 = CPU not set,
 * -3 = more than one CPU in mask (expected single-CPU pin). */
static int32_t g_affinityStatus = 0;
/* The CPU expected to be used: minimum 2 CPUs on target in order this test makes sense */
static int g_expectedCpu = 2;
/* Priority 0 keeps SCHED_OTHER; values > 0 need RT privileges (CAP_SYS_NICE). */
static const int g_expectedPriority = 0;
/* Priority readback: 0 = OK, -1 = policy or RT priority mismatch. */
static int32_t g_prioStatus = 0;

static bool wait_value(int32_t* atomic, int32_t val)
{
    for (int i = 0; i < 100; ++i)
    {
        int32_t x = SOPC_Atomic_Int_Get(atomic);

        if (x == val)
        {
            return true;
        }

        SOPC_Sleep(10);
    }

    return false;
}

static void affinity_readback_callback(SOPC_EventHandler* handler,
                                       int32_t event,
                                       uint32_t eltId,
                                       uintptr_t params,
                                       uintptr_t auxParam)
{
    cpu_set_t cpuset;
    int ret = 0;
    int policy = 0;
    struct sched_param param;

    SOPC_UNUSED_ARG(handler);
    SOPC_UNUSED_ARG(event);
    SOPC_UNUSED_ARG(eltId);
    SOPC_UNUSED_ARG(params);
    SOPC_UNUSED_ARG(auxParam);

    /* cpu_set_t: bitmask of CPUs a thread may run on (glibc, needs _GNU_SOURCE). */
    CPU_ZERO(&cpuset); /* Clear all bits in cpuset before readback. */
    /* pthread_getaffinity_np: GNU extension, read affinity of pthread_self(). */
    ret = pthread_getaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
    if (0 != ret)
    {
        g_affinityStatus = -1;
    }
    /* CPU_ISSET: true if cpu index is allowed in the cpuset mask. */
    else if (!CPU_ISSET((size_t) g_expectedCpu, &cpuset))
    {
        g_affinityStatus = -2;
    }
    /* CPU_COUNT: GNU macro, number of CPUs set in cpuset (expect 1 for a pin). */
    else if (1 != CPU_COUNT(&cpuset))
    {
        g_affinityStatus = -3;
    }
    else
    {
        g_affinityStatus = 0;
    }

    /* pthread_getschedparam: POSIX, read scheduling policy and sched_param of current thread. */
    if (0 == g_expectedPriority || (0 == pthread_getschedparam(pthread_self(), &policy, &param) &&
                                    /* SCHED_FIFO: RT policy used by SOPC when priority > 0. */
                                    policy == SCHED_FIFO && param.sched_priority == g_expectedPriority))
    {
        g_prioStatus = 0;
    }
    else
    {
        g_prioStatus = -1;
    }

    SOPC_Atomic_Int_Set(&g_affinityAndPrioDone, 1);
}

START_TEST(test_looper_create_prioritized_affinity_readback)
{
    SOPC_Looper* looper = NULL;
    SOPC_EventHandler* handler = NULL;
    SOPC_ReturnStatus status = SOPC_STATUS_NOK;

    SOPC_Atomic_Int_Set(&g_affinityAndPrioDone, 0);
    g_affinityStatus = 0;

    /* Third argument: CPU index; second: thread priority (0 = default). */
    looper = SOPC_Looper_CreatePrioritized("affinity_rb", g_expectedPriority, g_expectedCpu);
    ck_assert_ptr_nonnull(looper);

    handler = SOPC_EventHandler_Create(looper, affinity_readback_callback);
    ck_assert_ptr_nonnull(handler);

    /* Runs affinity_readback_callback on the looper thread, not here. */
    status = SOPC_EventHandler_Post(handler, 0, 0, 0, 0);
    ck_assert_int_eq(SOPC_STATUS_OK, status);

    ck_assert(wait_value(&g_affinityAndPrioDone, 1));
    ck_assert_int_eq(0, g_affinityStatus);
    ck_assert_int_eq(0, g_prioStatus);

    SOPC_Looper_Delete(looper);
}
END_TEST

#else /* __linux__ */

START_TEST(test_looper_prio_affinity_not_supported)
{
    /* Affinity readback uses Linux/glibc APIs; keeps TAP plan at 1..1 on other OS. */
}
END_TEST

#endif /* __linux__ */

Suite* tests_make_suite_looper_thread_prio_affinity(void)
{
    Suite* s = suite_create("Looper thread prio affinity");
    TCase* c = tcase_create("Looper thread prio affinity");

#ifdef __linux__
    tcase_add_test(c, test_looper_create_prioritized_affinity_readback);
#else
    tcase_add_test(c, test_looper_prio_affinity_not_supported);
#endif
    suite_add_tcase(s, c);

    return s;
}

int main(void)
{
    int number_failed;
    SRunner* sr;

    sr = srunner_create(tests_make_suite_looper_thread_prio_affinity());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
