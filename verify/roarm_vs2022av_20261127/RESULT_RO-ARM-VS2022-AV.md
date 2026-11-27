not met: no verdict yet. L0 put a disk hold on (C: 21 GB then 13 GB free, under the 25 GB line), so no arm and no further build ran. Verdict word ("old fault" / "new fault") is open.

## Done
- Worktrees: `.worktrees/RO-ARM-VS2022-AV` (orch/RO-ARM-VS2022-AV @ 1379467a6) and `.worktrees/RO-ARM-VS2022-AV-old` (detached @ 77dedfece, run 54 start head). Each about 7.4 GB on C:.
- Built VS2022 exe at 1379467a6 only: `D:/MashedWT/RO-ARM-VS2022-AV_data/mashed_re_new_vs.exe` (build_new_vs.log, rc 0, 4,444,160 bytes). The object build at 1379467a6 was started and stopped by me on the hold (build_new_ddk.log incomplete). Nothing built at 77dedfece.
- No arm ran, no game launched, no frida folders created by this row (Temp holds 698 frida-* folders, none mine).

## Fault address already in the G5 logs (not new data, from verify/knobonboots_20261121)
The exe's own VEH (`RwCore_Veh`, RwCore_Knob.cpp:160) logged `AV at` in every failing arm:
- VS2022 exe, `arms_vs/RO_ROLOADRWC-pr1.log`: AV at 00186339 (exe+0x176339), read of 00000000, esi 00000001 edi 005F2728, return chain +0x176589 +0x1758B3 +0x1755E1 +0x16E364. 005F2728 is the table passed to `Orig_0040d270` in the object-build arm log. exe+0x176339 is a VS2022 address: map it against that build's mashed_re.map to name the function. [UNCERTAIN] function name not looked up.
- Object build without image, `arms_ddk_noimg/RO_ROLOADRWC-pr1.log`: AV at 000166EC (exe+0x66EC), read 00000000, esi 005F2728.
So the fault is a NULL read at ~6 s, consumer of table 0x005F2728. Old/new question remains.

## Next (after the hold lifts)
1. Build VS2022 + object at 77dedfece (and object at 1379467a6). Needs about 1 GB of obj output per build beyond the worktrees.
2. One knob-off `RO_ROLOADRWC-pr1` arm per setup per head via `verify/knobonboots_20261121/run_arms_wrapped.py` (G5_EXE selects the exe), object build with `original/mashed_image.bin` absent.
3. Read the `AV at` line, map the exe offset in the build's .map, state the verdict.
