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

 File Name            : translate_browse_path_source_1.h

 Date                 : 24/07/2023 14:29:23

 C Translator Version : tradc Java V1.2 (06/02/2022)

******************************************************************************/

#ifndef _translate_browse_path_source_1_h
#define _translate_browse_path_source_1_h

/*--------------------------
   Added by the Translator
  --------------------------*/
#include "b2c.h"

/*--------------
   SEES Clause
  --------------*/
#include "constants.h"
#include "constants_statuscodes_bs.h"

/*----------------------------
   CONCRETE_VARIABLES Clause
  ----------------------------*/
extern t_entier4 translate_browse_path_source_1__BrowsePathSource_size_i;
extern constants__t_NodeId_i translate_browse_path_source_1__BrowsePathSource_tab_i[constants__t_BrowsePathResPerElt_i_max+1];

/*------------------------
   INITIALISATION Clause
  ------------------------*/
extern void translate_browse_path_source_1__INITIALISATION(void);

/*--------------------
   OPERATIONS Clause
  --------------------*/
extern void translate_browse_path_source_1__add_BrowsePathSource(
   const constants__t_NodeId_i translate_browse_path_source_1__nodeId);
extern void translate_browse_path_source_1__get_BrowsePathSource(
   const t_entier4 translate_browse_path_source_1__index,
   constants__t_NodeId_i * const translate_browse_path_source_1__nodeId);
extern void translate_browse_path_source_1__get_BrowsePathSourceSize(
   t_entier4 * const translate_browse_path_source_1__res);
extern void translate_browse_path_source_1__init_BrowsePathSource(void);

#endif
