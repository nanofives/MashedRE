# RESULT AUDIO-STACK-STEP1 (L-158 (a) step 1, run 46, top-down row)

## Verdict: GREEN (PASS). Both audio init arms reach 0x005ADC10, the same stop as AUDIOPOOL-5AE650. Every self-test passes.

- Branch `orch/AUDIO-STACK-STEP1`, merge commit `fd0134dff` = orch/integration `d1e5ac586` (carries IMG-5) + `8bf7a2079`.
- Both builds OK. `ddkobj.py check`: IDENTICAL 1456/1512, DIFFERENT 41, not-in-map 15, refusals 9. The DIFFERENT set is the
  union of the two parents' sets (section 3).
- The six IMG-5 image-ON arms: all six reach `Calling AppDestroy` and log `WinMain 0x00492370 returned 0`, on both builds.
- The two audio init arms: both exit at the named stop **0x005ADC10**, called from `_PortTop_Bound_005add60`.
  - VS2022 build: return 0x0023A71D = `_PortTop_Bound_005add60+0xd`.
  - Object build: return 0x000459FC = `_PortTop_Bound_005add60+0xc`.
  - AUDIOPOOL-5AE650 recorded the same offsets, +0xd and +0xc.
- G-CTRL, B and the 12x4 captures were not run: this is a top-down row (WORKER_RULES). The merge train runs the default gates.

## 1. Restack

The five component branches are already one linear chain:
`aa79679df` (STOPS) < `7e5466a15` (HWINIT-4627F0) < `c3eb90cee` (RWAUDIO-TABLE-632BA8) < `2ed7d34c3` (AUDIOPOOL-5AE650) <
`8bf7a2079` (AUDIOTICK-IMGTRAP-4568A0).
- `git merge-base --is-ancestor aa79679df 8bf7a2079` holds.
- The first-parent log of `orch/integration..8bf7a2079` lists the four predecessors in that order.

So the restack is one merge, `git -c merge.autoStash=false merge --no-ff 8bf7a2079`, on a worktree branched from `d1e5ac586`.
Cherry-picking was not used, because the chain contains merges from orch/integration.

### Conflicts (8) and resolutions

| file | kind | resolution |
|---|---|---|
| `mashedmod/exe_sources.rsp` | both sides appended TUs at the same point | union, ours first (+240 lines against d1e5ac586), no duplicate line |
| `re/tools/dual_copy_allowlist.txt` | IMG-5 / RWC-FRAME-CLUMP rows against audio rows at the tail | union (+64), no duplicate kind:rva key |
| `re/match/ADMITTED.tsv` | 0x00420050 (integration) against 12 audio s30 rows | union (+12), no duplicate rva |
| `re/match/PROVISIONAL.tsv` | 0x00455400 (IMG-5) against 0x005a5e70 and 0x005ae650 | union (+5 lines), no duplicate rva |
| `re/analysis/CHANGELOG.md` | IMG-5 C4/C1 lines against RWAUDIO-TABLE C1 lines | union, IMG-5 block first (newest first), then the audio block |
| `verify/porttop_20261108/BINDINGS.tsv` | row 0x004568a0 on both sides (`genfiles_merge bindings` reported 1 conflicting record) | IMG-5's row kept. Same binding (`Orig_004568a0`, matched), only the note differs |
| `mashedmod/src/mashed_re/Match/PortTop_004568a0.cpp` | add/add: IMG-5 and IMGTRAP both added the body of 0x004568a0 | IMG-5's file kept (detail below) |
| `mashedmod/src/mashed_re/Compat/PortTopCodePtrImage.inc` | generated file, both sides regenerated | taken from d1e5ac586, then regenerated with `py -3.12 re/tools/image/codeptr_image.py --write` on the merged tree |

- **0x004568a0 (add/add).** IMG-5 (merged on integration in run 45/46) and IMGTRAP-4568A0 both bound slot 0x005f99b0 to
  `Orig_004568a0` = `re/match/s09/004568a0.c`. Integration's `Compat/PortTopCodePtr.inc` already has
  `CP_FIX(0x005f99b0, Orig_004568a0, 0x004568a0)`. The auto-merge of the s09 source took IMGTRAP's body edit
  `FUN_004768c0((int)&DAT_006887d0)`, while IMG-5's prototype in the same file is `void FUN_004768c0(void *p0)`. Under
  C++ (MATCH_PORT) that does not compile.
  - Resolution: the source and the .cpp are both IMG-5's (`git checkout d1e5ac586 -- re/match/s09/004568a0.c
    mashedmod/src/mashed_re/Match/PortTop_004568a0.cpp`). IMG-5's RESULT records this source at match.py 100% with relocheck OK.
  - Result: IMGTRAP-4568A0 adds no change of its own to the merged tree beyond its RESULT and arms output files.
  - In this build, ddkobj reports 0x004568a0 IDENTICAL (`BYTECHECK.tsv`).
- **PortTopCodePtrImage.inc.** The regenerated file binds 107 image slots: d1e5ac586 bound 75, 8bf7a2079 bound 99. 107 is
  the size of the union of the two `CP_IMG_FIX` slot sets. Read from both parents' files.
  - `re/tools/image/CODEPTR_IMAGE.tsv` changes in 32 lines against d1e5ac586. These are the audio-table slots bound on
    the 8bf7a2079 side.
  - Against 8bf7a2079 it changes in 9 lines. These are the 9 teardown slots 0x005f99b0..0x005f9bb0 bound by IMG-5.
  - The generator warnings ("CODEPTR_SLOTS slot missing from the export", 0x00632c30 and others) are not new [UNCERTAIN:
    I did not run the generator on either parent to compare its warnings].
- Hand checks after resolving: no conflict markers in any changed file; no duplicate non-comment rva in `hooks.csv`
  (auto-merged); no duplicate line in the rsp; no duplicate rva in BINDINGS, ADMITTED or PROVISIONAL.

## 2. Builds (from the worktree, `MASHED_NO_DEPLOY=1`)

| build | result | rva-lint | exe sha256 (16) |
|---|---|---|---|
| VS2022 | Build OK | 313 known, NEW 0 | `e5fe42c847661b04` (snapshot `default/mashed_re.exe`) |
| object (`MASHED_DDKOBJ=1`) | Build OK | 313 known, NEW 0 | `968465da9339a483` (`mashedmod/build/ddkobj/mashed_re_ddk.exe`, snapshot `ddkobj/mashed_re.exe`) |

`original\mashed_re_dev.asi` was not deployed (`[skip] MASHED_NO_DEPLOY set`).

## 3. `py -3.12 re/tools/ddkobj/ddkobj.py check`

`IDENTICAL 1456/1512, DIFFERENT 41, not-in-map 15, units linked 1455/1740, refusals 9
(externalized-static-register-convention=5, wrapper-redefines-globals=4)`.

The DIFFERENT set has 41 rows:
- The 34 rows of AUDIOPOOL-5AE650's check (`D:/MashedWT/PORT-TOP-AUDIOPOOL-5AE650_data/ddkcheck.log`).
- 7 further rows: 0x00552890, 0x005530f0, 0x005533d0, 0x005538a0, 0x00554200, 0x00554940, 0x005572c0.
  - These 7 are already DIFFERENT on the integration side: `verify/ddkregconv7_20261113/BYTECHECK.tsv` lists them, among
    the 18 rows integration had.
  - That check also has the same 9 refusals (externalized-static-register-convention 5 and wrapper-redefines-globals 4).
- So nothing became DIFFERENT through the merge. Count: integration 18 + audio 34 - 11 shared = 41.

Provisional bodies keep their VS2022 body under L-126 (6a). The provisional sources are 0x00455400, 0x005a5e70 and
0x005ae650, plus the earlier audio provisionals.

## 4. Knob boots (`D:/MashedWT/AUDIO-STACK-STEP1_data/boots.sh`)

How the boots were run:
- `test-slot` outside, with no hub override. The run waited 10 min for memory, then started under the cap in slot 8/8.
- `mashed_lock.py run --label AUDIO-STACK-STEP1` inside.
- Private root `D:/MashedWT/AUDIO-STACK-STEP1_root`, a robocopy of AUDIOPOOL-5AE650's root. `original/mashed_image.bin`
  sha256 `81cc7775...9afcd8`, the same as IMG-5 and AUDIOPOOL.
- The arms are IMG-5 run 46's arm set (with the knobs-off control) and AUDIOPOOL-5AE650's two init arms
  (`verify/portorigtop_20261113/audiotick_stops_arms.py`, `ARMS_EXTRA="MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1
  MASHED_OT_CODEPTR=1"`, `KNOB_EXE` = the VS2022 snapshot for the vs arm).
- After the run, `original/mashed_image.bin` is absent, so the real original is untouched.

| arm | exe | knobs | rc | end |
|---|---|---|---|---|
| def_off | VS2022 | DET_FRAMES=3000 | 0 (43.5 s) | knobs off, `B17-SUMMARY reached-main-loop` |
| def_on_img1_cp_trap1 | VS2022 | ORIG_TOP + PORTSTATE_IMAGE + OT_CODEPTR + CODE_TRAP=1, 3000 frames | 0 (28.8 s) | AppDestroy, `WinMain 0x00492370 returned 0 after 3000 frames` |
| ddk_on_img1_cp_trap1 | object | same | 0 (33.0 s) | same |
| def_on_img1_cp_trap2 | VS2022 | same, CODE_TRAP=2 | 0 (29.3 s) | same |
| ddk_on_img1_cp_trap2 | object | same | 0 (27.8 s) | same |
| def_aud_img1_cp_trap1 | VS2022 | ORIG_TOP + OT_TIMER + OT_AUDIOTICK + PORTSTATE_IMAGE + OT_CODEPTR + CODE_TRAP=1, 600 frames | 0 (7.9 s) | AppDestroy, `WinMain ... returned 0 after 600 frames` |
| ddk_aud_img1_cp_trap1 | object | same | 0 (7.8 s) | same |
| vs_init_audio | VS2022 | ORIG_TOP + OT_TIMER + OT_AUDIOINIT + OT_AUDIOTICK + PORTSTATE_IMAGE + CODE_TRAP=1 + OT_CODEPTR, 600 frames | 0x005ADC10 (2.5 s) | `[OT_TIMER] STOP: the matched 0x00000000 called original 0x005ADC10 ... Return address 0023A71D` |
| ddk_init_audio | object | same | 0x005ADC10 (9.9 s) | same line, `Return address 000459FC` |

Return addresses were symbolized against `default/mashed_re.map` and `ddkobj/mashed_re_ddk.map`, taking the nearest
preceding symbol: `_PortTop_Bound_005add60+0xd` (VS2022) and `+0xc` (object).

**Self-tests.** Every arm that sets the knobs logs PASS, and the logs contain 0 `self-test FAIL` lines.

| self-test | PASS line | arms |
|---|---|---|
| `[portstate-image]` | PASS (0 of 8 checks failed) | all 8 image arms |
| `[OT_CODEPTR]` | applied 188 slots (11 bound, 177 stop thunk), PASS (0 of 367 checks failed) | all 8 image arms |
| `[code-trap]` | PASS (0 of 6 checks failed) | all 8 image arms |
| `[OT_TIMER]` | PASS (0 of 8413 checks failed) | the 2 AUDIOTICK arms |

[UNCERTAIN] The init arms print no `[OT_TIMER] self-test` line, because they stop before the line is written. AUDIOPOOL's
init arms had the same property.

**Same as IMG-5 run 46.** I compared `boots/def_on_img1_cp_trap1.log` with IMG-5's run-46 log of the same arm
(`D:/MashedWT/IMG-5-TEARDOWN-BODIES_data/run46/boots/def_on_img1_cp_trap1.log`), after masking 8-digit hex and taking
line counts.
- Both logs have 1864 lines.
- They differ only in 4 kinds of line: the cfglog timestamp, the root path, the x87 tid, and the B17 commit total
  (9892 KB against 9956 KB).

## 5. Next
- Lead: enqueue `orch/AUDIO-STACK-STEP1` at the tip sha in the REPORT. The train's default gates must be unchanged.
- The next audio stop is 0x005ADC10. It is reached from 0x005add60 and is already filed as `PORT-TOP-AUDIOSTOP-5ADC10`
  (AUDIOPOOL RESULT section 5).
- The AUDIOTICK image arms continue to AppDestroy. IMGTRAP's next stop, 0x004540C0 (entry 1), is passed on this tip,
  because IMG-5 bound entries 1..8.

## Raw data (`D:/MashedWT/AUDIO-STACK-STEP1_data/`, sha256)
```
a9c35068586d1d78ae2ab7cd2e013b4aab39f0adf1ece61974107d1195a402c5  boots.sh
e2b66a023dc6ec336154f6b2f5901d71bc1de9b712cdc984c68303df8ff5b834  boots.out
6246866ae92f440f9ec1fc2e19bbce52a5828109c20a1c97d0b1b6c03db9c2d2  build_vs.log
94cd05124ede66636237318280a9b76ad785be951cdd92878a37fe5df19b6033  build_ddk.log
411c26f2f234422a76b935200fca2e9dbdcb06dc28f63686699d43a793104d70  ddkcheck.log
7e03f81a2f5da88a4b71c8e17dffb92b5d077fcef08d897dfd011a1436a81696  boots/def_off.log
c0a5d7fa742c8996e789f684078e3e2f7a2ece56c68cf90c4e30049813daa6ad  boots/def_on_img1_cp_trap1.log
30aebe5e285829a464c3b5e6e26d448f4dbed23d6b9e675e60ece7720556738b  boots/ddk_on_img1_cp_trap1.log
f33b6a2269b42ec92c1d04ac69f8db6bb2fdd232e48fd1d04fcaf656839867ae  boots/def_on_img1_cp_trap2.log
d767e43d451d431f6ee04370438da699976ff9d95bbc3fde14a18d5cc9d48614  boots/ddk_on_img1_cp_trap2.log
fda12c713fb7c828a11654ab8ff778389b02bc68c55355263b488f557085e400  boots/def_aud_img1_cp_trap1.log
4ef8dd51344063bc2beb3a5239577dca268b98cddbef0dcf2723a48379da46e1  boots/ddk_aud_img1_cp_trap1.log
ad931085bdc6e9d4e46e88feb115259c61b5b24f9312e7244f17ae630277471d  vs_init_audio.log
59977eae37e7598b82f9c82f97118e1e4ed5ce53fad1227ec119a7108bd0a811  ddk_init_audio.log
e5fe42c847661b04ed64a64a8303e5be4cca931bdb939e2f50f6408ad57512dd  default/mashed_re.exe
968465da9339a483a06fd35f3dba9c1cb4d76825822f1381a0f26e9a3f40aec9  ddkobj/mashed_re.exe
81cc77758ffe93ac0b681eb1fed3d63912d25e7d059503af6d4f871dea9afcd8  ../AUDIO-STACK-STEP1_root/original/mashed_image.bin
```
The `boots/*.out` launcher files sit beside the logs.
