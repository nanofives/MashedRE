# PREREG D4-WSB-C4-SOLV-WS: SOLV write-set C4 (8 rows)

Committed alone, before any arm. Branch `orch/D4-WSB-C4-SOLV-WS` off `orch/integration` @ `6368c086a`
(contains D4-WSB-SHADOW-PRIVATE and the L38 demote).

## Rows (all C3 impl in `hooks.csv` at `6368c086a`)

| row | wrapper before | wrapper on this branch | file |
|---|---|---|---|
| `0x00568990` | `ShadowAB::Run` | `RunTracked` (`eb72bc0b6`) | `Collision/RwpSolverCore21.cpp` |
| `0x00568dd0` | `Run` | `RunTracked` (`eb72bc0b6`) | `Collision/RwpSolverCore11.cpp` |
| `0x00568fd0` | `Run` | `RunTracked` (`eb72bc0b6`) | `Collision/RwpSolverCore11.cpp` |
| `0x0056b7a0` | `Run` | `RunTracked` (`eb72bc0b6`) | `Collision/RwpSolverCore18.cpp` |
| `0x0056ba30` | `Run` | `RunTracked` (`eb72bc0b6`) | `Collision/RwpSolverCore21.cpp` |
| `0x00564c80` | `RunTracked` | unchanged | `Collision/RwpSolverCore4.cpp` |
| `0x00561390` | `RunTracked` | unchanged | `Collision/RwpSolverCore23.cpp` |
| `0x00563e70` | `RunTracked` | unchanged | `Collision/RwpSolverLeaves1.cpp` |

`eb72bc0b6` changes the 5 wrapper calls and adds `#include "../Core/ShadowTrack.h"` to Core18 / Core21. Unarmed,
`Run` and `RunTracked` both call `_impl`, so the default path is unchanged. No change to `Core/ShadowTrack.h` or
`Core/ShadowAB.h` (shared headers).

Build (`MASHED_NO_DEPLOY=1`): `mashed_re.exe` `f0cbcec50a020919c270da0699abf8ea737f4d533a03b20d6b5e006f188ac750`,
`mashed_re_dev.asi` `5c12899c1f32f8a49cbba8bc28faea69c15bd56d33f4d4b07877b877b4fb931e`, copied to the private root
`D:/MashedWT/D4-WSB-C4-SOLV-WS_root/original/` only. `original/mashed_re_dev.asi` untouched.

## Leg H: heap-tracked write-set (one boot per row)

`run_shadow4.py <rva>` (copy of `verify/wsbshadowpriv_20261027/run_shadow3.py`, data path and lock label changed),
env `MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1 SHADOW_TAG=heap`, `MASHED_SHADOW_AB=1` set by the driver, census
scenario (`--track 0 --mode 10 --cars 4 --hold 60 --hooks 0x<rva>`), the hook installed alone, 24 samples then the
driver kills its own game PID. Per-sample read: `pages=o:..,n:..`, `heap=o:..,n:..`, `stack`, `noise`, `ovf`, `ret`.

Read-out per row (from `shadow_ab_report.py` + the per-sample `heap=` fields):

- **HEAP-CLEAN**: verdict CLEAN, n >= 1, ndiff 0, proof `A/B-IS-REAL installed=e9`, no NOISY / UNTRACKED / OTHER-EXC
  line for the row, and heap pages compared summed over the samples > 0. This is the only leg-H result that
  allows C4.
- **CLEAN-IMAGE-ONLY**: CLEAN as above, heap pages 0, image pages > 0. Stays C3 (no heap page compared), reason in the
  RESULT.
- **CLEAN-NO-PAGES**: CLEAN, 0 pages of any kind. Stays C3. If NEED_PAGES is needed to tell "no writes sampled" from
  a harness gap, that arm gets an addendum first.
- **DIVERGENT**: any ndiff > 0. Stays C3. The RESULT reports `@page+off ndw v=pre/orig/port`. For `0x00564c80` the
  BVH-FU read was +1 / +1 at `0x007dc8c8` / `0x007dc8cc` with image-only tracking. If heap tracking makes it CLEAN,
  the +1 / +1 was the restore gap BVH-FU named. If it stays +1 / +1, it is filed as a U-n (port vs original count
  difference in `0x005646c0`'s pair loop), not cleared.
- **NO_LOG / hang**: re-run once. Two NO_LOG = BLOCKED for that row (stays C3).

A piz-collision crash (`MASHED.exe+0x1509b4`, `+0xe66db`, U-9677) is a re-run, not a result.

## Leg R: canonical-scenario racediff (two boots per row, hook installed alone on the ON boot)

`run_bvh4.py <rva>` with tool `c4_racediff_v7.py` (unchanged copy of `verify/wsbbvhfu_20261027/c4_racediff_v7.py`),
OFF = `--hooks __none__`, ON = `MASHED_HOOK_ONLY=0x<rva>`.

- Quick Battle window first (tool default) for the 5 SOLV rows, `0x00561390` and `0x00564c80` (both were C4-CLEAN in
  Quick Battle on the BVH-FU exe). Census (`C4_SCENARIO=census`) for `0x00563e70` (its BVH-FU leg).
- If a Quick Battle run captures 0 calls on both sides (after the driver's one automatic re-run), that row runs the
  census scenario instead.
- Pass = verdict `C4-CLEAN` with `installed=True` (byte `0xE9` at the RVA on ON) and > 0 calls on both sides.
- Any other verdict with calls captured = stays C3, `why` recorded. 0 calls in both scenarios = stays C3 (no scenario).

## Promotion rule (per row, through re-classify only)

C3 -> C4 iff leg H = HEAP-CLEAN **and** leg R = C4-CLEAN, both on this branch's build, **and** the default gates
match. Every other combination stays C3 with the reason in the RESULT. A new `[UNCERTAIN]` gets a U-n.

## Default gates (once, on this build)

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT=D:/MashedWT/D4-WSB-C4-SOLV-WS_root`,
under the test slot and `mashed_lock.py run --label D4-WSB-C4-SOLV-WS`. Must equal `re/orchestrator/merge_train.json`:
G-CTRL `5943c5f1/364350ae/52052890`, B `3a03670c/e85ea6fb/53778664`. A mismatch = RED, no promotion.

## Process

Every boot under `mashed_lock.py run` and the test slot. Only PIDs this step spawns are killed. Raw data in
`D:/MashedWT/D4-WSB-C4-SOLV-WS_data/`; committed: this file, the drivers, `shadow_summary.tsv`,
`racediff_summary.tsv`, the RESULT, the re-classify script.
