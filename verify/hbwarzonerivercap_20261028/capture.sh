#!/bin/bash
# HB-WARZONE-RIVER-CAP: original Warzone (challenge 11) captures with the poses_r2.json
# basis pinned, at settle times short enough that the race view is still up.
# Usage: capture.sh <case>:<settle> [...]   e.g. warzone_p2:2
R=/d/MashedWT/HB-WARZONE-RIVER-CAP_root
D=/d/MashedWT/HB-WARZONE-RIVER-CAP_data/orig
J=C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/HB-WARZONE-RIVER-CAP/verify/hbtexanimrefs_20261027/poses_r2.json
cd $R
N=0; T=$#
for cs in "$@"; do
  k=${cs%%:*}; s=${cs##*:}
  N=$((N+1)); out=$D/${k}_s$s; mkdir -p $out
  pose=$(py -3.12 -c "import json;print(json.load(open('$J'))['$k'][1])")
  st=FAIL; : > $out/audio_check.txt
  for attempt in 1 2; do
    # Standing rule: check the audio endpoint before EVERY original launch.
    ep=$(powershell.exe -NoProfile -Command "(Get-PnpDevice -Class AudioEndpoint -PresentOnly -ErrorAction SilentlyContinue | Measure-Object).Count" | tr -d '\r')
    echo "  attempt $attempt audio endpoints present: $ep" >> $out/audio_check.txt
    if [ "${ep:-0}" -lt 1 ]; then echo "[$N/$T] ${k}_s$s BLOCKED audio endpoint (0 present)"; exit 3; fi
    MASHED_WIN_POS=left-bl py -3.12 $R/re/tools/run_with_unlocked_save.py $R/original/gamesave_unlocked.bin -- py -3.12 $R/re/frida/race_draw_burst.py --challenge 11 --pose "$pose" --settle $s --settle-from-pose --out $out/orig.bmp > $out/capture.log 2>&1
    rc=$?
    [ -f $out/orig.bmp ] && [ -f $out/orig_cambasis.txt ] && { st=PASS; break; }
  done
  trk=$(grep -o "ORIGINAL TRACK = [A-Za-z]*" $out/capture.log | head -1)
  av=$(grep -c "5baf48\|1baf48" $out/capture.log)
  echo "[$N/$T] ${k}_s$s $st rc=$rc attempt=$attempt endpoints=$ep av5baf48=$av $trk"
done
