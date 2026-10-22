# PREREG r7keep: rule 7 keeps the score-scan winner (0x00410510) (2026-10-22)

Written before any code change. Branch `work/r7keep` from `e7f4de987` (dcmix merged). Opened on Mariano's go after
`verify/dcmix_20261022/RESULT_dcmix.md` decision 2.

## 1. The finding (static, from `original/MASHED.exe.unpatched`)

`FUN_00410510` switches on `DAT_007f0fd0` through the jump table at `0x00410844` (`0x00410602..0x0041060f`, index = rule - 4).
Rule 7 is index 3 -> `0x0041075a`:

```
0x0041075a  xor edi, edi                 ; loop counter only
0x0041075c  call 0x40e340                ; participants (mov eax,[0x008a94d0])
0x00410763  jle 0x410622
0x00410770  push edi / call 0x417730     ; metric[edi]
0x00410776  fcomp [0x5cc31c]             ; 3.0
0x00410784  je 0x410795                  ; metric > 3.0 -> esi = edi + 1
0x00410786  inc edi / call 0x40e340 / cmp edi, eax / jl 0x410770
0x00410790  jmp 0x410622                 ; no car above 3.0: esi NOT written
0x00410795  lea esi, [edi + 1]
0x00410622  cmp esi, ebp (ebp = 0) / je 0x410838   ; esi == 0 -> return 0
```

`esi` holds the winner from the two score scans above the switch. On the no-match exit the original keeps it. The exe
(`Race/RuleEngine.cpp:EvaluateResult`, `case 7: winner = 0;`) clears it, so it returns 0 where the original returns the
score-scan winner and ends the match.

## 2. Change under test

Knob `MASHED_RULE7_KEEP` (default **OFF**). ON: `case 7` no longer clears `winner`; the metric loop overwrites it only on a car
above 3.0, as `0x0041075a..0x00410798` does. Carried to the pure engine as `Persist::rule7KeepWinner` (set by TrackRenderer from
the env var), so the unit test can drive it. Trace `MASHED_RULE7_TRACE=<file>`: one line per rule-7 `EvaluateResult` call
(scan winner, metric winner, returned value, knob state); counts `n_eval` and `n_div` (scan winner != 0 and no metric above 3.0).

## 3. Reachability (why the prediction is "no effect")

In the exe a rule-7 segment ends (`SegmentCheck` case 7, from `0x00410d10`) on car 0 dead, any metric above 3.0, or
`participants - 1` dead, and always returns before the elimination block, the only path that awards score
(`ScoreAward`, FUN_0040eee0). Scores are seeded 6 (4 participants) or 4 (2-3 participants, team play, rules 1 / 2)
(`SeedMatchScores`, 0x0040b180). The score targets are > 11 (4 participants) and == 8 (2-3 participants or teams). The
original's rule-7 oracle run (`log/rules_oracle_rule7.json`, 2026-09-26) held scores at `[6,6,6,6]`. So the divergent case needs
a score that rule 7 itself never moves.

## 4. Predictions

| id | arm | prediction | fails if |
|---|---|---|---|
| P0 | knob OFF, recipe F (default) | G-CTRL `c3bfcef3 / 4ed4849b / 4c3548a1` x2, B `4985eb3b / b6e3d073 / 6717c15e`, (b) 9/9, (e) 3/3 same launch values, D2 3/3, 13 tracks table identical to dcmix | any hash or score differs |
| P1 | knob ON, recipe F (rule 0) | identical to P0 (case 7 is not executed under rule 0) | any hash differs |
| P2 | rule 7 (`MASHED_ROUND_RULE=7`, recipe F otherwise), knob OFF vs ON, trace on | `n_div = 0`; step / gates / ptrace hashes ON == OFF | `n_div > 0`, or any hash differs |
| P3 | `ruleengine_test` | existing checks pass; new seeded case (4 participants, car 2 score 12, alive refinement on, no metric above 3.0, rule 7): knob OFF -> 0, knob ON -> 3 and `p0Won` false; car 0 score 12 -> 1 and `p0Won` true | any check fails |
| P4 | original, `scenario_launch.py --oracle --rule 7` (stock, no hooks), JS law extended with the keep variant | every `EvaluateResult` call: both laws agree with the original (natural calls are not divergent) | a mismatch, or a call where the two laws differ |

The divergent branch itself has no natural witness on either side (section 3): its evidence is the disassembly in section 1
plus P3. That is stated as such in the result, not upgraded.

## 5. Decision rule

P0-P4 all hold -> report, and recommend flipping `MASHED_RULE7_KEEP` default ON with `MASHED_NO_RULE7_KEEP` as the opt-out
(Mariano decides). P2 with `n_div > 0` -> no flip recommendation until those calls are compared with the original. Any P0 / P1
difference -> the change is not isolated: stop and report.
