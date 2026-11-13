# PREREG RO-EXE-TICK-TRACE (2026-11-13)

Row: replay oracle 8. The exe side of the per-tick trace: `MASHED_REPLAY_TRACE=<dir>`, default OFF, read only when
`MASHED_REPLAY` is on (so only on the `MASHED_ORIG_TOP` path). Same files as `re/frida/detorig.py` writes for the
original (`meta.json`, `ticks.bin`, `ckptNN.data.bin`), so `re/frida/detorig_ticks.py` and `re/frida/detorig_report.py`
read an exe run directory with no change. Plus a `run_arms.py` arm kind `RO_<tag>[-<rep>]`. Committed alone, before any arm.
Depends on RO-EXE-REPLAY-KNOB (merged `fe4a56888`).

## Change under test
- `mashedmod/src/mashed_re/Match/PortTop_ReplayTrace.cpp` (new, exe only, ORIG_TOP only):
  - per tick, at the tick boundary (the knob's `PortTop_Replay_00493390`, after the virtual clock and tick advance and
    before the matched body: the order of `detorig.js`'s CModule `onEnter` on FUN_00493390), one record
    `[tick, N, FNV-1a hash per block ..., raw words ...]` appended to `ticks.bin`. Image `0x005ea000..0x00915008`
    (detorig.js `DATA_LO` / `DATA_HI`), block 1024 B, a dword in `[0x00995000, 0x30000000)` with the low 2 bits clear
    hashed as 0 (the heap rule), the raw ranges of `verify/romenu_20261112/run_arm.sh` by default. Hashing from tick 1
    (detorig.js boot mode). N = the knob's wrapped main-thread clock reads (the exe counterpart of detorig's main-thread
    clock-call count; its scope differs, see the knob RESULT, so compares use `--n-ok`).
  - full dumps `ckptNN.data.bin` of the same image at chosen ticks (`MASHED_REPLAY_TRACE_CKPT`), taken at the same point
    as that tick's record, and `k = 100` at the warp before its writes (detorig.js `menuCkpt`).
  - pages of the range that are not committed readable in the exe are hashed and dumped as 0 and listed in `meta.json`.
  - `meta.json` with the detorig.py keys the readers use: `ticks` (`total`, `nblk`, `recw`, `block`, `raw`, `file`,
    `phases`), `ckpt` (k, tick, n, file, sha256, phase, clock words), `warp`, `race` (`n` = N at the anchor, 0 if not
    reached), `boot`, `cfg`, `rc`, plus `side: "exe"`.
  - trace self-test: at every checkpoint tick an independent recompute of the block hashes and raw words from the dump
    buffer must equal the record written for that tick. Summary `[TRACE] self-test PASS|FAIL (f of n checks failed)`.
  - negative control `MASHED_REPLAY_TRACE_NEGCTL=<tick>:<va>`: +1 on the dword at `va` at that tick, before the hash.
- `Match/PortTop_Replay.cpp`: calls into the trace at init, tick, warp, anchor and summary. No change to what the knob does.
- `mashedmod/exe_sources.rsp`: the new TU.
- `verify/savelink_20261023/run_arms.py`: arm kind `RO_<tag>[-<rep>]`: `MASHED_ORIG_TOP=1`, `MASHED_REPLAY` =
  `A4_REPLAY_JSON` (default `verify/replayoracle_20261113/replay_selftest.json`), `MASHED_REPLAY_TRACE` = `<A4_OUT>/<arm>`,
  `MASHED_DET_FRAMES` = `A4_RO_FRAMES` (default 600), then the stack's knobs. The kind needs code, so it is a branch in
  run_arms.py. No new `STACK[...]` line. The stack for the negative control is a file `stacks/RONEG.txt`.
- `verify/replayoracle_20261113/trace_verify.py` (new): offline check of one run directory (record count = ticks, every
  checkpoint dump re-hashed = its tick's record, raw words = dump).

## Arms (root `D:/MashedWT/RO-EXE-TICK-TRACE_root`, test-slot outside, `mashed_lock.py run` inside, `MASHED_NO_DEPLOY=1`)
Scenario `replay_selftest.json` (the knob's: warp at tick 300, track 0, mode 10, 4 cars), 600 frames, checkpoints at
ticks 1, 150, 299, 301, 450, 600.

| arm | exe | extra |
|---|---|---|
| RO_DEF_ddk-a, RO_DEF_ddk-b | object build | none |
| RO_DEF-vs | VS2022 | none |
| RO_RONEG_ddk | object build | `MASHED_REPLAY_TRACE_NEGCTL=400:0x007f0ff0` (stacks/RONEG.txt) |
| off_ddk | object build | `MASHED_ORIG_TOP=1 MASHED_REPLAY=...` without `MASHED_REPLAY_TRACE` |

(arm names are final in the RESULT; `-<rep>` only names the repeat.)

## Predictions (pass criteria)
1. Default OFF: without `MASHED_REPLAY_TRACE` (off_ddk) no `[TRACE]` line and no file written; `[REPLAY] self-test PASS`
   as in the knob RESULT. The default build path is unchanged: the trace is reached only from the knob's code.
2. Both builds compile; rva-lint NEW = 0; `ddkobj.py check` unchanged against the knob RESULT's merged tree
   (no matched unit added or changed by this row).
3. Every trace arm: `[REPLAY] self-test PASS (0 of 666 ...)` and `[TRACE] self-test PASS (0 of n)`; 600 records;
   `trace_verify.py` OK on each directory.
4. Determinism (the oracle on the menu-less race, exe side): `detorig_ticks.py RO_DEF_ddk-a RO_DEF_ddk-b --n-ok`
   reports IDENTICAL-FOR-N = 600 of 600, NEW blocks 0. [UNCERTAIN] blocks the exe fills from real time or the OS
   (window messages, hand-path audio, heap pointers outside the rule) may differ; any that do are listed with their
   range and the run is RED for exactness until they are explained or masked.
5. Negative control: ddk-a vs RONEG first differs at tick 400, in the block holding `0x007f0ff0`.
6. Build difference (report, not pass/fail): ddk-a vs vs. The knob RESULT shows the VS2022 timer body stores a
   different `DAT_007f1010` on 207 calls, so a first difference in the block of `0x007f1010` is expected. Recorded as found.
7. Format: `detorig_ticks.py` reads an exe directory against the original boot-mode reference
   `D:/MashedWT/RO-MENU-REPLAY_data/r1` (same block, same raw ranges) without error. The verdict of that compare is
   recorded, not scored: the exe's ORIG_TOP path has no race yet (PORT-OT-RACE) and its warp tick differs.
8. Gates: not run (top-down row: all of it runs under `MASHED_ORIG_TOP` + `MASHED_REPLAY`). No G-CTRL, no B, no 12 captures.

Verdict rule: GREEN when 1, 2, 3, 4, 5 hold and 6, 7 are recorded. RED on a self-test failure, a `[TRACE]` line or a trace
file without the knob, or a crash that is not the U-9677 piz collision.
