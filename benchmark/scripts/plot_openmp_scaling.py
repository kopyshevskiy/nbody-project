#!/usr/bin/env python3
"""OpenMPSoA strong and weak scaling, with physical-core / SMT-sibling bands."""

import csv
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

RESULTS = Path("benchmark/results")
COLOR = "#17a2b8"
PHYSICAL_CORES = 4

times = {}
for row in csv.DictReader((RESULTS / "openmp_scaling.csv").open()):
    times[(row["scaling"], int(row["threads"]))] = float(row["time_seconds"])

threads = sorted({t for s, t in times if s == "strong"})
strong = [times[("strong", t)] for t in threads]
speedup = [strong[0] / v for v in strong]
weak = [times[("weak", t)] for t in threads]


def axes(ylabel, top):
    fig, ax = plt.subplots(figsize=(8, 5.5))
    boundary = PHYSICAL_CORES + 0.5
    ax.axvspan(min(threads) - 0.5, boundary, color="#f7d4d4", zorder=0)
    ax.axvspan(boundary, max(threads) + 0.5, color="#d6e4f5", zorder=0)
    ax.set_xlim(min(threads) - 0.5, max(threads) + 0.5)
    ax.set_ylim(0, top)
    ax.set_xlabel("Number of threads")
    ax.set_ylabel(ylabel)
    ax.set_xticks(threads)
    ax.grid(True, alpha=0.35, color="white", linewidth=1.2)
    ax.set_axisbelow(True)
    for side in ("top", "right"):
        ax.spines[side].set_visible(False)
    ax.text(2.5, top * 0.96, "physical cores", ha="center", va="top",
            fontsize=10, color="#8a4a4a")
    ax.text(6.5, top * 0.96, "SMT siblings", ha="center", va="top",
            fontsize=10, color="#3a5a8a")
    return fig, ax


fig, ax = axes("Speedup  $T(1)/T(P)$", max(threads) * 1.05)
ax.plot(threads, threads, "k--", linewidth=1.3, label="Ideal speedup", zorder=2)
ax.plot(threads, speedup, marker="o", markersize=7, color=COLOR, linewidth=2.2,
        label="Measured", zorder=3)
ax.set_title("OpenMP Strong Scaling — $N = 8192$")
ax.legend(frameon=True, facecolor="white", framealpha=0.9, fontsize=10, loc="lower right")
fig.tight_layout()
fig.savefig(RESULTS / "openmp_strong.png", dpi=200)
plt.close(fig)

fig, ax = axes("Execution time (s)", max(weak) * 1.12)
ax.axhline(weak[0], color="k", linestyle="--", linewidth=1.3,
           label="Ideal (constant)", zorder=2)
ax.plot(threads, weak, marker="o", markersize=7, color=COLOR, linewidth=2.2,
        label="Measured", zorder=3)
ax.set_title(r"OpenMP Weak Scaling — $N(P) = 8192\,\sqrt{P}$")
ax.legend(frameon=True, facecolor="white", framealpha=0.9, fontsize=10, loc="lower right")
fig.tight_layout()
fig.savefig(RESULTS / "openmp_weak.png", dpi=200)
plt.close(fig)

print("wrote", RESULTS / "openmp_strong.png")
print("wrote", RESULTS / "openmp_weak.png")
for t, s, sp, w in zip(threads, strong, speedup, weak):
    print(f"  P={t}  strong={s:7.3f}s  speedup={sp:.3f}  weak={w:7.3f}s")
