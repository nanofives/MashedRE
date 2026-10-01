# Arctic sea-tile fix — acceptance results (2026-09-30)

Rules are the ones pre-registered in [`PREREG.md`](PREREG.md), committed as `548ac7ce`
**before** any edit under `mashedmod/src`. Nothing below was amended after the fact.

| rule | verdict |
|---|---|
| 1 — exactly 25 sea instances at the original's positions | **PASS** |
| 2a — texture-detail recovery, 6 gating boxes, 2 poses | **PASS** (6/6) |
| 2b — edge placement vs the original's live-read matrix | **PASS** |
| 3 — the tiling is Arctic-only (other 12 tracks) | **PASS** (12/12, control passed) |
| 4 — Arctic cars and props unchanged | **FAIL as written** (3 of 5 boxes) — edit kept, see below |

## Binaries

| arm | SHA-256 | size |
|---|---|---|
| pre-fix, HEAD `0166167e` — `bin/mashed_re_prefix.exe` | `5327511B605A8DE4A580974AF15011DFE7A13C56BB71EDEE1030065A334F71F3` | 1 961 472 |
| post-fix — `bin/mashed_re_postfix.exe` | `0FF413EA33264B6D9F6F3437E3529AAB03DAC1E0ADFA0B1233DF560D3B223154` | 1 961 984 |

`scripts/lint_rva_bodies.py` on both: `122 finding(s) ... allowlisted=122 NEW=0`.
The prop-instance loop has exactly one body in the tree (`grep` for `MatIdentity(&id)` /
`s_skip_excluded` returns only `TrackRenderer.cpp:1712-1717`), so there is no second live
copy of the sea/track-hook code to drift.

## Rule 1 — PASS

`mashed_re.log` from the Arctic run (`post_all/00_Arctic/`):

```
SEA-TILE course_id=0 clump=2 dff=sea.dff instances=25 grid=5x5 origin=-150 step=60 y=-4.1
```

All 25 instances, as logged:

| # | position | # | position | # | position |
|---|---|---|---|---|---|
| 00 | (−150.000000, −4.100000, −150.000000) | 09 | (−90.000000, −4.100000, 90.000000) | 18 | (30.000000, −4.100000, 30.000000) |
| 01 | (−150.000000, −4.100000, −90.000000) | 10 | (−30.000000, −4.100000, −150.000000) | 19 | (30.000000, −4.100000, 90.000000) |
| 02 | (−150.000000, −4.100000, −30.000000) | 11 | (−30.000000, −4.100000, −90.000000) | 20 | (90.000000, −4.100000, −150.000000) |
| 03 | (−150.000000, −4.100000, 30.000000) | 12 | (−30.000000, −4.100000, −30.000000) | 21 | (90.000000, −4.100000, −90.000000) |
| 04 | (−150.000000, −4.100000, 90.000000) | 13 | (−30.000000, −4.100000, 30.000000) | 22 | (90.000000, −4.100000, −30.000000) |
| 05 | (−90.000000, −4.100000, −150.000000) | 14 | (−30.000000, −4.100000, 90.000000) | 23 | (90.000000, −4.100000, 30.000000) |
| 06 | (−90.000000, −4.100000, −90.000000) | 15 | (30.000000, −4.100000, −150.000000) | 24 | (90.000000, −4.100000, 90.000000) |
| 07 | (−90.000000, −4.100000, −30.000000) | 16 | (30.000000, −4.100000, −90.000000) | | |
| 08 | (−90.000000, −4.100000, 30.000000) | 17 | (30.000000, −4.100000, −30.000000) | | |

X set `{−150, −90, −30, 30, 90}`, Z set `{−150, −90, −30, 30, 90}`, Y set `{−4.1}` — exactly
the grid `FUN_00448940` builds (`0x004489bb` Y, `0x00448a65`/`0x00448a80` the −150.0 starts,
`0x00448a6f`/`0x00448a88` the 5×5 counts, `0x00448aab`/`0x00448ac1` the +60.0 step).
Sub-criteria (1) count==25, (2) set==grid, (3) all 25 listed: PASS.

**(4) branch off on a non-Arctic track: PASS.** `MASHED_TRACK_SEL=12` (training) logs
`instances=0`, `header lines=0`.

Two notes on this sub-criterion, both recorded rather than glossed:

- **My checker initially reported rule 1 FAIL, and the checker was wrong, not the rule.**
  It additionally demanded a `SEA-TILE-OFF` control line on training, which `PREREG.md`'s
  criterion — *"the branch does not fire: 0 sea-tile instances logged"* — does not ask for.
  The checker was corrected down to the committed criterion, not the criterion up to the
  checker.
- Training's missing `SEA-TILE-OFF` line is a **silent negative**, so it is not used as
  evidence. `training/COURSE.LUA` declares **no `Clump_Filename` at all** (extracted and
  grepped this session), so the loop body never runs there. The control is instead
  demonstrated **live on 8 other non-Arctic tracks** that do declare a clump 2 and print
  `SEA-TILE-OFF lines=1`: Egypt, City, Forest, Highway, SuperG, Warzone, rouabout, dump.
  Neustein (no `Clump_Filename`), Storm (`Clump_Filename(2,"Tree.dff")`, which did not load)
  and sands (no index 2) print 0, each for a read reason
  (memory `absent-log-proves-nothing-run-a-control`).

## Rule 2a — PASS, 6 of 6 gating boxes

`g` = mean |Sobel gradient| of luma, `L` = mean luma, `g/L` the brightness-invariant form.
Thresholds are the ones fixed in `PREREG.md`.

| pose | box | n | orig g / g/L | pre g / g/L | **post g / g/L** | need g ≥ | need g/L ≥ | pre→post diff | verdict |
|---|---|---|---|---|---|---|---|---|---|
| s8 | ROAD_UC | 11 700 | 2.3452 / 0.06276 | 0.4530 / 0.03047 | **2.1685 / 0.05525** | 1.3590 | 0.03138 | 10 633 (90.9%) | PASS |
| s8 | ROAD_MC | 12 000 | 2.0901 / 0.11399 | 0.1827 / 0.02320 | **1.3289 / 0.06761** | 0.5481 | 0.05700 | 2 555 (21.3%) | PASS |
| s8 | ROAD_LC | 10 500 | 2.8433 / 0.14923 | 0.0947 / 0.02031 | **2.5116 / 0.12086** | 0.2841 | 0.07462 | 5 583 (53.2%) | PASS |
| s14 | ROAD_FG | 13 200 | 2.9679 / 0.09224 | 0.1331 / 0.02243 | **2.2538 / 0.07583** | 0.3993 | 0.04612 | 11 445 (86.7%) | PASS |
| s14 | ROAD_MID | 12 000 | 2.6370 / 0.07472 | 0.4184 / 0.03534 | **2.3024 / 0.06207** | 1.2552 | 0.03736 | 11 198 (93.3%) | PASS |
| s14 | ROAD_R | 10 200 | 2.8027 / 0.12856 | 0.1764 / 0.02292 | **2.2801 / 0.08089** | 0.5292 | 0.06428 | 8 174 (80.1%) | PASS |

Non-gating, reported: s8 `ROAD_UL` n=14 000, orig 0.8229/0.04190, pre 0.6421/0.03425, post
0.7358/0.03255. It still cannot discriminate, which is why it was excluded on the baselines.

Across the six gating boxes the port's relative gradient goes from **0.22–0.49×** the
original's to **0.59–0.88×**. Visual record: `result_sheet.png` (original | pre-fix | post-fix,
both poses).

## Rule 2b — PASS

Decoded **from the raw bytes** in `verify/sea_level_20260929/orig_sea_matrix.json` this
session, not taken from the note's prose. All 4 live samples of the original's `clump[2]`
frame:

```
modelling (frame+0x10)  right=(1,0,0) up=(0,1,0) at=(0,0,1) pos=(-150.0000, -4.1000, -150.0000)
LTM       (frame+0x50)  right=(1,0,0) up=(0,1,0) at=(0,0,1) pos=(-150.0000, -4.1000, -150.0000)
```

Port tile 0 = `(-150.000000, -4.100000, -150.000000)` — equal. Grid anchors span
−150…+90 in X and Z, each tile ±30, so the sheet spans **X, Z ∈ [−180, +150]**, 330 × 330 m.
PASS.

Screen-horizon form: not used, for the reason stated in `PREREG.md` before the fix — the
horizon is not in frame at either committed original Arctic pose (forward pitch −61.83° at s8,
−33.73° at s14, half-FOV 24.23°).

## Rule 3 — PASS, 12 of 12

Prerequisite determinism control (pre-fix vs a pre-fix repeat boot), both tracks
`over16 = 0/307 200`, `mean = 0.000` — so a 0 result below is the gate, not just determinism:

| control | over16 | mean |
|---|---|---|
| track 12 training | 0/307 200 | 0.000 |
| track 0 Arctic | 0/307 200 | 0.000 |

Post-fix vs pre-fix at the same deterministic pose, tracks 1–12: **every one
`over16 = 0/307 200`, `mean = 0.000`** — Egypt, City, Forest, Highway, Neustein, Storm,
SuperG, Warzone, rouabout, sands, dump, training. The tiling is Arctic-only, as the
original's own `Course_Id`-keyed scope requires.

## Rule 4 — FAIL as written. The edit is kept. Here is exactly why.

| pose | box | n | over16 | verdict |
|---|---|---|---|---|
| s8 | CAR_A | 2 250 | **509** | FAIL |
| s8 | CAR_B | 2 250 | **475** | FAIL |
| s8 | CAR_C | 3 300 | **417** | FAIL |
| s14 | CTRL_BLDG | 12 825 | 0 | PASS |
| s14 | CTRL_PIPE | 7 200 | 0 | PASS |
| gate check s8 | whole frame, `MASHED_NO_SEA_TILE=1` vs pre-fix | 307 200 | 0, mean 0.000 | PASS |
| gate check s14 | whole frame, `MASHED_NO_SEA_TILE=1` vs pre-fix | 307 200 | 0, mean 0.000 | PASS |

**Two-boot rule applied.** A second post-fix boot at s8 is **bit-identical** to the first
(`over16 = 0/307 200` boot1 vs boot2), and the three failing counts reproduce **exactly**:
509 / 475 / 417. The failure is deterministic, not a transient boot.

**The failure is in my instrument, not in the fix, and the test that shows it does not use a
colour class.** Running the pre-fix exe at the s8 pose with `MASHED_LIBRW_AMBFOLD_SEA=1`
re-shades **water-class geometry only** — the scope key is the DFF asset name
(`ModelIsWaterAsset`, `RwSceneBuild.cpp:497-513`) ANDed with the geometry-flag class
(`BatchIsWaterClass`, `:476-478`). `|base − fold| > 6` is therefore a geometry-scoped mask of
where the mis-placed sea was actually visible pre-fix (77.96% of the frame, 239 502 px):

| box | n | sea px in box (pre-fix) | changed px | of which inside the sea mask | **outside** |
|---|---|---|---|---|---|
| CAR_A | 2 250 | 1 181 (52.5% of box) | 509 | 509 (100.0%) | **0** |
| CAR_B | 2 250 | 566 (25.2% of box) | 475 | 475 (100.0%) | **0** |
| CAR_C | 3 300 | 430 (13.0% of box) | 417 | 417 (100.0%) | **0** |

Every one of the 1 401 changed pixels was showing the mis-placed sea before the fix. **Zero
car-body pixels changed.** The boxes were placed by eye on a brightened pre-fix capture and
contain 13–53% background beside the car, so they violate rule 4's own premise — they are not
"outside the sea region". The rule is reported FAIL as written and **not amended**; the edit is
kept because the evidence above shows the 1 401 pixels are squarely inside what this fix is
supposed to change, and because the two prop boxes (n = 12 825 and 7 200) and both whole-frame
gate checks are clean.

What rule 4 would need to be a real test: car boxes derived from the car's projected
silhouette (the geometry is available — `orig_carproj.txt` projects each car's world position
through the pose) rather than eyeballed, or the sea mask used as an exclusion on the car box.
That is a next-session instrument fix, not something to retrofit onto a committed rule.

## Still open after this fix

- **U1b** (`re/analysis/SEA_LEVEL_2026-09-29.md` §7) — the original renders the 25 tiles from a
  dedicated Arctic pass (`0x00449030`) that then issues eight state pairs through
  `RwGlobals+0x20`; the meaning of those pairs, and so the sea's draw order and blend mode
  relative to our generic prop pass, is still `[UNCERTAIN]`. Placement is what was fixed.
- **U2** — the original clones 24 clumps and world-registers each (`0x004e45b0` at
  `0x00448a1f`); the port expresses the same result as 25 instances of one model. Whether that
  per-clone world registration changes culling or draw order is not established.
- **U-9064 consequence**, recorded by the investigation session and untouched here: the
  `geomlight-waterfold` "Arctic sea FIXED, Δ1.8" verdict compared the original's **road**
  against our **water** over the same mask, so that Δ never rested on a like-for-like
  comparison.
- Pre-existing, visible in these captures and out of scope: all four cars render red (livery),
  and the base-pose copter/rotor prop is mis-drawn.

**No tracker row is mutated.** This reproduces `FUN_00448940`'s *effect* in the port's own prop
system; it is not a verbatim port of that function, carries no hook and no Frida diff, so it
takes no `hooks.csv` row and no C-level — the same standing as the (a) and (d) render fixes.
