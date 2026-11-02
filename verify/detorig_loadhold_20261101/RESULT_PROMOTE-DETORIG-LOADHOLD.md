# RESULT PROMOTE-DETORIG-LOADHOLD: pin the track-load length, repeat the 6-pair test

Date 2026-11-01. Worker session cmvylb32z04w3pa1cijl8kwm7, lead cmvykvalu02zppa1co1e17wto.
Branch `orch/PROMOTE-DETORIG-LOADHOLD` (from `orch/integration` + `orch/PROMOTE-DETORIG-PILOT` merged in).
Review: **TEXT-ONLY** (harness and scorer under `re/frida/`, no build input).

## Verdict

**DoD met on the second branch: every remaining whole-image difference is explained region by region.**
Raw byte identity of the whole writable image is 0/15 in every pair, and cannot reach 15/15 under this harness
(heap layout and values written before the clock install, listed below).

1. **Load length pinned.** With the load hold (50 ms of real time in each main-thread clock call during phase 2,
   the `detorig.py` default since `off_g`), all 8 runs loaded the track in exactly **118** main-thread clock calls
   (pilot without the hold: 113, 118, 119, 122, 215). The call logs of off_g and off_h are identical call for call
   (118 of 118, `calllog/diff_off_g_off_h.txt`).
2. **Cause of the variance found** (traced, not inferred): the piz read `FUN_004b67e0` polls an overlapped read.
   Each poll calls a callback that reads QPC. The number of polls is the disk's real completion time. Detail below.
3. **6-pair test repeated at equal load length: NORMALIZED 15/15 and RACE-STATE 15/15 in all 6 pairs**, 0 unexplained
   dwords at any checkpoint. Pilot (same pairs, 118-call loads): NORMALIZED 0/15 in all.
4. **Negative control at equal load length now measured** (neg_c, +1 on the RenderWare RNG word at the ring cursor at
   the race anchor, load 118 calls): RACE-STATE **0/15**, NORMALIZED 0/15, about 35,000 unexplained dwords. So the
   scorer's classes do not absorb a one-word change, and the harness sees it.
5. **The last OFF vs ON difference has a writer**: 79 dwords at field `+0x2c` of the 256-row ring at `0x007ec9e4`,
   copied from an uninitialised stack slot by `FUN_00469aa0` (watchpoint runs off_w / on_w). Not race state.

| pair (all loads 118 calls) | RAW | NORMALIZED | RACE-STATE |
|---|---|---|---|
| off_g vs off_h (OFF/OFF) | 0/15 | **15/15** | **15/15** |
| on_c vs on_d (ON/ON) | 0/15 | **15/15** | **15/15** |
| off_g vs on_c (OFF/ON) | 0/15 | **15/15** | **15/15** |
| off_g vs on_d | 0/15 | **15/15** | **15/15** |
| off_h vs on_c | 0/15 | **15/15** | **15/15** |
| off_h vs on_d | 0/15 | **15/15** | **15/15** |
| off_i vs off_g (third OFF) | 0/15 | 15/15 | 15/15 |
| off_w vs on_w (watch arms) | 0/15 | 15/15 | 15/15 |
| off_g vs neg_c (negative control) | 0/15 | 0/15 | **0/15** |
| off_h vs neg_c | 0/15 | 0/15 | **0/15** |

ON = `MASHED_HOOK_ONLY=0x00468980,0x00470670` (A6b_Entry, A4_Entry, both C4), `.asi` SHA-256
`906f8601201f69b4670bfef68b438b719e515e0e4e523f892d92c9f095b8a8ad` (the pilot's copy). Witness byte at both RVAs is
`0xE9` in every ON run (on_c, on_d, on_w) and `0xD9` / `0x51` in every OFF run, at the anchor and all 15 checkpoints.

The negative control's RW ring and cursors differ from checkpoint 0. Car records (537 dwords), pickups (96), the
frame-timer block and CRT `rand` state differ from checkpoint 5 (dN 12,000) on, and through checkpoint 14.

## Why the track load took 113 to 215 calls

Phase 2 is one synchronous call: `FUN_004929d0` case 2 calls `FUN_0040d440` (course load), then sets
`DAT_00771968 = 3`. The call log (`--call-log`, new) recorded every main-thread clock call between the warp and the
race anchor with 14 stack dwords. In all 8 runs 105 of the 118 calls are one chain, checked against the stack frames
in the disassembly (`FUN_005ab040` frame 8 bytes, `FUN_005a6ea0` frame `0x10` + `esi`):

`QueryPerformanceCounter` (IAT `0x005cc0c4`) <- `FUN_005ab040` (ret `0x005ab058`, QPC when `DAT_007dce00 == 1`, else
`timeGetTime`) <- `FUN_005a6ea0` (ret `0x005a6ea9`, audio clock tick into `DAT_007dca50` / `DAT_007dca54`) <-
`FUN_005a60b0` (ret `0x005a60bf`) <- `FUN_0045d3f0(1)` (ret `0x0045d414`) <- `FUN_00429290` (ret `0x00429297`).

- `FUN_00429290` is registered as a callback at `0x004027c2..0x004027c7` (`push 0x429290; call 0x004b6560`), which
  stores it in `DAT_007d3e5c` (`FUN_004b6610`).
- `FUN_004b67e0` (piz read, `param_1` offset, `param_2` dst, `param_3` size) issues `ReadFile` with an `OVERLAPPED`,
  then loops (label `0x004b6873`): call `(*DAT_007d3e5c)(&DAT_008ad8a0, &DAT_008ab6e0, DAT_007d3e60)`, then
  `GetOverlappedResult(..., bWait = 0)`, and repeat while `GetLastError() == 0x3e4`. `FUN_004b6940` (piz open) also
  calls the callback once per archive.
- So each read costs one clock call per poll, and the number of polls is how long the disk takes. The load hold
  sleeps inside the callback's QPC call, before `GetOverlappedResult`, so every read in these runs completed before
  its first poll: one call per read.

Limit: a single read slower than 50 ms (cold file cache) would add a poll. `meta.json` records `race.dn_warp`;
`detorig.py` users should require `dn_warp == 118` for this scenario and re-run otherwise. The hold costs about
6 s per run (118 x 50 ms).

## Remaining whole-image diff, region by region

`detorig_report.py` classes (pilot definitions) plus four writer rules added here (`RULES` in the scorer, each
citing its writer). Counts for off_g vs on_c, 15 checkpoints. off_g vs off_h has the same set minus RING4 and
STACKROW (those two need the slot phase or the hooks to differ).

| class | dwords | where | explanation |
|---|---|---|---|
| PTR | 3,840 | `.data` pages `0x0061..0x0091` | both values heap addresses (`0x01000000..0x30000000`, 4-aligned): heap layout differs between processes |
| RESIDUE | 31 | `0x00616038`, `0x007f102c`, `0x00771e78`, `0x00772fb4/b8`, `0x007d4108`, `0x007dcdf0`, `0x007de100`, `0x0077391c`, frontend floats `0x00636ae8/afc`, `0x0067d858/974`, `0x007dc5fc..0x007dc610` (values below the PTR floor), others | one value through all 15 checkpoints in both runs: last written before the race window. `0x00616038` is seeded by `FUN_004a57e4` (system time / PID / TID / tick / QPC). `0x007f102c` is the real QPC sample from `0x00493478` (pilot). `0x0077391c` is the focus flag (only reader is the `no_focus_pause` gate) |
| OFFSET | 7 | `0x007f0ff0` (+50 per frame), `0x007dca50` (audio clock `FUN_005a6ea0`, store `0x005a6f16`), `0x0077198c`, `0x007719a4`, `0x007d7108`, `0x007dc958`, `0x00616110` | a - b the same at all 15 checkpoints: counters that step alike during the race from different start values (frames and real time before the clock install) |
| MODOFFSET/60 | 97 | `0x00771978` + 96 dwords at stride `0x44` from `0x00868348` | `0x00771978` is the 60-frame counter of `FUN_004926c0` (store `0x00492737`, reset `0x00492762`): same increments, different phase |
| MODOFFSET/4 | 1 | `0x007d70c8` | dynamic-VB ring slot, +1 mod 4 in `FUN_004dc8a0` |
| TOGGLE | 2 | `0x006147d8`, `0x00771a00` | values in 0..3 only |
| RULE:ACC60 | 2 | `0x00771994`, `0x0077197c` | `FUN_004926c0` adds each frame's `FUN_004950b0` delta to `0x00771994` (`0x00492731`) and at 60 frames stores the average in `0x0077197c` and zeroes it (`0x00492758`). The window it sums depends on the `0x00771978` phase. Rule: the counters differ and both values are whole virtual-QPC steps (4,500 = 15,000 x 3,000,000 / 10^7) |
| RULE:RING4 | 2 | `0x007d70cc..0x007d70e8` | `FUN_004dcbb0` stores the slot's fill offset at `[0x007d70c8*4 + 0x007d70cc]` (`0x004dccc8`). Rule: the two 4-slot arrays agree once each run is rotated to its own slot |
| RULE:WINMSG | 6 | `0x007e95a0..0x007e95bb` | the `MSG` passed to `PeekMessageA` (IAT `0x005cc270`, call `0x0049969d`): filled by Windows (time, cursor) |
| RULE:STACKROW | 79 | field `+0x2c` of rows in `0x007ec9e4..0x007f09e4` | see next section |
| UNEXPLAINED | **0** | | |

The generic classes are what the race window leaves unchanged or steps alike. The negative control shows they do not
swallow a real change (34,977 unexplained dwords in off_g vs neg_c).

## The `0x007ec9e4` ring (OFF vs ON only)

Watch arms off_w and on_w set hardware write watchpoints on `0x007eca10` and `0x007eca50` from the race anchor.
3 hits each, all at pc `0x00469daa` (the instruction after the store), backtrace through `0x00470948` (OFF) to
`0x0047113c`, `0x00425a92`, `0x0040fc71`, `0x004112da`, `0x00492d8a`, `0x004922b8` (`watch/*.jsonl`).

- The store is `mov [edx*4 + 0x7ec9e4], ecx` at `0x00469da3` in `FUN_00469aa0`. The loop `0x00469d90..0x00469daa`
  copies 16 dwords `[esp + 0x20 + 4*j]` into row `DAT_007e9de0` (row stride `0x40`); `0x00469dac..0x00469db7` adds 1
  to the row index and compares with `0x100`.
- `FUN_00469aa0` (`sub esp, 0x54` + 3 pushes) writes only `[esp+0x20..0x44]` before the copy (`0x00469c42`,
  `0x00469c49`, `0x00469c50`, `0x00469c5d`, `0x00469c67`, `0x00469c6b`, `0x00469c7b`, `0x00469c7f`, `0x00469c83`,
  `0x00469c87`). Its one stack-address argument (`lea ecx, [esp+0x2c]` at `0x00469bc2`, six dwords pushed) points at
  `[esp+0x14]`, outside the copied window. So row fields `+0x28..+0x3c` copy stack dwords the function never wrote.
- Field `+0x2c` held `0x00470948` in OFF (the return address of `call 0x00468980` at `0x00470943` in
  `FUN_00470670`) and `0x008845d4` in ON at every hit. The hooks replace that call, so the stale dword differs.
- Readers of fields `+0x24..+0x2c` as three dwords: `0x0040bebe..0x0040beca`, `0x0040c2d8..0x0040c2e4`,
  `0x00422e60..0x00422e70`. [UNCERTAIN] what those readers compute was not traced. RACE-STATE stayed 15/15 in every
  OFF/ON pair, so within this scenario the value does not reach car records, pickups, input, RNGs or the race clock.

This is the only place the two C4 hooks leave a different value in the image, and it is the content of a slot the
original never initialises.

## Harness changes (this branch)

- `re/frida/js/detorig.js` + `detorig.py`: `--call-log` (native, in the clock stubs: 64 bytes per main-thread clock
  call from the warp to the race anchor, `calllog.bin`), `--watch VA[,VA]` (debug-register write watchpoints on every
  thread from the race anchor, `watch.jsonl`, port of `scenario_launch.py --watch-write`). The main-thread test in
  both stubs now uses a rel32 `jne`. Stubs were self-tested in a 32-bit process before the arms (Q / N / log / cap /
  forwarding). Commit `695a950ad`.
- `re/frida/detorig_calllog.py`: call-site histogram and sequence diff of two call logs.
- `re/frida/detorig_report.py`: writer rules RING4, ACC60, WINMSG, STACKROW, tried before the generic classes;
  `by_class_and_page` in the JSON. Re-scoring the pilot's own data: off_c vs off_d and on_a vs on_b go from
  NORMALIZED 0/15 to 15/15.

## Runs

8 arms, one lock hold (`mashed_lock.py run --label PROMOTE-DETORIG-LOADHOLD`, non-solo, taken 23:13 after the
merge train; arms 23:13-23:25), each under `test-slot`, private root `D:\MashedWT\PROMOTE-DETORIG-LOADHOLD_root`
(robocopy of the pilot root). Order: off_g, on_c, off_h, on_d, neg_c (`--negctl 1`), off_i, off_w, on_w. All
`--call-log --timeout 400`, load hold 50 ms (default). 8/8 rc 0, 15 checkpoints each, no crash, no piz collision.
Scripts: `batch1.sh`, `run_arm.sh` (copies here).

Binaries in the private root (same as the pilot): `MASHED.exe`
`b9977dab70607720cf58bc7832937c203b1e0b33ff5896da586baf56aa79fe87`, `d3d9.dll`
`479019513e752bf68b6c4cb77fa2cc36d69fdcfb5be2c56fb87b295d0c88c7be`, `mashed_qol.asi`
`1567c95b4fdb38f27a1313f3857983f707867c0700b5ec99cc4988b57bf59df0`, `mashed_re_dev.asi` as above. No change to
`original\`, no unlock patch, no redeploy of `original\mashed_re_dev.asi`.

## Evidence

Committed: this file, `reports/` (scorer JSON per pair + `batch1_scores.txt`), `meta/` (per-run `meta.json`),
`calllog/` (call-site histogram per run, `diff_off_g_off_h.txt`), `watch/` (both watch logs), `raw_sha256.txt`
(SHA-256 of every raw checkpoint dump, call log, watch log and run log). Raw data stays in
`D:/MashedWT/PROMOTE-DETORIG-LOADHOLD_data/<run>/` (15 x 3.3 MB per run).

Gates: not run. The change is under `re/frida/` and changes no build input, so the exe gate hashes cannot move.

## Next

- RACE-STATE A/B at equal load length is now a usable C4 scenario route for hooks on the race path: run OFF x2,
  ON x2 and one negative control with the load hold, require `dn_warp == 118`, score with `detorig_report.py`.
  Any UNEXPLAINED dword is a finding.
- The image is not byte-identical because of heap layout and pre-install state. Installing the clock before the first
  frame (suspended create) would remove the OFFSET / MODOFFSET / RESIDUE classes that come from boot frames, but not
  PTR or the PID / time seeds. Not needed for the DoD.
- [UNCERTAIN] the readers of the `0x007ec9e4` ring (`0x0040bebe`, `0x0040c2d8`, `0x00422e60`) were not traced. A
  longer or different scenario could reach race state through them.
