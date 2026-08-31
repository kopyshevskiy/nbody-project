#!/usr/bin/env python3
"""SerialAoS vs SerialSoA: execution time and L1 cache-miss rate (N >= 64)."""

import csv
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

RESULTS = Path("benchmark/results")
COLOR = {"serial-aos": "#d62728", "serial-soa": "#1f77b4"}
LABEL = {"serial-aos": "AoS", "serial-soa": "SoA"}
MIN_BODIES = 64


def read(name, value):
    out = {}
    for row in csv.DictReader((RESULTS / name).open()):
        bodies = int(row["bodies"])
        if bodies >= MIN_BODIES:
            out[(row["implementation"], bodies)] = value(row)
    return out


def style(ax, bodies):
    ax.set_xscale("log", base=2)
    ax.set_xticks(bodies)
    ax.set_xticklabels([str(b) for b in bodies])
    ax.set_xlabel("Number of bodies (N)")
    ax.grid(True, alpha=0.3, which="both")
    ax.spines["top"].set_visible(False)
    ax.spines["right"].set_visible(False)
    ax.legend(frameon=False, fontsize=11)


def figure(data, ylabel, title, filename, log_y=False):
    bodies = sorted({b for _, b in data})
    fig, ax = plt.subplots(figsize=(8, 6))
    for impl in ("serial-aos", "serial-soa"):
        ax.plot(bodies, [data[(impl, b)] for b in bodies], marker="o", markersize=7,
                color=COLOR[impl], linewidth=2, label=LABEL[impl])
    if log_y:
        ax.set_yscale("log")
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    style(ax, bodies)
    fig.tight_layout()
    fig.savefig(RESULTS / filename, dpi=200)
    plt.close(fig)
    print("wrote", RESULTS / filename)


times = read("layout_time.csv", lambda r: float(r["time_seconds"]))
miss = read("layout_perf.csv",
            lambda r: 100.0 * int(r["l1_load_misses"]) / int(r["l1_loads"]))

figure(times, "Execution time (s)", "SerialAoS vs SerialSoA — Execution Time",
       "execution_time.png", log_y=True)
figure(miss, "L1 data-cache load-miss rate (%)",
       "SerialAoS vs SerialSoA — L1 Cache-Miss Rate", "l1_cache_miss.png")
