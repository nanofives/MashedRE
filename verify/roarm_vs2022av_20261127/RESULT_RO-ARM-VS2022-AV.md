Both exes at head 1379467a6 fault in `Orig_0040d270` (table 005F2728, NULL read, ~frame 300 of the replay arm): object build = same address as G5 (old fault, exe+0x66EC); VS2022 = fault moved (exe+0x176339 -> exe+0x17A159), now named `_Orig_0040d270`+0x109. The G5 VS2022 build has no map kept, so its old function is [UNCERTAIN].

## Verdict per exe
| exe | G5 fault (RESULT_KNOBON-BOOTS-G5) | this row, knob-off RO_ROLOADRWC-pr1, no staged mashed_image.bin | function (map) | verdict |
|---|---|---|---|---|
| object build (ddkobj, head 1379467a6) | AV 000166EC (exe+0x66EC), esi 005F2728 | AV at 000166EC (exe+0x66EC) read 00000000, esi 005F2728 | `_Orig_0040d270` 0001:00005690 VA 0x16690 +0x5C, ddk_top07_0040d270_PortTop_0040d270.obj (ddk_map.map line 2804) | old fault (identical address and esi) |
| VS2022 (head 1379467a6) | AV 00186339 (exe+0x176339), esi 1 edi 005F2728 | AV at 0018A159 (exe+0x17A159) read 00000000, esi 00000001 edi 005F2728 | `_Orig_0040d270` 0001:00179050 VA 0x18A050 +0x109, PortTop_0040d270.obj (vs_map.map line 2637) | fault moved (address differs, same registers esi 1 / edi 005F2728, same NULL read) |

- Preferred load base 0x00010000 in both maps (line 5), so the log's `AV at 0018xxxx` = map VA and `exe+X` = VA - 0x10000.
- The arm log line before the VS fault: `rva 0x0040d270: MASHED_OT_RACELOAD=1, matched body Orig_0040d270(table 005F2728, row 0) entered, call 1 (PORT-OT-RACE)` (arms_vs_map log line 461), then the AV. Both faults are inside the matched body of 0x0040d270 on table 005F2728.
- Why the VS2022 address moved: the old G5 exe was built from an older tree, so exe+0x176339 and exe+0x17A159 are different layouts, not necessarily different faults. At head 1379467a6 the old address 0x176339 falls in `_PortTop_Sel_00429290`+0x39 (vs_map.map line 2527), which is not the fault. The old build's own map was not kept, so "same function in the G5 exe" is [UNCERTAIN]. Test: rebuild VS2022 at the G5 head (needs a second worktree, not made, step 3 of the brief) and map exe+0x176339.

## Checks run
- VS2022 build at c67f372e7 (head 1379467a6 content), rc 0, build_vs_map.log, map vs_map.map. Object build, rc 0, ddkobj check IDENTICAL 2360/2526, DIFFERENT 78, not-in-map 88, 34 refusals (externalized-static-register-convention=13, wrapper-redefines-globals=21); no newly classified units (build only, no code change).
- Arms: `verify/roarm_vs2022av_20261127/run_arm.py` (test-slot outside, mashed_lock inside, private root D:\MashedWT\RO-ARM-VS2022-AV_root, `original\` has no mashed_image.bin, timeout 7200000). First attempt failed (root missing, robocopy under bash did not run), second attempt ran.
- No code changed, no knob flipped, gates not run (evidence row).

## Raw files (D:/MashedWT/RO-ARM-VS2022-AV_data) sha256
- vs_map.exe 414364d249245c06f2591a3985d1b56c68d2179c24e31d7c683b3f8bdc797108
- ddk_map.exe 1ecf84d9f7435e4ce9e80c89aa138b511db31e5caaea0f50daabf28b53aebf5c
- vs_map.map 92cb927eed5866bc1938dfda3c2166b36116816bcd3bd5829195caba6141dd06
- ddk_map.map 2056302a3fa552c5a52484df50341f7e8bc849ec08416fb3ed7a0e14fc9779a1
- arms_vs_map/RO_ROLOADRWC-pr1.log 0b7d6e7fb73aa737bf33a562ae191f633baabdda38cda740c50e02354fa312b3
- arms_ddk_map/RO_ROLOADRWC-pr1.log da60341cc180fbb349fc113a2a4c7bb34232b731c1a326d6ad183bfe55769b4e

## frida-<hash> folders in %LOCALAPPDATA%\Temp
3 at the end (0 at my first check, earlier worker saw 698). None created by this row (no Frida used).
