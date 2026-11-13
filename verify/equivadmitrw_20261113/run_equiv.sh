#!/bin/bash
# EQUIV-ADMIT-RW: re-run equiv.py per row (verdict line + --json) from the worktree root
D=verify/equivadmitrw_20261113
n=$(($(wc -l < $D/in.tsv)-1)); i=0
tail -n +2 $D/in.tsv | while IFS=$'\t' read -r rva src grp prior; do
  i=$((i+1))
  out=$(timeout 600 py -3.12 re/tools/match/equiv.py "$src" --only "$rva" 2>&1 | grep -E "^0x" | head -2 | tr '\n' ' ')
  echo "[$i/$n] $rva $src :: $out"
done
