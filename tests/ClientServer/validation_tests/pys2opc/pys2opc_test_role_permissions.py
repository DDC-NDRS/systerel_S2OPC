#!/usr/bin/env python3
# -*- coding: utf-8 -*-

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

"""
Role permissions tests:
- DefaultRolePermissions(DRP) tests :
    - DRP without RP
    - DRP with RP. RP overrides DRP (addition/restiction)
    - No DRP => RP ignored
- RolePermissions(RP) tests :
    - Read
    - Write
    - Call
    - TODO : AddNodes

Test sequence (with expected results) :
Note : obs = Observer, op = Operator.

# DRP Tests #
Int64_DRP_no_RP : 1. Read with DRP (obs) -> OK
                  2. Write with DRP (obs) -> NOK

Int64_DRP_RP : 3. Read with RP (obs) -> NOK (RP overrides DRP and RP write for obs)
               4. Write with RP (obs) -> OK

Int64_DRP_RP_2 : 5. Read with RP (obs) -> NOK (RP overrides DRP and no RP for obs)

Int64_DRP_RP_3 : 6. Read with RP (configAdmin) -> NOK (RP overrides DRP and RP write for configAdmin)
                 7. Write with RP (configAdmin) -> OK

Int64_RP_no_DRP (NS=2) : 8. Write with RP read only (obs) and no DRP -> OK (No DRP => RP ignored)

# RP Tests #
Int64_RP : 9. Read with RP (obs) -> NOK
           10. Read with RP (Op) -> OK
           11. Write with RP (Op) -> NOK
           TODO : Write with RP (ConfigureAdmin) -> OK

MethodNoArg : 12. Call with DRP (obs) -> NOK
              13. Call with RP (Op) -> OK
"""

import os
import getpass
import sys

from pys2opc import PyS2OPC_Client, SOPC_Failure, StatusCode, DataValue, VariantType
from tap_logger import TapLogger

TEST_PASSWORD_PRIV_KEY_ENV_NAME = "TEST_PASSWORD_PRIVATE_KEY"
TEST_USERNAME_ENV_NAME = "TEST_USERNAME"
TEST_PASSWORD_USER_ENV_NAME = "TEST_PASSWORD_USER"
# CONFIG_ADMIN = False

class PyS2OPC_Client_Test():
    @staticmethod
    def get_client_key_password() -> str:
        pwd = os.getenv(TEST_PASSWORD_PRIV_KEY_ENV_NAME)
        if pwd is None:
            print("{} not set: set it or enter it interactively".format(TEST_PASSWORD_PRIV_KEY_ENV_NAME))
            pwd = getpass.getpass(prompt='Client private key password:')
        return pwd

    @staticmethod
    def get_username_password(connConfigUserId: str) -> tuple[str, str]:
        # if CONFIG_ADMIN:
        #     return "me", "1234"
        username = os.getenv(TEST_USERNAME_ENV_NAME)
        if username is None:
            print("{} not set: set it or enter it interactively".format(TEST_USERNAME_ENV_NAME))
            username = getpass.getpass(prompt="UserName of user (e.g.: 'user1'): ")
        pwd = os.getenv(TEST_PASSWORD_USER_ENV_NAME)
        if pwd is None:
            print("{} not set: set it or enter it interactively".format(TEST_PASSWORD_USER_ENV_NAME))
            pwd = getpass.getpass(prompt='Password for user: ')
        return username, pwd

    @staticmethod
    def get_user_X509_key_password(userCertThumb : str) -> str:
        pwd = os.getenv(TEST_PASSWORD_PRIV_KEY_ENV_NAME)
        if pwd is None:
            print("{} not set: set it or enter it interactively".format(TEST_PASSWORD_PRIV_KEY_ENV_NAME))
            pwd = getpass.getpass(prompt=f'Client X509 {userCertThumb} user private key password:')
        return pwd


# overload the default method to get key password and username with environment variable
PyS2OPC_Client.get_client_key_password = PyS2OPC_Client_Test.get_client_key_password
PyS2OPC_Client.get_username_password = PyS2OPC_Client_Test.get_username_password
PyS2OPC_Client.get_user_X509_key_password = PyS2OPC_Client_Test.get_user_X509_key_password


# global variable corresponding to DataValues to write into NODES #
DV_Int64 = DataValue.from_python(12, False)
DV_Int64.variantType= VariantType.Int64

# Error code attends when role permissions isn't give to a service #
BadUserAccessDenied: int = 0x801F0000

if __name__ == '__main__':

    print("Start Tests Role Permissions")
    # --- Test Role Permissions --- #
    logger = TapLogger('validation_pys2opc_role_permissions.tap')

    op_configs = ["write", "x509_write"]

    for op_config in op_configs:
        print(f"\n--- Running tests with Operator connection config: '{op_config}' ---")
        with PyS2OPC_Client.initialize():
            try:
                # Connect clients #
                configs = PyS2OPC_Client.load_client_configuration_from_file(os.path.join('S2OPC_Client_Wrapper_Config.xml'))
                connect_obs_config_admin = PyS2OPC_Client.connect(configs["read"]) # Obs : anonymous
                connect_op = PyS2OPC_Client.connect(configs[op_config]) # Op : username/X509 depending on config
                # Clients connected #

                # - 1 : Tests DRP - #
                logger.begin_section(f'DRP tests - op_config={op_config}')

                # DRP no RP
                readResponse = connect_obs_config_admin.read_nodes(['ns=1;s=Int64_DRP_no_RP'])
                logger.add_test('Observer reads successfully (DRP:observer/configAdmin = Read). Result = 0x{:08X}.'
                                .format(readResponse.results[0].statusCode), StatusCode.isGoodStatus(readResponse.results[0].statusCode))
                writeResponse = connect_obs_config_admin.write_nodes(['ns=1;s=Int64_DRP_no_RP'], [DV_Int64])
                logger.add_test('Observer fails to write (DRP:observer/configAdmin = Read). Result = 0x{:08X}. Expected = 0x{:08X}.'
                                .format(writeResponse.results[0], BadUserAccessDenied), writeResponse.results[0] == BadUserAccessDenied)

                # DRP RP
                readResponse = connect_obs_config_admin.read_nodes(['ns=1;s=Int64_DRP_RP'])
                logger.add_test('Observer fails to read (RP:observer = Write overrides DRP:observer/configAdmin = Read). Result = 0x{:08X}. Expected = 0x{:08X}.'
                                .format(readResponse.results[0].statusCode, BadUserAccessDenied),readResponse.results[0].statusCode == BadUserAccessDenied)
                writeResponse = connect_obs_config_admin.write_nodes(['ns=1;s=Int64_DRP_RP'], [DV_Int64])
                logger.add_test('Observer writes successfully (RP:observer = Write overrides DRP:observer = Read). Result = 0x{:08X}.'
                                .format(writeResponse.results[0]), writeResponse.is_ok())

                # DRP RP 2
                readResponse = connect_obs_config_admin.read_nodes(['ns=1;s=Int64_DRP_RP_2'])
                logger.add_test('Observer fails to read (RP:observer/configAdmin = None overrides DRP:observer/configAdmin = Read). Result = 0x{:08X}. Expected = 0x{:08X}.'
                                .format(readResponse.results[0].statusCode, BadUserAccessDenied), readResponse.results[0].statusCode == BadUserAccessDenied)

                # DRP RP 3
                readResponse = connect_obs_config_admin.read_nodes(['ns=1;s=Int64_DRP_RP_3'])
                logger.add_test('ConfigAdmin fails to read (RP:configAdmin = Write overrides DRP:observer/configAdmin = Read). Result = 0x{:08X}. Expected = 0x{:08X}.'
                                .format(readResponse.results[0].statusCode, BadUserAccessDenied),readResponse.results[0].statusCode == BadUserAccessDenied)
                writeResponse = connect_obs_config_admin.write_nodes(['ns=1;s=Int64_DRP_RP_3'], [DV_Int64])
                logger.add_test('ConfigAdmin writes successfully (RP:configAdmin = Write overrides DRP:observer/configAdmin = Read). Result = 0x{:08X}.'
                                .format(writeResponse.results[0]), writeResponse.is_ok())

                # no DRP (NS=2)
                writeResponse = connect_obs_config_admin.write_nodes(['ns=2;s=Int64_RP_no_DRP'], [DV_Int64])
                logger.add_test('Observer write successfully (whereas RP:observer = Read) because no DPR => No RolePersmissions restriction. Result = 0x{:08X}.'
                                .format(writeResponse.results[0]), writeResponse.is_ok())

                # - 2 : Tests RP - #
                logger.begin_section(f'RP tests - op_config={op_config}')

                # RP Read/Write
                readResponse = connect_obs_config_admin.read_nodes(['ns=1;s=Int64_RP'])
                logger.add_test('Observer fails to read (RP:observer/configAdmin = None). Result = 0x{:08X}. Expected = 0x{:08X}.'
                                .format(readResponse.results[0].statusCode, BadUserAccessDenied), readResponse.results[0].statusCode == BadUserAccessDenied)
                readResponse = connect_op.read_nodes(['ns=1;s=Int64_RP'])
                logger.add_test('Operator reads successfully (RP:operator = Read). Result = 0x{:08X}.'
                                .format(readResponse.results[0].statusCode), StatusCode.isGoodStatus(readResponse.results[0].statusCode))
                writeResponse = connect_op.write_nodes(['ns=1;s=Int64_RP'], [DV_Int64])
                logger.add_test('Operator fails to write (RP:operator = Read). Result = 0x{:08X}. Expected = 0x{:08X}.'
                                .format(writeResponse.results[0], BadUserAccessDenied), writeResponse.results[0] == BadUserAccessDenied)
                # # TODO : Add this test when changing users on the same connection is possible
                # # As we're using the same “write” connection configuration for Operator and confAdmin,
                # # we need to switch between the two.
                # connect_op.disconnect()
                # CONFIG_ADMIN = True
                # connect_confAdmin = PyS2OPC_Client.connect(configs["write"]) # confAdmin : username : me, pwd : 1234
                # writeResponse = connect_confAdmin.write_nodes(['ns=1;s=Int64_RP'], [DV_Int64])
                # logger.add_test('Observer writes successfully (RP:ConfigureAdmin = Read). Result = 0x{:08X}.'
                #                 .format(writeResponse.results[0]), writeResponse.is_ok())

                # RP Call
                callMethodResponse = connect_obs_config_admin.call_method("ns=1;s=TestObject", "ns=1;s=MethodNoArg")
                logger.add_test('Observer fails to call (RP:observer = Read). Result = 0x{:08X}. Expected = 0x{:08X}.'
                                .format(callMethodResponse.callResult, BadUserAccessDenied), callMethodResponse.callResult == BadUserAccessDenied)
                callMethodResponse = connect_op.call_method("ns=1;s=TestObject", "ns=1;s=MethodNoArg")
                logger.add_test('Operator calls successfully (RP:observer = Read + Write + Call). Result = 0x{:08X}.'
                                .format(callMethodResponse.callResult), callMethodResponse.is_ok())
            except SOPC_Failure as f:
                print(f, flush=True)
                sys.exit(1)

    logger.finalize_report()
    sys.exit(1 if logger.has_failed_tests else 0)
