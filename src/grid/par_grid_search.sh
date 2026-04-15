#!/bin/bash

# Fixed values for the benchmark campaign
N_VALUE=50000000
P_VALUE=256

# Variable values explored by benchmark.sh
SEED_VALUES=(13)
MAX_KEY_VALUES=(65536 262144)
PARTITION_THREAD_VALUES=(4 8 16 32 64)
JOIN_THREAD_VALUES=(4 8 16 32 64)
