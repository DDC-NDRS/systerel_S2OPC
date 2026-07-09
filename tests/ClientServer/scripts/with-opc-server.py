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

"""Start a background OPC UA server, run a test command, then stop the server.

Used by CTest to run sample/client validation against toolkit_demo_server on
localhost:4841. Server readiness is checked with wait_server (TCP connect only).

Harness markers (stdout, for run_with_pwd.py synchronization and CI logs):
  S2OPC_HARNESS: server_start   : before server subprocess is spawned
  S2OPC_HARNESS: server_ready   : TCP port accepts connections
  S2OPC_HARNESS: server_timeout : readiness wait expired
  S2OPC_HARNESS: server_exited  : server process exited during startup

Configuration:
  --wait-server-timeout / S2OPC_WAIT_SERVER_TIMEOUT : server boot budget (default 60 s)
  --wait-timeout                                    : optional client command timeout
  --server-env                                      : extra KEY=VALUE pairs merged into os.environ

Server and client share the same environment and stdin (PTY when driven by pexpect).
"""

import argparse
import os
import shlex
import subprocess
import sys

import wait_server

description = '''Run a test command while a background S2OPC server listens on port 4841.

See module docstring for harness markers, timeouts and environment handling.'''

SERVER_SHUTDOWN_TIMEOUT = 10.0

# Stable stdout markers consumed by run_with_pwd.py (--timeout/--nb-passwords in CMake)
MARKER_SERVER_START = 'S2OPC_HARNESS: server_start'
MARKER_SERVER_READY = 'S2OPC_HARNESS: server_ready'
MARKER_SERVER_TIMEOUT = 'S2OPC_HARNESS: server_timeout'
MARKER_SERVER_EXITED = 'S2OPC_HARNESS: server_exited'


def log(msg):
    print(msg)
    sys.stdout.flush()


def parse_env(string_env):
    pairs = string_env.split()
    env = {}
    for pair in pairs:
        if "=" in pair:
            key, value = pair.split('=', 1)
            env[key] = value
        else:
            print('Malformed pair {} expect "Variable=Value" format'.format(pair))
            return None
    return env


def merge_env(extra_env):
    """Return os.environ updated with optional --server-env overrides."""
    env = os.environ.copy()
    if extra_env is not None:
        env.update(extra_env)
    return env


def stop_server(server_process):
    """Terminate the server gracefully, then kill if shutdown exceeds SERVER_SHUTDOWN_TIMEOUT."""
    if server_process.poll() is not None:
        return server_process.returncode

    server_process.terminate()
    try:
        return server_process.wait(timeout=SERVER_SHUTDOWN_TIMEOUT)
    except subprocess.TimeoutExpired:
        # 2 times to avoid OPCUA shutdown phase
        server_process.kill()
        server_process.kill()
        return server_process.wait()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('--server-cmd', metavar='CMD', help='The command to start the background server')
    parser.add_argument('--server-wd', metavar='DIR', help='The directory to run the server in')
    parser.add_argument('--server-env', metavar='ENV', help='Environment variable to run along the server')
    parser.add_argument('--wait-server', action='store_true', default=False,
                        help='Wait for the server to exit instead of killing it when the client is done')
    parser.add_argument('--wait-timeout',
                        help='Timeout duration expressed as a float in seconds. Return code of the function is 0 in this case.')
    parser.add_argument('--wait-server-timeout', type=float, default=wait_server.DEFAULT_OVERALL_TIMEOUT,
                        help='Max seconds to wait for server TCP endpoint on port 4841 (default: %(default)s, '
                             'override with S2OPC_WAIT_SERVER_TIMEOUT)')
    parser.add_argument('cmd', metavar='CMD', help='The command to run')
    parser.add_argument('args', metavar='ARGS', nargs=argparse.REMAINDER,
                        help='Parameters to pass to the command')

    args = parser.parse_args()

    if args.server_cmd is None:
        sys.stderr.write('Missing server command.\n')
        sys.exit(1)

    extra_env = None
    if args.server_env is not None:
        extra_env = parse_env(args.server_env)
        if extra_env is None:
            sys.exit(1)

    server_env = merge_env(extra_env)

    log(MARKER_SERVER_START)
    log('Starting server')
    server_process = subprocess.Popen(shlex.split(args.server_cmd),
                                      cwd=args.server_wd,
                                      env=server_env)

    if not wait_server.wait_server(wait_server.DEFAULT_URL,
                                   args.wait_server_timeout,
                                   server_process):
        exit_code = server_process.poll()
        if exit_code is not None:
            log('{} (exit code {})'.format(MARKER_SERVER_EXITED, exit_code))
        else:
            log(MARKER_SERVER_TIMEOUT)
            log('Timeout for starting server')
            stop_server(server_process)
        sys.exit(1)

    log(MARKER_SERVER_READY)
    cmd = [args.cmd] + args.args

    log('Starting test %s' % ' '.join(cmd))

    try:
        if not args.wait_timeout:
            subprocess.check_call(cmd, env=server_env)
        else:
            log('Launch command with timeout of %f seconds \n' % float(args.wait_timeout))
            subprocess.check_call(cmd, timeout=float(args.wait_timeout), env=server_env)
        test_ret = 0
    except subprocess.TimeoutExpired:
        # Nominal exit
        log('Test time out')
        test_ret = 0
    except subprocess.CalledProcessError as e:
        test_ret = e.returncode
    except FileNotFoundError as e:
        sys.stderr.write('%s: Not such file or directory\n' % e.filename)
        test_ret = 127

    if not args.wait_server:
        log('Test finished, killing server')
        stop_server(server_process)
        if wait_server.is_port_open(wait_server.DEFAULT_URL):
            log('Warning: port 4841 still open after server shutdown')

    log('Waiting for server to exit')
    server_ret = server_process.wait()

    log('Done with test_ret=%d and server_ret=%d' % (test_ret, server_ret))
    running_in_windows = sys.platform.startswith('win32')
    if test_ret == 0:
        # server return code can be checked on Linux only
        sys.exit(0 if running_in_windows else server_ret)
    else:
        sys.exit(test_ret)

# vim: set et ts=4 sw=4:
