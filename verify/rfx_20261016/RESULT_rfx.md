# RESULT rfx: the original's in-race shadows, start display and FX on the librw path

**Verdict.** On d1r1's 9 matched-pose frames the cite groups C04 / C08 / C09 go from 96 rows to 24,
0 RED on every frame. C08 (start display, 57 rows) is GREEN on 9/9 frames with the HUD inputs
transplanted (`MASHED_HUD_REPLAY`, PREREG amendment 1). car_shadow is GREEN on TRAINING and City,
Beam GREEN on City; on Arctic both become a STATE row (CULL 2 vs 1), the unported weather pass's
device cull leak U-9690, cited. Not ported, with scope (section 6): RWObjShad, flatshad, Headlight,
lensflare-fs8, VehicleIcons (24 rows). G-CTRL unchanged `c2e4d60d / b50bd39b / ba90ed88`. No-op
control (all three knobs off) equals the base build's draw records on 9/9 frames. Pre-registration
`PREREG_rfx.md` (`8a7f974e`, amendment 1 `44d8c595`), base `b26d9f3c`. Sections 1-7 measure
`work/rfx` on `b26d9f3c`; section 8 repeats every rule on `work/rfx-rb`, rebased onto `0020d521`:
the same 96 -> 24, 0 RED, G-CTRL `716cb2fd / 2a2afda1 / ba90ed88` unchanged against that base.
Section 9 does it again on `work/rfx-rb2`, rebased onto `d074095d` (rprops, librwrest_checks,
dupinstall2 merged): the same result. On that branch the rfx tracker rows are U-9703 / U-9704 /
U-9705 (head holds rprops' U-9699..U-9702).

## 1. What changed in the default build

| feature | default | opt-out | source |
|---|---|---|---|
| car shadow (`car_shadow`, one RpPTank quad per car) | ON | `MASHED_CAR_SHADOW=0` | `LibRw/RwRaceFx.cpp` RaceFx_DrawCarFx |
| headlight beam (`Beam`) on courses 0 / 0x1a / 0x21 / 0x24 | ON | `MASHED_CAR_BEAM=0` | same |
| in-race HUD: team panels + start lights, parallel HUD camera | ON | `MASHED_HUD3D=0` | `LibRw/RwRaceFx.cpp` RaceFx_RenderHud |

The HUD reads the game state the original reads. In the default dev route (`MASHED_ROUND`) Team
Play `DAT_0067ea64` is 0, and the original then draws the non-team panels `FUN_0041b630`, which is
not ported (U-9704), so only the start lights show. With `MASHED_SUBSTATE` off the start-light
clock `DAT_0063d588` is driven by a labelled stand-in adapter tied to the stand-in countdown
(zeroed 113 frames before its GO, then `+= dt`; TrackRenderer countdown block, U-9704). Under
`MASHED_SUBSTATE` the ported `FUN_0040dbd0` / `FUN_0041d930` tail drive it.

## 2. What the original does (static decode, `evidence/decomp/`)

**RpPTank.** car_shadow, Beam, RWObjShad, flatshad, lensflare-fs8 and PUGlow are RenderWare
RpPTank atomics: `FUN_00534b60` = create (callbacks `0x00623c78`), `FUN_00535700` / `FUN_00535910` =
lock / unlock, `FUN_00535690` = render callback, `FUN_00536780` picks the vertex callbacks by flags.
Matrix particles expand in `FUN_00537890` (v = p +- r/2 +- u/2, 0.5 = `0x005cc32c`), colour
`FUN_00537b20`, constant UV `FUN_00537e60` with (0,0)-(1,1) (`DAT_00613294`), index list
(0,1,2)(0,2,3) (`FUN_00536170`), drawn by `FUN_00535cb0` with the batch blend pair.

**car_shadow** `FUN_0041fcc0`: the car clump frame's modelling matrix, Rotate((1,0,0), 90, pre),
Translate(T[f], pre), Scale(S[f], pre), f = descriptor id / 6 (tables `0x005f6088` / `0x005f5fe8`,
13 families, `0x005f37a8`); alpha ftol(max(up.y, 0) x rec+0x288 x 255), rec+0x288 = 1 - ground-ray
fraction (`FUN_0041f590`, `0x0041f648`). **Beam** `FUN_0041faf0`: parts 25 / 28 (`rec+0x64` /
`rec+0x70`), centre = B + B.at + (A - B)/2, scale (0.75, 2, 1), alpha 32 x max(cam.at . at, 0),
blend 5 / 2, gate rec+0x294 & 0x40 = `DAT_0063bc44` (`FUN_0040da50`). Static check: the predicted
AABBs equal the recorded ones to 1e-5 (car_shadow from the trace's `R{i}`, 9/9; Beam from `F{i}`
and the DFF part frames, 6/6).

**Start display.** `FUN_00492e90` renders `FUN_0040dfc0` with the HUD camera `DAT_006905b4`
(clone of the race camera, near 0.1 / far 180, `FUN_00467110`), set each frame by `FUN_0040de30`:
identity frame, view window (0.6, 0.45), rwPARALLEL, ZTEST / ZWRITE off. States 5 / 6: team panels
`FUN_0041c0c0` -> `FUN_0041bc50` (29 slots by geometry part code, `FUN_0041b690`; visibility
`FUN_0041b7a0`; members and flag word `FUN_0041bf20` / `FUN_0041b720`; positions (+-0.45, 0.33, 1)
`FUN_0041c010`; score bar = slot 2's frame scaled score / target, `FUN_0040b6d0` / `FUN_0040b890`)
when Team Play is on, then the start lights `FUN_0041db80` (6 atoms of `startlights.dff`,
thresholds 0.34 / 0.82 / 1.30 / 1.86 / 4.0, slide `y = 0.35 + (t - 2.4) * 0.25` after 2.4 s,
`FUN_0041d930`). **Correction to d1r1 C08:** "every draw lies outside the view frustum" was tested
against the race camera; these draws use the parallel HUD camera and are on screen.

## 3. Instrument additions (diagnostic only)

- `DrawRec::SetNextGeometry` (D3d9Render/DrawRec): librw immediate mode streams through a dynamic
  write-only buffer; the FX code hands the recorder the CPU copy and calls the d1r1 hooks with the
  texture name and identity world. Inert unless `MASHED_DBG_D3DREC`.
- Replay: `R{i}_*` (render-root modelling matrix) now read for the shadow root under
  `MASHED_CAR_REPLAY`; `MASHED_HUD_REPLAY=<file>` takes the HUD state from the trace's `ba8c` and the
  start-light clock rebuilt from `ba8c` / `dtb`, and Team Play / slot table / scores from
  `hud_inputs.txt`.
- `re/frida/camera_probe.py --hud-out` (read-only): samples the HUD inputs and both team-panel
  records 4x per second. One TRAINING Quick Battle run (`evidence/hudprobe_training_hud.json`):
  Team Play 1, `DAT_0067e9fc` 10, `DAT_007f0fd0` 0, `DAT_008a94d0` 4, players (team, colour)
  (0,0) (1,3) (1,1) (1,2), scores 4 / 4 / 4 / 4 from GO+10 to GO+175. Its own panel records
  (count 1 / 3, flags `0x800001` / `0x2004088`, visible slots 0-3+28 / 0-3+8+12+24) are what the
  ported code computes from those inputs.
- `cites_rfx.json` = d1r1's cites + one re-pairing rule: with the score bars matched, the original's
  full-screen no-op Im2D quad (C03 d) pairs by triangle count with an untextured 2-tri City world
  draw of librw (both draws cited before rfx).

## 4. Results (`scores.txt`, `log/rfx/<arm>/`)

| frame | base | final, HUD replay | final, default |
|---|---|---|---|
| TRAINING 884 / 924 / 959 | 80 / 79 / 79 | 73 / 72 / 72 | 79 / 78 / 78 |
| Arctic 1051 / 1091 / 1126 | 80 / 80 / 80 | 74 / 74 / 74 | 80 / 80 / 80 |
| City 761 / 801 / 836 | 156 / 154 / 152 | 148 / 146 / 144 | 154 / 152 / 150 |

All CITED, 0 RED, on all three arms. C04 / C08 / C09 rows per frame, base -> final with HUD replay:
TRAINING 8/7/7 -> 1/0/0, Arctic 12/12/13 -> 4/4/5, City 12/12/13 -> 3/3/4 (96 -> 24). The
default arm (no HUD inputs) keeps the 6 C08 rows per frame: the dev route runs with Team Play off
and the standalone's own clock is past 4 s at those rows, so the HUD draws nothing there, which is
what `FUN_0040dfc0` does with those inputs minus the unported non-team panels (U-9704).
Two arm runs lost SFX.piz textures to another session's original holding the piz (U-9677 class:
NFL badges unbound on one City run, no Arctic records on one default run); both were re-run.

Rules (PREREG section 4):
- **R1** C08 GREEN 9/9 (HUD replay). car_shadow GREEN TRAINING 3/3 + City 3/3, Beam GREEN City 3/3;
  Arctic car_shadow / Beam STATE CULL 2 vs 1 (U-9690), CITED (P1 / P2 as predicted).
- **R2** new row keys vs base: the 6 Arctic STATE rows above (cited C19 / U-9690); the City
  `MOVED -` row re-pairs (key present in base, cite added in cites_rfx). Nothing else.
- **R3** G-CTRL `c2e4d60d / b50bd39b / ba90ed88` on the base exe and on both final builds
  (18664 / 56153 / 28076 lines, private ptrace; `evidence/gctrl_*.txt`).
- **R4** no-op control (`MASHED_CAR_SHADOW=0 MASHED_CAR_BEAM=0 MASHED_HUD3D=0`) == base draw
  records on 9/9 frames, field by field (`noop_cmp.py`, `noop.txt`): PASS 9/9 (both final builds).
- **R5** pixels (`pixels.py`, `pixels.txt`, mean abs diff vs the original's shot, all channels):
  default build -0.10..+0.07 per frame (no rise above 0.5); with the HUD replay -2.3..-4.0
  (TRAINING 884 15.31 -> 11.34, Arctic 1051 10.89 -> 7.83, City 761 16.47 -> 12.99).

## 5. Found on the way

1. librw's `defaultRenderCB_Shader` skips the texture bind for an untextured material, so the draw
   inherits the previous raster's alpha flag (blend + alpha test on); RW's driver unbinds. The HUD
   unbinds before its untextured score bars; the same quirk affects every untextured librw draw.
2. librw turns depth test off as ZFUNC ALWAYS with ZENABLE kept on when depth write is still on
   (`d3ddevice.cpp` setDepthTest); the HUD turns ZWRITE off first so the device reads ZENABLE 0.
3. librw's render-state cache outlives the frame: a pass that sets alpha-test / cull state must put
   it back, or the next frame's world pass inherits it (seen in arm b1: every C19 tuple moved).
4. The d1r1 Quick Battle reference runs with Team Play on (`DAT_0067ea64` = 1).

## 6. Not ported (U-9705), with scope

| texture | rows (T/A/C) | original | needs on the standalone |
|---|---|---|---|
| RWObjShad | 1 / 3 / 3 | RpPTank `DAT_00703068` from `FUN_00487d00`, `FUN_00481a30` over `DAT_006c6eb0` dynamic objects (flag `DAT_006cad50`), drawn `FUN_00487d70` | dynamic-object placement (U-9695) |
| flatshad | 0 / 3 / 0 | `FUN_0048e7b0` over the per-type particle buffers `0x00722250..0x007228b0` (`FUN_0048cf90`) | the particle simulation (C05) |
| Headlight | 0 / 3 / 3 | Im2D: `FUN_00412f30` light list (parts 0x19..0x1c / 0x2a / 0x37..0x3a), `FUN_00412cf0` / `FUN_00412e30`, `FUN_004128f0` projection, `FUN_00413a40` | a screen projection matched to the original camera |
| lensflare-fs8 | 0 / 3 / 3 | RpPTank `DAT_007030a8` (0x200), `FUN_004881d0` / `FUN_00488e70` from 128 lights at `DAT_0086ae4c`, `FUN_004891f0`; `DAT_007030ac` via `SkySecondaryDispatch` | the track light list |
| VehicleIcons | 0 / 1 / 1 | `FUN_00413b80` / `FUN_00413cb0` / `FUN_00413f50`, batch `DAT_0063bd50` | the icon builder's inputs |

## 7. Reproduce

```
py -3.12 verify\rfx_20261016\run_arm.py <exe> <arm> --rfx-cites [MASHED_HUD_REPLAY=<abs>\verify\rfx_20261016\hud_inputs.txt] [KNOB=V ...]
py -3.12 verify\rfx_20261016\rowdelta_arms.py base <arm>
py -3.12 verify\rfx_20261016\noop_cmp.py base <noop arm>
py -3.12 verify\rfx_20261016\run_gctrl.py <exe> <name>
py -3.12 re\frida\camera_probe.py --frame --early --hud-out <hud.json> --out <trace.csv> --static-out <static.json>
py -3.12 verify\rfx_20261016\make_hud_inputs.py <hud.json> <trace.csv> verify\rfx_20261016\hud_inputs.txt
```

## 8. Rebased onto `0020d521` (`work/rfx-rb`)

Base arm `hb` = `0020d521` built unchanged; `rb2` = `work/rfx-rb` at `e9da6145`. The rebase
carries one addition: since librwrest the copters are librw instances, so a pass that runs after
every instance drew car_shadow after the copters (2 ORDER rows on TRAINING in the first rebased
arm). `RaceSubmit_AddCarFxMarker` queues the car FX point after the AI car loop, where
`FUN_00410b30` calls `FUN_0041ebb0` (`0x00410bb4`, before `FUN_00425e40` at `0x00410c08`). Outside
the replay the beam's body frame is the stand-in car matrix, not the librwrest body pose (U-9703).

| frame | hb | rb2, HUD replay | rb2, default |
|---|---|---|---|
| TRAINING 884 / 924 / 959 | 77 / 76 / 76 | 70 / 69 / 69 | 76 / 75 / 75 |
| Arctic 1051 / 1091 / 1126 | 76 / 76 / 76 | 70 / 70 / 70 | 76 / 76 / 76 |
| City 761 / 801 / 836 | 153 / 151 / 149 | 145 / 143 / 141 | 151 / 149 / 147 |

`scores_rb.txt`, `rowdelta_rb_hud.txt`, `rowdelta_rb_default.txt`, `noop_rb.txt`, `pixels_rb.txt`.
- **R1** as on `work/rfx`: C08 GREEN 9/9 with the HUD replay; car_shadow GREEN TRAINING + City,
  Beam GREEN City; Arctic car_shadow / Beam STATE CULL 1 (original) vs 2 (standalone), U-9690.
  C04 / C08 / C09 rows 96 -> 24 (HUD replay).
- **R2** new row keys vs `hb`: the same 6 Arctic STATE rows (both arms), nothing else.
- **R3** G-CTRL `716cb2fd / 2a2afda1 / ba90ed88` on `hb`, on the first rebased build and on `rb2`
  (`evidence/gctrl_hb.txt`, `gctrl_rb.txt`, `gctrl_rb2.txt`).
- **R4** no-op control == `hb` draw records 9/9 (`noop_rb.txt`).
- **R5** pixels: default -0.09..+0.07; HUD replay -2.2..-4.0 (TRAINING 884 14.33 -> 10.36).
- One Arctic HUD-replay run on the first rebased build came back without the car FX records
  (U-9677 class); the sequential re-run was exact.

## 9. Rebased onto `d074095d` (`work/rfx-rb2`)

Base arm `hb2` = `d074095d` built unchanged; `rb3` = `work/rfx-rb2` at `2519b23f` (the rebase
conflicted only in `re/analysis/CHANGELOG.md`; the car FX marker still sits after the AI car loop
and before the copters). The base rows changed with rprops (City 153 -> 128).

| frame | hb2 | rb3, HUD replay | rb3, default |
|---|---|---|---|
| TRAINING 884 / 924 / 959 | 78 / 77 / 77 | 71 / 70 / 70 | 77 / 76 / 76 |
| Arctic 1051 / 1091 / 1126 | 78 / 77 / 77 | 72 / 71 / 71 | 78 / 77 / 77 |
| City 761 / 801 / 836 | 128 / 126 / 124 | 120 / 118 / 116 | 126 / 124 / 122 |

`scores_rb3.txt`, `rowdelta_rb3_hud.txt`, `rowdelta_rb3_default.txt`, `noop_rb3.txt`, `pixels_rb3.txt`.
- **R1** C08 GREEN 9/9 with the HUD replay; car_shadow GREEN TRAINING + City, Beam GREEN City;
  Arctic car_shadow / Beam STATE CULL 1 (original) vs 2 (standalone), U-9690. C04 / C08 / C09
  rows 96 -> 24 (HUD replay), 96 -> 81 (default).
- **R2** new row keys vs `hb2`: the same 6 Arctic STATE rows (both arms), nothing else.
- **R3** G-CTRL `716cb2fd / 2a2afda1 / ba90ed88` on `hb2` and `rb3` (`evidence/gctrl_hb2.txt`,
  `gctrl_rb3.txt`).
- **R4** no-op control == `hb2` draw records 9/9 (`noop_rb3.txt`).
- **R5** pixels: default -0.09..+0.07; HUD replay -2.2..-4.0 (TRAINING 884 14.33 -> 10.36).
