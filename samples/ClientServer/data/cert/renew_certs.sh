#!/bin/bash

# Licensed to Systerel under one or more contributor license
# agreements. See the NOTICE file distributed with this work
# for additional information regarding copyright ownership.
# Systerel licenses this file to you under the Apache
# License, Version 2.0 (the "License"); you may not use this
# file except in compliance with the License. You may obtain
# a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.


set -e

CONF_FILE=cassl.cnf
CONF_CLI=cli_req.cnf
CONF_SRV=srv_req.cnf
CA_KEY=cakey.pem
CA_CERT=cacert.pem

DURATION=730

# create csr for existing certificates
openssl req -new -config $CONF_CLI -sha256 -key encrypted_client_2k_key.pem -reqexts client_cert -out client_2k.csr
openssl req -new -config $CONF_CLI -sha256 -key encrypted_client_4k_key.pem -reqexts client_cert -out client_4k.csr
openssl req -new -config $CONF_SRV -sha256 -key encrypted_server_2k_key.pem -reqexts server_cert -out server_2k.csr
openssl req -new -config $CONF_SRV -sha256 -key encrypted_server_4k_key.pem -reqexts server_cert -out server_4k.csr
# And sign them, for the next two years
openssl ca -batch -config $CONF_FILE -policy signing_policy -extensions client_signing_req -days $DURATION -in client_2k.csr -out client_2k_cert.pem
openssl ca -batch -config $CONF_FILE -policy signing_policy -extensions client_signing_req -days $DURATION -in client_4k.csr -out client_4k_cert.pem
openssl ca -batch -config $CONF_FILE -policy signing_policy -extensions server_signing_req -days $DURATION -in server_2k.csr -out server_2k_cert.pem
openssl ca -batch -config $CONF_FILE -policy signing_policy -extensions server_signing_req -days $DURATION -in server_4k.csr -out server_4k_cert.pem

# Remove the CSRs
rm ./*.csr

# Output application certificates in DER format
for fradix in client_2k_ client_4k_ server_2k_ server_4k_; do
    openssl x509 -in ${fradix}cert.pem -out ${fradix}cert.der -outform der
done

# Output hexlified certificate to include in check_crypto_certificates.c
echo "Server signed 2k public key:"
hexdump -ve '/1 "%02x"' server_2k_cert.der
echo
echo -e "\nServer's 2k certificate thumbprint (SHA-1):"
openssl x509 -noout -fingerprint -in server_2k_cert.pem
echo
echo "Client signed 2k public key:"
hexdump -ve '/1 "%02x"' client_2k_cert.der
echo
echo -e "\nClient's 2k certificate thumbprint (SHA-1):"
openssl x509 -noout -fingerprint -in client_2k_cert.pem
