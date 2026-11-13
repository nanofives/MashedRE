# RESULT RO-EXE-REPLAY-KNOB (2026-11-13)

Row: replay oracle 7, `MASHED_REPLAY=<replay.json>`, default OFF. Kind: top-down / harness exe knob. Worker session
cmwe85ds387lqsc1bzgwjkgsv (claude3, Opus 5.5 medium), parent cmwe83w9a87acsc1b9sv0okq7. Branch `orch/RO-EXE-REPLAY-KNOB`
from `orch/integration` `a616fcc3b`. PREREG `PREREG_RO-EXE-REPLAY-KNOB.md` committed alone at `ba40c0e7d`, before any arm.

## Verdict

**GREEN.** **Self-test: `[REPLAY] self-test PASS (0 of 666 checks failed)` on the VS2022 build (vs_rep_a, vs_rep_b) and the
object build (ddk_rep).** The OT_TIMER self-test under the knob is `PASS (0 of 5990 checks failed)` on both builds, after
the model fix in "Run history".

- Default OFF: vs_off, vs_top, ddk_top exit rc 0 after 600 frames with 0 `[REPLAY]` lines and `MASHED_OT_TIMER` off.
- Object build: `ddkobj.py check` IDENTICAL for every unit this row adds or touches (table below). Totals IDENTICAL
  811/899, DIFFERENT 78, not-in-map 10, units linked 856/866, refusals 9 (wrapper-redefines-globals). None of the 78
  DIFFERENT and none of the 9 refusals is a unit of this row.
- Determinism: the `[REPLAY] tick` lines (tick, Q, `DAT_007f1030`, phase) of vs_rep_a, vs_rep_b and ddk_rep are
  byte-identical (md5 `7c06da27...` for all three). The VS2022 and object builds read the same virtual clock.
- Warp ran at tick 300 (phase 1 before, 1 -> 2). The race anchor was reached at the next wrapped clock read, in the
  same tick (prediction 6 said "not reached": wrong, see "Warp and anchor").
- Gates: not run (top-down row, WORKER_RULES "Row kinds"). Every change is on the `MASHED_ORIG_TOP` path. The one edit
  outside the new TUs that the default build compiles is the exe branch of `MatchS15_00493480.cpp`, reached only under
  `MASHED_OT_TIMER`, which needs `MASHED_ORIG_TOP`.

## What the knob does (`mashedmod/src/mashed_re/Match/PortTop_Replay.cpp`)

Each piece is the exe counterpart of the original-side rule in `re/frida/js/detorig.js`, boot mode:

| source | exe injection | original-side counterpart |
|---|---|---|
| clock | the exe IAT slot of QueryPerformanceCounter -> `Replay_QPC`. Virtual Q only for a main-thread call whose return address is the one `FF 15` call site in the matched 0x004950b0 body (found at init: VS2022 `+0x16`, object build `+0x18`). Every exe caller of 0x004950b0 is a matched unit (0x00428a30, 0x004926c0, 0x00492d30, 0x00493390, 0x00495110). Before the first tick Q += `boot_qstep` per call, after it only at the tick | `qpcStub` + `cfg.bootQstep`, CModule zeroes the step at tick 1 |
| tick | 0x00493480 calls 0x00493390 through `PortTop_Replay_00493390`: Q += qtick, T += ttick, tick += 1, then the matched body `PortTop_Bound_00493390` | CModule `onEnter` on FUN_00493390 |
| warp | at tick `warp.after_ticks`, inside the first wrapped clock read of that tick after the value is written. Refused unless `DAT_00771968 == 1`. Selection globals, `CarSlotStateSet` 0x0040e480 x4 (matched exe body), RNG pins, clock rebase to `q0 + warp.clock * qtick`, timer pin, normalisation PIN rows, `DAT_00771968 := warp.phase` | `warp()`, `cfg.warpClock`, `pinTimer()`, `pinNorm('warp')`, `tickSchedule(warpTick)` |
| CRT rand | `srand(holdrand)` on the main thread at the warp and the anchor (UCRT per-thread state, same LCG as `_rand` 0x005c229b: `0x343fd` / `0x269ec3`) | `pinRngs()` writes `ptd+0x14` |
| RW ring RNG | `PortTop_Replay_00534870`: the matched body (`Match/PortTop_00534870.cpp` = `re/match/rwl03/00534870.c`) on a ring of its own. For one call it sets the plugin offset `0x007dc578` so that `*(0x007d3ff8) + *(0x007dc578)` names the private struct, then restores the word. Seeded by the FUN_00534990 rule (`0x9a319039` at 0x00534a09, LCG `0x41c64e6d` / `0x3039`, rp = ring + 3, wp = ring, `0x136` draws at 0x00534a46) | `pinRngs()` on the original's ring |
| race anchor | first wrapped clock read after the warp that sees `DAT_00771968 == 3`: pins again, normalisation at the anchor, rows at anchor + 0 applied | `watch()` on every main-thread clock call |
| input | `PortTop_Replay_00497310(player, control)`: the matched body (`Match/PortTop_00497310.cpp` = `re/match/s16/00497310.c`), and once the warp ran, the script's value for player 0. Rows keyed by tick after the anchor (DESIGN section 2) | CModule `repl` over FUN_00497310, rows keyed by N |
| raw poll | the 0x004967e0 stand-in skips the hand keyboard poll (`Replay_RawPollNoDevice`): "no device" | DESIGN section 3, hook 0x00495fe0 |
| normalisation | `re/frida/warp_normalize.tsv` PIN rows (299 dwords) at the warp and the anchor, `normalize.at` = both | `--normalize`, `pinNorm` |

The knob turns `MASHED_OT_TIMER` on (log: `(implied by MASHED_REPLAY)`). `replay.json` is the DESIGN section 2 format
(`format: mashed-replay-1`), plus `clock.boot_qstep` / `boot_step`, `warp.clock`, `warp.phase` (`null` = no phase
write), `normalize` and `raw_poll`. Self-test scripts: `replay_selftest.json`, `replay_selftest_nophase.json`.

## Self-test (what PASS covers)

Init (66 checks, 0 failed on all three knob arms):
- RW ring port against an independent model written from the source, 0x136 + 512 draws: all outputs equal, cursors equal,
  `0x007dc578` restored after the draws.
- `srand(0x12345678)` then 8 `rand()` = the CRT LCG model. The CRT state is reset to 1 afterwards, so the boot is not seeded.
- 0x00497310: matched body = model on every non-joystick record (4 players x 13 actions), the dispatcher returns the
  script for player 0 only while active and the body otherwise.

Runtime:
- per tick (600 of 600, 0 failed): `DAT_007f1030` after the matched 0x00493390 = `(Q_model * 3,000,000) / QPF` (QPF from
  `0x00771e70` = 10,000,000), with Q_model computed from q0, the boot read count, the tick count and the warp rebase,
  independently of the wrapper's own Q.
- ticks = 0x00493480 calls (600 = 600).

## Run history (three runs, all under test-slot -> `mashed_lock.py run --label RO-EXE-REPLAY-KNOB`)

| run | builds | finding |
|---|---|---|
| 1 | first VS2022 + object build | REPLAY self-test PASS on both. Input rows applied 0: the anchor is set after its tick has started, so a row at anchor + 0 never fired. detorig.js applies dn = 0 at the anchor. Fixed: rows at anchor + 0 apply in the anchor itself |
| 2 | after the input fix | REPLAY PASS, input rows applied 1. The RO-BIND-TIMER self-test (`[OT_TIMER]`, not part of the PREREG) FAILED under the knob: VS2022 4 of 6000, object build 212 of 6000 |
| 3 | after the OT_TIMER model fix | all arms below. REPLAY PASS and OT_TIMER PASS on both builds |

The two OT_TIMER failures were in its model, not in a body (`Match/PortTop_00492370.cpp`):
- VS2022, 4 failures, all at call 300 (`DAT_007f0ffc`, `DAT_00771984`, `DAT_007f0ff0`, `DAT_007f102c`): the replay warp ran
  inside that call's 0x00493390 clock read, the same point as on the original, and its timer and normalisation pins
  changed the globals the model had read before the call. Fix: the model skips the call in which the warp ran (logged,
  `calls skipped for the replay warp 1`).
- Object build, 212 failures, all `DAT_007f1010`, each 1 ULP: the model computed `(float)now * 3.3333333e-07f` the
  VS2022 way (cvtsi2ss of `now`, a rounding once `now >= 2^24`, then mulss). The DDK body, which has the original's bytes,
  loads `now` exactly and rounds the product once at the store. Fix: the check accepts either form and counts the
  single-rounding hits: VS2022 0 (vs_timer, vs_rep_a), object build 104 on the real clock (ddk_timer) and 207 on the
  virtual clock (ddk_rep). [UNCERTAIN] why RO-BIND-TIMER's ddk_on reported 0 failures under the old model on the real
  clock. ddk_timer here, on the real clock, has 104 calls where the two forms differ and the body matches the
  single-rounding form. That earlier exe was not re-run, so the difference is not explained.

## Arms (run 3, `MASHED_DET_FRAMES=600`, root `D:/MashedWT/RO-EXE-REPLAY-KNOB_root`)

| arm | exe | knobs | result |
|---|---|---|---|
| vs_off | VS2022 | none | rc 0, 7.2 s, 0 REPLAY lines |
| vs_top | VS2022 | ORIG_TOP | rc 0, 600 frames, 0 REPLAY lines, OT_TIMER off |
| vs_timer | VS2022 | ORIG_TOP + OT_TIMER | rc 0, OT_TIMER PASS 0 of 6000 (RO-BIND-TIMER's arm, rerun after the model change) |
| vs_rep_a | VS2022 | ORIG_TOP + REPLAY | rc 0, **REPLAY PASS 0 of 666**, OT_TIMER PASS 0 of 5990 |
| vs_rep_b | VS2022 | ORIG_TOP + REPLAY | rc 0, **REPLAY PASS 0 of 666**, OT_TIMER PASS 0 of 5990 |
| ddk_top | object build | ORIG_TOP | rc 0, 600 frames, 0 REPLAY lines |
| ddk_timer | object build | ORIG_TOP + OT_TIMER | rc 0, OT_TIMER PASS 0 of 6000 (104 single-rounding) |
| ddk_rep | object build | ORIG_TOP + REPLAY | rc 0, **REPLAY PASS 0 of 666**, OT_TIMER PASS 0 of 5990 (207 single-rounding) |

vs_rep_nophase was not run: the PREREG made it conditional on vs_rep_a stopping after the warp, and it did not stop.

Knob-arm summary (identical in all three): ticks 600, wrapped QPC reads 1,800 (4 before the first tick, 1,796 after it;
which matched readers make them was not broken down), passed through 0, warp at tick 300, anchor at tick 300,
normalisation changed 3 dwords at the warp and 1 at the anchor, input rows applied 1, raw polls skipped 600, RW draws 620.

Tick lines (all three knob arms): tick 1 Q `0x4003756B` `DAT_007f1030` 322190547 phase 1; tick 240 Q `0x426344B0` phase 1;
tick 360 Q `0x43938778` `DAT_007f1030` 340122583 phase 3; tick 600 Q `0x45F5E1C8` `DAT_007f1030` 352122607 phase 3.

## Warp and anchor (what PORT-OT-RACE leaves open)

The warp found the menu (phase 1) at tick 300 and wrote phase 2. The matched 0x004929d0 moved 2 -> 3 in the next
iteration (`frame 300 after 0x004929d0: DAT_00771968 2 -> 3`), and the anchor's check in the next wrapped read
(0x00492d30) saw 3. **There is no track load on the exe's ORIG_TOP path yet.** The original takes 113 to 215 clock calls
in phase 2 (LOADHOLD RESULT). So "anchor reached" here means only that the phase word became 3, not that a race runs.
Phase 3 then ran to frame 600 with rc 0.

Blocked on PORT-OT-RACE, built here but unreached from game code (each exercised by the self-test only):
- the scripted input: the caller of 0x00497310 is the cooked input 0x00496530, which has no exe body. The 0x004967e0
  poll on the exe is the hand keyboard stand-in, so the script reaches no game state yet.
- the RW ring: no exe body binds 0x00534870's game-side wrappers 0x00472650 / 0x00472690 / 0x004b44f0 / 0x004b4510.
- CRT `rand`: its callers 0x004c03b0 (Lua `random`) and 0x00564310 have no exe body, so `srand` pins a state nothing reads.
- timeGetTime / GetTickCount: readers 0x005ab040 / 0x004a1570 have no exe body. No slot is wrapped, T is kept and reported.
- the per-tick hash and `ticks.bin`: row RO-EXE-TICK-TRACE.

Differences from the original side to carry into RO-EXE-TICK-TRACE / RO-ORACLE-GATE:
- the anchor check runs on wrapped matched-unit clock reads only. The original checks on every main-thread clock call,
  timeGetTime and GetTickCount included.
- the audio-thread suspension (`audioOff`) has no exe counterpart: the exe's audio is the hand path's.

## Builds
- VS2022: Build OK (`D:/MashedWT/RO-EXE-REPLAY-KNOB_build_vs3.log`), rva-lint NEW=0. Run 1 had one NEW finding on
  a self-test model whose comment named 0x004950b0, fixed by rewording the comment (no allowlist entry).
- Object build (`MASHED_DDKOBJ=1`): Build OK (`D:/MashedWT/RO-EXE-REPLAY-KNOB_build_ddk3.log`). `bytecheck_rows.tsv`:

| rva | exe symbol | size | diff_bytes | result |
|---|---|---|---|---|
| 0x00497310 | `_PortTop_Bound_00497310` (new) | 320 | 0 | IDENTICAL |
| 0x00534870 | `_PortTop_Bound_00534870` (new) | 176 | 0 | IDENTICAL |
| 0x004950b0 | `_QpcTimeScaledTo3Mhz` | 96 | 0 | IDENTICAL |
| 0x00493390 | `_PortTop_Bound_00493390` | 240 | 0 | IDENTICAL |
| 0x00493480 | `_FpsDiscretise` (callee rebound to the dispatcher) | 192 | 0 | IDENTICAL |
| 0x0040e480 | `_CarSlotStateSet` (called by the warp) | 32 | 0 | IDENTICAL |

## Commits
- `ba40c0e7d` PREREG (alone).
- this commit: the knob (`Match/PortTop_Replay.cpp`, `PortTop_00497310.cpp`, `PortTop_00534870.cpp`), the dispatcher edits
  (`MatchS15_00493480.cpp`, `PortTop_00492370.cpp`), `exe_sources.rsp`, the OT_TIMER model fix, the evidence below.

No tracker edit, no `ROADMAP.md` edit, no default flip, no `.asi` redeploy, no change to `original/`.

## Evidence
Committed (this folder): `PREREG_RO-EXE-REPLAY-KNOB.md`, this RESULT, `replay_selftest.json`,
`replay_selftest_nophase.json`, `replay_arms.sh`, `replay_lines_run3.txt` (the `[REPLAY]`, OT_TIMER summary and phase
lines of every run-3 arm), `bytecheck_rows.tsv`, `raw_sha256.txt` (SHA-256 of every raw log of runs 1 to 3, the arms
output, both exes and both build logs).

Raw (not committed): `D:/MashedWT/RO-EXE-REPLAY-KNOB_data/` (run 3 logs at the top, `run1/`, `run2/`), the build logs
`D:/MashedWT/RO-EXE-REPLAY-KNOB_build_{vs,ddk}{,2,3}.log`.
