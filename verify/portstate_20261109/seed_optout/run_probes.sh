#!/bin/bash
# PORT-SEED-OPTOUT arm 3 probes (PREREG section 2.3): P1 opt-out + videocfg absent, P2 opt-out + present (P1's file),
# P3 default seed + absent. B recipe env so each run self-exits. Run inside test-slot + mashed_lock.py run.
O=D:/MashedWT/PORT-SEED-OPTOUT_data; X=$O/exe_after
REC="--env MASHED_TRACK_VIEW=Training --env MASHED_CAR=1 --env MASHED_ROUND=1 --env MASHED_ROUND_RULE=0 --env MASHED_GAME_MODE=10 --env MASHED_DETERMINISTIC=1 --env MASHED_DET_FRAMES=1200 --env MASHED_DET_NO_AMBIENT_KEYS=1 --env MASHED_MUTE=1 --env MASHED_WIN_POS=primary-bl"
for c in p1 p2 p3; do rm -rf $O/cwd_$c; mkdir -p $O/cwd_$c/log; done
py -3.12 optout_probe.py --exe $X/mashed_re.exe --map $X/mashed_re.map --root D:/MashedWT/PORT-SEED-OPTOUT_root --cwd $O/cwd_p1 --out $O/probe_p1.json --env MASHED_NO_PORTSTATE_SEED=1 $REC --limit 120
cp $O/cwd_p1/videocfg.bin $O/cwd_p2/ 2>/dev/null
py -3.12 optout_probe.py --exe $X/mashed_re.exe --map $X/mashed_re.map --root D:/MashedWT/PORT-SEED-OPTOUT_root --cwd $O/cwd_p2 --out $O/probe_p2.json --env MASHED_NO_PORTSTATE_SEED=1 $REC --limit 120
py -3.12 optout_probe.py --exe $X/mashed_re.exe --map $X/mashed_re.map --root D:/MashedWT/PORT-SEED-OPTOUT_root --cwd $O/cwd_p3 --out $O/probe_p3.json $REC --limit 120
for c in p1 p2 p3; do echo "== $c"; grep -h "videocfg\]" $O/cwd_$c/log/mashed_re.log 2>/dev/null; sha256sum $O/cwd_$c/videocfg.bin 2>/dev/null; done
