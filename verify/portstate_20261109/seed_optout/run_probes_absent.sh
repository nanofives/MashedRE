#!/bin/bash
# PORT-SEED-OPTOUT arm 3, absent case: the exe resolves videocfg.bin in MASHED_ROOT, so the root's copy (written by this
# step's own H_DEF arm) is moved aside before each run. P4 opt-out + absent, P5 default seed + absent.
O=D:/MashedWT/PORT-SEED-OPTOUT_data; X=$O/exe_after; R=D:/MashedWT/PORT-SEED-OPTOUT_root
REC="--env MASHED_TRACK_VIEW=Training --env MASHED_CAR=1 --env MASHED_ROUND=1 --env MASHED_ROUND_RULE=0 --env MASHED_GAME_MODE=10 --env MASHED_DETERMINISTIC=1 --env MASHED_DET_FRAMES=1200 --env MASHED_DET_NO_AMBIENT_KEYS=1 --env MASHED_MUTE=1 --env MASHED_WIN_POS=primary-bl"
mkdir -p $R/log $O/vcfg; [ -f $R/videocfg.bin ] && mv $R/videocfg.bin $O/vcfg/root_before.bin
py -3.12 optout_probe.py --exe $X/mashed_re.exe --map $X/mashed_re.map --root $R --cwd $O/cwd_p4 --out $O/probe_p4.json --env MASHED_NO_PORTSTATE_SEED=1 $REC --limit 120
mv $R/videocfg.bin $O/vcfg/p4.bin
py -3.12 optout_probe.py --exe $X/mashed_re.exe --map $X/mashed_re.map --root $R --cwd $O/cwd_p5 --out $O/probe_p5.json $REC --limit 120
cp $R/videocfg.bin $O/vcfg/p5.bin
grep -h "videocfg\]" $R/log/mashed_re.log
sha256sum $O/vcfg/*.bin
