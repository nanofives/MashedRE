# RESULT PORT-COLOURSEL-LEFTRIGHT (2026-11-09)

PREREG: `PREREG_COLOURSEL_LEFTRIGHT.md` (committed alone, c05058944). Branch `orch/PORT-COLOURSEL-LEFTRIGHT` from
`orch/PORT-COLOURSEL-BAR-Y-FLIP` 5775e1ad1 (BAR_Y default ON). Code f97b2bda2, drivers after it.

## 1. Answer

**GREEN.** On the cup-flow colour select, the original's left/right writes the choice **0x0067f0c0**, clamped to 1..6
(0x00433443..0x004334da: dec at 0x00433487, inc at 0x004334bc, per live entry of the 0x007f1a14 list, edge
0x007f1044/0x007f1045 + p*0x4c with the processed byte clear). It does not touch 0x0067ea98.

With `MASHED_COLOURSEL_ICON=1` the exe now moves 0x0067f0c0 the same way. A key run goes 1 → 6 and stops at 6, then
the left goes to 5, and the icon follows.

The new `MASHED_COLOURSEL_BORDER=1` draws the single-row bar as the original's five rects. walk_04's border is then at
rows 207..208 / 231..232 and columns 46..47 / 582..583, which is the original exactly. In the draw-list diff the five bar
rows match the original's rect and colour exactly, and are flagged only as reordered within the frame.

Both knobs are default OFF. Gates are unchanged with the defaults and with both knobs ON.

## 2. Arm 1: the original (`re/frida/menu_draw_burst.py --nav`, `coloursel/run_lr_orig.sh`)

| nav after confirm x3 | 0x0067f0c0 | x 0x0067f0c4 | 0x0067ea98 |
|---|---|---|---|
| right | 2 | 240.0 | 4 |
| right x6 | **6** (clamp) | 520.0 | 4 |
| right, right, left | 2 | 240.0 | 4 |
| left | **1** (floor) | 170.0 | 4 |

The static reading is in PREREG section 1.

## 3. Change (f97b2bda2)

- `exe_main.cpp` screen-4 left/right: when `ColourSelIcon_On()`, decrement 0x0067f0c0 if > 1 / increment if < 6, before
  the existing `CarSelectCycleColour` (0x0067ea98 + FUN_00431b80), which stays as it is.
- `exe_main.cpp` colour-select rows: when the single-row path is on (BAR_Y, cup flow) and `ColourSelBorder_On()`, draw
  fill 48,207 534x26, left 46,207 2x26, top 46,207 538x2, bottom 46,231 538x2, right 582,207 2x26, in the original's
  order and colours. Otherwise the 3-quad bar is drawn as before.
- `Frontend/MenuNavSM.{cpp,h}`: `ColourSelBorder_On()` (env `MASHED_COLOURSEL_BORDER`, read once).
- run_arms.py: stack `CSIX` = ICON + BORDER. `re/frida/menu_draw_burst.py` is the HB-MENUBURST-NAV blob (f3075819e),
  carried here identical.

Build `b5989ce0` (`MASHED_NO_DEPLOY=1`): rva-lint `NEW=0`, Build OK.

## 4. Gates (private root `D:/MashedWT/PORT-COLOURSEL-LEFTRIGHT_root`, `coloursel/run_lr.py`)

| arm | hashes |
|---|---|
| H_DEF / H_CSIX | `8284be46 / 67931834 / 31fe043e` both |
| B_DEF_K0 / K1 / K26 | `1d5bf4f9` x3 |
| B_CSIX_K0 | `1d5bf4f9` |

## 5. walk_04 (nav demo, hand path, BAR_Y default ON)

| run | walk_04 sha256/16 | border rows at x 560 | border columns at y 220 (edges) | red px, column 1 |
|---|---|---|---|---|
| original (WALK04) | `d40b38fdd6a30c50` | 207 208 231 232 | 46 47 582 583 | 112 |
| default | `4d53d93f87f0332d` (= BAR-Y's ON file) | 207 233 | 582 583 584 | 0 |
| ICON | `546fa766d2e3a3bc` | 207 233 | 582 583 584 | 98 |
| BORDER | `6cec75cdb34e1789` | **207 208 231 232** | **46 47 582 583** | 0 |
| ICON + BORDER | `ba34030d1b7e2fe4` | **207 208 231 232** | **46 47 582 583** | 98 |

In each knob run the other 11 captures are byte-identical to the default.

## 6. Draw-list diff, ICON + BORDER (HB-MENUBURST-NAV recipe)

Original `cs_f0` (`D:/MashedWT/HB-MENUBURST-NAV_data/orig_cs.json`) against exe f282 (`stream_both.json`, colour select
frames 268..295), `--scale-b 1`. Summary: matched 35, mismatched 51, missing 12, extra 96. Listing:
`verify/portstate_20261109/COLOURSEL_LEFTRIGHT_diff_both.txt`. Before this step (HB-MENUBURST-NAV, BAR_Y + ICON) the
same diff had 14 missing.

| original row | exe row | state |
|---|---|---|
| A[16] fill 48,207 534x26 `7ff06e14` | B[175] 48,207 534x26 `7ff06e14` | equal rect and colour, reordered |
| A[17] left 46,207 2x26 `ffb45010` | B[176] the same | equal, reordered |
| A[18] top 46,207 538x2 | B[177] the same | equal, reordered |
| A[19] bottom 46,231 538x2 | B[178] the same | equal, reordered |
| A[20] right 582,207 2x26 | B[179] the same | equal, reordered |

"Reordered" means the five come later in the exe's frame (index 175..179 against 16..20), while their order among
themselves is the original's. The colour tiles (A[4..15]) and the frame-level draw order are other composition rows,
outside this step.

## 7. Arm 6: left/right on the exe (`coloursel/lr_exe_poll.py`, `MASHED_GOTO=1 MASHED_NAV_KEYS=e,e,r,r,r,r,r,r,l`)

| run | 0x0067f0c0 over the keys | 0x0067ea98 |
|---|---|---|
| ICON ON | 0 → **1** (screen-4 entry) → 2 → 3 → 4 → 5 → 6 → **6** → **5** | 0 → 1 … 6 → 5 (unchanged law) |
| ICON OFF | 0 throughout | 0 → 1 … 6 → 5 |

The ICON ON trace follows the original's law: entry 1, clamp at 6, the left steps back. With the knob on, 0x0067ea98 still
moves on its own wrap law (0..6). On the original it stays 4 on this screen. That cursor feeds the exe's race livery
(`exe_main.cpp:2257`), and the original commits the colour from 0x0067f0c0 through 0x007f1a1c at the confirm
(0x0043e56c..0x0043e57e, U-9672). So with ICON on, the icon and the race livery can differ until that commit is ported.
That is the remaining blocker for an ICON flip.

## 8. Commits

| commit | content |
|---|---|
| c05058944 | PREREG (alone) |
| f97b2bda2 | the two changes, `CSIX`, `menu_draw_burst.py` (HB-MENUBURST-NAV blob), `run_lr_orig.sh` |
| next | `run_lr.py` |
| this commit | `lr_exe_poll.py`, `run_lr_keys.sh`, `COLOURSEL_LEFTRIGHT_diff_both.txt`, this RESULT |

## 9. Raw files (`D:/MashedWT/PORT-COLOURSEL-LEFTRIGHT_data`, not committed)

| file | sha256 |
|---|---|
| exe/mashed_re.exe | b5989ce0a938f5258ce453e3648307a6a866ef9a43575c09c7e8d5de1c6ec8bb |
| build.log / run_lr.log | 05ff754b1d869d2badd04962cf668e7fd00b8f938fb212c929ccd6bcc38d7eb8 / e80e1032205afa3a8a76c4646d8edceb92b50fa100d038e01acc2b1f7d9c3240 |
| lr_orig.log | ec98be3cbb60fef2391eb3446de02a323d84bc8c825ee8301a4bc3703f9bd189 |
| lr_r1.peek.json, lr_rrl.peek.json | fb91b6c3d5646d38f1a98c1ca25ceeeeea3a5c3775fd0aa229795efbde162945 |
| lr_r6.peek.json / lr_l1.peek.json | 3246108a91c72a865d5d8234be6aa75867d4727ef38967da4bbf43e49be3bef6 / a9cac83715ae918e5db862e057f6e4ca4f8e25247236634052994094f8b61520 |
| lr_exe_on.json / lr_exe_off.json | 2d7c6e485d807e1df77746849cdb88f5a62ad6989841a289e0570ddfb067ac97 / e5084f678f0f2c821b51258f8776bf9c5bd4ca6011a24cedb51acdc9187c3a65 |
| lr_keys.log | df24ff391fb72fdb8dcf5627308c584eabf4ce9c60587aa550f71fb78ace7df5 |
| stream_both.json / diff_both.json / diff_both.txt | 6d58ee646d810298c2cfbc8431f4ee93884ecfcd39d0cb832a5cfabc105549fd / 15ef43726969b6cea37da91a0a8cf18c6c7552aa6a4f4f6235d77a5f42041008 / d9f96825649f20eef1a408d58fae4acaef230f7fbaa1ab20f63d5312900c428f |
| arms/H_DEF.step.csv, H_CSIX.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv / ptrace.log | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c / 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0, B_CSIX_K0 step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| walk_def / walk_icon / walk_border / walk_both walk_04 | 4d53d93f87f0332d3d757d584b0fbaa9bf5ea54a6883b70fd5da920157bba9b2 / 546fa766d2e3a3bc5a11265d270d8e6d2276b70369ce4695e4a8de76157836e2 / 6cec75cdb34e1789b18ffba8428b97550234534c84d98ad91ac470e00c682b7e / ba34030d1b7e2fe4eb69132a852cd296278fff04f788e3660607ea06670b1e2f |
