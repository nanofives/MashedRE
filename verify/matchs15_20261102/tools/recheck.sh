cd /c/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-S15
while read rva src blob m; do
  cur=$(git hash-object "$src")
  by=$(awk -F'\t' -v r="$rva" '$1==r{print $9}' verify/matchs15_20261102/results.tsv)
  extra=""; [ "$by" = "bytes" ] && extra="--bytes"
  mo=$(py -3.12 re/tools/match/match.py --flags "/O2 /GS /Ob1" --toolchain ddk $extra "$src" 2>&1 | tail -1)
  ro=$(py -3.12 re/tools/match/relocheck.py "$src" 2>&1 | grep -i -E "OK|BAD" | head -1)
  wr=$(grep -l "$(basename $src)" mashedmod/src/mashed_re/Match/MatchS15_${rva#0x00}.cpp 2>/dev/null | wc -l)
  echo -e "$rva\tblob_same=$([ "$cur" = "$blob" ] && echo y || echo n)\tby=$by\twrapper_inc=$wr\t$mo\t$ro"
done < /d/MashedWT/MATCH-S15_data/swap_src.txt
