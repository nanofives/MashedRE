# RESULT rhooks: per-track hook-slot draw model, scenario visibility, Dump train (U-9713, U-9716, U-9717)

**Verdict: the excluded-clump draws of five tracks now follow the original's hook slots, the race-reset scenario visibility
and SetClumpAnim are ported, all default ON with opt-outs.** On the ten capfix tracks (30 frames) the scoring goes from
979 rows to **879 rows, 0 RED** (cites `cites_rhooks.json`), matched draws **5578 -> 6405 of 7691** (72.5 -> 83.3 %),
matched triangles **86.6 -> 88.2 %**. C22 rows **50 -> 14**, C26 **6 -> 3**. G-CTRL unchanged on every arm
(**2b4d47bb / 356821c8 / f7a155e1**, base identical). No sim code changed.

**Final measurement, rebased (authoritative):** branch rebased onto `f76ae153b` (includes `07526b4e`); base2 = clean build of
`f76ae153b` (SHA-256 `ee479d133a86e908...`), final = `acb71f4db` (`633f49332129a9d2...`), both built with `MASHED_NO_DEPLOY=1`.
Ten tracks x 3 frames, 27 of 27 replays first attempt (`base2/`, `final/`, `final_off/`): **979 -> 879 rows, 0 RED, matched draws
5578 -> 6405 of 7691, triangles 86.6 -> 88.2 %**, row delta 108 gone / 8 new, every per-frame number equal to arm D on the old base
(`tables_final.md`). G-CTRL base2 = final = **2b4d47bb / 356821c8 / f7a155e1**. Opt-out (all three knobs) on the final exe: **21 of 21
draw files byte-identical to base2** (Egypt, Neustein, Highway, Sands, Roundabout, Dump, Warzone); Egypt, SuperG, Storm, Forest
default-ON files byte-identical to base2. Sections 1-6 below are the arm history on the old base `1fa71b277`.

Base `1fa71b277` (`race/first-frame-parity`), branch `work/rhooks`, worktree `.worktrees/rhooks`. Pre-registration
`PREREG_rhooks.md` (committed `0a5fd1a51` before the first port measurement) with three addenda, each committed before the
arm it predicts (`4eca933dc`, `00377be2f`, `328584f3f`).

## 1. What ran

| arm | content | exe SHA-256 (first 16) | replays | G-CTRL |
|---|---|---|---|---|
| base | clean build of `1fa71b277` | `a19dc61f7cbbba6c` | 10/10, first attempt | 2b4d47bb / 356821c8 / f7a155e1 |
| A | hook-slot draw model (groups, LOD, hidden clumps) | `2d1dd147c7d64803` | 10/10 | not run |
| B | A + slot render states + Warzone craters | `20f640917c5a7f38` | 10/10 | 2b4d47bb / 356821c8 / f7a155e1 |
| C | B + SetClumpAnim (train) | `c07be475fc423df7` | 10/10 (+1 Dump run of a first build, see Addendum 2) | 2b4d47bb / 356821c8 / f7a155e1 |
| D (final) | C + scenario visibility | `7661027a2e52f2c0` | 10/10 | 2b4d47bb / 356821c8 / f7a155e1 |

Replays: `verify/r10_20261016/run_tracks.py` (d1r1 `run_replay3d.py`, `MASHED_CAM_REPLAY_BASIS=1`, `MASHED_CAR_REPLAY=1`,
`MASHED_DBG_D3DREC=1`, no knobs), scoring `score_tracks.py` -> `re/tools/drawlist_diff.py` 3D mode. Per-frame table:
`tables_rhooks.md`. Row deltas: `rowdelta_arms.py`. G-CTRL: `run_gctrl.py` (d1r2 recipe, private ptrace via
`MASHED_PLAYERTRACE_OUT`).

## 2. Per track (final arm D vs base)

| track | matched draws (3 frames) | matched triangles | rows |
|---|---|---|---|
| Warzone | 175/175/133 -> **317/317/274** | 87.3/88.4/74.7 -> **93.9/95.1/82.4 %** | 129 -> 88 |
| Neustein | 103/99/54 -> **226/222/177** | 78.9/79.5/67.7 -> **87.2/87.9/79.9 %** | 67 -> 64 |
| Roundabout | 295/272/197 -> 306/283/208 | 83.2/90.0/88.6 -> 83.5/90.3/89.1 % | 120 -> 88 |
| Sands | unchanged 278/278/226 | unchanged | 88 -> 73 |
| Highway | unchanged 127/118/72 | unchanged | 70 -> 58 |
| Dump | unchanged 330/313/202 | unchanged | 63 -> 66 |
| Egypt, SuperG, Storm, Forest | unchanged | unchanged | unchanged |

Rows by cause (base -> final): C22 50 -> 14, C19 STATE 141 -> 120, C01 40 -> 18, C01c 26 -> 14, C04 12 -> 10, C21 80 -> 77,
C26 6 -> 3, C29 4 -> 1, C28 2 -> 3 (MISSING -> MOVED), new C30 1.

## 3. What the original does (decoded this lane, static)

Hook table `0x005f33f8` (13 x 0x48, slot k via thunk `0x0041e8b0 + 0x10*k`). Render order `FUN_00410b30`: world
`FUN_004270f0`, slot 2 (`0x004272df`), FOGENABLE 1, the switch table `FUN_004725c0` (call `0x00410b61`; `0x00691500`, 10 x
0x10, clump +0xc drawn when flag +8 set), cars, copters (`0x00410c03`), slot 10 (`0x00410c19`).

- **Mts copy group** `FUN_00479f30`: `FUN_00479e80` clones the clump's first atomic per record (no group above 0x30), each
  on a new frame := the record (`FUN_00479f10`); drawn by `FUN_00479f70`, copies 0..n-1, no frustum test.
- **LOD hook** `FUN_004758d0` (three call sites in the image: rouabout 17<-18 r60 `0x0044d325`, 19<-20 r30 `0x0044d348`,
  Warzone 20<-21 r60 `0x00452aba`); select `FUN_00557a60`: `trunc(((at.y dy + at.x dx) + at.z dz) * 10/range)` clamped
  0..9 (RC forced to chop at `0x00557b30`; Ghidra prints ROUND), slots 5..9 = L geometry.
- Per track (sites in `mashedmod/src/mashed_re/Render/TrackHookSlots.cpp`): Warzone groups A/B/C/D, slot 2 clumps 19, 20,
  group D, boat 1, river 0 (cull NONE `0x00452d16` / BACK `0x00452d5c`), slot 10 groups under ZWRITE 0 (`0x00452db2`),
  craters 9..16 as switch entries ON (`FUN_00452620`, `0x00452628`); Neustein slot 2 clumps 5, 6 + three Trunk groups
  (read from the bytes: the decompiler drops the last two calls); Sands slot 2 clumps 14..16, 5, 1, switch entry 0
  BridgeWhole ON, 18 / 19 / Jump 7 OFF (`FUN_00450300`); Roundabout slot 2 under cull NONE (`0x0044d756`) + the two LOD
  hooks; Highway: every excluded clump is drawn only from the traffic array `0x00890060` or the gated Jet.
- **SetClumpAnim** (`FUN_0047ad40`): record +0x70 clump, +0x71 anim, +0x74 time, +0x78 type; tick `FUN_00426700` from
  `FUN_00426ab0` at `0x0040fc94`, same units as the copter tick `0x0040fcb9`; type 0 `FUN_004752f0`, type != 0
  `FUN_004b47e0` -> `FUN_00532b80` (slerp, 0.999 threshold, LINEAR translation, negates key i+1 in place) -> `FUN_00532a60`.
  Dump: `local LINEAR = 1` (COURSE.LUA line 133).
- **Scenario visibility** `FUN_00471cf0` (race reset, `FUN_004111c0`): per Set_Sequence list, each scenario's objects ->
  physics membership `FUN_0047d080` and RENDER bit `FUN_0047d100` = (scenario == the list's first); a hidden object casts
  no shadow (`FUN_00481a30` tests `atomic+2 & 4`). Function entries (`Get_Function`, `2000 + i`, `FUN_00471560`) are
  skipped (`FUN_00471490`); slot 12 registers the names (`0x00426ec5`) before the Lua runs (`0x00426f14`).

## 4. Port (default ON, opt-outs)

- `Render/TrackHookSlots.{h,cpp}` (new, exe only): the five tracks' tables, `HookLodIndex` (`FUN_00557a60`).
  `TrackRenderer`: excluded clumps of a covered track are drawn only through the tables; group props; LOD per copy; slot
  order in `BuildPropOrder`; slot-10 groups after the copters. **`MASHED_NO_HOOKSLOTS=1`** restores the base handling.
- `LibRw/RwRaceSubmit`: `RaceSubmit_AddInstanceStates` (per-copy cull / zwrite, restored after the copy).
- SetClumpAnim records, `ClumpAnimTick` (inside `CopterTick`), `LinearAnimMatrix` (`FUN_004b47e0` + `FUN_00532b80`).
  **`MASHED_NO_CLUMPANIM=1`** keeps the clump static.
- Physics_Scenario_* parse and the `FUN_00471cf0` rule for RWP props and their shadows. **`MASHED_NO_SCENARIOVIS=1`**.
- No physics: the standalone has no scene-object physics, so hiding an object changes no sim input (G-CTRL identical).

## 5. Predictions

- **Pr-1** (C22 <= 20): **held**, 14 (Warzone 0, Sands 0, Highway 3, Roundabout 11).
- **Pr-2** (no new row except C22 kind changes; untouched tracks identical): **held for rows** (arm B: 98 rows gone, 2 new,
  both Roundabout railcar MOVED -> MISSING). Untouched-track draw files on arm B: SuperG, Storm, Forest, Dump byte-identical to base;
  Egypt differs in the standalone's own particle batch and wheel AABBs (see section 6), no row change.
- **Pr-3** (Neustein C29 gone): **held** (3 -> 0; the mesh was the Trunk base clump, never drawn by the original).
- **Pr-4** (LOD choice matches on all Roundabout / Warzone frames): **held**, no LOD row remains.
- **Pr-5** (G-CTRL unchanged): **held** on B, C, D.
- **Pr-6** (train): **held for the train** (MOVED by 0.0571 on 3 of 3 frames, bound 0.20), **failed for "no other Dump row"**:
  the train's roof is drawn textured by the standalone and untextured by the original (C30, 1 row + 2 off-screen C01).
- **Pr-7** (scenario rows): **held**: Roundabout Sign02 / interactiveHut and Warzone interactiveHut extras gone, two Warzone
  RWObjShad rows matched; Trucktyre and Neustein post stay; no new row.

Corrections made on the way, each recorded in an addendum before the affected measurement: the Warzone craters are drawn
(switch entries; arm A hid them, 3 C01c rows), and `LINEAR` is a COURSE.LUA local = 1 (addendum 1 had it as nil).

## 6. Controls

- **Opt-out (`arm_d_off/`).** The arm D exe with `MASHED_NO_HOOKSLOTS MASHED_NO_CLUMPANIM MASHED_NO_SCENARIOVIS` on the six
  affected tracks (Neustein, Highway, Sands, Roundabout, Dump, Warzone): **18 of 18 draw files byte-identical to base**;
  rows 0 gone / 0 new.
- **Determinism (`base_n0/`).** The base exe replayed again on Egypt and Neustein: 6 of 6 draw files byte-identical to
  the first base replay.
- **Egypt (old-base arms only).** Arms A..D give Egypt draw files that differ from base in the standalone's own particle batch (136 vs 144
  triangles) and car-wheel AABBs (under 0.003), with no row change. The arm D exe with all three opt-outs reproduces arm
  D's Egypt files, not base's, so the difference comes with the build, not with the hook logic (memory
  `sim-trajectory-is-code-layout-sensitive`). On the rebased final pair Egypt is byte-identical to base2. SuperG, Storm, Forest: byte-identical to base on arms A, B, C, D; Dump on arms A and B (C and D draw the train).
- **Instrument gates** as in r10: every frame passes except Neustein +105 I-NAME (94.8 %), unchanged on every arm.

## 7. Found on the way

1. **The decompiler drops calls after a PIC warning**: Neustein slot 2 `0x0044af10` decompiles to one group draw; the bytes
   have three (`0x0044af47`, `0x0044af53`, tail `jmp 0x0044af65`).
2. **A linear capstone sweep desynchronises** and missed `add edi,0x10134` (`0x00452628`, the craters); the byte-anchored
   `dispref.py` found it. Use both directions before calling a clump "never drawn".
3. **Ghidra's `ROUND` in `FUN_00557a60` is a chop**: the function forces RC = 11 before the `fistp`.
4. **Warzone's Lua adds the Hut C walls to `HUT_B_DESTROYED_SCENARIO`** (COURSE.LUA 394..397); the port follows it literally.
5. **`trainroof` exists only in CITY.TXD** inside dump.piz; the original draws the roof untextured (U-9724).
6. Neustein's posts: the original draws none of Post_1's 49 records at these frames; creation (`FUN_00480340` ->
   `FUN_0047ff70`) has no cap and no scenario holds them. Cause not decoded (U-9716).

## 8. Open (new U-rows)

U-9721 Roundabout railcars (3 world clones of clump 2 on spline course+0x100b8, scene physics bodies, `FUN_0044c8b0`,
`FUN_0044c4f0`, slot 1 `FUN_0044c920`); U-9722 Roundabout crane pose (slot 1 `FUN_0044cb00`, two atomics the original does
not draw); U-9723 Highway traffic (array `0x00890060`, slots 0 / 1 / 2) and the Jet gate `DAT_005f9748`; U-9724 TXD scope;
U-9725 SetClumpAnim one-tick lead on the replay rows; U-9726 hook-slot state that changes after the race starts (Sands bridge
stages and Debris, Warzone crater switch-off `0x0045260b`, boat `BoatAnim`, chunk flags). Leaf sprites (RpPTank
`FUN_00534b60(0x800, 0x87, 0)` at `0x0047c330`, per-frame `FUN_0047cbb0`, draw `FUN_0047cd30`) extend U-9701.

## 8b. The Warzone pontoon (track object 0x00684d18, orchestrator request; decoded, NOT ported)

seek414f00 ported the AI reader `0x00414f00` of this object and filed its 0 -> 1 trigger as U-9719. The object belongs to the
Warzone hook record: slot 0 `0x004527f0` builds it (`FUN_00451760(&DAT_00684d18, clump 1 Boat.dff, anim table +0x105b0)`, an
RpHAnim hierarchy, `FUN_004b4790`), slot 1 `0x00452b20` ticks it (`FUN_00451d80` at `0x00452ba2`), slot 2 draws it
(`FUN_00452130`, `0x00452d68`; this lane draws it at its rest pose, which matches the original on the 3 Warzone frames), slots
6 / 7 reset it (`FUN_00451cc0`).

- **Trigger (answers U-9719's question):** case 0 calls `FUN_00451820` @ 0x00451820: per car, car matrix x inverse(object
  matrix `[obj+0x94]`) (`FUN_004c4dc0`, `FUN_004c52f0` combine 2); x in (`[obj+0x20]`, `[obj+0x14]`) and z in
  (`[obj+0x28]`, `[obj+0x1c]`) sets `[obj+0x48] = 1`.
- **States:** 1 counts `[obj+0xa4]` from 1.0 down by units x `[0x005ce388]`, then 2; 2 adds units x `[0x005ce384]` to the
  anim time `[obj+0xc8]` (`FUN_004b47c0`) and goes to 3 once no car word `0x0088ff50..5c` is set and the time exceeds 2.0
  `[0x005cc574]`; 3 plays back to 0 (`FUN_0052df70`), then `FUN_00451cc0`; 4 holds; 5 waits for the object to leave the
  frustum. Every case copies `[obj+0x94]` to `obj+0x54` and calls `FUN_00451ad0`.
- **Car coupling:** `FUN_00451ad0` writes `0x0088ff50[v]` from the box test `FUN_00451990`; a car that has just left the box
  gets the object's movement `[obj+0xa8..0xb0]` (`FUN_0046bf50`) and a four-wheel test (`FUN_0041f1e0`, `FUN_00481750`,
  `FUN_00480720`). Slot 0 hides the Tower (label 1000) and Deck (label 2000) RWP objects' RENDER bit; the Deck stays a physics
  body the cars ride, its matrix reset by the callback `0x00452170` (`FUN_0047d180`).

**Why not ported here:** the producer of `0x0088ff50` / `0x00684de0` / `0x00684dac` is coupled to the car physics
(`FUN_0046bf50`, the wheel test) and to the Deck physics body, and the standalone has no scene-object physics. Writing only the
words would let the ported AI reader seek a pontoon the standalone's cars cannot ride, a sim change with no faithful
counterpart. It needs its own lane: RpHAnim hierarchy update (`FUN_00533ec0`) for Boat.dff / BoatAnim, the state machine, the
car coupling, a Deck collision body, then the Warzone exe AI measurement seek414f00 set up (`run_exe_arms.py`, mode-10 rows)
and D2 / D3. User decision listed in the final report.

## 9. Reproduce

```
py -3.12 verify\r10_20261016\run_tracks.py --exe <exe> --out <dir>            [MASHED_NO_HOOKSLOTS MASHED_NO_CLUMPANIM MASHED_NO_SCENARIOVIS]
py -3.12 verify\rhooks_20261016\make_cites_rhooks.py
py -3.12 verify\r10_20261016\score_tracks.py --work <dir> --prefix dr_ --cite verify\rhooks_20261016\cites_rhooks.json
py -3.12 verify\rhooks_20261016\rowdelta_arms.py <base dir> base_ <dir> dr_
py -3.12 verify\rhooks_20261016\run_gctrl.py <exe> <name>
py -3.12 verify\rhooks_20261016\clumpref.py 0x00448700 0x0045b400 ; py -3.12 verify\rhooks_20261016\dispref.py 0x10110 0x10210
```
