# RESULT wcsflip: MASHED_WCS_TRINORMAL default ON - NOT FLIPPED (13 tracks RED) (2026-10-20)

Branch `work/wcsflip` from `0d9f1e0c2`. PREREG `PREREG_wcsflip.md` (`4daee232f`, with the seed change), committed before any arm.
Exe `24a608c8` (`MASHED_NO_DEPLOY=1`). Evidence in `D:\MashedWT\wcsflip_data` (`MOVED_OFF_GIT.txt`). The seed change is reverted on
this branch (no default change); the branch carries this record only.

## Verdict: no flip (the 13-track gate fails; PREREG rule "If D2 or 13 tracks fail, do not flip")

| gate | measured | verdict |
|---|---|---|
| G-CTRL x2 | flip build: `H_DEF` = `H_DEF-2` = `91cc1c0b / 2d77a5d7 / ab6c9329` | reported |
| G-OPT | `MASHED_NO_WCS_TRINORMAL=1`: `H_NT` `df5ee50e / a90bbb16 / b3cd0e5f`; `B_NT` `c165d2e5 / c01e164a / 2e937af7` | PASS |
| G-B | flip build `B_DEF` `28a76cf2 / 76a0b4cd / 040ce640` (= u9773 `B_TN`); (b) A 1/3 (car 3), B 2/3 (cars 1, 3), C 2/3 (cars 1, 3). Changed vs the f5flip default: A car 2 PASS -> FAIL (`c0_distinct 36 [13,35]`), C car 3 FAIL -> PASS; A car 1 still FAIL (now `c1_distinct 16 [21,70]`) | as expected |
| G-D2 | 3 runs alone: 0.1900 x3 / 0.2550 x3 / 1963.95, 1963.95, 1965.35, H1 398/400; motion_diag logs byte-identical to the f5flip default's (the knob is inert on D2) | PASS 3/3 |
| G-13 | 39 frames, 869 rows, 866 cited, **3 RED**: Roundabout 1091 (34 rows, 1 RED) and 1131 (26, 1 RED), Training 959 (21, 1 RED); every RED row is `MOVED Advantage` (same texture and triangle count, AABB moved by up to 0.0212 / 0.0257); also cited count changes Arctic 1091 36 -> 35, Highway 853 18 -> 17, Training 924 21 -> 20 | **FAIL** |

The RED rows are car-body draws (`Advantage`) whose pose moved by about 0.02: with the knob the AI bodies tilt onto the terrain (the
point of the fix), and the replay's draw comparison has no cite for that class. Resolving it needs either the original-side pose at
those rows (does the original's body tilt the same way there) or a cite class for the moved car body; both are open (U-9776).
