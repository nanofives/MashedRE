# RESULT D4-WSF-SAVE-FLIP (decision L-28 (1), answered by Mariano: flip ON)

Verdict: GREEN.

Change: `MASHED_SAVE_KEEP_IMAGE` default ON. `KeepImageMode()` in `mashedmod/src/mashed_re/Race/GameFlow.cpp` now reads
`s_on = (v && v[0] == '0') ? 0 : 1`, so unset or any value but a leading `0` keeps the loaded image, and `=0` opts out.
The comment above it states the new default. `verify/wsfsave_20261026/run_wsf.py` now sets `=0` for the S1 (opt-out) arm and leaves it unset for S2.
No other file in the tree states the old default (grep of `KEEP_IMAGE`: only GameFlow.cpp and the wsfsave verify files, the latter are history).

Exe: `mashedmod/build/mashed_re.exe`, sha8 280e5d44, built from this worktree with MASHED_NO_DEPLOY=1.

## Gates (run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26, under mashed_lock)
| Gate | Got | merge_train.json ref |
|---|---|---|
| G-CTRL step / gates / ptrace | 5943c5f1 / b493262b / 09551e6a | 5943c5f1 / b493262b / 09551e6a |
| B K0 / K1 / K26 | 3a03670c / e85ea6fb / 53778664 | 3a03670c / e85ea6fb / 53778664 |

Unchanged on all six.

## Save write-back (run_wsf.py S1 S2, same inputs as D4-WSF-SAVE-RT)
| Arm | Input | Output sha8 | Reading |
|---|---|---|---|
| S2_P (new default) | arm_P 697b7af8 | 697b7af8 | exact, 0 bytes differ |
| S2_G (new default) | ghost file e2eabeac | e2eabeac | exact, 0 bytes differ |
| S2_0 (new default) | none | c33e740d | same as S1_0 |
| S1_P (=0 opt-out) | arm_P 697b7af8 | 54f3f898 | rebuilt, differs from input (old default) |
| S1_G (=0 opt-out) | ghost e2eabeac | 54f3f898 | rebuilt, same bytes as S1_P |
| S1_0 (=0 opt-out) | none | c33e740d | first-run write unchanged |

The opt-out `=0` still works and gives the old rebuild output. First-run (no input file) output is identical in both modes.

## Raw files (D:/MashedWT/D4-WSF-SAVE-FLIP_data, not committed)
SHA-256 (full):
- arms/H_DEF.step.csv 5943C5F1C3E2F3F2510139F7AF666FFA87941F6DFA66201B15277430B92BE415
- arms/H_DEF.gates.csv B493262BED8EA6ECAD4399F0EBAA53B902965423E74BFA07359188559FFF479F
- arms/H_DEF.ptrace.log 09551E6A5B1B456B3421A0E1AAC768CED7851C072C99A9341A22CA966CDFE3E1
- wsf/S2_P.bin 697B7AF8175B418F24FCD9DA81851B8FA42B4D236EC8FB6B90D110D48E53F078
- wsf/S2_G.bin E2EABEACDA9820BAC9B2937BBC055B6ABBD72F88B32EEFECABD966BE7AB289E9
- wsf/S1_P.bin and wsf/S1_G.bin 54F3F898F8B300F12FF37E3D7319E270B801BB6E3986C4A703C865706BB0DF74
- wsf/S1_0.bin and wsf/S2_0.bin C33E740DB03C5D95DB19F9593C09C6A5D9925EFC4888A823BDA61C58397E03A8
- inputs: savelink_data/arm_P.bin 697B7AF8..., savelink_data/ghost_g1/gamesave_captured.bin E2EABEAC...
- logs: gate_run.log, wsf_run.log
B arm step.csv files are in arms/ with sha8 as in the gate table.
