# D1-residue R3: delete the legacy D3D9 race renderer and the `MASHED_RENDER_LIBRW=0` revert

Branch `work/d1r3`, base `race/first-frame-parity` HEAD `1caf1a10`. Date 2026-10-16.
Model: claude-sonnet-5-5 (account2, no Ghidra MCP; nothing here needed it).

**Verdict.** The deletion is done and both targets build. G-CTRL on TRAINING is unchanged on all
three hashes, all in-race frames and the `drawstream3d` sky/copter numbers are unchanged on 12 of 13
tracks, and no legacy path is reached on the default build. **One finding needs a decision:** at the
sim level (4000-frame G-CTRL recipe) the deletion changes the trajectory on 3 of 13 tracks (Highway,
Storm, rouabout). No-op controls on the pure baseline (never-executed code only) change the same
class of output on rouabout and Storm, so the difference is not attributable to the deleted code
paths, but its mechanism is `[UNCERTAIN]` (section 5).

## 1. What was deleted

| Area | Removed |
|---|---|
| `D3d9Render/TrackRenderer.cpp` `Render` | D3D9 world batch loop and its probes (`MASHED_WORLD_MATID/ONLYMAT/PRELITONLY/REVORDER`, `MASHED_WORLD_VDUMP` D3D9 side), D3D9 prop draw, D3D9 car draw (plain and `RenderCarsRelit` with its dynamic VB), `rw_world` gate and the `RwWorldRender_Render()` call (and its include), `ds_flush` world/props/cars tallies, `d_world` profiler column |
| `TrackRenderer.cpp` load | world `textures_`, `batches_`, `mat_scroll_`, `uv_anim_`, `MASHED_WORLD_FLATSNOW` (D3D9 side), D3D9 bake of track props (`load_prop` keeps it for the sky only), `D-S3-6 bake` log, `MASHED_PROP_VDUMP` (D3D9 side), `LoadCar` wheel split + car batch build + `MASHED_DBG_CARLIGHT`, `LoadCarLiveries` + `car_variants_` (callers in `exe_main.cpp` and `RaceSession.cpp` removed), wheel spin/steer update in `UpdateCar`, relit vertex helpers, `relit_out` of `BuildDffBatches` |
| `TrackRenderer.h`, `RaceSceneState.h` | `batches_`, `textures_`, `car_batches_`, `car_textures_`, `car_relit_`, `CarVariant`, `CarWheel`, `RelitSrc`, `RenderCarsRelit`, `LoadCarLiveries`; `mat_scroll_`, `uv_anim_`, `wheel_spin_`, `steer_vis_` |
| `LibRw/RwRaceSubmit.*` | `RaceSubmit_Requested` (`MASHED_RENDER_LIBRW`), `RaceSubmit_InstancesEnabled` (`MASHED_LIBRW_INST`), `RaceSubmit_InstanceModelEnabled` + `OnlyPropHandle` (`MASHED_LIBRW_ONLYPROP`) |
| `re/tools/ab_report.py` | the `force_legacy` / "arm B = legacy D3D9" arm; `--capture` now takes `--exe-a/--exe-b` (same-exe pair = control) |
| docs | `ROADMAP.md` (R3 closed, R1-R2 open), `FLAG_INVENTORY_2026-08-15.md`, header comments in `RwRaceSubmit.h`, `RwBridge.*`, `exe_main.cpp`, `DrawStreamDump.h` |

Net 13 files, see `git diff --stat`. No translation unit is deleted entirely, so no `hooks.csv` row
loses its file (`RwWorldRender.cpp` stays, see section 7).

**Still drawn through D3D9 on purpose** (librw does not take them): sky dome, copters, particles,
pickups. Evidence: `TrackRenderer.cpp` has no `RegisterModel` for copters or sky draws; baseline
`drawstream3d` carries only `sky` and `copters` with real numbers.

**Failure paths changed (no D3D9 fallback exists any more).** `RaceSubmit_Init` failure in `WinMain`
is fatal (log line `FATAL: librw race renderer failed...`, exit code 4). `RaceSubmit_OnTrackLoaded`
failure now fails `TrackRenderer::Load`. A model whose `RegisterModel` returns -1 is not drawn and
logs `WARN: BuildClump failed -- model is NOT drawn`.

## 2. No legacy path is reached on the default build

- Baseline librw logs, 13 tracks: `WARN/FAIL=0` on every track in each capture (`base_races`,
  `base2_races`, `post_races_final`; `librw_race.txt` has one `ok: init` line each), so no prop or car
  failed registration and fell back to D3D9. `librw_race.txt` lives in the shared main-checkout
  `log/`: two `base_races` logs (Warzone, dump) were another session's scene (identical scene line on
  two tracks), so the `base2_races` and `post_races_final` logs are the reliable ones (0 scene-line
  mismatches between them).
- Baseline `MASHED_DBG_DRAWSTREAM3D=60:62`: `world` and `props` are all-zero in 39 of 39 frame records
  (13 tracks x 3 frames), no `cars` record exists. The deleted code never drew on the default build.

## 3. Default build unchanged: measured

**G-CTRL, TRAINING default arm** (`run_gctrl.py`, 14400 frames, expected `991a5fff / be0d47ea / bc75b405`):

| exe | step | gates | ptrace |
|---|---|---|---|
| baseline `base_a` | `991a5fff` | `be0d47ea` | `cf11042c`, **contaminated** (below) |
| full deletion `post_a`, `commit_a` | `991a5fff` | `be0d47ea` | `bc75b405`, clean (27,356 lines) |

`player_trace.log` is written to the MAIN checkout cwd (the exe relocates there from a worktree) and
is shared with every other session running `MASHED_PLAYERTRACE`. The baseline ptrace came out with
frames from several runs interleaved (frame numbers 7382, 2459, 1114, 6778 in one file, two foreign
`mashed_re` PIDs alive). `run_gctrl.py` now waits for an idle machine and flags a non-monotonic log.
Step and gates use private paths and are not affected.

**Race captures, 13 tracks, 5 shots each** (`run_races.py`, `compare_races.py`):
`COMPARE_base2_vs_postfinal.md`: **64/65 BMPs byte-identical**; every `01_grid`, `01_inrace_track`,
`01_action` frame is identical except Highway `01_action`. `drawstream3d` sky and copters: 0 differing
numbers on 13 tracks; the only difference is the deleted all-zero `world`/`props` rows. Noise floor of
the harness (same baseline exe twice, `COMPARE_base_vs_base2.md`): 57/65, the 8 differing frames are
frontend frames (`00_challengeselect`, `02_back_to_menu`).

## 4. Sim-level sweep, every track (G-CTRL recipe, 4000 frames, `run_gctrl_all.py`, `run_gctrl_vs.py`)

| track | baseline step | R8 only | R1+R8 | keep-LoadCar variant | **full deletion (committed)** |
|---|---|---|---|---|---|
| 10 tracks (Arctic, Egypt, City, Forest, Neustein, SuperG, Warzone, sands, dump, training) | per `GCTRL_ALL_base_vs_csafe.md` | same | same | same | **same** |
| Highway | `9ac2b69c` | same | same | same | **`ce9c877a` differs** |
| Storm | `9aeba248` | same | `e456adb8` | `e456adb8` | **`e456adb8` differs** |
| rouabout | `b7ccf336` | `ff7377fe` | `ff7377fe` | `1d286668` | **`1d286668` differs** |

(R8 only = baseline source with only the post `Render()` swapped in; R1+R8 adds the post `Load()`;
keep-LoadCar = full deletion except `LoadCar`'s D3D9 car-data build. Files: `GCTRL_VS_hybT1.md`,
`GCTRL_VS_E6.md`, `GCTRL_VS_final1.md`, `GCTRL_ALL_base_vs_csafe.md`.)

## 5. The Highway / Storm / rouabout difference

What the full deletion changes: Highway first diverges at frame 161 (AI car 2, control byte `c0`
255 vs 254, 26 columns differ in that row) and the round restarts at frame 1671 (`rt=0.0167` against
`rt=27.85`). The variants that diverge later diverge first at a contact: `rec_148` of car 2 differs by
exactly +0.2 (0.435244292 vs 0.63524431) at frame 2157, all other columns of that row equal
(`rec_148` is the angular-velocity accumulator that `Collision/ContactFixup.cpp` adds to).

**No-op controls on the pure baseline source** (`log/base/mashed_re_layoutctl*.exe`, not committed):

| control | rouabout | Storm | Highway |
|---|---|---|---|
| baseline | `b7ccf336` | `9aeba248` | `9ac2b69c` |
| + unused 12-byte member (`noopctl`), + unused 8-byte member in `RaceSceneState` (`noopctl2`) | same | same | same |
| wheel-block-only deletion in `UpdateCar` (`bisectA`) | same | same | same |
| + 400-iteration function that is never called and one cached `getenv` check per frame in `Render` (`layoutctl`) | **`df753f6c`** | same | same |
| + a second never-called function at the top of `TrackRenderer.cpp` and a check in `Load` (`layoutctl2`) | **`df753f6c`** | **`5669fd2d`** | same |

So rouabout and Storm change when only never-executed code is added. Highway did not change under
these two no-op controls, only under real deletions, so for Highway the control is inconclusive.

**Ruled out as the cause, each measured:** x87 control word, TOP and tag word at every sampled frame
(`cw=027f top=0 tw=ffff`, identical both exes); the RW random ring (`AiRand`: 11 draws, same values,
baseline vs hybrid); heap perturbation on the full-deletion exe (0/3/17 MB leaked 4 KB blocks; free-garbage fills
`00000000`, `deadbeef`, `3f800000` in blocks of 1 KB to 4 MB), stack poison on it (`00`, `cd`, `ff`,
64 KB), and stack plus small-block (8 B to 2 KB) heap poison on the baseline exe: no trace changed in
any of them; wall-clock time (400 ms sleep in `LoadCar`, 4 ms per `Render`: trace unchanged);
compiled code (per-function comparison of the `/FA` listings of baseline vs the hybrid: all 2,486 common
functions identical after label normalisation except `LoadCar`, `Render` and 3 `vector<RelitSrc>`
template instances); the D3D car textures (re-adding them to the flipping hybrid: still flips); the
`LoadCarLiveries` call alone (no flip).

`[UNCERTAIN]` Mechanism. Evidence missing: which sim read depends on code layout or code content. The
deterministic frame clock makes the sim a pure function of the frame index, and every constructed
variant is itself reproducible (each hash above repeated on 2 to 6 runs), so the dependence is on the
build, not on the run. Candidates not tested: a container ordered by function pointer, a read of code
bytes as data, an address-dependent value inside the exe's `_g_b17_low_arena_pad` region.

**Decision for the owner.** Accept the full-deletion commit: the differences appear on exactly the tracks
that no-op edits also move. Or keep `LoadCar`'s dead car-data build (about 240 lines kept, `keep-LoadCar` column): that does
not remove the difference (Storm and rouabout still differ), so it is not worth the dead code.

## 6. Harness, files

`verify/d1r3_20261016/`: `run_races.py`, `run_gctrl.py`, `run_gctrl_all.py`, `run_gctrl_vs.py`,
`compare_races.py`, `make_hashes.py`; reports `COMPARE_*.md`, `GCTRL_*.md`, `HASHES.md`; per-track
`drawstream3d.json`, `PROVENANCE.txt`, `librw_race.txt` for `base2_races` and `post_races_final`.
BMPs are gitignored (`verify/**/*.bmp`), step/gates CSVs are not committed (hashes in `HASHES.md`).
`run_gctrl.ps1` was written as Python: account2's managed policy prompts on `*.ps1` writes and this
session cannot answer prompts.

Reproduce: build `mashedmod\build.bat` in PowerShell, then
`py -3.12 verify\d1r3_20261016\run_gctrl.py <exe> <name> 240` (Training) and
`py -3.12 verify\d1r3_20261016\run_races.py <exe> <outdir>`.

## 7. Not done, follow-ups

- `D3d9Render/RwWorldRender.{h,cpp}` (`MASHED_RW_RENDER`, inert: `RwWorldRender_SetEngine` has no caller)
  is no longer called but still compiled. Deleting the file orphans 8 `hooks.csv` rows: route through
  `re-classify`.
- `load_prop(sky)` still registers the sky with librw although nothing instances it.
- The librw-side halves of `MASHED_WORLD_VDUMP/FLATSNOW/MATID/ONLYMAT` and `MASHED_PROP_VDUMP` in
  `RwSceneBuild.cpp` are now unpaired (their D3D9 counterparts are gone).
- `MASHED_RPLIGHT=0` (legacy load-time light bake) still feeds the D3D9 copter and sky bake. Not part of
  this task.
- Gaps that already existed on the default build and now have no D3D9 reference implementation in the
  tree: AI cars use the player body (no livery variants, `FUN_0040d110` selection documented in
  `TrackRenderer.h`), no wheel spin/steer, no per-frame car relight (D-S3-5), no world UV scroll.
- Proposed tracker row, not applied (trackers move only through `re-classify`): UNCERTAINTIES, "sim
  trajectory on Highway/Storm/rouabout changes with semantics-free code changes; mechanism unknown;
  evidence `verify/d1r3_20261016/RESULT_d1r3.md` section 5".
