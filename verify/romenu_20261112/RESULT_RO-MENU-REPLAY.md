# RESULT RO-MENU-REPLAY (2026-11-12)

Branch `orch/RO-MENU-REPLAY`, worktree `.worktrees/RO-MENU-REPLAY`. Data: `D:/MashedWT/RO-MENU-REPLAY_data`.
The handoff before the LIVING-PC reboot is `HANDOFF_RO-MENU-REPLAY.md` (commit `06bc290de`).

## Verdict

The replay oracle now runs in boot mode. It hashes every tick from the first main-loop tick, through 600 menu ticks,
then warps into the race (2724 race ticks, 3324 in total). There are two references per environment class:
- pre-reboot r1 / r2: IDENTICAL-FOR-N 3324 of 3324
- post-reboot r3 / r4: IDENTICAL-FOR-N 3324 of 3324

Both pairs are strict, with 0 allowed blocks.

Of the 113 PROV-INSITU NOT-EXECUTED bodies, 24 run in this scenario. 89 still do not run.

| Verdict | Bodies | Runs |
|---|---|---|
| IDENTICAL-FOR-3324 | 19 | 1 class-matched run each (see the table) |
| DIVERGED | 2: `0x0043dfd0` at menu tick 1, `0x0043d2a0` at menu tick 16 | 2 of 2 each, same tick and same blocks |
| CRASH in boot | 3: `0x0045d460`, `0x0048e820`, `0x00499400` | 3 of 3 each, AV `0xc0000005` before the first tick |

No build input changed (tooling and data only), so no builds, no `ddkobj` check and no gates were run.

## Self-tests

- References: r1 vs r2 and r3 vs r4 are both IDENTICAL-FOR-N 3324 of 3324, strict.
- provlink: `re/tools/prov/provlink_selftest.py` gives ok=40 bad=0 skip=0. It was re-run after the reboot.
- Negative control in the menu, `negm2` (`--negctl 0x0067f17c --neg-tick 300`): DIVERGED at tick **300** exactly,
  block `0x0067f000`. PASS.
  - The first menu control, `negm` (`--negctl 1 --neg-tick 300`, the RW RNG word at the cursor + 1), stays
    IDENTICAL-FOR-N 3324. The warp at tick 600 pins `rw_ring`, `rw_cursors` and `holdrand` (the `pins` line of every
    run log), so a menu-time bump of the RNG word is overwritten before any compared state reads it.
  - **Rule:** a menu-segment negative control must target a dword that the warp does not pin. The RNG form is valid
    only at the race anchor, as in PROV-INSITU.
- Merge check: `orch/integration` (PROV-CRASH-12) merged with no conflict (git auto-merged `re/frida/detorig.py`
  and `re/frida/js/detorig.js`). Checks after the merge:
  - `py -3.12 -m py_compile` passes on `detorig.py` and `prov_compare.py`
  - `node --check` passes on `js/detorig.js`
  - the merged `--crash-catch` works in boot mode: the three CRASH bodies, below

## Post-reboot environment findings (tooling changes in this branch)

1. **`0x0077398c` is OS-assigned.** The store at `0x004a173a` in FUN_004a1570 writes the result of
   `RegisterWindowMessageA` (import slot `0x5cc244`, call at `0x004a1732`). The store is guarded by the `== 0` test
   at `0x004a1724`, and `0x004a1754` reads it.
   - Whether it is still 0 at boot tick 0 varies by run: r3 has `0x0000c1fc`, r4 has `0`.
   - Added to `re/frida/warp_normalize.tsv` as `MASK:os` (compare-only, no pin).
2. **Boot classes.** Unmodified boot runs fall into classes that stay apart for the whole run. All boot runs with ticks were
   checked (every run with ticks: r1..r5, negm, negm2, the m_ and rep_ runs).
   - **Timer phase:** the tick-0 value of `0x007f1030` in the frame-timer block takes one of 4 values (`39a80501`,
     `39a82829`, `39a84b51`, `39a8b4c9`). `0x007f1010`, `0x007f102c` and `0x007f1030` then differ at every tick, not
     only at the race's first tick as the PROV-INSITU TIMER-PHASE rule covers.
     - No-body r5 (class `39a82829`) against r3 / r4 (class `39a80501`), with those three dwords masked, diverges at
       race tick 1752 in `0x00618000` (11 float dwords `0x00618258..0x00618290`).
     - So a class mismatch is a real state difference, not a compare artefact.
   - **`0x006147d8`** (MASK:os, window-message path flag): it is 0 in r1 / r2 and 1 in every post-reboot run checked.
     A run with 1 scored against references with 0 differs from tick 0 in `0x00773c00` and `0x00911c00`.
   - `re/frida/prov_compare.py` now records `boot_class` / `boot_class_match` and adds a note on a mismatch. It does
     not change the verdict.
   - **Rule:** a mismatched run is not body evidence. Re-run it, or score it against a no-body reference of its own
     class.
3. **Voided runs.** The 8 bisect verdicts first written after the reboot were all `DIVERGED 0`. They were scored
   against the pre-reboot r1 / r2, and a no-body control (r3) gave the same tick-0 difference.
   - They were moved out of `m_state.jsonl` into `m_state_postreboot_invalid.jsonl`.
   - They were re-scored against same-class references (`final_verdicts.json`).

## Per-body table (24 executed)

Calls are counted per segment (menu = before the warp, race = after). Ref is the reference pair the verdict was
scored against.

| RVA | calls menu | calls race | verdict | run(s) | ref |
|---|---|---|---|---|---|
| `0x00429bd0` | 0 | 18 | IDENTICAL-FOR-3324 | m_090404_001_4 | r1/r2 |
| `0x00433f40` | 600 | 0 | IDENTICAL-FOR-3324 | m_090404_001_4 | r1/r2 |
| `0x004627f0` | 1 | 0 | IDENTICAL-FOR-3324 | m_090404_001_4 | r1/r2 |
| `0x00473c20` | 584 | 0 | IDENTICAL-FOR-3324 | m_090404_001_4 | r1/r2 |
| `0x004d4f00` | 4 | 0 | IDENTICAL-FOR-3324 | m_090404_004_1 | r1/r2 |
| `0x00475a00` | 2 | 0 | IDENTICAL-FOR-3324 | m_090404_007_1 | r1/r2 |
| `0x004938e0` | 649 | 0 | IDENTICAL-FOR-3324 | m_090404_010_2 | r1/r2 |
| `0x00493fd0` | 50 | 0 | IDENTICAL-FOR-3324 | m_090404_010_2 | r1/r2 |
| `0x00555360` | 1 | 0 | IDENTICAL-FOR-3324 | m_090404_013_1 | r1/r2 |
| `0x005a6b70` | 29 | 0 | IDENTICAL-FOR-3324 | m_090404_015_2 | r1/r2 |
| `0x004022a0` | 78 | 0 | IDENTICAL-FOR-3324 | m_090404_015_2 | r1/r2 |
| `0x0042e5b0` | 584 | 0 | IDENTICAL-FOR-3324 | m_090404_017_1 | r1/r2 |
| `0x0045c480` | 600 | 0 | IDENTICAL-FOR-3324 | rep_0x0045c480 | r3/r4 |
| `0x004caea0` | 1 | 0 | IDENTICAL-FOR-3324 | m_124434_006_1 | r3/r4 |
| `0x004cc050` | 16 | 0 | IDENTICAL-FOR-3324 | rep_0x004cc050 | r3/r4 |
| `0x00401fe0` | 52 | 0 | IDENTICAL-FOR-3324 | m_124434_010_1 | r3/r4 |
| `0x00474890` | 654 | 0 | IDENTICAL-FOR-3324 | m_124434_013_1 | r3/r4 |
| `0x005a71f0` | 1 | 0 | IDENTICAL-FOR-3324 | m_124434_014_1 | r3/r4 |
| `0x004427c0` | 0 | 19 | IDENTICAL-FOR-3324 | rep_0x004427c0 (class `39a82829`) | r5 |
| `0x0043dfd0` | 600 | 0 | DIVERGED@1 menu, blocks `0x0067ec00` + `0x007d6000` | m_124434_011_1, rep_0x0043dfd0 (class `39a82829`) | r3/r4, r5 |
| `0x0043d2a0` | 1 | 0 | DIVERGED@16 menu, block `0x00898800` | m_090404_018_1, rep_0x0043d2a0 | r1/r2, r3/r4 |
| `0x0045d460` | - | - | CRASH: read of `0x0` at EIP `0x004cbd3a` (MASHED.exe+0xcbd3a), clock call N=18 | m_090404_005_1, rep_, cc_ | - |
| `0x0048e820` | - | - | CRASH: write of `0xe0722470` at EIP `0x004b6506` (MASHED.exe+0xb6506), N=142 | m_090404_008_1, rep_, cc_ | - |
| `0x00499400` | - | - | CRASH: read of `0x0` inside the provisional body (body RVA `0x00499400` + `0x5d`), N=2 | m_090404_012_1, rep_, cc_ | - |

The DIVERGED and CRASH verdicts were each repeated, and every repeat gave the same result. The IDENTICAL verdicts are
single class-matched runs (handoff step 2 asked for repeats of CRASH and DIVERGED only).

The class-mismatched runs `m_124434_003_1`, `_004_1` and `_007_1` (classes `39a84b51` / `39a8b4c9`) are not used.
Their bodies are covered by the repeats above.

`[UNCERTAIN]` Not tested here:
- which reader of the two DIVERGED blocks moves first
- whether the CRASH faults are caused by the body's own arguments or by its callers' state

The evidence missing for both is a write-watch on the first diverging dword (`--watch-write`) and the `crash.jsonl`
stacks under `cc_*`.

## Files

- `final_verdicts.json`: prov_compare output for every scored run above.
- `raw_sha256.txt`: SHA-256 of every raw file used: 21 run folders (r1..r5, negm, negm2, the scored runs, the reps,
  the cc_ runs), plus `m_state.jsonl`, `m_state_postreboot_invalid.jsonl`, `final_verdicts.json` and
  `census/prov.json`.
- `run_arm.sh`: the arm runner (`WT=600 CAP=4000 TMO=700`).
- Repeat batch script: `D:/MashedWT/RO-MENU-REPLAY_data/confirm_batch.sh`. Its log is `confirm_batch.log`.
