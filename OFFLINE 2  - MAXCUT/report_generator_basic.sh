#!/usr/bin/env bash
set -euo pipefail

# Compile the program
g++ -std=c++17 -O3 main.cpp -o main

# Output file
OUT=Results/summary.csv
mkdir -p $(dirname "$OUT")

# Write header
echo "Name,n,m,randomized,greedy,semi_greedy,local_search_value,local_search_iters,grasp_value,grasp_iters,known_best" > "$OUT"

# Known upper bounds
declare -A UB
UB=(
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
    [g48]=6000
    [g49]=6000 
    [g50]=5988
)

for i in $(seq 1 54); do
  graph="g${i}"
  infile="graphs/${graph}.rud"

  # Read first line of graph file and strip any trailing whitespace
  read -r n m < <(head -n1 "$infile" | tr -d '\r')
  
  # Run algorithms and capture outputs, removing any trailing whitespace
  rand_out=$(./main "$infile" randomized | tr -d '\r\n')
  randomized=$rand_out

  greedy_out=$(./main "$infile" greedy | tr -d '\r\n')
  greedy=$greedy_out

  semi_out=$(./main "$infile" semi_greedy | tr -d '\r\n')
  semi_greedy=$semi_out

  # For multi-value outputs, ensure we handle them properly
  ls_output=$(./main "$infile" local_search | tr -d '\r\n')
  read -r ls_val ls_iter <<< "$ls_output"

  gr_output=$(./main "$infile" grasp | tr -d '\r\n')
  read -r gr_val gr_iter <<< "$gr_output"

  # Get known best value if available
  known_best="${UB[$graph]:-}"

  # Write to CSV file - use printf to ensure no extra newlines
  printf "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n" \
    "$graph" "$n" "$m" "$randomized" "$greedy" "$semi_greedy" \
    "$ls_val" "$ls_iter" "$gr_val" "$gr_iter" "$known_best" >> "$OUT"
done

echo "Results written in $OUT yeayyyyyy!"
