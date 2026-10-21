# RESULT u9779: the round end at call 220 is FUN_00411170's write of 7 after FUN_0040d590 resolves the three AI cars; the port never ran that arm (2026-10-21)

Branch `work/u9779` from `d7afd53e4`. PREREG `PREREG_u9779.md` (`9579e3da6`, amendments 1 `82f669fb1` and 2 `73c723009`), committed
before every knob arm. Final exe `81661906` (`MASHED_NO_DEPLOY=1`, copy `D:\MashedWT\u9779_data\exe_81661906.exe`), private roots
`D:\MashedWT\u9779_root` / `u9779_root13`. Evidence `D:\MashedWT\u9779_data` (`MOVED_OFF_GIT.txt`). No default changed.

## 1. The original's 6 -> 7 writer and its trigger

The only store of 7 to `DAT_0063ba8c` in `MASHED.exe.unpatched` (byte search, `absref.py`: 19 immediate stores + the setter
FUN_0040e360) is `0x004111a6 mov dword ptr [0x63ba8c], 7` in FUN_00411170. It runs when FUN_00410d10 (`call` at 0x0041118f) returns
non-zero and FUN_00410510 (0x00411198) returns 0, then tail-jumps to 0x00418990 (FUN_004194f0).

Witnessed on the original, same recipe as `o_cc3_1`:
- `o_al_1` (`--alive-probe`): FUN_00410d10 runs 220 times. On the 220th call (seq 219) the zoom `DAT_00898980` is exactly 10.0 (the
  0x005cc55c compare of the elimination block), the alive vector goes 1/1/1/1 -> 0/0/0/1 inside the one call, it returns 1.
- `o_ep_1` (new `--elim-probe`: entry hooks on FUN_00422fd0, FUN_0040eee0, FUN_0040d590, FUN_0040b290, the tails 0x00418990 /
  0x00430290): `kill(0)`, `FUN_0040eee0(0,1)`, `FUN_0040b290(0,-2)`, `FUN_0040d590(3,1,2,1)`, `kill(1)`, `(1,-1)`, `kill(2)`, `(2,+1)`,
  `(3,+2)`, then the 0x00418990 tail with `DAT_0063ba8c` = 7. Scores 6,6,6,6 -> 4,5,7,8.

The chain, by RVA:
1. FUN_00410d10's elimination block: zoom == 10.0, FUN_0040e180 pair, the straggler (player, pct 8.16) -> FUN_00422fd0(0), which zeroes
   the alive word through FUN_0046c5c0 (`0x008815a4 + v*0xd04`).
2. FUN_0040eee0(0, 1) counts 3 remaining. Arm `DAT_008a94d0 == 4`, `FUN_0042f500() == 0`, 3 remaining: FUN_0040b290(0, -2), then
   `FUN_0042f6a0() in {3,4,5,10} && FUN_0042f500() == 0 && DAT_007f0fd0 == 0` walks the alive slots with `FUN_0040e470(i) == 2` and
   on the third calls FUN_0040d590(third, first, second, 1). Gate inputs measured: `DAT_0067e9fc` 10, `DAT_007f0fd0` 0,
   `DAT_008a94d0` 4, `DAT_0067ea64` 0, slot types 1,2,2,2.
3. FUN_0040d590 ranks the three by the wrap-adjusted FUN_00408ad0 race_pct (`0x008a96ec + v*0x30c`) and kills the two behind.
4. FUN_00410d10's alive-count tail: 1 alive -> return 1. FUN_00410510 -> 0 (no score > 11). 0x004111a6 writes 7.

Frame order: FUN_004111c0's sub-state-6 case calls FUN_00411170 at 0x004112cd and FUN_0040fc00 at 0x004112d5. So the round end is
decided on every car's position after the previous frame's physics, and the same frame's AI tick (FUN_00425a40 at 0x0040fc6c) reads
sub-state 7. FUN_0040fc00 then skips the clock add (0x0040fe46 cmp [0x0063ba8c],7 / 0x0040fe4d). Sub-state 7 goes to 0x004112dd
(FUN_00410860, then FUN_0040fc00).

## 2. What the port does

New diagnostic `MASHED_ELIM_TRACE` (default OFF, step dumps unchanged).
- The port kills the player at the same point: zoom 10.0 on the same positions. `TrackRenderer::ScoreOnElimination` awards -2 and
  stops. Its own comment said the FUN_0040e470 type-2 arms were not ported. Cars 1 / 2 / 3 race on (next kills at k 345 / 755 / 935).
- The AI host's FUN_0040e350 (`aib_game_sub_mode`) returns 6 always (knob `MASHED_SUBSTATE` off).
- `UpdateRace` runs after the AI tick and the player's physics and before the opponent loop commits. Its camera input is a mixed-time
  snapshot: trace row k holds the player at its record-dump frame k (`B_ILC0_K0.rec.msd`, 195/195) and cars 1 / 3 at k-1 (195/195).
  That is the 0.03 to 0.045 required-zoom deficit on every pair with the player from k~85.
- Gate inputs: live `DAT_007f0fd0` 0 (= original), slot table 1,2,2,2 (= original), live `DAT_0067e9fc` 0 (original 10; nothing in the
  exe writes it), the AI host's FUN_0042f6a0 stand-in `aib_round_type()` 3, the rule engine's `rule_` 4 (from the recipe's
  `MASHED_ROUND_RULE=4`; the original runs `DAT_007f0fd0` 0). The camera's progress inputs are the stand-in `race_[].progress`; the live
  race_pct 0x008a96ec (PATHPROG) is bit-equal to the original's at k 0..219.

## 3. Knobs (all default OFF)

| knob | what |
|---|---|
| `MASHED_ELIM_RESOLVE` | FUN_0040eee0's type-2 arms in `ScoreOnElimination` (3 remaining -> FUN_0040d590, 2 remaining -> pairwise kill, +2 / +1), gated on `aib_round_type()` in {3,4,5,10}, no team play, live `DAT_007f0fd0 == 0`; live race_pct, live slot table |
| `MASHED_ROUND_END7` | after a rule-engine round end (r == 0) until StartRound: FUN_0040e350 = 7 for the AI host, the frame-end clock add skipped, FUN_00424eb0's first loop (FUN_0046c570: record +0x9b0..+0x9b8 *= 0.85f, `_DAT_005ce264` = 0x3f59999a) |
| `MASHED_RACE_TAIL_LATE` | `UpdateRace` at the end of UpdateCar (after the opponent commit, the late path-progress tick, the pickup / registry ticks) |

FUN_0040d590 now has one body for both targets: `Frontend::RaceRankThree` (MenuLeaves_af4.cpp) takes its callees. The .asi export
passes the original's, the exe passes live race_pct / KillCar / ScoreAward. Its wrap bounds are literals now (80 / 20 / 100 are
exact; the exe reads `.rdata` 0x005cc730 / 0x005ccd6c / 0x005cc568 as 0, the RwMatrixRotate.cpp:72 class of defect). The .asi
install stays commented out. rva-lint clean.

## 4. Results on K0 (interleave on unless noted)

- ER: the port's resolve logs `remaining=3 ... alive=0,0,0,1 scores=4,5,7,8 pct=8.164267,16.720179,18.977242,20.087664`. That is the
  original's pct, bit-equal, and its scores. P1 holds.
- ER + E7: car 3 sees sub-state 7 one frame late (row 221). Cause: the frame order in section 1. P2 failed, which led to amendment 1.
- TL alone: the required zoom now tracks `o_al_1` at the same k within 1.03e-3 (was 0.03 to 0.045 low). It reaches 10.0 on the same
  row 219, and the live pct is sampled on the same k. P6 holds. The residual ~1e-4 to 1e-3 is another camera term, not pursued.
- ER + E7 + TL (R7): car 3's row 220 reads sub-state 7, c4 0, c5 0, and the clock stays constant, as in `o_cc3_1`. Cars 1 / 2 have
  no row past 219, as in the original. P7 holds.
- Records (`reck.py`): car 3 still differs from k=220. In the original its position freezes (6.0739, -22.0905) while the velocity decays
  (2009.32 -> 1676.83 -> 1396.51). With R7 the port damps (1632.52 at 220) but keeps integrating the position. The writers come from
  `--watch-write 958,9b0` (`o_ww_2`; the watch perturbs the regime, so it is used for writers only). After the round end, 0x0046ea62
  rewrites the same x, one substep's writers appear per frame (two while racing), and FUN_0046c570 at pc 0x0046c591 scales the
  velocity. FUN_00470c70 also hands A4 (0x00471076) the block 0x007f19b8 when FUN_0040e350() is not 6 / 0xb / 0xa
  (0x00470fdb..0x00470ff9). Not ported. Filed as **U-9781**. P8 as predicted.

## 5. Gates

| gate | knobs unset (exe `81661906`) | R7 | IL + R7 |
|---|---|---|---|
| G-CTRL | `15594ec8 / 7285d708 / 26a6e4ad` (= default) | `9081938f / 8418a4df / a91208bf` | `0b7be44b / c03963c0 / f356401d` |
| B dumps K0 / K1 / K26 | `7ab70c5f / 15262e4a / d1b4bc64` (= default); IL `9bd86ffe / 433abcd2 / 1a1ff727` (= u9778) | `400a045d / d978840c / 9ce021cc` | `803ab845 / 894dd04c / 2136aa4b` |
| `flag_a368` per K | 0 / 0,1 / 0,2 | 0 / 0,1 / 0,2 | 0 / 0,1 / 0,2 |
| (b) | A 3/3, B 2/3 (car 2), C 3/3; IL 9/9 | A 3/3, B 2/3 (car 2, same items), C 3/3 | 9/9 |
| D2 x3 alone | (u9778 default band) | 0.1900 x3 / 0.2550, 0.2550, 0.2549 / 1964.25, 1965.67, 1966.66, H1 398/400: PASS 3/3 | not run |
| 13 tracks | (default 873 rows) | 39 frames, 873 rows, all cited, **0 RED**, instrument gates PASS | not run |

(b) is inert by construction here. Its window is the first 220 calls per car for regimes A / B and 196 for C
(`re/tools/b_envelopes.json` `windows`). It ends where the round ends, so no PASS / FAIL can move and none did.

G-MECH (K0, IL, one knob at a time): ER `a369e1b1`, E7 `f6703741`, TL `8906520a`, ER + E7 `74988967`, all three `803ab845`.

Per-knob attribution of the row-220 match (car 3 vs `o_cc3_1`, K0, IL; `stepcmp.py`):

| stack | step sha8 | row 220 (orig: sub 7, c4 0, c5 0) | row 221 clock |
|---|---|---|---|
| ER | `a369e1b1` | sub 6, c4 255 (round ends in the port, AI not told) | runs (11050) |
| E7 | `f6703741` | sub 6, c4 255 (no round end at 220) | runs |
| TL | `8906520a` | sub 6, c4 255 | runs |
| ER + E7 | `74988967` | sub 6, c4 255; sub 7 from row 221 (one frame late) | stops (11000) |
| ER + TL | `5d3ac4f2` | sub 6, c4 255 | runs |
| E7 + TL | `d11b532e` | sub 6, c4 255 | runs |
| ER + E7 + TL | `803ab845` | **sub 7, c4 0, c5 0** (= original) | stops (11000, = original) |

All three are needed. ER makes the round end at 220, E7 makes the AI see it and stops the clock, TL puts the decision before the
AI tick.

Scope note (orchestrator, 2026-10-21): the sub-state-7 hold physics is outside the (b) window. The writer hunt stopped after
`o_ww_2`, and U-9781 records it. The one piece of sub-state-7 port work done before that note, FUN_00424eb0's FUN_0046c570 0.85
damp, is inside `MASHED_ROUND_END7` (amendment 2). It does not change row 220 (a command row) and acts only from k=220 on records.

## 6. Decisions for Mariano

1. **Recipe.** The B arms run `MASHED_ROUND_RULE=4`, and the original B captures run `DAT_007f0fd0` 0 (rule 0) with game mode
   `DAT_0067e9fc` 10. The port's live `DAT_007f0fd0` is 0, but its rule engine runs rule 4, and the exe never writes `0x0067e9fc`. The AI
   host reports FUN_0042f6a0 as 3. The knob reads the live `DAT_007f0fd0` and `aib_round_type()`, so it fires on the B recipe. A
   faithful recipe would set the rule engine to 0 and the game mode to 10. Whether to change the B / H recipes is your call (every
   hash moves).
2. **Flip.** The three knobs pass every gate measured here with no regression: D2 3/3, 13 tracks 0 RED, (b) unchanged, G-CTRL knob-off
   unchanged. They only become visible past k=220, which no scored gate reads. Flipping them is your call. Without U-9781, records
   still differ from k=220.

## 7. Next

- U-9781: sub-state-7 physics (position freeze, one substep, the A4 block 0x007f19b8).
- U-9780 (car 2, 1 ulp at 218) is unchanged.

Scripts: `absref.py` (absolute-address refs), `cslist.py`, `stepcmp.py` (aistep vs step dump on k), `reck.py` (records on k),
`run_arms.py` (stacks `ET`, `ER`, `E7`, `TL`, `R7m`, `R7`, `IL*`, `*C0..C3`), `apply_trackers.py`.
