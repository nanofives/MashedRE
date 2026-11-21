# RESULT RO-AUDIOINIT-45D460-REACH-3 (run 54, G3): past 0x005ABA20, next stop 0x005C92C0

Branch `orch/RO-AUDIOINIT-45D460-REACH-3`: a new worktree from `e1b17eba3` (BIND-BULK-PROFILE merged), with
`orch/RO-AUDIOINIT-45D460-REACH-2` `a61b25f9e` merged in (`git -c merge.autoStash=false`, no conflict, merge commit
`e816664c2`). The worker ran on claude3, Opus 5.5 medium. Row kind: race-path (both builds, ddkobj check, knob boot,
self-test, replay oracle on the object build). No G-CTRL, no B, no 12x4.

## Verdict: BLOCKED (G3 not met: the next stop needs a source and a generator entry)

- **0x005c91f0 -> 0x005aba20 now runs.** On the merged tree, the bulk wrapper `Match/PortTop_005c91f0.cpp` line 9
  maps `FUN_005aba20` to `PortTop_Bound_005aba20` (BIND-BULK-PROFILE's `bound_wins`). Both knob-on arms now get past
  the REACH-2 stop 0x005ABA20.
- **Next stop: 0x005C92C0, in both builds and in the RO arm.** All three runs log
  `[OT_TIMER] STOP: the matched 0x00000000 called original 0x005C92C0 (DAT_006904e8 = 0, DAT_006904ec = 0)`.
  The return address is 0x0024A2A8 (VS2022) and 0x00250F78 (object build). In each build's own map that address is
  `_PortTop_Bound_005aba20+0x278`, so the caller is the 0x005aba20 body. In the original, the only indirect call in
  0x005aba20 is `call eax` at `0x005abc6d`. The source (`re/match/s25/005aba20.c` line 158) loads it from
  `*(*(param_6+0xc)+0xc)`.
  - The exe stop thunk `PortTop_AStop_005c92c0` is named in only one place: `Match/PortTop_005c9230.cpp` line 15
    (`#define FUN_005c92c0 PortTop_AStop_005c92c0`). `re/match/s28/005c9230.c` stores `FUN_005c92c0`, `FUN_005c92f0`,
    `FUN_005c9310` and `FUN_005aae60` at record +0xc / +0x10 / +0x14 / +0x18. 0x005c9230 is bound through code-pointer
    slot 0x006351e0 (`CODEPTR_SLOTS.tsv` row 179, table 0x006351dc, walker 0x005bbb20). [UNCERTAIN] that the record
    0x005aba20 reads at +0xc is the one 0x005c9230 filled: this rests on the thunk name and the sources, not on a
    watch. Test: `--watch-write` on that record's +0xc. Logged in `re/orchestrator/UNCERTAIN_REGISTER.md`.
  - **0x005c92c0 has no hooks.csv row, no `re/match` source, and is in no run 53/54 bind list or gate rank.**
    Listing `0x005c92c0..0x005c92ec` (capstone on `original/MASHED.exe.unpatched`):
    `mov eax,[esp+8]`; `push esi`; `mov esi,[esp+8]`; `push 0x30806`; `push 1`; `mov ecx,[esi+0x34]`;
    `mov [esi+0x50],eax`; `add ecx,2`; `push ecx`; `call 0x005aea50`; `add esp,0xc`; `mov [esi+0x4c],eax`;
    `neg eax`; `sbb eax,eax`; `and eax,esi`; `pop esi`; `ret`. 0x005c92f0 (`mov [eax+0x50],0` ... `jmp 0x004522d0`)
    and 0x005c9310 are the next two record slots. Neither is reached yet.
  - **Why this row stops here.** Binding 0x005c92c0 needs a hooks.csv C1 row (the `c1_add.py` operation of
    re-classify), a new source from the listing, its PROVISIONAL.tsv row, and a `PROV_BIND` line in
    `verify/portorigtop_20261113/tools/audiotick_bind_gen.py` (or a `prov_bind` line in the audio profile). The brief
    counts a new force / prov entry as a binder edit, so this row stops and reports. The same pattern (code pointers
    stored by one body, new sources from the listings) is what REACH checkpoint 6 did for 0x005bb850's four pointers.
- **G3: 0x00604604 is not compared with ref1 at record 0.** The RO arm `RO_ROAUDINIT-pr1` on the object build
  (`mashed_re_ddk.exe` 53b919d0) exits with code 6066880 = 0x5C92C0 after 2 s: `ticks.bin` is empty (0 ticks,
  sha8 e3b0c442), `race.reached` false, and `[REPLAY] init self-test: 66 checks, 0 failed`. 0x0045d460 does not
  return, so DAT_00604604 / DAT_0068fc84 are never written by the port. There is no record 0 to score, so
  `detorig_firstdiv.py` was not run.
- **The stops 0x005abe30 / 0x005ac540 / 0x005bc050 / 0x005bc0f0 are not reached** (no log line in any arm). Each one is
  `stop` in the plan. equiv.py (tool commit 1ed7ceb1a, no diff on this branch) on each source:

  | rva | source | verdict |
  |---|---|---|
  | 0x005abe30 | re/match/s25/005abe30.c | NOT-PROVEN call-sequence: region ours +0x3f / orig 0x005abe7f: 2 vs 0 calls |
  | 0x005ac540 | re/match/s25/005ac540.c | EQUIV-IF-CALLERS-IGNORE(EAX,ECX) [VALUE-FLOW], callers 1, scan 0/1 ignore: 0x005bb981 reads/unknown |
  | 0x005bc050 | re/match/s27/005bc050.c | NOT-PROVEN call-target at region +0x0 (orig 0x005bc050) |
  | 0x005bc0f0 | re/match/s27/005bc0f0.c | NOT-PROVEN call-target at region +0x0 (orig 0x005bc0f0) |

  None of these is an admission. 0x005ac540 is class P3 (a conditional verdict with a reading or unknown caller). It
  gets a PROVISIONAL.tsv listing in the step that binds it. That step also needs its `PROV_BIND` line, a binder
  entry, so it was not listed here, because the listing alone would bind nothing. Full verdict lines:
  `D:/MashedWT/RO-AUDIOINIT-45D460-REACH-3_data/equiv_<rva>.txt`.

## 0x005a7b60: bound from the replay-verified source, PROVISIONAL.tsv row text brought in line

- The audio plan lists `005a7b60 bind re/match/s25/005a7b60.c C4` (kind replay: `msrc()` takes the one `re/match`
  source of a `c4=frida-replay` row). That file is git blob `1024e60ff2a20e58306d92fdbee9cd107189473d` on this branch,
  the same blob as the PROV-INSITU-REF1 replay `b2_032830_018_1` linked.
- `re/match/PROVISIONAL.tsv` row 0x005a7b60 (key rva + source): column `replay_prov_insitu` was
  `CRASH (PROV-INSITU; first fault in verify/provcrash_20261112/CRASH_VERDICTS.tsv)`. It now reads
  `IDENTICAL-FOR-600 (run b2_032830_018_1, PROV-INSITU-REF1 run 53, ref1, menu 300 + race 300, ...)`, with the
  verdict JSON's caveat and a note that the CRASH predates the PROV-FIX-9 fix (0f7b2e81f). The script is
  `D:/MashedWT/RO-AUDIOINIT-45D460-REACH-3_data/prov_text_5a7b60.py`. It changes one field, and status stays
  `provisional`.
- **Not dropped.** The row is already C4 on the head (PROV-INSITU-REF1 train), so this row has no promotion TSV and
  no drop step. The verdict JSON (`D:/MashedWT/PROV-INSITU-REF1_data/b2_032830_018_1.verdict.json`) carries the note
  `BOOT-CLASS mismatch {run 0x773d60 7601e8c0, ref 75d8e8c0}: verdict not body evidence`. With that note, unlisting
  the row as a "replay-C4 body" (the REPLAY-FALLBACK-8 `prov_unlist.py` route) would rest on a verdict the harness
  itself marks as not body evidence. Left for the lead and L0: re-run 0x005a7b60 on a matching boot class, then
  unlist. 29 of the 139 `c4=frida-replay` rows on the head are still listed `provisional` (count from hooks.csv x
  PROVISIONAL.tsv on this tree), so "dropped as for the 135" is not what the head shows for them either.
- Object build: 0x005a7b60 stays `skip:provisional` (VS2022 body), the same as at the head.

## 0x005b10a0: why it reuses the hand body AudioTableDispatchAt4, and what resolves it

- **Mechanism.** `audiotick_bind_gen.py` `reuse()` (lines 92-112) works like this. `msrc('005b10a0')` returns
  `re/match/s26/005b10a0.c` (the row is C4 `c4=frida-replay` with one re/match hit). That source names no `FUN_`
  callee (one indirect call `e->fn(...)`). The exe index has no match-include body for 0x005b10a0, and the row is
  C4, so `reuse()` takes the index's `rh` symbol: `AudioTableDispatchAt4`, `Audio/AudioTableDispatch.cpp`. The plan
  line is `005b10a0 reuse AudioTableDispatchAt4 C4`. The binder's audio profile
  (`porttop_bind.py plan --profile audio`) gives the same line, so both audio generators agree.
  `Match/PortTop_005b10d0.cpp` and `Match/PortTop_005b1140.cpp` map `FUN_005b10a0` to it. This reached the head
  side through REACH-2's regenerate, and its own plan dry run on the unedited head gave it too.
- **Evidence status.** The replay C4 (IDENTICAL-FOR-600, run `b3_060610_030_2`, write-set static empty,
  `verify/provinsitu_ref1_20261121/PROMOTIONS.tsv` row 12) was measured on `re/match/s26/005b10a0.c`, **not** on the
  hand body. The hand body's own evidence is the C3 Frida A/B `log/diff_scenario_batch_audio_table_dispatch_at4.csv`
  GREEN-4/4-NONDEGEN (hooks.csv note). That is neither reader evidence nor an equiv verdict. equiv.py on the s26
  source: `NOT-PROVEN call-regargs: region ours +0x0 / orig 0x005b10a0` (ECX / EDX at the indirect call differ, the
  same class as the 0x005a6d60 PROVISIONAL row).
- **Resolution, not applied here.** The replay-verified body binds when 0x005b10a0 is planned as `bind`. That takes
  `force 005b10a0` in `re/tools/bind/state/audio/rules.tsv`, or the same rva in `audiotick_bind_gen.py`'s `FORCE`.
  A named stop takes `stop_force`. Each is a new force / stop entry, which the brief names as a binder edit: stop and
  report. The reuse is **not reached** in any arm of this row: no log line names 0x005b10d0, 0x005b1140 or
  0x005b10a0. The two audio generators are the open question. The lineage is generated by `audiotick_bind_gen.py`,
  while the porttop_bind audio profile's `plan.json` is stale (selftest below). So the force line takes effect only
  once one of them is the audio generator.

## 1. What changed (files)

- From REACH-2 (merged, regenerated, byte-identical): `Match/PortTop_005bb910.cpp` (`FUN_005c91f0` ->
  `PortTop_Bound_005c91f0`), `Match/PortTop_AudioStops.cpp` (-2 stops), `Match/PortTop_BulkRefs.cpp` (-1 anchor),
  `Match/PortTop_005b10d0.cpp` / `_005b1140.cpp` (see 0x005b10a0), `audiotick_bind_gen.py` (STOP_FORCE 0x005c91f0
  line commented out), `exeidx.json`, `plan.json`, `BINDINGS.tsv`.
- **Regenerate on the merged tree (RULE 3), 0 files changed by each step:**
  - `exe_body_index.py` (2417 rvas from 2558 files).
  - `audiotick_bind_gen.py ... --write` (roots `D:/MashedWT/PORT-TOP-AUDIO-5BA1D0-STOPS_data/roots_arg.txt`; the
    listing `plan_write.txt` has the same sha256 as REACH-2's `plan1_write.txt`, bc33f487...).
  - `porttop_bind.py write --profile bulk` (`reuse 216, stop 192, bind 97`, "0 files changed").
  - `porttop_bind.py refs --profile raceload` (131 anchors, "0 files changed").
  - `sync_rsp_allowlist.py` (rsp + 0, allowlist + 0; it prints the 3 known `UNTAGGABLE ORIGTOP-HELD` seed lines that
    BIND-BULK-PROFILE section 4 explains).
  - `bindings_update.py` (updated 84, added 0, no diff).
  - `codeptr_inventory.py --write` (no diff).
  - After line endings were ignored, the tree differed from the merge commit only in this row's edit.
- This row: `re/match/PROVISIONAL.tsv` (one field of row 0x005a7b60),
  `re/orchestrator/UNCERTAIN_REGISTER.md` (+1 line), this RESULT.
- No edit of `re/tools/bind/porttop_bind.py`, no new force / alias / stop / prov / extra entry, no tracker edit, no
  promotion TSV.

## 2. Binding numbers

- **Executing or reachable from a bound caller on the top-down path:** +2 against the e1b17eba3 head, where the arm
  stops at 0x005C91F0 (REACH-2's STOP_FORCE).
  - 0x005c91f0 (reuse of the bulk body, as in REACH-2).
  - 0x005aba20 (`PortTop_Bound_005aba20`, provisional, runs up to its `call eax` at 0x005abc6d, by the stop's
    return address).
  - The 0x005aba20 callees before 0x005abc6d on the path taken are not counted: no trace. [UNCERTAIN] which of
    0x005acd10 / 0x005ac980 / 0x005ac5f0 / 0x005b3b60 / 0x005ae650 / 0x005ae800 / 0x005ae0b0 / 0x005aea00 ran.
- **Pre-bound (keep-alive only, `Match/PortTop_BulkRefs.cpp`):** 131 (head 132; -1 = 0x005c91f0, from REACH-2).
- **Bindable left unbound, with reason:**
  - 0x005c92c0: reached; no row and no source; needs c1_add + a source from the listing + PROVISIONAL + a PROV_BIND
    entry, which is a binder entry.
  - 0x005c92f0 and 0x005c9310: the same record, not reached, same needs.
  - 0x005abe30 / 0x005bc050 / 0x005bc0f0: not reached, NOT-PROVEN, PROVISIONAL + PROV_BIND needed.
  - 0x005ac540: not reached, P3, PROVISIONAL + PROV_BIND needed.
  - 0x005b10a0: reuse of the hand body; the replay body needs a force entry; not reached.

## 3. Object build (ddkobj check)

`ddkobj.py check` (after `mashed_lock.py status`, RULE 4): **IDENTICAL 1978/2037, DIFFERENT 33, not-in-map 26**, units
linked 1967/2231, refusals 11 (externalized-static-register-convention 7, wrapper-redefines-globals 4), provisional kept
VS2022 253 units + 6 fns, **dual body 32**.

- **Against the e1b17eba3 head** (merge train, `log/orch/run54_drain3.log` line 28): every number and the list of 33
  DIFFERENT rvas are equal. Newly DIFFERENT: none. Newly not-in-map: none.
- **Against `log/orch/run54_headcheck/`** (object build of 77dedfece, `cmp_head.py`), each delta comes from the head
  side:
  - DIFFERENT +1 = 0x00401690. REPLAY-FALLBACK-8 `092fd29da` unlisted it as a "replay-C4 body" (source
    `re/match/s01/00401690.c`, the replay-verified file), so it is classified.
  - not-in-map -1 = 0x00543bb0 (BIND-BULK-PROFILE: now IDENTICAL, 400 B).
  - IDENTICAL +2, functions +2, units linked +1, externalized-static refusals 8 -> 7: the same head-side commits.
  - dual body 32 = 32, same rvas. Provisional skips 253 = 253.
- **Listed unit -> 100% neighbour that fell back to its VS2022 body: no new pair.** The DIFFERENT list is equal to
  the head's. This row links no new unit: 0x005a7b60 and 0x005aba20 stay `skip:provisional`.

## 4. Arms, self-tests

Private root `D:/MashedWT/RO-AUDIOINIT-45D460-REACH-3_root` (robocopy of REACH-2's root, `mashed_re.log` left out).
Every build, check, selftest and arm ran after `mashed_lock.py status` through `lockwait.py` / `build_both.py`, which
block on PRIORITY-HOLD or SOLO. `lockstatus.txt` has 7 checks, none showed PRIORITY-HOLD or SOLO (the lock was HELD by
PROV-INSITU-REF1-WS2 or free). test-slot outside, mashed_lock inside.

Knob-on command: `ARMS_STEP=RO-AUDIOINIT-45D460-REACH-3 ARMS_ROOT=<root> ARMS_EXTRA="MASHED_PORTSTATE_IMAGE=1
MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1 MASHED_OT_AUDIOTHREAD_STANDIN=1 MASHED_ORIG_TOP_RWMOD=1" test-slot py -3.12
lockwait.py ... -- py -3.12 <ORCH>/re/orchestrator/mashed_lock.py run --label RO-AUDIOINIT-45D460-REACH-3 -- py -3.12
verify/portorigtop_20261113/audiotick_stops_arms.py vs_init_audio | ddk_init_audio`.

| arm | exe | stop | return address |
|---|---|---|---|
| REACH-2 (previous) | both | 0x005ABA20 (bulk stop, caller 0x005c91f0) | 00253CBF / 00058B83 |
| **vs_init_audio** | mashed_re.exe 29d6c997 | **0x005C92C0** | 0024A2A8 = `_PortTop_Bound_005aba20+0x278` |
| **ddk_init_audio** | mashed_re_ddk.exe 53b919d0 | **0x005C92C0** | 00250F78 = `_PortTop_Bound_005aba20+0x278` |
| **RO_ROAUDINIT-pr1** (object build) | mashed_re_ddk.exe 53b919d0 | 0x005C92C0 (exit 6066880), 0 ticks | 00250F78 |

- The knob-off control was not re-run. This row changes nothing on the knob-off path, and REACH-2's control (base stop
  0x005AEF30) holds for the same generated sources (regenerate gave 0 diffs).
- **Self-test result: PASS** in all three runs: portstate-image 0 of 8 failed, OT_CODEPTR 0 of 367 failed (188 slots,
  31 bound), code-trap 0 of 6 failed (pages 459/459, both knob-on arms), RO arm `[REPLAY] init self-test: 66 checks,
  0 failed`.
- Both builds: `build_both.py` gave VS2022 rc 0 (281 s) and object build rc 0 (600 s). rva-lint 465 findings, all
  allowlisted, NEW=0, ORIGTOP-HELD=0, in both logs.
- **`porttop_bind.py selftest`: FAIL 17.** This is BIND-BULK-PROFILE's 19 minus 2: `Match/PortTop_005b10d0.cpp` and
  `_005b1140.cpp` now equal what the audio profile writes. The raceload refs line reads 131 for 132. The other 17
  lines are the stale raceload / audio `plan.json` lines BIND-PROFILE-DATA section 3 listed, so none is new.
  **Merge bar (run 54): a binding row merges only with selftest 0.** This row does not refresh those two profiles'
  plans: that rewrites 17 wrappers of the raceload and audio lineages, and the audio one with the other generator.
- Gates not run (race-path row in the worker). Menu walk not run (no menu code changed).

## 5. Next step (for the lead / L0)

1. **Binder or data row for the audio record slots of 0x005c9230:** c1_add rows for 0x005c92c0 / 0x005c92f0 /
   0x005c9310, sources from the listings (0x005c92c0 is 17 instructions, cited above), PROVISIONAL rows, and the
   prov_bind lines. Then rebuild, the knob-on arms, and the RO arm for G3.
2. **One audio generator.** Either `audiotick_bind_gen.py` stays and takes the force / prov lines, or the lineage moves
   to `porttop_bind.py --profile audio`. With the profile, the 13 + 4 stale plan lines clear and selftest can reach 0.
   Then `force 005b10a0` binds the replay-verified s26 body instead of the hand body.
3. 0x005a7b60: replay on a matching boot class before it is unlisted as a replay-C4 body.
4. Then the stops 0x005abe30 / 0x005ac540 / 0x005bc050 / 0x005bc0f0 as they are reached (equiv table in the Verdict section).

## Raw data (`D:/MashedWT/RO-AUDIOINIT-45D460-REACH-3_data/`, full sha256 in `sha256.txt` there)

First 16 hex: `vs_init_audio.log` 77a2c57d7262acbc, `ddk_init_audio.log` cd1e7ca9ebcca7d3, `arms1on_vs.out`
0fa501b0adf5b1ba, `arms1on_ddk.out` 630eca9e553895f8, `roarm.out` eea3071ae6f10481, `ro/RO_ROAUDINIT-pr1/meta.json`
3902ddac34267bd7, `ro/RO_ROAUDINIT-pr1/ticks.bin` e3b0c44298fc1c14 (empty), `ddkcheck1.log` 0b68efbd8d027f64,
`bytecheck1.tsv` 137b840fb32dc4ee, `ddksummary1.json` c107063d210a9017, `bind_selftest.txt` e61890c964ae8c48,
`build1_vs.log` 89b74262ce29a6df, `build1_ddk.log` cfe21e8bab650cfb, `plan_write.txt` bc33f487b5ab4e56,
`bulk_write.txt` 781123f0ea600c52, `refs_write.txt` 9ecc99998b3674d6, `equiv_005abe30.txt` e4eb7c374818e6bd,
`equiv_005ac540.txt` 2aacb7940ad60d6b, `equiv_005bc050.txt` 9dd0ed9b31a110c9, `equiv_005bc0f0.txt` e37576599d0410d6,
`equiv_005b10a0.txt` 3f629a99a457674a, `lockstatus.txt` c7e4ea3320800740, `prov_text_5a7b60.py` 286503a6be7495b7,
`lockwait.py` 69d952c3391f1968, `build_both.py` a8dc9613f83cd2f4. Root log
`D:/MashedWT/RO-AUDIOINIT-45D460-REACH-3_root/mashed_re.log` 9561691037aa79db.
