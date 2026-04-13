#!/bin/bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RUNNER_SCRIPT="$SCRIPT_DIR/runners/run_with_slurm.sh"

cd "$SCRIPT_DIR"
mkdir -p out err

if [ "$#" -lt 1 ] || [ "$#" -gt 3 ]; then
    echo "Usage: $0 EXECUTABLE_NAME [REPEAT_COUNT(optional)] [GRID_CONFIG(optional)]"
    echo "  EXECUTABLE_NAME: make target to compile and benchmark (ex: hashjoin_sequential, hashjoin_parallel)"
    echo "  REPEAT_COUNT: positive integer (default: 1)"
    echo "  GRID_CONFIG: shell file exporting benchmark arrays (default: $SCRIPT_DIR/runners/grid_search.sh)"
    exit 1
fi

EXECUTABLE_INPUT="$1"
EXECUTABLE_TARGET="$(basename "$EXECUTABLE_INPUT")"
REPEAT_COUNT="${2:-1}"
GRID_CONFIG="${3:-$SCRIPT_DIR/runners/grid_search.sh}"

if ! [[ "$REPEAT_COUNT" =~ ^[1-9][0-9]*$ ]]; then
    echo "REPEAT_COUNT must be a positive integer, received: $REPEAT_COUNT"
    exit 1
fi

if [ ! -f "$GRID_CONFIG" ]; then
    echo "Grid configuration file not found: $GRID_CONFIG"
    exit 1
fi
if [ ! -f "$RUNNER_SCRIPT" ]; then
    echo "Runner script not found: $RUNNER_SCRIPT"
    exit 1
fi

source "$GRID_CONFIG"

if [ -z "${N_VALUE:-}" ] || [ -z "${P_VALUE:-}" ]; then
    echo "Grid configuration must define fixed N_VALUE and P_VALUE."
    exit 1
fi

if [ "${#SEED_VALUES[@]:-0}" -eq 0 ] || [ "${#MAX_KEY_VALUES[@]:-0}" -eq 0 ] || \
   [ "${#PARTITION_THREAD_VALUES[@]:-0}" -eq 0 ] || [ "${#JOIN_THREAD_VALUES[@]:-0}" -eq 0 ]; then
    echo "Grid configuration must define non-empty SEED_VALUES, MAX_KEY_VALUES, PARTITION_THREAD_VALUES and JOIN_THREAD_VALUES."
    exit 1
fi

mkdir -p compilation
if ! make "$EXECUTABLE_TARGET"; then
    echo "Compilation failed or unknown make target: $EXECUTABLE_TARGET"
    exit 1
fi
EXECUTABLE="$SCRIPT_DIR/$EXECUTABLE_TARGET"
if [ ! -x "$EXECUTABLE" ]; then
    echo "Compiled executable not found or not executable: $EXECUTABLE"
    exit 1
fi

TOTAL=$(( ${#SEED_VALUES[@]} * ${#MAX_KEY_VALUES[@]} * ${#PARTITION_THREAD_VALUES[@]} * ${#JOIN_THREAD_VALUES[@]} * REPEAT_COUNT ))
COUNT=0

echo "Benchmark executable: $EXECUTABLE"
echo "Grid source:          $GRID_CONFIG"
echo "Fixed parameters:     N=$N_VALUE P=$P_VALUE"
echo "Total runs:           $TOTAL"
echo

for ((RUN_INDEX=1; RUN_INDEX<=REPEAT_COUNT; RUN_INDEX++)); do
    for SEED in "${SEED_VALUES[@]}"; do
        for MAX_KEY in "${MAX_KEY_VALUES[@]}"; do
            for PARTITION_THREADS in "${PARTITION_THREAD_VALUES[@]}"; do
                for JOIN_THREADS in "${JOIN_THREAD_VALUES[@]}"; do
                    COUNT=$((COUNT + 1))

                    printf '[%d/%d] run=%d/%d N=%s P=%s seed=%s max_key=%s partition_threads=%s join_threads=%s\n' \
                        "$COUNT" "$TOTAL" "$RUN_INDEX" "$REPEAT_COUNT" \
                        "$N_VALUE" "$P_VALUE" "$SEED" "$MAX_KEY" "$PARTITION_THREADS" "$JOIN_THREADS"

                    JOB_ID=$(sbatch --parsable --wait \
                        "$RUNNER_SCRIPT" \
                        "$EXECUTABLE" \
                        "$N_VALUE" \
                        "$N_VALUE" \
                        "$SEED" \
                        "$MAX_KEY" \
                        "$P_VALUE" \
                        "$PARTITION_THREADS" \
                        "$JOIN_THREADS")
                done
            done
        done
    done
done
