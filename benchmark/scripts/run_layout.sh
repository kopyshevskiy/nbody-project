#!/usr/bin/env bash
# SerialAoS vs SerialSoA: execution time and L1 data-cache miss rate.
set -eu

binary=${1:-build/nbody}
steps=100
time_out=benchmark/results/layout_time.csv
perf_out=benchmark/results/layout_perf.csv

mkdir -p benchmark/results
echo "implementation,bodies,steps,time_seconds" > "$time_out"
echo "implementation,bodies,steps,l1_loads,l1_load_misses" > "$perf_out"

for bodies in 1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192; do
    for impl in serial-aos serial-soa; do
        line=$(taskset -c 0 "$binary" --implementation "$impl" \
            --bodies "$bodies" --steps "$steps" --no-output)
        echo "$impl,$bodies,$steps,${line##*time_seconds=}" >> "$time_out"

        log=$(mktemp)
        taskset -c 0 perf stat -e L1-dcache-loads,L1-dcache-load-misses \
            "$binary" --implementation "$impl" --bodies "$bodies" \
            --steps "$steps" --no-output > /dev/null 2> "$log"
        loads=$(grep -oP '^\s*[0-9,]+(?=\s+L1-dcache-loads)' "$log" | tr -d ,)
        misses=$(grep -oP '^\s*[0-9,]+(?=\s+L1-dcache-load-misses)' "$log" | tr -d ,)
        echo "$impl,$bodies,$steps,$loads,$misses" >> "$perf_out"
        rm -f "$log"

        printf '%-11s bodies=%-5s\n' "$impl" "$bodies"
    done
done
