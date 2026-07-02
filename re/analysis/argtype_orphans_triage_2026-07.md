# arg_type orphan triage — 2026-07-02

Scope: the 183 arg_types listed in `re/frida/ARG_TYPES.md` § "Registry arg_types
with NO dispatch handler in diff_template.js" (306 registry uses), per the WS-R6
task-C brief: classify each as (a) dead never-run entry, (b) real reimpl blocked
on a missing handler, or (c) misnamed. Worktree `r6/argtype-orphans-triage`;
no handlers authored; no `hooks.csv` edits.

## Headline

**The orphan list was 98% index artifact.** 179 of 183 "orphans" had a working
dispatch handler all along; of the remaining 4, 2 are deliberate non-diffable
markers and 2 were misnames (fixed here). **Zero hooks are blocked on a missing
`diff_template.js` handler — the ranked handler wishlist is empty.** Zero dead
entries were removed (the never-run-looking rows are deliberate markers).

| class | arg_types | registry uses | action |
|---|---|---|---|
| False orphan — handled by `early_window_leaf_diff.py` | 176 | 289 | none (index generator fixed) |
| False orphan — shared-branch dispatch in `diff_template.js` | 3 | 5 | none (index generator fixed) |
| (c) Misnamed → `void_setter_observe` | 2 | 4 | **fixed** in `hooks_registry.py` |
| Deliberate non-diffable marker | 2 | 8 | keep; now a recognized section in the index |
| (a) Dead never-run entries | 0 | 0 | — |
| (b) Blocked on missing handler (wishlist) | 0 | 0 | — |

After the fixes: `gen_arg_types_index.py` reports **112 JS handlers, 184
early-window-only, 2 markers, 0 true orphans**.

## Root cause: two blind spots in `scripts/gen_arg_types_index.py`

1. **First-match-only regex.** `HANDLER_RE.search(line)` recorded one name per
   dispatch line, so shared branches registered only their first alias:
   - `bytes_inplace_3` — dispatched at `diff_template.js:1149`
     (`arg_type === 'bytes_inplace' || arg_type === 'bytes_inplace_3'`); 2 uses
     (`audio_byte_swap_buffer` 0x005aec30 C3, `memset_inline` 0x004b64e0 C3).
   - `transform_vector` — dispatched at `:714` with `transform_point`; 1 use
     (`rw_v3d_transform_vector` 0x004c3880 C4).
   - `eax_implicit_int` — dispatched at `:2398` with `eax_implicit_ptr`; 2 uses
     (`controller_config_load_j5` 0x004971b0 C3, `harness_test_eax_implicit_int`
     0x00497190 harness self-test stub).
   Fixed with `finditer` (handler count 109 → 112).
2. **`early_window_leaf_diff.py` was never scanned.** The pure-leaf pre-crash
   lane (`PURE_LEAF_ARGTYPES`, 192 names, 184 of them absent from
   diff_template.js) is a real dispatch surface: hooks.csv evidence tags
   `green-earlywindow-rN` come from it, and it writes the same
   `log/diff_<hook>.csv` files as run_diff (`early_window_leaf_diff.py:3156`) —
   which is why 293 of the 306 "orphan" uses carried C3/C4 evidence
   (`log/*.csv` ×182, `green-earlywindow-rN` ×112). These arg_types are
   correctly refused by `run_diff.py` pre-flight; their lane is
   `py -3.12 re\frida\early_window_leaf_diff.py <hook>`. The index now lists
   them in their own section instead of calling them orphans.

The old index preamble ("Most are historical worker entries that predate the
2026-06-12 pre-flight and were never run") was wrong for ~98% of the list.

## (c) Misnames fixed (commit `83e938e9`)

Ghidra pre-screen (leaf-decoder, `Mashed_pool0` read-only): all four targets are
identical 10-byte leaves — `MOV EAX,[ESP+4]; MOV [<global>],EAX; RET` — writing
param_1 verbatim to one fixed global, no callees, no other reads/writes. That is
the exact `void_setter_observe` contract (`diff_template.js:369`), and the entry
shape (`signature {'ret':'void','args':['uint32']}` + `target_global` + scalar
tests) was already drop-in.

| old arg_type | hook | RVA | writes param_1 → | hooks.csv |
|---|---|---|---|---|
| `write_global_setter` | `timer_state_set` | 0x0041e130 | 0x0063d7e0 | C3, frida_diff=**pending** |
| `write_global_setter` | `pitch_param_set` | 0x00426630 | 0x0066d6fc | C3, frida_diff=**pending** |
| `write_global_setter` | `pitch_param2_set` | 0x004266f0 | 0x0066d700 | C3, frida_diff=**pending** |
| `int_write_observe` | `save_status_clear` | 0x004099e0 | 0x008a95a0 | C4, log/diff_save_status_clear.csv |

The rename makes the three `pending` rows immediately run_diff-able.

## Deliberate markers (keep; now indexed as such)

- `harness_limited` (7 uses) — synthetic call is unsafe/impossible:
  `crt_fast_error_exit` 0x004a4b93 (terminates the process via
  ___crtExitProcess), `crt_stack_probe` 0x004a3440, `crt_seh_prolog` 0x004a5984,
  `crt_seh_epilog` 0x004a59bf (SEH/stack glue), `piz_win32_open/read/close_compat`
  0x004b6710/0x004b67e0/0x004b6770 (would CloseHandle/clobber a live OS handle in
  DAT_007d3e48; C3 gated on a canonical-scenario observation, documented
  in-entry). Not dead: the entries carry the evidence-lane rationale.
- `register_abi_record` (1 use) — `phys_a4_control_integrate` 0x00470670
  (VehicleControlUpdate, C4). Register-ABI hot-path (EAX=0xd04 record,
  >1000 calls/s); marked `not_run_diffable: True`; its C4 lane is the
  installed-hook canonical-race telemetry (`re/frida/phys_c4_telemetry.py`,
  `MASHED_PHYS_C4_SELFTEST`). A run_diff handler is impossible by design, not
  missing.

## (b) Handler wishlist — empty, cross-checked against the R6 demand map

`re/analysis/r6_demand_map_2026-07.md` § 3 "Ranked gap list" (730 C2 rows): none
of the 17 orphan-linked registry entries appears in it — all are already C2+
with their own lanes, and the only race-slice member (`phys_a4_control_integrate`,
in the demand map's coverage appendix) is C4 via the telemetry lane. No
race-slice hook — and no hook at all — is waiting on a new `diff_template.js`
handler. The right next spend for the race slice remains the gap list's C2→C3
work itself, not harness authoring.

## (a) Dead entries — none removed

The 8 rows that looked never-run (EMPTY/`na` frida_diff) are all
`harness_limited` markers or the `harness_test_eax_implicit_int` trampoline
self-test stub (its arg_type is real, dispatched at `diff_template.js:2398`).
Removing any of them would delete documented rationale, not cruft.

## Follow-ups (out of scope here, flagged for the owning sessions)

1. **Run the three unblocked diffs**: `run_diff.py timer_state_set /
   pitch_param_set / pitch_param2_set`, then clear their `pending` via
   re-classify. Note the pre-existing inconsistency: those rows are C3 with
   `frida_diff=pending` (C3 requires a clean diff) — resolve by running the
   diff, or demote.
2. hooks.csv `piz_win32_*_compat` rows are C2 `mapped` while their registry
   entries describe an implemented defensive fix — worth a re-classify look.

## Provenance

- Join artifacts: session scratchpad (`orphan_join.tsv`, 306 rows =
  183 arg_types × their registry entries × hooks.csv status).
- Pre-screen: leaf-decoder on `Mashed_pool0` (read-only), full disassembly of
  0x0041e130 / 0x00426630 / 0x004266f0 / 0x004099e0; verdicts MATCH on all four
  expected globals.
- Commits: `45a8734f` (generator fix + regen), `83e938e9` (registry renames +
  regen), this note.
