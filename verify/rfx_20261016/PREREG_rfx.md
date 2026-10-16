# PREREG rfx (2026-10-16): port the original's in-race shadows, start display and FX

Lane rfx, branch `work/rfx` from `b26d9f3c` (race/first-frame-parity). Acceptance instrument: d1r1's
matched-pose per-draw recorder + `re/tools/drawlist_diff.py` 3D mode on its 9 frames
(`verify/d1r1_20261016/RESULT_d1r1.md`), cites `verify/d1r1_20261016/cites_d1r1.json`. This file is
committed before the first measuring run. Static decode is in `log/rfx/decomp*.txt` (copied to
`evidence/` with the result).

## 1. Groups and row counts (d1r1 evidence, 9 frames)

| group | rows T / A / C | textures |
|---|---|---|
| C08 STARTDISP | 18 / 18 / 21 | OrangeDisplay, GreenStartLight, NFLRed, NFLMelon, NFLBluejay, NFLGold (+ City MOVED `-`) |
| C09 FX | 0 / 10 / 10 | Beam, Headlight, lensflare-fs8, VehicleIcons |
| C04 SHADOW | 4 / 9 / 6 | car_shadow, RWObjShad, flatshad |

## 2. What the original draws (static decode, every value cited)

**car_shadow** (C04, 9 rows). Built per car by `FUN_0041fcc0` (called from `FUN_00420050`, EDI = the
per-car record `0x0063d9e0 + i*0x2ac`, `0x004200ce`): matrix = copy of the modelling matrix of the
frame of the car clump `*(rec+0x26c)` (`0x0041fd3c..0x0041fd58`), `RwMatrixRotate(m, (1,0,0), 90,
PRECONCAT)` (`0x0041fd5d..0x0041fd6e`), `RwMatrixTranslate(m, T[idx], PRECONCAT)` with T at
`0x005f6088` (`0x0041fd7f`), `RwMatrixScale(m, S[idx], PRECONCAT)` with S at `0x005f5fe8`
(`0x0041fd90`), idx = `*(*(rec+0x268)+0x80) / 6` (`0x0041fcf5..0x0041fd0c`); colour (0,0,0,a),
a = ftol(max(up.y*1 + (up.x+up.z)*[0x5d757c]=0.0, 0.0) * [rec+0x288] * 255.0 [0x5cd04c])
(`0x0041fd9f..0x0041fddd`). The batch `DAT_0063e4a0` (`FUN_0041eaa0`: `FUN_004770c0(b, 0x80c, 4,
"car_shadow")`, no blend override, so src/dst = `DAT_006132c4/c8`) is RenderWare's **RpPTank**:
`FUN_00534b60` = create, `FUN_00535700` = lock, `FUN_00535910` = unlock, `FUN_00535690` = the
atomic render callback. Matrix particles expand in `FUN_00537890`: v0 = p + r/2 - u/2, v1 = p - r/2
- u/2, v2 = p - r/2 + u/2, v3 = p + r/2 + u/2 (0.5 = `0x5cc32c`), indices (0,1,2)(0,2,3)
(`FUN_00536170`); colour per vertex `FUN_00537b20`; constant UV `FUN_00537e60`. Drawn by
`FUN_0041ebb0` (ZWRITE 0, flush+render `e4a0` then `e49c`), from `FUN_00410b30` after the four
`FUN_00420050` car renders (`0x00410bb9`). **Static check (done):** the AABB predicted from the
trace's `R{i}_*` (render-root modelling matrix) with idx 1 equals the recorded `car_shadow` AABB to
1e-5 on 9/9 frames (with `F{i}_*` it misses by 0.003-0.014).

**Beam** (C09, 6 rows). `FUN_0041faf0`, gated by `*(byte*)(rec+0x294) & 0x40` (`0x0041fb08`): A =
LTM of part 25's frame (`rec+0x64`), B = LTM of part 28's frame (`rec+0x70`) (slot = part code,
`FUN_00420420` `*(rec + part*4) = atomic`); `RwMatrixRotate(B, (1,0,0), 90, PRECONCAT)`; B.pos +=
1.0*B.up' + 0.5*(A.pos - B.pos) (`0x0041fbc3..0x0041fc31`); `RwMatrixScale(B, (0.75, 2.0, 1.0),
PRECONCAT)`; colour (255,255,255,a), a = ftol(max(dot(cam.at, B.at''), 0) * 32.0 [0x5cd27c])
(`0x0041fc44..0x0041fc87`). Batch `DAT_0063e49c` "beam", blend 5/2 (`FUN_00476cb0(b,5,2)`).

**Start display** (C08, 57 rows). Drawn by `FUN_0040dfc0` (states 5/6, default branch) under the
**HUD camera** `DAT_006905b4` with ZTEST/ZWRITE off (`FUN_00492e90`), set up by `FUN_0040de30`:
identity frame, view window (0.6, 0.45), projection 2 = rwPARALLEL. Team panels (`FUN_0041c0c0` ->
`FUN_0041bc50` x2, `team0panel`/`team1panel` from Panel.piz, positions (+-0.45, 0.33, 1.0)
`FUN_0041c010`, visibility `FUN_0041b7a0`, membership `FUN_0041bf20`/`FUN_0041b720`) when
`DAT_0067ea64 != 0`; start lights (`FUN_0041db80`, `startlights.dff`, y slide `FUN_0041d930`, timer
`DAT_0063d588` thresholds 0.34/0.82/1.30/1.86/4.0). **Correction to d1r1:** C08's "every draw lies
outside the view frustum" used the race camera; these draws are on screen (the original shot BMPs
show the panels and lights at the top).

**Filed, not ported unless time remains** (precise scope goes to the result): RWObjShad
(`FUN_00487d00` from `FUN_00481a30`, batch `DAT_00703068`, drawn by `FUN_00487d70`), flatshad
(`FUN_0048e7b0` per-type particle buffers `0x00722250..0x007228b0`), Headlight (Im2D,
`FUN_00412f30` -> `FUN_004128f0` -> `FUN_00413a40`), lensflare-fs8 (`FUN_004881d0`, `FUN_00488e70`,
`FUN_004891f0`, `SkySecondaryDispatch`), VehicleIcons (`FUN_00413b80` / `FUN_00413f50`).

## 3. Instrument additions (diagnostic only, default OFF, no effect on a plain run)

I-A. The new FX draws go through librw immediate mode, which does not set D3DTS_WORLD; the FX code
calls the d1r1 hooks `rw_drawrec_pre(texname, identity)` / `rw_drawrec_post()` around each draw so
the recorder logs the right texture and world (vertices are world-space, like the original's PTank
draws whose D3DTS_WORLD is identity).
I-B. Under `MASHED_CAR_REPLAY=1` the shadow root uses the trace's `R{i}_*` (the original's render
root), and the beam parts are `local(part) x F{i}` (frames 51/54 of ADVANTAGE0.DFF are pure
translations under frame 22, `verify/rfx_20261016/dffparts.py`).
I-C. HUD inputs under replay (part B) are fixed by an amendment committed before part B's first
measuring run.

## 4. Pass rules (per ported texture)

R1 GREEN: the texture has no row (MISSING / EXTRA / MOVED / STATE / ORDER / TEX1) on any of the 9
frames where the original draws it. A STATE row whose only difference is one already cited and
unported (U-9690 Arctic device cull, d1r1 section 4) is reported as CITED, not GREEN.
R2 No new rows: per frame, every row of the port's diff whose (kind, texture) key is absent from the
base diff is listed; the lane passes only with none, or each one cited with a U-id and its cause.
R3 G-CTRL (default build, `verify/d1r1_20261016/run_gctrl.py` recipe): `c2e4d60d / b50bd39b /
ba90ed88` before and after; ptrace hash only when its frame numbers increase strictly.
R4 No-op control: with the lane's opt-out knobs set, the standalone draw records equal the base
build's on 9/9 frames (draw events compared field by field).
R5 Pixels: `re/tools/imgdiff.py` against the original's shot BMP reported per frame; any rise > 0.5
is investigated before a default-ON decision.

Default: a texture that passes R1-R4 ships default ON with an opt-out knob; otherwise default OFF.

## 5. Predictions

P1 car_shadow GREEN on TRAINING and City (6 frames). Arctic: STATE CULL 2 vs 1 (the U-9690 leak;
the port sets CULLBACK as `FUN_00410b30` does at entry, `(*RwGlobals+0x20)(0x14,2)`).
P2 Beam GREEN on City (3), Arctic STATE CULL as P1.
P3 With the alpha test states the original's device uses (GREATER / 0, `FUN_004d5bc0`) set for these
draws, no AREF/AF STATE rows on the new draws.
P4 G-CTRL unchanged (render-only code).
