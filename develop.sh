#!/usr/bin/env bash
# Open VS Code inside the pinned environment. Replaces the old
# `nix-shell --run "code ."`: the editor, clangd and the toolchain now come from
# the flake (see flake.nix, devShells.vscode).
set -euo pipefail

NAME=$(basename "$PWD")
TMPDIR="/tmp/code-$NAME"
mkdir -p "$TMPDIR"

exec nix develop ".#vscode" --command env TMPDIR="$TMPDIR" code .
