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

import argparse
import re
import subprocess
import sys
import yaml
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from textwrap import wrap
from dataclasses import dataclass, field


# ---------------------------------------------------------------------------
# Replacement for lisa.trace.Trace
# Reads a trace-cmd .dat file via `trace-cmd report` and builds per-event
# DataFrames with (timestamp index, __comm column) — same layout as LISA.
# ---------------------------------------------------------------------------

class Trace:
    """
    Minimal drop-in for lisa.trace.Trace, backed by `trace-cmd report`.

    trace-cmd report line format:
        COMM-PID  [CPU]  FLAGS  TIMESTAMP: EVENT_NAME: FIELDS

    Example (FLAGS field like 'dNHff' sits between [CPU] and TIMESTAMP):
        Publisher-26205 [006] dNHff 11269.867295: MessageCtx_Send_Publish_Message: (40c680)
    """

    # Matches:  COMM-PID  [CPU]  FLAGS  TIMESTAMP:  EVENT_NAME:
    # (.+?) non-greedy so COMM stops at the last -PID boundary
    # FLAGS field ([^\d\s]+) may be immediately followed by TIMESTAMP with no space
    # (seen on iMX93 where trace-cmd outputs e.g. "dNHff212919476255110:" instead
    # of "dNHff 11269.867295:").  \s* handles both variants.
    _LINE_RE = re.compile(
        r'^\s*(.+?)-(\d+)\s+\[\d+\](?:\s+[^\d\s]+)?\s*([\d.]+):\s+(\S+?):\s'
    )

    # Nanosecond timestamps have no decimal point and are very large integers.
    # Threshold: if timestamp > 1e10 it is treated as nanoseconds → convert to seconds.
    _NS_THRESHOLD = 1e10

    def __init__(self, filepath):
        try:
            result = subprocess.run(
                ['trace-cmd', 'report', '-i', filepath],
                capture_output=True, text=True, check=True
            )
        except FileNotFoundError:
            sys.exit("ERROR: 'trace-cmd' not found. Install it with: sudo apt install trace-cmd")
        except subprocess.CalledProcessError as e:
            sys.exit(f"ERROR: trace-cmd report failed:\n{e.stderr}")

        raw = {}  # event_name -> list of (timestamp_s, comm)
        for line in result.stdout.splitlines():
            m = self._LINE_RE.match(line)
            if not m:
                continue
            comm, _pid, ts_str, event = m.group(1), m.group(2), m.group(3), m.group(4)
            ts = float(ts_str)
            if ts > self._NS_THRESHOLD:   # nanoseconds → seconds
                ts /= 1_000_000_000
            raw.setdefault(event, []).append((ts, comm.strip()))

        self._events = {}
        for event, rows in raw.items():
            timestamps, comms = zip(*rows)
            self._events[event] = pd.DataFrame(
                {'__comm': comms},
                index=pd.Index(timestamps, name='timestamp')
            )

    @property
    def available_events(self):
        return sorted(self._events.keys())

    def df_event(self, event_name):
        # Accept exact match or suffix match (e.g. "MyFunc" matches "probe_app:MyFunc")
        if event_name in self._events:
            return self._events[event_name]
        matches = [k for k in self._events if k.endswith(event_name)]
        if len(matches) == 1:
            return self._events[matches[0]]
        if len(matches) > 1:
            raise KeyError(f"Ambiguous event name '{event_name}': {matches}")
        raise KeyError(f"Event '{event_name}' not found. Available: {self.available_events}")


# ---------------------------------------------------------------------------
# Data structures
# ---------------------------------------------------------------------------

@dataclass
class Histogram:
    name  : str
    min   : np.int64
    max   : np.int64
    avg   : np.int64
    bin   : np.int64
    xaxis : list = field(default_factory=list)
    yaxis : list = field(default_factory=list)


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def parse_command_line():
    parser = argparse.ArgumentParser(description="Calculate delta times between probes.")
    parser.add_argument("-p", "--probes", action='store_true',
                        help="show all probes available in the trace file and exit")
    parser.add_argument("--save-images", type=str,
                        help="Save histogram as PNG. Argument: output filename (without extension if multiple pairs)")
    parser.add_argument("--show", action='store_true',
                        help="Display histogram interactively (default when --save-images is not set)")
    parser.add_argument("--filepath", type=str,
                        help="Override the trace file path from the YAML config (accepts absolute paths)")
    parser.add_argument('TRACECONFIG', type=str, help='Trace configuration file (YAML format)')
    return parser.parse_args()


# ---------------------------------------------------------------------------
# Analysis helpers
# ---------------------------------------------------------------------------

def realign_timestamps_probes(probe1_timestamps, probe2_timestamps):
    """
    Realign timestamps between probe1 and probe2.
    probe1 occurs after probe2.

    Example:                                  *realignment
                probe1_timestamps
                0       71208.409796
                1       71208.509497
                2       71208.610161
                3       71208.709825
                                       --->   Last probe1_timestamp deleted (sizes match)
                probe2_timestamps:
                0       71208.308972   --->   Deleted occured before probe1_timestamp 0
                1       71208.409637          *0
                2       71208.509394          *1
                3       71208.610062          *2
    """
    if probe2_timestamps.iloc[1] < probe1_timestamps.iloc[0]:
        probe2_timestamps = probe2_timestamps.iloc[1:]
        probe2_timestamps = probe2_timestamps.reset_index(drop=True)
    return probe1_timestamps, probe2_timestamps


def analyze_trace(trace_probe1, trace_probe2):
    probe1_timestamps = trace_probe1.reset_index().iloc[:, 0]
    probe2_timestamps = trace_probe2.reset_index().iloc[:, 0]

    probe1_timestamps, probe2_timestamps = realign_timestamps_probes(
        probe1_timestamps, probe2_timestamps
    )

    # Align to the shorter series
    min_len = min(len(probe1_timestamps), len(probe2_timestamps))
    probe1_timestamps = probe1_timestamps.iloc[:min_len]
    probe2_timestamps = probe2_timestamps.iloc[:min_len]

    delta_time = (probe1_timestamps.values - probe2_timestamps.values) * 1_000_000

    delta_series = pd.Series(delta_time)
    delta_series = delta_series[delta_series.notnull()].round().astype(int)
    return delta_series


def show_statistics(histogram):
    print(f"  Statistics for {histogram.name}")
    print(f"    ==   min : {histogram.min} µs")
    print(f"    ==   max : {histogram.max} µs")
    print(f"    ==   avg : {histogram.avg} µs")
    print()


def format_histogram(df_subset, name, scale=1):
    hist_min = int(df_subset.min())
    hist_max = int(df_subset.max())
    hist_avg = int(round(df_subset.mean()))
    hist_bin = max(1, (hist_max - hist_min) // max(1, scale))

    df_subset_occurrences = df_subset.value_counts(bins=hist_bin, sort=False)

    margin = 1
    yaxis = margin * [0] + df_subset_occurrences.tolist() + margin * [0]
    xaxis = np.linspace(hist_min - margin * scale,
                        hist_max + margin * scale,
                        hist_bin + margin * 2)

    return Histogram(name, hist_min, hist_max, hist_avg, hist_bin, xaxis, yaxis)


def plot_histogram(histogram: Histogram, save_path=None, show=False):
    fig, ax = plt.subplots(figsize=(10, 5))

    ax.step(histogram.xaxis, histogram.yaxis, linewidth=0.8)

    metrics = f"Min: {histogram.min} µs,  Max: {histogram.max} µs,  Avg: {histogram.avg} µs"
    ax.set_xlabel('Time elapsed (µs)\n' + metrics)

    x_center = histogram.avg if histogram.avg > 100 else 100
    ax.set_xlim([max(0, x_center - 100), x_center + 100])

    ax.set_ylabel('Occurrences')
    ax.set_yscale('log')
    ax.set_title('\n'.join(wrap(histogram.name, 60)))
    ax.grid(True, which='both', linestyle='--', alpha=0.4)

    fig.tight_layout()

    if save_path:
        print(f"Save figure in file {save_path}")
        fig.savefig(save_path, dpi=150)

    if show:
        plt.show()

    plt.close(fig)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def show_probes(trace):
    print("\nAvailable probes/events:\n")
    for e in trace.available_events:
        print(f"  {e}")


if __name__ == "__main__":
    args = parse_command_line()

    with open(args.TRACECONFIG, "r") as stream:
        try:
            metadata_trace = yaml.safe_load(stream)
        except yaml.YAMLError as exc:
            sys.exit(f"ERROR reading YAML config: {exc}")

    trace_filepath = args.filepath if args.filepath else metadata_trace['filepath']
    trace = Trace(trace_filepath)

    if args.probes:
        show_probes(trace)
        sys.exit(0)

    if not trace.available_events:
        print(f"WARNING: trace file '{trace_filepath}' contains no events "
              f"(probes may not have been active during recording — redo measurement)")
        sys.exit(0)

    program_name = metadata_trace['program']
    do_show = args.show or (args.save_images is None)

    try:
        pairs = metadata_trace['probes_pair']
        for idx, pair in enumerate(pairs):
            probe1_name = pair['probe1']
            probe2_name = pair['probe2']

            df_probe1 = trace.df_event(probe1_name)
            df_probe2 = trace.df_event(probe2_name)

            # Filter by process name
            df_probe1 = df_probe1[df_probe1['__comm'] == program_name]
            df_probe2 = df_probe2[df_probe2['__comm'] == program_name]

            delta = analyze_trace(df_probe1, df_probe2)
            hist_name = f"Delta time between {probe2_name} and {probe1_name}"
            histogram = format_histogram(delta, hist_name)

            show_statistics(histogram)

            # Determine save path (add index suffix when multiple pairs)
            save_path = None
            if args.save_images:
                if len(pairs) > 1:
                    base, *ext = args.save_images.rsplit('.', 1)
                    suffix = f"_{idx+1}"
                    save_path = f"{base}{suffix}.{ext[0]}" if ext else f"{base}{suffix}.png"
                else:
                    save_path = args.save_images

            plot_histogram(histogram, save_path=save_path, show=do_show)

    except KeyError as e:
        sys.exit(f"ERROR: {e}")
    except ValueError:
        print(f"""
   == ERROR : No probes available in {metadata_trace['filepath']}
              Make sure probes are well configured in {args.TRACECONFIG}
              If probes are well configured, redo measurements ==
""")
        sys.exit(1)
