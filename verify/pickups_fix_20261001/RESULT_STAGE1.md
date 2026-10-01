# Render fix (c) — pickups, STAGE 1 (PLACEMENT): acceptance run

Rules as pre-registered in `PREREG_STAGE1.md` (commit `b2649833`, written before
any edit under `mashedmod/src`). Fix commit `c9615225`. Pre-stage HEAD
`f7ab571b`.

## Binaries

| arm | what | SHA-256 |
|---|---|---|
| A | pre-stage HEAD `f7ab571b`, unmodified | `B685C9BCAE814B2EA6F26F3C33CEFBF093E5A0F4BAD371F79A7C4E379F6EE74A` |
| B | HEAD + the P3 projection dump ONLY (no placement change) | `862CC617D1939EBF4F50FF86267A18755CB19EEAF91B7F0EFE074FFC31B710DD` |
| C | B + the placement fix | `B7184CE2D84690BA183263C7469547D6B3C4246BA31FF6566784E76B8EB2BEDD` |

Files: `bin/A_head_f7ab571b.exe`, `bin/B_logonly.exe`, `bin/C_placement.exe`.
Built with `mashedmod\build.bat` from PowerShell. One-RVA-one-body lint:
**122 findings, all allowlisted, NEW = 0** on both C builds.

Second-live-copy check (manual, as required): the only pickup placement code is
`TrackRenderer.cpp`'s block at :1360-1460 plus `PickupField::InitReal`, and the
only in-race pickup draw is `PickupField::Render` (single call site,
`TrackRenderer.cpp:6028`). `Powerup/PowerupEffects.cpp` and
`Powerup/PowerupSystem.h` are effect logic, not placement or draw;
`exe_main.cpp`'s POWERUPICONS code is the frontend 2D preview row. No second
copy.

## P1 — placement equality vs the live original. **PASS**

Port list vs the original's live-read pool, element-wise in pool order,
**bit-identical float32** positions.

| track | n (orig samples) | orig count | port count | result |
|---|---|---|---|---|
| TRAINING | 6, all agreeing | 5 | 5 | **EXACT on all 5** (type, respawn, x, y, z) |
| ARCTIC | 6, all agreeing | 7 | 7 | **EXACT on all 7** |

Order matches too (no permutation fallback triggered).

Original-side artifacts, both read-only `Memory` reads, no `Interceptor`:

- TRAINING `verify/pickups_20260929/orig_qb.bmp.pickuprecs.json` — Quick Battle,
  rank `DAT_0067ea74` = 1.
- ARCTIC `orig_arctic_ctrl.bmp.pickuprecs.json` — Challenge Cup entry 3, cup rows
  0-3 unlocked on a SAVE COPY (`save_rows0123.bin`, applied through
  `re/tools/run_with_unlocked_save.py`; `original/gamesave.bin` restored and
  sha-verified `bd18788182b2343e` after every run). Rank read back as 1.

Negative controls: routes that read rank 0 place **nothing** (live count 0,
`FUN_004264d0` early return `0x004265a6`) — measured twice on TRAINING. A second
ARCTIC run with rank force-poked to 1 gave a record set **identical** to the
unpoked control, so the control is the citation and the poke is not load-bearing.

Port-side logs: `C_r1/12_training/mashed_re.log`, `C_r1/00_Arctic/mashed_re.log`
(`PUPLACE` / `PUDROP` / `PUINIT` lines). Checker:
`pu_placement_check.py --log … --orig … --piz …`.

## P2 — filter parity. **MATCH on all 13 tracks**

Live-measured on the two tracks with an original read (same artifacts as P1):
TRAINING 17 atomics → 12 dropped (all type 21) → 5 kept; ARCTIC 23 → 16 → 7.
Both the kept set and the dropped set match.

Port vs the independent reference implementation of the same three predicates
(`re/tools/powerups_gold_dump.py` + `pu_placement_check.reference_filter`), all
13 tracks, labelled **port-vs-reference-implementation**, not live evidence:

| Arctic | Egypt | City | Forest | Highway | Neustein | Storm | SuperG | Warzone | rouabout | sands | dump | training |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 7 | 7 | 10 | 10 | 11 | 8 | 7 | 7 | 14 | 7 | 9 | 9 | 5 |

all MATCH. Per-track marker dump: `dff_markers_all_tracks.csv` (327 rows).

**A real disagreement was found and resolved here, in the reference reader's
favour of the port.** `sands` first reported DIFFER on 2 of 9 kept markers. Cause:
`sands` geometries 8/18/23 and 14 of `rouabout`'s carry **two** RW user-data
arrays with the **same name**, array 0 with one element and array 1 with twelve;
the Python reader keyed them by name, so array 1 shadowed array 0. The original
reads `FUN_004b5190(atomic, 0, 0)` = array 0 positionally, which is what the C++
does. Verified at byte level (`geo8: ud n=2, array0 nameLen=13 '0.tv_part_id'
dt=1 cnt=1 v0=0x0507`) and by an isolated harness build of
`Track/PowerupMarkers.cpp`. Python fixed; all 13 then MATCH. This also closes the
`rouabout`/`sands` [UNCERTAIN] in the diagnosis note's §2.3 — the "39 elements vs
25 geometries" was this duplicate array, not a mapping ambiguity.

## P3 — no collateral pixels. **PASS** (Arctic), **UNMEASURABLE** (training)

Mask = the union of both arms' per-orb screen discs, each one the ENGINE's own
projection of the orb centre and its draw bound through the live
`D3DTS_VIEW * D3DTS_PROJECTION` and viewport, dilated 3 px. No colour class, no
hand-drawn box.

| arm pair | track | frame | pose | diff INSIDE mask | diff OUTSIDE mask | verdict |
|---|---|---|---|---|---|---|
| B vs C | Arctic | `01_grid` | deterministic default | 888 | **0** | PASS |
| B vs C | Arctic | `01_action` | deterministic default | 854 | **0** | PASS |
| B vs C | Arctic | `01_grid` | pinned original basis | 59 | **0** | PASS |
| B vs C | Arctic | `01_inrace_track` | default | 0 | 0 | UNMEASURABLE (vacuous) |
| B vs C | Arctic | `01_action`, `01_inrace_track` | pinned | 0 | 0 | UNMEASURABLE (vacuous) |
| B vs C | training | `01_grid` | both poses | 0 | 0 | UNMEASURABLE (vacuous) |
| B vs C | training | `01_action`, `01_inrace_track` | both poses | — | — | NOT COMPARABLE (collection guard fired) |

Guards, all satisfied:

- **Instrumentation control.** A vs B, whole frame, 2 boots, both tracks:
  **0 differing pixels on every in-race frame** (`01_grid`, `01_action`,
  `01_inrace_track`). The two MENU frames (`00_challengeselect`,
  `02_back_to_menu`) are unstable run to run in one boot of arm A on Arctic
  (A_r1 vs A_r2 = 45803 / 44580 px; A_r2 vs B_r2 = 0), so they fail the two-boot
  rule on their own and are excluded. They carry no in-race pickup draw, so P3
  does not rest on them.
- **Projector cross-check.** Independent Python projection through the committed
  12-float `MASHED_CAM_POSE` basis + published lens vs the engine-dumped screen
  centres: **max |d| = 0.002 px** (Arctic arm C, n=5), **0.001 px** (training arm
  C, n=3), **0.001 px** (Arctic arm B, n=12). Tolerance 1.0 px. PASS.
- **Two-boot rule.** Every in-race frame of both arms is **bit-identical across
  two boots** (B_r1 vs B_r2 = 0, C_r1 vs C_r2 = 0, all 6 track×frame pairs). The
  two PASS verdicts re-run on boot 2 give the same 888 / 854 and 0 outside.
- **No collection at the compared frame.** Enforced, and it is what disqualifies
  training's `01_action` (pre arm: active 7 of 8) and `01_inrace_track` (post arm:
  active 4 of 5).
- **Non-vacuity.** Satisfied: Arctic is non-vacuous (888 / 854 / 59 px inside the
  mask), so the comparison did detect the fix.

[UNCERTAIN] **Why the TRAINING frames are vacuous.** Measured, not assumed: with
`MASHED_NO_PICKUPS=1` as a control, the pickup draw contributes **0 pixels** to
all three captured TRAINING frames in arm C (on Arctic `01_grid` the same control
gives 877 px). So no pickup is visible on any comparable TRAINING frame in either
arm, and there is nothing for the diff to find. The projected centres land on
distant track and on the right-hand start barrier
(`dbg_training_grid_orbs.png`), which is consistent with occlusion, but that was
not proven and no cause is claimed. It is not a stage-1 regression: arm B, with
HEAD's own placement, is equally invisible there.

## P4 — collection behaviour unchanged. **PASS**

- `git diff f7ab571b -- …/PickupField.cpp` hunks are at lines 1-10 (includes),
  129-150 (`InitReal`), 221, 233 and 273 (`Render` + the new dump). **No hunk
  falls inside `Update` (155-183) or `CollectAt` (185-203)**, and
  `PickRadius()` is still `worldR_ * 0.04f` in the header.
- Collection still fires post-fix: the P3 guard reports arm C reaching
  `active = 4 of 5` on TRAINING `01_inrace_track`, i.e. an orb was taken.

The shared table `PickupField::orbs_` changes CONTENTS by design — that is the
fix. The collection code does not.

## Deliberately NOT changed in stage 1 (named so it is not read as an oversight)

- **Collection radius.** Still `worldR_ * 0.04f` (measured 1.101 on TRAINING,
  1.968 on Storm). The original uses a **0.5** sphere — `FUN_00484cf0(&local_98)`
  at `0x00459228`, radius literal `0x3f000000`. A 2x-4x divergence, left alone
  because it is collection logic and this child was scoped to placement and look.
- **Rank 0 / rank 2 arms.** Only the normal-race arm is wired. `rank == 0` places
  nothing (`0x004265a6`); `rank == 2` accepts ONLY type 21 and replaces it via the
  randomiser `FUN_00458d00` @`0x00458d00`. [UNCERTAIN] what drives
  `DAT_0067ea74`; measured values this session are 1 (Quick Battle, Challenge Cup
  entry 3) and 0 (two other routes).
- **The look.** Texture, blend state, billboard size and the invented bob are
  untouched — that is stage 2.

## Reproduce

```
py -3.12 verify/pickups_fix_20261001/run_race.py <exe> <outdir> 0,12 \
    --env MASHED_DBG_PICKUPDUMP=1 [--pose verify/pickups_fix_20261001/pose_*.txt]
py -3.12 verify/pickups_fix_20261001/pu_placement_check.py --log <log> --orig <json> --piz <piz>
py -3.12 verify/pickups_fix_20261001/pu_check.py diff   <preBMP> <preDUMP> <postBMP> <postDUMP>
py -3.12 verify/pickups_fix_20261001/pu_check.py xcheck <dump> --pose <basis.txt>
py -3.12 verify/pickups_fix_20261001/pu_check.py whole  <bmpA> <bmpB>
py -3.12 re/tools/powerups_gold_dump.py --all --csv <out.csv>
```
