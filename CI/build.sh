#!/usr/bin/env bash
# Compile gate. Runs ON the Windows UE5.8 host. UE_ROOT = the UE5.8 (source) engine root.
set -uo pipefail
: "${UE_ROOT:?set UE_ROOT to the UE5.8 engine root}"
UPROJECT="$(cd "$(dirname "$0")/.." && pwd)/ProjectEmergence.uproject"
"$UE_ROOT/Engine/Build/BatchFiles/RunUAT.sh" BuildCookRun -project="$UPROJECT" \
  -noP4 -platform=Win64 -clientconfig=Development -build -nocook -stage=false || exit 1
echo '{"gate":"build","ok":true}'
