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

# Define the docker images used in S2OPC

# Public images (registry.gitlab.com/systerel/s2opc)
BUILD_DIGEST=registry.gitlab.com/systerel/s2opc/build@sha256:8cbb2881dc3798ce6b6cf078fa744ef357b30509a5c3e12e2e59cf8bed9c521f # build:1.51
MINGW_DIGEST=registry.gitlab.com/systerel/s2opc/mingw_build@sha256:d158e52c51d2f97d4bd13bcc93ebbe5793025c0032dd7e6116bdf9293fc790bf # mingw_build:1.15
RPI_DIGEST=registry.gitlab.com/systerel/s2opc/rpi-build@sha256:070b1ca58667ee5908902ba3eb03e5fa05993d3a86d2c8ddeb6f7f41ed62609c # rpi_build:1.9
CHECK_DIGEST=registry.gitlab.com/systerel/s2opc/check@sha256:8be8c50cd484989c987b6c28ded2e03f30e81ec27bb53e41cecde8cdc3bb86c8 # check 1.22
TEST_DIGEST=registry.gitlab.com/systerel/s2opc/test@sha256:8fbec254f6b8c8467b780d407c6f3c2e87d87899e141b8b2c9013ba30b4f0c8d # test:2.16
ZEPHYR_DIGEST=registry.gitlab.com/systerel/s2opc/zephyr_build:v3.7.0-c
FREERTOS_DIGEST=registry.gitlab.com/systerel/s2opc/freertos_build:v1.0@sha256:09addc3a3bfcd46ff5581ab21229330fb4cc506c331cf7a31b0a58a0d886de0d # freertos_build:v1.1 (FreeRTOS for STM32-H723ZG only for CI purpose)
CYBERWATCH_DIGEST=registry.gitlab.com/systerel/s2opc/cyberwatch@sha256:df8486140065bb3ea2d7394283ef1adfd403d341edc9ffe28582721ceb199499 # registry.gitlab.com/systerel/s2opc/cyberwatch:2.0
SONARQUBE_DIGEST=registry.gitlab.com/systerel/s2opc/sonarqube@sha256:4baaa859d74163e40467c5fa630ad205a83a4190b7fe472d32f071619a31ca5e #registry.gitlab.com/systerel/s2opc/sonarqube:1.0

# Private images
GEN_DIGEST=docker.aix.systerel.fr/c838/gen@sha256:e7c7f0427d49d162c66410e05eb04a5859ec5e87a04d2ee82bbf25772acf7adf # docker.aix.systerel.fr/c838/gen:1.4
UACTT_WIN_DIGEST=com.systerel.fr:5000/c838/uactt-win@sha256:98b4488aa85310d1e668af8f42be4973e2e7494fe9f44adbaf81896f794db794 # com.systerel.fr:5000/c838/uactt-win:2.3
UACTT_LINUX_DIGEST=com.systerel.fr:5000/c838/uactt-linux@sha256:e9118a5aa597d4e7f2bcf1f53817f120182ef3633103b9e6cf128c3945f46ca2 # com.systerel.fr:5000/c838/uactt-linux:1.6
NETWORK_FUZZING_DIGEST=docker.aix.systerel.fr/c838/opcua-network-fuzzer@sha256:35e40ba847bdfe5f3e7112f465adb2c70d1f0694521f6ba8d018c55f651ef205 # docker.aix.systerel.fr/c838/opcua-network-fuzzer:3.3
PIKEOS_DIGEST=docker.aix.systerel.fr/c838/pikeos@sha256:3d5dcbab75b0c98a8d27d1cfa239bfa43b5b87bc1bbaca2a7c8a5bfe2adc5aeb # docker.aix.systerel.fr/c838/pikeos:1.2.1
