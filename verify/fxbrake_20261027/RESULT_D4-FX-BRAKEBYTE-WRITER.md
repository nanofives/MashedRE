# RESULT D4-FX-BRAKEBYTE-WRITER: the slot 2/3 brake-byte writer is the FUN_00418560 override replay, and the port pairs it (2026-10-27)

**Verdict: GREEN.** The byte the FX gate reads at ctrl+5 for slots 2 and 3 is written by the original's
`FUN_00418560` override-replay tail at `0x00418816`. The port has the same tail (`Ai/AiStandalone.cpp`, step tail
`ctrl[5] = kAiReplayB5`), armed by the default-ON `MASHED_AI_CONTACT_EVENT`. In the 220-call (b) window the port's
step byte, replay flag and FX-visible byte match the original call for call: 220 / 220 on all three slots on 8 of 8
regime-A original captures. No sim change is needed. The only source edit is a comment on the pair-log diagnostic.

L-34's open line ("brake byte overwritten before the FX read, writer not found") came from a sampling phase. The pair
log's `c5_step` is not the block after the AI tick. Under `MASHED_AI_CONTACT_EVENT` it is `Ai_StepOutCtrl(v)`, the
byte `FUN_00416250` returns, sampled before `FUN_00417640` and the replay tail (`TrackRenderer.cpp` AiStepDump,
`so ? so[5] : U8(blk + 5)`). The FX gate reads the block after the tail. The two differ only on replay calls.

## 1. The writer in the original (`original/MASHED.exe.unpatched`, SHA-256 BDCAE093...3C0E, anchor matches)

`re/tools/decomp_pc.py 0x00418560 0x00417370 0x0040ca00 --callers` (pool slot 10, read-only) plus capstone over
the unpatched image (`tail_disasm.txt`):

| address | instruction | effect |
|---|---|---|
| 0x004187ce..0x004187d0 | `push edi; push ebp; call 0x417640` | post-step `FUN_00417640` |
| 0x00417643..0x0041764b | `call 0x426c00; cmp eax,0x21; jne 0x417729` | returns unless course 0x21 (G-CTRL runs Training) |
| 0x004187d5..0x004187e0 | `mov eax,[esi+0x89a4fc]; cmp eax,ebx; je 0x418819` | replay only while the timer `0x0089a4fc + v*0x74` != 0 |
| 0x004187e2..0x004187f0 | `sub eax,[0x7f1008]` ... `jns` / store 0 | countdown, clamped at 0 |
| 0x0041880a | `mov dl,[esi+0x89a518]` | stored b5 |
| **0x00418816** | **`mov byte ptr [edi+5], dl`** | **the ctrl+5 write** (edi = `0x007f1038 + slot*0x4c`) |
| 0x0041882a / 0x0041883a | `movzx eax,[edi+5]` / `mov [esi+0x89a518],eax` | the final byte is stored back for the next call |

The timer is set by `FUN_00417370` (`0xb4 - 0xb4 * ftol([0x0089a360] - 10.0)`), whose only caller is the contact
drain `FUN_00422ba0`. `FUN_0040ca00`'s only caller is `FUN_0040ce00`. Neither appears among `FUN_00418560`'s
direct callees, so the FX gate reads the byte as the tail left it. So on a replay call the visible byte is the
previous call's final byte, frozen for the whole override window.

## 2. Port side: every Z0 mismatch is a replay call (`replay_join.py`, `replay_join_Z0.txt`)

Z0 arm from D4-FX-INPUTS (knobs OFF + `MASHED_FX_PAIRLOG`, exe 5055fc4b), alive FX-gate rows:

| v | rows | c5_step != c5_fx | replay calls | mismatch on replay | mismatch off replay | replay rows with fx == previous final |
|---|---|---|---|---|---|---|
| 1 | 203 | 0 | 0 | 0 | 0 | 0 / 0 |
| 2 | 215 | 32 | 73 | 32 | **0** | 73 / 73 |
| 3 | 231 | 18 | 27 | 18 | **0** | 27 / 27 |

Replay predicate for the port dump, which reads the timer after the countdown: `ai_override(N-1) != 0 or
ai_override(N) != 0`. The step.csv `c5` equals the pair log's `c5_step` on 649 / 649 rows, so both are the
pre-tail byte.

## 3. Original pairing, call for call (`visible_c5.py`, `visible_pair.py`, `visible_c5.txt`, `visible_pair_Z0.txt`)

Both sides cut to the (b) window, 220 calls from the first `c4 != 0` (the `ovon.py` rule). The original's aistep
probe sits inside `FUN_00416250`, before the countdown, so its replay predicate is `ai_override(N) != 0`. Visible
byte = previous visible byte on a replay call, else the step byte.

Window counts, port Z0 and every regime-A original capture alike:

| v | replay calls | onsets (window call) | step c5 > 0x40 | visible c5 > 0x40 |
|---|---|---|---|---|
| 1 | 0 | none | 47 (21.4%) | 47 (21.4%) |
| 2 | 73 | 23, 60, 205 | 41 (18.6%) | **9 (4.1%)** |
| 3 | 15 | 205 | 66 (30.0%) | 72 (32.7%) |

Per-call equality, port Z0 against each original (step byte / replay flag / visible byte, out of 220):

| original | regime | v=1 | v=2 | v=3 |
|---|---|---|---|---|
| o_breg_r1, r3, r4, r5, r7, r8, o_p0a, o_p2b (8 files) | A | 220 / 220 / 220 | 220 / 220 / 220 | 220 / 220 / 220 |
| o_breg_r2 | C (no 205 contact) | 166 / 196 / 166 of 196 | 195 / 196 / 195 of 196 | 181 / 205 / 175 |
| o_p2a, o4 | B (1+3 at 159) | 180 / 191 / 157 | 193 / 205 / 207 | 177 / 176 / 179 |

So the replay suppresses 32 of slot 2's 41 window brake calls on the original too. The port reproduces it exactly.
Port timer reads 1390 after the countdown against the original's 1440 before it (one `DAT_007f1008` step of 50,
already noted in memory `contact-schedule-from-override-onsets`).

## 4. What this leaves for the slot 2 FX rate (0.26x)

The brake byte does not cause it inside the window. In the port, slots 1 and 2 are dumped for 220 AI calls and the
pair log has 13,998 non-alive FX rows per slot. Slot 3 runs 14,218 calls. The original's census counts 2,041 FX-gate
frames for car 2 with brk > 0x40 on 26.7% of them (`verify/fxinputs_20261026/score_killdamage_Y.txt`), against 4.1%
visible in the window. So the original's extra brake frames come after call 220, where the port's slot 2 no longer
drives. [UNCERTAIN] the original car 2's alive span and kill frame in this recipe against the port's. Missing
evidence: an original-side per-frame alive / slot-word series for car 2 past window call 220 (the census has only
10 s snapshots).

## 5. Change and gates

- `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`: comment only, on the `MASHED_FX_PAIRLOG` block. It says
  `c5_step` is the pre-tail byte and cites `0x00418816`. No code change, no build, no default touched.
- Gates: not run. The edit is comment-only, so the exe is unchanged. The cited port data (exe 5055fc4b) is
  pre-flip, and G-CTRL h1 (step.csv) was `5943c5f1` with the knobs both OFF and ON (D4-FX-INPUTS section 5), so the
  step-dump window measured here is the same on today's default.
- Trackers: no U-row was filed for this item. The open item lived only in `RESULT_D4-FX-INPUTS.md` section 4, so no
  `re-classify` transaction is owed.

## 6. Next

1. Slot 2 FX rate: pair the original's car 2 alive / kill timeline past window call 220 with the port's (kill at
   AI call 220). Cheapest path: add a per-frame alive flag and slot word to `orig_fx_inputs.py` and re-run one
   original capture.
2. Carried from D4-FX-INPUTS: rec+0x44 / +0x48 = 0xbf800000 reader; second-round reset of the kill words.

## Raw data, SHA-256 (not committed, or already in git where noted)

```
5e15b80ce7abd2edc7037766bf5b28e88e6ac49d990af21bdc1296e4dd62e2dd D:/MashedWT/D4-FX-INPUTS_data/arms5/H_Z0.pair.csv
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 D:/MashedWT/D4-FX-INPUTS_data/arms5/H_Z0.step.csv
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 D:/MashedWT/D4-FX-INPUTS_data/arms5/H_DEF.step.csv
01ac202ab18d2d3dbb85635c2d6c8320b9c6a9ff6b0ff75d51e36dd1d4ab4843 verify/d3_bscore_20261009/o_breg_r1.msd.aistep.csv (git)
e4dd0a24ec04c21476f4af441c0474ec55a9ab383b4b09cab39c81ab5c9afe08 verify/d3_bscore_20261009/o_breg_r2.msd.aistep.csv (git)
081c4614b7e895faad1b01ccc5c24dc826d369e5afb0dee1f47f33f856f58862 verify/d3_bscore_20261009/o_breg_r3.msd.aistep.csv (git)
7d5fea3dae2340e003b4e671958f64e359032203c29cde9603121e57e0fec4a7 verify/d3_bscore_20261009/o_breg_r4.msd.aistep.csv (git)
110cc7fa378ac37b62d031c61ba6118b90d5142840a61b4527deda0b0fb68dc9 verify/d3_bscore_20261009/o_breg_r5.msd.aistep.csv (git)
cf1e1266e08372207cd8bac8f5e166e657053882d92ee34ad2b1fd79105c43e1 verify/d3_bscore_20261009/o_breg_r7.msd.aistep.csv (git)
822be7ca3e6688518fb931ac89ee5a19b9c0ec99c46d050b778a32c1a4ea1322 verify/d3_bscore_20261009/o_breg_r8.msd.aistep.csv (git)
c344b17099f4acca87c90da2cc3f77c217e84f66c1609ba7cffe0213343b08fb verify/d3_car2pickup_20261013/o_p0a.msd.aistep.csv (git)
4b036c5377276f2f9dfd3f7744652d8759bcc63a23f962bde3a904c480977e4b verify/d3_car2pickup_20261013/o_p2a.msd.aistep.csv (git)
3123edca4d593b855799c3d54b937374909d479e34cc26a31b89fd33348f228e verify/d3_car2pickup_20261013/o_p2b.msd.aistep.csv (git)
3daca65ed0833af2e5ae4f78c93f2765d42d00c922f6df08ad4afb893d75816e verify/d3_ai_20260927/o4.msd.aistep.csv (git)
bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e original/MASHED.exe.unpatched
```
No game was run and nothing was built in this step, so no test slot or game lock was taken.
