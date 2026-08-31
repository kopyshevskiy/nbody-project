#!/usr/bin/env bash
# OpenMPSoA strong scaling (fixed N) and weak scaling (N = N_base * sqrt(P)).
# Threads 1-4 land on distinct physical cores, threads 5-8 add the SMT siblings.
set -eu

binary=${1:-build/nbody}
steps=100
base=8192
out=benchmark/results/openmp_scaling.csv

mkdir -p benchmark/results
echo "scaling,threads,bodies,steps,schedule,time_seconds" > "$out"

run() {
    local scaling=$1 threads=$2 bodies=$3 schedule=static
    [ "$threads" -gt 4 ] && schedule=dynamic

    line=$(OMP_NUM_THREADS="$threads" OMP_SCHEDULE="$schedule,1" OMP_DYNAMIC=false \
        taskset -c "0-$((threads - 1))" "$binary" --implementation openmp \
        --bodies "$bodies" --steps "$steps" --no-output)
    echo "$scaling,$threads,$bodies,$steps,$schedule,${line##*time_seconds=}" >> "$out"
    printf '%-6s threads=%s bodies=%-6s\n' "$scaling" "$threads" "$bodies"
}

for threads in 1 2 3 4 5 6 7 8; do
    run strong "$threads" "$base"
done

for threads in 1 2 3 4 5 6 7 8; do
    bodies=$(awk -v b="$base" -v p="$threads" 'BEGIN { printf "%.0f", b * sqrt(p) }')
    run weak "$threads" "$bodies"
done
