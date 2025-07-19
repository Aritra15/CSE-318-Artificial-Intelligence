# MaxCut Problem Implementation

This repository contains implementations of various algorithms for solving the MaxCut problem.

## Algorithms Implemented

- **Greedy**: Deterministically selects vertices based on maximum edge weights.
- **Randomized**: Randomly assigns vertices to sets X and Y.
- **Semi-Greedy**: Uses a Restricted Candidate List based on α parameter.
- **Semi-Greedy Optimized**: An optimized version with incremental contribution updates.
- **Local Search**: Improves a given solution by moving vertices between partitions.
- **GRASP**: Combines Semi-Greedy construction with Local Search for better solutions.

## Files Description

- `graph.cpp`: Graph representation and utility functions
- `Algorithms.cpp`: Implementation of all MaxCut algorithms
- `main.cpp`: Command-line interface to run individual algorithms
- `report_generator_parallel.sh`: Script to run benchmarks with parallel processing
- `plotMaxcut.ipynb`: Jupyter notebook to visualize and analyze results

## Compilation

```bash
g++ -std=c++17 -O3 main.cpp -o main.exe
```

## Usage

### Running Individual Algorithms

```bash
./main.exe <graph_file> <algorithm_type>
```

Where `algorithm_type` is one of: `greedy`, `randomized`, `semi_greedy`, `local_search`, `grasp`

### Generating Reports

**Note**: The report generation script requires Linux or WSL (Windows Subsystem for Linux) to run.

```bash
chmod +x report_generator_parallel.sh
./report_generator_parallel.sh
```

This will:
1. Run all algorithms on all graph instances
2. Measure their performance and solution quality
3. Generate a CSV file with the results in `Results/2105010.csv`

## Algorithm Parameters

- Semi-Greedy and GRASP: α = 0.6 (controls randomness)
- GRASP: 50 iterations
- Local Search: 10 iterations with Semi-Greedy starting solutions
