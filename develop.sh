#!/usr/bin/env bash
export NIXPKGS_ALLOW_UNFREE=1
NAME=`basename "$PWD"`
mkdir "/tmp/code-$NAME"
nix-shell --run "TMPDIR=\"/tmp/code-$NAME\" code ."
