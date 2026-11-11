#!/bin/bash
# EQUIV-ADMIT-1: re-run equiv.py (verdict lines at the tool commit) and the Unicorn cross-check (default and seeded)
# over the 33 rows released by L-126. Run from the worktree root.
D=verify/equivadmit_20261111
tail -n +2 $D/admit33_in.tsv | while IFS=$'\t' read -r rva src rest; do
  i=$((i+1))
  out=$(timeout 600 py -3.12 re/tools/match/equiv.py "$src" --only "$rva" 2>&1 | grep -E "^0x|EQUIV|NOT-PROVEN" | head -3 | tr '\n' ' ')
  echo "[$i/33] $rva $src :: $out"
done | tee $D/equiv33.log
py -3.12 re/tools/match/equiv_dyn.py --tsv $D/admit33_in.tsv --runs 16 --out $D/dyn33_default.tsv 2>&1 | tee $D/dyn33_default.log
py -3.12 re/tools/match/equiv_dyn.py --tsv $D/admit33_in.tsv --runs 16 --seeded --out $D/dyn33_seeded.tsv 2>&1 | tee $D/dyn33_seeded.log
