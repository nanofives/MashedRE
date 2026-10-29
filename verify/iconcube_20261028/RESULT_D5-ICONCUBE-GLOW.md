# RESULT D5-ICONCUBE-GLOW (pickup glow on the rfx emitters)

Branch `orch/D5-ICONCUBE-GLOW` off `orch/integration` `327fd83c6`. PREREG
`verify/iconcube_20261028/PREREG_D5-ICONCUBE-GLOW.md` (`4c0f30c70`, committed alone). DEFERRED D-11076, L-45.
STANDALONE-ONLY: the original was not launched. Every original fact below is static (decomp_pc.py + capstone on
`original/MASHED.exe.unpatched`, SHA-256 `bdcae093...efd3c0e`) or from captures already on disk.

**Verdict: GREEN.** The glow is ported on the rfx emitters behind `MASHED_RFX_EMITTERS` (default OFF). Default and
knob-ON gates equal the merge_train references, the default 13-track table is identical to its reference, the
wiring check finds 0 mismatches in 70,608 particles, and the knob-ON draw lists carry a PUGlow draw with the
original's render state on all 39 captured rows. Original-side parity is pending (no audio endpoint).

## 1. What changed

- `mashedmod/src/mashed_re/Gameplay/PickupGlow.{h,cpp}` (new, exe-only, `exe_sources.rsp`): the glow batch
  (`FUN_004770c0(&DAT_0068b968, 0x817, 0x19, puglow)`, `0x004587c5..0x004587d2`, on `Rfx::Create`) and six
  cdecl shims for the sprite-queue callees of the verbatim FUN_00459000: `PK_GlowColour` (`0x004769f0`),
  `PK_GlowPos` (`0x004769a0`), `PK_GlowSize` (`0x004769d0`), `PK_GlowRotate` (`0x00476a30`, the float argument
  as its bits), `PK_GlowSubmit` (`0x00476d00`), `PK_GlowFlush` (`0x00476df0`). Knob off: each returns at once.
  Diagnostic `MASHED_DBG_PUGLOW=<file>` (PRE / POST pool rows and the flushed particles per tick).
- `re/tools/naked_specs/pickups_m2a.json` + regenerated `Gameplay/PickupTick_naked.inc`: the 11 call sites of
  those six RVAs bind `PK_Glow*` instead of `PK_Nop` (no other line of the .inc changes).
  `Gameplay/PickupTick.cpp`: the extern declarations, the dump hooks around `PK_459000`, header comment.
- `LibRw/RwRaceFx.cpp`: `DrawPickupGlow` in `RaceFx_DrawWorldFx`, after `DrawLensSmall` and before
  `DrawHeadlights`; `PuGlowTexture` loads `PUGlow` from SFX.piz FX.TXD on first use. `RwRaceFx.h` comment.
- Knob: `MASHED_RFX_EMITTERS=1` (existing, default OFF, not in FaithDefaults) and `PickupsOrig_Enabled()`.
  `MASHED_NO_PICKUP_GLOW=1` keeps the emitter layer on and the glow off.
- Not changed: the IconCube clumps (D5-ICONCUBE-SPIN), the D3D9 PickupField orbs, every default.

## 2. Static facts

| fact | where |
|---|---|
| glow batch create: `FUN_0040bb30("puglow")` (string `0x005ce5cc`), then `FUN_004770c0(0x0068b968, 0x817, 0x19, tex)` | `0x004587b4..0x004587d2` |
| batch teardown: `FUN_004768c0(0x0068b968)` | `FUN_00458880` `0x0045889c..0x004588a1` |
| pool loop entry n-1 .. 0, stride 0x50, entry base `0x0068b198`; type 0x15 forces state 0 | `0x0045901f..0x0045903b` |
| state 2 glow: timer +0x1c > 0, x = +0x18 - +0x1c < `[0x005ce548]` (1.0); size = `[0x005ce540]` / `[0x005ce544]` (1.25) * fsin(x / 1.0 * `[0x005ce2f4]` (pi)); pos +0x38; rot 0 | `0x00459057..0x004590dd` |
| state 1 glow: d2 = ((dy dy) + (dx dx)) + (dz dz), d = +0x2c - +0x38; size 1.25 * clamp(1.0 - d2, 0, 1) (1.25 when d2 <= 0); FUN_00458a40 first (`0x004591e8`); pos = frame +0x40; rot = +0x04 * `[0x005cd7a8]` (0.017453292) | `0x0045912b..0x00459226` |
| colour argument `0x005f9838 + type * 4` | LEAs `0x00459090`, `0x004591f0` |
| `DAT_005f9838` 22 dwords, no writer (2 refs, both LEAs); `0x005f9890` (index 22) = 0, no reference | image, decomp_pc --datarefs |
| one flush after the loop | `0x0045927a..0x0045927f` |
| render: `RwRenderStateSet(8, 0)`, `(6, 1)`, `FUN_004770a0(0x0068b968)`, `(8, 1)`, `(6, 1)`, then the cubes | `0x00458b22..0x00458b56`, cubes from `0x00458b5c` |
| frame order: FUN_004891f0 `0x00410c23`, FUN_0045b990 `0x00410c46` (-> FUN_0045a130 `0x0045b991`, FUN_00458b10 `0x0045b996`), FUN_00413a00 `0x00410c53` | FUN_00410b30 |
| texture: FX.TXD entry `PUGlow` (offset 0xbb08 of the extracted FX.TXD) | SFX.piz |

The exe's own image covers the original's .data range (`exe_main.cpp`, B7 wedge note), so `0x005f9838` in the
exe is not the original's table. `PK_GlowColour` turns the pointer back into the index and reads the transcribed
table; a pointer outside it logs once and stages the NULL default (`0x00613290`).

## 3. Arms

Exe `mashed_re.exe` SHA-256 `6d207351a96db37e1c18963f2f42459af465512e29d3f8051d572efcc1104839`, built from
`95aedfa80` with `MASHED_NO_DEPLOY=1` (`.asi` not deployed). Private root `D:/MashedWT/D5-ICONCUBE-GLOW_root`, every
game run under `mashed_lock.py run --label D5-ICONCUBE-GLOW` and `test-slot`. Every arm self-exited 0, so no PID
had to be stopped.

| arm | result | evidence |
|---|---|---|
| G0 build | exit 0, `[rva-lint] 16 finding(s) ... NEW=0` | build log in the data dir |
| A1 default gates | G-CTRL `5943c5f1 / 364350ae / 52052890`, B `3a03670c / 9fcdbd3a / 53778664`: all six equal `merge_train.json` **PASS** | `RUNLOG_gates.txt` |
| A2 default 13 tracks | 39 of 39 score rows identical to `verify/hbfullcheck_20261028/tracks_score.txt`, INSTRUMENT GATES PASS **PASS** | `A2_tracks_score_off.txt`, `RUNLOG_tracks.txt` |
| A3 knob-ON gates | `MASHED_RFX_EMITTERS=1` on all four arms: the same six sha8 **PASS** (the glow writes no sim state) | `RUNLOG_gates.txt` (the knob is in each arm's env line) |
| A4 wiring | 14,218 ticks, 70,608 particles (state 1: 70,488, state 2: 120, of which 94 sizes bit-exact and 26 within the 1 ulp the PREREG allows), 0 count and 0 particle mismatches **PASS** | `A4_check.txt` |
| A5 draw | Training ON: one PUGlow draw per row, k = 4 / 4 / 5 (original 5 / 5 / 4), n = [4k, 2k]. OFF: none **PASS** | `A5_report.txt` |
| A6 pixels | 884: 7,017 px differ, one box x 494..635 y 168..271 (the red type-12 glow). 959: 533 px, one box x 281..316 y 51..79 (the blue type-19 glow). 924: 0 px (report, below) | `A6_report.txt`, `crops/` |

**A4 negative controls** (`negctl_check_glow.py`, added after the clean run, not in the PREREG): M1 colour index + 1
fails 70,608 of 70,608. M2 state-1 position from the PRE frame fails 5. **M3 (state-1 size in double, a
rounding-level mutant) passes**, because no state-1 particle took the shrink path: d2 > 0 on 0 of 70,488. The
original's own pool capture on disk (`verify/d3_mode7_m2_20261010/o_pk1.msd.pool.tsv`, 1,890 ticks, 8,848 live
entries) has 0 displaced entries too. So the size formula `1.25 * clamp(1 - d2, 0, 1)` is not exercised by any run on
either side. It runs in the verbatim asm (`0x0045916c..0x004591cd`), not in this step's code: the shim copies the
size the asm computed.

**A5 across all 13 tracks (report).** Each of the 39 original capture rows has a PUGlow draw. With the knob ON the
port's PUGlow draw state equals the original's on every row (Z 1, ZW 0, ZF 4, AT 1, AREF 0, AF 5, AB 1, SRC 5,
DST 6, CULL 2 in `drawlist_diff.py`'s terms): 1 row matched outright (02_Neustein 1169), 24 rows `MOVED` (same
texture and triangle count, AABB moved by 0.0264 to 0.48, median 0.172, `A5_moved.txt`), 14 rows split because the
port has one particle fewer than the original (13 rows) or one more (Training 959) (`A5_tally.txt`). The pool state
is not matched in this replay (the count follows which entries are collected or respawning at that row), so the
count is reported, not scored. Against OFF the knob-ON table moves 15 rows (one PUGlow match on Neustein 1169, one
more cited row on 14), 0 RED, INSTRUMENT GATES PASS (`A5_table_moves.txt`, `A5_tracks_score_on.txt`).
The fields that differ from the original on the PUGlow draw differ the same way on the already-ported librw FX
draws of the same frame (car_shadow, RWObjShad): LIGHTING 1 vs 0, a vertex shader (vs 1, fvf 338, stride 36) vs
fixed function (vs 0, fvf 322, stride 24), texture format 21 vs 26, tss [4,2,1] vs [4,4,1]. That is the librw im3d
path, not the glow. The original records two PUGlow events per row and the port one. car_shadow and RWObjShad show
the same two-vs-one pattern.

**A6 (report).** Training 884: the red glow (type 12, `0xff241cec`) where the original's frame shows the red glow
behind its IconCube (`crops/training_00884_{on,off,orig}.png`). Training 959: the blue glow (type 19, `0xffff0000`)
at the screen position of the original's blue glow and cube (`crops/training_00959_*.png`). Training 924: 0 px
differ. The port's 4 glows of that row are outside the 640x480 frame, and the entry the original shows far ahead
(AABB z-min -20.55 in the original, -3.37 in the port) is not live in the port's pool at row 924. It is live again
at row 959 (port z-min -20.59). The crops show one look difference this step does not own: no cube is drawn in front
of the glow (D5-ICONCUBE-SPIN), so the port's glow centre is visible where the original's is covered.

## 4. Raw files (not committed)

| file | SHA-256 |
|---|---|
| `D:/MashedWT/D5-ICONCUBE-GLOW_data/arms/H_GLOWD.puglow.tsv` (A4 dump, 23 MB) | `4601ab5136765b8c4a31b4aed8a9e9e786023cc17915381763ee3942cca454c2` |
| `D:/MashedWT/D5-ICONCUBE-GLOW_data/mashed_re_g1.exe` (tested exe) | `6d207351a96db37e1c18963f2f42459af465512e29d3f8051d572efcc1104839` |
| `D:/MashedWT/D5-ICONCUBE-GLOW_data/arms/H_DEF.step.csv` (= `H_GLOW.step.csv`) | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `D:/MashedWT/D5-ICONCUBE-GLOW_data/t13_on/13_Training/stand/d1r2/stand_row_00884.draws.json` | `b2ea5dedcf4e8c5d11295f180f2776eba4fec550d87faab87909447eee865165` |
| `.../t13_on/13_Training/stand/d1r2/stand_row_00884.bmp` | `7487cd3128feef562f4b14b30dbb0f92c779b222277a6bc413a3e88e734fdcab` |
| `.../t13_off/13_Training/stand/d1r2/stand_row_00884.bmp` | `889dd7a8a81284708b67d364cdda651e292c125939613fe4458c7ecca8958187` |
| `.../t13_on/13_Training/stand/d1r2/stand_row_00924.bmp` | `badc4ca6317c3e56bbc1bcc435b6088f791851c8f754b1b040d2ad391146ed36` |
| `.../t13_on/13_Training/stand/d1r2/stand_row_00959.bmp` | `96e4a67d09183ab298e68b77223dd20403d5d396d38634a9adf6dce8cf18c0c9` |
| `.../t13_off/13_Training/stand/d1r2/stand_row_00959.bmp` | `f9f360a57bea007926a90cdf0ab930b4a92e48ba1b500a0ca8896770a150212a` |
| `log/d1r1/orig_training/orig_row_00884.bmp` (main checkout, original capture of 2026-10-16) | `4305d063e89363811c3baaadf1696c709baf25b374a43ad7d9727326c180e702` |
| `log/d1r1/orig_training/orig_row_00959.bmp` | `62d9749ed17662ffdbece6746658d7e82a89086594f2a093e2e98c8c88089f2a` |

The arm folders (`arms/`, `t13_off/`, `t13_on/`) stay in `D:/MashedWT/D5-ICONCUBE-GLOW_data`.

## 5. Pending legs (BLOCKED: needs the audio endpoint, the original does not boot)

- P1: original-side PUGlow draw at a matched pose with a matched pool (a live capture with the pool state
  transplanted), to score the particle count A5 only reports.
- P2: original-side FUN_00459000 sprite-queue call log (the six callees' arguments per tick) against the A4 dump on
  one replayed pool (`o_pk1.msd.pool.tsv`). A4 checks the port against the static formula, not against a live run.
- P3: a run with a displaced live pickup (d2 > 0), on either side, to exercise the shrink path. Neither the port's H
  recipe nor the original's `o_pk1` capture contains one.

## 6. Not claimed, follow-ups for the lead

- No C-level change, no tracker edit. Candidate re-classify transaction: `exe_file` =
  `mashedmod/src/mashed_re/Gameplay/PickupGlow.cpp` on `0x004769a0`, `0x004769d0`, `0x004769f0`, `0x00476a30`,
  `0x00476d00`, `0x00476df0` (the exe callers of the Rfx core, same RVAs as the D5-RFX-EMITTERS rows), and a note on
  D-11076 that the glow half is ported behind `MASHED_RFX_EMITTERS` (the cube half stays with D5-ICONCUBE-SPIN).
- Default flip: turning `MASHED_RFX_EMITTERS` on is a decision row. A3 shows the gates do not move with it, and the
  13-track table moves 15 rows (0 RED). The flip is cleaner after D5-ICONCUBE-SPIN, because the PickupField orbs
  (`@gen:pickup`, additive, D3D9 path) still draw beside the glow (`EXTRA C06-PICKUP @gen:pickup` rows).
- Merge note: D5-ICONCUBE-SPIN is open in parallel and can touch `PickupTick.cpp`, `pickups_m2a.json` and
  `PickupTick_naked.inc` (its re-skin is the `0x00458dd0` binding, the line above this step's six). This step changes
  6 spec lines and 11 .inc call lines. Regenerate the .inc from the merged spec, from the main checkout:
  `gen_naked_asm.py` reads `original/MASHED.exe.unpatched` relative to the CWD and truncates its output on failure.
- No `.ps1` was edited.
