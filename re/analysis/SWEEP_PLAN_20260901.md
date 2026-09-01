# Frida-sweep manifest — area-loop fleet, 2026-09-01

Clean stopping point: all 5 children committed, reported, and are HOLDING (idle, slots + worktrees retained). This is the integration plan for the parent to run (or a fresh session to run from a focused kickoff).

Base branch: `race/first-frame-parity` (parent tip — already has the area-loop infra, RwTexDictionaryCreate C3, the 0-arg path2 fix).

## Branch tips (wound down)

| branch | tip | C3s landed | notes |
|---|---|---|---|
| area/render | 2cfa245a | 1 — RwMatrixInvert_CofactorPath 0x004c4eb0 | path1 5/5 + path2 PASS |
| area/frontend | 5daa893a | 2 — TextSpriteScaled 0x004739f0, TextCtrlCodeRemap 0x004277a0 | path1 10/10, 13/13; path2 for 004277a0 DEFERRED to sweep |
| area/ai | fd0dd389 | 1 — AiSteeringAngleError 0x00415e20 (already re-classified on branch) | booted-race 3585-call 0-mism GREEN; x87 leak fixed |
| area/track | 9531a30a | 0 (pending verifier) | 3 thunks NEEDS-BOOTED-RACE; built the course-load verifier |
| area/hud | 64017a0f | 0 | docs + new DisasmPC.java tool + ledger only; trivial merge |

**C3s to land: 4** (render 1, frontend 2, ai 1). **hud/track land 0 this sweep** (hud refuted B-0001; track's cluster promotes only if its verifier passes).

## Conflict-resolution table (all additive — combine, don't overwrite)

| file | sources | resolution |
|---|---|---|
| `re/frida/verify_hook_install_template.js` (callFn) | **3**: parent (0-arg `signature.args.length===0` guard), render (`fmt_desc_pair_compare` case), frontend (`eax_ptr_ebx_outbuf` + `draw_quad_observe` cases) | keep ALL cases in callFn; each is a distinct `if`. TRICKIEST merge. |
| `re/frida/diff_template.js` | frontend (`eax_ptr_ebx_outbuf` handler) | single source; take it. render reused existing `fmt_desc_pair_compare` (did NOT touch this file). |
| `re/frida/scenario_launch.py` | track (`--assert-course-load` + `courseLoadAsserts` rpc, additive/guarded) | single source; take it. |
| `re/frida/hooks_registry.py` | **3**: track (3 entries), frontend (text_ctrl_code_remap), render (rw_matrix_invert). ai did NOT touch it — its self-test is env-gated (MASHED_AI_STEER_SELFTEST), not a registry arg_type. | combine all entries; different insertion points, additive. |
| `mashedmod/build.bat` | render only (+Render\RwMatrixInvert.cpp to exe list) | single source. |
| `mashedmod/asi_sources.rsp` | **3**: track (CourseLoadDispatch.cpp), frontend (TextCtrlCodeRemap.cpp), render (RwMatrixInvert.cpp) | add all 3 lines. |
| `hooks.csv` | ai (1), frontend (2), render (1) → 4 C3 rows, distinct RVAs | line-merge. |
| `re/AREA_LEDGER.md` | all 5 + parent | biggest textual conflict; reconcile top-table + each area section by hand. |
| `re/analysis/CHANGELOG.md` | ai, frontend (3), render (2), parent | all prepend below `<!-- ENTRIES -->`; combine, do NOT overwrite (477-entry-loss precedent). |
| `UNCERTAINTIES.md` | frontend (U-0459), render (U-4930) | line-merge (resolved markers). |
| `re/PROMOTION_QUEUE.md` | all | combine rows. |
| new files | RwMatrixInvert.cpp, TextCtrlCodeRemap.cpp, CourseLoadDispatch.cpp, DisasmPC.java, area_hud_*.md | no conflict. |

## Dry-run result (2026-09-01, `git merge-tree` area/render+area/frontend, in-memory)

Only **2 files conflict**; everything else auto-merges (`hooks_registry.py`, `AREA_LEDGER.md`, `asi_sources.rsp`, `hooks.csv`, `PROMOTION_QUEUE.md`, `UNCERTAINTIES.md` — better than feared):
- **`re/frida/verify_hook_install_template.js`** — CONFLICT, but PURELY ADDITIVE: render and frontend both insert a new `if (CONFIG.arg_type === ...)` block at the SAME anchor (`callFn`, line 133, just after the parent's 0-arg + scalars guards). No logic overlap. **Resolution: stack all cases** — parent 0-arg guard, then render's `fmt_desc_pair_compare`, then frontend's `draw_quad_observe` + `eax_ptr_ebx_outbuf`. Take both `+` hunks.
- **`re/analysis/CHANGELOG.md`** — CONFLICT (both prepend below `<!-- ENTRIES -->`). Keep ALL entries; do not overwrite (477-entry-loss precedent).

track's `scenario_launch.py` and frontend's `diff_template.js` are single-source (no conflict). So the entire 5-branch sweep has exactly **2 hand-merges** (JS callFn + CHANGELOG), both additive. Low risk.

## Sweep steps

1. Branch `sweep/20260901` off `race/first-frame-parity`.
2. Merge area/render, area/frontend, area/ai, area/track, area/hud (order: hud→render→ai→frontend→track, simplest-first). Resolve per the table.
3. `py -3.12 scripts/gen_arg_types_index.py` (regen ARG_TYPES.md for eax_ptr_ebx_outbuf).
4. Rebuild canonical `.asi` via **PowerShell** `& "mashedmod\build.bat"`; **verify `.asi` epoch < 120s** (a background/incremental build gave a false-pass once — always epoch-check).
5. Integration verifications (all against the canonical .asi):
   - path1 re-diffs: `run_diff.py rw_matrix_invert` / `text_sprite_scaled` / `text_ctrl_code_remap` (rw_tex + ai already GREEN).
   - path2: `run_verify_hook.py text_ctrl_code_remap` (frontend deferred) + spot-check others.
   - **track course-load verifier** (no driving car): `scenario_launch.py --assert-course-load --hold 0` (baseline PASS) → `... --hooks 0x0040d020,0x0040d440,0x0047b9e0` (cluster PASS = no-regression → cluster promotes to C3).
   - ai booted-race already GREEN (3585 calls) — re-run optional.
6. On all-GREEN: land the C3s, move PROMOTION_QUEUE rows to Merged, commit the sweep.
7. Tell children to `resume`.

## Gotchas carried into the sweep
- Build false-pass: `cmd /c` from Git Bash no-ops; use PowerShell; ALWAYS epoch-check the .asi.
- Shared `original\mashed_re_dev.asi` is fleet-shared — render's build (1172 exports) is currently deployed; the canonical rebuild overwrites it.
- x87 ST0 leak class ([[x87-st0-float10-fnptr-void-leak]]): watch any new float10-returning fn-ptr declared void.
- B-0003 (0x004726f0 ST0-return-typed-void) still OPEN, parent-owned — not in this sweep.
