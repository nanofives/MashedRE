# RESULT KNOBON-BOOTS-G5 (run 55, goal G5): knob boots, evidence only

Row kind: evidence only (knob boots). Binds no RVA, edits no source, no default changed. Branch `orch/KNOBON-BOOTS-G5` from
`orch/integration` @ 39ee297f2. Worker cmwr14k1h0b7fp121xvmtq0ew (claude-sonnet-5-5 medium), lead cmwr0mi2n088jp121jgz9oroy.
Files added: see section 6.

## Verdict

1. **Neither `MASHED_OT_RL_D110` (0x0040d110) nor `MASHED_OT_TITLETICK` (0x00429290) executes its bound body on any boot run.** The replay arm
   reaches the matched course load and stops at the Lua library stop 0x004B7330 before either call site, and the title tick pointer is
   recorded and never called. Both knobs change nothing observable: same stop, same bound-body set, same line counts as the knob-off arm.
2. **The race-frame knob (`MASHED_OT_RACEFRAME`, 0x004111c0) executes 0 times** (`0x004111c0 calls 0` at every logged frame): no arm leaves
   phase 1 for a race tick before the 0x004B7330 stop.
3. **The race-frame replay arm sets `MASHED_RWC_FRAME=1`**: stack `ROLOADRWC` (the one whose arms reach the race load) sets it, so there
   0x00492e90 runs as the RW core body. The PORT-TOP-06 knob-boot recipe and the plain `ROLOAD` stack do not set it. Section 4.

## 1. Setup

| item | value |
|---|---|
| VS2022 exe | `mashed_re_vs.exe` SHA-256 `17cf3e8c77ab53ea7992d06020d660bad341ed815f0ab32e78fe9d9dc7e00cc4` (`build_vs.log`: Build OK, `MASHED_NO_DEPLOY=1`) |
| object build exe | `mashed_re_ddk.exe` SHA-256 `476f7d437499e5cf006dda103a3652e3c834e3b53ec5aecde08d9869581d381f` (`MASHED_DDKOBJ=1`, Build OK). `ddkobj.py check` not run: evidence-only row, no unit changed |
| root | `D:\MashedWT\KNOBON-BOOTS-G5_root` (robocopy of `original\`, plus `original\mashed_image.bin` SHA-256 `81cc77758ffe93ac0b681eb1fed3d63912d25e7d059503af6d4f871dea9afcd8`, copied from the earlier race-bind row's root) |
| wrapper | test-slot outside, `mashed_lock.py run --label KNOBON-BOOTS-G5` inside; `mashed_lock.py status` showed free before every build and boot set |
| harnesses | frontend boots: `verify/porttop_20261108/knob_boot.py` via `run_boots.py`; replay arms: `verify/savelink_20261023/run_arms.py` (`RO_<stack>-pr1`, `A4_RO_CKPT=1`) via `run_arms_wrapped.py` |

Run history, because it changes what the numbers mean: replay arms on the VS2022 exe fail with `0xC0000005` at about 6 s even for the knob-off
`ROLOADRWC` baseline (data dir `arms_vs`), and the object build without `mashed_image.bin` does the same (`arms_ddk_noimg`). Only the object build
with the staged image reproduces the earlier row's stop (exit code 4944688 = 0x004B7330, `RO_ROLOADRWC-pr1`, 42 s). All arm numbers below come
from that last set (`arms/`). The two failed sets are not cited as results.

## 2. The three knobs on the replay arm (object build + image), exit mode

| arm | knob env added to `ROLOADRWC` | exit code | stop (address, class) | bound bodies executed |
|---|---|---|---|---|
| `RO_ROLOADRWC-pr1` (no knob) | none | 4944688 = 0x004B7330 | **0x004B7330**, `third-party-library[lua-4.0]`, caller 0x0047b860, return 0026D415, call 1 (log line 457, "race-load stop (PORT-OT-RACE)") | histogram below |
| `RO_G5D110-pr1` | `MASHED_OT_RL_D110=1` | 4944688 | same 0x004B7330, call 1 | same set, same counts |
| `RO_G5TT-pr1` | `MASHED_OT_TITLETICK=1` | 4944688 | same 0x004B7330, call 1 | same set, same counts |
| `RO_G5RF-pr1` | `MASHED_OT_RACEFRAME=1` (with `MASHED_RWC_FRAME=1`) | 4944688 | same 0x004B7330, call 1 | same set, same counts |

Bodies that logged, lines per rva (`hist.py` over the arm logs; identical on all four arms): 0x00402750 1, 0x0040cda0 4, 0x0040d270 1 (entered, call 1),
0x004264d0 2, 0x00426e10 1 (matched course load, track 30, entered before the stop), 0x00427ca0 1, 0x00428390 1, 0x00452f70 1, 0x004548e0 1,
0x00454e70 1, 0x004576f0 1, 0x004583a0 1, 0x004587a0 1, 0x00458c70 2, 0x0045a6b0 1, 0x0045b830 1, 0x0045bae0 1, 0x0048e820 1, 0x0048ff20 1,
0x00492270 1, 0x00492370 1, 0x004924f0 1, 0x00492d30 3, 0x00492e90 3, 0x00493540 1, 0x00493600 1, 0x00493710 1, 0x004b7330 1 (the stop).
28 distinct rvas, 37 lines. These are log-line counts: 0x00492d30 and 0x00492e90 log only at frames 0, 120, 240 (frame log gate in
`PortTop_00492370.cpp`) and run every frame up to the warp at tick 300, so 3 is the number of logged lines, not of calls.

Why the two knobs do not run: in `re/match/top07/0040d270.c` 0x0040d110 is called at lines 107 and 115 (PREREG cp2), after the course load path that reaches
"Searching piz for COURSE.LUA" and the 0x004B7330 stop in this arm. No `rva 0x0040d110` line, no `PortTop_Bound_0040d110` line and none of its callees
0x0040cea0 / 0x0041a8d0 / 0x00420420 appear in any arm log (grep over all `arms/RO_*-pr1.log`). 0x00429290 is only the pointer 0x00402750 passes to 0x004b6560
(`[PIZ_FS] 0x004b6560 callback 001DC210 arg 0 recorded (not called by the exe reader)`, log line 143; `PortTop_00402750_Pieces.cpp:231-235` says the same, "never calls it,
so neither path runs on this tree"); no line shows a call. Running either needs the Lua stop 0x004B7330 passed first (130 race-load functions, 88 C2 with no source,
earlier row section 3).

## 3. Count mode (`MASHED_OT_RL_STOPS=count`), same three knobs

`RO_G5D110C-pr1`, `RO_G5TTC-pr1`, `RO_G5RFC-pr1` each give the same three stops, call 1 each, then **exit `0xC0000005`** at about 6 s:
0x004B7330 (return 0026D415, caller 0x0047b860, Lua), 0x004C0510 (return 0026D420, caller 0x0047b860, `third-party-library[lua-4.0]`), 0x0047B980 (return 0005A9EF,
caller 0x00440bc0, RegisterScriptCommand), then the fault. This is the earlier row's cp3 count-arm result and its [UNCERTAIN] fault (register line
"PORT-OT-RACE-BIND-HANDLER (count-arm fault)"), unchanged by the knobs. Fault address not captured here. 0x0040d110, 0x0040cea0 and 0x00429290 do not appear before it.

## 4. Race-frame boots with and without `MASHED_RWC_FRAME`

Replay base on every row: `MASHED_ORIG_TOP=1 MASHED_DETERMINISTIC=1 MASHED_OT_RACELOAD=1 MASHED_PORTSTATE_IMAGE=1 MASHED_REPLAY=...` (`run_arms.py` `RO_` recipe).

| arm | env added | result |
|---|---|---|
| `RO_G5RF-pr1` WITH | `MASHED_OT_RACEFRAME=1 MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1 MASHED_RWC_FRAME_N=100000` | 0x004B7330 stop (section 2). Log lines "frame 0 / 120 / 240 rendered by 0x00492e90 (matched, RW core) = 1, phase 1". `0x004111c0 calls 0` |
| `RO_G5RFN-pr1` WITHOUT | the same with `-MASHED_RWC_FRAME -MASHED_RWC_FRAME_N` (`MASHED_RWC_CORE=1` stays) | exit **0xFFFFFFFF** at about 0 s, `WinMain 0x00492370 returned -1 after 0 frames`. Log: "bring-up probe done ... returning 0 from RwEngineStart so 0x00493710 runs its failure path (the rest of the top-down path is not on the RW core yet)". No bound body past boot, 0x00492e90 never runs, no race stop |
| `RO_G5RFP-pr1` no core | `ROLOAD` + `MASHED_OT_RACEFRAME=1`, neither RWC variable | exit `0xC0000005` at 5 s, no "rendered by 0x00492e90" line (hand RenderFrame stand-in), 23 distinct rvas, no race-load stop line, `0x004111c0 calls 0` at frames 0, 120, 240 |
| count twins `RO_G5RFC-pr1` / `RO_G5RFNC-pr1` | plus `MASHED_OT_RL_STOPS=count` | RFC: the three stops of section 3, then `0xC0000005`. RFNC: `0xFFFFFFFF` at frame 0, same as RFN |

Which one the race-frame replay arm uses: `verify/savelink_20261023/stacks/ROLOADRWC.txt:4-6` sets `MASHED_RWC_CORE=1`, `MASHED_RWC_FRAME=1`, `MASHED_RWC_FRAME_N=100000`
(its header: "the ROLOAD replay arm on the RW core frame route"). It is the stack of the earlier row's `RO_ROLOADRWC-pr1` / `RO_ROLOADRWCC-pr1`
(`verify/racebind_20261121/RESULT_PORT-OT-RACE-BIND-HANDLER.md` section 5). `stacks/ROLOAD.txt` and `ROIMG.txt` set neither. The PORT-TOP-06 knob-boot recipe
(`verify/porttop06_20261121/tools/knob_boots.py:10`, `BASE`) sets neither. **For GATERANK-ALLKNOBS: on `ROLOADRWC`, 0x00492e90 is the RW core body
`RwCore_00492e90` (`Match/PortTop_00492370.cpp:312-315`, `RwCore_FrameRoute()` true), measured by the "rendered by 0x00492e90 (matched, RW core)" lines. On `ROLOAD` and the
knob-boot recipe it is the hand RenderFrame stand-in (`PortTop_00492370.cpp:322-328`).** Counting it as bound holds for `ROLOADRWC` only. `MASHED_RWC_FRAME` cannot simply be
dropped from that arm either: RWC_CORE without FRAME aborts at frame 0 (`RO_G5RFN`).

## 5. Frontend-only boots (PORT-TOP-06 recipe, VS2022 exe, 3000 frames, no replay), first set of eight

`MASHED_ORIG_TOP=1 MASHED_OT_RACELOAD=1 MASHED_DET_FRAMES=3000` plus the knob. `WinMain 0x00492370 returned 0 after 3000 frames`, rc 0, for `rl_d110` (55 s), `rl_d110_count`,
`titletick` (27 s), `titletick_count`, `raceframe_rwcframe` (RACEFRAME + CORE + FRAME, 61 s) and `raceframe_rwcframe_count`. All stay in phase 1 with `0x004111c0 calls 0`
(frames up to 1800 logged), and 0x0040d270 is never entered, so the three knobs execute nothing there either. `raceframe_norwcframe` and its count twin (RACEFRAME + CORE, no FRAME):
rc 0xFFFFFFFF in 0.7 s, `WinMain returned -1 after 0 frames`, the same abort as `RO_G5RFN`. Logs: `D:/MashedWT/KNOBON-BOOTS-G5_data/<label>.log`, not committed.

## 6. Checks run, unfinished scope, evidence

Checks run: VS2022 build and object build (both `=== Build OK ===`); `mashed_lock.py status` free before each build and boot set; 8 frontend boots (section 5); replay arm sets: VS2022 exe
(10 arms, all `0xC0000005` or `0xFFFFFFFF`), object build without image (9 arms, same), object build with image (10 arms: 9 in the first pass plus `RO_G5RFP`, sections 2 to 4).
Not run (evidence-only row): `ddkobj.py check`, binder selftest, G-CTRL, B, 12 captures, menu walk.

Unfinished scope: (a) the `0xC0000005` of the count-mode arms, address not captured ([UNCERTAIN], already in `re/orchestrator/UNCERTAIN_REGISTER.md`); (b) the same fault on the VS2022 exe
and on the object build without `mashed_image.bin`, cause not investigated; (c) making 0x0040d110, 0x00429290 and 0x004111c0 execute needs the Lua stop 0x004B7330 passed or stood in,
the parent's Lua decision (earlier row, section 6); (d) the exit-mode stop is call 1 on every arm, no later stops measured.

Committed: this RESULT, `run_boots.py`, `run_arms_wrapped.py`, `arm_log_extract.txt` (stop and entry lines of the arms, RUNLOG heads), `raw_sha256_KNOBON-BOOTS-G5.txt`, nine stacks
`verify/savelink_20261023/stacks/G5{D110,D110C,TT,TTC,RF,RFC,RFN,RFNC,RFP}.txt`. Raw (not committed): `D:/MashedWT/KNOBON-BOOTS-G5_data` (`arms/RO_*-pr1.log` and trace dirs, `arms_vs`,
`arms_ddk_noimg`, frontend `*.log`, `*.out`, build logs, both exes). SHA-256 of the cited raw files in `raw_sha256_KNOBON-BOOTS-G5.txt`.

Commands (worktree root): `G5_EXE=mashed_re_ddk.exe py -3.12 verify/knobonboots_20261121/run_arms_wrapped.py RO_ROLOADRWC-pr1 RO_G5D110-pr1 RO_G5D110C-pr1 RO_G5TT-pr1 RO_G5TTC-pr1
RO_G5RF-pr1 RO_G5RFC-pr1 RO_G5RFN-pr1 RO_G5RFNC-pr1 RO_G5RFP-pr1`; `py -3.12 verify/knobonboots_20261121/run_boots.py`.
