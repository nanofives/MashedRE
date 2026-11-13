# RESULT RO-EXE-TICK-TRACE (2026-11-13)

Row: replay oracle 8, `MASHED_REPLAY_TRACE=<dir>`, default OFF, read only when `MASHED_REPLAY` is on (so only on the
`MASHED_ORIG_TOP` path). Kind: race-path row on the oracle (top-down harness code). Worker session run 42 (claude2, Opus 5.5
medium), lead cmwejtp8k8scesc1b9ir2im1l. Branch `orch/RO-EXE-TICK-TRACE` from `orch/integration` `4e959adb1`.
PREREG `PREREG_RO-EXE-TICK-TRACE.md` committed alone at `6047c6ed4`, before any arm.

## Verdict

**GREEN.** **Self-test: `[TRACE] self-test PASS (0 of 33294 checks failed)` on all 17 trace arms, object build and VS2022
build, and `[REPLAY] self-test PASS (0 of 666 checks failed)` on every knob arm (both builds).** The offline check
`trace_verify.py` is OK on all 17 exe directories and on the original boot-mode reference `RO-MENU-REPLAY_data/r1`.

| PREREG prediction | result |
|---|---|
| 1 default OFF | holds. vs_off (no `MASHED_ORIG_TOP`, `MASHED_REPLAY_TRACE` set): rc 0 after 6.6 s, 0 `[TRACE]` / `[REPLAY]` lines, no directory created. RO_ROOFF-ddk (knob on, trace var removed): `[REPLAY]` PASS, 0 `[TRACE]` lines, no directory |
| 2 builds, lint, ddkobj | holds. Both builds "Build OK", rva-lint NEW=0. `ddkobj.py check`: IDENTICAL 1151/1181, DIFFERENT 11, not-in-map 19, units linked 1132/1399, refusals 4 (wrapper-redefines-globals). This row adds no matched unit. The knob's units stay IDENTICAL (table below) |
| 3 self-tests, 600 records, verifier | holds on every arm |
| 4 determinism | holds **with `MASHED_DETERMINISTIC=1`**, which is now part of the `RO_` arm kind: 6 of 6 pairs of 4 runs are IDENTICAL-FOR-N **600 of 600**, NEW blocks 0. Without it, runs split into two classes at tick 181 (below) |
| 5 negative control | holds. ddkl vs RONEG-ddk2 (+1 on `0x007f0ff0` at tick 400): IDENTICAL-FOR-N 399 of 600, NEW block `0x007f0c00` from record 399 (= tick 400), raw `0x007f0ff0` `0x1356` -> `0x1357` |
| 6 build difference (report) | ddkl vs vs2: first NEW block `0x007f1000` (frame timer block) at record 7, raw `0x007f1010` `42d6fe63` vs `42d6fe64` (1 ULP). This is the VS2022 double-rounding body the knob RESULT names (OT_TIMER FAIL 207 of 5990 on the VS2022 arms). The 3 constant blocks `0x005f9800`, `0x00771800`, `0x00773000` hold exe-image addresses (`0x001c61b0` vs `0x001b2de0`, 11 dwords): the two exes link differently |
| 7 format vs the original | holds. `detorig_ticks.py r1 RO_DEF-ddka --n-ok` runs without error (verdict recorded, not scored: IDENTICAL-FOR-N 17 of 600, first NEW block `0x00898c00` at record 17, 540 blocks differ from record 0, 507 raw words differ). Expected: the exe ORIG_TOP path has no race yet (PORT-OT-RACE), and r1 warps at tick 600, this script at 300. `detorig_report.py` reads exe checkpoint dumps too |
| 8 gates | not run (top-down row, everything runs under `MASHED_ORIG_TOP` + `MASHED_REPLAY`) |

## What was built

- `mashedmod/src/mashed_re/Match/PortTop_ReplayTrace.cpp` (new, exe only, ORIG_TOP only):
  - per tick, in the knob's `PortTop_Replay_00493390` after the virtual clock / tick advance and before the matched timer
    body (detorig.js CModule `onEnter` order): record `[tick, N, hash per block, raw words]` appended to `ticks.bin`.
    Image `0x005ea000..0x00915008`, 1024 B blocks (3245), FNV-1a over dwords, heap rule `[0x00995000, 0x30000000)` low 2 bits
    clear -> 0, the 7 raw ranges of `verify/romenu_20261112/run_arm.sh` (2304 words). Record = 5551 dwords, the same layout as
    the original reference r1 (`nblk` 3245, `recw` 5551). Hashing from tick 1 (detorig boot mode).
  - `ckptNN.data.bin` at the `MASHED_REPLAY_TRACE_CKPT` ticks, taken right after that tick's record, and k 100 inside the warp
    before its writes (detorig `menuCkpt`).
  - pages not committed-readable in the range are hashed / dumped as 0 and listed in `meta.json` (`unreadable`). Measured:
    0 of 812 pages, 0 map changes, on every arm.
  - `meta.json`: detorig.py's keys (`ticks` with `phases`, `ckpt`, `warp`, `race`, `boot`, `cfg`, `rc`) plus `side: "exe"`,
    `selftest`, `page_map_changes`. `race.n` is N at the anchor (0 and `reached: false` if not reached).
  - self-test: at each checkpoint tick an independent recompute of every block hash and raw word from the dump buffer must
    equal that tick's record (33294 checks per arm = 6 x (3245 + 2304)).
  - `MASHED_REPLAY_TRACE_NEGCTL=<tick>:<va>`, `_RAW`, `_BLOCK`, `_CAP` (default 6000).
- `Match/PortTop_Replay.cpp`: calls into the trace at init, tick, warp (before and after), anchor and summary. Nothing the
  knob does changed.
- `mashedmod/exe_sources.rsp`: the new TU.
- `verify/savelink_20261023/run_arms.py`: arm kind `RO_<tag>[-<rep>]` (the kind needs code, so it is a branch; no new
  `STACK[...]` line): `MASHED_ORIG_TOP=1`, `MASHED_DETERMINISTIC=1`, `MASHED_REPLAY` = `A4_REPLAY_JSON` (default
  `verify/replayoracle_20261113/replay_trace.json`), `MASHED_REPLAY_TRACE=<A4_OUT>/<arm>`, `MASHED_DET_FRAMES` = `A4_RO_FRAMES`
  (600), `MASHED_REPLAY_TRACE_CKPT` = `A4_RO_CKPT` (1,150,299,301,450,600), then the stack. `A4_PLAN=1` prints it.
- stacks (own files): `RONEG.txt` (negative control), `ROOFF.txt` (trace removed), `ROMPG.txt` (`MASHED_DET_MPG_PIN=0`),
  `RODET.txt` (`MASHED_DETERMINISTIC=1`, now the kind's default; kept so arms h..k reproduce under their names).
- `verify/replayoracle_20261113/replay_trace.json`: `replay_selftest.json` with the normalisation file at
  `.worktrees/orch-integration/re/frida/warp_normalize.tsv` (byte-identical to this branch's). **The knob's committed
  `replay_selftest.json` names `.worktrees/RO-EXE-REPLAY-KNOB/re/frida/warp_normalize.tsv`, a removed worktree.**
- `verify/replayoracle_20261113/trace_verify.py`, `trace_arms.sh`.

## The tick-181 split (found by the oracle, explained)

Without `MASHED_DETERMINISTIC=1`, 7 object-build runs fell into two classes: {ddka, ddkc, ROMPG-f, ROMPG-g} and
{ddkb, ddkd, ROMPG-e}. Between classes one block, `0x00898800`, differed at record 180 (tick 181) only. Non-pointer
dwords at the tick-181 dump (ddkc vs ddkd): 12 words in `0x00898a20..0x00898a8c`, the Im2D vertex scratch the hand-path
frontend writes (`Frontend/DrawQuadPrimitives.cpp` `kVBufBase`, `Frontend/BucketMixed_t3.cpp`, `D3d9Render/RwIm2DBridge.cpp`).
Class A's last quad there is x 513.96..574.04, y 40..120; class B's is x 80..560, y 80..320.
- `MASHED_DET_MPG_PIN=0` (frontend.mpg frozen on frame 0) did not remove it (ROMPG e vs f and e vs g split).
- `MASHED_DETERMINISTIC=1` removed it: 4 runs, 6 pairs, 600 of 600. In `exe_main.cpp:470-475` `DetTicks()` returns
  `GetTickCount()` unless that knob is on, and the comment at `exe_main.cpp:453-454` lists the splash / menu animation
  timers among its callers. The knob does not wrap `GetTickCount` (knob RESULT: no matched reader). So a hand-path menu
  timer on the wall clock picked a different last quad at tick 181.
- [UNCERTAIN] which `DetTicks` caller produced that quad: not traced. The evidence is the A/B on the knob only.
This is hand-path state, not race state, and it goes away once ORIG_TOP no longer draws the menu through the hand path.
The `RO_` kind now sets `MASHED_DETERMINISTIC=1` (harness env, not an exe default).

Two words differ between every pair of runs: `0x00772fb0` and `0x007e9584`, the same per-run value (`0x01750952` /
`0x028e0100`, ...). Both are `RULE:HWND` in `re/frida/warp_normalize.tsv:29-30` (window handle, OS-assigned). They differ
from record 0, so `detorig_ticks.py` lists them as constant blocks (`0x00772c00`, `0x007e9400`) and they do not end the count.

## Arms (root `D:/MashedWT/RO-EXE-TICK-TRACE_root`, every boot test-slot outside and `mashed_lock.py run --label RO-EXE-TICK-TRACE` inside)

| arm | exe | stack / env | trace self-test | REPLAY | OT_TIMER |
|---|---|---|---|---|---|
| RO_DEF-ddka, -ddkb, -ddkc, -ddkd | object | DEF (before the kind set DETERMINISTIC) | PASS 0/33294 | PASS 0/666 | PASS 0/5990 |
| RO_ROMPG-e, -f, -g | object | MPG pin | PASS | PASS | PASS |
| RO_RODET-h, -i, -j, -k | object | DETERMINISTIC | PASS | PASS | PASS |
| RO_DEF-ddkl | object | DEF (kind with DETERMINISTIC) | PASS | PASS | PASS |
| RO_RONEG-ddk (no DET), RO_RONEG-ddk2 | object | negctl 400:0x007f0ff0 | PASS | PASS | FAIL 1/5990 (the control's own +1 on `DAT_007f0ff0`) |
| RO_DEF-vs (no DET), RO_DEF-vs2, -vs3 | VS2022 | DEF | PASS | PASS | FAIL 207/5990 (double-rounding body, as the knob RESULT) |
| RO_ROOFF-ddk | object | trace removed | no line, no files | PASS | PASS |
| vs_off | VS2022 | no knob, `MASHED_REPLAY_TRACE` set | no line, no files | no line | no line |

Every arm rc 0, self-exited, 600 records, warp at tick 300 (phase 1 -> 2), anchor at tick 300, N 903 at the anchor.
Compares (`ticktrace_cmp/*.json`, all `--n-ok`):

| pair | IDENTICAL-FOR-N | NEW blocks |
|---|---|---|
| ddka/ddkc (same class; ddkb and ddkd ticks.bin are byte-identical, sha8 a83732da) | 600 / 600 | 0 |
| ddka/ddkb, ddka/ddkd, ddkb/ddkc, ddkc/ddkd (cross class) | 180 / 600 | `0x00898800` from record 180 |
| RODET h/i, h/j, h/k, i/j, i/k, j/k | 600 / 600 | 0 |
| RODET-h / DEF-ddkl | 600 / 600 | 0 |
| vs2 / vs3 | 600 / 600 | 0 |
| ddkl / RONEG-ddk2 | 399 / 600 | `0x007f0c00` from record 399 |
| ddkl / vs2 | 7 / 600 | `0x007f1000` from record 7 |
| r1 (original) / ddka | 17 / 600 | `0x00898c00` from record 17 (format check only) |

`detorig_report.py RO_DEF-ddkl RO_RONEG-ddk2 --norm re/frida/warp_normalize.tsv`: normalized 5/7, race-state 7/7, the one
UNEXPLAINED dword in the frame timer block at checkpoints k4 / k5 (ticks 450 / 600), PTR 899.

Knob units in the object build (`mashedmod/build/ddkobj/BYTECHECK.tsv`): `0x0040e480` `_CarSlotStateSet`, `0x004950b0`
`_QpcTimeScaledTo3Mhz`, `0x00493480` `_FpsDiscretise`, `0x00493390` `_PortTop_Bound_00493390`, `0x00497310`
`_PortTop_Bound_00497310`, `0x00534870` `_RwCore_00534870`: all IDENTICAL, diff_bytes 0.

## Differences from the original side (for RO-FIRST-DIVERGENCE / RO-ORACLE-GATE)
- N is the knob's wrapped main-thread QPC reads in matched units, not every main-thread clock call. Compare with `--n-ok`.
- exe checkpoints are taken at tick boundaries (field `tick`). The original's are taken at clock-call counts after the
  anchor (`cfg.ckpts`, dn 2000..30000). `detorig_report.py` pairs checkpoints by k, so an exe-vs-original checkpoint compare
  needs the same k at matching moments. Not done here: there is no exe race to compare yet.
- `holdrand` is null in exe checkpoints (the UCRT per-thread rand state is not reachable). `rw_ring` / `rw_cur` are the knob's
  own ring. `witness` is empty (no inline JMPs on the exe).

## Commits
- `6047c6ed4` PREREG (alone).
- this commit: the TU, the knob call sites, `exe_sources.rsp`, the `RO_` kind, 4 stack files, `replay_trace.json`,
  `trace_verify.py`, `trace_arms.sh`, this RESULT and the evidence below.

No tracker edit, no `ROADMAP.md` edit, no default flip, no `.asi` redeploy, no change to `original/`. No push, no merge.

## Evidence
Committed: this RESULT, `PREREG_RO-EXE-TICK-TRACE.md`, `trace_verify_out.txt` (verifier output, with the SHA-256 of every
`meta.json`, `ticks.bin` and `ckptNN.data.bin` of all 17 exe arms and r1), `ticktrace_cmp/` (every `detorig_ticks.py` JSON),
`ticktrace_lines.txt` (`[TRACE]` / `[REPLAY]` / OT_TIMER summary lines of every arm), `raw_sha256_ticktrace.txt` (both exes,
both build logs, every arm log, the arm outputs `arms1..7.out`, `RUNLOG.txt`).
Raw (not committed): `D:/MashedWT/RO-EXE-TICK-TRACE_data/` (arm directories, logs), build logs
`D:/MashedWT/RO-EXE-TICK-TRACE_build_{vs,ddk}.log`. Exes: VS2022 `413145d11d9ff0d8a633d592f978292bdb81f319819ba8e88b552e1604e0e537`,
object build `72914fa06c89c630a4e7d2b1e284f85a2c846468d5784b9a1979c8b753ca9199`.

Gates: not run (top-down row, knob default OFF).
