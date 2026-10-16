# PREREG rfx2 (2026-10-16): the rest of rfx's C04 / C09 draws, the HUD residue, the beam flag

Lane rfx2, branch `work/rfx2` from `04dee0ed` (race/first-frame-parity, rfx-rb2 merged). Acceptance
instrument: d1r1's matched-pose per-draw recorder + `re/tools/drawlist_diff.py` 3D mode on its 9
frames, adjudicated with rfx's cites (`verify/rfx_20261016/cites_rfx.json`, copied unchanged to
`cites_rfx2.json`), with rfx's HUD replay (`MASHED_HUD_REPLAY=verify/rfx_20261016/hud_inputs.txt`)
on the HUD arm. Scripts: copies of rfx's (`run_arm.py`, `scores.py`, `rowdelta_arms.py`,
`noop_cmp.py`, `pixels.py`, `run_gctrl.py`) writing to `log/rfx2/<arm>`. This file is committed
before the first measuring run. Static decode lives in `log/rfx2/*decomp*.txt` (copied to
`evidence/` with the result).

## 1. Scope and row counts (rfx RESULT section 6, HUD-replay arm `rb3_hud`, 9 frames)

| item | rows T / A / C | tracker | order |
|---|---|---|---|
| RWObjShad (C04) | 1 / 3 / 3 | U-9705 | 1 |
| Headlight (C09) | 0 / 3 / 3 | U-9705 | 2 |
| lensflare-fs8 (C09, two draws per frame) | 0 / 3 / 3 | U-9705 | 3 |
| flatshad (C04) | 0 / 3 / 0 | U-9705 | 4 |
| VehicleIcons (C09) | 0 / 1 / 1 | U-9705 | 5 |
| non-team panels `FUN_0041b630`, per-mode HUDs, start-light clock stand-in | no rows on the 9 frames (Team Play on in the reference) | U-9704 | 6 |
| beam headlight flag and body frame stand-ins | none (Beam GREEN on City) | U-9703 | 7 |

Work goes in that order. An item is ported only from a static decode with every constant cited;
an item whose standalone inputs do not exist is filed with the exact missing input, not
approximated.

## 2. Arms

- `base`: `04dee0ed` built unchanged (frozen copy `log/rfx2/base_exe/`), default and HUD replay.
- `<item>`: the branch with the item ported, default and HUD replay.
- `noop`: the final build with every new knob set to its opt-out value.
- G-CTRL: `run_gctrl.py` on the frozen base and on the final build.

## 3. Pass rules (rfx's R1-R5, unchanged)

R1 GREEN: the texture has no row (MISSING / EXTRA / MOVED / STATE / ORDER / TEX1) on any of the
frames where the original draws it. A STATE row whose only difference is one already cited and
unported (U-9690 Arctic device cull, d1r1 section 4) is reported as CITED, not GREEN.
R2 No new rows: per frame, every row of the port's diff whose (kind, texture) key is absent from
the base diff is listed; the lane passes only with none, or each one cited with a U-id and cause.
R3 G-CTRL on the default build: the base build's three hashes before and after (rfx measured
`716cb2fd / 2a2afda1 / ba90ed88` on `d074095d`; the hashes of this base are measured, not assumed).
R4 No-op control: with the lane's opt-out knobs set, the standalone draw records equal the base
build's on 9/9 frames (draw events compared field by field).
R5 Pixels: `re/tools/imgdiff.py` against the original's shot BMP per frame; any rise > 0.5 is
investigated before a default-ON decision.

Default: an item that passes R1-R4 ships default ON with an opt-out knob; otherwise default OFF
with a recommendation.

## 4. What is known before measuring (rfx decode, `log/rfx2/decomp1.txt`)

- RWObjShad: `FUN_00481a30` (from `FUN_0040ce00`) walks `DAT_006c6eb0` objects; per object a
  shadow flag `DAT_006cad50[i] = FUN_00487280(...)` and, below an angle test, `FUN_00487d00` adds
  one matrix particle to the RpPTank batch `DAT_00703068` (cap `DAT_00703070`); `FUN_00487d70`
  draws it with cull mode 1 and ZWRITE off. Every other object of the loop is refreshed only on
  alternate frames (`(i & 1) == (DAT_007f101c & 1)`).
- Headlight: `FUN_00412f30` builds per-car light records from car part frames (0x19..0x1c, 0x37,
  0x38, 0x39, 0x3a, 0x2a), `FUN_00413a00` / `FUN_004128f0` project them into screen quads,
  `FUN_00413a40` draws them as one Im2D indexed list. The 3D-mode matcher pairs screen draws by
  texture, triangle count and AABB within 0.01 + 1e-4 x extent (about 0.05 px here), so GREEN
  needs the original's projection and light set exactly.
- lensflare-fs8: two RpPTank draws per frame, `DAT_007030ac` (0x2000 particles, drawn with Z test
  off before the world) and `DAT_007030a8` (0x200, `FUN_00488e70` from the 128-entry light list at
  `0x0086ae24`, drawn by `FUN_004891f0`).
- flatshad: `FUN_0048e7b0` over the per-type particle buffers `0x00722250..0x007228b0`; needs the
  particle simulation (C05).
- VehicleIcons: `FUN_00413c70` from `FUN_00412f30`, batch `DAT_0063bd50`.

## 5. Predictions (refined per item by amendment before that item's first measuring run)

P1 Every ported item: its rows go GREEN on the replay arm on the frames where its inputs match the
original's (car poses and camera are transplanted; world objects and lights come from the
standalone's own track load).
P2 G-CTRL unchanged (render-only code; the clock adapter of U-9704 writes only HUD globals).
P3 No new row keys outside the ported textures.

## Amendment 1 (2026-10-16, written by the resuming session before the final measurement)

1. **Base moved.** The lane was rebased twice: onto `34ea5b71` (carlod) and `f0f2d886` (rstates), then
   onto `10c18c59` (d2engage, default unchanged) for the final measurement. Base arms are re-measured on
   each frozen base exe (`log/rfx2/base3_exe` = `f0f2d886`, `log/rfx2/base4_exe` = `10c18c59`); the
   `04dee0ed` base arms of the first session are history.
2. **Cite file.** rstates updated the shared adjudication (d1r1 cites + rprops notes + rfx's C03 rule +
   rstates' C15c); `cites_rfx2.json` is now `verify/rstates_20261016/cites_rstates.json` copied unchanged
   plus one entry, `C09b-HEADLIGHT-BRAKE` (City 801: the original's cut frame has two brake-light sprites
   with no brake plate drawn on any car; the instrument's brake input below has no brake there).
3. **Instrument input.** Every arm (base and port) runs with `--brake`: carlod's matched-pose
   `MASHED_CAR_BRAKE_ROWS` from `verify/carlod_20261016/brake_rows.json` (the controller's brake byte
   `0x007f103c + ctrl*0x4c + 1`, which the brake-light sprites of `FUN_00412f30` also read).
4. **Original-side witness (read-only).** `re/frida/camera_probe.py --fx-out` samples the object-shadow
   arrays (`0x006c6eb0` list, bbox `0x006c9a88`, shadow matrix `0x006c3670`, triangle cache `0x006c74f8`),
   the course light LTM (`DAT_0065743c`), the race camera (`DAT_006905b0` view matrix, window, clip
   planes), the Headlight records / screen quads and the lensflare light list (`0x0086ae20`) on one race
   per track (`log/rfx2/fxw_training`, `fxw2_city`, `fxw2_arctic`). They are used to check each port's
   arithmetic offline before its arm, never as the arm's input.
5. **Recorder.** `DrawRec::SetNextGeometry` takes a `screen` flag so a librw Im2D draw is recorded with
   the `"screen"` key the original's XYZRHW draws carry (the 3D matcher groups by it).
6. **Items as measured before this amendment** (HUD-replay arm, `--brake`, on `f0f2d886`): RWObjShad
   7/7 GREEN, Headlight 5/6 (City 801 = item 2), lensflare-fs8 6/6. flatshad and VehicleIcons are not
   ported (their inputs: the particle simulation, and the per-car `DAT_008989b0` with the icon fade
   history `DAT_0063bc60`, have no standalone or trace source).
