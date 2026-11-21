# RESULT PROV-INSITU-REF1-WS2 (run 54, 2026-11-21): CHECKPOINT, not final

Worker cmwqebtl072f9nz1cfoy3lc9k (claude2, claude-opus-5-5 medium), lead REPLAY-AUDIO (cmwqea6x671wxnz1cgusoou57).
Branch `orch/PROV-INSITU-REF1-WS2` from `orch/integration` `77dedfece`. Row kind: replay, match-only. Nothing is bound
into the exe: no build, no ddkobj check, no G-CTRL, no B, no 12x4 captures. No promotion TSV yet.

## STATE (pre-reboot checkpoint, 2026-11-21 ~11:50)

- **Done:** the class table for the 203 IDENTICAL-FOR-600 bodies that PROV-INSITU-REF1 did not promote
  (`WS2_CLASSES.tsv`, below). It is built from re-decoding the ref1 write logs (`ws2_classes.py`).
  Also done: 22 of 31 singles, the ws_g01 split down to 2 members, and the no-step controls (below).
- **Running when stopped:** three drivers. Each was stopped between runs, and only this row's PIDs were killed
  (the game lock was free afterwards). The three runs in flight were killed and have no state line, so they re-run
  on resume: `nw_s_00448220`, `ws_g01baba`, `s_00480d60`. Their partial folders are in the raw folder and are
  listed in `WS2_RAW_SHA256.txt`.
- **Next, in order** (each command resumes from `D:/MashedWT/PROV-INSITU-REF1-WS2_data/ws2_state.jsonl`, run from
  this folder):
  1. `test-slot py -3.12 ws2_driver.py singles`: 9 singles left (`0x00480d60` on).
  2. `test-slot py -3.12 ws2_driver.py split`: the rest of ws_g01 (ws_g01baba, ws_g01babb), then ws_g03 and ws_g05.
  3. `test-slot py -3.12 ws2_driver.py stride`: 31 frame-only bodies in 5 groups, every K-th call from the race
     anchor (`ws2_wstride.json`, `ws2_STRIDE_PLAN.tsv`; PROV-WRITESET-SCENARIO method).
  4. `test-slot py -3.12 ws2_driver.py nowlog nw_s_00448220=0x00448220`: rerun of the killed control.
  5. Run `re/tools/prov/prov_writeset.py` over the IDENTICAL single and stride runs. The singles take
     `--bodies D:/MashedWT/PROV-INSITU-REF1_data/bodies309.json`, plus `bodies_nw.json` for `0x005185a0`. Then the
     cut rule (`ws_cut.py` logic: covered, image > 0, stepped run IDENTICAL), `re/promotions/PROV-INSITU-REF1-WS2.tsv`,
     and `c4_reclass.py batch --tsv` as a dry run.
- Every arm runs through `ws2_arm.py`: the ref1 argv unchanged, test-slot outside and `mashed_lock.py run` inside.
  It blocks while `mashed_lock.py status` shows PRIORITY or SOLO. `lockstatus.log` shows the waits, for example
  s_004f0f10 re-checked every 60 s from 11:03 to 11:09. Root: `D:\MashedWT\PROV-INSITU-REF1_root`, reused
  read-only. Its MASHED.exe `b9977dab...fe87`, unpatched `bdcae093...3c0e`, gamesave.bin `bd187881...2a70`, d3d9.dll
  `47901951...c7be` and mashed_re_dev.asi `906f8601...a8ad` were re-hashed at start and are equal to the ref1 root.

## Why the 203 bodies fail the write-set leg (`WS2_CLASSES.tsv`)

257 IDENTICAL-FOR-600 bodies, minus the 54 promoted in PROV-INSITU-REF1, leaves 203.

| class | count | subclass | count |
|---|---|---|---|
| A long-race hold | 3 | standing long-race DIVERGED on the current source (`0x00469aa0`, `0x0046e9e0`, `0x004709a0`) | 3 |
| B escapes | 131 | heap/other only | 47 |
| | | caller-stack through a pointer (for example `mov [ecx], eax`), nothing else | 26 |
| | | caller-stack own argument slot, esp/ebp-relative above the return address, + heap/other | 24 |
| | | caller-stack own argument slot only | 15 |
| | | caller-stack through a pointer + heap/other | 6 |
| | | heap/other + image-masked | 5 |
| | | image-masked only | 3 |
| | | own argument slot + heap/other + image-masked | 2 |
| | | caller-stack both forms + heap/other | 2 |
| | | caller-stack both forms | 1 |
| C frame only (L-40) | 38 | race calls, stride re-step planned | 31 |
| | | all calls before the race anchor, or every call already stepped (`0x004ddcb0`, `0x005abd30`, `0x005ab410`, `0x004e51e0`, `0x005b2080`, `0x005b04b0`, `0x00405540`) | 7 |
| D covered+image, stepped run diverged | 3 | `0x00420050`, `0x0046ddb0`, `0x00448220` | 3 |
| E not-run | 28 | ws_g05 write log full: 2,097,152 records, 826,682 dropped; `0x004df120` alone took 2,083,048 steps in 6 calls | 16 |
| | | no main-thread call stepped | 11 |
| | | not in a ws group (`0x005185a0`, wired later in the NOT-WIRED leg) | 1 |

- Heap/other = a store target outside the image, the stack window and the blob, and below 0x70000000. No memory map
  is recorded, so heap and other allocations are not told apart. No escape store lands in the exe's code or .rdata,
  in the prov stub area or above 0x70000000 (`ws2_classes.py` region split). Under L-38 a heap writer needs a
  heap-tracked compare with more than 0 pages compared. The replay harness hashes the image only, so no run of
  this harness can cover these 86 bodies (every B row with heap/other).
- Image-masked = a store into a 1 KiB block that ref1 and ref2 already disagree on. No compare of the two references
  can cover it.
- Own argument slot: the store addresses `[esp+k]` / `[ebp+k]` and lands at base_esp+4 .. base_esp+0x1c, that is,
  the body's incoming parameter slots (base_esp points at the return address). Example: `0x00463640` +0x101
  `mov dword ptr [esp + 0x34], ecx` stores into base_esp+0x4. Of the 15 bodies with no other escape, 11 also store
  into the compared image. Classing these slots as frame would change the leg's definition. **DECISION text for
  the lead**, below. [UNCERTAIN] Whether any caller reads such a slot after the call, and whether the original
  function stores the same slot. Tests: disassemble the original extent of each of the 15 for the same
  esp/ebp-relative positive-offset store, and scan the callers' code after each call site for a read of the
  argument area (register line added).
- E "no main-thread call stepped": `s_005b6b00` (single) stepped 0 main-thread frames and logged 10 other-thread
  steps, so its calls run on a non-main thread. The write log records main-thread steps only (detorig.js
  wsInstall, PROV-WRITESET-SCENARIO "Limits"). The other 10 of this subclass are checked by their singles.

## Singles so far (`ws2_state.jsonl`, stepped, `--prov-wlog 10`)

| rva | ref1 leg | single | replay | main-thread frames stepped | other-thread steps |
|---|---|---|---|---|---|
| 0x005b6b00 | not-run | s_005b6b00 | IDENTICAL-FOR-600 | 0 | 10 |
| 0x00420050 | covered+image, diverged | s_00420050 | DIVERGED@300, block 0x692400 | 10 | 0 |
| 0x0046ddb0 | covered+image, diverged | s_0046ddb0 | IDENTICAL-FOR-600 | 10 | 0 |
| 0x00448220 | covered+image, diverged | s_00448220 | DIVERGED@302, block 0x896400 | 10 | 0 |
| 0x005752b0 | not-run | s_005752b0 | IDENTICAL-FOR-600 | 10 | 0 |
| 0x00576880 | not-run | s_00576880 | IDENTICAL-FOR-600 | 10 | 0 |
| 0x00574ad0 | not-run | s_00574ad0 | IDENTICAL-FOR-600 | 10 | 0 |
| 0x00445aa0 | not-run (ws_g05) | s_00445aa0 | DIVERGED@302, block 0x896400 | 10 | 0 |
| 0x004e8120 | not-run (ws_g05) | s_004e8120 | IDENTICAL-FOR-600 | 10 | 0 |
| 0x0046ef70 | not-run (ws_g05) | s_0046ef70 | IDENTICAL-FOR-600 | 10 | 0 |
| 0x004cf5a0 | not-run (ws_g05) | s_004cf5a0 | IDENTICAL-FOR-600 | 10 | 0 |
| 0x004f0f10 | not-run (ws_g05) | s_004f0f10 | IDENTICAL-FOR-600 | 10 | 0 |
| 0x00408b00 | not-run (ws_g05) | s_00408b00 | IDENTICAL-FOR-600 | 10 | 0 |
| 0x00477be0 | not-run (ws_g05) | s_00477be0 | IDENTICAL-FOR-600 | 10 | 0 |
| 0x004e8c70 | not-run (ws_g05) | s_004e8c70 | IDENTICAL-FOR-600 | 10 | 0 |
| 0x004f3e90 | not-run (ws_g05) | s_004f3e90 | IDENTICAL-FOR-600 | 10 | 0 |
| 0x004e7060 | not-run (ws_g05) | s_004e7060 | IDENTICAL-FOR-600 | 10 | 0 |
| 0x004e9330 | not-run (ws_g05) | s_004e9330 | IDENTICAL-FOR-600 | 10 | 0 |

Write-set decode of these runs: not run yet (step 5). Each of these bodies was IDENTICAL-FOR-600 in its PROV-INSITU-REF1
verdict run, where nothing was stepped. `0x00420050`, `0x00448220` and `0x00445aa0` diverge only when single-stepped.

## Splits so far (stepped, `--prov-wlog 10`)

- ws_g01 repeat (`ws_g01_rep`): DIVERGED@300, block 0x692400. That is the same first tick and block as the PROV-INSITU-REF1 run.
- Half a (10): DIVERGED@300, 0x692400 -> aa (5) DIVERGED@300 -> aab (3) DIVERGED@300 -> aabb (2: `0x00564040`,
  `0x00420050`) DIVERGED@300, 0x692400. aaba (1: `0x004c2a30`) is IDENTICAL. The single `s_00420050` gives the same tick and
  block, so the member is `0x00420050`.
- Half b (10): DIVERGED@302, block 0x896400 -> ba (5) DIVERGED@302 -> baa (2) DIVERGED@302 -> baab (1: `0x00407e20`) DIVERGED@302,
  0x896400, so the second member is `0x00407e20`. baaa (1: `0x00546e70`) is IDENTICAL. bab (3: `0x004b42c0`, `0x005b7980`, `0x00468980`) DIVERGED@535 in `0x7d5000`, `0x7d6000`, with its split not finished.
  Member lists per label are in `ws2_state.jsonl`.
- So ws_g01 holds two stepping-sensitive members, one per half, and a third set (bab) diverges later, at 535.
- ws_g03 and ws_g05: not split yet. Their single-member evidence so far: `0x00448220` (ws_g03) alone gives ws_g03's
  tick and block (302, 0x896400). `0x00445aa0` (ws_g05) alone gives 302 / 0x896400 too.

## No-step controls (`--prov-wlog 0`)

- `nw_ws_g01`, `nw_ws_g03`, `nw_ws_g05` (20 bodies each) and `nw_s_00420050` (1 body) all report DIVERGED@0, block
  0x00912800. The cause is not a body, and these runs are not usable as controls. At tick 1 (checkpoint k=201) six
  dwords differ: 0x00912b34 / 0x00912b44 / 0x00912b54 / 0x00912bb4 / 0x00912bc4 / 0x00912bd4 read 0 against ref1's
  4 / 0x1b / 0x3e (twice). The stepped runs `s_00420050` and ws_g02 (PROV-INSITU-REF1), and ref2, equal ref1 there.
  [UNCERTAIN] Why a run with `--prov-wlog 0` leaves these dwords 0 at tick 1. The PROV-INSITU-REF1 verdict runs had
  no write log either and were IDENTICAL. Test: a no-body run with `--prov-wlog 0` against one with the option left
  out (register line added).

## DECISION text for the lead (not decided here)

1. **Own argument slots.** Should a store to the body's own incoming parameter slots (esp/ebp-relative, base_esp+4 ..
   base_esp+4*nargs) count as frame for the write-set leg? Options: (a) keep it caller-stack (L-40, today's rule,
   0 bodies move); (b) count it as frame only when a static scan shows no caller reads the slot after the call
   (up to 11 bodies with an image store become cut candidates, after re-stepping in an IDENTICAL run). Evidence:
   `WS2_CLASSES.tsv`, `esc_detail_ref1groups.json`. Recommendation: (b) behind the caller scan, as its own tool row.
2. **Stepping-sensitive bodies.** For `0x00420050`, `0x00448220`, `0x00445aa0` and `0x00407e20`, every stepped run so far diverges, while
   the unstepped verdict run is IDENTICAL. The cut rule needs an IDENTICAL stepped run, so these cannot pass under
   today's rule. Options: (a) keep the rule; (b) accept the stepped stores of calls that end before the first
   diverging tick. Recommendation: (a), plus a harness row to find what the single-step changes. [UNCERTAIN] The
   mechanism: no test was run. Candidates to test: a read of stack memory below esp, which the trap frame
   overwrites, or x87/SSE state across the trap.

## Evidence and raw data

- Tools in this folder: `ws2_arm.py`, `ws2_driver.py`, `ws2_classes.py`, `ws2_table.py`. Tables: `WS2_CLASSES.tsv`,
  `ws2_STRIDE_PLAN.tsv`, `ws2_wstride.json`.
- Raw: `D:/MashedWT/PROV-INSITU-REF1-WS2_data`. The run folders, `ws2_state.jsonl`, `esc_detail_ref1groups.json`,
  `lockstatus.log` and the `driver_*.log` files are there. SHA-256 of the state, detail and lock files, and of each
  run's `ticks.bin` / `wlog.bin` / `meta.json` / `prov_install.json`, are in `WS2_RAW_SHA256.txt`.
