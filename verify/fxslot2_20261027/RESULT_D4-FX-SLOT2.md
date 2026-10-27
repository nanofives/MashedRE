# RESULT D4-FX-SLOT2: slot 2's 0.26x came from the original reference recipe, not from the port (2026-10-27)

**Verdict: GREEN.** No port change. The original reference behind "slot 2 at 0.26x" (`verify/fxgates_20261026/fx_score.txt`
215.3, `verify/fxinputs_20261026/score_*`) was taken with a warp launch that leaves the slot table `0x007f1a14` at
`[0,0,0,0]` and with player control 4 pulsed for the whole run. In that state all four cars share ctrl block 0, and the
pulses carry the original through 4 more rounds. The port's G-CTRL recipe has neither. Re-captured on the port's recipe
(`--poke-ctrl-slots`, no pulses), the original's car 2 alive series and slot word pair the port's call for call: one alive
span, 220 racing calls, the same kill chain on the same frame, slot word 2 throughout, no respawn. Its FX rate is 41.2 and
60.7 per 1000 active frames (2 reps, same winner as the port) against the port's 55.3.

Branch `orch/D4-FX-SLOT2` off `orch/integration` @ `ebc7b0c36` (`git merge-base` = `ebc7b0c36`). No build, no exe change,
nothing deployed. Private root `D:\MashedWT\D4-FX-SLOT2_root`. Every original run went through `proyectos_test_slot.py` and
`mashed_lock.py run`, and the launcher killed only the PID it spawned.

## 1. Why the old reference is not the port's scenario

Both old capture scripts (`verify/fxgates_20261026/orig_fx_gates_count.py:89`, `verify/fxinputs_20261026/orig_fx_inputs.py`)
launch `scenario_launch.py --track 0 --mode 10 --cars 4 --car 1 --rule 0 --hold N` and nothing else.

- **Slot table.** `scenario_launch.py:4624-4632`: a warp launch skips the allocator at `0x0043f895`, so "every car reads slot
  0 and all four AI cars write controller 0's ctrl block". Both consumers index blocks through that table
  (`disasm_fxgate_brk.txt`, unpatched image, SHA-256 BDCAE093...3C0E):
  - AI step caller `FUN_00418560`: `mov edi, [eax + 0x7f1a14]` (eax = v << 4) at `0x0041856c`, `imul edi, edi, 0x4c` at
    `0x00418572`, `add edi, 0x7f1038` at `0x00418575`.
  - FX gate `FUN_0040ca00`: table pointer `0x7f1a54 - 0x10 * (4 - car)` (`0x0040ca14..0x0040ca27`), then
    `mov edx, [ecx]` at `0x0040cb32`, `imul edx, edx, 0x4c` at `0x0040cb34`, `cmp byte ptr [edx + 0x7f103d], 0x40` at `0x0040cb37`.
  Capture o1 (the old recipe, `alive_o1_unpoked_pulsed.txt`): the slot words read `[0,0,0,0]` on every row from frame 0
  to 3435. The brake byte read through that word is block 0's for every car: cars 0, 1 and 2 count 178 frames > 0x40 in
  their first spans (frames 0..757 to 0..780), which cover the same frames.
- **Input.** Without `--statediff-out`, `scenario_launch.py:5455` calls `E.press(4, 250)` every loop pass. The press
  replaces `FUN_00497310`'s return with 0xff for player 0's control 4 (`scenario_launch.py:62-71`). The port's H recipe
  (`verify/savelink_20261023/run_arms.py` `base_env`) has no player input.
- **Effect (o1).** 5 rounds in 3573 frames. Car 2 alive spans 763 / 343 / 439 / 988 frames. FX in the old census came from
  every round, read block 0's byte 5, and kept a pulsed player in the race.

## 2. The port's recipe on the original (`--poke-ctrl-slots --statediff-out`, no pulses)

`orig_alive_series.py` (CModule: one row per `FUN_0040ca00` call, AI calls per v at `FUN_00416250` entry under phase 3,
events at `FUN_00422fd0` / `FUN_0040eee0` / `FUN_0040d590` entry). Capture oA2 (`alive_oA2_poked.txt`):

| frame | `0x0063ba8c` | alive (car 0..3) | AI calls v1 / v2 / v3 | event |
|---|---|---|---|---|
| 0..369 | 3 | 1111 | 1..371 | |
| 370 / 371..482 | 4 / 5 | 1111 | 371..372 | |
| 483..702 | 6 | 1111 | 372..591 | 220 frames in state 6 |
| **703** | **7** | **0001** | 591 / 591 / 592 | `FUN_00422fd0(0)`, `FUN_0040eee0(0, 1)`, `FUN_0040d590(3, 1, 2, 1)`, `FUN_00422fd0(1)`, `FUN_00422fd0(2)` |
| 704..13743 | 7 | 0001 | 591 / 591 / ..13632 | none |

Slot words `[0,1,2,3]` on every row. Ctrl byte 0xa is 0 on every frame for every car, so `FUN_00410860`'s exit gate never
passes and the run stays in state 7. A first rep (oA, rows lost when the launcher ended the game before the final read,
snapshots only) shows the same shape: cars 1 and 2 stop at 581 AI calls, car 3 runs alone in state 7.

The kill chain is `FUN_0040eee0`'s 4-car arm (decomp of `0x0040eee0`, `DAT_008a94d0 == 4` branch): 3 remaining, mode
10, `FUN_0042f500() == 0`, `DAT_007f0fd0 == 0`, so the third type-2 car gets `FUN_0040d590(car3, car1, car2, param_2)`.

**The port** (H_Z0, exe 5055fc4b, same G-CTRL triple as today's default): player alive f1..f220, `alive=0` at f221
(`H_Z0.ptrace.log`). The same frame logs `ELIM-RESOLVE remaining=3 mode=10 ran=1 alive=0,0,0,1`
(`TrackRenderer.cpp` `ElimResolveAI`, `Resolve3_40d590(i, first, second, d)`), then `R6 ROUND END winner=car3`. Slots 1 and
2 have 220 AI calls, slot 3 has 14,218. `RoundEnd7ExitTick` never passes. The pair log reads blocks 0x7f1084 / 0x7f10d0 /
0x7f111c, so the slot words are 1 / 2 / 3 (see the D4-FX-INPUTS RESULT, section 4).

So past AI call 220 the two sides agree. Car 2 is dead with no respawn on both, its slot word stays 2, and the
survivor is car 3 with throttle 0 in state 7. Port: c4 = 0 on 14,000 of 14,000 state-7 calls. Original: c4 = 0 on 814,
2,517 and 903 state-7 calls of `o_breg_r1/r2/r3.msd.aistep.csv`, where slots 1 and 2 also have exactly 220 (r2: 196)
state-6 calls. The old "port kill at call 220, original alive to call 984" reading compared racing calls with all calls.
The original also counts the 372 AI calls in states 3..5. The port's step dump has no equivalent rows.

## 3. FX rates on the matched recipe (`fx_split_score.py`, `fx_split_score.txt`)

`orig_fx_gates_count.py` (copy of the D4-FX-GATES script plus an alive split on rec+0x4 at each `FUN_0040c690` call).
Port = H_Y1 (`DEAD_COAST + KILL_DAMAGE`, now the default). Emits per 1000 active frames (D4-FX-GATES denominator):

| car | port all (active) | orig fA2 all (active) | orig fA3 all (active) | port alive | orig alive fA2 / fA3 |
|---|---|---|---|---|---|
| 0 | 55.2 (308) | 71.7 (307) | 52.1 (307) | 65.7 (198) | 75.8 / 55.6 (198 / 198) |
| 1 | 106.6 (394) | 130.4 (391) | 125.3 (391) | 157.4 (216) | 191.6 / 200.9 (214 / 214) |
| **2** | **55.3 (416)** | **41.2 (413)** | **60.7 (412)** | **59.1 (220)** | **54.8 / 59.6 (219 / 218)** |
| 3 | 215.5 (232) | 258.6 (232) | 258.6 (232) | 215.5 (232) | 258.6 / 258.6 (232 / 232) |

Active-frame counts agree to 1 to 4 frames on every car, alive and dead. Slot 2 is inside the original's own rep spread.
All 4 cars are within 2x (lowest ratio: car 3, 0.83x). The emit counts are small (12 to 60), so the per-rep spread is wide.

A third rep (fA, first run) had car 1 as the winner. Car 1 then had 9,252 active frames in state 7, and car 2 103.6 on
589. Winners differ between runs (memory `round-level-outcomes-do-not-reproduce`). [UNCERTAIN] how the original's state-7
winner reaches the 62,500 speed gate in that run with c4 = 0 on every state-7 AI call in the aistep captures. Missing
evidence: a per-frame |v| and ctrl series for the winner of a car-1-wins run. In this step's scenario the port's winner is
car 3, and the 2 original reps with winner car 3 give 232 active frames on both sides.

## 4. Change and gates

- No source change and no build. Files added are capture scripts, scorers and summaries in `verify/fxslot2_20261027/`.
- Gates: not run. The exe is unchanged (no source edit on this branch).
- Trackers: no U-row covered the 0.26x (it lived in two RESULT files), so no `re-classify` transaction is owed.

## 5. Next

1. Re-baseline the FX comparison on this recipe. `verify/fxgates_20261026/fx_score.txt` and the
   `verify/fxinputs_20261026/score_orig*.txt` / `score_killdamage_Y.txt` original columns come from the unpoked, pulsed
   recipe, so their per-slot ratios (slot 1 at 0.64x included) are not port comparisons. Use `orig_fx_gates_count.py ...
   --poke-ctrl-slots --statediff-out <msd>` for future original FX references.
2. Carried from D4-FX-INPUTS: the rec+0x44 / +0x48 = 0xbf800000 reader, and the second-round reset of the kill words (needs
   a port recipe that leaves state 7).

## Files in this folder

`orig_alive_series.py` (per-frame series capture), `alive_series_score.py`, `alive_o1_unpoked_pulsed.txt`,
`alive_oA2_poked.txt`, `orig_fx_gates_count.py` (FX count + alive split), `fx_split_score.py`, `fx_split_score.txt`,
`capdis.py`, `disasm_fxgate_brk.txt`.

## Raw data (D:/MashedWT/D4-FX-SLOT2_data, not committed), SHA-256

```
625dac00b330d7787fbe38e58de526a23ae5231851c62a16937af0d539e6f864 o1.rows.bin
12393cd43ebcb1d30b0b701d776e7718e2d117f595d13c10beeaba9988e8e470 o1.events.json
0ed8164d6552714c0eebe5e8fd9a91d5456cf4932944688549c859acb9b8942a o1.json
fdc45ca78fac012d6e34990ff7e99f65a5629f1dcd8a6dcebb1b9986df6403b9 oA2.rows.bin
a35544f83bb8a05ffeec2d9d616a0711c2df7c3e6bcb3af1ee706ff0747b0cbf oA2.events.json
e73b3870a7e541e7136ab515595db76720c0cefe7f65ee6006c8e584e52b5351 oA2.json
90bcd2c8b1cf437762563a2720220f632ae9aafde3233e10f08f1a4ebc55c0df oA2.msd
b5ece79b6f3850d71781c1af57ea23e6d7ba38b3f67de0ab8d006123496062f3 fA.json
cd72d3f1bdbae26d7799e37649c12eeb86c89ba96def14b8a5f07a4653c04532 fA2.json
a9587ada64e2e5a6e41171bf35938446e94e89050fd0b0b7f662c3467d7c91cb fA3.json
a15c0ff7fb2792e9fff8e4a6c0e0adc77a130b99b9434a60d78c7019290f5d8c fA.msd
ab095cb520bac31674071c2ff2d48ec2b3253d4793f13191d5598cb06cb63616 fA2.msd
20efa04e0924ebdab2ffd6b426166349acee692d59c94c2c08b68778ca2b2f40 fA3.msd
a604fe39d3cf389433fd326d67ec6b4011ff1d03c4eb1f4722d60d3b2b581baf o1_run.txt
74a13f0b03627271230e0c97163a77ce3ab836b1ca7b09764349d5793e767e9a oA2_run.txt
09ca160825234280a9c7807ff0f62365b6024b6f2e02b3694144ceb73e190a05 oA_fA_run.txt
a08b1f3ca67e9efa4183a40e37509a2baed7d9ede2349f8be8ff90349ca856d7 fA23_run.txt
7476bd32e1c8ff6f8c3cbf0136f291a35375fee619e4ee227c3f259d0a88c609 D:/MashedWT/D4-FX-INPUTS_data/arms5/H_Y1.fx_debug.txt
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 D:/MashedWT/D4-FX-INPUTS_data/arms5/H_Z0.step.csv
09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1 D:/MashedWT/D4-FX-INPUTS_data/arms5/H_Z0.ptrace.log
cfafa3d4d272b2f872f5ca921464ad6d018fdffd17942bf6438da85956474a37 D:/MashedWT/D4-FX-INPUTS_data/arms5/H_Z0.log
bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e original/MASHED.exe.unpatched
```
Decomp: `re/tools/decomp_pc.py 0x0040eee0 0x0040e470 0x00422fd0 0x0040d590 --callers` (main checkout pool slot,
read-only; the worktree copy found no pool), output `D:/MashedWT/D4-FX-SLOT2_data/decomp1.txt`.
