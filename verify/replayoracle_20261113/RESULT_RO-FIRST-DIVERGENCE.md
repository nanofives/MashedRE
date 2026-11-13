# RESULT RO-FIRST-DIVERGENCE (2026-11-13): first-divergence report, exe ORIG_TOP replay vs the original

Row: replay oracle 9. Kind: race-path / top-down row diagnosing the oracle (no oracle gate exists yet). Worker session
(claude2, moved to claude3 at 95% usage), lead cmwezbsdu03y9lh1nk6vyjtuv. Branch `orch/RO-FIRST-DIVERGENCE` from
`orch/integration` `69cd03777`. **Status: WORK IN PROGRESS, stopped by L0 STOP for run 43.** What is measured and what
is not is listed below. The ledger row was not moved to review.

## Verdict so far

**The tool works and the first divergence is found. Two items are still open (end of this file).**

- **Self-test (tool):** the planted divergence (`RONEG`: +1 on `0x007f0ff0` at tick 400, exe object build) is reported
  at record 399 = tick 400, block `0x007f0c00`, dword `0x007f0ff0`, `0x00001356` (ref) vs `0x00001357` (run), equal at
  tick 399 (`0x00001324` both). Every dump re-hashes to its own tick record (OK on all 4 dumps). The two blocks that
  differ at every record between two exe runs (`0x00772c00`, `0x007e9400`, the RULE:HWND words) are classed EXPLAINED at
  tick 399 and not counted. **PASS.** One gap, fixed in source but not yet re-run: with tick-based frames the planted
  write itself (pc `0x1a2de5` = `_ReplayTrace_Tick+0x85`, value `0x1357`, tick 400) is not inside "the frame tick 399 ->
  400". The `rec` field (below) makes the frame exact.
- **Self-test (exe trace):** `[TRACE] self-test PASS` on all 5 exe arms (0 of 16647 checks on fd1 / fd2 / fdvs, 0 of 11098
  on fdref / fdneg), `[REPLAY] self-test PASS (0 of 666)` on all 5.
- **Watch self-test (exe):** `0x007f1030` watched on fd1: 604 hits, 600 of them at one pc (`0x18b0f` =
  `_PortTop_Body_00493390+0xcf`, the matched 0x00493390 body), one per tick 1..600, the rest at boot (`_Store7f1030_Qpc3Mhz`,
  tick 0) and at the warp (tick 300). With 4 slots: `dr7 0xDDDD0055`, armed rc 0 on the main thread.

## The first divergence (exe object build vs the original, matched replay configuration)

Reference: the original, boot-mode replay matched to `replay_trace.json` (q0 `0x40000000`, warp at tick 300 with clock
300, 600 records): runs o1, o2, o3b. Run: the exe object build with `MASHED_REPLAY` + `MASHED_REPLAY_TRACE`: RO_DEF-fd1,
RO_DEF-fd2. Masks `re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv` (21 blocks wholly masked). Report:
`firstdiv/fd_o1_fd1.md`, `firstdiv/fd_o2_fd2.md`, `firstdiv/fd_o3b_fd2.md`.

### 1. Record 0 (tick 1): the exe image pad

- **First divergent record: 0 (tick 1).** 537 counted blocks differ at every one of the 600 records. The differences are
  already there at tick 0 (original: the PE entry point, `--ckpt-ticks 0`; exe: the trace init in `Replay_Init`).
- **First block `0x005ea000`, first dword `0x005ea004`:** ref `0x004a57e4` at ticks 0 and 1 (= the file initializer in
  `.data` of `MASHED.exe.unpatched`), run `0x00000000` at ticks 0 and 1. No absolute-operand reference in `.text`.
  Writers: exe none (watch armed from the trace init, 0 hits in 600 ticks, fd2). Original: no write that changed the
  value before tick 1 (o3b). One same-value write was seen at pc `0x00492502` (in FUN_004924f0 [boot]), value
  `0x004a57e4`, at tick 0. [UNCERTAIN] which of the two watched dwords it stored to: o3b watched
  `0x005ea004` and `0x007f1030`, Frida's context has no DR6, and neither value changed.
- **Dword census at tick 1** (the 537 blocks, both dumps re-hash OK against record 0): 81,067 differing dwords. 80,116
  VALUE, 10 IMGPTR, 317 PTR, the rest MASK / RULE rows. The VALUE dwords split against the original's file image:

| category | section | dwords | run value 0 | first va |
|---|---|---|---|---|
| FILEINIT-MISSING (ref = file initializer, run not) | .data | 47,577 | 47,576 | `0x005ea004` |
| FILEINIT-MISSING | .data (bss) | 25 | 0 | `0x007032ec` |
| BOOT-WRITE (run = file initializer, ref not: the original wrote it before tick 1) | .data | 224 | 224 | `0x00604604` |
| BOOT-WRITE | .data (bss) | 31,698 | 31,698 | `0x0063bd98` |
| BOOT-WRITE | _rwdseg | 2 | 2 | `0x00915000` |
| OTHER | .data | 465 | 465 | `0x006147dc` |
| OTHER | .data (bss) | 125 | 0 | `0x0067d974` |

- The source states the cause of the FILEINIT-MISSING class: `mashedmod/src/mashed_re/Track/CourseTableState.cpp:4-6`:
  "file-backed .data in the original ... The exe's image pad (exe_main.cpp B17) holds them zero-filled". The run holds
  0 in 47,576 of the 47,577 `.data` dwords.
- **First BOOT-WRITE dword `0x00604604`:** ref `0x00000004`, run `0x00000000`. Original writer: pc `0x0045d6a8` in
  FUN_0045d460 [audio] (hooks.csv C2, PERMDICT.RWS loader), at tick 0, backtrace `0x0045d6a8 | 0x004669fb | 0x004028e0`
  (o2). No absolute-operand reference (`xref.py --string 0x00604604`: none). Exe: 0 hits.

### 2. First NEW block: `0x00898c00` at record 17 (tick 18) = IDENTICAL-FOR-N 17 of 600

- The detorig_ticks.py verdict on the same pair. The block is equal on both sides at records 0..16.
- 20 dwords, stride `0x34`, `0x00898c00 .. 0x00898fdc`. At tick 17: 0 on both sides. At tick 18: ref `0x409fffff`
  (float 4.9999995), run `0x00000000`. BSS (file initializer 0). No absolute-operand writer.
- **Original writer (o2, watch from the install):** pc `0x00432763` in FUN_004325c0 [frontend] (hooks.csv C4, `impl`,
  `Match/MatchS08_004325c0.cpp`, notes "menu slide anim tick"), the only hit in frame 17 -> 18. 530 hits on the word in
  all: `0x00432763` 283 (ticks 17..299) and `0x00432777` 247. Ghidra caller of FUN_004325c0: FUN_0043dfd0 only (its exe
  source is the provisional body `Match/ProvS08_0043dfd0.cpp`).
- **Exe writer (fd2):** one hit in 600 ticks, at tick 0, pc `0x1b43f` = `_MenuEntryArrayInit+0xf`, value 0. Nothing writes
  the word during the menu. [UNCERTAIN] whether the exe ORIG_TOP path calls 0x0043dfd0 / 0x004325c0 at all. Evidence
  needed: a watch or a call counter on the exe body `_MenuAnimTickTwin`.
- The original side is deterministic here: o1 vs o2 have no NEW block before record 301 outside `0x00614400`
  (`0x006147d8`, MASK:os). Raw timer words `0x007f1030` / `0x007f1010` / `0x007f102c` differ from record 0 (boot-call-count
  dependence before the warp pin). o1 vs fd1 and o2 vs fd2 give the same record-17 block and values.

### 3. VS2022 build

RO_DEF-fdvs (VS2022 exe, sha8 `c84c3a41`) vs o2 gives the same answers: record 0 with 537 blocks, the same census, NEW
block `0x00898c00` at record 17. fd2 vs fdvs: IDENTICAL-FOR-N 600 of 600, 5 constant blocks (exe-image addresses, as
RO-EXE-TICK-TRACE found). Writers resolve through `mashed_re.map` (`_MenuEntryArrayInit+0x28`, `_Store7f1030_Qpc3Mhz+0x24`).

## What was built

- `re/frida/detorig_firstdiv.py` (new). Inputs: two tick-trace directories in the detorig format (original or exe).
  - It finds the first record with a counted block or raw word different. Wholly masked blocks are left out, and so are
    constant blocks EXPLAINED at a tick both runs dumped.
  - It re-hashes the tick c - 1 / c dumps against their records.
  - It lists the differing dwords with both values at c - 1 and c, the class (MASK / RULE, PTR, IMGPTR, VALUE), the file
    initializer and section, and the static referrers. The referrers are absolute operands with read / write access,
    named from the Ghidra facts and hooks.csv.
  - It gives a census by FILEINIT-MISSING / BOOT-WRITE / OTHER, and the writers from `watch.jsonl` (exe pcs through
    `--exe-map`).
  - When the evidence is missing it prints a re-run plan. Options: `--tick`, `--new-only`, `--masks`, `--ignore`,
    `--json`, `--md`.
- `re/frida/detorig.py` / `js/detorig.js`:
  - `--ckpt-ticks` (boot mode, k = 200 + i, tick 0 = at the install). Checkpoint fields that cannot be read before the
    CRT / RW exist are null.
  - `tick` in checkpoints and watch hits.
  - `rec` (records written) in watch hits. **Not yet run.**
  - In `w` mode the agent's own pins at the warp / anchor run with this thread's watch suspended. o3 failed without this
    (`onDue Error: single-step triggered at pinTimer`). o3b passed with it.
- `mashedmod/src/mashed_re/Match/PortTop_ReplayTrace.cpp` (exe only, under the already default-OFF `MASHED_REPLAY_TRACE`):
  - `MASHED_REPLAY_TRACE_WATCH=<va,...>` (up to 4): DR0..DR3 write watch on the main thread, armed by a helper thread,
    with a vectored handler. Hits go to `watch.jsonl`: tick, n, eip, slot, value, ebp-chain, and per-pc counts.
  - `MASHED_REPLAY_TRACE_WATCH_FROM`.
  - `MASHED_REPLAY_TRACE_CKPT` accepts tick 0 (dump at the trace init).
  - `meta.json` `watch`.
  - `rec` in hits: **built (second build: Build OK, the same ddkobj summary, rva-lint NEW=0,
    log `D:/MashedWT/RO-FIRST-DIVERGENCE_build_ddk2.log`), not yet run in an arm.**
- `verify/savelink_20261023/run_arms.py`: `RO_` kind passes `A4_RO_WATCH` / `A4_RO_WATCH_FROM`.
- `verify/replayoracle_20261113/firstdiv_arms.sh`: the arm driver (test-slot outside, mashed_lock inside).

## Builds (first build, the one every arm used)

- `MASHED_DDKOBJ=1` build: Build OK. One build.bat run made both `mashed_re.exe` (VS2022) and
  `build/ddkobj/mashed_re_ddk.exe`.
- rva-lint NEW=0, asm-lint OK.
- `ddkobj.py check`: IDENTICAL 1188/1214, DIFFERENT 11 (`0x00401690 0x004332a0 0x0043c5b0 0x0045a3a0 0x00475a00
  0x004929d0 0x00493710 0x00493900 0x00498810 0x004c0c20 0x004c0f00`), not-in-map 15, units linked 1164/1440, refusals 4
  (wrapper-redefines-globals). This row adds no matched unit.
- Log: `D:/MashedWT/RO-FIRST-DIVERGENCE_build_ddk.log`.

## Deviations

- **No PREREG was committed before the arms.** The exe change is harness code under `MASHED_REPLAY_TRACE`, which is
  default OFF and needs `MASHED_REPLAY` + `MASHED_ORIG_TOP`, so the default path does not execute it. WORKER_RULES still
  asks for a PREREG on a new knob, and this row did not write one.
- Gates: not run (top-down row). No 12 captures. No tracker, ROADMAP or `.asi` edit. Nothing pushed.

## Open (next session)

1. Finish the rebuild with `rec`. Re-run RO_DEF-fdref / RO_RONEG-fdneg and confirm that the planted write is the last
   writer in frame `rec == 399`. Re-run one original watch arm to confirm `rec` there.
2. Exe side of `0x00898c00`: does the ORIG_TOP menu call 0x0043dfd0 -> 0x004325c0? Watch or count `_MenuAnimTickTwin`.
3. For RO-ORACLE-GATE: the record-0 classes need a decision (fill the pad from the file image, or mask
   FILEINIT-MISSING / BOOT-WRITE until the boot is matched). That is a decision row, not this row's call.
4. Commit a PREREG for the watch knob if the lead wants one.

## Evidence

- Committed: this RESULT, `firstdiv/` (every report `.md` / `.json`, `ticks_*.json`, `arms_*.out`), and
  `raw_sha256_RO-FIRST-DIVERGENCE.txt`. The latter holds the SHA-256 of each cited raw file: per-arm `meta.json`,
  `ticks.bin`, `watch.jsonl`, the tick dumps, the arm logs and the build log.
- Raw (not committed): `D:/MashedWT/RO-FIRST-DIVERGENCE_data/{o1,o2,o3,o3b,RO_DEF-fd1,RO_DEF-fd2,RO_DEF-fdvs,RO_DEF-fdref,RO_RONEG-fdneg}`.
  Root: `D:/MashedWT/RO-FIRST-DIVERGENCE_root`.
- Exes as used by the arms: object build `70845a28bbe60596eab48013ecb13afe78fd7b3819cbc0362a239c2ede4dc4b6`,
  VS2022 sha8 `c84c3a41` (run_arms.py line). The full VS2022 hash was not taken before the rebuild replaced the file.
