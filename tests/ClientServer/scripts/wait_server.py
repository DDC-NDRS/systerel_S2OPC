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

"""Wait until an OPC UA server accepts TCP connections on localhost:4841.

Readiness is inferred from a successful TCP connect (not an OPC UA handshake).
Used by with-opc-server.py before launching the test client.

Configuration:
  overall_timeout argument / S2OPC_WAIT_SERVER_TIMEOUT : total wait budget (default 60 s)
  CONNECT_ATTEMPT_TIMEOUT                              : per-attempt socket timeout (1 s)

When server_process is supplied, wait_server returns False immediately if that
process exits, so with-opc-server.py can fail fast instead of waiting the full budget.
"""

import os
import socket
import sys
import time
from urllib.parse import urlparse

DEFAULT_OVERALL_TIMEOUT = float(os.environ.get('S2OPC_WAIT_SERVER_TIMEOUT', '60'))
CONNECT_ATTEMPT_TIMEOUT = 1.0
POLL_INTERVAL = 0.1
DEFAULT_URL = 'opc.tcp://localhost:4841'


def wait_server(url, overall_timeout=None, server_process=None):
    """Wait until TCP connects to the OPC UA endpoint or the budget expires.

    Returns True when the port accepts connections, False otherwise.
    If server_process is provided, returns False as soon as that process exits.
    """
    if overall_timeout is None:
        overall_timeout = DEFAULT_OVERALL_TIMEOUT

    pr = urlparse(url)
    hostname = pr.hostname or 'localhost'
    port = pr.port or 4841
    deadline = time.time() + overall_timeout

    while True:
        if server_process is not None:
            exit_code = server_process.poll()
            if exit_code is not None:
                return False

        if time.time() >= deadline:
            return False

        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(CONNECT_ATTEMPT_TIMEOUT)
        try:
            sock.connect((hostname, port))
            sock.close()
            return True
        except (ConnectionError, OSError, socket.timeout):
            sock.close()
            time.sleep(POLL_INTERVAL)


def is_port_open(url, connect_timeout=CONNECT_ATTEMPT_TIMEOUT):
    """Return True if a TCP connection to url succeeds (post-shutdown port check)."""
    pr = urlparse(url)
    hostname = pr.hostname or 'localhost'
    port = pr.port or 4841
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(connect_timeout)
    try:
        sock.connect((hostname, port))
        sock.close()
        return True
    except (ConnectionError, OSError, socket.timeout):
        sock.close()
        return False


if __name__ == '__main__':
    # Check args
    if len(sys.argv) == 1:
        sUrl = DEFAULT_URL
        timeout = DEFAULT_OVERALL_TIMEOUT
    elif len(sys.argv) == 2:
        sUrl = sys.argv[1]
        timeout = DEFAULT_OVERALL_TIMEOUT
    elif len(sys.argv) == 3:
        sUrl = sys.argv[1]
        timeout = float(sys.argv[2])
    else:
        print('Usage:', sys.argv[0], '[ENDPOINT_URL] [OVERALL_TIMEOUT_SECONDS]')
        sys.exit(1)

    sys.exit(0 if wait_server(sUrl, timeout) else 1)
