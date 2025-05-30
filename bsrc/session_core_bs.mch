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
    session_core_bs

SEES
    constants,
    constants_statuscodes_bs,
    request_handle_bs,
    message_out_bs,
    message_in_bs,
    channel_mgr

ABSTRACT_VARIABLES
    a_client_app_context,
    a_client_token,
    a_server_token,
    a_user_server,
    a_user_client,
    a_user_secu_client,
    a_user_server_cert,
    a_NonceServer,
    a_NonceClient

INVARIANT
    a_client_app_context         : t_session +-> t_session_application_context_i &
    a_client_token               : t_session       +-> t_session_token         &
    a_server_token               : t_session_token +-> t_session               &
    a_user_server                : t_session       +-> t_user                  &
    a_user_client                : t_session       +-> t_user_token            &
    a_user_secu_client           : t_session       +-> t_SecurityPolicy        &
    a_user_server_cert           : t_session       +-> t_byte_buffer           &
    a_NonceServer                : t_session       +-> t_Nonce                 &
    a_NonceClient                : t_session       +-> t_Nonce

INITIALISATION
    a_client_app_context         :: {{}} ||
    a_client_token               :: {{}} ||
    a_server_token               :: {{}} ||
    a_user_server                :: {{}} ||
    a_user_client                :: {{}} ||
    a_user_secu_client           :: {{}} ||
    a_user_server_cert           :: {{}} ||
    a_NonceServer                :: {{}} ||
    a_NonceClient                :: {{}}

OPERATIONS

    delete_session_token(p_session, p_is_client) =
    PRE
        p_is_client : BOOL &
        p_session : t_session_i &
        p_session : t_session
    THEN
        a_user_server := {p_session} <<| a_user_server           || /* If session had a user, remove this relation */
        a_user_client := {p_session} <<| a_user_client           ||
        a_user_secu_client := {p_session} <<| a_user_secu_client ||
        a_user_server_cert := {p_session} <<| a_user_server_cert ||
        a_server_token := a_server_token |>> {p_session}         || /* If session had token, remove this relation */
        a_client_token := {p_session} <<| a_client_token
    END
    ;

    delete_session_application_context(p_session) =
    PRE
        p_session : t_session_i &
        p_session : t_session
    THEN
        a_client_app_context := {p_session} <<| a_client_app_context
    END
    ;

    session <-- server_get_session_from_token (session_token) =
    PRE
        session_token : t_session_token_i
    THEN
        IF  session_token : dom(a_server_token)
        THEN
            session :(session : t_session_i & session = a_server_token(session_token))
        ELSE
            session := c_session_indet
        END
    END
    ;

    session_token <-- client_get_token_from_session (session) =
    PRE
        session : t_session_i &
        session : t_session &
        session : dom(a_client_token)
    THEN
        /* Guarantee only 1 session token by session provided by set_session_token PRE */
        session_token :(session_token : t_session_token_i &
                        session_token = a_client_token(session))
    END
    ;

    token <-- server_get_fresh_session_token (p_channel_config_idx, session) =
    PRE
        p_channel_config_idx : t_channel_config_idx_i &
        p_channel_config_idx : t_channel_config_idx &
        session : t_session_i &
        session : t_session &
        session /: ran(a_server_token)
    THEN
        IF t_session_token = dom(a_server_token)
        THEN
            token := c_session_token_indet
        ELSE
            ANY l_ntoken WHERE
                l_ntoken : t_session_token - dom(a_server_token)
            THEN
                a_server_token (l_ntoken) := session ||
                token := l_ntoken
            END
        END
    END
    ;

    ret <-- server_is_valid_session_token (token) =
    PRE
        token : t_session_token_i
    THEN
        ret := bool(token : dom(a_server_token))
    END
    ;

    client_set_session_token (session, token) =
    PRE
        session : t_session_i &
        session : t_session &
        session /: dom(a_client_token) &
        token : t_session_token_i &
        token : t_session_token
    THEN
        a_client_token (session) := token
    END
    ;

    notify_set_session_state (session, prec_state, state, sc_reason, is_client) =
    PRE
        session    : t_session_i    &
        session    : t_session      &
        prec_state : t_sessionState_i &
        state      : t_sessionState_i &
        sc_reason : t_StatusCode_i  &
        (state = e_session_closed => sc_reason : t_StatusCode) &
        is_client : BOOL
    THEN
        /* Generates events on session state change (application, internal events) */
        skip
    END
    ;

    drop_user_server (p_session) =
    PRE
        p_session : t_session_i &
        p_session : t_session
    THEN
        a_user_server := {p_session} <<| a_user_server
        /* Also deallocate user */
    END
    ;

    set_session_user_server (session, p_user) =
    PRE
        session : t_session_i &
        session : t_session   &
        p_user  : t_user_i    &
        p_user  : t_user
    THEN
        a_user_server (session) := p_user
    END
    ;

    set_session_user_client (session, p_user_token) =
    PRE
        session      : t_session_i    &
        session      : t_session      &
        p_user_token : t_user_token_i &
        p_user_token : t_user_token
    THEN
        a_user_client (session) := p_user_token
    END
    ;

    ret <-- session_has_user_server(p_session) =
    PRE
        p_session : t_session_i &
        p_session : t_session
    THEN
        ret := bool(p_session : dom(a_user_server))
    END
    ;

    p_user <-- get_session_user_server (session) =
    PRE
        session : t_session_i &
        session : t_session   &
        session : dom(a_user_server)
    THEN
        p_user :(p_user : t_user_i &
                 p_user : t_user &
                 p_user = a_user_server (session))
    END
    ;

    p_user_token <-- get_session_user_client (session) =
    PRE
        session : t_session_i &
        session : t_session   &
        session : dom(a_user_client)
    THEN
        p_user_token :(p_user_token : t_user_token_i &
                       p_user_token = a_user_client (session))
    END
    ;

    p_user_secu <-- get_session_user_secu_client (session) =
    PRE
        session : t_session_i &
        session : t_session   &
        session : dom(a_user_secu_client)
    THEN
        p_user_secu := a_user_secu_client (session)
    END
    ;

    p_user_server_cert <-- get_session_user_server_certificate(session) =
    PRE
        session : t_session_i &
        session : t_session   &
        session : dom(a_user_server_cert)
    THEN
        p_user_server_cert :(p_user_server_cert : t_byte_buffer_i &
                             p_user_server_cert = a_user_server_cert (session))
    END
    ;

    p_bres <-- is_same_user_server (p_user_left, p_user_right) =
    PRE
        p_user_left : t_user_i &
        p_user_left : t_user &
        p_user_right : t_user_i &
        p_user_right : t_user
    THEN
        p_bres :: BOOL
    END
    ;

    p_bres, p_nonce <-- server_set_fresh_nonce(p_session, p_channel_config_idx) =
    PRE
        p_session : t_session_i &
        p_session : t_session &
        p_channel_config_idx : t_channel_config_idx_i &
        p_channel_config_idx : t_channel_config_idx
    THEN
        p_bres,
        p_nonce,
        a_NonceServer
        :(
            p_bres : BOOL &
            p_nonce : t_Nonce_i &
            (p_bres = TRUE
             =>
             a_NonceServer(p_session) : t_Nonce &
             {p_session} <<| a_NonceServer = {p_session} <<| a_NonceServer$0 &
             p_nonce = a_NonceServer(p_session)) &
            (p_bres = FALSE
             =>
             a_NonceServer = a_NonceServer$0 &
             p_nonce = c_Nonce_indet)
         )
    END
    ;

    /* This implements the crypto challenge in CreateSession.
     * valid tracks the alloc and success of signature
     */
    status, signature <-- server_create_session_req_do_crypto(p_req_msg, p_endpoint_config_idx, p_channel_config_idx) =
    PRE
        p_req_msg : t_msg_i &
        p_req_msg = c_msg_in &
        c_msg_in : t_msg &
        a_msg_in_type = e_msg_session_create_req &
        p_endpoint_config_idx : t_endpoint_config_idx_i &
        p_endpoint_config_idx : t_endpoint_config_idx &
        p_channel_config_idx : t_channel_config_idx_i &
        p_channel_config_idx : t_channel_config_idx
    THEN
        status,
        signature
        :(
            status        : t_StatusCode_i        &
            signature     : t_SignatureData_i     &

            (status = e_sc_ok
             =>
             signature : t_SignatureData) &
            (status /= e_sc_ok
             =>
             signature = c_SignatureData_indet)
         )
    END
    ;

    /* Returns true if user token policies for the current SC security policy includes possible user token encryption */
    p_bres <-- server_may_need_user_token_encryption(p_endpoint_config_idx, p_channel_config_idx) =
    PRE
        p_endpoint_config_idx : t_endpoint_config_idx_i &
        p_endpoint_config_idx : t_endpoint_config_idx &
        p_channel_config_idx : t_channel_config_idx_i &
        p_channel_config_idx : t_channel_config_idx
    THEN
        p_bres :: BOOL
    END
    ;

    valid_cert <-- may_validate_server_certificate(p_session, p_channel_config_idx, p_user_server_cert, p_user_secu_policy) =
    PRE
        p_session : t_session_i &
        p_session : t_session &
        p_channel_config_idx : t_channel_config_idx_i &
        p_channel_config_idx : t_channel_config_idx &
        p_user_server_cert : t_byte_buffer_i &
        p_user_server_cert = a_user_server_cert (p_session) &
        p_user_secu_policy : t_SecurityPolicy &
        p_user_secu_policy /= e_secpol_None
    THEN
        valid_cert :: BOOL
    END
    ;

    valid, signature <-- client_activate_session_req_do_crypto(session, channel_config_idx, server_nonce) =
    PRE
        session            : t_session_i            &
        session            : t_session              &
        channel_config_idx : t_channel_config_idx_i &
        channel_config_idx : t_channel_config_idx   &
        server_nonce       : t_Nonce_i              &
        server_nonce       : t_Nonce                &
        server_nonce       = a_NonceServer(session)
    THEN
        valid,
        signature,
        a_NonceClient
        :(
            valid : BOOL &
            signature : t_SignatureData_i &
            a_NonceClient : t_session +-> t_Nonce & /* PROOF added, how is it modified? */

            (valid = TRUE
             =>
             signature : t_SignatureData)
            &
            (valid = FALSE
             =>
             signature = c_SignatureData_indet)
        )
    END
    ;

    p_user_token_signature <-- sign_user_token(p_user_server_cert, p_server_nonce, p_user_secu_policy, app_context) =
    PRE
        p_user_server_cert   : t_byte_buffer_i      &
        p_server_nonce       : t_Nonce_i            &
        p_server_nonce       : t_Nonce              &
        p_user_secu_policy   : t_SecurityPolicy     &
        p_user_secu_policy   /= e_secpol_None       &
        app_context          : t_session_application_context_i
    THEN
        p_user_token_signature :: t_SignatureData_i
    END
    ;

    clear_Signature(p_signature) =
    PRE
        p_signature : t_SignatureData_i &
        p_signature : t_SignatureData
    THEN
        skip
    END
    ;

    client_set_NonceServer(p_session, p_resp_msg) =
    PRE
        p_session : t_session_i &
        p_session : t_session   &
        p_resp_msg : t_msg_i &
        p_resp_msg = c_msg_in &
        c_msg_in : t_msg &
        a_msg_in_type = e_msg_session_create_resp
    THEN
        a_NonceServer
        :(a_NonceServer : t_session +-> t_Nonce &
            {p_session} <<| a_NonceServer = {p_session} <<| a_NonceServer$0 &
            p_session : dom(a_NonceServer)
        )
    END
    ;

    nonce <-- get_NonceServer(p_session, p_is_client) =
    PRE
        p_session : t_session_i &
        p_session : t_session   &
        p_session : dom(a_NonceServer) &
        p_is_client : BOOL
    THEN
        nonce :(
            nonce : t_Nonce_i &
            nonce = a_NonceServer(p_session)
        )
    END
    ;

    valid, nonce_needed <-- client_create_session_req_do_crypto(p_session, p_channel, p_channel_config_idx) =
    PRE
        p_session : t_session_i &
        p_session : t_session   &
        p_channel : t_channel_i &
        p_channel : s_channel_connected &
        p_channel_config_idx : t_channel_config_idx_i &
        p_channel_config_idx : t_channel_config_idx
    THEN
        IF a_SecurityPolicy(p_channel) = e_secpol_None THEN
            valid :: BOOL || /* This fails when we cannot retreive the SecurityPolicy  <-- but this should not be possible */
            nonce_needed := FALSE
        ELSE
            nonce_needed := TRUE ||
            valid,
            a_NonceClient
            :(
                valid : BOOL &
                a_NonceClient : t_session +-> t_Nonce &

                (valid = TRUE
                 =>
                 {p_session} <<| a_NonceClient = {p_session} <<| a_NonceClient$0 &
                 a_NonceClient(p_session) : t_Nonce) &
                (valid = FALSE
                 =>
                 a_NonceClient = a_NonceClient$0)
             )
        END
    END
    ;

    nonce <-- get_NonceClient(p_session) =
    PRE
        p_session : t_session_i &
        p_session : t_session   &
        p_session : dom(a_NonceClient)
    THEN
        nonce :(
            nonce : t_Nonce_i &
            nonce = a_NonceClient(p_session)
        )
    END
    ;

    // Nonce client only used by client
    drop_NonceClient(p_session) =
    PRE
        p_session : t_session_i &
        p_session : t_session
        /* PROOF  &
        p_session : dom(a_NonceClient) */
    THEN
        a_NonceClient := {p_session} <<| a_NonceClient
    END
    ;

    remove_NonceServer(p_session, p_is_client) =
    PRE
        p_session : t_session_i &
        p_session : t_session &
        p_is_client : BOOL
    THEN
        a_NonceServer := {p_session} <<| a_NonceServer
    END
    ;

    p_valid <-- client_create_session_set_user_token_secu_properties (p_session, p_channel_config_idx, p_resp_msg) =
    PRE
        p_session : t_session_i &
        p_session : t_session &
        p_session : dom(a_user_client) &
        p_channel_config_idx : t_channel_config_idx_i &
        p_channel_config_idx : t_channel_config_idx &
        p_resp_msg : t_msg_i &
        p_resp_msg = c_msg_in &
        c_msg_in : t_msg &
        a_msg_in_type = e_msg_session_create_resp
    THEN
        p_valid,
        a_user_secu_client,
        a_user_server_cert
        :(  p_valid : BOOL &
            a_user_secu_client : t_session +-> t_SecurityPolicy &
            a_user_server_cert : t_session +-> t_byte_buffer &
            (   p_valid = TRUE
                =>
                {p_session} <<| a_user_secu_client = {p_session} <<| a_user_secu_client$0 &
                p_session : dom(a_user_secu_client) &
                {p_session} <<| a_user_server_cert = {p_session} <<| a_user_server_cert$0 &
                p_session : dom(a_user_server_cert)
            ) &
            (   p_valid = FALSE
                =>
                a_user_secu_client = a_user_secu_client$0 &
                a_user_server_cert = a_user_server_cert$0
            )
        )
    END
    ;

    /* This function is not called when there is no client nonce (ie SC:Sec_pol is None) */
    p_valid <-- client_create_session_check_crypto(p_session, p_channel_config_idx, p_resp_msg) =
    PRE
        p_session : t_session_i &
        p_session : t_session   &
        p_session : dom(a_NonceClient) &
        p_channel_config_idx : t_channel_config_idx_i &
        p_channel_config_idx : t_channel_config_idx &
        p_resp_msg : t_msg_i &
        p_resp_msg = c_msg_in &
        c_msg_in : t_msg &
        a_msg_in_type = e_msg_session_create_resp
    THEN
        p_valid,
        a_NonceServer
        :(  p_valid : BOOL &
            a_NonceServer : t_session +-> t_Nonce &

            (   p_valid = TRUE
                =>
                {p_session} <<| a_NonceServer = {p_session} <<| a_NonceServer$0 &
                p_session : dom(a_NonceServer)
            ) &
            (   p_valid = FALSE
                =>
                a_NonceServer = a_NonceServer$0
            )
        )
    END
    ;

    valid <-- client_activate_session_resp_check(p_session, p_channel_config_idx, p_resp_msg) =
    PRE
        p_session : t_session_i &
        p_session : t_session &
        p_channel_config_idx : t_channel_config_idx_i &
        p_channel_config_idx : t_channel_config_idx &
        p_resp_msg : t_msg_i &
        p_resp_msg = c_msg_in &
        c_msg_in : t_msg &
        a_msg_in_type = e_msg_session_activate_resp
    THEN
        valid :: BOOL
    END
    ;

    p_valid <-- server_activate_session_check_crypto(p_endpoint_config_idx, p_session, p_channel, p_channel_config_idx, p_activate_req_msg) =
    PRE
        p_endpoint_config_idx : t_endpoint_config_idx_i &
        p_endpoint_config_idx : t_endpoint_config_idx &
        p_session : t_session_i &
        p_session : t_session   &
        p_session : dom(a_NonceClient) &
        p_channel : t_channel_i &
        p_channel : s_channel_connected &
        a_SecurityPolicy(p_channel) /= e_secpol_None &
        p_channel_config_idx : t_channel_config_idx_i &
        p_channel_config_idx : t_channel_config_idx &
        p_activate_req_msg : t_msg_i &
        p_activate_req_msg = c_msg_in &
        c_msg_in : t_msg &
        a_msg_in_type = e_msg_session_activate_req
    THEN
        p_valid :: BOOL
    END;

    client_close_session_req_msg (req_msg) =
    PRE
        req_msg : t_msg_i &
        req_msg = c_msg_in &
        c_msg_in : t_msg &
        a_msg_in_type = e_msg_session_close_req
    THEN
        skip
    END;

    client_close_session_resp_msg (resp_msg) =
    PRE
        resp_msg : t_msg_i &
        resp_msg = c_msg_out &
        c_msg_out : t_msg &
        a_msg_out_type = e_msg_session_close_resp
    THEN
        skip
    END;

    server_close_session_check_req (req_msg, resp_msg) =
    PRE
        req_msg : t_msg_i &
        req_msg = c_msg_in &
        c_msg_in : t_msg &
        a_msg_in_type = e_msg_session_close_req &
        resp_msg : t_msg_i &
        resp_msg = c_msg_out &
        c_msg_out : t_msg &
        a_msg_out_type = e_msg_session_close_resp
    THEN
        /* TODO: return delete subscription boolean value for subscription service */
        skip
    END;

    session_do_nothing (session) =
    PRE
        session : t_session_i &
        session : t_session
    THEN
        skip
    END
    ;

    set_session_app_context(p_session, p_app_context) =
    PRE
        p_session     : t_session_i    &
        p_session     : t_session      &
        p_app_context : t_session_application_context_i
    THEN
        a_client_app_context(p_session) := p_app_context
    END
    ;

    p_app_context <-- get_session_app_context(p_session) =
    PRE
        p_session     : t_session_i &
        p_session     : t_session
    THEN
        p_app_context :(p_app_context : t_session_application_context_i & p_app_context = a_client_app_context(p_session))
    END
    ;

    /* Generate internal event to create given session on event treatment.
       Used to transform multiple sessions treatment into single session treatment
    */
    client_gen_create_session_internal_event (session, channel_config_idx) =
    PRE
        session : t_session_i &
        session : t_session &
        channel_config_idx : t_channel_config_idx_i &
        channel_config_idx : t_channel_config_idx
    THEN
        skip
    END
    ;

    /* Generate internal event to activate given orphaned session on event treatment.
       Used to transform multiple sessions treatment into single session treatment
    */
    client_gen_activate_orphaned_session_internal_event (session, channel_config_idx) =
    PRE
        session : t_session_i &
        session : t_session &
        channel_config_idx : t_channel_config_idx_i &
        channel_config_idx : t_channel_config_idx
    THEN
        skip
    END
    ;

    /* Generate internal event to activate given session on event treatment.
       Used to transform multiple sessions treatment into single session treatment
    */
    client_gen_activate_user_session_internal_event (session, p_user_token) =
    PRE
        session : t_session_i &
        session : t_session &
        p_user_token : t_user_token_i &
        p_user_token : t_user_token
    THEN
        skip
    END
    ;

    timer_created <-- server_session_timeout_start_timer (session, resp_msg) =
    PRE
        session : t_session_i &
        session : t_session &
        resp_msg : t_msg_i &
        resp_msg = c_msg_in &
        c_msg_in : t_msg &
        a_msg_in_type = e_msg_session_create_resp
    THEN
        /* Use resp message to retrieve revised session timeout and activate first timer:
           - trigger TIMER_SE_EVAL_SESSION_TIMEOUT*/
        timer_created :: BOOL
    END
    ;

    server_session_timeout_stop_timer (session) =
    PRE
        session : t_session_i &
        session : t_session &
        c_msg_in : t_msg &
        a_msg_in_type = e_msg_session_create_resp
    THEN
        /* To be called when session closed */
        skip
    END
    ;

    expired <-- server_session_timeout_evaluation (session) =
    PRE
        session : t_session_i &
        session : t_session
    THEN
        /* Evaluate if session timeout occured:
        - Evaluate timeout: Current timeref - Latest message timeref = Elapsed time without message
          * if > session lifetime => expired := TRUE
          * otherwise             => expired := FALSE
        - Otherwise: re-activate timer for: revised session timeout - elapsed time since latest message
        */
        expired :: BOOL
    END
    ;

    server_session_timeout_msg_received (session) =
    PRE
        session : t_session_i &
        session : t_session
    THEN
        /* Record TimeReference of the latest received message */
        skip
    END

END
