# D1 session evidence — the committed subset (2026-08-16)

The session produced ~460 MB of BMP capture sets across ~20 `verify/d1_*` directories. Those
are **not committed**; they are regenerable from the recipes in each `RESULT.md`. This
directory holds the small subset that the load-bearing claims actually rest on, so no note
cites an artifact that cannot be opened — the failure mode filed as D0 item 8 in
`re/analysis/SESSION_VERIFICATION_AUDIT_2026-08-15.md`.

| file | backs |
|---|---|
| `fx_before_orange.png` / `fx_after_cut.png` | the saturated-orange in-race frame, and the same frame with the FX particle class cut. `verify/d1_fxcut/RESULT.md` |
| `mirror_prefix_banner_REVERSED.png` | **the whole mirror finding in one image**: sponsor text reading backwards ("supersonic", "EMPIRE") in a pre-fix capture, while the HUD reads correctly. `verify/d1_mirrorfix/AUDIT.md` |
| `mirror_postfix_banner_correct.png` | same banners, same track, post-fix, reading correctly |
| `orig_training.png` | the original MASHED.exe on TRAINING, the reference for every same-view claim |
| `sa_basis_mirrored_before.png` | standalone at the original's exact pose+lens, still mirrored (89.68%) |
| `sa_basis_matched.png` | same pose, right axis corrected (33.79%) — landmarks aligned natively |
| `d1_lens_orig_lens.json` | the measured lens: `viewWindow (0.6, 0.45)`, near 0.1, far 360, projType 1, plus both passing cross-checks |
| `d1_basis_orig_cambasis.txt` | the 12-float camera basis read off the RwCamera frame |
| `d1_carproj_orig_carproj.txt` | car world positions projected through the transplanted pose — the camera-space test |
| `d1_frame_orig_frame.json` | RwCamera frame modelling/LTM matrices and the controller Euler angles |

Regenerating the full sets: original side is
`py -3.12 re/frida/race_draw_burst.py --out <dir>/orig.bmp --settle 4.0`; standalone side is
the recipe at the top of `verify/d1_fxcut/RESULT.md`. Both are deterministic — the session
repeatedly reproduced prior captures at 0.00%.
