#!/usr/bin/env bash
set -euo pipefail

CSV="experiments_plan_final.csv"
BIN="./bin/interrupts_sim"
OUTDIR="output_files"
VEC="vector_table.txt"
DEV="device_table.txt"

# Ensure necessary bits
[[ -x "$BIN" ]] || BIN="./bin/interrupts"
[[ -x "$BIN" ]] || { echo "❌ Simulator binary not found in ./bin"; exit 1; }
[[ -f "$CSV" ]] || { echo "❌ CSV not found: $CSV"; exit 1; }
[[ -f "$VEC" && -f "$DEV" ]] || { echo "❌ Missing vector_table.txt or device_table.txt"; exit 1; }

mkdir -p "$OUTDIR"

# Strip comments/blank lines from traces so the parser won’t choke on '#'
sanitize_trace() {
  local in="$1"; local tmp
  tmp="$(mktemp)"
  awk 'NF && $0 !~ /^[[:space:]]*#/' "$in" > "$tmp"
  echo "$tmp"
}

total=$(($(wc -l < "$CSV") - 1))
idx=0
echo "🚀 Starting $total test cases..."

# Skip header; read CSV columns
tail -n +2 "$CSV" | while IFS=, read -r case_id trace_path context isr vec speed out_path notes_rest; do
  idx=$((idx+1))

  # Trim
  case_id="$(echo "$case_id" | xargs)"
  trace_path="$(echo "$trace_path" | xargs)"
  context="$(echo "$context" | xargs)"
  isr="$(echo "$isr" | xargs)"
  vec="$(echo "$vec" | xargs)"
  speed="$(echo "$speed" | xargs)"
  out_path="$(echo "$out_path" | xargs)"

  printf "\n[%02d/%02d] ▶️ Case %s | trace=%s | ctx=%sms | isr=%sms | vec=%s | cpu=%sx\n" \
    "$idx" "$total" "$case_id" "$trace_path" "$context" "$isr" "$vec" "$speed"

  if [[ ! -f "$trace_path" ]]; then
    echo "⚠️  Missing trace: $trace_path — skipping"
    continue
  fi

  sanitized="$(sanitize_trace "$trace_path")"
  export CONTEXT_MS="$context" ISR_MS="$isr" VECTOR_BYTES="$vec" CPU_SPEED="$speed"

  # Run simulator; it writes real output to execution.txt
  "$BIN" "$sanitized" "$VEC" "$DEV" >/dev/null
  rm -f "$sanitized"

  if [[ ! -f execution.txt ]]; then
    echo "❌ execution.txt missing after Case $case_id!"
    exit 1
  fi

  # Save per-case output
  mkdir -p "$(dirname "$out_path")"
  cp -f execution.txt "$out_path"
  echo "   → wrote $out_path"
done

echo -e "\n✅ All $total cases completed. Results in '$OUTDIR/'"
