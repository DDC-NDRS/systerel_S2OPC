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

/** Hard-coded configuration */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdbool.h>

#define xstr(s) str(s)
#define str(s) #s

/* Server Configuration */

#define SERVER_CERT_PATH "./server_public/server_2k_cert.der"
#define SERVER_KEY_PATH "./server_private/encrypted_server_2k_key.pem"
#define ENCRYPTED_SERVER_KEY true /* set to false otherwise */
#define PASSWORD_ENV_NAME "TEST_PASSWORD_PRIVATE_KEY"
#define USER_PASSWORD_ENV_NAME "TEST_PASSWORD_USER_SECUADMIN"
#define CLIENT_CERT_PATH "./client_public/client_2k_cert.der"
#define CLIENT_KEY_PATH "./client_private/encrypted_client_2k_key.pem"
#define ENCRYPTED_CLIENT_KEY true /* set to false otherwise */
#define DEFAULT_ENDPOINT_URL "opc.tcp://localhost:4843"
#define PKI_PATH "./S2OPC_Demo_PKI"
#define APPLICATION_URI "urn:S2OPC:localhost"
#define PRODUCT_URI "urn:S2OPC:localhost:product"
#define SERVER_DESCRIPTION "S2OPC Server + PubSub"
#define CLIENT_DESCRIPTION "S2OPC Client for SKS"
#define ADDRESS_SPACE_PATH "./s2opc_pubsub_nodeset.xml"
#define SLEEP_TIMEOUT 100
#define WAIT_STOP_START 2000
// ns=1;s=PubSubStatus
#define NODEID_NS_PUBSUB_STATUS 1
#define NODEID_STRING_PUBSUB_STATUS "PubSubStatus"
// ns=1;s=PubSubConfiguration
#define NODEID_NS_PUBSUB_CONFIG 1
#define NODEID_STRING_PUBSUB_CONFIG "PubSubConfiguration"
// ns=1;s=PubSubStartStop
#define NODEID_NS_PUBSUB_COMMAND 1
#define NODEID_STRING_PUBSUB_COMMAND "PubSubStartStop"
// ns=1;s=AcyclicPubSendStatus
#define NODEID_NS_ACYCLICPUB_SEND_STATUS 1
#define NODEID_STRING_ACYCLICPUB_SEND_STATUS "AcyclicPubSendStatus"
// ns=1;s=DataSetMessageFilteringStatus
#define NODEID_NS_DSM_FILTERING_STATUS 1
#define NODEID_STRING_DSM_FILTERING_STATUS "DataSetMessageFilteringStatus"
// ns=1;s=ResetDataSetMessageSequenceNumberStatus
#define NODEID_NS_RESET_DSM_SN_STATUS 1
#define NODEID_STRING_RESET_DSM_SN_STATUS "ResetDataSetMessageSequenceNumberStatus"
#define SYNCHRONOUS_READ_TIMEOUT 10000
#define PUBSUB_SKS_SIGNING_KEY "./signingKey.key"
#define PUBSUB_SKS_ENCRYPT_KEY "./encryptKey.key"
#define PUBSUB_SKS_KEY_NONCE "./keyNonce.key"

extern char* ENDPOINT_URL;

/* PubSub Configuration */
#define DEFAULT_PUBSUB_CONFIG_PATH "./config_pubsub_server.xml"
extern char* PUBSUB_CONFIG_PATH;

#endif /* CONFIG_H_ */
