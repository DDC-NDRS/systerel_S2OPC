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

/** \file samples_platform_dep.h
 *
 * \brief Provides the abstraction of OS-dependant features required for samples
 */

#ifndef SAMPLES_PLATFORM_DEP_H_
#define SAMPLES_PLATFORM_DEP_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef PRINT
#if SOPC_PIKEOS
#include <vm.h>

#define PRINT vm_cprintf
#else
#define PRINT printf
#endif
#endif

/**
 * Specific platform must provide the software entry point
 * that will call the applicative ::SOPC_Platform_Main function.
 * Generally this is the ::main function (but name/prototype may vary).
 *
 * The ::SOPC_Platform_Main entry point must be defined for each sample.
 */
extern void SOPC_Platform_Main(void);

/**
 * \brief Specific platform setup initialization.
 * Typical usage: setup network if not set-up by O.S.
 * \post The network must be up and interfaces ready.
 */
void SOPC_Platform_Setup(void);

/**
 * \brief Specific platform shut down.
 * \param reboot True to request a reboot, false for a shutdown.
 */
void SOPC_Platform_Shutdown(const bool reboot);

/**
 * \brief Specific platform: name of the default network interface.
 */
const char* SOPC_Platform_Get_Default_Net_Itf(void);

/**
 * \brief Read a line from the console.
 *
 * \return a new allocated string read from the console, or NULL in case of success
 *
 */
char* SOPC_Shell_ReadLine(void);

/**
 * \brief Display some target-specific debug information.
 * There are no expectations in this implementation which can be empty.
 * It is intended to allow the host application to display some OS-internal
 * informations.
 * \param param An optional parameter passed (specific to each implementation)
 */
void SOPC_Platform_Target_Debug(const char* param);

#endif /* SAMPLES_PLATFORM_DEP_H_ */
