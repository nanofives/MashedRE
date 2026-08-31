# verify/ evidence manifest

**Generated 2026-08-24.** This file exists so that a citation of `verify/<dir>`
always resolves to tracked content, even though the heavy capture blobs in those
directories are deliberately **not** committed.

## Why

D0.6 recorded the failure mode: 14 C4 rows cited evidence under gitignored `/log/`,
so the citations pointed at files git could not vouch for. The same latent fragility
existed here, one order of magnitude larger -- 165 untracked `verify/` directories
holding 1.60 GB, 35 of them cited by tracked files including `hooks.csv`,
`UNCERTAINTIES.md`, `DEFERRED.md` and `ROADMAP.md`.

Disposition, 2026-08-24:

- **129 uncited directories deleted** (299,613,877 bytes). Every one was checked for
  reparse points first -- the repo has two prior incidents where a recursive delete
  followed a junction into the real game install (`re/diag/KNOWN_ISSUES.md`
  WORKTREE-SYMLINK-WIPE). Zero found; `original/` verified intact after.
- **`verify/carpos_probe` was NOT deleted** despite being the largest single
  directory (836 MB). It is cited by `re/analysis/QOL_PATCH_PLAN_2026-08.md`.
- **`verify/d07_batch1_20260818` was NOT deleted** despite having no path citation:
  it holds the `regression 16/16 byte-identical` evidence for the ledger's
  `d07_addback_batch1`, which is verified-but-unpromoted. Delete only after promotion.
- **`.bmp` / `.msd` / `.png` under `verify/` are now gitignored.** The readable
  verdicts (`RESULT.md` / `AUDIT.md` / `README.md`) were already tracked, and the
  `.txt` / `.json` telemetry is committed alongside this manifest, so each cited
  directory keeps a tracked, human-readable record of what it proved.

**Known residue, stated rather than hidden:** 616 `.bmp`/`.png`/`.msd` files
totalling 322,698,309 bytes are ALREADY in git history from before this rule. The
ignore rules stop new blobs landing; they do not retract those, and no history
rewrite was performed to remove them.

## Reading a row

`blobs` = local-only `.bmp`/`.msd`/`.png` (count and bytes as of generation).
`text` = `.txt`/`.json` in the directory. `cited by` = tracked files naming this
path, excluding files inside the directory itself. A row with blobs but no local
copy on your machine is expected after a fresh clone -- regenerate with the harness
that produced it (`re/analysis/parity_tooling.md` for draw-list/screenshot sets,
`re/frida/scenario_launch.py --statediff-out` for `.msd`).

| directory | blobs | blob bytes | text | newest | cited by |
|---|---:|---:|---:|---|---|
| `screen_id` | 44 | 33,709,964 | 35 | 2026-08-30 | re/analysis/structs/contcfg_record.md; UNCERTAINTIES.md (U-9049, U-9050); re/analysis/CHANGELOG.md; verify/screen_id/README.md |
| `carpos_probe` | 858 | 875,664,990 | 1 | 2026-08-03 | re/analysis/QOL_PATCH_PLAN_2026-08.md; re/orchestrator/state.json |
| `statediff_proto` | 24 | 60,241,872 | 9 | 2026-08-21 | mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp; re/analysis/CHANGELOG.md; re/orchestrator/state.json; re/tools/statediff/NOISE_MASK.md; re/tools/statediff/README.md |
| `dsproof` | 64 | 58,985,856 | 3 | 2026-08-16 | mashedmod/src/mashed_re/exe_main.cpp; re/analysis/CHANGELOG.md; re/orchestrator/state.json; verify/d1_measure/BISECT_ROUND_BOUNDARY.md; verify/inrace_ab/RESULT.md |
| `sametrack3` | 54 | 48,624,683 | 0 | 2026-08-16 | UNCERTAINTIES.md; re/analysis/CHANGELOG.md; re/orchestrator/state.json; verify/d1_frame/RESULT.md; verify/d1_lens/RESULT.md; verify/d1_sky_orig/RESULT.md |
| `wedge_nodrive_20260820` | 11 | 42,407,408 | 13 | 2026-08-20 | re/analysis/D2_WEDGE_REMEASURE_2026-08-20.md; re/orchestrator/state.json |
| `d1_recheck_20260818` | 38 | 30,378,190 | 0 | 2026-08-18 | re/analysis/RE_MASTER_PLAN_2026-07.md |
| `campose2` | 34 | 29,806,060 | 0 | 2026-08-15 | re/analysis/CHANGELOG.md |
| `dsproof2` | 32 | 29,492,928 | 2 | 2026-08-16 | mashedmod/src/mashed_re/exe_main.cpp; re/analysis/CHANGELOG.md; verify/inrace_ab/RESULT.md |
| `d1_mirrorfix` | 32 | 29,492,928 | 0 | 2026-08-16 | mashedmod/src/mashed_re/LibRw/RwRaceSubmit.cpp; re/orchestrator/read_fleet/runs/w1_relink/w1b_doc_roadmap.md; re/orchestrator/read_fleet/runs/w1_relink/w1c_changelog_gap.md; verify/d1_evidence/README.md; verify/d1_steersign/RESULT.md |
| `d1_control_20260818` | 32 | 29,492,928 | 0 | 2026-08-18 | re/analysis/RE_MASTER_PLAN_2026-07.md |
| `d07_batch1_20260818` | 32 | 29,492,928 | 0 | 2026-08-18 | *(no path citation)* |
| `allmode` | 32 | 29,492,928 | 0 | 2026-08-16 | re/analysis/CHANGELOG.md; re/orchestrator/read_fleet/runs/w1_relink/w1b_doc_roadmap.md; verify/d1_evidence/README.md; verify/d1_fxbloom/RESULT.md; verify/d1_fxcut/RESULT.md; verify/d1_nopart/RESULT.md |
| `inrace_ab` | 32 | 29,492,928 | 0 | 2026-08-16 | re/analysis/CHANGELOG.md |
| `sametrack` | 29 | 26,727,966 | 4 | 2026-08-16 | UNCERTAINTIES.md; re/analysis/CHANGELOG.md; re/orchestrator/state.json; verify/d1_frame/RESULT.md; verify/d1_lens/RESULT.md; verify/d1_sky_orig/RESULT.md |
| `parity_20260819` | 23 | 16,423,387 | 3 | 2026-08-19 | re/analysis/CHANGELOG.md; re/frida/trace_savedata_gate.py |
| `d1_librw` | 16 | 14,746,464 | 0 | 2026-08-15 | verify/d1_evidence/README.md |
| `race1` | 11 | 9,410,946 | 0 | 2026-08-15 | DEFERRED.md; ROADMAP.md; mashedmod/src/mashed_re/exe_main.cpp; re/analysis/CHANGELOG.md; re/analysis/LIBRW_SIZING_2026-08.md; re/analysis/SESSION_VERIFICATION_AUDIT_2026-08-15.md; re/analysis/archive/ROADMAP_v2_2026-06-09.md; re/diag/cruft_triage_2026-07-02.md |
| `d1_probe` | 10 | 9,216,540 | 0 | 2026-08-15 | verify/d1_evidence/README.md |
| `a8_steer_20260823` | 1 | 7,789,576 | 0 | 2026-08-23 | ROADMAP.md; re/analysis/D2_REALPHYS_REMEASURE_2026-08-21.md; re/orchestrator/state.json; re/tools/statediff/field_trace.py |
| `wedge_drivelate_20260821` | 5 | 6,004,880 | 15 | 2026-08-21 | re/analysis/D2_WEDGE_REMEASURE_2026-08-20.md |
| `qol_asi_20260801` | 23 | 3,800,986 | 1 | 2026-08-02 | mashedmod/src/qol_asi/mashed_qol.cpp; re/analysis/QOL_PATCH_PLAN_2026-08.md |
| `shimport_20260814` | 2 | 1,221,057 | 2 | 2026-08-16 | re/analysis/CHANGELOG.md |
| `d1_sky_orig` | 1 | 921,654 | 4 | 2026-08-16 | verify/d1_frame/RESULT.md; verify/d1_lens/RESULT.md |
| `d1_lens` | 1 | 921,654 | 5 | 2026-08-16 | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp; re/orchestrator/read_fleet/runs/w1_relink/w1b_doc_roadmap.md; re/orchestrator/read_fleet/runs/w1_relink/w1c_changelog_gap.md; verify/d1_frame/RESULT.md |
| `d1_frame` | 1 | 921,654 | 6 | 2026-08-16 | verify/d1_basis/RESULT.md; verify/d1_carproj/RESULT.md |
| `d1_carproj` | 1 | 921,654 | 7 | 2026-08-16 | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp; re/frida/race_draw_burst.py; verify/d1_basis/RESULT.md |
| `d1_basis` | 1 | 921,654 | 8 | 2026-08-16 | mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp; mashedmod/src/mashed_re/LibRw/RwRaceSubmit.cpp; re/orchestrator/read_fleet/runs/w1_relink/w1b_doc_roadmap.md; re/orchestrator/read_fleet/runs/w1_relink/w1c_changelog_gap.md; verify/d1_mirrorfix/RESULT.md |
| `scenario_batch` | 19 | 809,659 | 0 | 2026-07-31 | re/frida/run_diff_scenario_batch.py |
| `link_ab_20260729` | 13 | 350,916 | 0 | 2026-07-29 | hooks.csv; re/analysis/CHANGELOG.md |
| `wedge_rate_20260820` | 4 | 313,648 | 1 | 2026-08-20 | re/analysis/D2_WEDGE_REMEASURE_2026-08-20.md |
| `capture_20260729` | 4 | 140,614 | 0 | 2026-07-29 | re/analysis/plans/state_lane_attach_point_2026-07-29.md |
| `wedge_noopcook_20260820` | 4 | 73,456 | 13 | 2026-08-20 | re/analysis/D2_WEDGE_REMEASURE_2026-08-20.md |
| `wedge_rate2_20260820` | 8 | 53,504 | 19 | 2026-08-20 | re/analysis/D2_WEDGE_REMEASURE_2026-08-20.md |
| `multi_state` | 8 | 31,488 | 0 | 2026-07-29 | re/frida/multi_state_driver.py |
| `stalker_batch` | 3 | 11,808 | 0 | 2026-07-29 | re/frida/stalker_write_surface_batch.py |
| `wedge_hold38_20260821` | 0 | 0 | 13 | 2026-08-21 | re/analysis/D2_WEDGE_REMEASURE_2026-08-20.md |

**Totals:** 36 directories, 1,424,801,792 blob bytes local-only, 2,029,217 bytes of text.
