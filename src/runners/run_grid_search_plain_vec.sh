#!/bin/bash
#SBATCH --job-name=plain_vec_with_timeout_5min
#SBATCH --time=00:10:00
#SBATCH --nodes=1
#SBATCH --partition=gpu-shared
#SBATCH --output=out/slurm-%j-plain_vec.log
#SBATCH --error=err/slurm-%j-plain_vec.log

set -e

cd "$SLURM_SUBMIT_DIR"
OUTPUT_CSV=${1:-}
CONFIG_FILE=${2:-"$SLURM_SUBMIT_DIR/runners/grid_config.sh"}
REPEAT_COUNT=${3:-1}

if [ ! -f "$CONFIG_FILE" ]; then
    echo "Configuration file not found: $CONFIG_FILE"
    exit 1
fi

if ! [[ "$REPEAT_COUNT" =~ ^[1-9][0-9]*$ ]]; then
    echo "Repeat count must be a positive integer, received: $REPEAT_COUNT"
    exit 1
fi

source "$CONFIG_FILE"

TOTAL=$(( ${#N_VALUES[@]} * ${#P_VALUES[@]} * ${#HASH_VALUES[@]} * REPEAT_COUNT ))
COUNT=0

for ((RUN_INDEX=1; RUN_INDEX<=REPEAT_COUNT; RUN_INDEX++)); do
    for N in "${N_VALUES[@]}"; do
        for P in "${P_VALUES[@]}"; do
            for HASH in "${HASH_VALUES[@]}"; do
                COUNT=$((COUNT + 1))

                printf '[%d/%d] run=%d/%d N=%s P=%s HASH=%s ' \
                    "$COUNT" "$TOTAL" "$RUN_INDEX" "$REPEAT_COUNT" "$N" "$P" "$HASH"

                CMD=(bash "$SLURM_SUBMIT_DIR/runners/run_plain_vec.sh" "$N" "$P" "$HASH")
                if [ -n "$OUTPUT_CSV" ]; then
                    CMD+=("$OUTPUT_CSV")
                fi

                "${CMD[@]}"
            done
        done
    done
done

echo ""
echo "All jobs submitted."
