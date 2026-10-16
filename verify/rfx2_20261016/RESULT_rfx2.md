# RESULT rfx2: the rest of rfx's C04 / C09 draws, the non-team HUD, the beam body frame

**Verdict.** On d1r1's 9 matched-pose frames (HUD replay, carlod's brake rows) the C04 / C08 / C09 rows
go from 24 to 7, 0 RED: RWObjShad GREEN 7/7, lensflare-fs8 GREEN 6/6, Headlight GREEN 5/6 (City 801 is
one MISSING / EXTRA pair from an instrument input, cited C09b, U-9711). Not ported: flatshad (3 rows,
the particle simulation) and VehicleIcons (2 rows, per-car inputs with no source). The non-team HUD
panels `FUN_0041b630` are GREEN 3/3 on a new Team-Play-off reference and ship default ON. G-CTRL
`716cb2fd / 2a2afda1 / ba90ed88` on the base and the final build. No-op control 9/9. Base `10c18c59`
(race/first-frame-parity with carlod, rstates, d2engage merged), branch `work/rfx2`.

## 1. What changed in the default build

| feature | default | opt-out | source |
|---|---|---|---|
| object shadows `RWObjShad` (`FUN_00481a30` / `FUN_00487280` / `FUN_00487d70`) | ON | `MASHED_OBJ_SHADOW=0` | `LibRw/RwRaceFx.cpp` DrawObjShadows |
| Headlight sprites (`FUN_00412f30` / `FUN_00412cf0` / `FUN_004128f0` / `FUN_00413a40`) | ON | `MASHED_HEADLIGHT=0` | DrawHeadlights |
| lensflare-fs8, both RpPTank batches (`FUN_00488e70` / `FUN_004883b0`) | ON | `MASHED_LENSFLARE=0` | LensBuild / DrawLensSmall / RaceFx_DrawLensBig |
| non-team HUD panels (`FUN_0041b630` -> `FUN_0041b340`, logic `FUN_0041adb0` / `FUN_0041ae20` / `FUN_0041af50`) | ON | `MASHED_HUD_NONTEAM=0` | RaceFx_RenderHud |
| beam / light body frame = X * pose(part 0x13) * Y * root outside the replay (U-9703) | ON | `MASHED_FX_BODYPOSE=0` | TrackRenderer fx_car |
| start-light clock step = `FUN_0040fc00`'s `(int)(dt*3000+0.5) * [0x5cc948]` (0x0040fc57) | ON | (adapter, U-9704) | TrackRenderer UpdateCar |

In the default dev route the non-team panels draw nothing yet: the standalone leaves `DAT_008a94d0`
(`FUN_0040e340`, the HUD's player count) at 0, so `FUN_0041adb0` activates no record. They draw under
the HUD replay and will draw once that global carries the participant count (follow-up).

## 2. What the original does (static decode, `evidence/decomp/`)

**RWObjShad.** `FUN_00481a30` (tick, from `FUN_0040ce00`) walks the `DAT_006c6eb0` RWP objects: within
50 units of the camera ([0x5cd098] 2500 on the squared distance) it refreshes the frustum state of every
other object per frame (`(i & 1) == (DAT_007f101c & 1)`, sphere radius x [0x5cc55c] 10, `FUN_004c1b40`),
recomputes the shadow when the object turns visible, and adds one matrix particle to `DAT_00703068` when
acos(shadow at . (0,1,0)) < [0x5cd07c] pi/4, alpha ftol(((1 - a [0x5cf234]) + 1) [0x5cd6d4] 64). The
shadow `FUN_00487280`: a ray from the object along twice the course light's at-vector (`DAT_0065743c` =
course + 0x105e4) into the collision world, material colours 0xff010101 / 0xffff0080 cast none
(`FUN_0045c110`), the orientation from the hit triangle (`FUN_0045c030`), the object's bbox
(`DAT_006c9a88`, from the RW Physics primitive: box `0x0055cbf0`, capsule `0x00563090`) through the inverse
light LTM for the extent, scale (span y / tan(acos(light at . right)), span x * 1.4), offset by the hit
point; re-runs use the cached triangle (`FUN_00547230`, +0.01 in y). Drawn first in `FUN_0040bde0` with
cull off and ZWRITE off.

**Headlight.** `FUN_00412f30` builds per car the brake lights 0x39 / 0x3a (brake byte), the headlights
0x19 / 0x1a / 0x1c / 0x1b and tail lights 0x37 / 0x38 (headlight flag `DAT_0063bc44`); `FUN_00412cf0` puts
each record at part + (part - camera) * fade, size |part - camera| * 0.025 + 0.05, alpha ftol(min(at .
camera at, 0)^2 * 255 * scale). `FUN_004128f0` projects through the camera's view matrix (camera + 0x20),
clips to the raster with the uv pulled in, z mapped between RwGlobals +0x18 / +0x1c; `FUN_00413a40` draws
one Im2D list (blend 5 / 2, ZWRITE off, cull off).

**lensflare-fs8.** `FUN_00487e70` adds lights to the 128-entry list at `0x0086ae20` (City's slot 0
`0x0044b100`: RWP objects by label + offsets 0x005f96e8.., Mts slots 2 / 3; Arctic's `0x00448940`: Mts
slots 1 / 2 / 3 and one light whose k `FUN_00448ef0` bobs). `FUN_00488e70` (small batch, one particle per
element) and `FUN_004883b0` (big batch, 16 mirrored particles per light) fill the two RpPTank atomics ONCE
(`DAT_007030b4`), camera-facing (`FUN_00537fb0`: right' = -right, up' = up; `FUN_00536e90`). The big batch
draws from Arctic's sky render `FUN_004492b0` and City's slot 2 `FUN_0044b8f0`, the small one from
`FUN_004891f0` after `FUN_0040bde0`.

## 3. Measurement

Original-side witness (read-only, `camera_probe.py --fx-out`, `evidence/witness_*.json`) checked each
port's arithmetic offline before its arm (`evidence/offline/`): the shadow matrices of the three visible
TRAINING objects to 3e-4 before the RW sqrt table (the port then uses `Math::LutNormalize`); 762 Headlight
quads on 44 City samples to 0.018 px, the view-matrix relation to 5.7e-6; both lensflare AABBs on City
761 to 1e-4 and on Arctic 1051 once the bob light holds one tick's k ((sin 0.2 + 1) * 0.15).

`scores_fin.txt` (rows (RED) [C04/C08/C09] matched), `--brake`, base `10c18c59`:

| frame | base, HUD replay | final, HUD replay | base, default | final, default |
|---|---|---|---|---|
| TRAINING 884 / 924 / 959 | 16 / 15 / 15 | 15 / 15 / 15 | 22 / 21 / 21 | 21 / 21 / 21 |
| Arctic 1051 / 1091 / 1126 | 29 / 29 / 30 | 26 / 26 / 27 | 35 / 35 / 36 | 32 / 32 / 33 |
| City 761 / 801 / 836 | 17 / 17 / 18 | 14 / 16 / 15 | 23 / 23 / 24 | 20 / 22 / 21 |

C04 / C08 / C09 rows: 24 -> 7 (HUD replay), 78 -> 61 (default). 0 RED on every frame of every arm.

- **R1** RWObjShad 7/7, lensflare-fs8 6/6, Headlight 5/6 GREEN; non-team panels 3/3 on the Team-Play-off
  reference (`run_nt.py`, C08 rows 5 / 5 / 5 -> 0 / 0 / 0, matched 210 -> 230).
- **R2** new row keys vs base: one, City 801 `EXTRA Headlight`, cited C09b: the original's cut frame holds
  26 sprites (two brake-light records) with no brake plate drawn on any car, the instrument's brake input
  (carlod's rows, read from the plates) has no brake there. Nothing else, on either arm.
- **R3** G-CTRL `716cb2fd / 2a2afda1 / ba90ed88` on the base (`gctrl_base4b`) and the final builds
  (`gctrl_fin2`, `gctrl_fin3`). Two first runs that overlapped the Team-Play-off TRAINING capture came back
  short (7815 / 7773 step lines, piz lock class U-9677) and were re-run.
- **R4** all opt-outs set (`MASHED_OBJ_SHADOW=0 MASHED_HEADLIGHT=0 MASHED_LENSFLARE=0 MASHED_HUD_NONTEAM=0
  MASHED_FX_BODYPOSE=0`): draw records equal the base's 9/9 (`noop_fin.txt`).
- **R5** pixels (`pixels_fin.txt`): every frame equal or lower (at most 0.23 lower), no rise.
- Body frame witness (`MASHED_DBG_FXBODY` under the replay): the chain against the trace's F{i}, 2845
  samples, median 0, max 0.054 (the standalone's own suspension pose, not the original's).

## 4. Not ported, with what blocks it

| item | rows | blocked by |
|---|---|---|
| flatshad | Arctic 3 | `FUN_0048e7b0` reads the per-type particle buffers `0x00722250..0x007228b0` that the particle simulation `FUN_0048cf90` writes (C05, not ported). |
| VehicleIcons | Arctic 1126, City 836 | `FUN_00413cb0` draws per car x 5 icon slots whose fade `DAT_0063bc60` moves by 2 dt and whose flag slot 0 is `FUN_00442c80`: `DAT_008989b0[car] > [0x5cc9b8]` in mode 6; neither the per-car float nor the fade history has a standalone or trace source. |
| Headlight City 801 | pair | the brake byte at that frame (U-9711). |
| start-light clock zero | (none on the frames) | the original zeroes `DAT_0063d588` at the state 3 -> 4 transition of `FUN_004102f0` (0x00410387), which exists only under `MASHED_SUBSTATE`; the step is the original's. |
| per-mode HUDs | (none on the frames) | `FUN_0041e850`, `FUN_0041ded0` / `FUN_0041c300` / `FUN_0041a3e0`, `FUN_00403160`: no capture of those modes. |
| beam / light flag | (none) | the slot-liveness stand-in `aib_alive` and `FUN_0041eeb0`'s 0x10 / 0x400 clears (U-9703). |

## 5. Found on the way

1. **lensflare is built once.** `FUN_00488e70` fills both batches only while `DAT_007030b4 == 0`, which
   `FUN_00487e70` / `FUN_00487e40` reset; per frame only the camera-facing expansion changes.
2. **The tick's frustum is the last render's.** `FUN_00481a30` runs after the director moved the camera,
   but against the frustum planes of the last `RwCameraBeginUpdate`: with this frame's planes Arctic 1051
   drew 5 shadows instead of 7.
3. **City 801's record holds two frames** that differ by the two brake-light sprites (48 / 52 triangles);
   the matcher's cut frame is the second.
4. librw's depth state: a Z test switched off while ZWRITE is on keeps ZENABLE (ZFUNC ALWAYS); the
   lensflare draw after Arctic's sky needs the test switched off with ZWRITE already off.
5. The recorder now flags librw Im2D draws as screen draws (`DrawRec::SetNextGeometry(.., true)`), which
   the 3D matcher groups by.
6. `#64x64:21` (7 triangles on the ground) is MISSING on the Team-Play-off reference frame 915 with and
   without this lane's code: a runtime texture the d1r1 cites do not name.

## 6. Reproduce

```
py -3.12 verify\rfx2_20261016\run_arm.py <exe> <arm> --brake [MASHED_HUD_REPLAY=<abs>\verify\rfx_20261016\hud_inputs.txt] [KNOB=V ...]
py -3.12 verify\rfx2_20261016\rowdelta_arms.py base4_hud fin3_hud
py -3.12 verify\rfx2_20261016\noop_cmp.py base4_hud fin3_noop
py -3.12 verify\rfx2_20261016\run_gctrl.py <exe> <name>
py -3.12 verify\rfx2_20261016\run_nt.py <exe> <arm> [MASHED_HUD_NONTEAM=0]
py -3.12 re\frida\camera_probe.py --frame --early --track <n> --fx-out <fx.json> --out <trace.csv>
py -3.12 re\frida\camera_probe.py --frame --early --shots-race 30,70,105 --drawrec --carframes --track 0 --teamplay 0 --hud-out <hud.json> --out <trace.csv>
```
