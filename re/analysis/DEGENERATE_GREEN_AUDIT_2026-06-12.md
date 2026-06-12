# Degenerate-GREEN audit — 2026-06-12

Companion to the non-degeneracy assertion added to `re/frida/run_diff.py`
(2026-06-12) after the batch_ah post-mortem (menu-attach false-GREEN class).
Raw replay output: `DEGENERATE_GREEN_AUDIT_raw.txt` (regenerate any time with
`py -3.12 re\frida\replay_degeneracy_check.py` — read-only).

## What changed in the harness

1. **Non-degeneracy assertion** (`run_diff.py`): a 0-mismatch run whose every
   observed value on BOTH sides is trivial (zero / empty / all-zero
   fingerprint) now reports **INCONCLUSIVE-DEGENERATE (exit 5)** instead of
   GREEN. Exceptions:
   - the hook's `arg_type` is in `SEEDED_ARG_TYPES` (54 handlers that pre-fill
     the observed region with a non-zero sentinel or registry input — for
     them an all-zero result proves the target actively wrote; derived by
     `re/frida/audit_seeded_argtypes.py`, manually reconciled);
   - the hook sets `degenerate_ok=True` in `hooks_registry.py` (with stated
     reasoning), or the run passes `--allow-degenerate`.
2. **arg_type pre-flight** (`run_diff.py`): a registry `arg_type` with no
   handler in `diff_template.js` is now a FATAL error before spawn
   (previously fell through to a default path that could pass without
   testing anything — `feedback_worker_invented_arg_types`).

## Retroactive findings (NOT auto-applied; hooks.csv untouched)

Replaying all historical `log/diff_*.csv` through the new rule:

| Population | Count |
|---|---:|
| Past-GREEN CSVs | 503 |
| All-trivial but seeded arg_type — GREEN stands | 29 |
| No registry entry (hook renamed since the run) | 4 |
| **Residual suspects: all-trivial + observe-only arg_type** | **184 (37%)** |

Residual-suspect concentration by arg_type: `none` 83, `int_scalar` 68,
`int_pair` 9, `void` 9, the rest single digits — i.e. the no-arg / scalar-arg
lanes whose outputs come from live game arrays that are zero at menu-attach
(player scores, lap getters, menu/hud dispatchers, replay/save paths). Full
per-hook list in the raw output.

### Doubly suspect: phantom arg_types (4 hooks)

`write_global_setter` (pitch_param_set, pitch_param2_set, timer_state_set)
and `int_write_observe` (save_status_clear) exist **only in
hooks_registry.py — there is no handler in diff_template.js**. Their
historical GREENs were produced by whatever default path the template fell
into. The new pre-flight refuses these outright until a handler is added or
the registry entry is fixed.

## What this does and does not mean

- It does **not** mean 184 reimpls are wrong. It means those 184 path1 runs
  carried no discriminating power on their own. Many of the affected hooks
  hold additional evidence (path2 install verification, canonical
  observation, nav-driven diffs) — their C-status may stand on that.
- Triage lane (per row, cheapest first):
  1. If the hook's C3+ status cites non-path1 evidence, note that and move on.
  2. Re-run the diff at a populated state (scenario-attach lane,
     `re/analysis/scenario_attach_lane.md`) — most residual suspects are
     exactly the live-state readers that lane exists for.
  3. If all-zero output is genuinely the function's behavior on every test
     vector, set `degenerate_ok=True` in its registry entry with a one-line
     justification.
- Any demotion goes through `re-classify` with this audit cited as evidence;
  this file does not mutate trackers.
