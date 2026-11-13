# RESULT RO-OT-TIMER-STRICT-2P31 (top-down / tool row)

Branch `orch/RO-OT-TIMER-STRICT-2P31` from `orch/integration` @ `69cd03777`. No source change was needed (section 1).

## Verdict

GREEN. On `orch/integration` the strict OT_TIMER check already follows the original's bytes for `now >= 2^31`. Both builds pass it with
`now` above 2^31: VS2022 0 of 6000 failed, object build 0 of 6000 failed. No residual cases.

## 1. State of the check model

`mashedmod/src/mashed_re/Match/PortTop_00492370.cpp:483`:
`f1010x = (now < 0x80000000u) ? F1010SingleRounding(now) : f1010`, with `f1010 = FBits((float)now * 3.3333333e-07f)` (line 478).
This is the model the OT-TIMER-VS2022-FORM RESULT section 3 asked for. It entered with the RO-EXE-REPLAY-KNOB merge
(`fe4a56888`) and the comment at lines 479-483 cites the same evidence (original bytes: `0x005cc94c` fadd 2^32 fix-up, then fmul).
The row's premise (both builds fail strict above 2^31) described the run-4 form of the line, which the OT-TIMER-VS2022-FORM
working tree used. No edit was made, so there is nothing for the merge train to apply.

## 2. Runs (this row, committed tree, nothing edited)

Builds, `MASHED_NO_DEPLOY=1`: VS2022 and `MASHED_DDKOBJ=1`, both `Build OK`, rva-lint NEW=0.
`ddkobj.py check` (inside the object build): IDENTICAL 1188/1214, DIFFERENT 11 [0x00401690 0x004332a0 0x0043c5b0 0x0045a3a0
0x00475a00 0x004929d0 0x00493710 0x00493900 0x00498810 0x004c0c20 0x004c0f00], not-in-map 15, units linked 1164/1440,
refusals 4 (wrapper-redefines-globals=4). `0x00493390 _PortTop_Body_00493390` and `0x00493480 _FpsDiscretise` are IDENTICAL in BYTECHECK.tsv.

Arms: `knob_boot.py <label> D:/MashedWT/OT-TIMER-VS2022-FORM_root 180 MASHED_WIN_POS=primary-bl MASHED_DET_FRAMES=600 MASHED_ORIG_TOP=1 MASHED_OT_TIMER=1`,
test-slot outside, `mashed_lock.py run` inside, rc 0, 600 frames each.

| arm | exe | `now` range (call 1 .. call 600) | self-test |
|---|---|---|---|
| vs_strict_a | VS2022 | 4100124285 .. 4199615181 (all >= 2^31) | PASS, 0 of 6000 checks failed, VS2022 double-rounding form 0 |
| ddk_strict_a | object build | 2820178160 .. 2836629922 (all >= 2^31) | PASS, 0 of 6000 checks failed, VS2022 double-rounding form 0 |

Both arms ran in the `now >= 2^31` half, so the branch of the model this row concerns was exercised on every call (600 calls each,
checks on `DAT_007f1010` included). The `now < 2^31` half was not re-run here. It is covered by the OT-TIMER-VS2022-FORM RESULT
(VS2022 + fix 0 of 6000 on two arms) and the same line 483 branch is unchanged.

Self-test result for the top-down RESULT: OT_TIMER self-test PASS on both builds (0 of 6000 each).

## 3. Not run

- No menu walk (no menu code changed), no G-CTRL, no B, no 12 captures (top-down row, WORKER_RULES "Row kinds").
- No tracker or C4 change. `[UNCERTAIN]` from OT-TIMER-VS2022-FORM stands: the x87 control word was not read directly. The PC_24
  reading is consistent with the passes above (the fadd rounding model matches on 1200 calls in this half) but a `_controlfp` log was not taken.
- The strict-check sensitivity above 2^31 was not re-demonstrated against the old model here (no control arm with the run-4 line).

## Raw data (`D:/MashedWT/RO-OT-TIMER-STRICT-2P31_data/`, SHA-256)

- vs_strict_a.log `bf3d6722c5f69bfdf3c3cd05d78f05e5e4936d85efd12f43aeecae2ebcc572b9`
- ddk_strict_a.log `22bebd2f7289b745450701b5fae8c7d6e351375c02550c5b3b78c5af5917d47c`
- mashed_re_vs.exe `cc13f08ce566b157afe5c63d7a63395e1595fb81c3f4647fc189dd09d04fb02c`
- mashed_re_ddk.exe `413412303e3e5162b13b251507acfb23b62db8541e73610012ff938e72440bdb`
