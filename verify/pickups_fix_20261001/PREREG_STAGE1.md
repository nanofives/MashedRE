# Render fix (c) — pickups. STAGE 1 (PLACEMENT): pre-registered acceptance rules

Written and committed **before any edit under `mashedmod/src`**. Branch
`race/first-frame-parity`, pre-stage HEAD `f7ab571b`.

Diagnosis is NOT re-derived here — it is
`re/analysis/PICKUPS_LOOK_PLACEMENT_2026-09-29.md`. Stage 1 lands only Fix B1 +
Fix B2 from that note's §4. The LOOK (Fix A1/A2) is stage 2 and is out of scope
for every rule below.

---

## 0. What stage 1 changes, and what it must not

CHANGES (placement only):

1. The port reads pickup positions from the track piz's `POWERUPS_GOLD.DFF`,
   the file the original actually opens — `FUN_004264d0` @`0x004264d0` pushes
   `"powerups_gold.dff"` (`0x005cd4e4`) at `0x004265bd`, calls the clump loader
   `FUN_0042a5d0` at `0x004265c2`, and on a non-null clump calls the live
   placement path `FUN_00426460` at `0x004265d2`. The Lua text scan the port
   uses today is the original's **fallback** arm (`JE 0x004265df`), unreachable
   on shipping data because all 13 track pizzes carry the DFF.
2. Per atomic it takes the frame's modelling-matrix translation
   (`frame + 0x40`, i.e. RwFrame modelling matrix `+0x10` + RwMatrix pos
   `+0x30`) and the geometry's RW USERDATA (`0x011f`) element-0 int `v`, then
   `type = v & 0xff`, `respawn = (float)(v >> 8)` — the exact split
   `FUN_00426460` @`0x00426460` performs before calling `FUN_00458fd0`.
3. It applies `FUN_00458e00` @`0x00458e00`'s accept/reject predicates, in that
   function's order:
   - `if (0x18 < DAT_0068b9a8) return -1;` — pool cap 25.
   - reject when the squared distance to any already-accepted position is
     `< _DAT_005cc558` (`0x3a83126f` = 0.00100000005).
   - `else if (param_2 == 0x15) return -1;` — reject BLANK (type 21) in the
     normal-race arm.
4. It stores the position verbatim into the orb (the original writes `*param_1`
   into both `+0x2c..0x34` and `+0x38..0x40`), dropping the port's invented
   `worldR_ * 0.012f` Y lift at `PickupField.cpp:139`.
5. It drops the "every 8th AI gate gets an orb" race-path fallback
   (`PickupField.cpp:118-129`). The original has no such behaviour.

MUST NOT CHANGE (and rule P4 tests it):

- powerup gameplay logic: box state, the dispatcher `FUN_0045bba0`, ammo,
  cooldown, and the **collection** code `PickupField::Update` / `CollectAt` /
  `PickRadius`.
- the pickup LOOK: texture, blend state, billboard size, bob. Stage 2.

EXPLICITLY DEFERRED, and named so no later reader mistakes it for an oversight:

- The collection radius stays `worldR_ * 0.04f`. The original's is a **0.5**
  sphere (`FUN_00484cf0(&local_98)` at `0x00459228`, radius literal
  `0x3f000000`). That is collection logic, which this child is told not to
  touch; it is a real divergence and is recorded as such, not fixed here.
- The `rank == 0` (place nothing, `FUN_004264d0` `0x004265a6`) and `rank == 2`
  (BLANK-only, randomised by `FUN_00458d00` @`0x00458d00`) arms are NOT wired.
  The port implements the normal-race arm only, which is the arm both live
  original measurements below were taken in (`DAT_0067ea74` read back as 1 on
  both). What sets `DAT_0067ea74` is still [UNCERTAIN] (note §5).

---

## 1. Evidence already in hand (original side, live, read-only)

Both reads come from the running original through
`re/frida/pickup_pos_probe2.py` — read-only `Memory` reads of the pickup pool,
**no `Interceptor`** (hot-path rule). Pool anchors: records at `0x0068b198`,
stride `0x50` (`+0x18` respawn, `+0x20` state, `+0x24` type, `+0x2c..0x34`
position, `+0x38..0x40` anchor); live count `DAT_0068b9a8` @`0x0068b9a8`;
writer `FUN_00458e00` @`0x00458e00`.

| track | route | rank `DAT_0067ea74` | live count | artifact |
|---|---|---|---|---|
| TRAINING | Quick Battle | 1 | **5** | `verify/pickups_20260929/orig_qb.bmp.pickuprecs.json` (6 samples, all agree) |
| ARCTIC | Challenge Cup entry 3, cup rows 0-3 unlocked on a SAVE COPY | 1 | **7** | `verify/pickups_fix_20261001/orig_arctic_ctrl.bmp.pickuprecs.json` (6 samples, all agree) |

Negative controls measured this session, same probe: the Challenge route with
only row 0 unlocked loads TRAINING with `DAT_0067ea74 = 0` and places **0**
boxes — `FUN_004264d0`'s `rank == 0 -> return` at `0x004265a6`. Depth-3 mode
cursor 2 likewise: TRAINING, rank 0, count 0. A second ARCTIC run with
`DAT_0067ea74` force-poked to 1 returned a record set **identical** to the
unpoked control, so the control (no poke) is the citation and the poke is not
load-bearing.

Reference reader, validated against both of those live reads with **exact
IEEE-754 equality on every float**: `re/tools/powerups_gold_dump.py` (new this
session). It walks the real chunk tree — FRAMELIST `0x0e`, GEOMETRYLIST `0x1a`,
per-geometry EXTENSION `0x03` -> USERDATA `0x011f` — rather than scanning the
blob for a key string, which closes the `rouabout`/`sands` [UNCERTAIN] in the
note's §2.3. Measured: the live pool order is the **reverse** of the DFF atomic
order on both tracks.

---

## 2. Rules

A rule PASSES only on the stated value. "Report it" means report it; do not
amend a rule to fit the result.

### P1 — placement equality vs the live original  (brief rule (i))

For TRAINING and ARCTIC: the port's pickup list, read from the standalone's own
per-run dump, equals the original's live-read pool **element-wise in pool
order**: same count, and per element same `type`, same `respawn`, and
**bit-identical** world `x, y, z` (`|Δ| == 0.0`, compared as float32).

- n: original = 6 agreeing samples per track (already captured, §1);
  standalone = 2 independent boots per track, which must agree with each other.
- PASS iff count and all three float components and type and respawn match
  exactly, on BOTH tracks.
- A match on positions but not on ORDER is reported as PARTIAL, with the
  permutation printed.

### P2 — filter parity  (brief rule (ii))

The port KEEPS and DROPS the same `POWERUPS_GOLD.DFF` atomics as
`FUN_00458e00` @`0x00458e00`.

- Live-measured on TRAINING and ARCTIC: the kept set must equal the live pool
  (same artifacts as P1) and the dropped set must equal the DFF's remaining
  atomics. TRAINING: 17 atomics, 12 dropped (all type 21), 5 kept. ARCTIC: 23
  atomics, 16 dropped, 7 kept.
- For the other 11 tracks the port's keep/drop sets are compared against
  `re/tools/powerups_gold_dump.py`'s independent implementation of the same
  three predicates. This is **port-vs-reference-implementation**, not live
  evidence, and is reported with that label. It cannot promote anything.
- PASS iff both live tracks match exactly AND no track shows a keep/drop
  disagreement against the reference reader.

### P3 — no collateral pixels  (brief rule (iii))

At a pinned deterministic pose, on TRAINING and ARCTIC, the post-fix build's
captured frame differs from the pre-stage HEAD build's captured frame in
**zero** pixels outside the union of the two arms' pickup regions.

Region construction — **projected geometry only**:

- A pickup region is the screen-space disc centred on the orb's world centre
  projected through the **live `D3DTS_VIEW` * `D3DTS_PROJECTION` and the live
  viewport**, radius = the orb's own draw bound (billboard half-size
  `worldR_ * 0.018` scaled by `sqrt(2)` for the quad corner, plus the bob
  amplitude `worldR_ * 0.008`) projected through the same transform, dilated by
  **3 px**.
- The projection is computed **by the engine**, from the matrices it actually
  drew with, and dumped per captured frame beside the BMP. No colour class, no
  hand-drawn rectangle, no connected-component silhouette.
- The union is over BOTH arms (an orb that disappears can only change pixels
  inside its own former region).

Guards, all of which must hold or P3 is VOID rather than PASS:

- **Instrumentation control.** The dump is added by a logging-only build B
  (= HEAD + the dump, no placement change). B must differ from plain HEAD A in
  **zero** pixels over the WHOLE frame, 2 boots, both tracks. Otherwise the
  instrument is not inert and P3 cannot be read.
- **Projector cross-check.** An independent Python projection of the same world
  centres through the committed 12-float `MASHED_CAM_POSE` basis and the
  published lens (`fovy = 2*atan(0.45)`, aspect `800/600`, near `0.1`) must
  agree with the engine-dumped screen centres to **<= 1.0 px**, max over all
  orbs. This is what proves the mask is not stale.
- **Two-boot rule.** Each arm is captured twice; a verdict counts only if the
  two boots of the same arm are pixel-identical.
- **Non-vacuity.** If the union mask contains **zero** differing pixels on both
  tracks, the comparison proved nothing (no pickup was on screen) and P3 is
  reported **UNMEASURABLE**, not PASS. In that case the capture is repeated at
  a pose with at least one on-screen orb.
- **No collection may have occurred** in either arm at the compared frame
  (each arm's dump reports `active == total`); otherwise gameplay, not
  placement, is driving the difference and the frame is not comparable.

### P4 — collection behaviour unchanged

Placement and collection share one table (`PickupField::orbs_`). The table's
CONTENTS change by design — that is the fix. The collection CODE must not.

- `git diff f7ab571b -- mashedmod/src/mashed_re/D3d9Render/PickupField.cpp`
  shows **no hunk** inside `PickupField::Update`, `PickupField::CollectAt` or
  `PickupField::PickRadius`, and `PickRadius`'s expression is still
  `worldR_ * 0.04f`.
- A post-fix race run still collects: the D3 orb diagnostic
  (`TrackRenderer.cpp:3862`, `OrbCount` / `ActiveOrbCount` / `PickRadius`)
  reports `ActiveOrbCount < OrbCount` at some sample, i.e. at least one orb was
  taken.
- PASS iff both hold.

---

## 3. Build and binary discipline

- Build with `mashedmod\build.bat` from PowerShell. The one-RVA-one-body lint
  must report **NEW = 0**; and a manual check that no second live copy of the
  pickup placement or pickup draw code exists is run and recorded.
- Both exes (pre-stage HEAD baseline, logging-only control, post-fix) are
  pinned with their SHA-256 under `verify/pickups_fix_20261001/bin/`.
- Standalone capture: `MASHED_RACE_DEMO=1 MASHED_GOTO=6 MASHED_DETERMINISTIC=1
  MASHED_TRACK_SEL=<i> MASHED_MUTE=1 MASHED_WIN_POS=left-bl MASHED_TITLE=...`,
  never `MASHED_NAV_DEMO`, and the driver waits for process EXIT before reading
  any capture.
- Only PIDs this session spawns are killed, and only on timeout.

## 4. Failure handling

If any rule fails, it is reported with its value. The stage's source edit is
reverted unless the failure can be shown to lie outside the fix's scope, and if
it is kept despite a failure the reason is stated explicitly in RESULT.
