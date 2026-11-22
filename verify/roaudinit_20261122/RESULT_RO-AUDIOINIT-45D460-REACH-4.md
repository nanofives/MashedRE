# RESULT RO-AUDIOINIT-45D460-REACH-4 (run 55, G2): past 0x005C92C0, 0x005B92D0 and 0x005AE4C0, next stop 0x005B8910

Branch `orch/RO-AUDIOINIT-45D460-REACH-4`, worktree `.worktrees/RO-AUDIOINIT-45D460-REACH-4`. Created from orch/integration
`218ca9883`. `orch/RO-AUDIOINIT-45D460-REACH-3` `b49240097` merged in (`git -c merge.autoStash=false`). Before the report,
orch/integration `c353570bb` (RACE-LOAD-LUA landed) was merged again with no conflict (merge `95d76f4be`). The worker ran
on claude3, Opus 5.5 medium (moved from claude2 at the start, as the brief asked). Row kind: race-path binding row. It ran
both builds, the ddkobj check, the knob boot, the binder selftest and the RO arm. No G-CTRL, no B, no 12 captures.

## Verdict: BLOCKED (G2 not met)

**G2: 0x00604604 was not compared with ref1 at record 0.** The RO arm `RO_ROAUDINIT-pr1` on the object build
(`mashed_re_ddk.exe`, final tree) exits with code 5998864 = 0x005B8910 after 1 s. `ticks.bin` is empty (0 ticks, sha8
e3b0c442), `race.reached` is false, and `[REPLAY] init self-test: 66 checks, 0 failed`. 0x0045d460 does not return, so the
port never writes DAT_00604604. There is no record 0 to score, so `detorig_firstdiv.py` was not run.

The stop moved three times in this row (knob-on arms of both builds, then the RO arm on the same exe):

| pass | tree | stop (both builds and the RO arm) | caller (each build's map) |
|---|---|---|---|
| REACH-3 | b49240097 | 0x005C92C0 | `_PortTop_Bound_005aba20+0x278` (call eax 0x005abc6d) |
| 1 | de422fb44 | 0x005B92D0 | `_PortTop_Bound_005aa560+0xb9` (VS 0023EF69, DDK 00262EE9) |
| 2 | e6df023a1 | 0x005AE4C0 | `_PortTop_Bound_005ae3a0+0x23` (VS 0024B963, DDK 00051A43) |
| 3 | 8b0fd24de | 0x005B8910 | `_PortTop_Bound_005aa560+0x18a` (VS 0023EFFA, DDK 00262F8A) |
| final | 95d76f4be (merged head c353570bb) | 0x005B8910 | `_PortTop_Bound_005aa560+0x18a` (VS 0023EFFA, DDK 0026D5BA) |

**Why it stops here.** 0x005b8910 (715 B, `re/console/pc_funcs.csv` row 6186) has no hooks.csv row and no re/match
source. 0x005b8680 stores its address at +0x28 (`re/match/s27/005b8680.c` line 52). Its Ghidra decompilation calls
0x005b8c40 on every return path except the early `return 0`, which is taken when 0x005c6bb0 returns 0
(`D:/MashedWT/RO-AUDIOINIT-45D460-REACH-4_data/decomp_005b8910.txt`). 0x005b8c40 (1676 B) is C2 with a source
(`re/match/s27/005b8c40.c`) and is a named stop in the plan. A draft source of 0x005b8910 written from the decompilation
scores 71.0% (164/176 ins). It is not committed: the draft is `D:/MashedWT/RO-AUDIOINIT-45D460-REACH-4_data/draft_005b8910.c`.
Binding it would only move the stop to 0x005b8c40, which needs an equiv run, a listing check of 1676 B and a `prov_bind`
line. That did not fit in this row.

## 1. 0x005a7b60 boot class (REPLAY-005A7B60-BOOTCLASS, done first)

- The PROV-INSITU-REF1 run `b2_032830_018_1` (source `re/match/s25/005a7b60.c`, git blob
  `1024e60ff2a20e58306d92fdbee9cd107189473d`, the same blob on this branch) was re-scored against the same-boot-class
  no-body pair c0 / c1 of that row. That is the `rescore_c0c1.py` method, and the class rule in `re/frida/prov_compare.py`
  lines 230-247 ("re-run or scored against a same-class reference"). The kernel32 pointer at 0x00773d60 is `7601e8c0` in
  the run, c0 and c1, and the timer word 0x007f1030 is `13419e9b` in all three.
  - b2_032830_018_1 vs c0/c1: **IDENTICAL-FOR-N 600**, `boot_class_match` True, no notes.
  - Control c0 vs c1: IDENTICAL-FOR-N 600, `boot_class_match` True.
  - The earlier verdict against ref1/ref2 (`75d8e8c0`) had the note `BOOT-CLASS mismatch ... verdict not body evidence`.
    The same-class score removes that note.
  - L-128 legs, from the PROV-INSITU-REF1 row (`verify/provinsitu_ref1_20261121/PROMOTIONS.tsv`, VERDICTS.tsv line 304):
    jmp_ok True, calls_run 2 (body executed), write-set leg `static covered`, no STUB.
- **Promotion TSV: not committed.** `c4_reclass.py batch --tsv` (dry run) on the row gives `SKIP already C4 (no write)`:
  hooks.csv already carries `c4=frida-replay` for 0x005a7b60 (PROV-INSITU-REF1 train). The TSV is kept as data:
  `D:/MashedWT/RO-AUDIOINIT-45D460-REACH-4_data/REPLAY-005A7B60-BOOTCLASS.tsv`.
- **PROVISIONAL.tsv row dropped** with `verify/replayfb8_20261121/prov_unlist.py 0x005a7b60=re/match/s25/005a7b60.c`
  (the REPLAY-FALLBACK-8 route), in commit de422fb44. ddkobj now links its DDK object. It is DIFFERENT (632 of 768 bytes,
  source 60.9%), classified as a replay-C4 body, the same class as 0x00401690.

## 2. 0x005b10a0 (explained, kept bound)

- It is bound from `re/match/s26/005b10a0.c` (force line, BIND-AUDIO-ONE-GENERATOR). ddkobj gives DIFFERENT (26 of 48
  bytes). equiv gives NOT-PROVEN call-regargs (REACH-3).
- Its replay run `b3_060610_030_2` had the same boot-class mismatch note. Re-scored against c0/c1 in the same way:
  **IDENTICAL-FOR-N 600, `boot_class_match` True**. PROMOTIONS.tsv row: jmp_ok True, calls_run 4425, write-set
  `static empty`. So it is a replay-C4 body measured on that source, and the DIFFERENT is that class, not a fault. Not
  reached in any arm of this row (no log line names 0x005b10a0 / 0x005b10d0 / 0x005b1140). It is not in PROVISIONAL.tsv.

## 3. Binds (what changed)

| rva | source | how | evidence |
|---|---|---|---|
| 0x005c92c0 | re/match/s28/005c92c0.c (new, from the listing) | c1_add row, match promotion line, `prov_bind` already in the profile | match.py `--bytes` BYTE-IDENTICAL 45 B, relocheck OK 1/0 |
| 0x005c92f0 | re/match/s28/005c92f0.c (new) | same | BYTE-IDENTICAL 23 B, relocheck OK 1/0 |
| 0x005c9310 | re/match/s28/005c9310.c (new) | same | BYTE-IDENTICAL 104 B, relocheck OK 0/0 |
| 0x005b92d0 | re/match/s27/005b92d0.c (new) | c1_add row, match promotion line (next stop after 0x005c92c0, no rules.tsv line needed) | BYTE-IDENTICAL 15 B, relocheck OK 0/0 |
| 0x005ae4c0 | re/match/s26/005ae4c0.c (existing, C3) | PROVISIONAL.tsv row **class P3**, `prov_bind 005ae4c0` in `state/audio/rules.tsv` | 35.5%. equiv.py (tool commit 1ed7ceb1a) NOT-PROVEN branch-condition at orig 0x005ae51d. Checked against the listing 0x005ae4c0..0x005ae54a. One register line (UNCERTAIN_REGISTER.md, test = in-situ replay) |
| 0x005a7b60 | (unlisted, section 1) | | |

- The three record slots are byte-identical, so they need no PROVISIONAL.tsv line. A provisional listing of a
  byte-identical, exe-included source is what the drop rule removes. The `prov_bind` lines BIND-AUDIO-ONE-GENERATOR put in
  `state/audio/rules.tsv` stay but are not needed: the planner binds them through the branch promotion TSV (`match`, 360).
- `re/console/pc_funcs.csv`: three extents added after 0x005c9230 (`0x005c92c0,0x005c92ec,45`,
  `0x005c92f0,0x005c9306,23`, `0x005c9310,0x005c9377,104`). match.py refuses an rva with no Ghidra size. 0x005c9230's own
  extent was added the same way (dd0b0f2e2).
- `state/audio/rules.tsv`: `stop_force 005c91f0` removed (left as a comment line), and `prov_bind 005ae4c0` added. The audio
  profile was regenerated with `porttop_bind.py write --profile audio` after each step, then `sync` (rsp +3, +1, +1; allowlist
  +1 ORIGTOP-ONLY line for 0x005ae4c0 against its hand body in `Util/PromoLoop_sessionB.cpp`), `bindings` (no `--step`: a
  `--step` run relabeled 85 rows and was reverted), and `externs` (0 unguarded) each time. `samesrc`: 4, all pre-existing
  (0x00407600 / 0x00411350 / 0x0044b000 / 0x0047d150, none of this row).
- Conflicts of the first merge were all generated files (`PortTop_005b10d0.cpp`, `PortTop_005b1140.cpp`, `BINDINGS.tsv`).
  The head side was taken, then regenerated with the tool. The 0x005b10a0 lines are from the head's force line. The second
  merge had no conflict, and `write --profile audio` gave 0 files changed on it.
- Promotions: `re/promotions/RO-AUDIOINIT-45D460-REACH-4.tsv`, 4 rows route `match` (0x005c92c0 / 0x005c92f0 / 0x005c9310 /
  0x005b92d0). `c4_reclass.py batch --tsv` dry run: **4 pass, 0 refused, 0 skipped**. No `--apply`.
- hooks.csv: only through `re/tools/c1_add.py` (4 rows, none -> C1, CHANGELOG lines by the tool).

## 4. UNCERTAIN_REGISTER

- REACH-3's line (the record 0x005aba20 calls through at +0xc is the one 0x005c9230 filled) is **not closed**. The
  `--watch-write` on that record +0xc was not run. The exe watch knob (`A4_RO_WATCH` / `MASHED_REPLAY_TRACE_WATCH`) takes a
  fixed VA, and the record is heap. New observation (not the test): with 0x005c92c0 bound, both knob-on arms run past
  `call eax` at 0x005abc6d into later callees (pass 1 stop is in 0x005aa560, reached after 0x005aba20). The value at +0xc was
  therefore a callable body, consistent with `PortTop_Bound_005c92c0`. That is still not a writer pc. The line stays open.
- New line: 0x005ae4c0 provisional (reading only), test = in-situ replay against a same-boot-class pair.

## 5. Binding numbers (final tree)

- **Executing or reachable on the top-down path, new in this row:** 3 executed and 2 reachable.
  - 0x005c92c0 executed (the stop left it, and the next stop is in a later caller).
  - 0x005b92d0 executed (same).
  - 0x005ae4c0 executed (same, provisional, VS2022 body in both builds: `skip:provisional`).
  - 0x005c92f0 / 0x005c9310 bound from the same record, not seen executing (no trace).
  - 0x005a7b60 now links its DDK object on the 0x0045d460 path. [UNCERTAIN] whether it ran in these arms (no counter on it).
- **Pre-bound keep-alive (`Match/PortTop_BulkRefs.cpp`):** selftest `raceload refs: 127 caller-less bound bodies` on the
  merged tree (the file is RACE-LOAD-LUA / raceload-owned, unchanged by this row).
- **Bindable left unbound, with reason:**
  - 0x005b8910: the current stop. No row or source. The draft is 71.0%, and binding it moves the stop to 0x005b8c40.
  - 0x005b8c40: C2 source, called by 0x005b8910 on every non-error path. Needs equiv, a listing check (1676 B), a
    PROVISIONAL row and a `prov_bind` line.
  - 0x005abe30 / 0x005bc050 / 0x005bc0f0 (NOT-PROVEN) and 0x005ac540 (P3): not reached, unchanged from REACH-3.

## 6. Object build (ddkobj check, final tree)

`ddkobj.py check`: **IDENTICAL 2358/2464, DIFFERENT 78, not-in-map 28**, units linked 2343/2617, refusals 19
(externalized-static-register-convention 13, wrapper-redefines-globals 6), provisional kept VS2022 255 units + 7 fns, dual
body 33.

- **Against the drain head** (BIND-AUDIO-ONE-GENERATOR merged tree, `verify/bindaudio_20261121/RESULT_BIND-AUDIO-ONE-GENERATOR.md`
  line 92: IDENTICAL 2354/2459, DIFFERENT 77, not-in-map 28. RACE-LOAD-LUA's RESULT line 101 reports its merged summary
  equal to the head's):
  - DIFFERENT +1 = **0x005a7b60**, classified as a replay-C4 body (section 1).
  - IDENTICAL +4 = 0x005c92c0 / 0x005c92f0 / 0x005c9310 / 0x005b92d0 (BYTECHECK rows IDENTICAL, 0 differing bytes).
  - Functions +5 (those 4 plus 0x005a7b60).
  - Provisional units +1 = 0x005ae4c0 (`skip:provisional`).
  - Refusals 19 = 19, dual body 33 = 33.
- **Against `log\orch\run55_headcheck\`** (`cmp_head.py`): new DIFFERENT 0x0041f590, 0x0041f770, 0x00420870, 0x0046b540,
  0x005a7b60, 0x005b10a0, and new not-in-map 0x004103a0. All except 0x005a7b60 are in the drain head's own list (the
  BIND-AUDIO-ONE-GENERATOR RESULT, lines 94-100: 0x005b10a0 is its force line, the other five come from the head). The one
  from this row is 0x005a7b60, classified above.
- **Listed unit -> 100% neighbour that fell back to its VS2022 body:** none new. The one new provisional unit,
  `PortTop_005ae4c0.cpp`, binds only 0x005ae4c0 (provisional functions in linked units: 7 = 7).

## 7. Builds, self-tests, arms

- Every build, check, selftest and arm ran after `mashed_lock.py status` (`lockwait.py` / `build_both.py` block on
  PRIORITY-HOLD or SOLO). The first build waited out the merge train's SOLO. test-slot outside, mashed_lock inside, private
  root `D:/MashedWT/RO-AUDIOINIT-45D460-REACH-4_root` (robocopy of REACH-3's root, `mashed_re.log` left out).
- Both builds, 4 times, rc 0 every time. Final: VS2022 79 s, object build 929 s. rva-lint: 656 findings, all allowlisted,
  NEW=0, ORIGTOP-HELD=9. The head check build log also shows ORIGTOP-HELD=9.
- **Binder selftest (RULE 7): PASS, 0 failures, on the final merged tree** (`bind_selftest2.txt`). Per profile: raceload
  plan IDENTICAL (bind 182 reuse 119 stop 110), audio IDENTICAL (bind 450 reuse 82 stop 79), bulk IDENTICAL (60 moved lines,
  all named in moved.tsv), raceframe IDENTICAL, renderframe IDENTICAL, gates IDENTICAL, BINDINGS.tsv IDENTICAL. Remaining
  failures: none. The head's list at the stop was FAIL 14 (audio). RACE-LOAD-LUA's RESULT names FAIL 14 as the head's own,
  and this merged tree gives 0. Also PASS 0 failures on the pre-merge tree after the first binds (`bind_selftest1.txt`).
- Knob-on arms (REACH-3's set: `MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1
  MASHED_OT_AUDIOTHREAD_STANDIN=1 MASHED_ORIG_TOP_RWMOD=1` + `MASHED_OT_AUDIOINIT=1 MASHED_OT_AUDIOTICK=1`, through
  `verify/portorigtop_20261113/audiotick_stops_arms.py vs_init_audio ddk_init_audio`):
  - Stops are listed in the Verdict table.
  - Self-tests in every pass: portstate-image PASS 0 of 8 failed, OT_CODEPTR PASS 0 of 367 failed (188 slots, 31 bound).
  - code-trap self-test pages 459/459 with the 4 fault checks ok in pass 1 (ddk arm). The final pass logs have no code-trap
    line before the stop.
- RO arm `RO_ROAUDINIT-pr1` (`run_arms.py`, `stacks/ROAUDINIT.txt`, object build): exit 0x005B8910, `[REPLAY] init
  self-test: 66 checks, 0 failed`, 0 ticks.
- The knob-off control was not re-run. This row changes nothing on the knob-off path: every change is a PortTop wrapper,
  a profile line or a tracker/data file.
- Not run: G-CTRL, B, 12 captures (race-path row), menu walk (no menu code changed), the `--watch-write` leg (section 4).

## STATE / next step

1. 0x005b8910: c1_add row, source (draft 71.0% from the decompilation, `draft_005b8910.c`), equiv, PROVISIONAL row (P3 if
   conditional), a `prov_bind` line in `state/audio/rules.tsv`.
2. 0x005b8c40: equiv on `re/match/s27/005b8c40.c`, listing check, PROVISIONAL row, `prov_bind` line. Both in one rebuild,
   since 0x005b8910 calls it on every non-error path.
3. Then the knob-on arms and `RO_ROAUDINIT-pr1` again for G2 (0x00604604 vs ref1 at record 0).
4. The REACH-3 register line needs a writer watch on a heap record. The exe watch takes a fixed VA, so it needs either a
   first pass that logs the record VA or a relative watch knob (a harness row, not a binding row).

## Raw data (`D:/MashedWT/RO-AUDIOINIT-45D460-REACH-4_data/`, full sha256 in `sha256.txt` there)

First 16 hex: `rescore_5a7b60.json` 8004c55bd0615db4, `rescore_5b10a0.json` 5e2b3c1179af89bb, `equiv_005ae4c0.txt`
d024295d8f470faf, `bind_selftest1.txt` ba977604fa71e534, `bind_selftest2.txt` 24aea1606bc77da7, `ddkcheck1.log`
8029200bcb64f817, `ddkcheck4.log` c31263dda3b8ae5e, `build4_vs.log` 63ab245cf028d352, `build4_ddk.log` c51874a3adfdcbfd,
`arms_knob.out` 4cb9c3619020f5ac, `roarm.out` 101df98503a0ffc3, `vs_init_audio.log` 15a35878710f70f8, `ddk_init_audio.log`
fb76b7df1010b88d, `ro/RO_ROAUDINIT-pr1/meta.json` dbef1e8e361227a0, `ro/RO_ROAUDINIT-pr1/ticks.bin` e3b0c44298fc1c14 (empty),
`ro/RO_ROAUDINIT-pr1.log` cee012f52769941e, `pass1/arms_knob.out` d7c251f3410a9b35, `pass2/arms_knob.out` 3c06bd63d842f308,
`pass3/arms_knob.out` dd801aceef96cc81, `decomp_005b8910.txt` f81e434a1bb7ce9f, `draft_005b8910.c` 73c4074bce89cff2,
`REPLAY-005A7B60-BOOTCLASS.tsv` 3e47b977a9c18f10, `lockstatus.txt` c9e476a6459ef31c. Helper scripts in the same folder:
`listing_r4.py`, `mk_sources.py`, `mk_source2.py`, `mk_source3.py`, `add_funcs.py`, `add_prov_5ae4c0.py`, `add_reg.py`,
`drop_stopforce.py`, `rescore_5a7b60.py`, `build_both.py`, `lockwait.py`, `arms_driver.py`, `mapsym.py`, `cmp_head.py`, `waitfor.py`.
