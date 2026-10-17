# PREREG dflip: three user-decided default flips (2026-10-17)

Branch `work/dflip` from `race/first-frame-parity` `964c31f04`. Model claude-opus-5-5, account3. Written and committed
before any run of this lane. Builds `MASHED_NO_DEPLOY=1`; every run on a private game root on D:
(`D:\MashedWT\dflip_root`, `D:\MashedWT\dflip_root13`, copies of `original\` with `original\` inside); only PIDs this lane
spawns are stopped.

## 1. What changes (user decisions 2026-10-17)

1. `MASHED_HEADING_ASSIST` (FUN_0046c7d0, AI slots 1..3, `Race/HeadingAssist.cpp`, d3b) default ON, mask 0x1.
   Opt-out `MASHED_NO_HEADING_ASSIST=1`; `MASHED_HEADING_ASSIST=<mask>` still overrides the mask.
2. Instrument only: the draw-list I-NAME gate (`verify/d1r1_20261016/check_gates.py`, used by
   `verify/rsky_20261016/run13.py score`) counts a texture name `#WxH:<D3DFORMAT>` as named on a row when the same name is
   drawn on the other side of that row (original vs standalone). The car-shadow render textures (`#256x256:21`,
   `#64x64:21`, rres) are drawn under that name on both sides. A `#...` name on one side only stays unnamed. No change to
   the shadow lane or the exe.
3. `MASHED_PONTOON_CARRY` (Warzone object `0x00684d18`, `Track/WarzonePontoon.cpp`) default ON, which implies STATE and
   ANIM (the code's existing chain: CARRY -> STATE -> ANIM). Opt-outs `MASHED_NO_PONTOON_CARRY=1` (STATE + ANIM stay),
   `MASHED_NO_PONTOON_STATE=1` (ANIM stays), `MASHED_NO_PONTOON_ANIM=1` (all off = the 964c31f04 default). The exit
   approximation (U-9738) is accepted by the user.

## 2. Arms (`run_arms.py`, `run13.py`)

- G-CTRL: `H_DEF`, `H_DEF-2` (repeat), `H_OPT` (all four opt-outs), `H_NOHA` (assist opt-out only).
- D2: `D2_DEF_1..3`, each alone (wall-clock bound), `score_all.py d2` (score_d2.py bounds `d81a8df6`, H1 via a8_launch.py).
- D3 (b): `B_DEF_K{0,1,26}` + the extra ring phases `K{2,4,27,54,57}`; `B_L5_K{0,1,26}`; scorer `percar.py`
  (`ai_ctrl_window.check_regime` on `re/tools/b_envelopes.json`, the per-car envelope). Collateral `coll.py` on `H_DEF`
  (+`-2`) and `H_L5` (+`-2`).
- 13 tracks x 3 frames: `run13.py run` on the new default, `run13.py score --cite verify/rres_20261016/cites_rres.json`.
- Warzone: `W_DEF`, `W_DEF-2` (determinism), `W_OPT` (control), `W_NOHA` (pontoon alone, to separate the assist's effect on
  the AI reader), `WL_DEF` (18000 frames, the full-race no-crash arm).

## 3. Expectations (pre-registered)

| gate | expectation | basis |
|---|---|---|
| G-CTRL new default | changes; predicted = d3b's `H_HA` trio `4c81db89 / aa03f7ae / 7d3af534` (the pontoon is Warzone-only, so TRAINING sees only the assist); `H_DEF` = `H_DEF-2` | RESULT_d3b.md section 4 |
| G-CTRL opt-out | `H_OPT` = `d3b90ee0 / 80f01f35 / 56d62ae7`; `H_NOHA` = the same | d3b G-NOOP, pontoon G-CTRL all knobs |
| 13 tracks | 867 rows, 0 RED, 39 frames. Draw files: 36 tracks' frames byte-identical to the base except Warzone (the boat is three per-atomic draws under ANIM): Warzone 24 / 21 / 27 = 72 rows, matched 326 / 326 / 296, as pontoon's N4 CARRY arm. The assist does not reach the matched-pose replay (car poses replayed), so no other track's rows move. Any row change is cited | RESULT_pontoon.md rebased table |
| I-NAME | PASS on every frame; Neustein row 1169 standalone 59/59 and Highway row 928 77/77 (the four shadow quads now named on both sides); original Neustein 1169 passes too. INSTRUMENT GATES: PASS overall | U-9736 |
| D2 | 3/3 in bounds: slip 1500-2000 0.1890, slip 2000-2600 ~0.2482, driving-median 1912..1915, H1 >= 200/400 and median >= 900 (expected 398/400, 1362.7). The D2 recipe runs solo, so the AI-slot assist does not reach it (d3b `D2_HA_1` was byte-identical to d2close's `D2_DEF_2`) | RESULT_d3b.md section 4 |
| D3 (b) default | A 1/3, B 2/3, C 0/3 (= d3b HA); no car that passes on the pre-dflip default (A car 2, B cars 1 and 3) turns to FAIL | RESULT_d3b.md section 4 |
| D3 (b) L5 on the new default | A 1/3 with car 3 passing and car 2 failing `c0#` (d3b HA5), B 2/3, C 0/3: a default-passing car (A car 2) turns to FAIL, so the rule says keep `MASHED_AI_LUT` OFF | RESULT_d3b.md section 4 |
| collateral | `H_DEF` 42 of 94 (d3b H_HA); `H_L5` 39 (d3b H_HA5) | RESULT_d3b.md section 4 |
| Warzone | `W_DEF` = `W_DEF-2` (deterministic); the object cycles (state 0 -> 1 -> 2 ...) and every AI reader `0x00414f00` ret-1 line carries the word set; `W_NOHA` reproduces pontoon's CARRY arm (gate1 1380, ret1 885, enter at 1839); `W_DEF` counts may differ from that because the assist moves the AI cars (reported, not gated); `W_OPT` = pontoon's default (no pontoon trace, gate1 0, ret1 0); `WL_DEF` exits by itself after 18000 frames, no crash | RESULT_pontoon.md W table |

A concurrent piz-load crash (`MASHED.exe+0x1509b4` / `+0xe66db`) is a re-run, never a finding (only the original side can
show it; this lane runs the exe only).

## 4. Tracker transaction (after the runs, through re-classify)

U-9191 cause found (resolved, the assist now default ON); new U-9743 = the player (slot 0) form of the assist (structural,
D2 input); hooks.csv notes for `0046c7d0`, `004093b0`, `00443dc0`, `00443300` (no C-level change); U-9736 resolved by the
gate rule above; U-9738 note "exit approximation accepted by the user 2026-10-17, stays open"; DEFERRED rows for U-9730 and
U-9741 with re-pickup conditions; CHANGELOG line.
