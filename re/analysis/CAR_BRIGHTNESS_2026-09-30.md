# Car renders at ~0.5x brightness in the standalone — defect (d), 2026-09-30

Investigation child. Scope: **defect (d) only** — the car body renders at about half the
original's brightness. The first defect in `re/analysis/CAR_GRAY_CHASSIS_2026-09-29.md` (the
grey chassis from the 27 untextured hull/locator atomics) is separately diagnosed and is NOT
this note's subject. Defect (d) is the same thing that note recorded as **M5** and left open
as **O2**.

**Status of this file at first commit: PRE-REGISTRATION ONLY.** Everything below the
`# PRE-REGISTRATION` heading was written and committed *before* any capture in this session.
Measurements are appended afterwards under `# MEASURED`.

---

## Run anchors

| item | value |
|---|---|
| git HEAD at pre-registration | `cc5d376b` (branch `race/first-frame-parity`) |
| standalone binary (PINNED, the only exe this session runs) | `verify/car_bright_20260930/bin/mashed_re.exe` |
| its SHA-256 | `65A76A2E31590ACFFAC85A910AA15235DF2C6FB96314D9F5BBB71E1A75AEA3F4` (**verified before first run**) |
| its size | 1 960 448 B, copied from `mashedmod/build/` 2026-09-30 14:22 |
| original | `original/MASHED.exe` (ten boot patches applied; `.unpatched` carries the version anchor) |

This session **never** runs `mashedmod\build.bat`, never edits anything under `mashedmod/src`,
and never touches `mashedmod\build\`. A parallel D2 child holds the build slot. Every game
launch is muted (`MASHED_MUTE=1`), carries `MASHED_TITLE`, uses `MASHED_WIN_POS=left-bl`, never
uses `MASHED_NAV_DEMO`, and only PIDs spawned by this session are killed.

---

# PRE-REGISTRATION

## P0 — What is already measured (inherited, not re-derived)

From `CAR_GRAY_CHASSIS_2026-09-29.md` M5, carried in as the starting point:

* ORIGINAL in-race body pixels (`verify/car_gray_20260929/orig_bb_a.png`, 640x480 **R5G6B5**
  backbuffer): dominant **(232,52,56)**. The `Advantage` TXD paint swatch decodes to
  **(236,52,60)**. Within R5G6B5 quantization that is **texel x 1.00**.
* STANDALONE with the grey shell neutralised at the asset level: dominant **(118,26,30)** =
  **texel x 0.50**.
* `MASHED_DBG_CARLIGHT=1` on Training reports
  `amb=(0.500,0.500,0.500) sun=(1.000,1.000,1.000) L=(-0.734,0.030,-0.678)`,
  `body_batches: tot=67 lit=64 hasN=64 relit=64 prelit=3`.

This session treats P0 as a prior to be **re-confirmed on the pinned build**, not as given.

## P1 — What I will compare

Four channels, in this order. A channel is only used for what it can actually see.

1. **Pixels, matched frames (primary quantitative channel).**
   * Original side: `original/MASHED.exe` warped into a race by the
     `re/frida/scenario_launch.py` / `capture_relight_parity.py` plumbing, backbuffer dumped
     through the d3d9 shim's on-demand `MASHED_ORIG_BBDUMP_REQ` channel. `PrintWindow` on the
     original returns an all-white frame on this machine, so the backbuffer dump is the only
     original-side pixel channel.
   * Standalone side: the pinned exe under a copy of `re/tools/sa_capture.py` with the `EXE`
     constant overridden to the pinned path (the script hardcodes `mashedmod/build/mashed_re.exe`
     at `re/tools/sa_capture.py:13`, which this session must not run).
   * Statistic: for a hand-delimited **car-body pixel set** in each frame, the per-channel
     ratio `standalone / original` of the **dominant body colour** and of the **mean over the
     body pixel set**, with `n` pixels reported for each side.
   * Comparison is **ratio of body pixels to the source texel**, per side, not a raw
     side-to-side pixel diff. The two sides are not pose-locked, so absolute pixel diffs
     (`re/tools/imgdiff.py`) are used only as a secondary sanity channel, never as the verdict.

2. **Per-batch material colour / geometry flags, ORIGINAL side, read live (primary causal
   channel).** One-shot memory read, **no `Interceptor` on any hot path**: at race phase, walk
   the per-player atomic table at `DAT_0063d9e0 + player*0x2AC` (established in
   `CAR_GRAY_CHASSIS_2026-09-29.md` M7 from `FUN_00420420` @ `0x00420420`). For every non-null
   slot record: slot index (= part code), `RpAtomic*`, its `RpGeometry*`, and from the geometry
   `flags` (+0x08), `numTriangles` (+0x10), `numVertices` (+0x14), `numMorphTargets` (+0x18),
   `numTexCoordSets` (+0x1C), the material list at +0x20 with `material[i]->color` (+0x04, RwRGBA)
   and `material[i]->texture` (+0x00) name.
   The RW struct offsets used here are asserted, not assumed — see P4 gate G0.

3. **Light colours.** Original side: enumerate the `RpWorld`'s lights at race time and record
   each light's type and RGBA. Standalone side: the `MASHED_DBG_CARLIGHT=1` line plus the
   librw ambient light constructed at `RwRaceSubmit.cpp:555`.

4. **Texture modulate state and D3D render states.** Original side: read back
   `GetTextureStageState(0, D3DTSS_COLOROP/COLORARG1/COLORARG2)` and
   `GetRenderState(D3DRS_LIGHTING/D3DRS_AMBIENT/D3DRS_COLORVERTEX/D3DRS_DIFFUSEMATERIALSOURCE)`
   at the moment a car batch is drawn, via the d3d9 shim (which already wraps the device), or
   by a single entry hook on the draw. Standalone side: the corresponding calls in
   `mashedmod/src/mashed_re/D3d9Render/` and the librw submit path.
   This channel exists specifically to discriminate `D3DTOP_MODULATE` from `D3DTOP_MODULATE2X`.

5. **Prelit colour.** For each car batch on both sides: whether `rpGEOMETRYPRELIT` (0x08) is
   set, and if so the prelit RGBA of a sample vertex.

## P2 — Candidate terms (the hypothesis set, fixed before measuring)

Exactly one of these must carry the ~0.5. They are mutually exclusive as *the dominant term*;
a mixture is an explicit outcome (see the decision rule).

| id | term | prediction if TRUE |
|---|---|---|
| **T1** | The original does **not light the car body at all** — `rpGEOMETRYLIGHT` (0x20) is **cleared** on the body geometries at load, and `rpGEOMETRYMODULATEMATERIALCOLOR` (0x40) is set with material colour `0xffffffff`, so the body renders at `texel x white = texel x 1.0`. Our port keeps the asset's `LIGHT` flag and lights the body with `amb 0.5 + sun*max(0,N.L)`, and Training's `L.y = 0.030` makes the directional term ~0 on up-facing panels, landing at 0.5. | Channel 2 on the ORIGINAL shows the big textured body geometries with flags **lacking 0x20** at runtime, while the same geometries in the shipped DFF **have 0x20** (`0x200b3`, recorded in `CAR_GRAY_CHASSIS_2026-09-29.md` R5). |
| **T2** | Texture stage op differs: the original uses `D3DTOP_MODULATE2X` (or an equivalent 2x) on the car, we use `D3DTOP_MODULATE`. | Channel 4 on the original shows `COLOROP == D3DTOP_MODULATE2X` for the car draw. |
| **T3** | Ambient magnitude/convention: the original's effective car ambient is 1.0 where ours is 0.5 — e.g. `ParseLightsDffFaithful` drops a x2, or the original's `D3DRS_AMBIENT` is white. | Channel 3 shows the original's ambient light at 1.0, or channel 4 shows `D3DRS_AMBIENT == 0xffffffff`, while the parsed LIGHTS.DFF value is 0.5. |
| **T4** | A missing lighting pass: the original adds a second directional/headlight/specular pass on the car that we omit. | Channel 3 shows more than one non-ambient light reaching the car, and the sum reproduces 1.0. |
| **T5** | Vertex prelit handling: the original's body carries a prelit of white that we drop or halve. | Channel 5 shows `rpGEOMETRYPRELIT` set on the original's body geometry with prelit `0xffffffff`, and absent/darker on ours. |
| **T6** | None of the above; the halving is elsewhere (an explicit `* 0.5` in our colour path, an 8-bit `>>1`, a 127-vs-255 normalization). | Channels 2-5 all agree between the sides, and a grep-located explicit halving in `mashedmod/src/mashed_re/` on the car colour path accounts for it. |

**T1 is the pre-registered leading hypothesis.** Its static basis is already in the tree:
`CAR_GRAY_CHASSIS_2026-09-29.md` M7 records that `FUN_00420420` @ `0x00420420` calls
`FUN_004b52c0(geom, 0x20, 0)` (clear `rpGEOMETRYLIGHT`) and `FUN_004b52c0(geom, 0x40, 1)`
(set `rpGEOMETRYMODULATEMATERIALCOLOR`) and writes `*(*(geom+0x20)+4) = 0xffffffff` (material 0
opaque white) for part codes `0x11`, `0x15`, `0x18`. Naming T1 as the root cause requires
showing those part codes **are** the drawn body — static citation alone is not enough.

## P3 — Tolerance

* **Brightness ratio.** The R5G6B5 backbuffer quantizes the original to 5/6/5 bits, i.e. worst
  case +/-4 on R and B and +/-2 on G out of 255. Against a texel of (236,52,60) that is up to
  **+/-1.7% on R**, more on the small G/B values. Run-to-run jitter on the standalone was
  measured at mean abs 0.0106/255 in `CAR_GRAY_CHASSIS_2026-09-29.md` M3, i.e. negligible.
  **Tolerance on the measured ratio: +/-0.05 absolute** (so "0.5x" means the measured ratio
  falls in `[0.45, 0.55]` and "1.0x" means `[0.95, 1.05]`). A ratio outside both bands is
  reported as its own number and blocks the T1 verdict.
* **Flags / material colours / render states** are **exact**. These are integers; there is no
  tolerance. A single differing bit is a difference.
* **n reporting is mandatory.** Every ratio is reported with the number of body pixels on each
  side and the number of batches it was derived over. A ratio without an `n` is not a
  measurement.

## P4 — Gates that must pass before any verdict is written

* **G0 — struct-offset self-check.** Before trusting channel 2, the RW offsets must be
  validated against something already known. The walk must reproduce, for at least one car
  geometry, a `numTriangles` and a material count that match the independent Python DFF parse
  in `CAR_GRAY_CHASSIS_2026-09-29.md` M2 (`ADVANTAGE0.DFF`: 71 geometries, 71 materials; the
  four hulls at 56/44/60/40 triangles; 27 materials with colour `(102,102,102,255)` and no
  texture; 44 with a texture and `(255,255,255,255)`). If the walk cannot reproduce those
  numbers, the offsets are wrong and channel 2 is discarded, not "interpreted".
  *Rationale: a hand-rolled struct walk that misreads a header lands in the wrong place and
  fakes a confident claim (memory `pe-rva-mapping-field-order`).*
* **G1 — the output channel must be proven live.** A zero/empty result from any probe is only
  usable if a **positive control** on the same run shows the probe fired at all. Specifically,
  an empty per-player table is only reported as "the original draws nothing there" if the same
  read returns non-null slots for at least one other index.
  *Rationale: memory `missing-output-channel-fakes-a-red` and `absent-log-proves-nothing-run-a-control`.*
* **G2 — the standalone's 0.5 must be re-confirmed on the pinned build**, not inherited from
  the 2026-09-29 build. If the pinned build already renders the body at 1.0x, defect (d) is
  fixed and this note records that instead.
* **G3 — no Interceptor on a hot path.** Frida `Interceptor.attach` above ~1000 calls/s
  destabilises Mashed in ~6 s. Channel 2 is a one-shot read. Channels 3-4 use at most one
  entry hook on a per-frame-or-rarer function, for a bounded number of samples.

## P5 — Decision rule (the term is named by this, not by judgement after the fact)

Evaluated in order; the first rule that fires decides.

1. If **G2** shows the pinned standalone body ratio is in `[0.95, 1.05]`, the defect is not
   reproducible on this build. Report that, name no root cause, stop.
2. Else if channel 2 shows, on the ORIGINAL at runtime, that **every** geometry carrying the
   car's painted body texture has `flags & 0x20 == 0` while the shipped DFF has `0x20` set for
   those same geometries, **and** channel 4 shows the original's car draw uses plain
   `D3DTOP_MODULATE` (not 2X), **and** channel 3 shows no extra light that would supply the
   missing 0.5 — then the root-cause term is **T1**, "the original clears `rpGEOMETRYLIGHT` on
   the drawn body parts and the port does not".
3. Else if channel 4 shows the original's car draw uses `D3DTOP_MODULATE2X` while ours uses
   `D3DTOP_MODULATE` — the term is **T2**.
4. Else if channel 3 shows the original's effective car ambient is within `[0.95, 1.05]` while
   ours is within `[0.45, 0.55]` and flags agree — the term is **T3**.
5. Else if channel 3 shows a second light reaching the car that the port lacks, and removing it
   from the arithmetic reproduces our 0.5 — the term is **T4**.
6. Else if channel 5 shows a prelit difference that accounts for the factor — the term is **T5**.
7. Else — **T6**, and the specific halving must be cited as `file:line` with the surrounding
   code, or the verdict is `[UNCERTAIN]` with the residual stated as a number.

**Mixture clause.** If two terms each account for part of the factor, the note reports both
with their individual measured contributions and does **not** collapse them into one headline
"root cause". Naming a single term requires that term to account for the full factor within
the P3 tolerance.

**Non-reproduction clause.** If the original's runtime flags **match** the shipped DFF (0x20
still set), T1 is **refuted** regardless of how well the static decomp in M7 reads, and the
note says so. The static citation does not outrank the runtime measurement.

## P6 — Coupling constraint (pre-registered, binding on the fix target)

`re/analysis/race_terrain_ambient_20260830.md` establishes that the terrain over-brightness is
a **separate** defect at a **different** site: the manual prelit fold in `BuildClump`
(`RwSceneBuild.cpp:479`), which adds `amb_world_` into **non-lit prelit** batches. The car body
is **lit** (`LIGHT`-flagged, has normals) and by that note's own measurement "never enters the
fold in either build".

Therefore the fix this note proposes **must not** be a global light/ambient change. The
pre-registered constraint on any fix target I name:

* it must be reachable **only** from the car/vehicle load or submit path, and
* the note must state explicitly which non-car surfaces share the site, and
* if the site is shared with terrain, sea, or props, the proposal must be scoped (car-only
  predicate) and must say so, or be rejected.

A proposal that lowers a global ambient is out of bounds by pre-registration, even if it makes
the car ratio correct.

## P7 — What this session will NOT do

* Not implement the fix. The deliverable is a root cause plus a named fix target plus an
  acceptance test.
* Not build, not edit `mashedmod/src`, not touch `mashedmod/build`.
* Not mutate any tracker unless the measurement genuinely changes a row's evidence, and then
  only via the `re-classify` skill.

---

# MEASURED

## Headline

**The term is the track's DIRECTIONAL light DIRECTION, not any brightness scale.**
`ParseLightsDffFaithful` composes the light frame's at-vector up the frame chain starting at
the light's **own** frame instead of its **parent**, applying that frame's rotation one time
too many. The result is a wrong unit vector on **13 of 13 shipped tracks**. On TRAINING it
turns a sun at elevation `L.y = +0.352` into a sun at `L.y = +0.030` — a sun sitting on the
horizon — so the car's up-facing panels (roof, bonnet, boot: most of what a chase camera sees)
receive essentially no directional term and fall back to the ambient floor of 0.5.

Fix site: **`mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:853`** (and its twin at
`:751`). One token each.

**The pre-registered hypothesis set was incomplete.** T1–T6 are each refuted below by
measurement. The actual term is a seventh, named **T7** here, and is proven by four
independent lines of evidence (M3–M6).

---

## M1 — G2 PASSES: the pinned build still renders the body at exactly texel x 0.5000

Texel: the `Advantage` TXD paint swatch is **(236,52,60)**
(`verify/car_gray_20260929/crop_txd_swatches.png`, re-measured this session: 7 224 of 7 400
selected pixels at that exact value, spread 0).

Pinned build `65A76A2E…`, TRAINING (`MASHED_TRACK_SEL=12`), muted, `MASHED_WIN_POS=left-bl`,
top-down race view, car box `(236,64,280,102)` of `verify/car_bright_20260930/sadrive_t48.png`:

| statistic | value | ratio to texel |
|---|---|---|
| dominant body colour | **(118,26,30)** (n=48, 35.6% of body) | **(0.5000, 0.5000, 0.5000)** |
| R p50 | 123 | 0.5212 |
| n body pixels | **135** | — |

The 2026-09-29 build agrees: `verify/car_gray_20260929/nodummy_t16.png`, whole frame,
dominant **(118,26,30)** at n=10 575 of 29 234 body pixels, ratio **0.5000** exactly, p50
0.5297. Defect (d) reproduces. G2 passes; the decision rule does not short-circuit at step 1.

Note the halving is *exact* in all three channels (118/236, 26/52, 30/60), which is what
`ambient = 0.5` with `N·L = 0` produces, not a stray `*0.5f`.

## M2 — The ORIGINAL is NOT uniformly bright: it lights the body, with the SAME ambient floor

Original reference used: `verify/car_gray_20260929/orig_bb_a.png` (640x480 R5G6B5 backbuffer
dump, TRAINING, in-race, four cars, player car bottom-centre). Zoom of the player car:
`verify/car_bright_20260930/orig_playercar_zoom.png`.

Two boxes on that one car, same frame, same pose:

| surface | box | n body px | dominant | ratio to texel | R p50 ratio |
|---|---|---|---|---|---|
| **up-facing** (bonnet + roof) | `(288,330,330,356)` | **790** | (232,52,56) | **0.983 / 1.000 / 0.933** | **0.915** |
| **away-facing** (rear + lower flank) | `(270,360,360,400)` | **1106** | (112,24,24) | **0.475 / 0.462 / 0.400** | 0.542 |

The away-facing dominant **(112,24,24)** is `texel x 0.5` after R5G6B5 quantisation
(118→14→112, 26→6→24, 30→3→24). So:

* the original's **ambient floor is 0.5**, identical to ours — and so is its saturation
  ceiling at 1.0;
* the original's **up-facing panels sit at 0.92–0.98**, i.e. they receive a large directional
  term;
* our up-facing panels sit at **0.50–0.52**, i.e. they receive **none**.

This single table refutes four pre-registered terms at once (see M7).

## M3 — Evidence line 1: the asset says the direction is something else entirely

`re/tools/piz_extract.py` + an **independent** RW chunk parser written this session
(`verify/car_bright_20260930/light_dir_check.py`, which reads the DFF directly and does not
call any port code). TRAINING's `LIGHTS.DFF` (`original/TOASTART/TRACKS/training.piz`, 528 B,
`CLUMP{numAtomics=0, numLights=2, numCameras=0}`):

| light | type | colour | frame | frame parent | frame at-vector |
|---|---|---|---|---|---|
| `@0x00017c` | 1 DIRECTIONAL | (1.000, 1.000, 1.000) | 2 | 1 | **(-0.387516, -0.352184, -0.851937)** |
| `@0x0001c8` | 2 AMBIENT | (0.500, 0.500, 0.500) | 3 | 1 | (0, 0, -1) |

Frames 0 and 1 are identity, so the light frame's **world** at-vector equals its local
at-vector: `(-0.387516, -0.352184, -0.851937)`.

The port's runtime output for the same track (`mashed_re.log`, this session, pinned build):

```
WS-E lights: ambient=0xFF808080 (RGB 128,128,128) sun=0xFFFFFFFF dir=(0.734,-0.030,0.678)
CARLIGHT rp_on=1 car_relight=1 has_sun=1 amb=(0.500,0.500,0.500) sun=(1.000,1.000,1.000)
         L=(-0.734,0.030,-0.678) body_batches: tot=67 lit=64 hasN=64 relit=64 prelit=3
```

Ambient (0.5,0.5,0.5) and sun (1,1,1) match the asset exactly — TRAINING is the only one of
the 13 tracks with that pair, so the right `LIGHTS.DFF` is being read. **The direction does
not match and is not a negation of it.**

## M4 — Evidence line 2: an offline model of the defective loop reproduces the binary exactly, on two tracks

The composition loop is `TrackRenderer.cpp:848-866`:

```cpp
850                        float v[3] = {frames[pending_frame].rot[6],
851                                      frames[pending_frame].rot[7],
852                                      frames[pending_frame].rot[8]};
853                        for (int fi = pending_frame; fi >= 0; ) {
854                            const float* m = frames[fi].rot;
855                            const float x = m[0]*v[0] + m[3]*v[1] + m[6]*v[2];
856                            const float y = m[1]*v[0] + m[4]*v[1] + m[7]*v[2];
857                            const float z = m[2]*v[0] + m[5]*v[1] + m[8]*v[2];
858                            const std::int32_t pa = frames[fi].parent;
859                            if (pa == fi) break;        // guard
860                            if (pa < 0) { v[0]=x; v[1]=y; v[2]=z; break; }
861                            v[0]=x; v[1]=y; v[2]=z; fi = pa;
862                        }
```

`rot[6..8]` is the frame's own at-axis **already expressed in its parent's space** (RW
row-vector convention: `at_world = (0,0,1) · M_self · M_parent · … = row2(M_self) · M_parent · …`).
The seed is therefore already one composition in. Starting the loop at `fi = pending_frame`
(`:853`) multiplies by `M_self` a second time and yields `row2(M_self) · M_self · M_parent · …`.

Computing that by hand for TRAINING frame 2:

```
v   = at        = (-0.387516, -0.352184, -0.851937)
m   = frame2.rot = [-0.910257, 0, 0.414044,  -0.145820, 0.935931, -0.320578,
                    -0.387516, -0.352184, -0.851937]
x = m0*v0 + m3*v1 + m6*v2 =  0.352747 + 0.051354 + 0.330131 = +0.734232
y = m1*v0 + m4*v1 + m7*v2 =  0.000000 - 0.329620 + 0.300038 = -0.029582
z = m2*v0 + m5*v1 + m8*v2 = -0.160448 + 0.112902 + 0.725797 = +0.678251
```

`(+0.734232, -0.029582, +0.678251)` — the binary logs `dir=(0.734,-0.030,0.678)`. Every
printed digit.

**Pre-registered prediction test on a second track.** Before running it, the same offline
model predicted Arctic's as-built value as `(-0.977284, -0.138071, +0.160787)`. Run on the
pinned build, `MASHED_TRACK_SEL=0`:

```
WS-E lights: ambient=0xFF334D4D (RGB 51,77,77) sun=0xFF99B3B3 dir=(-0.977,-0.138,0.161)
CARLIGHT ... amb=(0.200,0.300,0.300) sun=(0.600,0.700,0.700) L=(0.977,0.138,-0.161)
```

Predicted and observed agree to all printed digits. The model of the defect is validated on
two independent tracks.

## M5 — Evidence line 3: the function violates its own documented contract

`TrackRenderer.cpp:688-689`, in the header comment of `ParseLightsDffDirectional`:

```
// Returns false if there is no such light. Arctic -> colour (153,178,178),
// dir (0.577,-0.577,-0.577).
```

Arctic's asset at-vector is `(+0.577350, -0.577350, -0.577350)` — the comment records the
**parent-start** (correct) value. The code as built returns `(-0.977, -0.138, +0.161)`. The
documented expected value and the implementation disagree, and the asset is on the comment's
side.

## M6 — Evidence line 4: the original's measured pixels are impossible under the as-built direction

`L` is the "toward the light" vector, `sun_L_ = -normalize(sun_dir_)`
(`TrackRenderer.cpp:1218-1226`). A perfectly up-facing panel (`N = (0,1,0)`) receives
`amb + sun * max(0, L.y)`:

| | TRAINING `L.y` | up-facing value | measured on the ORIGINAL |
|---|---|---|---|
| correct (parent-start) | **+0.3522** | 0.5 + 1.0x0.3522 = **0.852** | — |
| as-built (`:853`) | **+0.0296** | 0.5 + 1.0x0.0296 = **0.530** | — |
| **original, up-facing box, n=790** | — | — | **dominant 0.983, p50 0.915, mean 0.909** |
| **standalone, pinned, n=135** | — | — | **dominant 0.500, p50 0.521** |

The original's up-facing panels measure 0.91–0.98. The as-built direction caps them at 0.530.
**The as-built direction is refuted against the running original**, with a margin far outside
the +/-0.05 tolerance of P3. The correct direction predicts 0.852 as the floor for a
*horizontal* panel and more for panels tilted toward `L` (which also has +X and +Z
components) — consistent with the measured 0.92–0.98. The standalone's 0.500/0.521 matches
the as-built prediction of 0.530 within tolerance.

**Aggregate lit-surface split** (`verify/car_bright_20260930/surface_split.py`; floor =
ratio <= 0.58, sunlit = ratio >= 0.85):

| capture | n body px | mean ratio | at ambient floor | sunlit |
|---|---|---|---|---|
| ORIGINAL, TRAINING player car | **2325** | 0.735 | **29.2%** | **42.5%** |
| STANDALONE pinned, TRAINING car | **135** | 0.623 | **60.0%** | 17.0% |
| STANDALONE 2026-09-29 build, TRAINING full frame | **25704** | 0.632 | **64.6%** | 19.9% |

Two standalone captures from different builds and different frames agree (60.0% / 64.6%)
against the original's 29.2%. **Caveat, stated rather than hidden:** the three boxes are not
the same framing and the fractions are framing-sensitive, so this table is corroboration, not
the primary measurement. The primary measurement is the per-surface one above, which is
pose-robust because it compares an up-facing panel on each side against the same texel.

## M7 — All thirteen tracks are wrong

`verify/car_bright_20260930/light_dir_check.py` → `light_dir_check.txt`. Every row is computed
from the shipped asset; "as-built" replays `:853`, "correct" starts at the parent.

| track | correct `L.y` | as-built `L.y` | up-facing: correct → as-built | ratio |
|---|---|---|---|---|
| Arctic | +0.5774 | +0.1381 | 0.5464 → 0.2828 | **0.518** |
| City | +0.9755 | +0.2130 | 1.0000 → 0.4775 | **0.478** |
| dump | +0.2850 | +0.1475 | 0.5990 → 0.4615 | 0.771 |
| Egypt | +0.4186 | +0.5301 | 0.5930 → 0.6711 | 1.132 |
| Forest | +0.5043 | +0.2737 | 0.7530 → 0.5916 | 0.786 |
| Highway | +0.9231 | +0.6391 | 1.0000 → 0.8391 | 0.839 |
| Neustein | +0.7240 | +0.1403 | 0.9292 → 0.4622 | **0.498** |
| rouabout | +0.7196 | +0.8819 | 0.8976 → 1.0000 | 1.114 |
| sands | +0.5810 | +0.9263 | 0.7729 → 1.0000 | 1.294 |
| Storm | +0.4264 | +0.6584 | 0.3691 → 0.4665 | 1.264 |
| SuperG | +0.6319 | +0.0820 | 0.9319 → 0.3820 | **0.410** |
| **training** | **+0.3522** | **+0.0296** | **0.8522 → 0.5296** | **0.621** |
| Warzone | +0.5608 | +0.3881 | 0.9608 → 0.7881 | 0.820 |

**13 / 13 diverge.** Five tracks land at or below 0.52 on up-facing panels — that is the "car
renders at about 0.5x brightness" report. Four tracks come out *brighter* than correct, so the
defect is not a darkening: it is a randomly re-aimed sun. (Both branches happen to keep every
sun above the horizon, so the bug does not announce itself as an obviously impossible value.)

## M8 — The defect reaches BOTH renderers from this one site

librw is the default renderer (`RwRaceSubmit.cpp:172-175`) and the active car path. It takes
the same variable:

```
RwRaceSubmit.cpp:567  // A directional light points along its frame's at-vector, which is
RwRaceSubmit.cpp:568  // exactly what sun_dir_ holds (the direction the light travels).
RwRaceSubmit.cpp:571  m->at.x = st.sun_dir_[0]; m->at.y = st.sun_dir_[1]; m->at.z = st.sun_dir_[2];
```

The legacy D3D9 path takes `sun_L_ = -normalize(sun_dir_)` (`TrackRenderer.cpp:1218-1226`),
read at `:1412` (props/track), `:2048` (copters), `:2298` (player car), `:2623` (AI cars),
`:4663-4665` (per-frame relight, rotated into model space). Both renderers are fed by the same
upstream value, so **one fix corrects both** — which is why the defect was visible on the
librw default and on `MASHED_RENDER_LIBRW=0` alike
(`CAR_GRAY_CHASSIS_2026-09-29.md` R2 saw the same halving on both paths).

## M9 — Pre-registered terms T1–T6, each refuted by measurement

| term | verdict | the measurement that kills it |
|---|---|---|
| **T1** original clears `rpGEOMETRYLIGHT` on the body, so it draws at `texel x white` | **REFUTED** | M2: the original's body is **not** uniform. 29.2% of its paint sits at the 0.5 ambient floor and 42.5% is sunlit. An unlit `MODULATEMATERIALCOLOR`-white body would be 1.0 everywhere. (The static basis for T1 is real — `FUN_00420420` @ `0x00420420` does clear `0x20` and force material white via `FUN_004b52c0` @ `0x004b52c0` — but only for part codes `0x11`, `0x15`, `0x18` and `0x3b..0x3e`, i.e. a handful of parts, not the 44 textured body atomics.) |
| **T2** original uses `D3DTOP_MODULATE2X` | **REFUTED** | M2: both sides share the same ambient floor at 0.5 and the same ceiling at 1.0. A 2x stage op would put the original's floor at 1.0. |
| **T3** original's car ambient is 1.0, ours 0.5 | **REFUTED** | M2: the original's away-facing dominant is (112,24,24) = `texel x 0.5` after R5G6B5 — the identical ambient. |
| **T4** a missing second light supplies the difference | **REFUTED** | M3: TRAINING's `LIGHTS.DFF` contains exactly two lights (`numLights=2`), one ambient and one directional, and `amb + sun·N·L` already reaches the measured 1.0 ceiling without a third. |
| **T5** prelit handling | **REFUTED** | The body geometry carries no `rpGEOMETRYPRELIT` (flags `0x200b3`, bit `0x08` absent — `CAR_GRAY_CHASSIS_2026-09-29.md` R5), and both sides' floors coincide at ambient-only. A white prelit would lift both to 1.0. |
| **T6** an explicit halving in our colour path | **REFUTED** | Whole-tree sweep of `mashedmod/src/mashed_re/` and `deps/librw/src/d3d` for `* 0.5`, `0.5f *`, `/ 2.0f`, `>> 1`, `127`, `128.0f`: **no halving on any colour path**. The only colour-adjacent `0.5f` are round-to-nearest quantisers (`TrackRenderer.cpp:304`, `:362`, `:1213`). The two renderers' car-colour formulas are algebraically identical: `texel x clamp(prelit + amb·1 + sun·max(0,N·L)·1) x matCol`, with `surfProps = {1,1,1}`. |

**T7 (the actual term): the directional light's world at-vector is composed one frame too
many**, so the sun is aimed wrongly on every track. Not a scale error anywhere.

---

# ROOT CAUSE

`ParseLightsDffFaithful` (`mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:790`) builds
the track's directional-light world direction by walking the RW frame chain, but starts the
walk at the **light's own frame** instead of at that frame's **parent**:

**`mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:853`**

```cpp
for (int fi = pending_frame; fi >= 0; ) {          // <-- starts at the light's own frame
```

The seed `v = frames[pending_frame].rot[6..8]` is the frame's at-axis already expressed in its
parent's coordinates, so the loop applies `M_self` a second time. On TRAINING this turns
`(-0.387516, -0.352184, -0.851937)` into `(+0.734232, -0.029582, +0.678251)`; the "toward the
light" vector `L = -normalize(dir)` drops from `L.y = +0.352` to `L.y = +0.030`, and the car's
up-facing panels lose their entire directional term. 13/13 tracks are affected.

There is no RVA for this. The defect is in port-only glue: the original does not compute this
at all — real RenderWare obtains the direction from `RwFrameGetLTM` on the `RpLight`'s frame,
which is composed correctly by construction. The relevant original-side anchors are the
vehicle-init flag writes that this investigation *ruled out* as the cause:
`FUN_00420420` @ **`0x00420420`** (per-player vehicle init) and `FUN_004b52c0` @
**`0x004b52c0`** (`*(geom+0x08) |= / &= ~mask`, i.e. the `RpGeometry.flags` setter), decompiled
this session from a read-only pool slot.

---

# READY-TO-APPLY FIX (do NOT apply here — this session is capture-only)

**File**: `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`

**Change 1 — `ParseLightsDffFaithful`, line 853:**

```cpp
-                        for (int fi = pending_frame; fi >= 0; ) {
+                        // rot[6..8] is the frame's own at-axis ALREADY expressed in its
+                        // parent's space (RW row-vector: at_world = (0,0,1)·M_self·M_parent…
+                        // = row2(M_self)·M_parent…), so the parent-chain walk must START AT
+                        // THE PARENT. Starting at pending_frame applied M_self twice and
+                        // mis-aimed the sun on 13/13 tracks — TRAINING L.y +0.352 -> +0.030.
+                        // re/analysis/CAR_BRIGHTNESS_2026-09-30.md
+                        for (int fi = frames[pending_frame].parent; fi >= 0; ) {
```

**Change 2 — `ParseLightsDffDirectional`, line 751:** the identical loop, the identical
one-token change. Both must land; `ParseLightsDffFaithful` is the one on the default path, but
leaving the twin wrong would keep a second, silently-disagreeing copy of the same computation
(exactly the hazard in memory `duplicate-rva-implementations-drift`).

Nothing else changes. `sun_L_`'s negate-and-normalise (`:1218-1226`) is correct, the ambient
parse is correct, the colour arithmetic on both renderers is correct, and there is no scale
factor to touch.

## Coupling check (P6) — why this does NOT disturb the terrain

Pre-registration P6 forbids fixing car brightness with a global light change. This fix changes
**only the direction of the directional term**, and:

* **TRAINING terrain (ROAD.DFF, geom flags `0x2008b`) has no `rpGEOMETRYNORMALS` (0x10) and no
  `rpGEOMETRYLIGHT` (0x20)**, so it receives no directional term at all. Already measured:
  `re/analysis/race_terrain_ambient_20260830.md` reports `MASHED_TERRAIN_NOLIGHT=3` ("sun
  zeroed") gives frame mean **18.50 vs base 18.47, band aggregate identical (147,123,79)**.
  Deleting the sun entirely does nothing to the terrain, so re-aiming it cannot either.
* **Arctic sea and the other water DFFs (`0x1000f`)** likewise carry no `LIGHT` and no normals
  — unaffected, so the unresolved sea question in `race_terrain_ambient_20260830.md` is not
  reopened by this change.
* **The world BSP** (`0x4001004d`, no normals, no LIGHT per `TrackRenderer.cpp:639-641`) —
  unaffected.
* **The terrain ambient fill / `BuildClump` prelit fold** (`RwSceneBuild.cpp:585-620`, the
  actual terrain-overbright site) is gated on `!b.lit` and is untouched here.

**What DOES change besides the car**, and must be looked at: geometry that genuinely carries
`NORMALS|LIGHT` — **lit props** (e.g. SIGN02/CRATE01, `0x10037`) and the **copters**
(`TrackRenderer.cpp:2048`). Those changes are the point of the fix, not collateral, but they
are visible and are in the acceptance test below.

---

# PRE-REGISTERED ACCEPTANCE TEST FOR THE FIX CHILD

Written before the fix exists. Pass requires **all four**.

**A1 — exact direction, both tracks (cheapest, run first).** Rebuild, then with
`MASHED_DBG_CARLIGHT=1`, `MASHED_MUTE=1`, `MASHED_TITLE=…`, `MASHED_WIN_POS=left-bl`:

| track | `mashed_re.log` must log `dir=` | `CARLIGHT` must log `L=` |
|---|---|---|
| `MASHED_TRACK_SEL=12` (TRAINING) | `(-0.388,-0.352,-0.852)` | `(0.388,0.352,0.852)` |
| `MASHED_TRACK_SEL=0` (Arctic) | `(0.577,-0.577,-0.577)` | `(-0.577,0.577,0.577)` |

Exact to the 3 printed decimals. The Arctic row is independently the value the source's own
comment at `TrackRenderer.cpp:688-689` already claims. `verify/car_bright_20260930/light_dir_check.py`
prints the expected pair for all 13 tracks (`CORRECT` rows) if another track is wanted.

**A2 — the car's up-facing paint recovers.** Re-capture TRAINING in a top-down race view with
the pinned-capture recipe
(`py -3.12 verify/car_bright_20260930/sa_capture_pinned.py <prefix> <times> MASHED_TRACK_SEL=12 MASHED_MUTE=1 MASHED_TITLE=… SA_EXE=<new exe>`),
then `py -3.12 verify/car_bright_20260930/body_ratio.py <png> <car box> <label>`:

* the body's **dominant** ratio must leave the `[0.45, 0.55]` band and land **>= 0.80**
  (correct prediction 0.852 for a horizontal panel; the original measures 0.915 p50 / 0.983
  dominant on n=790);
* report **n body pixels** with it — a ratio without an `n` does not count.

**A3 — the lit-surface split moves toward the original.**
`py -3.12 verify/car_bright_20260930/surface_split.py <png> <car box> <label>`: the
**ambient-floor fraction must fall from ~60–65% to 25–40%** (original: 29.2% on n=2325).
Tolerance is wide on purpose — this statistic is framing-sensitive (M6 caveat). A2 is the
binding one; A3 is the direction-of-travel check.

**A4 — terrain and sea regression guard (the P6 constraint).** Same binary, same pose, TRAINING
and Arctic:

* TRAINING terrain band aggregate must be unchanged vs the pre-fix build within run-to-run
  jitter (mean abs <= 0.02/255, the figure measured in `CAR_GRAY_CHASSIS_2026-09-29.md` M3).
  `py -3.12 re/tools/imgdiff.py <pre>.bmp <post>.bmp --grid 8x6`, reading the terrain cells.
* Arctic sea region must be unchanged by the same test.
* If either moves, stop: something other than the directional term changed, and the fix is not
  what this note describes.

A failing A4 with a passing A1 is the interesting case and must be reported, not worked around.

---

# OPEN / [UNCERTAIN]

* **O1 [UNCERTAIN] — no direct runtime read of the ORIGINAL's `RpLight` direction.** The
  original-side evidence here is (a) the shipped asset and (b) the original's measured
  per-surface pixel brightness, which is impossible under the as-built direction (M6). I did
  **not** enumerate the original's `RpWorld` lights at race time and read the light frame's
  LTM at-vector — that needs an `RpWorldForAllLights`/`RwFrameGetLTM` RVA this session did not
  establish. What is missing: a direct confirmation that the original's runtime direction
  equals the asset at-vector. Nothing measured contradicts it, and RW composes the LTM
  correctly by construction, but it is inferred rather than read.
  *Next command*: `py -3.12 re/tools/decomp_pc.py 0x00479330 --callees --strings` to find the
  `RpWorldAddLight` call after the `LIGHTS.DFF` `RpClumpStreamRead` at `FUN_0042a5d0`, then a
  one-shot Frida read of the light frame's LTM `+0x50`, matrix at-row `+0x20`.
* **O2 — the four tracks that get BRIGHTER** (Egypt 1.13x, rouabout 1.11x, Storm 1.26x, sands
  1.29x, M7) were not visually checked. The fix will *darken* their up-facing car panels. That
  is the correct direction per the asset, but no original-side reference exists for those
  tracks, so it is unverified. Only TRAINING has a measured original reference in this note.
* **O3 — the lit props and copters** (`TrackRenderer.cpp:1412`, `:2048`) change with this fix
  and were not measured on either side. A4 does not cover them.
* **O4 — the residual after the fix is not predicted to be zero.** The original's up-facing
  dominant is 0.983 where a horizontal panel under the correct direction gives 0.852; the gap
  is panel tilt, and it is not separately verified. If A2 lands at ~0.85 and the original is at
  ~0.92–0.98 on a *matched* pose, that gap is a second, smaller question — not a failure of
  this fix.
* **O5 — the grey-chassis fix state.** The pinned build already renders the car in its livery
  paint (the frames in this note show a red car, not grey blocks), so the
  `CAR_GRAY_CHASSIS_2026-09-29.md` fix appears to be in this build. Not verified against
  `hooks.csv` or the diff; irrelevant to defect (d) either way, since M1 measures the paint
  directly.

---

# Evidence index (`verify/car_bright_20260930/`)

| file | what |
|---|---|
| `bin/mashed_re.exe` | the pinned standalone, SHA-256 `65A76A2E…` (not committed) |
| `sa_capture_pinned.py` | `re/tools/sa_capture.py` with the exe pinned; every standalone capture here used it |
| `light_dir_check.py` / `.txt` | independent per-track CORRECT-vs-AS-BUILT direction computation (M3, M7) |
| `body_ratio.py` | body-paint brightness as a ratio to the (236,52,60) texel (M1, M2) |
| `surface_split.py` | ambient-floor / sunlit fraction of the car's paint (M6) |
| `orig_train.bmp` + `.draw3d.json` | ORIGINAL TRAINING reference frame, `race_draw_burst.py --settle 4.0 --mode-sel 1` |
| `orig_cambasis.txt` | the 12-float camera basis for that frame — **use this**, not `orig_campose.txt` (the 6-float form is discredited) |
| `orig_playercar_zoom.png` | 4x zoom of the original's player car — the up-facing/away-facing split of M2 |
| `sadrive_t48.png` | STANDALONE pinned, TRAINING, top-down car view — the M1 measurement |
| `sapose_t30.png` | STANDALONE at the transplanted original basis (pose transplant verified working) |
| `sa_arctic_t30.png` | STANDALONE Arctic — the M4 prediction test run |

Original-side reproduction:

```powershell
py -3.12 re\frida\race_draw_burst.py --out verify\car_bright_20260930\orig_train.bmp `
    --settle 4.0 --mode-sel 1
```

Standalone reproduction (TRAINING, pinned exe, muted, left-bl, no NAV_DEMO):

```powershell
py -3.12 verify\car_bright_20260930\sa_capture_pinned.py verify\car_bright_20260930\sadrive `
    26,32,40,48,56 MASHED_TRACK_SEL=12 MASHED_DEMO_DRIVE=1 MASHED_DBG_CARLIGHT=1 `
    MASHED_MUTE=1 MASHED_TITLE="car-brightness SA drive (d)" `
    "KEYS=4:enter,5.5:enter,7:enter,8.5:enter,10:enter,11.5:enter,13:enter,16:enter,17.5:enter,19:enter"
```

