#!/bin/bash
#SBATCH --job-name=ass3_heat
#SBATCH --partition=short
#SBATCH --nodes=1
#SBATCH --cpus-per-task=16
#SBATCH --time=00:30:00

# 1. Compile the OpenMP C++ code
g++ -w -O3 -fopenmp -I${HOME}/cfitsio-4.6.2/ -L${HOME}/cfitsio-4.6.2/.libs -o heat_omp heat_omp.cpp fitsfile.cpp -lcfitsio -lm

# 2. Tell Linux where the shared library is hiding before running!
export LD_LIBRARY_PATH=${HOME}/cfitsio-4.6.2/.libs:$LD_LIBRARY_PATH

echo "=============== AMDAHL'S LAW ==============="
# The problem size remains strictly fixed at 2000x2000.
FIXED_GRID=2000

for THREADS in 1 4 8 12 16; do
    export OMP_NUM_THREADS=$THREADS
    ./heat_omp $FIXED_GRID
done

echo ""
echo "=============== GUSTAFSON'S LAW ==============="
# The problem size scales proportionally with the thread count.
# Baseline (1 thread): 400x400 (160,000 pixels)
# Grid Dimension Calculation: 400 * sqrt(THREADS)

THREADS_ARR=(1 4 8 12 16)
GRID_ARR=(400 800 1131 1385 1600)

for i in "${!THREADS_ARR[@]}"; do
    export OMP_NUM_THREADS=${THREADS_ARR[$i]}
    SCALED_GRID=${GRID_ARR[$i]}
    ./heat_omp $SCALED_GRID
done