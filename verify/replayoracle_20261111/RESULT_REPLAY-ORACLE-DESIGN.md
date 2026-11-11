# RESULT REPLAY-ORACLE-DESIGN: is an exact race-replay oracle feasible

Date 2026-11-11. Worker session cmwbtmppz2d14sc1bn329o991 (claude2), parent cmwbssxl22bg4sc1ba3ghf7dh.
Branch `orch/REPLAY-ORACLE-DESIGN` from `orch/integration` `ee3562e27`. Design: `DESIGN_REPLAY-ORACLE.md` (same folder).
Review: **TEXT-ONLY** (harness under `re/frida/`, probes and reports under `verify/replayoracle_20261111/`, no build input).

## Verdict

**Feasible, with conditions.** The original side of the oracle is exact today. The exe side cannot be measured yet,
because the ORIG_TOP path does not reach a race.

1. **The original replays itself exactly.** A "tick clock" was added to the existing `detorig` harness: every
   main-thread clock read returns a value that depends only on the frame count. With it, plus the existing RNG pins
   and scripted input, two runs of the menu-less race (track 0, mode 10, player + 3 AI) match:
   - at every one of **2,724 of 2,724 ticks** (about 45 s of race), compared per 1 KB block
   - in **6 of 6 pairs**
   - outside three named spots: the window-message flags `0x006147d8` / `0x00771a00` and the audio blocks
     `0x007dc400..0x007dd400`
   - at all 15 full-image checkpoints, race state and the normalised whole image are identical (15/15) in the same
     6 pairs
2. **The tick clock removes the load-length blocker.** The track load still took 118 or 119 polls (disk time), and
   one run made a different number of audio-clock calls per frame from tick 8 on. Pairs across both kinds of
   difference were identical. Earlier work (PROMOTE-DETORIG-PILOT / -LOADHOLD) needed equal load lengths and a 50 ms
   load hold. This rule needs neither, and it is a rule the exe can implement: no recording, no call counting.
3. **The harness sees a real change.** A +1 on one RenderWare RNG word at the race anchor first shows at tick 7 in
   the per-tick compare. It reaches the car records at tick 1,050, and RACE-STATE is 0/15.
4. **Conditions for the exe side**, each a row below:
   - the ORIG_TOP path reaches the race (PORT-OT-RACE)
   - the clock, tick, RNG and input bindings sit behind a default-OFF knob
   - a per-dword mask for the families the exe legitimately replaces (CRT, RenderWare core, D3D/d3dx9, audio,
     DirectShow, Lua, qhull), plus the OS-filled words and the stale-stack ring fields
   - bit-identical float code (DDKOBJ-SPIKE). Without it, only the integer, state and RNG parts stay exact and float
     state is scored (DESIGN section 6).

## The measurement

Runs: 13 launches of `MASHED.exe` from the private root `D:\MashedWT\REPLAY-ORACLE-DESIGN_root` (robocopy of
`original\`). Each launch was inside `mashed_lock.py run --label REPLAY-ORACLE-DESIGN` with `test-slot` outside, and
only the PIDs `detorig.py` spawned were killed.
- 12 runs completed with 15 checkpoints each.
- `t_a` failed to attach within 20 s and was not re-run (batch 2 supersedes it).
- 0 crashes, 0 piz collisions.
- Binaries: `MASHED.exe` `b9977dab...`, `d3d9.dll` `47901951...`, `mashed_qol.asi` `1567c95b...`, `mashed_re_dev.asi`
  `906f8601...` (not hooking: `MASHED_RE_NO_AUTO_HOOK=1`), `dinput8.dll` `5f850f29...`. These are the same files as
  the LOADHOLD root. No change to `original\`.

Each run: menu, install, warp at N = 600, race anchor at the first clock call that sees phase 3. Then 30,000
main-thread clock calls with:
- per-tick hashes at the entry of `FUN_00493390`
- 15 full-image checkpoints every 2,000 calls
- the LOADHOLD input script (accelerate from 0, steer pulses)

### Batch 2: the final configuration (`batch2.sh`)

Settings:
- `--clock tick`: QPC advances 166,667 counts and timeGetTime / GetTickCount 17 ms only at the entry of `FUN_00493390`.
- Timer pin at the warp: `0x007f1030`, `0x007f102c` = the current `FUN_004950b0` value, `0x00771984 = 0`,
  `0x00771998` / `0x0077199c` = value / 3000.
- `--q0 0xC0000000`, pointer rule `PTR_LO = 0x00995000`.
- Raw per-tick copies of six blocks: `0x00603400`, `0x00614400`, `0x00618000`, `0x006dcc00..0x006dd400`, `0x00773800`,
  `0x007dd000`.

Load lengths (main-thread clock calls in phase 2): t2_a 119, t2_b 118, t2_c 119, t2_h 118 (50 ms load hold), t2_neg 118.

| pair | load calls | per-tick IDENTICAL-FOR-N (ignore list below) | NEW blocks outside the list | checkpoints NORMALIZED | RACE-STATE |
|---|---|---|---|---|---|
| t2_a vs t2_b | 119 / 118 | **2,724 / 2,724** | 0 | 15/15 | 15/15 |
| t2_a vs t2_c | 119 / 119 | **2,724 / 2,724** | 0 | 15/15 | 15/15 |
| t2_a vs t2_h | 119 / 118 | **2,724 / 2,724** | 0 | 15/15 | 15/15 |
| t2_b vs t2_c | 118 / 119 | **2,724 / 2,724** | 0 | 15/15 | 15/15 |
| t2_b vs t2_h | 118 / 118 | **2,724 / 2,724** | 0 | 15/15 | 15/15 |
| t2_c vs t2_h | 119 / 118 | **2,724 / 2,724** | 0 | 15/15 | 15/15 |
| t2_b vs t2_neg (negative control) | 118 / 118 | **7** (first NEW block `0x00613000` at tick 7) | 477 | 0/15 | 0/15 |
| t2_h vs t2_neg | 118 / 118 | **7** | 477 | 0/15 | 0/15 |

Ignore list: `0x006147d8-0x006147dc,0x00771a00-0x00771a04,0x007dc400-0x007dd400`, with `--n-ok` (a change in clock
calls per tick does not end the count). It works at block granularity, so the blocks `0x00614400`, `0x00771800`,
`0x007dc400` and `0x007dd000` are not watched once they first differ:
- In `0x00614400` the raw per-tick copy shows `0x006147d8` as the only differing word at any tick, in every pair.
- `0x00771800` differs only in t2_a vs t2_h (from tick 122). The checkpoint classes there are TOGGLE (`0x00771a00`)
  and PTR.
- `0x007dd000` differs in t2_a at ticks 2 and 3 only: 177 words that are 0 in t2_a and audio-table values in the
  others. No word of it differs at any checkpoint.

Scores: `reports/batch2_scores.txt`, `reports/batch2_identical_for_n.txt`, `reports/ticks*_t2_*.json`,
`reports/ckpt_t2_*.json`.

### Batch 1: the steps before (`batch1.sh`, smoke)

| pair | clock | load calls | NORMALIZED | RACE-STATE | note |
|---|---|---|---|---|---|
| n_a vs n_b | N clock + 50 ms load hold (LOADHOLD config) + tick hashes | 118 / 118 | 15/15 | 15/15 | reproduces LOADHOLD with per-tick resolution |
| t_b vs t_c | tick clock, no timer pin | 119 / 118 | 15/15 | 15/15 | first pair with unequal load length to match |
| t_c vs t_h | tick clock, no pin, t_h with load hold | 118 / 118 | 15/15 | 15/15 | |
| smoke vs t_c | tick clock, no pin | 118 / 118 | 2/15 | 0/15 | `0x00771984` drift and `0x007f0ffc` from the real sample left before the install. This is why the timer pin was added |
| t_b vs t_neg | tick clock, negctl | 119 / 123 | 0/15 | 0/15 | |

Batch 1 was scored with the old pointer floor `0x01000000` first. t_b vs t_c then read NORMALIZED 5/15. The unexplained
dwords listed for smoke vs t_b were heap pointers at `0x00d0xxxx` (run t_b), below that floor, plus the audio status
word `0x007dc764`. Batch 1's
committed JSONs are re-scored with `PTR_LO = 0x00995000`.

### What still differs between two runs, and why

| what | where | evidence | race state? |
|---|---|---|---|
| window-message flag | `0x006147d8` (stores `0x00493eeb`, `0x00494485`, `0x0049479a`) | raw per-tick copy; TOGGLE class | no: not in the race regions; RACE-STATE 15/15 |
| window-message flag | `0x00771a00` | TOGGLE class (LOADHOLD and here) | no |
| `MSG` of `PeekMessageA` | `0x007e95a0..0x007e95bb` | RULE:WINMSG | no |
| audio tables | `0x007dc400..0x007dd400`; status word `0x007dc764` (store `0x005505dc`) | t2_a ticks 2-3; batch 1 | no, in this scenario |
| main-thread audio-clock calls per tick | `FUN_005ab040` from `FUN_005a6ea0` | t2_a vs t2_b N per tick differs from tick 8 | no under the tick clock. Under the N clock this shifts every later clock value |
| heap pointers | 2,863 to 3,316 dwords | PTR class | no |
| boot residue | 17 to 19 RESIDUE, 4 to 6 OFFSET, 97 MODOFFSET/60 in 5 of 6 pairs (`0x00771978` + 96 dwords at stride `0x44` from `0x00868348`) | checkpoint classes | no: written before the install, unchanged or stepping alike |

### Sources that a small hook did not pin

| source | why | what it would take |
|---|---|---|
| audio thread (`__beginthread` from `FUN_005aef30`) | own thread on real time, fed by overlapped disk reads | mask it (done here), or run without the audio thread on both sides. Row RO-AUDIO-ISOLATION shows whether any reader moves race state |
| window messages | filled by Windows (`PeekMessageA` `0x0049969d`) | mask (done), or a `PeekMessageA` IAT stub returning no message on the main thread |
| boot residue | the harness installs at the menu | install at process start (ASI) on the original, and normalise the boot counters at the warp on both sides (rows RO-ORIG-ASI, RO-WARP-NORMALIZE) |
| raw key tables `0x00772ffc` / `0x00773030` | not pinned. Constant in these runs (no key pressed) | hook the poll `FUN_00495fe0` to report no device |
| heap-held race state (Lua, RW-Physics, RwFrame) | not in the static image | [UNCERTAIN] seen only through the static records it writes |

## Harness changes (this branch)

- `re/frida/js/detorig.js`:
  - `cfg.tick`: a native CModule `onEnter` on `FUN_00493390` advances the virtual clock per tick, and from the race
    anchor hashes the image per block, with optional raw ranges.
  - `pinTimer()` at the warp in tick mode.
  - the call log can run past the anchor (`callLogRace`).
- `re/frida/detorig.py`:
  - new options `--clock tick`, `--tick-hash`, `--tick-raw`, `--tick-va`, `--qtick`, `--ttick`, `--tick-cap`,
    `--tick-block`, `--q0`, `--call-log-race`
  - writes `ticks.bin`
  - `PTR_LO = 0x00995000`
- `re/frida/detorig_ticks.py` (new): per-tick compare with IDENTICAL-FOR-N, NEW blocks, raw-word first differences,
  `--ignore`, `--n-ok`, `--explain` (classes from the full dumps).
- `re/frida/detorig_report.py`: returns the per-dword class map to `detorig_ticks.py`. The `--json` output is unchanged.
- `verify/replayoracle_20261111/nondet_scan.py`, `data_owner_map.py`: static census (DESIGN sections 1 and 4).

No other `re/frida` harness was added. The tick work extends `detorig`. The registry (`hooks_registry.py`) is per
function and does not fit a whole-state replay.

## Rows needed to build the oracle (build order; not added to the ledger)

| id | title | depends on | size |
|---|---|---|---|
| RO-WARP-NORMALIZE | Normalise the boot counters at the warp on the original. Pin `0x007f0ff0`, `0x00771978`, the `0x00868320` table and the other RESIDUE / OFFSET / MODOFFSET dwords of this RESULT, then re-measure whole-image NORMALIZED with fewer classes. Defines the normalisation set the exe applies too | none | small |
| RO-BIND-TIMER | On the ORIG_TOP path, bind `0x00493480` and `0x00493390` (and `0x004926c0`) to their 100% matched bodies, so the exe has the tick boundary and the clock globals | none (overlaps PORT-OT-RACE) | small |
| RO-STACKROW-READERS | Trace the readers `0x0040bebe`, `0x0040c2d8`, `0x00422e60` of ring `0x007ec9e4` fields `+0x24..+0x2c`. Does stale stack reach race state? | none | small |
| RO-AUDIO-ISOLATION | Show that the audio blocks `0x007dc000..0x007de000` and the audio-clock call count never reach race state. Either trace their readers, or run the original with the audio thread not started on both arms and compare race state | none | small |
| RO-MASKS | Per-dword mask file for the exe-vs-original compare: bound families at address level (`data_owner_map.py` refined from pages to addresses, plus register-based struct globals from match sources), OS words, stale-stack fields, residue. Consumed by `detorig_ticks.py` and `detorig_report.py` | RO-WARP-NORMALIZE | medium |
| RO-ORIG-ASI | Move the tick clock, timer pin, RNG pins, input script and per-tick hash from Frida into a replay mode of an ASI loaded at process start on the original. Same `meta.json` / `ticks.bin` / checkpoint format | RO-WARP-NORMALIZE | medium |
| RO-EXE-REPLAY-KNOB | `MASHED_REPLAY=<replay.json>`, default OFF: `MATCH_PORT` clock wrappers, tick counter at the bound `0x00493390`, `srand` pins, `0x00534870` port with its own ring, `0x00497310` scripted input, raw poll reports no device, warp at a fixed tick. PREREG + gates unchanged | RO-BIND-TIMER, RO-WARP-NORMALIZE, PORT-OT-RACE (to reach a race) | medium |
| RO-EXE-TICK-TRACE | The same per-tick hash, raw ranges and full dumps in the exe behind `MASHED_REPLAY_TRACE`, same file format, and a `run_arms.py` arm kind `RO_<tag>` | RO-EXE-REPLAY-KNOB | small |
| RO-FIRST-DIVERGENCE | First-divergence report: block -> re-run to tick - 1 / tick -> dwords, both values, owning symbol (hooks.csv, xref facts), writer (original `detorig.py --watch`, exe in-process debug-register watch) | RO-EXE-TICK-TRACE, RO-MASKS | medium |
| RO-ORACLE-GATE | Run the oracle as the PORT-TOP-FLIP check: reference run of the original, exe run with the knob, verdict IDENTICAL-FOR-N or first divergence | RO-FIRST-DIVERGENCE, PORT-OT-RACE | small |
| RO-TOLERANCE | Only if DDKOBJ-SPIKE fails: integer / state / RNG / N exact, floats scored by ULP and per-field bands (DESIGN section 6) | RO-EXE-TICK-TRACE, DDKOBJ-SPIKE verdict | medium |
| RO-INPUT-RECORD | Optional: log `FUN_00497310` player-0 values per tick on the original to turn a human-played sequence into a `replay.json` input list | none | small |

The first four have no dependency and can run in parallel now. The exe-side rows wait for PORT-OT-RACE: the knob
can be built before then, but there is nothing to compare until the exe races.

## Decision input for Mariano

- **What was checked:** whether the original game, run twice with the same setup, does exactly the same thing frame
  by frame in a race. It does: 2,724 frames out of 2,724 match in every pair of runs, except a few values Windows and
  the audio thread write, and none of those touch the race. A deliberate one-number change shows up after 7 frames.
  So the reference side of a replay check is ready.
- **What this buys:** today G-CTRL and B can only say "same as the hand-written path". A replay check would say "same
  as the original game, and here is the first frame and memory address where it is not". That is the check that can
  justify flipping the top-down path to default.
- **What it costs:** about 10 rows, most of them small, listed above in build order. Four can start now. The exe side
  needs the top-down path to reach a race first (PORT-OT-RACE).
- **The fork:** if DDKOBJ-SPIKE shows the original's compiled code can be linked into the exe, the check can be exact:
  any difference is a port bug. If it cannot, floats will differ from the first frame, and the check can only be exact
  on whole numbers, game states and random-number use, and score the rest within tolerances. Then it narrows where to
  look but cannot by itself prove the port right.
- **Recommendation:** run the four independent rows now (RO-WARP-NORMALIZE, RO-BIND-TIMER, RO-STACKROW-READERS,
  RO-AUDIO-ISOLATION). Decide on the exe-side rows once DDKOBJ-SPIKE reports, because its result decides between the
  exact and the tolerance build.

## Evidence

Committed here:
- `DESIGN_REPLAY-ORACLE.md`, this file
- `nondet_scan.py` / `.txt`, `data_owner_map.py` / `.tsv`
- `run_arm.sh`, `batch1.sh`, `batch2.sh`, `batch1.log`, `batch2.log`
- `reports/` (per-pair tick and checkpoint JSON, score text)
- `meta/` (each run's `meta.json`)
- `raw_sha256.txt`: SHA-256 of every raw file cited (each run's `ckptNN.data.bin`, `ticks.bin`, `calllog.bin`,
  `meta.json`, run logs)

Raw data stays in `D:/MashedWT/REPLAY-ORACLE-DESIGN_data/<run>/` (1.1 GB).

Gates: not run. The change is under `re/frida/` and `verify/`, and no build input changed, so the exe gate hashes
cannot move. No tracker edit, no `ROADMAP.md` edit, no default flip, no patch to `original/MASHED.exe`.
