# Scenario-attach promotion lane (race-state diffs) — scoped 2026-06-12

Pivot (A) from the batch_ah post-mortem (menu-attach ceiling, 8/48), now
scoped with a working probe. The lane re-runs C2→C3 synthetic diffs at a
state where the candidate's inputs are actually populated: **attached inside
a live Quick Battle race** instead of at the menu.

## Why

The dominant menu-attach blocker is live-state degeneracy: the arrays the
candidates read are zero before a race, so both sides trivially agree
(false-GREEN — now caught by `run_diff.py`'s INCONCLUSIVE-DEGENERATE verdict,
see `DEGENERATE_GREEN_AUDIT_2026-06-12.md`). Probe measurement (below): 2/3
of the candidate-cited globals are zero at menu.

## Machinery (all pre-existing, proven)

- **Input-override**: `FUN_00497310` return-override (4=confirm, 11=up,
  12=down) — OS-input free, works under Frida spawn. From
  `statenav.py`/`parity_shots.py`/`race_refs.py`.
- **State polling**: `DAT_0067e9f8` (menu-stack depth), `DAT_0067eca4`
  (phase; 3=menu, leaves 3 when the race loads), cursor write at
  `DAT_0067ed80 + (depth-1)*0x40`.
- **Drive recipe** (verified 2026-06-10 in `race_refs.py`, re-verified by the
  probe): title → confirm to depth 3 → `setsel(1)` (Quick Battle on a fresh
  save) → confirm through car/track setup → press confirm until `phase != 3`.
  ~20 s wall clock to in-race.

## Probe results (2026-06-12, `re/frida/scenario_attach_probe.py`)

| Measurement | Value |
|---|---:|
| Candidate C2 rows (C2 + state-rich subsystem + cites `DAT_*`) | 192 |
| Distinct cited globals probed (u32 read at both states) | 216 |
| Globals **zero at menu** | **144 (67%)** |
| Of those, **non-zero in race** | 54 |
| Candidate rows with ≥1 zero→non-zero flip | **66 / 192 (34%)** |

Per-address values and the unlocked-RVA list: committed snapshot
`re/analysis/scenario_attach_probe_2026-06-12.json` (live re-runs write to
`log/scenario_attach_probe.json`, gitignored). The probe is read-only on game
state (snapshots only).

34% is a **lower bound** — the probe reads only the first dword of each cited
global; arrays that populate beyond their first element (per-slot strides
like the `0x4e`-stride score block at `DAT_00899a40`) undercount.

## Diff-lane design

`hooks.csv` already has a `scenario` column — use it. A registry hook entry
gains `'scenario': 'race'`; `run_diff.py` (or a thin wrapper) performs the
drive recipe between attach and the path1 vector loop:

1. Spawn + attach exactly as today (`MASHED_RE_NO_AUTO_HOOK=1`).
2. Load the diff agent AND the nav agent (they are independent scripts; the
   nav agent only hooks `FUN_00497310`).
3. Wait for menu → drive to race → wait `phase != 3` plus ~4 s.
4. Run the standard path1 A/B vectors on the populated state.
5. The non-degeneracy assertion stays active: if a hook still observes only
   zeros in-race, the run is INCONCLUSIVE there too — that's signal, not
   noise (wrong scenario or wrong globals).

The on-game-thread A/B pattern (nav-driven harness, `menu_anim_diff.py`
family) composes with this for functions that must run on the game thread:
same drive, then `replaceFast` A/B at race state.

## Batch guidance (per the 2026-06-12 sizing policy)

- First batch: 2×5 drawn **only from the 66 confirmed-unlocked RVAs**
  (`log/scenario_attach_probe.json` → `unlocked_rvas`), predicted yield well
  above the ~30% gate.
- The 184 residual suspects in `DEGENERATE_GREEN_AUDIT_2026-06-12.md` overlap
  heavily with this lane's constituency — re-validating them here clears two
  debts at once (their C3 evidence becomes real, and the degenerate-GREEN
  ledger shrinks).
- Caveats: the drive assumes a fresh-save cursor layout (Quick Battle at
  selection index 1); per-run race values are nondeterministic, which is fine
  for A/B (both sides observe the same state in the same run) but means CSVs
  are not comparable across runs.

## Next steps

1. ~~Implement `scenario: 'race'` support in `run_diff.py`~~ **DONE
   2026-06-12**: `drive_to_race()` runs between attach and the vector loop
   when a registry entry sets `'scenario': 'race'`; nav agent shared at
   `re/frida/nav_agent.js`. Exit 6 = failed to reach race state. Validated
   live on `is_car_slot_active` (0x0040e370): its old all-early-out vectors
   were degenerate; with in-bounds slots 0..3 in-race it returns 1 for the
   four live cars, 10/10 GREEN non-trivial.
2. Emit the first 2×5 batch from the unlocked-66 list via `promote-c3-batch`
   (the skill's sizing policy section already points here).
3. After the first GREEN batch, extend the probe to stride-aware reads for a
   tighter unlocked count.
4. A/B time-skew caveat for vector choice: prefer discretely-changing state
   (slot flags, lap counts, gate indices) over per-frame floats (lap
   fraction, positions) — the original and reimpl calls are not atomic
   against the running game thread.
