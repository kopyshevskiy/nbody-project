#!/usr/bin/env bash
# SerialSoA (CPU) vs CudaSoA (GPU, block size 256): execution time.
set -eu

binary=${1:-build-cuda/nbody}
steps=100
out=benchmark/results/cuda_comparison_time.csv

mkdir -p benchmark/results
echo "implementation,bodies,steps,time_seconds" > "$out"

for bodies in 512 1024 2048 4096 8192; do
    for impl in serial-soa cuda; do
        line=$(taskset -c 0 "$binary" --implementation "$impl" \
            --bodies "$bodies" --steps "$steps" --no-output)
        echo "$impl,$bodies,$steps,${line##*time_seconds=}" >> "$out"
        printf '%-11s bodies=%-5s\n' "$impl" "$bodies"
    done
done
