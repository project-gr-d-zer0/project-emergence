#!/usr/bin/env bash
# Acceptance gate. Runs named Automation Specs headless, emits machine-readable pass/fail for the evaluator.
set -uo pipefail
: "${UE_ROOT:?set UE_ROOT}"
UPROJECT="$(cd "$(dirname "$0")/.." && pwd)/ProjectEmergence.uproject"
TESTS="${1:-Emergence.}"   # run all Emergence.* by default; pass a name to gate one criterion
"$UE_ROOT/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" "$UPROJECT" \
  -execcmds="Automation RunTests $TESTS; Quit" -unattended -nopause -nosplash -nullrhi -stdout \
  -ReportOutputPath="$(dirname "$0")/../Saved/TestReports"
# Automation writes index.json under ReportOutputPath -> the evaluator parses it for per-It() pass/fail.
