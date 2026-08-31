#!/usr/bin/env bash
# CudaSoA: effect of the CUDA block size (threads per block) on execution
# time, for a fixed number of bodies. Each block size is a separate binary,
# built with -DCUDA_BLOCK_SIZE=<size> passed through CMAKE_CUDA_FLAGS
# (see run_cuda_build.sh), since the shared-memory tile is sized at compile
# time.
set -eu

bodies=8192
steps=100
out=benchmark/results/cuda_blocksize_time.csv

mkdir -p benchmark/results
echo "block_size,bodies,steps,time_seconds" > "$out"

for bs in 64 128 256 512; do
    binary="build-cuda-bs${bs}/nbody"
    line=$(taskset -c 0 "$binary" --implementation cuda \
        --bodies "$bodies" --steps "$steps" --no-output)
    echo "$bs,$bodies,$steps,${line##*time_seconds=}" >> "$out"
    printf 'block_size=%-4s bodies=%-5s\n' "$bs" "$bodies"
done
