# RESULT PORT-TOP-4669B0 (2026-11-12, run 41, section D4)

Verdict: GREEN. Top-down row (knob path only). Under the new sub-knob `MASHED_OT_AUDIOINIT=1` (default OFF) the exe runs
the byte-matched 0x004669b0 at 0x00402750's call, so DAT_006904e8 = 1 and the three audio-tick bodies (0x0045d3f0,
0x00466b50, 0x0045d430) pass their gate. Not a C4 claim: 0x004669b0 already carries C4 by route match (`c4=match`,
PORT-TOP-MATCH-01), and this row changes nothing on the RE axis.

Self-test result: **`[OT_AUDIOINIT] self-test PASS (0 of 12 checks failed)`** on all four arms with the knob on (VS2022 and
object build), and **`[OT_TIMER] self-test PASS`** on the four arms that reach 600 frames (0 of 8413 with AUDIOTICK and
the knob off, 0 of 6000 with the knob on and AUDIOTICK off).

## Body
- No new body was needed. `re/match/topm01/004669b0.c` is 100% (32/32), `match.py --bytes` BYTE-IDENTICAL 160 B, relocheck
  OK checked=24, src sha256 cf57913d1c6c156d, flags `/O2 /GS /Ob1` (hooks.csv row 0x004669b0, C4 `c4=match`,
  `verify/porttopm01_20261108/RESULT_PORT-TOP-MATCH-01.md`). So no equiv.py run, no ADMITTED.tsv or PROVISIONAL.tsv row.
- The s10 copy (`re/match/s10/004669b0.c`, 84.4%) that RO-AUDIOTICK-BODIES cited is not used.

## What changed
- New `mashedmod/src/mashed_re/Match/PortTop_004669b0.cpp` (exe only, `// RVA-LINT: ORIGTOP-ONLY`, `#ifdef MASHED_STANDALONE`,
  added to `exe_sources.rsp`). It includes the topm01 source as `PortTop_Bound_004669b0` and binds its callees:

| callee | binding | why |
|---|---|---|
| 0x004625b0 | `AudioCharacterBankPathsBuild` (Audio/AudioCharacterBankPaths.cpp) | existing exe body, C4 frida |
| 0x00429290 (3 calls, string args 0x006036e8 / 0x00603768 / 0x006037e8) | `PortTop_AInit_00429290`, logs first call | same choice as 0x00402750's `PortTop_StandIn04E_00429290` |
| 0x004627f0 (sound engine HW init, no exe body) | `PortTop_AInit_004627f0` -> hand piece 3 `PortTop_Piece_004669b0` (HandTop_InitAudio) | PORT-TOP-04E rule: a callee doing a job the hand init does binds to the piece doing it |
| 0x0045d460 (permdict.rws loader, no exe body) | `PortTop_AInit_0045d460`, logs first call | piece 3 already loaded the bank |

- Dispatcher `PortTop_AudioInit_004669b0`: knob off, it calls piece 3 exactly as before (byte-for-byte the old path).
  Knob on, it runs the matched body, then checks the return value (1) and the eleven globals the source writes:
  DAT_006904e8 = 1, DAT_0068fccc = 1000, DAT_0068fcbc = 0x3f800000, DAT_006900d0 = 0, DAT_008aa300[0..3] = 0,
  DAT_00690490[0..2] = 0 (12 checks).
- `Match/PortTop_00402750.cpp`: `#define FUN_004669b0 PortTop_AudioInit_004669b0` (was `PortTop_Piece_004669b0`).
- `Match/PortTop_00492370.cpp`: the `[OT_TIMER]` gate check now expects DAT_006904e8 == `PortTop_AudioInit_Ran()`
  (0 with the knob off, 1 once the matched body ran) instead of a fixed 0.
- `verify/porttop_20261108/BINDINGS.tsv`: 0x004669b0 stand-in -> matched; new level-3 rows 0x004625b0 (matched),
  0x004627f0 and 0x0045d460 (stand-in). 0x00429290 keeps its existing row (RVAs are unique there).

## Builds
- VS2022 (`MASHED_NO_DEPLOY=1`): Build OK. rva-lint OK, 104 known, NEW=0. Log `D:/MashedWT/PORT-TOP-4669B0_build_vs.log`.
- Object build (`MASHED_DDKOBJ=1`): Build OK, link rc=0. Log `D:/MashedWT/PORT-TOP-4669B0_build_ddk.log`.
- `py.exe -3.12 re/tools/ddkobj/ddkobj.py check`: IDENTICAL 813/901 (812/900 before this row), DIFFERENT 78 (the same
  78 as RO-AUDIOTICK-BODIES run 3, none of them this row's), not-in-map 10, units linked 858/868, 9 refusals
  (`wrapper-redefines-globals`) plus 1 `skip:provisional` (0x0043dfd0), none this row's. Log `D:/MashedWT/PORT-TOP-4669B0_ddkcheck.log`.
- BYTECHECK.tsv rows for this path:

| rva | exe symbol | exe VA | size | verdict |
|---|---|---|---|---|
| 0x004669b0 | `_PortTop_Bound_004669b0` | 0x00018c80 | 160 | IDENTICAL, diff_bytes 0 |
| 0x00402750 | `_Orig_00402750` | 0x00014d90 | 752 | IDENTICAL, diff_bytes 0 |

## Knob boots
`test-slot` outside, `mashed_lock.py run --label PORT-TOP-4669B0` inside (status free before both builds; the run queued
behind RO-EXE-REPLAY-KNOB's window and the memory gate). Script `verify/portorigtop_20261112/audioinit_arms.sh`,
output `verify/portorigtop_20261112/arms_4669b0.out`. Root `D:/MashedWT/PORT-TOP-4669B0_root` (robocopy of `original\`),
`MASHED_DET_FRAMES=600`, all arms `MASHED_ORIG_TOP=1 MASHED_OT_TIMER=1`.

| arm | exe | extra knobs | result | log sha256 (16) |
|---|---|---|---|---|
| vs_audio | VS2022 | AUDIOTICK | rc 0, 600 frames, OT_TIMER PASS 0/8413, audio branch 600, piece 3 ran | b5acbde65375e024 |
| ddk_audio | object | AUDIOTICK | rc 0, 600 frames, OT_TIMER PASS 0/8413, audio branch 600, x87-only 102 | 42b3b4bbfcfe3331 |
| vs_init | VS2022 | AUDIOINIT | rc 0, 600 frames, OT_AUDIOINIT PASS 0/12, OT_TIMER PASS 0/6000 | ab262d348578309b |
| ddk_init | object | AUDIOINIT | rc 0, 600 frames, OT_AUDIOINIT PASS 0/12, OT_TIMER PASS 0/6000, x87-only 104 | e1b229cd1a899d6f |
| vs_init_audio | VS2022 | AUDIOINIT + AUDIOTICK | OT_AUDIOINIT PASS 0/12, then STOP rc 0x00550BE0 after 1.9 s | fa771eee4aeae0d7 |
| ddk_init_audio | object | AUDIOINIT + AUDIOTICK | OT_AUDIOINIT PASS 0/12, then STOP rc 0x00550BE0 after 2.2 s | c32ca77619eb23c5 |

Logs: `D:/MashedWT/PORT-TOP-4669B0_data/<arm>.log`.

## The audio bodies now run past their gate
With both knobs, on frame 0's first audio-branch call, the matched 0x0045d3f0 takes its
`(DAT_006904e8 != 0) && (DAT_006904ec == 0)` branch (re/match/s10/0045d3f0.c) and calls 0x00550be0, which is bound to
the named stop: `[OT_TIMER] STOP: the matched 0x0045D3F0 called original 0x00550BE0 (DAT_006904e8 = 1, DAT_006904ec = 0)`,
exit code 0x00550BE0, on both builds (return address 0017D01A VS2022, 00018C5F object build). Before this row the same
body returned at once with DAT_006904e8 = 0 (RO-AUDIOTICK-BODIES, 600 audio-branch calls, no stop). The stop's summary
line reads "0 of 0 checks" because the stop fires inside the first 0x004926c0 call, before the counters move.

So the next binding on this path is 0x00550be0 (callee of 0x0045d3f0), then 0x005a60b0, then the 52 stopped callees of
0x00466b50 and 0x005a60e0 of 0x0045d430 (BINDINGS.tsv level 3). Until those bind, `MASHED_OT_AUDIOINIT=1` and
`MASHED_OT_AUDIOTICK=1` cannot both be on for a full run. Either one alone runs 600 frames clean.

## Not done / notes
- No G-CTRL, no B, no 12 captures (top-down row, WORKER_RULES "Row kinds"). Nothing on the default path changed: the new
  TU compiles only under `MASHED_STANDALONE`, its caller is reached only under `MASHED_ORIG_TOP=1`, and with
  `MASHED_OT_AUDIOINIT` unset the dispatcher calls piece 3 as before.
- Menu walk not run (no menu code changed).
- No new librw binding (L-124). No unmatched body (L-126). No tracker edit (hooks.csv already has 0x004669b0 at C4 c4=match).
- The comments in Match/PortTop_0045d3f0.cpp / PortTop_0045d430.cpp / PortTop_00466b50.cpp and the stop in
  PortTop_00492370.cpp still say the exe never sets DAT_006904e8. That holds with `MASHED_OT_AUDIOINIT` unset, the default.
- Turning `MASHED_OT_AUDIOINIT` on by default on the knob path is a P1-class flip (L-127) and needs the OFF/ON/OFF/ON
  capture comparison, not run here.
