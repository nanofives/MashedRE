# RESULT PORT-TOP-AUDIO-5AEF30-KNOB: 0x005aef30 knob stand-in, then the init-arm stops toward 0x0045d460

## Verdict: HANDOFF (knob part GREEN; the arms pass census 42..69 and stop at 0x005BA1D0, census 71; 0x0045d460 not reached)

- **Self-test result: PASS** in every arm, both builds, every checkpoint and the merged tree (portstate-image 0 of 8 failed,
  OT_CODEPTR 0 of 367 failed, code-trap 0 of 6 failed, pages 459/459).
- **Knob** `MASHED_OT_AUDIOTHREAD_STANDIN` (default off, PREREG `PREREG_PORT-TOP-AUDIO-5AEF30-KNOB.md`, committed alone at
  91351fc0c). Knob off, merged tree: both builds stop at **0x005AEF30** at the base offsets, `_PortTop_Bound_005a8060+0x18c`
  (VS2022) and `+0x2cd` (object build), as on RESULT_PORT-TOP-AUDIO-5A8F30-STOPS 3.2. No default-path TU changed (every new TU
  is `RVA-LINT: ORIGTOP-ONLY` and runs only under `MASHED_ORIG_TOP` + `MASHED_OT_AUDIOINIT`).
- Knob on (+ the existing `MASHED_ORIG_TOP_RWMOD=1`, section 1.3), merged tree: both builds stop at **0x005BA1D0**, return
  address `_PortTop_Bound_005aa560+0x18a` in both maps.
- **RO-AUDIOINIT-45D460-REACH: not answered.** 0x0045d460 is not reached, so DAT_00604604 after it and the oracle with
  `MASHED_OT_AUDIOINIT` on the RO arm were not run. Remaining scope: section 4.
- Object build: DIFFERENT 28 = the base's 25 (fresh base check of 0917bc3d0, `bytecheck0_base.tsv`) + 0x005bce80 and 0x005bbdb0
  (admitted, below 100% by definition) + 0x005ae3a0 (C4 by frida-replay, source 84.8%, newly reached and bound). No
  unexplained DIFFERENT unit.
- Gates: not run (top-down row). No menu walk (no menu code changed). equiv.py = tool commit 1ed7ceb1a, no diff on the branch
  or the merge.

## 1. Checkpoint 1 (7bef1d5e9): the knob

### 1.1 Stand-in
`mashedmod/src/mashed_re/Match/PortTop_Knob_005aef30.cpp`, `PortTop_Knob_005aef30(param_1, param_2)`; the generator
(`tools/audiotick_bind_gen.py`, `KNOB`) maps the callee `FUN_005aef30` of a bound wrapper to it (today only
`Match/PortTop_005a8060.cpp`). Knob off it calls `PortTop_AudioCalleeStop(0x005aef30, 0, _ReturnAddress())`, the same call as the
stop thunk, so the stop and its return address are unchanged. Knob on it returns param_1, starts no thread, writes nothing.
Evidence (listing of `MASHED.exe.unpatched`) is in the PREREG: the caller 0x005a8060 tests the return only for zero
(0x005a8330 / 0x005a8332 -> 0x005a8346); its caller 0x005a7fe0 does not read 0x005a8060's eax (0x005a801a then the pushes at
0x005a801f..0x005a802e). The +8 store (0x005aef3d) is left out: its reader is the thread entry 0x005aef70, and the bound caller
passes 0 (0x005a8321) over a +8 that 0x005aef00 zeroed (0x005aef19).

Log line on the knob-on arm: `rva 0x005aef30: stand-in ... returns param_1 007DCB54, no thread, param_2 00000000 not stored, desc
+0 00000000 +4 00239860 +8 00000000 +c 00000002 +10 00000000`. Known memory differences against the original with the knob on:
0x007dcb54 stays 0 (the original stores the thread handle there, 0x005aef5a), and 0x007dcb58 holds the exe address of the
0x005a84d0 stop thunk (`_PortTop_AStop_005a84d0`) where the original holds 0x005a84d0 (0x005aef0c of the pushed 0x005a84d0).
[UNCERTAIN] line PORT-TOP-AUDIO-5AEF30-KNOB (descriptor readers) in `re/orchestrator/UNCERTAIN_REGISTER.md`.

### 1.2 0x005bce80 (census 53): admitted
Seven source variants stayed at 69.6% (the original holds 0x7b8 in eax across both +0x40 stores, 0x005bce92 / 0x005bceaf; the
DDK folds it). `equiv.py`: `EQUIV-IF-CALLERS-IGNORE(EAX,ECX) [REG,VALUE-FLOW] callers 1, scan says ignore 1/1: 0x005bce4e`.
equiv_dyn cross-check with eax,ecx ignored: default SAME 32/32, seeded SAME 32/32. ADMITTED.tsv row. Bound with its stored
code pointer 0x005bd9e0 (C4 match).

### 1.3 Knob on: 0x00551330 needs the RWMOD record slots
Knob on with the base knob set, both builds passed 0x005aef30 and then faulted in `_PortTop_Bound_00551330` (+0x9f VS2022 /
+0x94 object): `call [RwGlobals+0xf4]` with a 0 slot (object build: slot 0x0110B414 = record 0x0110B320 + 0xf4). Path: 0x004627f0
-> 0x004624c0 -> 0x00551190 -> 0x00551330 (xref.py callers). The librw RwGlobals record (`LibRw/RwApiBind.cpp`,
`MashedRw_InstallRwGlobals`) fills +0xf4 only under the existing default-off knob `MASHED_ORIG_TOP_RWMOD=1`
(`RegisterRwEarlySlots`, `Put(0xf4, ...)`). With it set the log prints `0x0040bbb0 printf: RtFSManagerRegister successful` and the
arms reach 0x005BAD30. All later knob-on arms set `MASHED_ORIG_TOP_RWMOD=1`. No code change for this.

## 2. Checkpoints 2 and 3 (2c7d4b78f): census 58 to 69

| rva | census | source | result | status |
|---|---|---|---|---|
| 0x005bad30 | 58 | re/match/s27/005bad30.c, rewritten from the listing (Ghidra's frame was wrong: unaff_EBP / stack0xffffff70) | 33.8% -> 94.4% (142/142 ins, 16 bytes: the two 16-bit clamps 0x005badd4 / 0x005bae02 keep the 0x80 block as the fall-through) | equiv NOT-PROVEN (call-target tool limit at the import call 0x005bae39): PROVISIONAL.tsv, bound |
| 0x005c1920 | 59 | import thunk `jmp [0x5cc01c]`, IAT slot = DSOUND.dll ordinal 11 (pefile) | exe body `Match/PortTop_Imp_005c1920.cpp` resolves ordinal 11 by LoadLibrary / GetProcAddress (the exe has no import of it); stops if missing | generator ALIAS, not C4 (hooks.csv lists the row as CRT `waived-platform-crt`; not changed) |
| 0x005bbdb0 | 60 | re/match/s27/005bbdb0.c, rewritten from the listing (two stack blocks, 0x12 bytes memset at [esp], 0x24 bytes at +0x14) | 18.8% -> 88.2% (85/85 ins, eax / edx swap) | equiv `EQUIV [REG,VALUE-FLOW]`, xcheck default SAME 32, seeded INCONCLUSIVE 32: ADMITTED.tsv |
| 0x005bac00 | 62 | re/match/s27/005bac00.c, rewritten (12-dword block, float fields for the immediates) | 59.0% -> 100.0%, 294 B BYTE-IDENTICAL, relocheck OK checked=6 | promotion TSV |
| 0x005aac00 | 66 | re/match/s25/005aac00.c (new; no Ghidra function, pc_funcs 0x005aac00..0x005aac17) | 100.0%, 24 B BYTE-IDENTICAL, relocheck OK checked=0 | hooks.csv C1 row through `re/tools/c1_add.py`, promotion TSV |
| 0x005a9e60 | 68 | re/match/s25/005a9e60.c, the +0x48 return reads volatile (every path stores +0x48 and reads it back: 0x005a9ea8 / 0x005a9eb6, 0x005a9ed6 / 0x005a9ee4, 0x005a9eef / 0x005a9ef6) | 75.4% -> 100.0%, 161 B BYTE-IDENTICAL, relocheck OK checked=2 | promotion TSV |
| 0x005a9f10 | 69 | existing PROVISIONAL.tsv row (81.0%, NOT-PROVEN, PROV-FIX-9) | unchanged | PROV_BIND, bound |

Not reached and left as stops: 0x005baa60 (called by 0x005bac00 only when the property query returns 3; not in the census
window), 0x005ae250 (census 74, 64.9%, equiv `NOT-PROVEN call-regargs` at 0x005ae2cd; it becomes a provisional when reached).
Bound by the closure with no work: 0x005ae300, 0x005ae470 (C4 match), 0x005ae3a0 (C4 frida-replay).

Promotions: `re/promotions/PORT-TOP-AUDIO-5AEF30-KNOB.tsv`, 3 rows (0x005bac00, 0x005aac00, 0x005a9e60), dry run `batch: 3 pass,
0 refused` (`c4_dry2.txt` was 2 of 2 before 0x005a9e60 was added; re-run on the merged tree in section 3). No `--apply`.
Previous sources in the data folder (`<rva>_before.c`).

Arms: checkpoint 2 stopped at **0x005AAC00** (`_PortTop_Bound_005adef0+0x23` VS2022 / `+0x20` object), checkpoint 3 at
**0x005BA1D0** (`_PortTop_Bound_005aa560+0x18a`), both builds, self-tests PASS.

## 3. Merge of orch/integration (5205c3771)
- `git -c merge.autoStash=false merge orch/integration` (c447fddd3, 10 commits past 0917bc3d0, incl. PORT-UNBLOCK-RWGLOBALS
  and RACE-COURSELUA-BODIES), no conflicts. equiv.py, match.py and re/tools/ddkobj: no diff.
- Plan on the merged tree: 384 bind (328 match, 28 provisional, 19 admitted, 9 replay), 71 reuse, 88 stops, no new TU.
- VS2022 Build OK, rva-lint 381 known, NEW 0. Object build OK. `ddkobj.py check`: IDENTICAL 1765/1812, DIFFERENT 28 (list in
  the verdict), not-in-map 19, units linked 1759/1991, refusals 11, provisional kept VS2022 221 units + 5 fns.
- Arms (merged tree):

  | arm | knob | stop | return address | log sha256 (16) |
  |---|---|---|---|---|
  | vs_init_audio | off | 0x005AEF30 | `_PortTop_Bound_005a8060+0x18c` | 04c361d7648f4ccb |
  | ddk_init_audio | off | 0x005AEF30 | `_PortTop_Bound_005a8060+0x2cd` | 5d049753242e7d65 |
  | vs_init_audio | on + RWMOD | 0x005BA1D0 | `_PortTop_Bound_005aa560+0x18a` | e70e34b9c3a3caa5 |
  | ddk_init_audio | on + RWMOD | 0x005BA1D0 | `_PortTop_Bound_005aa560+0x18a` | 0ab4031d0d5eef69 |

## 4. Handoff: what is left to reach 0x0045d460
- **0x005ba1d0** (census 71, 1350 B, 0x005ba1d0..0x005ba716, no Ghidra caller: reached as a code pointer from 0x005aa560 at
  +0x18a; source 33.8%). Its callees (xref.py): 0x005ba720 / 0x005ba760 / 0x005bbf30 / 0x005c7990 bound or C4; 0x005bac00 /
  0x005bbdb0 / 0x005c1920 bound by this row; unbound 0x005baf00 (census 88, 82.6%), 0x005baf60, 0x005baf90, 0x005bb000 (census
  75, 42.7%, it also calls 0x005aef30: map it to the knob through the generator's `KNOB`), 0x005bbc10 (76, 44.4%), 0x005bbd50
  (77, 45.7%), 0x005bbfc0 (87, 94.1%), 0x005bc400.
- Then census 72..92 in first-call order (`git show c2e0db02d:verify/census_20261116/audio_window_65.tsv`), incl. 0x005ae250
  (74), 0x005c7d30 (79, no source), 0x005bc470 (81), 0x005bf670 (86, its `__stdcall` declaration is why 0x005bf610 is in
  STOP_FORCE), 0x005a91b0 (92, no source).
- Recipe that worked here: read the listing and rewrite from it when Ghidra's frame is wrong (0x005bad30, 0x005bbdb0, 0x005bac00
  went from 19..59% to 88..100%), float fields for immediates stored twice, volatile for read-back-after-store. Driver steps:
  generator --write, revert CR-only rewrites, `sync_rsp_allowlist.py` (ARMS_STEP set), `bindings_update.py`,
  `codeptr_inventory.py --write`, both builds, `ddkobj.py check`, arms with `MASHED_OT_AUDIOTHREAD_STANDIN=1
  MASHED_ORIG_TOP_RWMOD=1` added to ARMS_EXTRA.
- Once 0x0045d460 returns: RO-AUDIOINIT-45D460-REACH as written in the ledger (DAT_00604604 vs ref 4 at record 0; the RO arm also
  needs `MASHED_OT_AUDIOTHREAD_STANDIN=1` and `MASHED_ORIG_TOP_RWMOD=1`).

## Lock status at build/run time
Times -0300; printed by the same command right before the step.

| step | time | status |
|---|---|---|
| base VS2022 build | 16:03:10 | `free 'mashed_machine'` |
| base object build | 16:08:25 | `free 'mashed_machine'` |
| cp1 VS2022 build, first try | 16:17:20 | `HELD merge-train:20261116_160346`, `PRIORITY-HOLD` x2: no build; until-loop wait to 16:29:08 (`free`, `PRIORITY-LINGER 95s`) |
| cp1 VS2022 build | 16:30:23 | `free`, `PRIORITY-LINGER 19s more` (no HOLD, no SOLO) |
| cp1 object build | 16:31:44 | `free 'mashed_machine'` |
| cp1 arms knob off | 16:35:50 | `free 'mashed_machine'` |
| cp1 arms knob on | 16:46:01 | `free 'mashed_machine'` |
| cp1 arms knob on + RWMOD | 16:58:16 | `free 'mashed_machine'` |
| cp2 VS2022 build | 17:21:13 | `free 'mashed_machine'` |
| cp2 object build | 17:22:33 | `free 'mashed_machine'` |
| cp2 arms knob on + RWMOD | 17:26:11 | `free 'mashed_machine'` |
| cp3 VS2022 build | 17:39:43 | `free 'mashed_machine'` |
| cp3 object build | 17:40:28 | `free 'mashed_machine'` |
| cp3 arms (driver launch failed: `test-slot` not found by CreateProcess, no game started) | 17:44:07 | `free 'mashed_machine'` |
| cp3 arms knob on + RWMOD | 17:44:30 | `free 'mashed_machine'` |
| merged VS2022 build | 17:55:56 | `free 'mashed_machine'` |
| merged object build | 17:57:29 | `free 'mashed_machine'` |
| merged arms knob off | 18:01:23 | `free 'mashed_machine'` |
| merged arms knob on + RWMOD | 18:11:30 | `free 'mashed_machine'` |

## Arms command
`ARMS_STEP=PORT-TOP-AUDIO-5AEF30-KNOB ARMS_ROOT=D:/MashedWT/PORT-TOP-AUDIO-5AEF30-KNOB_root ARMS_EXTRA="MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1 [MASHED_OT_AUDIOTHREAD_STANDIN=1 MASHED_ORIG_TOP_RWMOD=1]" test-slot py -3.12 <ORCH>/re/orchestrator/mashed_lock.py run --label PORT-TOP-AUDIO-5AEF30-KNOB -- py -3.12 verify/portorigtop_20261113/audiotick_stops_arms.py vs_init_audio ddk_init_audio`
Private root: robocopy of `D:/MashedWT/PORT-TOP-AUDIO-5A8F30-STOPS_root`, old `mashed_re.log` removed.

## Raw data (`D:/MashedWT/PORT-TOP-AUDIO-5AEF30-KNOB_data/`, sha256 in `sha256.txt` there; first 16 hex here)
- arms logs: `cp1off/` vs 8b8a6735fa1f23fa ddk bab0e6e5b3388793; `cp1on/` vs 3a4d9aa45ef3e182 ddk 134e658340db22df; `cp1onrw/` vs
  8e4731632930b00b ddk 9d60e58eb4893aa0; `cp2on/` vs 12992bb4be203816 ddk c7ceeba2644e6c13; `cp3_on/` vs 41ab9b8a2c51d1fd ddk
  f79717f2ea323291; `cp4_off/` and `cp4_on/` in section 3; `arms_cp4_off.out` 207676537182d646, `arms_cp4_on.out` 517c1b4a7486fd88
- object check: `bytecheck0_base.tsv` cd20cb05ebf2bdb4, `ddkcheck0.log` 6ba6a458069b3ca5, `bytecheck_cp4.tsv` c1589114b7d80937,
  `ddkcheck_cp4.log` fb778841fc05c7b4
- equiv: `equiv_005bce80.txt` dd4cf9dbfc68a703, `equiv_005bad30.txt` bba3a3d6651d1b79, `equiv_005bbdb0.txt` 069fa33eb44be29e,
  `equiv_005a9e60.txt` 79e40c92f9dd5914 (the 75.4% source), xcheck `bce80_xcheck_{default,seeded}.txt` b7a383e2ebbcbf31,
  `bbdb0_xcheck_default.txt` df773241e8a3e559, `bbdb0_xcheck_seeded.txt` 5f17f02c6316ffa1
- promotions dry runs `c4_dry1.txt` da5056e959a716a8, `c4_dry2.txt` c74d91d6ac3fba35, `c4_dry3.txt` de1479d120318fac (merged tree: `batch: 3 pass, 0 refused`)
- previous sources: `005bce80_before.c` f082a279c4fa3179, `005bad30_before.c` b68909960e4a5ce3, `005bbdb0_before.c`
  c51534b5527ca9a0, `005bac00_before.c` d4baf51e2b80f7ea, `005a9e60_before.c` 56a1659fab3e5119
- `lockstatus.txt` 7647587fb10a6601 (the table above)
