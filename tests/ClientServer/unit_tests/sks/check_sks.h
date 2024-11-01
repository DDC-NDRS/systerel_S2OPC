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
 * \brief Tests suites are gathered here.
 * Inspired from https://github.com/libcheck/check/blob/master/tests/check_check.h
 */

#ifndef CHECK_SKS_H
#define CHECK_SKS_H

#include <stdbool.h>
#include <stdint.h>

#include <check.h>

bool wait_value(int32_t* atomic, int32_t val);

Suite* tests_make_suite_manager(void);

#endif // CHECK_SKS_H
