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

/******************************************************************************

 File Name            : namespace_array_it.h

 Date                 : 16/10/2024 09:42:47

 C Translator Version : tradc Java V1.2 (06/02/2022)

******************************************************************************/

#ifndef _namespace_array_it_h
#define _namespace_array_it_h

/*--------------------------
   Added by the Translator
  --------------------------*/
#include "b2c.h"

/*--------------
   SEES Clause
  --------------*/
#include "constants.h"

/*----------------------------
   CONCRETE_VARIABLES Clause
  ----------------------------*/
extern t_entier4 namespace_array_it__currentnamespaceUriIdx_i;
extern t_entier4 namespace_array_it__nb_namespaceUris_i;

/*------------------------
   INITIALISATION Clause
  ------------------------*/
extern void namespace_array_it__INITIALISATION(void);

/*--------------------
   OPERATIONS Clause
  --------------------*/
extern void namespace_array_it__continue_iter_namespaceUris(
   t_bool * const namespace_array_it__p_continue,
   t_entier4 * const namespace_array_it__p_namespaceUriIdx);
extern void namespace_array_it__init_iter_namespaceUris(
   const t_entier4 namespace_array_it__p_nb_namespaceUris,
   t_bool * const namespace_array_it__p_continue);

#endif
