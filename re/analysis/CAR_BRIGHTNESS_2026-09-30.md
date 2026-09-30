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

*(appended after the pre-registration commit)*
