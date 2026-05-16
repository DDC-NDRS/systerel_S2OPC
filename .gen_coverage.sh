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


# Calls lcov/genhtml after project has been built with WITH_COVERAGE=1
set -e

REPORT_DIR=./report
REPORT_FILE=$REPORT_DIR/report.info
LCOV_IGNORE="--ignore-errors source,inconsistent,empty"

mkdir -p $REPORT_DIR

# Each test job uploads its gcov files under build-<job_name>/, so that
# concurrent artifact extraction in coverage-analysis does not overwrite
# .gcda files across jobs. Fall back to build/ for local runs.
BUILD_DIRS=()
for d in build-test-*; do
    [ -d "$d" ] && BUILD_DIRS+=("$d")
done
[ ${#BUILD_DIRS[@]} -eq 0 ] && BUILD_DIRS=(build)

# Capture one .info per build dir, then merge (lcov sums counters per file/test).
LCOV_ADD_ARGS=()
for BUILD_DIR in "${BUILD_DIRS[@]}"; do
    PARTIAL="$REPORT_DIR/${BUILD_DIR}.info"
    lcov $LCOV_IGNORE \
        -d "$BUILD_DIR/src/Common/CMakeFiles/s2opc_common.dir/" \
        -d "$BUILD_DIR/src/ClientServer/CMakeFiles/s2opc_clientserver.dir/" \
        -d "$BUILD_DIR/src/PubSub/CMakeFiles/s2opc_pubsub.dir/" \
        -c -o "$PARTIAL"
    LCOV_ADD_ARGS+=(-a "$PARTIAL")
done

lcov $LCOV_IGNORE "${LCOV_ADD_ARGS[@]}" -o $REPORT_FILE
# Remove bogus mbedtls files
lcov $LCOV_IGNORE -r $REPORT_FILE "/usr/*" -o $REPORT_FILE
genhtml $LCOV_IGNORE -o $REPORT_DIR -t "Code coverage from all tests" $REPORT_FILE
lcov $LCOV_IGNORE --summary $REPORT_FILE
