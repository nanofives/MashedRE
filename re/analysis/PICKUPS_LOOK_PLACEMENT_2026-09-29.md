# Power-up pickups: why the standalone draws glowing blobs, and why they sit in the wrong places

> **STATUS 2026-10-01.** Part B (placement) is FIXED and verified — commit `c9615225`,
> acceptance `verify/pickups_fix_20261001/RESULT_STAGE1.md`. Part A (the look) is NOT started;
> the blocker is the acceptance instrument, not the code (see that RESULT and
> `re/NEXT_SESSION.md`). Two claims below were corrected by measurement while landing it:
>
> 1. **§2.1's user-data decode.** The dword is read from RW USERDATA (`0x011f`) **array 0,
>    element 0**, positionally, exactly as `FUN_004b5190(atomic, 0, 0)` does — never by name.
>    Many geometries carry a **second** array (`FVF.UserData` on training and Warzone), and on
>    `sands` and `rouabout` the second array has the **same name** as the first, with twelve
>    elements. A name lookup lets it shadow array 0 and returns the wrong type for 3 of sands's
>    25 markers and 14 of rouabout's 25. Byte-level evidence: `sands` geo 8 has
>    `numUserDatas = 2`, array 0 `nameLen=13 '0.tv_part_id' dtype=1 count=1 v0=0x0507`.
> 2. **§2.3's `rouabout` / `sands` [UNCERTAIN] is CLOSED** by the same finding — the "39
>    user-data elements vs 25 geometries" was that duplicate array, not a mapping ambiguity.
>    With array-0 indexing all 13 tracks parse clean (`re/tools/powerups_gold_dump.py --all`):
>    rouabout 25 atomics, 18 BLANK, **7 non-blank**; sands 25, 16 BLANK, **9 non-blank**.
>
> Also measured 2026-10-01, closing part of §5's first [UNCERTAIN]: `DAT_0067ea74` reads **1**
> on Quick Battle (TRAINING) and on Challenge Cup entry 3 (ARCTIC), and **0** on two other
> routes, which place nothing at all. What *drives* it is still underived.

Session 2026-09-29, branch `race/first-frame-parity`. INVESTIGATION only — no source under
`mashedmod/src` was touched and `mashedmod\build.bat` was not run (another session holds the build).

Standalone binary under test — copied before the run, run only from the copy:

```
verify/pickups_20260929/bin/mashed_re.exe
SHA-256  dafdaf5e44a32c53cdc00b41fc619c9396b50a70a60babae4ce5aebaa81c160e
git HEAD at copy time  b5923d20e5503f6a83efa5283d3657eb5358284b
```

`mashedmod/build/` contains no DLLs, so nothing else needed copying. All standalone runs used
`MASHED_MUTE=1 MASHED_TITLE="pickup investigation" MASHED_WIN_POS=left-bl`. All original-side runs
went through `re/tools/run_with_unlocked_save.py` (gamesave restored and sha-verified after each,
`bd18788182b2343e`). `original/` was not modified.

Ghidra evidence came from a read-only pool slot (`Mashed_pool0`, acquired and released) driven by
`re/tools/decomp_pc.py` / `disasm_va.py` — the Ghidra MCP would not load this session. No master write.

---

## 1. MEASURED — Part A, the look

### 1.1 What the ORIGINAL draws

Captured live: `verify/pickups_20260929/orig_qb_full.png` (640x480, Quick Battle, TRAINING) and the
crop `verify/pickups_20260929/orig_qb_boxcrop.png` (source rect 266,38..314,86, nearest-8x).
The pickup at world `(-1.009, 0.270, -20.220)` is visible: a **small textured cube carrying a
readable icon**, wrapped in a **blue four-pointed glow sprite**.

The code behind that image:

| Element | What it is | RVA / address |
|---|---|---|
| Box mesh | `RpClump` streamed from `IconCube.dff`, **25 independent copies** | name string `0x005ce5a8`; read loop `0x00458820`..`0x0045883d` in `FUN_004587a0`; `FUN_004b3e40` -> `RpClumpStreamRead` `0x004e7420` |
| Box geometry | 14 verts, 12 tris, **one** material, cube half-extent **0.1874** | measured from `Powerups.piz :: ICONCUBE.DFF` (authored material/texture name `Oil`) |
| Box draw | clump render (walks `+8` atomic list, invokes each atomic's render callback at `node[2]`) | `FUN_004e6680` @ `0x004e6680`, called at `0x00458b34` inside `FUN_00458b10` |
| Icon texture | per **type**, `RwTexDictionaryFindNamedTexture`-shape lookup in the `PowerUpIcons.txd` dictionary handle `DAT_0068b9ac` | `FUN_00458630` @ `0x00458630` (12 call sites `0x00458647`..`0x0045872a` into `FUN_004c5c00` @ `0x004c5c00`); txd name `0x005ce5b8`, handle written `0x004587e6` |
| Icon bind | `RpMaterialSetTexture`-shape on the clump's first material | `FUN_00458dd0` @ `0x00458dd0` -> `FUN_004b4080` then `FUN_004e8090` @ `0x00458de9` |
| Spin | **Y-axis rotation, 1.0 degree per frame** | increment `+0x08` init `0x3f800000` (1.0f) and axis `+0x0c` init `(0, 0x3f800000, 0)` = (0,1,0), both in `FUN_004587a0`; applied by `RwFrameRotate`-shape `FUN_004c1520` in `FUN_00458a40`; degrees confirmed by the deg->rad multiply `_DAT_005cd7a8` = `0x3c8efa35` = 0.0174532924 at `0x00459210` |
| Bob | **none.** No sine on Y anywhere in the box path | only positional motion is a damped drift `*0.949999988` (`0x005cc9dc`) with spring-back `*0.05` (`0x005cc9a0`) below `1e-5` (`0x005cc990`), in `FUN_00458a40` |
| Glow | **separate sprite batch**, texture `puglow`, 25 elements | `FUN_004770c0(&DAT_0068b968, 0x817, 0x19, puglow)` at `0x004587cd`; name string `0x005ce5cc`; rendered `FUN_004770a0` at `0x00458b25` with Z-write off (`RwRenderStateSet(8,0)` at `0x00458b10`) |
| Glow size | base **1.25 x 1.25**, shrunk by `clamp(1 - |pos-anchor|^2, 0, 1)` | `DAT_005ce540` / `DAT_005ce544` = `0x3fa00000`; clamp hi `0x005cd0c8` (double 1.0), lo `DAT_005d757c` (0.0) |
| Glow blend | plugin `+0xa4` = `DAT_006132c4` = **5**, `+0xa8` = `DAT_006132c8` = **6** (SRCALPHA / INVSRCALPHA), **not additive** | written in `FUN_004770c0` |
| Glow colour | per **type**, dword table at `DAT_005f9838`, stride 4, index = type id | submit `FUN_004769f0(&DAT_005f9838 + type*4)` at `0x004591f8` |
| Cull | skip when `dot(pos-camPos, camAt) <= 0` or `|pos-camPos|^2 >= 800.0` | `FUN_00458b10`; far constant `_DAT_005ce5d4` = `0x44480000` = 800.0 |
| Respawn look | state 2 draws the **glow only**, size `1.25 * sin(pi * (respawn-timer) / 1.0)`, no cube | `FUN_00459000`; pi `_DAT_005ce2f4` = `0x40490fdb`; window `_DAT_005ce548` = `0x3f800000` |
| Collision | sphere radius **0.5**, tag 7, callback `FUN_00458920` | `FUN_00484cf0(&local_98)` at `0x00459228`; radius literal `0x3f000000` |

`DAT_005f9838` raw dwords (index = type id):

```
[0..6]=0xffffffff [7]=0xfff1c12d [8]=0xffffffff [9]=0xff00e0ff [10]=0xff0d8a51
[11]=0xffac78f1  [12]=0xff241cec [13..15]=0xffffffff [16]=0xff1d94f7
[17]=0xffab3578  [18]=0xff84ff84 [19]=0xffff0000 [20]=0xffffffff [21]=0xffffffff
```

Byte order **measured, not assumed**: the captured box is type 19, whose dword is `0xffff0000`;
read as RwRGBA bytes in increasing address order that is `r=0 g=0 b=255 a=255` = blue, and the
captured glow is blue. So the table is `{R,G,B,A}` bytes, little-endian dword.

`FUN_00458630`'s type -> icon-name map (every case calls `FUN_004c5c00(DAT_0068b9ac, <name>)`):

| type | icon name (string VA) |
|---|---|
| 2, 16 | `flamethrower` `0x005ce568` |
| 6, 12, 13 | `mine` `0x005ce594` |
| 7 | `mortar` `0x005ce58c` |
| 9 | `machinegun` `0x005ce59c` |
| 10 | `depthcharge` `0x005ce580` |
| 11 | `missile` `0x005ce578` |
| 17 | `shotgun` `0x005ce560` |
| 18 | `flare` `0x005ce558` |
| 19 | `oil` `0x005ce4fc` |
| 21 | `chaos` `0x005ce550` |
| 22 | `airstrike` `0x005cd130` |
| default | returns 0 -> the material gets a NULL texture |

Eleven names, matching the 11 textures in `POWERUPS/Powerups.piz :: POWERUPICONS.TXD` and the 11
lines of `original/TOASTART/Common/POWERUPS/textures/PowerUpIcons.lst`.

### 1.2 What the STANDALONE draws instead

Captured: `verify/pickups_20260929/sa_arctic_t24_ctx.png` (right half of the 512x384 frame) and the
crop `verify/pickups_20260929/sa_arctic_t24_orbcrop.png` (source rect 440,60..512,150 of the full
frame). A featureless cyan blob. No icon, no box, no structure.

`mashedmod/src/mashed_re/D3d9Render/PickupField.cpp`:

- `EnsureTexture` (line 64) **generates a 32x32 A8R8G8B8 radial gradient at runtime**: white RGB,
  alpha 1.0 inside r<0.55 falling to 0 at r=0.9 (lines 77-85). That is the only texture the class
  ever binds (line 253).
- `Render` (line 222) builds **camera-facing quads by hand** (lines 241-246), two triangles per orb,
  half-size `worldR_ * 0.018` (line 232).
- Blend is **additive**: `SRCBLEND=SRCALPHA, DESTBLEND=ONE` (lines 267-268).
- Colour comes from `kKindCol[5]` (lines 16-22) via `ColForType` -> `KindFromType`, which the source
  itself marks `[SCAFFOLD] ... an INVENTED stand-in` (line 92-96).
- There is a **bob** the original does not have: `sin(phase_ * 2.0) * worldR_ * 0.008` (line 231).
- There is **no spin** despite the header claiming "bobbing + spinning billboards"
  (`PickupField.h:3`) — nothing rotates the quad.

### 1.3 Root cause, Part A

**The icon is not missing because the texture failed to load. It is loaded and then never reached.**

`LoadPowerupIcons()` (`mashedmod/src/mashed_re/exe_main.cpp:6783`) decodes **all 11**
`POWERUPICONS.TXD` textures and registers them — measured this session, `mashed_re.log` line
`powerup icons: dict=11 loaded=11 ready=1`. But it uploads them only into the **frontend 2D quad
block**, slots `kSlotPowerup0` = 69..79 and Im2D handles `kHandlePowerup0` = 60..70
(`exe_main.cpp:866-869`), for the s24/s18 preview row. `PickupField` has no reference to that block
and no reference to any TXD; it bypasses the whole thing and manufactures its own glow bitmap.

So: **scaffold draw, with the correct textures already resident and unbound.** Grep confirms
`mashedmod/src` contains zero references to `ICONCUBE`, `PUGLOW`, `POWERUPMODELS` or
`POWERUPS_GOLD.DFF`; the only mention of the pool is a comment in `qol_asi/mashed_qol.cpp:973-977`.

A useful asset already in the tree: `exe_main.cpp:851` `kPowerupIdToIcon[23]` is a transcription of
`FUN_00458630`'s type->icon mapping into indices of the same `kNames[]` order the loader uses.

---

## 2. MEASURED — Part B, the placement

### 2.1 Where the ORIGINAL gets pickup positions

Tested, not assumed. The loader is `FUN_004264d0` @ `0x004264d0`:

```
0x0042652e  apcStack_120[1..3] = "powerups_gold.lua"  (0x005cd4f8)   -- same string for all 3 ranks
            path = "toastart/tracks/" (0x005cd4c0) + trackname + DAT_005cd280
            FUN_00458c70()                      -- reset all 25 pool slots
            rank = FUN_0042fe30()
0x004265a6  if (rank == 0) return               -- NOTHING is placed
0x004265b4  FUN_00495280(&path)                 -- open the track piz
0x004265bd  PUSH 0x005cd4e4  ("powerups_gold.dff")
0x004265c2  CALL FUN_0042a5d0                   -- DFF clump loader
0x004265ce  JE 0x004265df                       -- clump == 0 -> Lua fallback
0x004265d2  CALL FUN_00426460                   -- <-- THE LIVE PLACEMENT PATH
0x004265d8  RpClumpDestroy
0x004265df  fallback: FUN_0042a470(0, "powerups_gold.lua", 1) -> FUN_0047a130
0x004265f5            FUN_0042a470(0, "powerups.lua", 1)      -> FUN_0047a130
```

`FUN_00426460` @ `0x00426460` (`__fastcall`, clump in ECX):

```c
n = FUN_004b3fc0(clump, local_80);         // RpClumpForAllAtomics-shape, stack cap 32
for (i = 0; i < n; i++) {
    frame = *(int *)(atomic + 4);          // RwObject.parent
    v     = FUN_004b5190(atomic, 0, 0);    // RW user-data plugin, array 0, element 0
    FUN_00458fd0(frame + 0x40, v & 0xff, (float)(v >> 8));
}
```

`frame + 0x40` is the modelling-matrix position (RwFrame modelling matrix at `+0x10`, RwMatrix pos
at `+0x30`).

**The user-data dword is decoded here for the first time (this resolves the Ghidra pass's open
item 1).** Every `POWERUPS_GOLD.DFF` carries one RW USERDATA chunk (`0x011f`) per geometry, element
name `0.tv_part_id` (`0.Part_ID` on `rouabout`), dataType 1, count 1. Measured values across three
tracks are all of the form `0x05NN`:

```
training[12] = 0x0513 -> type 19 respawn 5      Arctic[10] = 0x0509 -> type 9  respawn 5
training[ 7] = 0x0507 -> type  7 respawn 5      City[ 4]   = 0x050b -> type 11 respawn 5
training[ 0] = 0x050c -> type 12 respawn 5      City[26]   = 0x0512 -> type 18 respawn 5
```

so **`tv_part_id` = `type | (respawn_seconds << 8)`**, exactly the split `FUN_00426460` performs.
Full dump: `verify/pickups_20260929/orig_pickup_positions.csv`.

**Live confirmation.** `re/frida/pickup_pos_probe2.py` (read-only `Memory` reads, no `Interceptor`)
reads the 25-slot pool and the live count. Quick Battle on TRAINING, settle 16 s, 6 samples, all
agreeing — `verify/pickups_20260929/orig_qb.bmp.pickuprecs.json`, log `orig_qb.log`:

| slot | state | type | respawn | anchor (x,y,z) | matches |
|---|---|---|---|---|---|
| 0 | 1 | 19 | 5.0 | (-1.009, 0.270, -20.220) | TRAINING DFF atomic 12, `tv_part_id` 0x0513 |
| 1 | 1 | 7 | 5.0 | (16.800, -0.624, 0.800) | DFF atomic 7, 0x0507 |
| 2 | 1 | 19 | 5.0 | (15.900, 0.140, 25.400) | DFF atomic 2, 0x0513 |
| 3 | 1 | 9 | 5.0 | (-0.907, 0.270, 24.051) | DFF atomic 1, 0x0509 |
| 4 | 1 | 12 | 5.0 | (1.882, 0.669, -3.097) | DFF atomic 0, 0x050c |

Live count **5**. Every position, every type and every respawn time is the DFF's, to the last
decimal. Three of the five (`-1.009,0.270,-20.220`, `-0.907,0.270,24.051`, `1.882,0.669,-3.097`)
are **not** in `TRAINING :: POWERUPS_GOLD.LUA` at all, which has `(-1.6,0.27,-16.8)`,
`(0.4,0.27,28.0)` and `(2.0,0.65,-3.5)` in their place. Each record also carries spin increment
1.0 and axis (0,1,0), confirming §1.1.

**Why 5 of 17.** `FUN_00458e00` @ `0x00458e00` (the spawn function `FUN_00458fd0` feeds) rejects
type 21 (BLANK) outright unless `rank == 2`:

```
rank = FUN_0042fe30()
if (DAT_0068b9a8 > 0x18) return -1                 // pool cap 25
if any existing anchor within |d|^2 < 0.00100000005 (_DAT_005cc558 = 0x3a83126f) return -1
if (rank == 2) { if (type != 0x15) return -1; type = FUN_00458d00(); entry+0x28 = 1; }
else if (type == 0x15) return -1
```

TRAINING's DFF has 17 atomics, 12 of them type 21 -> exactly 5 survive. `FUN_00458d00` @
`0x00458d00` is the BLANK randomiser: table `{12,19,18,17,16,11,10,9,7}` with `FUN_00472690(0,8)`,
or `{12,18,17,16,11,10,9,7}` with `FUN_00472690(0,7)` when `FUN_004576b0() != 0`.

**Negative control, and it is a real one.** The Challenge-Cup route (`--challenge 3`, which also
loaded TRAINING) gave live count **0** with all 25 records zeroed —
`verify/pickups_20260929/orig_recs.log`. That is `FUN_004264d0`'s `rank == 0 -> return` at
`0x004265a6`. The Quick Battle run above is the positive control that proves the probe reads the
right array, so the zero is a real "nothing placed", not a broken read.

**The Lua files are the fallback and are dead on shipping data.** Every one of the 13 track pizzes
contains `POWERUPS_GOLD.DFF`, so the `JE` at `0x004265ce` is never taken. `powerups_gold.lua` has
xrefs only at `0x0042652e`/`0x0042653d`/`0x00426541`/`0x00426545`; `powerups.lua` only at
`0x004265f5`/`0x00426608`; both inside `FUN_004264d0`'s fallback arm. `powerups_all.lua`
(`0x005ce628`, single xref `0x0045bb78` in `FUN_0045bae0`) contains **no positions at all** — only
`Set_Track_Powerups(course, medal, t1..t5)`, the per-course/medal availability roster written by
`0x0047b280` into the record from `FUN_00430250`. `COURSE.LUA` in Arctic and City contains zero
`powerup` lines; `Powerup_Filename` exists as a key (`0x005cde80`, handler `FUN_0047aa50`) but is
commented out in every shipped `COURSE.LUA` checked (Highway and Storm carry
`--Powerup_Filename("filename.dff")`). `LAPDATA.LUA` (`0x005cd578`) is xref'd only from
`FUN_00426e10` and places nothing. No spline or AI-gate derivation exists anywhere in this chain.

### 2.2 Where the STANDALONE gets pickup positions

`TrackRenderer.cpp:1348-1405` text-scans the track piz for the **first entry whose name starts with
`POWERUPS`** and `sscanf`s `Set_Current_Position` / `Set_Current_Type` /
`Set_Current_Respawn_Time` / `Place_Powerup` out of it, resolving type names from the `local NAME =
id` lines. `InitPickups()` (`TrackRenderer.cpp:3808`) then calls `pickups_.InitReal(...)`, or falls
back to `pickups_.Init(spots, R)` — "every 8th gate gets an orb" (`PickupField.cpp:118-129`) — when
the parse yielded nothing.

So the standalone reads **exactly the file the original never opens**, and when that file is absent
it invents positions off the AI gate ribbon.

Measured this session (`mashed_re.log`, line `R4 pickups: N real powerup spawns`):

- Arctic: `R4 pickups: 18` -> 18 orbs at the LUA positions.
- TRAINING: `R4 pickups: 8`.
- Storm: `R4 pickups: 0` -> gate fallback; the orb diagnostic
  (`verify/pickups_20260929/sa_storm_pu.csv.orbs.csv`) reports **7 orbs**, `pick_r` 1.968, placed at
  gate centres.

`InitReal` also lifts every orb by `worldR_ * 0.012` above the authored Y
(`PickupField.cpp:139`), and `Init` by `worldR_ * 0.02` (line 122). The original writes the DFF
frame position verbatim into both `+0x2c..0x34` and `+0x38..0x40` (`FUN_00458e00`) — no lift.

### 2.3 Position comparison table

`DFF box` = atomics in `<track>.piz :: POWERUPS_GOLD.DFF` (the original's placement source).
`orig placed` = what `FUN_00458e00` actually accepts in a normal race (rank not 0 and not 2) = the
non-BLANK subset; in rank 2 it is instead the BLANK subset, randomised; at rank 0 it is 0.
`SA orbs` = what the standalone places. `LUA pos also in DFF` = how many of the standalone's
positions coincide exactly with a real box position.

| track | DFF boxes | non-BLANK (orig placed, normal race) | BLANK (orig placed, rank 2) | SA orbs | LUA pos also in DFF |
|---|---|---|---|---|---|
| Arctic | 23 | 7 | 16 | 18 | 4 / 18 |
| City | 32 | 10 | 22 | 18 | 4 / 18 |
| dump | 25 | 9 | 16 | 19 | 8 / 19 |
| Egypt | 19 | 7 | 12 | 17 | 7 / 17 |
| Forest | 30 | 10 | 20 | 19 | **0 / 19** |
| Highway | 27 | 11 | 16 | 14 | 4 / 14 |
| Neustein | 24 | 8 | 16 | gate fallback | no LUA |
| rouabout | 25 | [UNCERTAIN] | [UNCERTAIN] | 13 | 5 / 13 |
| sands | 25 | 9 | 16 | gate fallback | no LUA |
| Storm | 19 | 7 | 12 | **7 (gate fallback, measured)** | no LUA |
| SuperG | 31 | 7 | 24 | gate fallback | no LUA |
| training | 17 | **5 (measured live)** | 12 | 8 | 3 / 8 |
| Warzone | 30 | 14 | 16 | 16 | 3 / 16 |

Per-position detail for all 13 tracks, both sources, with decoded type and respawn:
`verify/pickups_20260929/orig_pickup_positions.csv`.

[UNCERTAIN] `rouabout` (39 user-data elements vs 25 geometries) and `sands` (28 vs 25) have more
user-data chunks than geometries, so my sequential geometry mapping is not safe there and the
type split is not reported. The position list is unaffected (positions come from frames, not
user-data). Next command to resolve: extend `re/tools/dff_dump.py` to walk the GEOMETRY extension
chunk per geometry index instead of scanning the whole blob for the key string, then re-run the
table generator.

### 2.4 Root cause, Part B

Three independent defects, in order of size:

1. **Wrong source file.** The standalone parses `POWERUPS_GOLD.LUA`; the original places from
   `POWERUPS_GOLD.DFF` (`FUN_004264d0` `0x004265bd`..`0x004265d2`) and only falls back to the Lua
   when the DFF is missing, which never happens on shipping data. The two files disagree: on Forest
   they share **zero** positions, on Arctic and City 4 of 18.
2. **No BLANK filter and no rank gate.** The standalone places every parsed spawn. The original
   rejects type 21 unless `rank == 2` (`FUN_00458e00`), and places nothing at all when
   `FUN_0042fe30()` returns 0 (`0x004265a6`) — measured live as count 0 on the Challenge route.
3. **Invented fallback.** On the four tracks with no `POWERUPS_GOLD.LUA` (Neustein, sands, Storm,
   SuperG) the standalone puts orbs on the AI gate ribbon, which is pure invention — Storm measured
   at 7 gate-centre orbs while the original places 7 boxes at completely different, authored
   positions.

---

## 3. REFUTED

- **"The icons were never loaded."** They are. `powerup icons: dict=11 loaded=11 ready=1` in
  `mashed_re.log` on every standalone run this session. They are loaded into the frontend 2D quad
  block and PickupField never looks at it.
- **"`POWERUPS_GOLD.LUA` is the original's placement data."** It is the fallback branch only, and
  the fallback is unreachable on every shipped track. Live positions match the DFF, not the Lua.
- **"The per-track `POWERUPS_GOLD.DFF` is track art."** Its geometries are 8-vertex, 12-triangle,
  materially-untextured grey cubes (`mats=[(None,(102,102,102,255))]`, half-extent 0.1353) with one
  user-data int each. They are placement markers. The rendered object is `ICONCUBE.DFF` from
  `Common/POWERUPS/Powerups.piz` (14 verts, textured, half-extent 0.1874).
- **"`DAT_0068d1f0` is the per-slot pickup-box state."** It is a **4-entry per-player** array
  (`FUN_0045bba0`'s loop counter over the 4 powerup slots at `0x0088fc70`, stride `0xb4`, bound
  `0x0088ff40`). The world box state is `0x0068b198 + i*0x50 + 0x20`. Prior note
  `D3_CONTACT_PORT_2026-09-28.md` / U-9139 should be read with that correction.
- **"The original's glow is additive."** Blend literals written in `FUN_004770c0` are 5 and 6
  (SRCALPHA / INVSRCALPHA). The standalone's `DESTBLEND=ONE` is not what the original does.
- **"The orb bobs in the original."** No sine on Y exists in the box path. The standalone's bob is
  invented; the spin the standalone's header claims does not exist in the standalone's code.

---

## 4. READY-TO-APPLY FIXES

Not applied — this is an investigation session and another child holds the build.

### Fix A1 — bind the real per-type icon (smallest change that kills the blob)

**File** `mashedmod/src/mashed_re/D3d9Render/PickupField.cpp` / `.h`, plus one accessor in
`exe_main.cpp`.

1. Give `PickupField` a texture-provider hook, e.g.
   `void SetIconProvider(IDirect3DTexture9* (*fn)(int icon_idx));`, and have `exe_main.cpp` supply
   `[](int i){ return g_quad_renderer.slot_texture(kSlotPowerup0 + i); }`. The 11 textures are
   already resident (`LoadPowerupIcons`, `exe_main.cpp:6783`); nothing new is loaded.
2. Select the icon index from the real type with the existing transcription
   `kPowerupIdToIcon[23]` (`exe_main.cpp:851`), which is `FUN_00458630`'s map. Type with index -1
   draws **nothing** — the original's `default: return 0` gives the material a NULL texture, and
   `FUN_00459000`'s draw loop skips a null sprite.
3. In `Render`, call `dev->SetTexture(0, icon)` per orb instead of the one shared `tex_`. That
   means one `DrawPrimitiveUP` per distinct icon (group orbs by icon index; at most 11 batches,
   typically 3-5).
4. Replace the invented `kKindCol` tint with `DAT_005f9838[type]` (table in §1.1), read as
   `{R,G,B,A}` bytes. Delete `KindFromType`'s use for colour.
5. Change `DESTBLEND` from `D3DBLEND_ONE` to `D3DBLEND_INVSRCALPHA` (`PickupField.cpp:268`) and
   drop the bob at line 231.

**Expected effect:** the blob becomes a readable per-type icon with the original's colour and
blend. Cited: `FUN_00458630` `0x00458630`, `FUN_00458dd0` `0x00458dd0`, `DAT_005f9838`,
`FUN_004770c0` `0x004770c0` blend literals 5/6.

### Fix A2 — draw the real object (full fidelity)

**File** `mashedmod/src/mashed_re/D3d9Render/PickupField.cpp`, plus a loader in `TrackRenderer.cpp`
next to the existing `load_prop` helper.

1. Load `Common/POWERUPS/Powerups.piz :: ICONCUBE.DFF` once through `Track::DffModel` +
   `BuildDffBatches` (the same path the track props already use), and load
   `Powerups.piz :: PUGLOW.PNG` for the glow sprite.
2. Draw per live orb: the cube at the box position, world-Y rotation of `angle` degrees where
   `angle += 1.0` per frame (`+0x08` init `0x3f800000` in `FUN_004587a0`; `RwFrameRotate`-shape
   `FUN_004c1520` in `FUN_00458a40`; deg->rad `0x005cd7a8` = 0.0174532924), cube half-extent
   **0.1874 world units, absolute — not scaled by `worldR_`**.
3. Draw the glow as a camera-facing `puglow` quad of **1.25 x 1.25** absolute
   (`DAT_005ce540`/`544` = `0x3fa00000`), Z-write off, before the cubes (`FUN_00458b10`
   `0x00458b10`/`0x00458b25`).
4. Cull exactly as `FUN_00458b10`: skip when `dot(pos - camPos, camAt) <= 0` or
   `|pos - camPos|^2 >= 800.0` (`_DAT_005ce5d4` = `0x44480000`).
5. Respawn state: glow only, size `1.25 * sin(pi * (respawn - timer) / 1.0)`, cube hidden
   (`_DAT_005ce2f4` = `0x40490fdb`, `_DAT_005ce548` = `0x3f800000`).

**Expected effect:** visually matches `verify/pickups_20260929/orig_qb_boxcrop.png`.

### Fix B1 — place from `POWERUPS_GOLD.DFF`, not the Lua

**File** `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`, the block at lines 1347-1405.

Replace the `POWERUPS*` text scan with:

1. Find entry `POWERUPS_GOLD.DFF` in the track piz. Parse it with the existing DFF parser.
2. For each atomic: position = its frame's world translation (parent chain, as
   `re/tools/dff_dump.py` does); `v` = the geometry's RW USERDATA (`0x011f`) array-0 element-0
   int32; `type = v & 0xff`, `respawn = (float)(v >> 8)`.
3. Keep the current Lua text parse **only** as the fallback when the DFF entry is absent — mirrors
   `FUN_004264d0`'s `JE 0x004265df`. It will not fire on shipping data.
4. Drop the "every 8th gate" fallback in `PickupField::Init` from the race path entirely (leave the
   method for dev use, or delete it): the original has no such behaviour, and it is what puts orbs
   on Neustein/sands/Storm/SuperG where no box belongs.

**Expected effect:** Arctic goes from 18 Lua positions (4 of them real) to the 23 authored box
positions; Storm/SuperG/sands/Neustein stop inventing.

### Fix B2 — apply the original's spawn filter

**File** `mashedmod/src/mashed_re/D3d9Render/PickupField.cpp` (`InitReal`) or the caller.

Port `FUN_00458e00` @ `0x00458e00` verbatim:

- Cap at **25** boxes (`if (0x18 < count) reject`).
- Dedupe: reject a spawn whose squared distance to an existing anchor is
  `< 0.00100000005` (`_DAT_005cc558` = `0x3a83126f`).
- Rank gate. `rank = FUN_0042fe30()` (`0x0042fe30`: `FUN_0042f6a0() != 0xb ? DAT_0067ea74 : 1`):
  - rank 0 -> place nothing at all (`FUN_004264d0` `0x004265a6`).
  - rank 2 -> accept **only** type 21, and replace the type with `FUN_00458d00()`
    (`0x00458d00`: table `{12,19,18,17,16,11,10,9,7}` via `FUN_00472690(0,8)`; the 8-entry variant
    `{12,18,17,16,11,10,9,7}` via `FUN_00472690(0,7)` when `FUN_004576b0() != 0`).
  - any other rank -> **reject type 21**, accept the rest verbatim.
- Store the position unchanged into both anchor and current position. **Remove the
  `worldR_ * 0.012` Y lift** at `PickupField.cpp:139`.
- Set the collection radius to the original's **0.5** sphere (`0x3f000000` at `0x00459228` /
  `FUN_00484cf0`) instead of `worldR_ * 0.04` (`PickupField.cpp:159`, `PickRadius()` in the header),
  which measured 1.101 on TRAINING and 1.968 on Storm — 2x to 4x too large.

**Expected effect:** TRAINING in a normal race becomes exactly the 5 boxes measured live
(types 19, 7, 19, 9, 12, respawn 5 s each). Arctic becomes 7.

---

## 5. OPEN

- [UNCERTAIN] `FUN_0042fe30`'s return semantics (`DAT_0067ea74`, or 1 when `FUN_0042f6a0() == 0xb`).
  It gates placement entirely at 0 and switches to BLANK-only at 2. The existing plate calls it a
  medal-rank getter; that was not re-derived here.
  Next command: `py -3.12 re\tools\decomp_pc.py 0x0067ea74 --datarefs`.
- [UNCERTAIN] How the BLANK randomiser interacts with `Set_Track_Powerups` / `powerups_all.lua`.
  `FUN_00458d00`'s table is a fixed 9-entry list, not the 5-type per-course roster
  `0x0047b280` writes, so the roster must be consumed somewhere else.
  Next command: `py -3.12 re\tools\decomp_pc.py 0x00430250 --callers` then decompile the readers of
  the record it returns.
- [UNCERTAIN] `rouabout` / `sands` user-data-to-geometry mapping (§2.3).
- [UNCERTAIN] `Powerup_Filename` (`FUN_0047aa50`) has no identified consumer; it writes into
  `DAT_006bf1c8 + (DAT_006bf1cc[0x1d4] + 0x91)*0x40`. It is a fourth, unexercised placement source.
  Next command: `py -3.12 re\tools\decomp_pc.py 0x006bf1c8 0x006bf1cc --datarefs`.
- `FUN_00534b60` / `FUN_00535700` / `FUN_00535910` match RenderWare `RpPTank`
  create/lock/unlock structurally (flag normalisation, and the cluster-selector -> stride map in
  `FUN_00476df0`). The binary is stripped; this is a structural correspondence, not an asserted
  symbol.
  Next command: `py -3.12 re\tools\console\xtwin.py 0x00534b60`.

---

## 6. Evidence index

| Path | What |
|---|---|
| `verify/pickups_20260929/bin/{mashed_re.exe,SHA256.txt,GIT_HEAD.txt}` | pinned standalone under test |
| `verify/pickups_20260929/orig_qb_full.png` | ORIGINAL capture (640x480, Quick Battle TRAINING); the box is at ~(290,62) |
| `verify/pickups_20260929/orig_qb_boxcrop.png` | crop of it: textured icon cube + blue glow |
| `verify/pickups_20260929/sa_arctic_t24_ctx.png` | STANDALONE Arctic, right half; the orb is the cyan blob top-right |
| `verify/pickups_20260929/sa_arctic_t24_orbcrop.png` | crop of it: featureless cyan blob, no icon |
| `verify/pickups_20260929/orig_qb.bmp.pickuprecs.json` | live 25-slot pool read, Quick Battle TRAINING, count 5 |
| `verify/pickups_20260929/orig_qb.log` | the run that produced it (track detect, campose, lens) |
| `verify/pickups_20260929/orig_recs.log` | negative control: Challenge route, count 0, all records zero |
| `verify/pickups_20260929/orig_pickup_positions.csv` | all 13 tracks, DFF boxes (pos+type+respawn) and LUA spawns |
| `verify/pickups_20260929/sa_storm_pu.csv.orbs.csv` | standalone Storm gate fallback, 7 orbs |
| `re/frida/pickup_pos_probe2.py` | the read-only pool probe (race_draw_burst + a Memory-read block) |

Probe offset note, so the JSON is re-readable: `pickup_pos_probe2.py` reads from `0x0068b1a0`,
which is **+8** from the true record base `0x0068b198` (`0x00458881`, `0x00458e75`). Probe offset
`+X` is true record offset `+(X+8)`. The `state`/`type`/`anchor` columns printed by the script are
therefore mislabelled by one field; the raw dump in the JSON is correct and §2.1's table is decoded
with the correction applied.
