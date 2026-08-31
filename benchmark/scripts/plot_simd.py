#!/usr/bin/env python3
"""SerialSoA vs SIMDSoA (AVX2): execution time."""

import csv
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

RESULTS = Path("benchmark/results")
COLOR = {"serial-soa": "#1f77b4", "simd": "#2ca02c"}
LABEL = {"serial-soa": "SoA", "simd": "SIMD (AVX2)"}

times = {}
for row in csv.DictReader((RESULTS / "simd_time.csv").open()):
    times[(row["implementation"], int(row["bodies"]))] = float(row["time_seconds"])

bodies = sorted({b for _, b in times})

fig, ax = plt.subplots(figsize=(8, 6))
for impl in ("serial-soa", "simd"):
    ax.plot(bodies, [times[(impl, b)] for b in bodies], marker="o", markersize=7,
            color=COLOR[impl], linewidth=2, label=LABEL[impl])

ax.set_xscale("log", base=2)
ax.set_yscale("log")
ax.set_xticks(bodies)
ax.set_xticklabels([str(b) for b in bodies])
ax.set_xlabel("Number of bodies (N)")
ax.set_ylabel("Execution time (s)")
ax.set_title("SerialSoA vs SIMD SoA — Execution Time")
ax.grid(True, alpha=0.3, which="both")
ax.spines["top"].set_visible(False)
ax.spines["right"].set_visible(False)
ax.legend(frameon=False, fontsize=11)

fig.tight_layout()
fig.savefig(RESULTS / "simd_execution_time.png", dpi=200)
print("wrote", RESULTS / "simd_execution_time.png")
