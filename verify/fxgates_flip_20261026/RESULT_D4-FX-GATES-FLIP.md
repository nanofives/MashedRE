# RESULT D4-FX-GATES-FLIP

Verdict: GREEN. Decision L-25 = A (Mariano, 2026-10-26) applied: the original's skid and smoke gates are now the default.

## Change
`mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp` (EmitCarFx, the `s_origGates` static): the knob is now
`getenv("MASHED_NO_FX_ORIG_GATES") == nullptr`. Default ON, `MASHED_NO_FX_ORIG_GATES=1` opts out (same shape as
`MASHED_NO_HEADING_ASSIST`). The old opt-in name `MASHED_FX_ORIG_GATES` is no longer read. No other behaviour change.
`verify/fxgates_20261026/fx_arms.py` still sets the old name, so its F1 arm is now the default and F0 needs the new opt-out. Left untouched.

## Gates (exe mashed_re.exe sha8 2dfd62f4, SHA-256 2DFD62F4334CD36654B938B01A6FAC49F98B386E1A3F9A661FCDD12907F34FC1)
| Gate | Expected (merge_train.json) | Got |
|---|---|---|
| G-CTRL step / gates / ptrace | 5943c5f1 / b493262b / 09551e6a | 5943c5f1 / b493262b / 09551e6a |
| B K0 / K1 / K26 | 3a03670c / 2516d903 / 53778664 | 3a03670c / 2516d903 / 53778664 |
| 13 tracks (cites_rres.json) | verify/rpct_20261022/tracks_flip_score.txt | 43 lines, line-for-line identical, 0 RED, INSTRUMENT GATES: PASS |

Runner: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` and `verify/rsky_20261016/run13.py run|score`,
both under `mashed_lock.py run`, private roots `D:\MashedWT\D4-FX-GATES-FLIP_root` and `_root13`. Each arm ran once, no piz-collision re-runs, no retries.
The lock reported one solo window held by merge-train13 before mine; I waited and ran after it released.

## Raw data (not committed)
| Path | SHA-256 |
|---|---|
| `D:/MashedWT/D4-FX-GATES-FLIP_data/arms/H_DEF.step.csv` | 5943C5F1C3E2F3F2510139F7AF666FFA87941F6DFA66201B15277430B92BE415 |
| `D:/MashedWT/D4-FX-GATES-FLIP_data/arms/H_DEF.gates.csv` | B493262BED8EA6ECAD4399F0EBAA53B902965423E74BFA07359188559FFF479F |
| `D:/MashedWT/D4-FX-GATES-FLIP_data/arms/H_DEF.ptrace.log` | 09551E6A5B1B456B3421A0E1AAC768CED7851C072C99A9341A22CA966CDFE3E1 |
| `D:/MashedWT/D4-FX-GATES-FLIP_data/arms/B_DEF_K0.step.csv` | 3A03670C3991B702AF9FD44B1D49DBDB4F91D6BE00876723CB1BA7891895B83F |
| `D:/MashedWT/D4-FX-GATES-FLIP_data/arms/B_DEF_K1.step.csv` | 2516D9034A1774893BF2F59EDD368C3783AD6CFE0ED491DC68B3D84A0AFEC7F6 |
| `D:/MashedWT/D4-FX-GATES-FLIP_data/arms/B_DEF_K26.step.csv` | 5377866478B33A9827AE5C343B8861189F8F0A838785BF2B83DA5F6118931E2D |
| `D:/MashedWT/D4-FX-GATES-FLIP_data/tracks_flip_score.txt` | 2A44A74801EA6911382FA7CF6F5BE11F0BB6075FD88052D139D27D80D76ED0F6 |

Logs: `D:/MashedWT/D4-FX-GATES-FLIP_data/run_arms.log`, `run13.log`. 13-track captures: `D:/MashedWT/D4-FX-GATES-FLIP_data/t13`.

## Not covered
Gates are sim and draw-list gates. They do not measure how often the skid smoke fires. Why the skid-flag inputs (rec+0xb0c, ctrl brake byte) rarely fire is D4-FX-INPUTS.
