#!/bin/bash
# PORT-COLOURSEL-COMMIT arms 2/3 (exe): MASHED_GOTO=1 + MASHED_NAV_KEYS e,e (cup-flow colour select), the moves of arm 1,
# e (colour confirm, 0xff1d0000), e (challenge confirm), then 41 unmapped 'x' taps (~20 s) so the race loads before the
# key script quits. commit_exe_poll.py records 0x0067f0c0 / 0x0067ea98 / 0x007f1a1c and the livery lines. Run inside
# test-slot; each boot takes mashed_lock itself.
D=D:/MashedWT/PORT-COLOURSEL-COMMIT_data
L="py -3.12 C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/orch-integration/re/orchestrator/mashed_lock.py run --label PORT-COLOURSEL-COMMIT --"
X=$(printf 'x,%.0s' $(seq 1 40))x
for spec in "on:n1:" "on:n3:r,r," "on:n6:r,r,r,r,r,r,r," "on:n5l:r,r,r,r,r,r,l," "off:n3:r,r," "off:n6:r,r,r,r,r,r,r,"; do
  knob=${spec%%:*}; rest=${spec#*:}; lab=${rest%%:*}; mv=${rest#*:}
  E=(); [ "$knob" = on ] && E=(--env MASHED_COLOURSEL_ICON=1)
  $L py -3.12 verify/portstate_20261109/coloursel/commit_exe_poll.py --exe $D/exe/mashed_re.exe \
     --root D:/MashedWT/PORT-COLOURSEL-COMMIT_root --out $D/exe_${knob}_${lab}.json --keys e,e,${mv}e,e,$X "${E[@]}" --limit 90 \
     > $D/exe_${knob}_${lab}.log 2>&1
  echo "$knob $lab rc=$?"; grep -a "1a1c=\|livery: slot 0" $D/exe_${knob}_${lab}.log | tail -2
done
