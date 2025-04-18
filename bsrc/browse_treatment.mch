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

MACHINE
    browse_treatment
SEES
    constants,
    constants_statuscodes_bs

DEFINITIONS

    d_variables_in ==
        in_session,
        in_maxReferencesPerNode,
        in_BrowseView,
        in_BrowseValue_NodeId,
        in_BrowseValue_BrowseDirection,
        in_BrowseValue_ReferenceTypeId,
        in_BrowseValue_IncludeSubtypes,
        in_BrowseValue_NodeClassMask,
        in_BrowseValue_ResultMask,
        in_ReleasePrevContinuationPoint,
        isBrowseValueContextDefined;

    d_variables_persist ==
        s_ContinuationPoints,
        id_ContinuationPoint,
        ContinuationPoint_session
    ;

    d_variables_out ==
        out_BrowseResult_BrowseStatus,
        out_BrowseResultReferences,
        out_BrowseResult_nb_References,
        out_BrowseResult_ReferenceTypeId,
        out_BrowseResult_isForward,
        out_BrowseResult_NodeId,
        out_BrowseResult_BrowseName,
        out_BrowseResult_DisplayName,
        out_BrowseResult_NodeClass,
        out_BrowseResult_TypeDefinition
    ;

    d_variables_out_status ==
        isBrowseResultComputed,
        isBrowseResultAllocated
    ;

    d_inv_in ==
        in_session                       : t_session_i           &
        in_maxReferencesPerNode          : NAT                   &
        in_BrowseView                    : t_NodeId_i            &
        in_BrowseValue_NodeId            : t_NodeId_i            &
        in_BrowseValue_BrowseDirection   : t_BrowseDirection_i   &
        in_BrowseValue_ReferenceTypeId   : t_NodeId_i            &
        in_BrowseValue_IncludeSubtypes   : BOOL                  &
        in_BrowseValue_NodeClassMask     : t_BrowseNodeClassMask_i &
        in_BrowseValue_ResultMask        : t_BrowseResultMask_i  &
        in_ReleasePrevContinuationPoint  : BOOL                  &
        isBrowseValueContextDefined      : BOOL
    ;

    d_inv_in_coherence ==
        (isBrowseValueContextDefined = FALSE =>
            in_session                      = c_session_indet            &
            in_maxReferencesPerNode         = 0                          &
            in_BrowseView                   = c_NodeId_indet             &
            in_BrowseValue_NodeId           = c_NodeId_indet             &
            in_BrowseValue_BrowseDirection  = e_bd_indet                 &
            in_BrowseValue_ReferenceTypeId  = c_NodeId_indet             &
            in_BrowseValue_IncludeSubtypes  = FALSE                      &
            in_BrowseValue_NodeClassMask    = c_BrowseNodeClassMask_indet &
            in_BrowseValue_ResultMask       = c_BrowseResultMask_indet   &
            in_ReleasePrevContinuationPoint = FALSE
        )
    ;

    d_inv_persist ==
        s_ContinuationPoints <: t_ContinuationPoint &
        id_ContinuationPoint : s_ContinuationPoints >-> t_ContinuationPointId & // Each continuation has a unique Id
        ContinuationPoint_session : s_ContinuationPoints >-> t_session // a continuation point is associated to a session (=> 1 CP / session)
    ;
    d_inv_out ==
        out_BrowseResult_BrowseStatus        : t_StatusCode_i             &
        out_BrowseResultReferences           : t_BrowseResultReferences_i &
        out_BrowseResult_nb_References       : NAT                        &
        out_BrowseResult_ReferenceTypeId : seq(t_NodeId_i)         &
        out_BrowseResult_isForward       : seq(BOOL)               &
        out_BrowseResult_NodeId          : seq(t_ExpandedNodeId)   &
        out_BrowseResult_BrowseName      : seq(t_QualifiedName_i)  &
        out_BrowseResult_DisplayName     : seq(t_LocalizedText_i)  & /* Spec (v1.04) part 4 table 167 indicates it as mandatory
                                                                        (should be t_LocalizedText),
                                                                        but if node is not present we do not have it*/
        out_BrowseResult_NodeClass       : seq(t_NodeClass_i)      &
        out_BrowseResult_TypeDefinition  : seq(t_ExpandedNodeId_i) &

        size(out_BrowseResult_ReferenceTypeId) = out_BrowseResult_nb_References &
        size(out_BrowseResult_isForward)       = out_BrowseResult_nb_References &
        size(out_BrowseResult_NodeId)          = out_BrowseResult_nb_References &
        size(out_BrowseResult_BrowseName)      = out_BrowseResult_nb_References &
        size(out_BrowseResult_DisplayName)     = out_BrowseResult_nb_References &
        size(out_BrowseResult_NodeClass)       = out_BrowseResult_nb_References &
        size(out_BrowseResult_TypeDefinition)  = out_BrowseResult_nb_References
    ;

    d_inv_out_status ==
        isBrowseResultComputed             : BOOL &
        isBrowseResultAllocated            : BOOL &
        (isBrowseResultComputed = TRUE => isBrowseResultAllocated = TRUE)
    ;

    d_inv_out_coherence ==
        (isBrowseResultComputed = FALSE =>
            out_BrowseResult_BrowseStatus        = c_StatusCode_indet             &
            out_BrowseResultReferences           = c_BrowseResultReferences_indet &
            out_BrowseResult_nb_References     = 0                          &
            out_BrowseResult_ReferenceTypeId   = {}                         &
            out_BrowseResult_isForward         = {}                         &
            out_BrowseResult_NodeId            = {}                         &
            out_BrowseResult_BrowseName        = {}                         &
            out_BrowseResult_DisplayName       = {}                         &
            out_BrowseResult_NodeClass         = {}                         &
            out_BrowseResult_TypeDefinition    = {}
        )
    ;

    d_init_in ==
        d_variables_in
        :(
            in_session                      = c_session_indet            &
            in_maxReferencesPerNode         = 0                          &
            in_BrowseView                   = c_NodeId_indet             &
            in_BrowseValue_NodeId           = c_NodeId_indet             &
            in_BrowseValue_BrowseDirection  = e_bd_indet                 &
            in_BrowseValue_ReferenceTypeId  = c_NodeId_indet             &
            in_BrowseValue_IncludeSubtypes  = FALSE                      &
            in_BrowseValue_NodeClassMask    = c_BrowseNodeClassMask_indet &
            in_BrowseValue_ResultMask       = c_BrowseResultMask_indet   &
            in_ReleasePrevContinuationPoint = FALSE                      &
            isBrowseValueContextDefined     = FALSE
        )
    ;
    d_init_persist ==
        d_variables_persist
        :(
            s_ContinuationPoints = {} &
            id_ContinuationPoint = {} &
            ContinuationPoint_session = {})
    ;
    d_init_out ==
        d_variables_out
        :(
            out_BrowseResult_BrowseStatus        = c_StatusCode_indet             &
            out_BrowseResultReferences           = c_BrowseResultReferences_indet &
            out_BrowseResult_nb_References     = 0                          &
            out_BrowseResult_ReferenceTypeId   = {}                         &
            out_BrowseResult_isForward         = {}                         &
            out_BrowseResult_NodeId            = {}                         &
            out_BrowseResult_BrowseName        = {}                         &
            out_BrowseResult_DisplayName       = {}                         &
            out_BrowseResult_NodeClass         = {}                         &
            out_BrowseResult_TypeDefinition    = {}
        )
    ;

    d_init_out_status ==
        d_variables_out_status :(
            isBrowseResultComputed             = FALSE &
            isBrowseResultAllocated            = FALSE
    )


ABSTRACT_VARIABLES
    d_variables_in,
    d_variables_persist,
    d_variables_out,
    d_variables_out_status

INVARIANT
    d_inv_in &
    d_inv_in_coherence &
    d_inv_persist &
    d_inv_out &
    d_inv_out_status &
    d_inv_out_coherence

INITIALISATION
    d_init_in ||
    d_init_persist ||
    d_init_out ||
    d_init_out_status

OPERATIONS

    p_service_StatusCode <-- set_browse_value_context(p_session, p_maxTargetRef, p_browseView,
                                                      p_nodeId, p_browseDirection, p_referenceType,
                                                      p_includeSubtypes, p_nodeClassMask, p_resultMask,
                                                      p_autoReleaseCP) =
    PRE
        isBrowseValueContextDefined = FALSE         &
        p_session : t_session_i                     &
        p_maxTargetRef : NAT                        &
        p_browseView : t_NodeId_i                   &
        p_nodeId : t_NodeId_i                       &
        p_browseDirection : t_BrowseDirection_i     &
        p_browseDirection : t_BrowseDirection       &
        p_referenceType : t_NodeId_i                &
        p_nodeClassMask : t_BrowseNodeClassMask_i   &
        p_resultMask    : t_BrowseResultMask_i      &
        p_autoReleaseCP : BOOL                      &
        p_includeSubtypes : BOOL
    THEN
        /* Set new input context */
        p_service_StatusCode,
        d_variables_in
        :(
            p_service_StatusCode : {e_sc_ok, e_sc_bad_out_of_memory} &
            d_inv_in &
            d_inv_in_coherence &

            (p_service_StatusCode = e_sc_ok =>
                in_session                       = p_session         &
                in_maxReferencesPerNode          = p_maxTargetRef    &
                in_BrowseView                    = p_browseView      &
                in_BrowseValue_NodeId            = p_nodeId          &
                in_BrowseValue_BrowseDirection   = p_browseDirection &
                in_BrowseValue_ReferenceTypeId   = p_referenceType   &
                in_BrowseValue_IncludeSubtypes   = p_includeSubtypes &
                in_BrowseValue_NodeClassMask     = p_nodeClassMask   &
                in_BrowseValue_ResultMask        = p_resultMask      &
                in_ReleasePrevContinuationPoint  = p_autoReleaseCP   &

                isBrowseValueContextDefined = TRUE) &
            (p_service_StatusCode /= e_sc_ok =>
                isBrowseValueContextDefined = FALSE)
        )
    END
    ;

    p_service_StatusCode <-- set_browse_value_context_from_continuation_point(p_session, p_continuationPointId) =
    PRE
        isBrowseValueContextDefined = FALSE &
        p_session : t_session_i &
        p_session : t_session &
        p_continuationPointId : t_ContinuationPointId_i
    THEN
        /* Set new input context */
        d_variables_in,
        d_variables_persist,
        p_service_StatusCode
        :(  p_service_StatusCode : {e_sc_ok, e_sc_bad_out_of_memory, e_sc_bad_continuation_point_invalid} &
            d_inv_in &
            d_inv_in_coherence &

            isBrowseValueContextDefined = bool(p_service_StatusCode = e_sc_ok) &
            /* If context is set then the continuation point is released */
            (p_service_StatusCode = e_sc_ok =>
                p_continuationPointId : ran(id_ContinuationPoint$0) &
                p_session : ran(ContinuationPoint_session$0) &
                ContinuationPoint_session$0~(p_session) = id_ContinuationPoint$0~(p_continuationPointId) &
                s_ContinuationPoints = s_ContinuationPoints$0 - {ContinuationPoint_session$0~(p_session)} &
                id_ContinuationPoint = {ContinuationPoint_session$0~(p_session)} <<| id_ContinuationPoint$0 &
                ContinuationPoint_session = {ContinuationPoint_session$0~(p_session)} <<| ContinuationPoint_session$0) &
            (p_service_StatusCode /= e_sc_ok =>
                s_ContinuationPoints = s_ContinuationPoints$0 &
                id_ContinuationPoint = id_ContinuationPoint$0 &
                ContinuationPoint_session = ContinuationPoint_session$0)
        )
    END
    ;

    clear_browse_value_context =
    BEGIN
        d_init_in
    END
    ;

    p_serviceStatusCode, p_continuationPointId, p_nbReferences <-- compute_browse_result =
    PRE
        isBrowseValueContextDefined = TRUE &
        isBrowseResultComputed = FALSE     &
        isBrowseResultAllocated = FALSE
    THEN
        d_variables_out,
        d_variables_out_status,
        p_serviceStatusCode,
        p_continuationPointId,
        p_nbReferences,
        s_ContinuationPoints,
        id_ContinuationPoint,
        ContinuationPoint_session
        :(  d_inv_out &
            d_inv_out_status &
            d_inv_out_coherence &

            p_serviceStatusCode : t_StatusCode_i &
            p_serviceStatusCode : t_StatusCode   &
            p_serviceStatusCode : {e_sc_ok, e_sc_bad_out_of_memory, e_sc_bad_node_id_unknown,
                                   e_sc_bad_reference_type_id_invalid, e_sc_bad_no_continuation_points} &
            p_nbReferences : NAT &
            p_continuationPointId : t_ContinuationPointId_i &
            isBrowseResultComputed = bool(p_serviceStatusCode = e_sc_ok or
                                          p_serviceStatusCode = e_sc_bad_no_continuation_points) &

          d_inv_persist
        )
    END
    ;

    /* PROMOTED OPERATIONS */

    bres <-- release_continuation_point (p_session, p_continuationPointId) =
    PRE
        p_session : t_session_i &
        p_session : t_session &
        p_continuationPointId : t_ContinuationPointId_i
    THEN
        s_ContinuationPoints, id_ContinuationPoint, ContinuationPoint_session, bres
        :(
            bres = bool(id_ContinuationPoint(ContinuationPoint_session$0~(p_session)) = p_continuationPointId) &
            /* Release the continuation point  */
            (bres = TRUE =>
                s_ContinuationPoints = s_ContinuationPoints$0 - {ContinuationPoint_session$0~(p_session)} &
                id_ContinuationPoint = {ContinuationPoint_session$0~(p_session)} <<| id_ContinuationPoint$0 &
                ContinuationPoint_session = {ContinuationPoint_session$0~(p_session)} <<| ContinuationPoint_session$0) &
            (bres = FALSE =>
                s_ContinuationPoints = s_ContinuationPoints$0 &
                id_ContinuationPoint = id_ContinuationPoint$0 &
                ContinuationPoint_session = ContinuationPoint_session$0)
        )
    END
    ;

    set_session_closed(p_session) =
    PRE
        p_session : t_session_i &
        p_session : t_session
    THEN
        /* De-associate and de-activate the continuation point associated with session */
        ContinuationPoint_session := ContinuationPoint_session |>> {p_session} ||
        IF p_session : ran(ContinuationPoint_session) THEN
            s_ContinuationPoints := s_ContinuationPoints - {ContinuationPoint_session~(p_session)} ||
            id_ContinuationPoint := {ContinuationPoint_session~(p_session)} <<| id_ContinuationPoint
        END
    END
    ;

    continuation_points_UNINITIALISATION =
    BEGIN
        s_ContinuationPoints := {} ||
        ContinuationPoint_session := {} ||
        id_ContinuationPoint := {}
    END
    ;

    p_refTypeId, p_isForward, p_NodeId, p_BrowseName, p_DisplayName, p_NodeClass, p_TypeDefinition
      <-- getall_browse_result_reference_at(p_refIndex) =
    PRE
        isBrowseResultComputed = TRUE &
        p_refIndex : NAT &
        p_refIndex : 1..out_BrowseResult_nb_References
    THEN
        p_refTypeId :(p_refTypeId : t_NodeId_i & p_refTypeId = out_BrowseResult_ReferenceTypeId(p_refIndex)) ||
        p_isForward := out_BrowseResult_isForward(p_refIndex) ||
        p_NodeId :(p_NodeId : t_ExpandedNodeId_i & p_NodeId = out_BrowseResult_NodeId(p_refIndex)) ||
        p_BrowseName := out_BrowseResult_BrowseName(p_refIndex) ||
        p_DisplayName := out_BrowseResult_DisplayName(p_refIndex) ||
        p_NodeClass := out_BrowseResult_NodeClass(p_refIndex) ||
        p_TypeDefinition := out_BrowseResult_TypeDefinition(p_refIndex)
    END
    ;

    p_nb_references, p_browseResult <-- getall_and_move_browse_result =
    PRE
        isBrowseResultComputed = TRUE
    THEN
        p_nb_references := out_BrowseResult_nb_References ||
        p_browseResult := out_BrowseResultReferences || /* Transfer ownership of BrowseResultReferences to caller */
        d_init_out  ||
        d_init_out_status
    END
    ;

    clear_browse_result =
    BEGIN
        d_init_out ||
        d_init_out_status
    END

END
