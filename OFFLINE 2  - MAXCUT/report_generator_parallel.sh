#!/usr/bin/env bash
set -euo pipefail

g++ -std=c++17 -O3 main.cpp -o main.exe

OUTPUT_FILE="Results/2105010.csv"
TEMP_DIR="Results/temp"
mkdir -p "$(dirname "$OUTPUT_FILE")"
mkdir -p "$TEMP_DIR"

echo "Name,n,m,randomized,greedy,semi_greedy,local_search_value,local_search_iters,grasp_value,grasp_iters,known_best" > "$OUTPUT_FILE"

declare -A KNOWN_BEST=(
  [g1]=12078
  [g2]=12084
  [g3]=12077
  [g11]=627
  [g12]=621
  [g13]=645
  [g14]=3187
  [g15]=3169
  [g16]=3172
  [g22]=14123
  [g23]=14129
  [g24]=14131
  [g32]=1560
  [g33]=1537
  [g34]=1541
  [g35]=8000
  [g36]=7996
  [g37]=8009
  [g43]=7027
  [g44]=7022
  [g45]=7020
)

# function to process a single graph
process_graph() {
  local graph_num=$1
  local graph_name="g${graph_num}"
  local graph_file="graphs/${graph_name}.rud"
  local temp_output="${TEMP_DIR}/${graph_name}.csv"
  
  echo "[$(date +%H:%M:%S)] Processing $graph_name..."
  
  # Get number of vertices and edges from first line of the graph file
  if [[ ! -f "$graph_file" ]]; then
    echo "Error: File $graph_file not found" >&2
    return 1
  fi
  
  local n m
  read -r n m < <(head -n1 "$graph_file" | tr -d '\r')
  
  # Running all algos
  echo "  Running randomized..."
  local randomized=$(./main.exe "$graph_file" randomized | tr -d '\r\n')
  
  echo "  Running greedy..."
  local greedy=$(./main.exe "$graph_file" greedy | tr -d '\r\n')
  
  echo "  Running semi_greedy..."
  local semi_greedy=$(./main.exe "$graph_file" semi_greedy | tr -d '\r\n')
  
  echo "  Running local_search..."
  local ls_output=$(./main.exe "$graph_file" local_search | tr -d '\r\n')
  local local_search_val local_search_iters
  read -r local_search_val local_search_iters <<< "$ls_output"
  
  echo "  Running grasp..."
  local grasp_output=$(./main.exe "$graph_file" grasp | tr -d '\r\n')
  local grasp_val grasp_iters
  read -r grasp_val grasp_iters <<< "$grasp_output"
  
  
  local known_best="${KNOWN_BEST[$graph_name]:-}"
  
  # writing results to temporary file cause parallel jobs might not finish in order
  printf "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n" \
    "$graph_name" "$n" "$m" "$randomized" "$greedy" "$semi_greedy" \
    "$local_search_val" "$local_search_iters" "$grasp_val" "$grasp_iters" "$known_best" > "$temp_output"
  
  echo "[$(date +%H:%M:%S)] Completed $graph_name"
}

# check no. of cores available
if command -v nproc &>/dev/null; then
  NUM_CORES=$(nproc)
else
  NUM_CORES=4
fi

NUM_JOBS=$(( NUM_CORES * 3 / 4 ))
if (( NUM_JOBS < 2 )); then
  NUM_JOBS=2
fi

echo "Using $NUM_JOBS parallel jobs (out of $NUM_CORES detected cores)"

# Function to process a range of graphs sequentially
process_graph_range() {
  local start=$1
  local end=$2
  local pid=$$
  
  for i in $(seq $start $end); do
    process_graph $i
  done
}

# Split 54 graphs into groups for parallel processing
TOTAL_GRAPHS=54
GRAPHS_PER_JOB=$(( TOTAL_GRAPHS / NUM_JOBS + 1 ))

# Start parallel jobs
echo "Starting $NUM_JOBS parallel jobs to process $TOTAL_GRAPHS graphs..."
for job in $(seq 0 $(( NUM_JOBS - 1 ))); do
  start=$(( job * GRAPHS_PER_JOB + 1 ))
  end=$(( (job + 1) * GRAPHS_PER_JOB ))
  
  if (( end > TOTAL_GRAPHS )); then
    end=$TOTAL_GRAPHS
  fi
  
  if (( start <= TOTAL_GRAPHS )); then
    echo "Starting job $job to process graphs $start to $end"
    process_graph_range $start $end &
  fi
done

echo "Waiting for all jobs to complete..."
wait
echo "All processing jobs completed"


echo "Combining results into $OUTPUT_FILE..."
for i in $(seq 1 $TOTAL_GRAPHS); do
  graph_name="g$i"
  temp_file="${TEMP_DIR}/${graph_name}.csv"
  if [[ -f "$temp_file" ]]; then
    cat "$temp_file" >> "$OUTPUT_FILE"
  else
    echo "Warning: Results for $graph_name not found"
  fi
done

# Cleanup temporary files
rm -rf "$TEMP_DIR"

echo "All done! Results written to $OUTPUT_FILE"
echo "Total runtime: $SECONDS seconds"