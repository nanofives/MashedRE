# RESULT PORT-TOP-AUDIO-5AA3D0-STOPS: the init-arm stops from 0x005AA3D0 on

## Verdict: GREEN (9 stops passed in 3 checkpoints; both builds now stop at 0x005A8F30, census order 23)

- **Self-test result: PASS** in every arm, both builds, all three checkpoints (portstate-image 0 of 8 failed,
  OT_CODEPTR 0 of 367 failed, code-trap 0 of 6 failed).
- No newly DIFFERENT unit outside the bodies this row links: the base has 24 (fresh base check, section 1.6); this row
  adds 0x005aa3d0 (admitted 93.0%) and 0x005a6340 (admitted 82.4%), both linked for the first time. The merge with
  orch/integration (section 3.3) added none.
- Byte matches: 0x005b36b0, 0x005aad50, 0x005aee20 (`re/promotions/PORT-TOP-AUDIO-5AA3D0-STOPS.tsv`, dry run 3 pass).
  Admitted: 0x005aa3d0, 0x005a6340. Provisional: 0x005aa330, 0x005ad420, 0x005a8ea0 (decision L-171), 0x005aa200
  (decision row requested). 21 C4 rows bound in bulk from the census. None of the admitted or provisional bodies is C4.
- Not reached: 0x0045d460, so RO-AUDIOINIT-45D460-REACH is not answered (Next).

Base: orch/integration @ 810042e0e. Branch orch/PORT-TOP-AUDIO-5AA3D0-STOPS. Top-down row: both builds, ddkobj check,
knob boot (init arms), self-tests. Gates: not run (top-down row). No menu walk: no menu code changed.
equiv.py = the merged copy (tool commit 1ed7ceb1a, no diff on this branch).

## Stops passed (one line each)

| # | stop | how it was passed | checkpoint |
|---|---|---|---|
| 1 | 0x005aa3d0 | **admitted** (L-126): source reshaped 86.0% -> 93.0%, `EQUIV-IF-CALLERS-IGNORE(ECX)`, callers 2/2 ignore; bound | 1 |
| 2 | 0x005aa330 | **provisional** (54.7%, NOT-PROVEN call-args at 0x005aa350, masked-high-bits tool limit); bound with 0x005aa3d0 (0x005a9c50's other named stop) | 1 |
| 3 | 0x005b36b0 | **byte match**: source rewritten from the listing, BYTE-IDENTICAL 57 B, relocheck OK; promotion row; bound (0x005aa3d0's callee) | 1 |
| 4 | 0x005a6340 | **admitted**: plain `EQUIV` on the merged tool (82.4%); bound. Plus the census bulk bind (21 C4 rows), the 0x00632c2c entries, 0x005aad50 (byte match) and 0x005a8ea0 / 0x005ad420 (provisional) with the code-pointer slots regenerated | 2 |
| 5 | 0x005aee20 | **byte match**: source returns the AL counter, BYTE-IDENTICAL 28 B, relocheck OK; promotion row; bound | 3 |
| 6 | 0x005aa200 | **provisional** (66.1%, NOT-PROVEN ult-1 / eq-0 form gap at 0x005aa271); bound; decision row requested | 3 |
| 7 | 0x005adbe0, 0x005ad420, 0x005aaf50 / 0x005aaef0 / 0x005aae90 | passed in the arms (bound in checkpoint 2); the arms enter the 0x00632c28 entry 0x005a8ea0 | 3 |

## 1. Checkpoint 1: 0x005aa3d0 (admitted), 0x005aa330 (provisional), 0x005b36b0 (match)

### 1.1 0x005aa3d0
- Old source (86.0%) loaded field +0x10 / +0x14 into a local and formed the loop pointer after the ushort count test;
  the DDK sank the base load below the test, and equiv.py reported `NOT-PROVEN branch-condition` at 0x005aa400: the
  compared expression was `and32(ins32(X, ld16(count)), 0xffff)` on both sides with X = the register whose low half
  takes the count (ours param_2's register, orig the +0x4c pointer). The and clears X, so this was a form gap, not a
  value difference.
- New source: `puVar4 = (undefined4 *)(*(int *)(iVar5 + 0x10) + 0x10);` before the count load (same for +0x14 in the
  else arm, 0x005aa430). match.py 93.0% (regs 96.5%, ops 96.5%, 57/57 ins). Residual: the original keeps the base in
  ecx and forms the pointer with `lea esi,[ecx+0x10]`, ours loads into esi and adds 0x10. Four other spellings scored
  61.4% to 93.0%.
- equiv.py: `EQUIV-IF-CALLERS-IGNORE(ECX) [REG,VALUE-FLOW] callers 2, scan says ignore: 2/2: 0x005a9c5e:ignores
  0x005a9c66:ignores` (both call sites are in 0x005a9c50). Every caller ignores, so it is admitted with no decision row
  (L-126). equiv_dyn.py cross-check with ecx ignored: default `INCONCLUSIVE same=0 inc=32 diff=0`, seeded
  `SAME same=32 inc=0 diff=0` (32 conclusive runs in one mode, the EQUIV-ADMIT-1 coverage rule). Without the ignore
  list the seeded run reported `DIFFERS` on ecx only, the register the verdict names. ADMITTED.tsv row (source commit
  bc5fd77ae, tool commit 1ed7ceb1a). Not C4.

### 1.2 0x005aa330
- Source unchanged (`re/match/s25/005aa330.c`, 54.7%, 53/53 ins). equiv.py: `NOT-PROVEN call-args: region ours +0x1d /
  orig 0x005aa350 call 0x5ae030 arg 0`: both sides compute `[[p+0x4c]+0x10] + and32(ins32(X, ld16([[p+0x4c]+0xc])),
  0xffff)*24 - 0x18` and differ only in X (ours eax_in, orig `[p+0x4c]`). Same masked-high-bits form as 1.1, a tool limit.
  Five reshapes (base local, swapped product, tail locals, end pointer first, char pointer) did not move the score or
  the verdict. Listed provisional (PROVISIONAL.tsv) and bound. Not C4.

### 1.3 0x005b36b0
- The old source (53.1%) tested `param_1 != piVar1 + 1` (node + 4 bytes == list head), a different condition from the
  original, which compares the next node with the list end `param_1 + 4 bytes` (`lea edi,[eax+4]` 0x005b36c7,
  `cmp ecx,edi` 0x005b36d5). equiv.py had reported it as `NOT-PROVEN branch-condition` at 0x005b36d3.
- New source from the listing (guard `param_2 <= *param_1 && *param_1 != 0`, result `node - 0x20`, do-while walk that
  returns 0 at the list end): `match.py --bytes` 100.0% 29/29 ins, 57 B BYTE-IDENTICAL; relocheck OK checked=0;
  equiv.py `EQUIV [BYTE-IDENTICAL]`. Not in PROVISIONAL.tsv (no drop). Promotion row
  `re/promotions/PORT-TOP-AUDIO-5AA3D0-STOPS.tsv`; dry run `[1/1] 0x005b36b0 match PASS byte-identical 57 B, relocheck
  OK (0/0)`, no `--apply` on the branch. hooks.csv is C3 (hand body `Audio/AudioList_ag.cpp`, frozen hand path,
  untouched).

### 1.4 Binding
- `audiotick_bind_gen.py`: `PROV_BIND |= {'005aa330'}`; 0x005aa3d0 binds through ADMITTED.tsv, 0x005b36b0 through the
  promotion TSV. Generator fix: for a hooks.csv c4=match row it now prefers the note's `re/match/<d>/<rva>.c`
  (0x005aa510's note cites `re/match/s30/005aa480.c` first, and the plan had bound 0x005aa510 to that file).
- Roots unchanged (`roots.txt` sha256 73566306..., PORT-TOP-AUDIO-BODY-45D460's). `exe_body_index.py` re-run (1996 rvas;
  the committed index stays the base's).
- Plan: 276 bind (237 match, 17 admitted, 14 provisional, 8 replay), 64 reuse, 33 stops. Against the base plan:
  0x005aa3d0 / 0x005aa330 / 0x005b36b0 stop -> bind; 0x005aa510 / 0x005ad910 / 0x005ad9d0 show C4 now (applied by the
  merge train), same sources.
- Wrappers changed: `PortTop_005a9be0.cpp`, `PortTop_005a9c50.cpp`, `PortTop_005adb70.cpp`, `PortTop_AudioStops.cpp`
  (three stops removed). New TUs `PortTop_005aa3d0.cpp`, `PortTop_005aa330.cpp`, `PortTop_005b36b0.cpp`. newfiles.txt
  +3, rsp +3, allowlist +1 (ORIGTOP-ONLY DUP-IN-TARGET 0x005b36b0 with `Audio/AudioList_ag.cpp`). BINDINGS.tsv updated
  36. The generator rewrote the other wrappers with LF endings only (no content change); not committed.

### 1.5 Builds, check, arms
- VS2022: Build OK, rva-lint 327 known, NEW 0. Object build: Build OK.
- `ddkobj.py check`: IDENTICAL 1573/1614, DIFFERENT 25, not-in-map 16, units linked 1564/1782, refusals 11, provisional
  kept VS2022 207 units + 5 fns. DIFFERENT = the 24 of RESULT_PORT-TOP-AUDIOINIT-SPLIT-5A9970 checkpoint 4 (same list) +
  **0x005aa3d0** (`_PortTop_Bound_005aa3d0`, 28 bytes differ, first +0x25), newly linked here: the admitted 93.0% body.
  0x005b36b0 IDENTICAL. 0x005aa330 is provisional (kept VS2022). Fresh base check: section 1.6.
- Arms:

  | arm | build | stop | return address | log sha256 (16) |
  |---|---|---|---|---|
  | vs_init_audio | VS2022 | **0x005A6340** after 9.2 s | 0x0023AB85 = `_PortTop_Bound_005a62b0+0x35` (`mashed_re.map`, symbol at 0x0023ab50) | 05f2412afa83d4a0 |
  | ddk_init_audio | object | **0x005A6340** after 9.9 s | 0x00048743 = `_PortTop_Bound_005a62b0+0x33` (`mashed_re_ddk.map`, symbol at 0x00048710) | 405d50c29553232f |

  - The STOP line in both logs: `the matched 0x00000000 called original 0x005A6340 (DAT_006904e8 = 0, DAT_006904ec = 0)`.
    0x005a6340 is a plan stop (C2, no admitted / provisional / match source).
  - **Self-tests PASS in both arms:** portstate-image 0 of 8 failed, OT_CODEPTR 0 of 367 failed (188 slots, 11 bound),
    code-trap self-test pages 459/459 and read / write / execute ok.

### 1.6 Fresh base check
This worktree detached at the base 810042e0e, object build (Build OK), `ddkobj.py check`: IDENTICAL 1572/1612,
DIFFERENT 24, not-in-map 16, units linked 1562/1779, refusals 11, provisional kept VS2022 206 units + 5 fns. The 24
are the same list as checkpoint 1's minus 0x005aa3d0. So checkpoint 1's only newly DIFFERENT unit is 0x005aa3d0, the
admitted 93.0% body linked for the first time (classified in 1.1). Raw: `ddkcheck_base.log`, `bytecheck_base.tsv`,
`build_base_ddk.log`.

## 2. Checkpoint 2: 0x005a6340 (admitted), the census bulk bind, the 0x00632c28 / 0x00632c2c / 0x00632c38 tables

### 2.1 0x005a6340
- Source unchanged (`re/match/s25/005a6340.c`, 82.4%, 164/170 ins, residual named in its header). On the merged tool
  equiv.py gives plain `EQUIV [LAYOUT,VALUE-FLOW]`. equiv_dyn.py: default `SAME same=32 inc=0 diff=0`, seeded
  `SAME same=32 inc=0 diff=0`. ADMITTED.tsv row (source commit c9ff1a67c, tool commit 1ed7ceb1a). Not C4.

### 2.2 The census bulk bind
- Census `verify/census_20261116/audio_window_65.tsv` (orch/T2-RACEPATH-CENSUS @ bbbe54319, read with git show; copy and
  the c2e0db02d version with first-call order in the data folder): 29 rows `C4-unbound`. They were added to the roots
  (`verify/portorigtop_20261113/audio_5aa3d0/roots.txt`, 67 roots = the 39 base roots + 28; 0x005aef30 left out, below).
- Bound by match (hooks.csv c4=match source): 0x005aae90 0x005aaef0 0x005aaf50 0x005ac7b0 0x005ac7e0 0x005aca40
  0x005ad6a0 0x005adbe0 0x005aea50 0x005b34f0 0x005b73e0 0x005ba720 0x005bbbb0 0x005bbed0 0x005bbf30 0x005bc800
  0x005bc860 0x005be0f0 0x005c7990 0x005c9680 0x005c96e0, plus their match callees 0x005a6670 and 0x005aadc0.
  Already bodies (reuse): 0x005aef00, 0x005bc450, 0x005bf660. Stops (no exe body): 0x004a8815 / 0x005c2dd9 /
  0x005c2e79 (CRT, C4 by libid, no source to bind).
- **Left out, two rows:**
  - 0x005aef30 calls `__beginthread` (0x005c2e79): it has no exe body, the VS2022 link failed (LNK2019
    `___beginthread` in `_PortTop_Bound_005aef30`), and binding it would start the second thread from the exe. Left a
    stop; it is census order 42, after the init stops. [UNCERTAIN] line PORT-TOP-AUDIO-5AA3D0-STOPS (a).
  - 0x005bf610 calls 0x005bf670, which its source declares `__stdcall` (`re/match/s28/005bf610.c` line 28) and which has
    no body; the generated cdecl stop thunk does not link (LNK2019 `_PortTop_AStop_005bf670@4`). Generator
    `STOP_FORCE |= {'005bf610'}`, so 0x005be0f0 calls the 0x005bf610 stop thunk.
- In the object build 12 of the bulk bodies are `not-in-map` (0x005ad6a0 0x005b73e0 0x005ba720 0x005bbbb0 0x005bbed0
  0x005bbf30 0x005bc800 0x005bc860 0x005be0f0 0x005c7990 0x005c9680 0x005c96e0): roots no bound body calls yet, so the
  linker keeps no code for them. Every bulk body in the map is IDENTICAL.

### 2.3 The three tables 0x005a6110 walks
- 0x005a6110 calls 0x005ad080 with (0x00632c2c, 3), then (0x00632c28, eax = 1) and (0x00632c38, eax = 1)
  (`push 3` / `push 0x632c2c` 0x005a6110 / 0x005a6112; `push eax` / `push 0x632c28` 0x005a6124 / 0x005a6125; `push eax` /
  `push 0x632c38` 0x005a6137 / 0x005a6138). 0x005ad080 returns 0 when an entry returns 0
  (`re/match/*/005ad080.c`: `if (param_1[i]() == 0) return 0;`).
- **0x005aad50** (entry 0x00632c38): no Ghidra function. pc_funcs.csv row (72 B, `ret` at 0x005aad97, then `nop` pad),
  hooks.csv C1 row through `c1_add.py`. Source from the headless decompile: `match.py --bytes` 100.0% 22/22 ins, 72 B
  BYTE-IDENTICAL, relocheck OK checked=8; promotion row (dry run `[2/2] 0x005aad50 match PASS byte-identical 72 B,
  relocheck OK (8/0)`). It stores 0x005aadc0 (bound, C4 match) and 0x005aae10 / 0x005aae30 / 0x005aae60 (no source;
  the stored values are their stop thunks) at +0xc..+0x18.
- **0x005a8ea0** (entry 0x00632c28): no Ghidra function. pc_funcs.csv row (137 B, `ret` at 0x005a8f28), hooks.csv C1
  row. Best 77.6% (41/49 ins): the DDK cross-jumps the two failure tails the original keeps apart (0x005a8eee and
  0x005a8f04: `push esi` / `call 0x005a9d00` / `xor eax,eax` / `pop esi` / `ret`); nested-if and return-variable
  spellings gave 65.3% / 77.6%. equiv.py `EQUIV-IF-CALLERS-IGNORE(EAX) [LAYOUT] callers 0`: the one caller is indirect
  (0x005ad080 through 0x00632c28) and it reads EAX. A conditional verdict with a reading caller is not an admission
  (L-126): PROVISIONAL.tsv row + decision row (section Decision). Bound. Not C4.
- **0x005ad420** (callee of the three 0x00632c2c entries): guard spelling from bc5fd77ae, 74.5%; equiv.py `NOT-PROVEN
  call-regargs` ecx at the 0x005aea00 call 0x005ad4e5 (ours the ecx after the 0x004522d0 call, orig param_5), the
  register line PORT-TOP-AUDIOSTOP-5A61B0 (b). PROVISIONAL.tsv row, bound. Not C4.
- `codeptr_inventory.py --write`: the five slots 0x00632c28..0x00632c38 now resolve to `PortTop_Bound_005a8ea0` /
  `_005aaf50` / `_005aaef0` / `_005aae90` / `_005aad50` (16 slots bound, was 11). `CODEPTR_SLOTS.tsv` and
  `Compat/PortTopCodePtr.inc` regenerated; no other slot changed.

### 2.4 Binding
- `audiotick_bind_gen.py`: `PROV_BIND |= {'005ad420', '005a8ea0'}`, `STOP_FORCE |= {'005bf610'}`; roots as in 2.2.
- Plan: 303 bind (261 match, 18 admitted, 16 provisional, 8 replay), 67 reuse, 45 stops.
- Wrappers changed: `PortTop_005a62b0.cpp` (0x005a6340 bound), `PortTop_005aeb30.cpp`, `PortTop_AudioStops.cpp`. New
  TUs: 27 (`PortTop_005a6340`, `_005a6670`, `_005a8ea0`, `_005aad50`, `_005aadc0`, `_005ad420` and the 21 bulk bodies of
  2.2). newfiles.txt +27, rsp +27, allowlist +1, BINDINGS.tsv updated 44.

### 2.5 Builds, check, arms
- VS2022: Build OK, rva-lint 328 known, NEW 0. Object build: Build OK.
- `ddkobj.py check`: IDENTICAL 1585/1639, DIFFERENT 26, not-in-map 28, units linked 1589/1809, refusals 11,
  provisional kept VS2022 209 units + 5 fns. DIFFERENT = checkpoint 1's 25 + **0x005a6340** (the admitted 82.4% body,
  linked for the first time). not-in-map +12 = the 2.2 list. All other new units IDENTICAL (bytecheck2.tsv against
  bytecheck1.tsv). The base list is section 1.6; nothing else moved.
- Arms:

  | arm | build | stop | return address | log sha256 (16) |
  |---|---|---|---|---|
  | vs_init_audio | VS2022 | **0x005AEE20** after 3.2 s | 0x0023D722 = `_PortTop_Bound_005a6340+0x22` (`mashed_re.map`, symbol at 0x0023d700) | aeec857d9084d467 |
  | ddk_init_audio | object | **0x005AEE20** after 3.0 s | 0x0004A580 = `_PortTop_Bound_005a6340+0x20` (`mashed_re_ddk.map`, symbol at 0x0004a560) | 6e2e9dc8429b6078 |

  - 0x005a6340's source calls 0x005aee20 through its bit-scan macro (`#define SH_005a6340 (1 << FUN_005aee20(0x28))`,
    `re/match/s25/005a6340.c` line 38). 0x005aee20 is census order 4 (hooks.csv C3, hand body `Audio/AudioMemory.cpp`).
  - **Self-tests PASS in both arms:** portstate-image 0 of 8 failed, OT_CODEPTR 0 of 367 failed (188 slots, 16 bound),
    code-trap 0 of 6 failed (pages 459/459).

## 3. Checkpoint 3: 0x005aee20 (match), 0x005aa200 (provisional), merge of orch/integration

### 3.1 0x005aee20
- The old source (78.6%) returned `void` and masked the shift count with 0x1f. The original returns the bit index in AL:
  `xor al,al` 0x005aee24, `mov esi,1` / `mov cl,al` / `shl esi,cl` 0x005aee27..0x005aee2e (no mask), `test edx,esi`,
  `inc al` 0x005aee34, `cmp al,0x20` / `jb` 0x005aee36, leaving al = the index or 0x20.
- New source: `unsigned char` counter, do-while, `break` on the first set bit, `return bVar1`. `match.py --bytes`
  100.0% 13/13 ins, 28 B BYTE-IDENTICAL; relocheck OK checked=0; equiv.py `EQUIV [BYTE-IDENTICAL]`. Three other loop
  spellings are also byte-identical. Promotion row (dry run `[3/3] 0x005aee20 match PASS byte-identical 28 B, relocheck
  OK (0/0)`). hooks.csv is C3 (hand body `Audio/AudioMemory.cpp`, frozen hand path, untouched).
- Callers declare a 4-byte return (`int` / `undefined4` / `unsigned int`, 9 sources). Every use in them is a shift count,
  `1 << FUN_005aee20(0x28)` (`re/match/s25/005a6340.c` line 38, `re/match/s28/005c6cc0.c` line 36, and the macro text
  named in the headers of 0x005a8060 / 0x005b9f30 / 0x005c7a90 / 0x005c8120), so only CL is read and the byte return
  does not reopen the `bool` / `int` class of RESULT_PORT-TOP-AUDIOINIT-SPLIT-5A9970 1.2.

### 3.2 0x005aa200 (census order 5, 0x005a62b0's callee at 0x005a62f2)
- Source unchanged (`re/match/s25/005aa200.c`, 66.1%, 112/108 ins). equiv.py `NOT-PROVEN branch-condition` at
  0x005aa271: ours `(ushort)param_3 >= 1` (ult 1), orig `== 0`, the same value for an unsigned count.
- The `param_3 != 0` / `param_5 != 0` spelling (46.4%, `005aa200_eq0_variant.c` in the data folder) proves further, to
  `EQUIV-IF-CALLERS-IGNORE(EDX,ECX) [LAYOUT,FRAME,VALUE-FLOW]`, callers 11, scan says ignore 0/11: 0x005a62f2,
  0x005a802e, 0x005a8ee2, 0x005b7482, 0x005b862e, 0x005b9ee2, 0x005c6c6e, 0x005c7a42, 0x005c80ee `reads/reads`;
  0x005b6bc2, 0x005bce62 `unknown/unknown`. Read at two of them: after 0x005a62f2 the caller runs `add esp,0x18` /
  `test eax,eax` / `jne` and then `push esi` / `call 0x005a9d00` or `call 0x005aeb30` (0x005a62f7..0x005a630b); after
  0x005a8ee2 the same shape (0x005a8ee7..0x005a8efb). Neither reads ecx or edx itself before its next call.
  [UNCERTAIN: the scan's `reads` comes from the next call, not from the caller's own instructions; whether 0x005a9d00 /
  0x005aeb30 / 0x005b34f0 read ecx or edx at entry is not resolved by a tool.]
- Listed provisional (the 66.1% source, NOT-PROVEN), bound. Not C4. Decision row requested from the lead (section
  Decision).

### 3.3 Merge of orch/integration
- `git -c merge.autoStash=false merge orch/integration` (215d5bbb9, no rebase) after the pre-merge commit 12857f712:
  conflicts in `re/analysis/CHANGELOG.md` and `re/tools/dual_copy_allowlist.txt` (one add/add hunk each), resolved as the
  union of both sides. Against orch/integration the merged tree differs in those two files, hooks.csv, ADMITTED.tsv,
  PROVISIONAL.tsv, BINDINGS.tsv, UNCERTAIN_REGISTER.md, exe_sources.rsp and pc_funcs.csv only by this row's lines (the
  six BINDINGS.tsv removals are this row's stop -> bound updates). Merge commit ec6a20f1e.
- The merge brought source changes (`re/match/s08/00432b30.c`, `0043dfd0.c`, five PortTop wrappers, `PortTop_StandInsRL.cpp`,
  rsp +8). `exe_body_index.py` on the merged tree (2032 rvas) and the plan dry run give the same plan as before the merge
  (305 bind, 67 reuse, 43 stops).

### 3.4 Builds, check, arms (merged tree)
- VS2022: Build OK, rva-lint 330 known, NEW 0. Object build: Build OK.
- `ddkobj.py check`: IDENTICAL 1593/1647, DIFFERENT 26, not-in-map 28, units linked 1597/1818, refusals 11, provisional
  kept VS2022 210 units + 5 fns. DIFFERENT is checkpoint 2's list exactly: no newly DIFFERENT unit from this checkpoint
  or from the merge. 0x005aee20 IDENTICAL; 0x005aa200 provisional (kept VS2022).
- Arms:

  | arm | build | stop | return address | log sha256 (16) |
  |---|---|---|---|---|
  | vs_init_audio | VS2022 | **0x005A8F30** after 2.7 s | 0x0023E4C5 = `_PortTop_Bound_005a8ea0+0x35` (`mashed_re.map`, symbol at 0x0023e490) | 1deb4ce006563c16 |
  | ddk_init_audio | object | **0x005A8F30** after 2.9 s | 0x0023C2B5 = `_PortTop_Bound_005a8ea0+0x35` (`mashed_re_ddk.map`, symbol at 0x0023c280) | 0f8d95f16ce36690 |

  - 0x005a8ea0 (the 0x00632c28 entry) calls 0x005a8f30 right after 0x005a9ab0 (`push esi` / `call 0x5a8f30`
    0x005a8ecd / 0x005a8ece). So both builds passed census orders 4 to 22: 0x005aee20, 0x005aa200, 0x005adbe0,
    0x005ad420 and the three 0x00632c2c entries, and entered the 0x00632c28 entry through the regenerated slot. Both
    builds run the provisional (VS2022) body of 0x005a8ea0, hence the same +0x35.
  - **Self-tests PASS in both arms:** portstate-image 0 of 8 failed, OT_CODEPTR 0 of 367 failed (188 slots, 16 bound),
    code-trap 0 of 6 failed.

## Next (not done in this row)
- **0x005a8f30** (census order 23, both builds): `re/match/s25/005a8f30.c` is raw g2s output, 30.4% (97/168 ins, 589 B),
  equiv.py `NOT-PROVEN branch-condition` at the entry: the same masked-shift form as 3.1
  (`and8(ext8(and32(ret,0x1f)),0x1f)` against `and8(ext8(ret))`). It needs a source written from the listing; its
  header names the repeated `1 << FUN_005aee20(0x28)` macro.
- Then the census window in first-call order (c2e0db02d version): 24 to 39 are bound C4 rows except 0x005b9f30 (29),
  0x005b8680 (34), 0x005c7a90 (36), 0x005c6cc0 (38), 0x005a8060 (40) with no C4 body; 41 0x005aef00 reuse; 42
  0x005aef30 (`__beginthread`, left a stop, register line (a)); 43 to 47 are CRT / second-thread rows (0x005c2e79,
  0x004a8815, 0x005c2dd9, 0x005aef70, 0x005a84d0); 48 to 92 hold 22 more rows with no C4 body (0x005c8120 0x005b74c0
  0x005b6be0 0x005bce80 0x005bad30 0x005c1920 0x005bbdb0 0x005bac00 0x005ba1d0 0x005ae250 0x005bb000 0x005bbc10 0x005bbd50
  0x005c7d30 0x005bc470 0x005bf670 0x005bbfc0 0x005baf00 0x005a91b0 and the stops 0x005aac00 0x005a9e60 0x005a9f10).
- 0x005bf610: forced to a stop (its callee 0x005bf670 is `__stdcall` with no body); bind it once 0x005bf670 has one.
- **RO-AUDIOINIT-45D460-REACH not answered:** the arms do not reach 0x0045d460 yet, so DAT_00604604 at record 0 and the
  MASHED_OT_AUDIOINIT oracle re-run are still open.

## Decision
- 0x005a8ea0: `EQUIV-IF-CALLERS-IGNORE(EAX)`, the one caller (0x005ad080, indirect through 0x00632c28) reads EAX. Listed
  provisional, not admitted (L-126). **Decision L-171** (filed by the lead): keep it provisional (recommended: every
  return path gives 0 or the object, the gap is the merged failure tails) or require a shape that keeps the tails apart.
- 0x005aa200 (checkpoint 3, section 3.2): the source that clears the NOT-PROVEN form gap proves to
  `EQUIV-IF-CALLERS-IGNORE(EDX,ECX)` with 0 of 11 callers scanned as ignoring. The bound source is the unchanged 66.1%
  one, listed provisional as NOT-PROVEN. The lead files its own decision row (question text sent to the lead).

## Lock status at build/run time
Times -0300, the start time of each command. `mashed_lock.py status` prints no time; the line is the one printed in
the same command, before the step.

| checkpoint | step | start | status printed just before the step |
|---|---|---|---|
| 1 | VS2022 build | 10:59:13 | `free 'mashed_machine'` (printed in the command right before, about 10:58) |
| 1 | object build | about 11:05 (same command as VS) | none |
| 1 | arms | 11:14:18 | none; `mashed_lock.py run` printed `HELD` at start |
| 1 | base object build (section 1.6) | 11:28:18 | `free 'mashed_machine'` |
| 2 | (no build) | 11:36 to 11:41 | `SOLO merge-train13:20261116_111112 since 11:36:44` and `PRIORITY-LINGER`: waited in a loop until `free` at 11:41:20 |
| 2 | build try 1: VS (link failed), then object build | 11:44:44 | `free 'mashed_machine'` before VS; none before the object build (about 11:46) |
| 2 | build try 2: VS2022 build | 11:49:18 | `free 'mashed_machine'` |
| 2 | build try 2: object build | about 11:51:46 (same command) | none |
| 2 | arms | 11:59:29 | none; `mashed_lock.py run` printed `HELD 'mashed_machine' pid=20160` at start |

The object builds of checkpoints 1 and 2 and both arms runs had no status check right before them. If the merge train
20261116_114223 held PRIORITY-HOLD between 11:44 and 12:11, the checkpoint 2 object builds ran inside it. From
checkpoint 3 on, the status is printed before each build and each arms run and recorded here.

| checkpoint | step | time printed | status printed just before the step |
|---|---|---|---|
| 3 | VS2022 build (merged tree) | 12:35:31 | `free 'mashed_machine'` |
| 3 | object build (merged tree) | 12:40:33 | `free 'mashed_machine'` |
| 3 | arms | 12:47:40 | `free 'mashed_machine'`; `mashed_lock.py run` then printed `HELD 'mashed_machine' pid=11848` |

(`lockstatus3.txt` in the data folder.)

## Arms command
`ARMS_STEP=PORT-TOP-AUDIO-5AA3D0-STOPS ARMS_ROOT=D:/MashedWT/PORT-TOP-AUDIO-5AA3D0-STOPS_root ARMS_EXTRA="MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1" test-slot py -3.12 <ORCH>/re/orchestrator/mashed_lock.py run --label PORT-TOP-AUDIO-5AA3D0-STOPS -- py -3.12 verify/portorigtop_20261113/audiotick_stops_arms.py vs_init_audio ddk_init_audio`
Private root: robocopy of `D:/MashedWT/PORT-TOP-AUDIOINIT-SPLIT-5A9970_root`, its old `mashed_re.log` removed.

## Raw data (`D:/MashedWT/PORT-TOP-AUDIO-5AA3D0-STOPS_data/`, sha256)
Checkpoint 1:
- `cp1/vs_init_audio.log` 05f2412afa83d4a06ecf3bc0a029b303e489bff2e646c0ceda3181e7f778f6ba
- `cp1/ddk_init_audio.log` 405d50c29553232147c55cb09e35b014e4190943428ed648f2550bfac59e0413
- `arms1.out` 49a211d32f1c8466d6cef924ee968d3b65059392b93691ca17037224fd8d13d6
- `build1_vs.log` 12870b249d21fb937365b90de4be2bc3906154ce29f8cda87dd8394e4c3db95d
- `build1_ddk.log` 7062ea45e7a7148bb4914978bb40f9f37197a774ad29786fc048b00421e110c1
- `ddkcheck1.log` 6028784f6c6eaa8f8b4aca5410829faef723b2fc838bc80c109d29f6fd817e61
- `bytecheck1.tsv` 8833963692ed00bbc21a5a9ff1cd87fdd79b1087760cc88a4a3811df7130b94d
- `plan1_dry.txt` d059b0739916df4c5c4fd1e124bcf2546ea54390e4267e3ea340f61abf99eb6f
- `plan1_write.txt` 4518f0722b57a76affcfcda46d2734aa8088c15fe9bbf1cb832a185e9eee7973
- `equiv_a3d0.txt` df65d593c35331cdddbda54e5b29b62f9b9ebd30071345708c2b3fb57cb5565a
- `equiv_a330.txt` 02e8bc07eb7928da86676bbbde3f669faca5c5320c28d7fa7593f5e731056522
- `a3d0_xcheck_default.txt` 1c62b99adae9b49346ce99a544be39b0c485a16371a1c5965db7a13322544f73
- `a3d0_xcheck_seeded.txt` efd6a4cfef64cb55b0c4f25efc51fe114b6f550d1e130b298481cd25fdc42ddb
- `005b36b0_before.c` 902dc90b198a1eccb42d19b0640da66e2018eccf219b7001761ca302de38c380
- `b36b0_match_bytes.txt` 61f256eee4a4bceee41451d67f09558a740f968112806803b61c39a99aec2b94
- `b36b0_relocheck.txt` 73eff4d6c451cf509953e491a045f4188c6ad9c276962c754dce518da696cac4
- `c4_dry1.txt` 8e991f19b3a6d3a7c5bc82d3d265bcb2d1ebbc5a30fbae4bd60bd70b2d674fb1
- `exeidx1.json` 6f7403c4e9476ef48f7d1f2c871e843254d444ea8c41cc14e871d576df6e4513
- `roots.txt` 73566306c2eb5600447befbb32c4667ff8465d6d18e268986f50ac1fe77142bb
- `sync1.txt` eef80ba63a0ed94a64d6d5a1d791a8b15ef3f3f868d398e4b9a4dc16bbfc9bde

Base check (section 1.6):
- `ddkcheck_base.log` cd187b211157810159c94b5b184e620666e3e14bcabdcb34131b72cdef2a7ec4
- `bytecheck_base.tsv` 62efcef05b7b081dc1fcad91ee2535fa54c8bb8949da8b01b803c5842f82dde5
- `build_base_ddk.log` 67259cb4b02eb8f770926d1b06ea0ba93144a619a94a3e11bc2ad35237484f0a
Checkpoint 2:
- `cp2/vs_init_audio.log` aeec857d9084d467975ffffdf8bf0a736d751dbf759e142181e91db60e2feeda
- `cp2/ddk_init_audio.log` 6e2e9dc8429b6078c5c78b12deac9119301220380029a1f5967f32df54068360
- `arms2.out` e35a633101f34dbf45412c2a9c2e6d632cd5347e18a52313a177ba3b6d6805eb
- `build2_vs.log` 09d18aef298c0494d7d6b5f40e12d93e687f6880d7a143c3179b9395aa8240a3
- `build2_ddk.log` 9315237bd6ce94891b195db585456ac61b90967652832eb6aa73d16fa7524db4
- `ddkcheck2.log` dad59c3a5d0eaae491761d9348c521626fbd67c4b4fa57fd68be23d34a6b37bf
- `bytecheck2.tsv` fcb2462f40c3c642d0a7e410946cba424f6191eb9a291d111b47ed2eb65bc7ab
- `plan4_dry.txt` aad8e6eec45e3c9654f0c5db94700b1be7a5118a07260e2354c4a577c98a1d4e
- `plan4_write.txt` d02c624288e3d4ec56bec050a5fc30486e11b711162fd0cca1b628d11fbeb670
- `equiv_6340.txt` de01e11f3b2e3abf390310434112843edb098e17199e2e5b4afca99659805a30
- `6340_xcheck_default.txt` 62a6c49205cb2cd557c23af1ec3bd35bfd259bd623a6ddf691c863285825bfaf
- `6340_xcheck_seeded.txt` 62a6c49205cb2cd557c23af1ec3bd35bfd259bd623a6ddf691c863285825bfaf
- `equiv_d420.txt` af6eda7bbfd2d1737fc28fc5f3b0136c15d8815f4d117a749be067eb79db3aca
- `c4_dry2.txt` 83240936f00455f7d576e57be83d5e6080f902d83892833652ba8e8a6fc042e6
- `codeptr4.txt` ea3fc94edd2e310264908518d9bdc98f1020643ba9d06a909b9553a74ba571b4
- `roots4.txt` fd1ba37a34f71b8d718ba3f81e014beae7bc5467f2a5756f1d91a3a740caa65b
- `exeidx2.json` 6f7403c4e9476ef48f7d1f2c871e843254d444ea8c41cc14e871d576df6e4513
- `audio_window_65.tsv` a53faacf8ff6d33d5d964ca8e6fef68c449deb34c320d4efeac93e6f2b08bf13
- `audio_window_65_v2.tsv` 7e226127dd9b81445fae46d4901cc9fc1aa70f1ec8dee2abefb3cf5052a6aec6
Checkpoint 3:
- `cp3/vs_init_audio.log` 1deb4ce006563c1650a3e69b436e106bc706ed32fe0c56b4209a9b7784124b35
- `cp3/ddk_init_audio.log` 0f8d95f16ce366902a475b4bd5bc196ee7f1cae97e5d7308a209eef94c103f8e
- `arms3.out` fdeaac5319fa2ddbebb335b6037db3a90885b49eabd13f98e397057e8863a899
- `build3_vs.log` b4ed0b348558f13d1d1f20519a272a30de6819737c0eb0b375d27bf90b5ababe
- `build3_ddk.log` a6a5c195cf5753ce263b5d35d5bf313e19dcc1a33a72ff668e7808f9a4ee16ba
- `ddkcheck3.log` 95f0b2f08ba0285451b848ed362d1cf10edf775070baf793e38c246985d2d1e3
- `bytecheck3.tsv` 747ff8aaeb71f164fe986104020c8c025b271a04a5bb4c98941adcf91cc64fde
- `plan5_dry.txt` 518418e49e498b09b0bf95419f97a952807f9f4f7e4e46061f997d8a2e99f84a
- `plan5_write.txt` 591136f267a7d26cee236e8d5e5ed40f4b9327f4ad12ce9dafe8c730a8bfbdd0
- `plan6_dry.txt` 518418e49e498b09b0bf95419f97a952807f9f4f7e4e46061f997d8a2e99f84a
- `exeidx3.json` 7af60fca9f76b5fb13be5478602815fe5a29d3e1b215d967c414b497eb783c9b
- `equiv_a200.txt` 81c5483fdb5d8da7c95f6cf71a9d81056a1d5a8752abd8b11f1cb6f0700052c4
- `equiv_a200_eq0.txt` d9686802790500c6235db84f31adbd3071f2a1166368f2d084af3f3d49af4ae1
- `005aa200_eq0_variant.c` 5697a116ee08f91a7679b0092bf36d482c1b03cfaee41f876437319a4c55ebf7
- `005aee20_before.c` 6ac05448525af20a0a96ee1bd13c7ea3206e6fb8a15d04e3b9403b8cc5a96ce9
- `ee20_match_bytes.txt` 6df7e97ae709627ab03335d69095b1fc798dc6a4910c0b9eca3f0547ee8efddf
- `ee20_relocheck.txt` b321012150c45ab02147f494d031d1b737c5994ef37893edd03e723e50d28c42
- `c4_dry3.txt` ea72c127909056bfbbba24388a2483298a4343c65d01db92325f8a75c5752e75
- `lockstatus3.txt` c3aa978e816c198fab830913fe1839d2950a5c89ca6735dfd3a38332ce277f2d
