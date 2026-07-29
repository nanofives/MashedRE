# C2→C3 throughput — diagnosis + lane rebuild (2026-07-28)

**Question asked:** "is there a quicker way we can pan out C2→C3 promotions in batches?
currently each batch is giving me one or two promotions and it's very time consuming."

**Answer:** the 1–2/batch rate was never a candidate-supply problem. It was one boot per
live-state hook against a ~15-boot GPU wedge budget, plus a filter that certified
un-promotable rows. Both are now fixed. Commits: `6732d94d`, `ce439d99`, `977ded4a`,
`01de89f2`, `2d220b26`, `24fb2b69`, `c1a22634`.

---

## The diagnosis chain (and three wrong turns, recorded so they aren't repeated)

Lifetime rate is **1.67 GREEN/round over 248 rounds** (415 GREEN). Four hypotheses were
tested in order; the first three were WRONG:

| # | hypothesis | verdict |
|---|---|---|
| 1 | Pool too narrow (frontier = 19 leaf rows) | **WRONG** — widening was already done 2026-07-24: `callee_gate_cascade.tsv` holds **1416** callee-ready rows, and **0 had been consumed** in the four days since. |
| 2 | Filters too weak | **PARTLY RIGHT** — real defects found and one fixed, but not the binding constraint. |
| 3 | Need an arg_type resolver | **WRONG** — `promote_classify.py` already IS one; it was hardcoded to the 19-row leaf frontier. Pointed at the real pool it resolves **3/440 render, 0/1416 backlog** (its templates are display-independent LEAF shapes). |
| 4 | **One boot per hook** | **RIGHT.** `run_diff_scenario.py` usage is `<hook_name>` — singular. 947 of 1416 backlog rows classify STATE and need that lane. ~15 boots/session × 1 hook = the observed rate. |

Also disproved en route: a "gate-buster" list of 6 C1 RenderWare callees (RpClumpDestroy et al.)
would unblock nothing — they are third-party and stay C1 by design. `callee_gate_cascade_2026-07-24.md`
had already made and corrected that exact error. **There are 0 first-party rows below C2 in
hooks.csv**; all 797 C1 rows are third-party library.

---

## Fixed

### 1. `c3_filter_v4.py` live-state detector (`6732d94d`)
Knew only COM/Win32 names (IDirect3D*, DialogBoxParam, fopen). Did not recognise `DAT_007d3ff8`
— the RwEngine instance pointer, non-null only after RwEngineInit/Open — so functions dispatching
through the live device vtable were certified promotable.

- render: `live_state_side_effect` **42 → 138**, passed **530 → 440**.
- DEREF/BASE shapes only (`*DAT_007d3ff8`, `DAT_007d3ff8 + 0x`), never bare mentions: deref-or-base
  fires on 90/530, bare mention on 123. 6-row random sample of the 90 dropped: **0 false positives**.

### 2. `promote_classify.py --input` (`ce439d99`)
The resolver can now be pointed at any TSV with an `rva` column. Default path unchanged.

### 3. Many-hooks-per-boot lane (`977ded4a`, `01de89f2`)
`re/frida/run_diff_scenario_batch.py` = one spawn + `run_diff_scenario`'s `drive_to_results`
navigation + the many-hooks loop `run_diff_warm` already proved.

- **48 force-calls in 1.2s** of in-race window, 48/48 GREEN (~25ms each). The window is NOT the cap.
- State reuse confirmed for integer getters via `--repeat-first`.
- Ports the ZERO-ARG baseline criterion, so a 0-mismatch run returning the menu default is
  INCONCLUSIVE, never GREEN. Without it all four control probes "pass" vacuously.

### 4. x87 dirty-stack false-RED (`2d220b26` diagnosis, `24fb2b69` fix)
A float hook batched after a *specific* polluter false-REDs with **exactly 1 mismatch, always
vector idx=0** (original side returns empty / garbage = NaN/indefinite, clean from vector 1 on).

- First diagnosis ("any preceding hook") was WRONG — heading_atan2 + audio_vec_length alternating
  3× with no scrub = 6/6 GREEN. The polluter was `camera_path_all_nodes_eq2` specifically.
- Fix = **CW-PRESERVING** scrub between hooks: save CW → FNINIT → restore CW. A bare FNINIT resets
  the control word to 0x037F and would trade a dirty-stack false-RED for a *rounding* false-RED.
- A/B, identical ordering: scrub OFF **1/5** GREEN → scrub ON **3/5**, polluter's own RED 8/8
  unchanged at both positions.

### 5. One promotion (`c1a22634`)
`0x004233e0 HeadingAtan2ToGameAngle` C2→C3. Not newly authored — parked BLOCKED-ON-ENV since
branch `c3/batch-p2w2-s1`; the port was finished and only evidence was missing. 16/16 bit-identical,
16 distinct originals across all four sign quadrants. C2 4034→4033, C3 849→850.
**It had read RED 1/16 earlier the same day purely from the x87 harness bug** — trusting that RED
would have filed a correct port as defective.

---

## Open work, ranked

1. **Wire the lane into `promote-c3-batch/SKILL.md` as the STATE lane.** Deliberately NOT done
   while the x87 trap was live; the trap is now fixed, so this is unblocked. Must carry the
   ordering/scrub and quiescence caveats.
2. **3 genuine REDs** in already-registered hooks, stable across positions, unaffected by the scrub:
   `camera_path_all_nodes_eq2` (0x0047c270) 8/8, `camera_path_any_node_nonzero` (0x0047c2d0) 8/8,
   `smplfzx_stateblock_get_logged` (0x004853b0) 10/10. Real port defects.
3. **3 tracker-drift rows with non-degenerate evidence already on disk** — registered, still C2:
   `sprite_slot_dispatch` (11 distinct), `text_sprite_scaled` (10), `rtfshandler_is_eof` (2).
   Candidate free promotions. NOTE: 7 further drift rows have `distinct=1` = INCONCLUSIVE-DEGENERATE,
   NOT promotion evidence (skill rule 4).
4. **The `arg_type=unknown` hole.** All 440 filtered render rows carry `arg_type = unknown`; notes
   have no such field, so `c3_filter_v4`'s `arg_type_unsupported` check is structurally vacuous —
   it reports 0 eliminations because it can never fire. This is the gate the ledger shows kills the
   most candidates (⅔ per the orchestrator's measurement). Unsolved.
5. **Side-effecting hook reuse** is untested. Order read-only first, control last.

## Standing gotchas for this lane

- `--scenario race` is POPULATED but **not quiescent** (`run_diff_scenario` defaults to `results`
  for that reason) → a per-frame-varying getter can false-RED on frame advance between the A and B
  calls. A *stable* RED across repeats is not that artefact.
- The runner's sentinel-delta column is **confounded** — in a running race live globals mutate every
  frame regardless of our calls, so it reports CHANGED unconditionally. `--repeat-first` is the
  trustworthy instrument.
- Navigation is flaky: one run never left the frontend for 130s and correctly aborted rather than
  emitting a false GREEN. Costs a spawn.
- Only 5 registry entries carry `scenario_sentinel`; multi-vector controls were driven by a CLI
  `--sentinel` never persisted. Pass `--sentinel 0xADDR` or add the field.
- Spawn budget: ~12 boots used this session against the ~15 wedge threshold. Watch it.
