# RESULT PORT-TOP-05 (D4, run 39, 2026-11-12) - GREEN

Row: L-96 top-down, message pump 0x00499690 and the frame subtree of the run function 0x00492290 (0x00492d30,
0x004929d0 -> 0x0040d440) under the knob; knob path menu -> Quick Battle -> race start. Row kind: top-down.
Branch orch/PORT-TOP-05 from orch/integration 0e277caaf.

## 1. What runs on the MASHED_ORIG_TOP path now
The run function 0x00492290 (matched since PORT-TOP-02) calls, per frame: 0x004929d0, 0x00492d20, 0x00492d30, 0x00492e90.

| RVA | Before | Now |
|---|---|---|
| 0x00499690 message pump | matched (WindowMsgPump, Match/MatchS16_00499690.cpp) | unchanged: already the matched body, both targets |
| 0x004929d0 game-mode machine | hand GameModeMachine_Tick (no-op by default) | matched Orig_004929d0, re/match/s15/004929d0.c (100%), Match/PortTop_004929d0.cpp |
| 0x0040d440 (case 2) | no exe body | matched Orig_0040d440, re/match/s02/0040d440.c, Match/PortTop_0040d440.cpp |
| 0x0040d270 (under 0x0040d440) | - | stand-in PortTop_StandIn05_0040d270, logs its args: the hand path loads the course at the menu launch (exe_main MenuLaunchRace) |
| 0x00492d30 frame machine | hand HandTop_FrameUpdate | matched Orig_00492d30, re/match/s15/00492d30.c (100%), Match/PortTop_00492d30.cpp |
| 0x004030d0 (case 1, 0x0042b930 == 0x21) | no exe body | matched Orig_004030d0, re/match/s01/004030d0.c, Match/PortTop_004030d0.cpp |
| 0x0043d7c0 (cases 1/4/5/6/7) | no exe body | matched Orig_0043d7c0, re/match/s08/0043d7c0.c (100%), Match/PortTop_0043d7c0.cpp |
| 0x00496920 (tail) | .asi only | matched TimerTable772ffcGet, Match/MatchS16_00496920.cpp added to the exe list (one body, both targets) |
| 0x00498860 (tail, screenshot) | no exe body | matched Orig_00498860 + static 0x00498810, re/match/s16/00498810.c, Match/PortTop_00498860.cpp |
| 0x0043dfd0 frontend machine | - | stand-in PortTop_StandIn05_0043dfd0 = the hand menu navigation + input globals. The provisional body (ProvS08_0043dfd0.cpp, 63 counting stubs) is not wired |
| 0x004111c0 race tick (cases 3/6) | - | counting stand-in. The knob path's race is the hand session (GameFlow_Update), run by the wrapper after 0x00492d30 |

The four callees the PROVISIONAL-BODIES follow-up named (0x004030d0, 0x0043d7c0, 0x00496920, 0x00498860) now have exe
bodies, all from matched sources. Level-2 callees with no exe body are named stand-ins in Match/PortTop_StandIns05.cpp
(each logs its first call): 0x0043df00 (under 0x004030d0), 0x0040ab40 / 0x0040ac80 / 0x0040de10 / 0x0040e360 / 0x0043d2a0
(under 0x0043d7c0, reached only after a modal box reaches state 3), 0x004ce8e0 (image write under 0x00498810). Callees
with exe bodies bind by name: Set86ecc8 0x00472640, Scan42c150 0x0042c150, Set77196c_1 0x00493570, Set771970_1 0x00493580
(the four .asi-only single-function TUs were added to exe_sources.rsp), plus the matched exe bodies listed in each TU header.
0x00498860's CRT callees (_ftime64 0x004a43d2, _ctime64 0x004a43b9, sprintf 0x004a2b60) are adapters to the VS2022 CRT. Its
format string s_fmt_005d0120 is added to the .rdata seed (gen_rdata_seed.py, 1 row). DAT_005d0118 (the extension string) is
not seeded: the generator only takes `s_` names. 0x00498860 runs only when 0x00496920(5) != 0, which no run reached.

Frame split (exe_main.cpp): HandTop_FrameUpdate is now four pieces with the same default-path order: Harness (dev harness
steps, window title), Menu (UpdateMenuSelection), InputGlobals (WriteMashedInputGlobalsFromKeyboard, CookCtrlByteA),
GameFlow (GameFlow_Update). On the knob path PortTop_Bound_00492d30 runs the deterministic frame cap and Harness, then the
matched 0x00492d30 (whose 0x0043dfd0 call runs Menu + InputGlobals), then InputGlobals if 0x0043dfd0 did not run this
frame, then GameFlow. Diagnostic switch `MASHED_TOP05_HANDFRAME=1` restores the old stand-ins for both 0x004929d0 and
0x00492d30.

Phase word DAT_00771968: MainLoopInit 0x00492770 writes 1. The hand frontend never writes DAT_0067ecb0 = 1 (the original
frontend's launch signal, 0x0043dfd0 at 0x0043f371 region of re/match/s08/0043dfd0.c), so by default the matched machine
stays in phase 1 for the whole run, including the hand race. 0x0042b930 returns 0x21 (DAT_0067ecb0 = 33 after init), so
case 1 runs 0x004030d0 every frame. With `MASHED_PORTSTATE_GAMEFLOW=1` the hand launch writes phase 2, and the matched
machine runs case 2 (0x0040d440) and goes to 3.

## 2. Builds
- Default build (VS2022, MASHED_NO_DEPLOY=1): OK, rva-lint clean after 3 ORIGTOP-ONLY allowlist lines (0x004929d0
  DUP-IN-TARGET with Race/GameModeMachine.cpp's 95.7% gffu copy; 0x0040d440 and 0x00492d30 CROSS-TARGET with their .asi
  copies). Log D:/MashedWT/PORT-TOP-05_build1.log.
- Object build (MASHED_DDKOBJ=1, MASHED_NO_DEPLOY=1): OK. Log D:/MashedWT/PORT-TOP-05_objbuild.log.
- `ddkobj.py check`: IDENTICAL 724/806, DIFFERENT 79, not-in-map 3, refusals 48 (static-register-convention 39,
  wrapper-redefines-globals 9). Baseline (PROVISIONAL-BODIES): 714/796, 79, 47. The 10 new units are IDENTICAL:
  0x00492d30, 0x004030d0, 0x0043d7c0, 0x004929d0 (s15), 0x0040d440, 0x00472640, 0x0042c150, 0x00493570, 0x00493580,
  0x00496920. The one new refusal is re/match/s16/00498810.c (static-register-convention: its static 0x00498810 takes the
  Camera in EAX); it keeps its VS2022 body, nothing worked around. 0x004929d0's gffu copy (GameModeMachine.cpp, 95.7%)
  was DIFFERENT before this row and still is.
- re/match/s01/004030d0.c converted to the MGLOBAL form (verify/porttop_20261108/mglobal_convert.py, 5 globals) so C++
  can compile it: match.py 100.0%, 0 differing bytes, BYTE-IDENTICAL; relocheck OK (checked=16).

## 3. Knob boot and self-test
Private root D:/MashedWT/PORT-TOP-05_root. Every launch ran inside one `mashed_lock.py run` hold under the test slot
(script verify/porttop05_20261112/run_checks.sh).

| Run | Env | Result |
|---|---|---|
| boot_mat | MASHED_ORIG_TOP=1 MASHED_PROV_43DFD0_SELFTEST=1 MASHED_DET_FRAMES=600 | rc=0, WinMain returned 0 after 600 frames. `frame subtree: matched 0x004929d0 and 0x00492d30`, phase 1 throughout, 0x0043dfd0 stand-in 481 calls by frame 480, 0x004111c0 0 calls. **Self-test: `[prov-43dfd0] self-test returned code=00000000 stub calls=5 distinct stubs=5 of 63`** (same as PROVISIONAL-BODIES) |
| boot_hand | same + MASHED_TOP05_HANDFRAME=1 | rc=0, 600 frames, self-test returned code=00000000 |

## 4. Knob path menu -> Quick Battle -> race start
Route of PORT-STATE-02 P7: `MASHED_DBG_MENU=1 MASHED_NAV_KEYS=e,d,e,e,e,e,x*1200 MASHED_NAV_KEYS_GAP=60` (main menu,
screens 2/4/6/24 = Quick Battle, race), all with MASHED_ORIG_TOP=1.

| Run | rc | NAV_KEYS lines | sha8 | flow (frontend / loading / in race) |
|---|---|---|---|---|
| qb_hand1 (HANDFRAME=1) | 0 | 1687 | 479d0282 | 366 / 30 / 1291 |
| qb_mat1 | 0 | 1687 | 479d0282 | 366 / 30 / 1291 |
| qb_hand2 (HANDFRAME=1) | 0 | 1687 | 479d0282 | 366 / 30 / 1291 |
| qb_mat2 | 0 | 1687 | 479d0282 | 366 / 30 / 1291 |
| qb_mat_gf (+ MASHED_PORTSTATE_GAMEFLOW=1) | 0 | 1687 | 479d0282 | 366 / 30 / 1291 |

The matched arms reach the race (flow 2) and quit, WinMain returned 0 after 1688 frames. qb_mat_gf witness (mashed_re.log):
frame 366 `DAT_00771968 1 -> 2` (hand launch), then 0x004929d0 case 2 ran the matched 0x0040d440, `0x0040d270: stand-in ...
args table=005F2728 row=3`, DAT_0063ba78 = DAT_0063ba7c = 3, `2 -> 3`. On frame 367 the matched case 3 moved it `3 -> 7`
because 0x0042c1c0 (returns DAT_0067eab0) is nonzero during the hand race. The hand GameFlow wrote 3 at frame 396 (load gate
done) and the machine took 7 again. In phase 7, 0x00492d30 calls only 0x0043d7c0, so the wrapper wrote the input globals,
and the trace stayed identical. [UNCERTAIN] which hand-path writer leaves DAT_0067eab0 nonzero in the race: not traced in
this row. It matters once the phase word is driven by the original frontend (PORT-TOP-08).

## 5. Menu walk (12 nav-demo captures, MASHED_DET_MPG_PIN=0)
hand / matched / hand / matched, all MASHED_ORIG_TOP=1: rc=0, `pinned pos=55 hash=28538c74`, class E, n_eq_E 12/12 in
all four (runs.tsv). Class P1 holds (captures and logs unchanged matched against hand), logged in re/orchestrator/DECISIONS.md
as "auto, class P1".

## 6. Gates
Not run: top-down row (G-CTRL / B / 12 captures x 4 are computed with the knob off and cannot see this change). Default
path change: HandTop_FrameUpdate split into four static functions called in the same order. The merge train runs the
default gates.

## 7. Not C4, trackers
No promotion. Every new body is a byte-matched source (already C4 by match where hooks.csv says so). No hooks.csv edit.
verify/porttop_20261108/BINDINGS.tsv: 9 rows updated (0x004929d0, 0x00492d30, 0x0040d440, 0x004030d0, 0x004111c0,
0x0043d7c0, 0x0043dfd0, 0x00496920, 0x00498860).

## 8. Next
- PORT-TOP-08 (frontend 0x0042c960 / 0x0043dfd0) is unblocked: 0x00492d30 is off its stand-in. The 0x0043dfd0 call site is
  PortTop_StandIn05_0043dfd0 in Match/PortTop_StandIns05.cpp. Once the original frontend writes DAT_0067ecb0 = 1 at the launch,
  the matched 0x004929d0 leaves phase 1 by itself, and the PORTSTATE_GAMEFLOW mirror is no longer needed.
- 0x004111c0 stays a counting stand-in until the race path moves to the original's tick (RO rows).

## Raw data (D:/MashedWT/PORT-TOP-05_data)
```
833821defa0cf8f08c4d8d7c35f8512aaf5de75fc0ac6ba7ad80ccdb58d58bb4  boot_mat.log
623d7d23e4f108d6a68c116deef24c4f7fc3539f50ec8cc69d8bc3ed0cdcbff8  boot_hand.log
56ac1cc7db9fbf75c3babae6cfaddb2bb77a3f37015a44f17b274b377339b2f8  qb_hand1.log
8a488e70776d302707e34e407c9c43195097228d5acabe98e106277c22a2791e  qb_hand2.log
ca1d00b963d79f9f652a1b6b1212e89d84b02addacaa6e1bae651d79b6f7e7e7  qb_mat1.log
f2073add103bfaaf2dec1d69d83167fc8b227e5397705b089ff3506d3c7868e4  qb_mat2.log
47758b84633d2cb34626cf1ae2b35e893311afc168631c8b12c610ec5bf68485  qb_mat_gf.log
c71165d6157e3260a23cf81f7e17b80cff58ae975583ece5842eb0b1195bd36b  runs.tsv
6470894f5a376c2891a8f3f8a242f50ba3cf2935648d81aad6556575cffaf521  BYTECHECK.tsv
2bb1611705c717f08474bd8652b725c7bde16085ee052baa1fb568677c1cab74  ddkobj_summary.json
```
Walk captures: D:/MashedWT/PORT-TOP-05_data/walk_{hand,mat}{1,2}/.
