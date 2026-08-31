#!/usr/bin/env python3
"""CudaSoA: execution time as a function of the CUDA block size."""

import csv
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

RESULTS = Path("benchmark/results")

rows = sorted(
    csv.DictReader((RESULTS / "cuda_blocksize_time.csv").open()),
    key=lambda r: int(r["block_size"]),
)
block_sizes = [int(r["block_size"]) for r in rows]
times = [float(r["time_seconds"]) for r in rows]

fig, ax = plt.subplots(figsize=(7, 5.5))
positions = range(len(block_sizes))
ax.bar(positions, times, color="#d62728", width=0.6)
ax.set_xticks(positions)
ax.set_xticklabels([str(b) for b in block_sizes])
ax.set_xlabel("Threads per block")
ax.set_ylabel("Execution time (s)")
ax.set_title(f"CudaSoA — Execution Time vs Block Size (N={rows[0]['bodies']})")
ax.grid(True, alpha=0.3, axis="y")
ax.spines["top"].set_visible(False)
ax.spines["right"].set_visible(False)

for x, t in zip(positions, times):
    ax.text(x, t, f"{t:.3f}", ha="center", va="bottom", fontsize=9)

fig.tight_layout()
fig.savefig(RESULTS / "cuda_blocksize_time.png", dpi=200)
print("wrote", RESULTS / "cuda_blocksize_time.png")
