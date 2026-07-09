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

"""Automate password entry for sample binaries under CI (pexpect wrapper).

Used by CTest validation tests to drive samples that prompt on /dev/tty
(server private key, client key, user password). Typical chain:

  tap-wrap.py -> run_with_pwd.py -> with-opc-server.py -> sample binary

The child runs in a pseudo-TTY so prompts from SOPC_AskPass_CustomPromptFromTerminal
are visible to pexpect. When paired with with-opc-server.py, this script first waits
for a harness start marker (see HARNESS_START_MARKERS) before answering prompts.

Configuration:
  --timeout / RUN_WITH_PWD_TIMEOUT : per-expect timeout in seconds (default 60)
  --nb-passwords                     : must match the number of terminal prompts
  --no-wait-harness-start            : skip harness marker wait (standalone use)

Progress and failures: child process output is forwarded to stdout (as in CTest logs);
harness diagnostics only (step progress, failure reason) go to stderr.
"""

import argparse
import os
import platform
import signal
import sys

if platform.system() == "Windows":
    # pexpect API for windows was tested without success
    import wexpect as pexpect
    # string output
    stdout = sys.stdout
else:
    import pexpect
    # binary output
    stdout = sys.stdout.buffer

DEFAULT_TIMEOUT = float(os.environ.get('RUN_WITH_PWD_TIMEOUT', '60'))
PASSWORD = "password"
# Matches prompts such as "Private key password:" and "Password for client key:"
PASSWORD_PATTERN = '.*[Pp]assword.*:'
# Printed by with-opc-server.py before the background server is spawned
HARNESS_START_MARKERS = [
    'S2OPC_HARNESS: server_start',
    'Starting server',
]
description = '''Answer terminal password prompts via pexpect (CI driver for sample binaries).

Waits for ".*[Pp]assword.*:" prompts (--nb-passwords times), optionally after a
with-opc-server harness start marker. See module docstring for timeout options.'''

binProc = None


def signal_handler(sig, frame):
    if binProc is not None:
        binProc.kill(sig)


def log_stderr(msg):
    """Log harness diagnostics on stderr (visible in ctest -V output)."""
    sys.stderr.write(msg + '\n')
    sys.stderr.flush()


def dump_process_output(proc):
    if proc.before is not None:
        stdout.write(proc.before)
        stdout.flush()


def report_failure(proc, reason):
    dump_process_output(proc)
    log_stderr('run_with_pwd: {} (exitstatus={}, signalstatus={})'.format(
        reason, proc.exitstatus, proc.signalstatus))


def run_with_password(cmd_argv, nb_pwd, interact, timeout, wait_harness_start):
    """Spawn cmd_argv, answer nb_pwd terminal password prompts, then wait for exit."""
    global binProc
    if platform.system() == "Windows":
        binProc = pexpect.spawn(' '.join(cmd_argv), timeout=timeout)
    else:
        binProc = pexpect.spawn(cmd_argv[0], cmd_argv[1:], timeout=timeout)

    try:
        if wait_harness_start:
            log_stderr('run_with_pwd: waiting for harness start marker')
            binProc.expect(HARNESS_START_MARKERS, timeout=timeout)

        for i in range(0, nb_pwd):
            log_stderr('run_with_pwd: waiting for password prompt {}/{}'.format(i + 1, nb_pwd))
            binProc.expect(PASSWORD_PATTERN, timeout=timeout)
            binProc.sendline(PASSWORD)
            stdout.write(binProc.before)
    except pexpect.TIMEOUT as exc:
        report_failure(binProc, 'timeout waiting for password prompt')
        # Child still alive (stuck on prompt or harness wait): terminate so we do
        # not rely on CTest timeout and leave with-opc-server holding port 4841.
        if binProc.isalive():
            binProc.close(force=True)
        raise exc
    except pexpect.EOF as exc:
        report_failure(binProc, 'process ended before password prompt')
        # Child already exited; close() reaps the zombie and releases the PTY.
        binProc.close()
        raise exc

    if interact:
        binProc.interact()

    retCode = binProc.wait()
    sys.exit(retCode)


if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)

    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('--nb-passwords', metavar='nb_pwds',
                        help='Number of password prompts to answer', default=1, type=int)
    parser.add_argument('--password', metavar='pwd', help='Change the default password to this value')
    parser.add_argument('--timeout', metavar='seconds', type=float, default=DEFAULT_TIMEOUT,
                        help='Timeout in seconds for each expect (default: %(default)s or RUN_WITH_PWD_TIMEOUT)')
    parser.add_argument('--no-wait-harness-start', default=False, action='store_true',
                        dest='no_wait_harness_start',
                        help='Do not wait for with-opc-server harness start marker before password prompts')
    parser.add_argument('--interact', default=False, action='store_true', dest='interact',
                        help='Switch behavior to interact after entering password (works only for linux)')
    parser.add_argument('cmd', metavar='CMD', nargs='+', help='The command to run')
    parser.add_argument('args', metavar='ARG', nargs=argparse.REMAINDER, help='The command arguments')

    args = parser.parse_args()

    if args.password is not None:
        PASSWORD = args.password

    cmd_argv = args.cmd + args.args
    wait_harness_start = not args.no_wait_harness_start

    run_with_password(cmd_argv, args.nb_passwords, args.interact, args.timeout, wait_harness_start)
