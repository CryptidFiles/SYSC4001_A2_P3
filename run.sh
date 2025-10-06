#!/usr/bin/env bash
set -euo pipefail

CSV="experiments_plan_final.csv"   # <-- uses the FINAL csv
BIN="./bin/interrupts_sim"
OUTDIR="output_files"
mkdir -p "$OUTDIR"

if [[ ! -x "$BIN" ]]; then
  echo "Error: $BIN not found. Run 'source build.sh' first."
  exit 1
fi
if [[ ! -f "$CSV" ]]; then
  echo "Error: $CSV not found in repo root."
  exit 1
fi

total=$(($(wc -l < "$CSV") - 1))
idx=0

# Read 7 fixed fields; slurp the rest (notes) into notes_rest so commas don't break parsing
tail -n +2 "$CSV" | while IFS=, read -r case_id trace_path context isr vec speed out_path notes_rest; do
  idx=$((idx+1))
  printf "\n[%02d/%02d] Case %s  ctx=%s  isr=%s  vector=%s  cpu=%sx\n" \
    "$idx" "$total" "$case_id" "$context" "$isr" "$vec" "$speed"

  if [[ ! -f "$trace_path" ]]; then
    echo "  ✖ Trace '$trace_path' not found."
    exit 1
  fi

  # Run simulator (it writes to execution.txt)
  CONTEXT_MS="$context" ISR_MS="$isr" VECTOR_BYTES="$vec" CPU_SPEED="$speed" \
    "$BIN" "$trace_path" vector_table.txt device_table.txt

  # Copy the produced execution.txt to the per-case output
  if [[ ! -f "execution.txt" ]]; then
    echo "  ✖ execution.txt not found after run."
    exit 1
  fi
  cp -f "execution.txt" "$out_path"
  echo "  → wrote $out_path"
done

echo -e "\nAll cases completed. See $OUTDIR/."
