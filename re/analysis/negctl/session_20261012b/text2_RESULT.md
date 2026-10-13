# text2: screen 18/24 leftovers and the three remaining text call sites (2026-10-13, claude-opus-5-5)

Worktree `.worktrees/text2`, branch `work/text2`, parent `77e7e529`. Part A commit `92f8d3ec`, Part B in the
commit after it. Evidence `verify/scr19_parity_20261012/text2/` (Part A) and `.../text2/sweep/` (Part B).

## Verdicts

| item | verdict | before | after |
|---|---|---|---|
| A1 mode labels x | **FIXED** | dx +2.0000 (labelX 66) | dx 0.0000 on every label of s18 and s24 |
| A2 mode values x | **FIXED** | dx +3.5000 (valRight 312.5) | dx 0.0000 (max 0.0001) on every value of s18 and s24 |
| A3 value arrow | **FIXED** | s18 left arrows x -0.54..-12.69 px, y -0.25, 12.5 px square, value measured at 0.6, always opaque. No right arrow | x/y/w/h/colour exact on all 12 arrows of s18 and s24, at strict and 0.01 px |
| A4 s18 plates | **FIXED** | 1 px high, x 62.5, 169+81 wide, 1 px borders | every plate quad matches at strict and 0.01 px, on s18 and s24 |
| B1 scr4 digit (`5389`, was 5364) | **FIXED** (law). Count stays `[UNCERTAIN]` | +18.1918 (3 of 4 paired), dx +4.85, dh -1.6992 | at n=3: dy +17.0000, dx/dh 0.0000. At n=4 (the count the original capture had): dx/dy/dh 0.0000 on 4 of 4 |
| B2 team tabs (`5762`, was 5741) | **FIXED** | +0.8081, dx +0.494, dh -1.6312 (s16) | 0.0000 / ≤0.0001 / 0.0000 |
| B3 player numbers (`5957`, was 5912) | **FIXED** (law). Screen-15 x carries a state difference | +1.8418, dx +5.0 (s15) / +1.0 (s16), dh -1.6992 | s16: 0.0000 / 0.0000 / 0.0000. s15: dy 0.0000, dh 0.0000, dx +130.0000 = one ability column (state, section B3) |

Every site on the shared law stays 0.0000 on all 33 screens. widget_red stays 0 on all 12 scr19/30/32
draw-list diffs.

Remaining RED, all cited below:
1. The 12 arrows on each of s18/s24 are flagged `reordered` and nothing else. The draw sequence differs
   from the original's (A3).
2. B1's row count n (`[0x00772ffc]`) is live device state. The standalone models 3, and the probe's original
   had 4.
3. B3's screen-15 x: the original capture is a bare nav push and leaves `[0x0067e850]` at 0. The standalone's
   entry sequence sets it to 1, so its sprite and number sit one column right.

## Part A: what the bytes say

All listings are from `original\MASHED.exe.unpatched` via `scr32_texty_recheck/mdis.py`, files `text2/bytes_*.txt`.

**Value column, `FUN_0043af10` per row (`0x0043b0fd..0x0043b17c`):**
- `A = W * 0.484375` (`[0x005cda78]` `0x3ef80000`, `0x0043b10d`).
- `ay = H * (y - 6.0) * 0.00208333344` (`[0x005cd0a0]` 6.0, `[0x005cc560]` `0x3b088889`, `0x0043b12b..0x0043b139`).
- `aw = W * 0.01875` (`[0x005cda74]` `0x3c99999a`, `0x0043b14d`).
- `ah = H * 0.025` (`[0x005cc9a4]` `0x3ccccccd`, `0x0043b176`).
- W / H are the words `[0x0067ea54]` / `[0x0067ea56]` (`FUN_0042b8b0` / `FUN_0042b8c0`, `0x0042b8b0: mov ax, [0x67ea54]`).
  The original's quads put the right arrow at x 310 = 640 * 0.484375, so 640 x 480 on this frame.

**Value text:** `FUN_00427e00(id, (A - 1.0) / W * 640.0, y, colour, s, 1)`.
- s18: `0x0043b4f2..0x0043b524`, s = `0x3f051eb8`.
- s24: `0x0043b2f4..0x0043b326`, s = `0x3f147ae1`.
- x = 309.0, the `0x439a8000` the s18 capture passes. **A2.**

**Label:** the record path `FUN_0043c5b0` passes the record x `[esi-0x14]` (`0x0043cc68`). The capture shows
`0x42800000` = 64.0 on every row of both screens. **A1.**

**Left arrow (A3):** `FUN_00473870("Arrow", x, ay, aw, ah, colour, 1)` at `0x0043b590`.
- `w = _ftol2(FUN_004282a0(id, s))` with the value's own s (`0x0043b529`/`0x0043b52f`). `0x004a2c48` is MSVC
  `_ftol2`, which truncates.
- `D = (W * (w + 0x12)) / 640` (`0x0043b53e..0x0043b55f`: imul, `0x66666667` multiply, `sar 8`, sign fix).
- `x = (A - D) + 2.0` (`0x0043b568..0x0043b573`, `[0x005cc574]` 2.0).
- s24 is the same at `0x0043b339..0x0043b372`.
- `FUN_0040bb50` (`0x0040bb50..0x0040bb64`) is only the texture lookup `FUN_004c5c00([0x63b8fc], name)`.

**Right arrow (A3, not drawn by the standalone before):** `FUN_004739f0("Arrow", A, ay, aw, ah, colour,
0x3f7ff972, 0, 0, 1.0, 0, 1)` at `0x0043b3d5` (s18) / `0x0043b1f8` (s24). The u is mirrored, and the captured
vertices confirm it: TL u 0.9999, TR u 0.

**Arrow gate:** s24 row 3 draws both arrows only when `FUN_00430830(1) != 0` (`0x0043b186..0x0043b199`,
`0x0043b35f..0x0043b388`). The port calls the existing `SplitScreenTrackAssignmentFor(game_mode, 1)` for it.
s18 has no such test. Both arrows take the value colour `edi`, which includes the `0x30` alpha on unavailable
rows.

**Plates (A4), `FUN_0043c5b0`:**
- `y = rec.y - 13.0 + 1.0` (`0x0043c766` `[0x005cd8d8]`, `0x0043c77e` `[0x005cc320]`). x 60.0 (`0x0043c7a1`).
- Tables `0x5f72a0` (s18) and `0x5f7370` (s24) take the narrow branch (`0x0043c78b..0x0043c7b0`):
  - solid w 166.0 (`0x43260000` @`0x0043c7e3`)
  - fade x 226.0 (`0x0043c7f3`)
  - fade w 84.0 (`0x0043c7eb`)
  - Other tables use 210 / 270 / 100 (`0x0043c7c9..0x0043c7d9`).
- h 26.0 (`0x0043c80c`).
- Borders are drawn only when h > 2.0 (`0x0043cd0a` / `0x0043cf35`). Each is 2.0 thick (`0x0043cd43` / `0x0043cf6b`):
  - top at y
  - bottom at y + ftol(h) - 2 (`0x0043cd86` / `0x0043cfb6`)
  - left at x - 2.0, w 2.0, h ftol(h) (`0x0043cdfb`, `0x0043cde3`, `0x0043ce05`)
- Border colours:
  - selected `b4 50 10 ff` (`0x0043cf3e..0x0043cf4f`)
  - idle `13 15 50` with alpha `0xff` (`0x0043cd13..0x0043cd1d`, `0x0043cbcf`)
  - `FUN_0042aad0` (`0x0042aad0: mov byte [eax+3], 0x30`) sets alpha 0x30 when the row's avail slot is 0
    (`0x0043ccaf..0x0043ccbd`).
- Selected fill: `f0 6e 14 a0` (`0x0043cef5..0x0043cf04`).
- Selected row also gets the "Button" cap (`0x005cda7c`) through `FUN_004739f0` (`0x0043d0b0`) at x - 2 - 13
  (`0x0043d02b`, `0x0043d063`), w 13.0 (`0x0043d08b`).

The standalone already used x 60 / h 26 / 2 px borders / left border / Button for the generic list rows
(`exe_main.cpp:4786..4847`). The s18/s24 block had its own ink-fitted copy, and that copy was the 1 px.

Two things went past the brief's wording, both from the bytes:
- The right arrow, left border, Button cap and idle/disabled border colours were added or corrected along with y.
  "Plate quads and arrow x match the original" cannot be GREEN without them.
- The Button cap keeps the generic path's u0 = 0.125 crop (#7, user round 2026-06-13). The original passes full
  0..1 UV, and the draw-list diff does not compare UV.

## Part A: measurements

Glyph vertices against `scr32_texty_recheck/orig_frame{18,24}.json` (`text2/quad_partA.txt`), base → A:

| screen | site | before dx / dy / dh | after dx / dy / dh |
|---|---|---|---|
| 18 | labels `0x43d164`/`0x43d10c`, 7 rows | +2.0000 / 0.0000 / 0.0000 | 0.0000 / 0.0000 / 0.0000 |
| 18 | values `0x43b529`, 6 rows | +3.5000 / ≤0.0001 / 0.0000 | 0.0000 / ≤0.0001 / 0.0000 |
| 24 | labels, 8 rows | +2.0000 / 0.0000 / 0.0000 | 0.0000 / 0.0000 / 0.0000 |
| 24 | values `0x43b32b`, 7 rows | +3.5000 (row 4: +3.5001) / 0.0000 / 0.0000 | 0.0000 (row 4: +0.0001) / 0.0000 / 0.0000 |

Draw lists against fresh original captures `text2/dl_orig{18,24}.json` (`--exclude-tex 9 --rotate-a 0x42e65a`,
`text2/drawlist_partA.txt`, bucketed by original caller with `text2/s18sum.py`):

| screen | diff | before: plate+arrow RED | after: plate+arrow RED | after detail |
|---|---|---|---|---|
| 18 | anim4 | 80 (+6 band tiles) | 12 | 12 arrows `reordered`, 0 plate |
| 18 | strict | n/a | 12 | same |
| 18 | 0.01 px | n/a | 12 | same |
| 24 | anim4 | 82 (+6 band tiles) | 12 | 12 arrows `reordered`, 0 plate |
| 24 | strict / 0.01 px | n/a | 12 | same |

The `region` column in `drawlist_partA.txt` also counts 6 background band tiles (w 128, `60ffffff`, standalone
`RenderFrame`). They sit in the region box and are not widget rows. They are identical before and after.

**Why the arrows still read `reordered`.** In the original, `FUN_0043c5b0` draws every plate first. Then come
the `0x436926` preview frame and the `0x4db7da` panel. Only then does `FUN_0043af10` draw the values and arrows
(`dump_orig18_f0.txt` records 4..53, 54..70, 71..82). The standalone draws each row's arrows inside the plate
loop, and its preview and panel after the block (`dump_sa18_a.txt`). Matching the order means moving the
standalone's preview, panel and value pass, which are other subsystems' draws. Left as is. The geometry and
colour of every arrow are exact.

## Part B

### B1: screen 4 digit (`exe_main.cpp:5389`, original ret `0x00433efc`)

**Pairing.** The original makes exactly 4 calls from `0x433efc` on screen 4, all `FUN_00427f00(str, 111.0, y,
0xff000000, 0.6, 0)` at y 189/223/257/291 (`orig_frame4.json`). The string is `'1' + row`, built at
`0x00433ed9..0x00433ee6`, or `row - 6` on the `[0x007f0fe8]` path at `0x00433e82..0x00433eac`. The standalone
draws `'1' + r` for r = 0..2.

So the probe's pairing is correct by string: standalone "1" ↔ original "1". The positions differ because the
two sides lay out a different number of rows. That is the +17 px of the +18.19.

**Law, `FUN_004335f0` `0x00433e42..0x00433ef7`:**
- `x = float(_ftol2(FUN_004282a0(0x36, 0.8)) / 2 + 4) + rec.x + 15.0`. The measure is at `0x00433e42..0x00433e62`
  (cdq/sub/sar is signed /2), `fadd [edi-4]` at `0x00433ed0`, and `[0x005cc9b0]` 15.0 at `0x00433edb`.
- `y = rec.y` (`[edi]`, `0x00433ecc`). The icon gets `rec.y - 1.0` (`0x00433e24..0x00433e28`).
- Scale `0x3f19999a` (`0x00433ed3`), align 0 (`0x00433ece`).

`rec` is `FUN_004332a0`'s profile record:
- x 85.0 (`0x42aa0000` @`0x00433567`), or 170 + 70*(choice-1) (`0x004335d4..0x004335de`).
- y = `Q_ListBaseY(n, 0x22) + 34*k` (`0x0043350c..0x0043351b`, `[0x005cd908]` 34.0 @`0x00433598`).
- `n = FUN_00496910()` = `mov eax, [0x00772ffc]` (`0x00496910`).
- n = 4 gives 189, which matches the capture.

**Cause.** The anchor was the fitted bar centre `by + bh/2` = 206.35 + 34r, against the law's 206 + 34r at
n = 3. The cell was 0.55 against 0.6, and x was `ix + isw + 5` = 115.85 against 111.

**Fix.** The digit now uses the law with n = the standalone's `nrows4`. The count stays the existing
`[UNCERTAIN]` (live device state). The bars and icon are untouched.

**Proof at equal n.** Build `mashed_re_b4.exe` is Part B with only `nrows4 = 4`, built and then reverted
(`sweep/quad_b4.txt`). Against the n = 4 original it reads dx/dy/dh **0.0000 on 4 of 4 digits**. The base build
had 3 of 4 matched at +18.1918. At the shipped n = 3 the sweep reads dy +17.0000 = 206 - 189, with dx/dh 0.0000.

### B2: team header tabs (`exe_main.cpp:5762`, rets `0x0043ab7a` / `0x0043abb4`)

`FUN_0043aa30` makes two calls:
- `0x0043ab5e..0x0043ab75`: `FUN_00427e00(0x9f, 0x43a00000 = 320.0, 0x431a0000 = 154.0, colour, 0x3f4ccccd = 0.8, 2)`
- `0x0043ab98..0x0043abaf`: id `0xa0`, x `0x43fa0000` = 500.0

Their plates are `FUN_0042f8d0(250.0 | 430.0 (0x43d70000), 140.0, 140.0, 28.0)` (`0x0043ab1f..0x0043ab55`,
`0x0043ab7c..0x0043ab8c`). The standalone's x (hx + hw/2) and y (hy + hh/2) already land on 320/500 and 154.

**Cause:** the cell only, the 0.752 ink fit against 0.8. **Fix:** cell 0.8 and y written as the literal.
The 0.752 constant (`hcell`) is deleted. Its only other user, the ability path, already used 0.75.

**After (s16):** dy 0.0000, dx +0.0000 / +0.0001, dh 0.0000. Screen 9 does not draw this path in the
standalone: 0 matched before and after, and the screen-9 records are byte-identical between the builds apart
from the checker.

### B3: player numbers (`exe_main.cpp:5957`, rets `0x0043a8f1` (s15) / `0x0043ae75` (s9, s16))

Both renderers use `FUN_00427f00(str, x, y, colour, 0x3f19999a, 0)`.

| | ability, `FUN_0043a610` `0x0043a810..0x0043a8ec` | team, `FUN_0043aa30` `0x0043adb5..0x0043ae70` |
|---|---|---|
| base | `[0x0067e850 + prof*12] * 0x82 + 64.0` (`0x0043a810..0x0043a83b`, `[0x005cd6d4]`) | `pick*0xb4 + 82.0` (`[esp+0x30]`) |
| x | `_ftol2(FUN_004282a0(0x36, 0x3f666666)) / 2 + base + 65.0` (`0x0043a87b..0x0043a8dd`, `[0x005cda6c]` 65.0) | `(_ftol2(FUN_004282a0(0x36, 0.9)) / 2 + 4.0) + base + 65.0` (`[0x005cc35c]` 4.0 @`0x0043ae1f`) |
| y | `(rowY - 2.0) + 15.0` (`0x0043a83f..0x0043a853`) | `rowY - 2.0 + 15.0` (`0x0043adb5..0x0043adce`) |

The base is the same value that places the row's car sprite: `FUN_004739f0` at `0x0043a93a` takes x from the
same slot. So the number slides with the pick, as the sprite does. Captured: x 137 (s15) and 159 (s16), y 213 +
40r. That gives `_ftol2(...)/2` = 8 and base 64 / 82.

**Cause.** The standalone anchored at the plate centre (rowY + 14 against + 13), used cell 0.55, and x =
static `carX + 78` instead of the sliding base + 73 (+4 team).

**Fix:** the law, using the existing `AbilityPick`/`TeamPick` base.

**After:**
- s16: dx/dy/dh 0.0000 on both drawn rows.
- s15: the glyph box goes from (142.0, 204.7408, h 18.6912) to (267.0, 202.899, h 20.3904). Against the original
  (137.0, 202.899, h 20.3904), that is **dy 0.0000, dh 0.0000, dx +130.0000**. The matcher's 40 px window
  therefore reports it unmatched.

The +130 is exactly one `0x82` column, and it is state, not law. The original capture is a bare
`FUN_0043d2a0(15, 0)` push, and the drawn profile's `[0x0067e850]` reads 0 (number at 137 = 8 + 64 + 65). The
standalone's `MASHED_GOTO=15` runs the entry sequence. At `exe_main.cpp:5562` that sets all twelve entries to 1
when `CarSlotAssign` returns 0x1000, and `MenuHelpers.cpp:84..88` says the original's screen-15/16 entry does the
same reset. The standalone's car sprite already sat at x 194 (= 1*130 + 64) before this change (`sa15_base.json`),
so it carries the same offset. A like-for-like check needs an original capture taken through the entry
sequence, or a standalone run with `[0x0067e850]` = 0.

The string stays "1". The original builds `'1' + profile` (`0x0043a8b6..0x0043a8c5`, `0x0043ae38..0x0043ae47`),
which is "1" for the profile-0 rows the port draws. Drawing the true per-profile digit is a follow-up.

## Part B: verification

**Per-call-site dy, all 14 measured standalone sites.** 33 screens (0..33 except 27), base (77e7e529) → B,
against `scr32_texty_recheck/orig_frame<S>.json`. Sources: `sweep/quad_diff_sweep.txt`, `sweep/sites_table.md`.

| line (now) | site | ret | screens | dy before | dy after | notes |
|---|---|---|---|---|---|---|
| 4598, 4600 | watermark | `0x43c6c7`, `0x43c6f8` | 32 | 0.0000 | 0.0000 | |
| 4709, 4711 | prompt + shadow | `0x43d239`, `0x43d25a` | 33 | 0.0000 | 0.0000 | |
| 4921, 4931 | menu rows s0.8 | `0x43d164`/`0x43d10c` | 20 / 3 | 0.0000 | 0.0000 | |
| 5101, 5131 | scr19/32 values | `0x431202`, `0x431426` | 19, 32 | 0.0000 | 0.0000 | |
| **5389** | scr4 digit | `0x433efc` | 4 | **+18.1918** (3/4) | +17.0000 at n=3, **0.0000 (4/4) at n=4** | dx 0.0000, dh 0.0000 |
| 5762 | tabs, ability | `0x43a9b5..0x43aa0c` | 15 | 0.0000 | 0.0000 | |
| **5762** | tabs, team | `0x43ab7a`/`0x43abb4` | 16 | **+0.8081** | **0.0000** | dh -1.6312 → 0.0000 |
| **5957** | numbers, team | `0x43ae75` | 16 | **+1.8418** | **0.0000** | dx +1.0 → 0.0000, dh -1.6992 → 0.0000 |
| **5957** | numbers, ability | `0x43a8f1` | 15 | **+1.8418** | 0.0000 (box) | dx +5.0 → +130.0 (state) |
| 6285 | cup names | `0x439aec` | 6, 7 | 0.0000 | 0.0000 | |
| 6625, 6645 | s18/s24 rows / values | `0x43d164`/`0x43d10c`/`0x43b529`/`0x43b32b` | 18, 24 | ≤0.0001 | ≤0.0001 | dx +2.0/+3.5 → 0.0000 (Part A) |

Per-screen mean |dy| is 0.0000 on 32 of 33 screens. The exception is screen 4 at 0.6000 (n = 3 against the
capture's 4). It was 0.6421 before, and screens 15/16 were 0.0396/0.1716 before.

**Record level, base → B, all 33 screens** (`sweep/ds_compare_all.txt`):
- Every screen differs in the 42 checker tiles, the boot animation the previous pass's control showed.
- Screens 6/7 differ in one more non-glyph record, the time-based tex-86 pulse noted there.
- Glyph changes appear only on screen 4 (3 digits), screen 15 (2 numbers) and screen 16 (12 tab glyphs + 2
  numbers).
- Screens 18 and 24 each gain 14 records:
  - s18: 6 right arrows + 7 left borders + 1 Button.
  - s24: 6 right arrows + 8 left borders + 1 Button, minus the row-3 left arrow.

**scr19/30/32 widget draw lists** (`sweep/drawlist_summary.txt`, against `scr32_texty_recheck/dl_orig*.json`):
- widget_red 0 on all 12 (19a, 19b, 30, 32 × anim4 / strict / 0.01 px).
- The `other` rows are the ones the previous passes listed: the 512x512 backdrop's -0.5 offset at 0.01 px, and
  the scr19a preview quads `0x474bd3`/`0x474d0c`.

**Coverage.** No new reachable screen came up. The 33 unreachable call sites are unchanged. Of the 47, these 14
are measured. 5389 is now measured exactly, at a matched n.

## Follow-ups not taken

- Draw order on s18/s24 (values and arrows after the preview/panel draws), so that the 12 `reordered` rows go
  GREEN.
- The screen-4 live device count `[0x00772ffc]` (`FUN_00496910`). The bars and icon there are still ink-fitted
  (`by0`, `iy`). The bytes give icon y = rec.y - 1.0, centred on rec.x.
- An original screen-15/16 capture taken through the entry sequence, so that `[0x0067e850]` = 1 on both sides.
- Per-profile digit strings on screens 15/16 (`'1' + profile`, or `- 6` on the `[0x007f0fe8]` path).

## Proposed CHANGELOG line

`2026-10-13 — Frontend s18/s24 rows and three text sites from the bytes (work/text2): labels x 64.0 / values
x (A-1)/W*640 = 309 (A = W*0.484375, FUN_0043af10 0x0043b10d / 0x0043b4f2), both arrows ported (right
FUN_004739f0 at A, left FUN_00473870 at (A - W*(ftol(M(id,s))+18)/640) + 2, 12x12 at H*(y-6)*0.00208333,
s24 row 3 gated on FUN_00430830(1)), plates from FUN_0043c5b0 (x 60, 166 + fade 226/84, y rec.y-12, 2 px
borders + left border + Button, alpha 0x30 via FUN_0042aad0); scr4 digit (FUN_004335f0 0x00433e42..: x
ftol(M(0x36,0.8))/2+4+rec.x+15, y Q_ListBaseY(n,34)+34k, 0.6), team tabs 0.8 (0x0043ab60), player numbers
(0x0043a810.. / 0x0043adb5..: off the sliding sprite base, y rowY+13, 0.6). Labels/values dx/dy/dh 0.0000 on
s18/s24, every plate and arrow quad exact at 0.01 px (arrows still `reordered`), sites 0x433efc (at n=4),
0x43ab7a/0x43abb4, 0x43ae75 0.0000, 0x43a8f1 dy/dh 0.0000 with dx +130 from [0x0067e850] state; widget RED 0
on scr19/30/32. Evidence verify/scr19_parity_20261012/text2/, note
re/analysis/negctl/session_20261012b/text2_RESULT.md.`

## Provenance

`verify/scr19_parity_20261012/text2/PROVENANCE.txt`.
