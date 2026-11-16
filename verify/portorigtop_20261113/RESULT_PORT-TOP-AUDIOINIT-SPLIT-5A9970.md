# RESULT PORT-TOP-AUDIOINIT-SPLIT-5A9970: the init-arm build split, then the stops after it

## Verdict: in progress (checkpoints 1-3 done)

Base: orch/PORT-TOP-AUDIOSTOP-5A61B0 @ c3b0b6356. Branch orch/PORT-TOP-AUDIOINIT-SPLIT-5A9970. Top-down row: both builds,
ddkobj check, knob boot, self-tests. Gates: not run (top-down row). No menu walk: no menu code changed.

## Stops passed (one line each)

| # | stop | how it was passed | checkpoint |
|---|---|---|---|
| 1 | build split (VS2022 0x005AAF50 / object 0x005A9AB0) | first divergence located: `0x005add60`'s source returned `bool`; VS2022 C++ set only AL, so `0x005ada90`'s `== 1` failed in the VS2022 build only. Source return type -> `int`, still BYTE-IDENTICAL + relocheck OK; both builds now stop at **0x005A9AB0** | checkpoint 1 |
| 2 | 0x005a9ab0 | provisional body `re/match/s25/005a9ab0.c` 67.9% (EQUIV-IF-CALLERS-IGNORE(ECX), 16 of 17 callers read or unknown: listed provisional + decision row), with two behaviour fixes to the old source; bound. Its callees 0x005a9c50 (admitted, plain EQUIV 96.7%) and 0x005a9be0 (C4 match) bound. Both builds pass 0x005a9970 and stop at **0x005AA560** (table entry 7, 0x005aa480) | checkpoint 2 |
| 3 | 0x005aa560 | provisional body `re/match/s25/005aa560.c` 79.8% (unchanged source; NOT-PROVEN call-args at 0x005aa593 is a tool limit: 0x005aad40 takes no arguments), bound with 6 C4 callees (0x005aa030, 0x005aa7e0, 0x005aa900, 0x005aa9c0, 0x005aabe0, 0x005adef0 by match; 0x005aa7a0 by frida-replay). Both builds stop at **0x005AA510** (0x005aa480's callback) | checkpoint 3 |

## 1. Checkpoint 1: the first divergence

### 1.1 Where the two builds part
Static path, all from sources and capstone on `original/MASHED.exe.unpatched`:
- `0x004627f0` (`re/match/s10/004627f0.c`) calls `0x005a5f00(0, 1)` first, then `0x005a6110`. It does not test the result.
- `0x005a5f00` walks the 16-pair init table `0x00632ba8` through `0x005a5f60` (`re/match/s25/005a5f60.c`), which stops at
  the first entry that returns 0, runs the shutdown side of the entries before it, and returns 0.
- Table entries (dumped from the image): [0] 0x005a5e70, [1] 0x005ae5b0, [2] 0x005ae1d0, [3] 0x005adf90, [4] 0x005add80,
  [5] 0x005ada90, [6] **0x005a9970** (slot 0x00632bd8), [7] 0x005aa480, ...
- The object build stops in entry 6 (`_PortTop_Bound_005a9970+0x8f`, its 0x005a9ab0 call). The VS2022 build stops in
  `0x005a6110`, so its table walk returned before entry 6 ran.
- The earlier rows' arms (PORT-TOP-AUDIOSTOP-5ADC10 / -5AD770 / -5A61B0) show both builds inside entry 5
  (`0x005ada90 -> 0x005add60 -> 0x005adc10`). Entry 5's tail is `if (FUN_005add60() == 1) return 1;`
  (`re/match/s30/005ada90.c` line 58; original `call 0x5add60` 0x005adb21 / `cmp eax,1` 0x005adb26 / `jne` 0x005adb29).

### 1.2 The unit: 0x005add60
`re/match/s26/005add60.c` declared `bool FUN_005add60(void) { return FUN_005adc10(0x00633720, 0xf) != 0; }`, while its
caller's source declares `int FUN_005add60(void)`. Disassembly of the two builds (base c3b0b6356 binaries):

| build | body | tail |
|---|---|---|
| original | 0x005add60 | `neg eax` 0x005add6f / `sbb eax,eax` 0x005add71 / `neg eax` 0x005add73 / `ret` |
| object (DDK, IDENTICAL) | 0x00049590 | `neg eax` / `sbb eax,eax` / `neg eax` / `ret` (0x0004959f..0x000495a5) |
| VS2022 (C++) | 0x0023b860 | `test eax,eax` 0x0023b870 / `setne al` 0x0023b872 / `ret` 0x0023b875 |

`0x005adc10` returns its first argument (`return param_1;`, `re/match/s26/005adc10.c`), 0x00633720. In the VS2022 build
EAX leaves `0x005add60` as 0x006337xx with only AL = 1, and the VS2022 `0x005ada90` tests the whole register
(`cmp eax,1` at 0x0023b786), so entry 5 returned 0 and the walk failed. In the DDK the C `bool` is
`unsigned char` (preamble) but cl 13 computes the full 0/1 in EAX, so the object build passed.

Fix: return type `int`, same expression. Under the DDK:
- `match.py re/match/s26/005add60.c --bytes`: 100.0% (8/8 ins), 22 B, 0 differing unmasked bytes, BYTE-IDENTICAL
  (`add60_match_bytes.txt`). `relocheck.py`: OK checked=2 unchecked=0 (`add60_relocheck.txt`).
- The row is C4 by match (src sha256 36920b591b37bfd8). New sha b1b814b00cbf37f1: `re/promotions/PORT-TOP-AUDIOINIT-SPLIT-5A9970-refresh.tsv`,
  dry run `c4_reclass.py refresh --tsv`: `[1/1] 0x005add60 refresh REFRESH ... (byte-identical 22 B, relocheck OK (2/0))`,
  0 refused (`refresh_dry.txt`).
- VS2022 build after the fix: `_PortTop_Bound_005add60` at 0x0023b860 ends `neg eax` / `sbb eax,eax` / `neg eax` / `ret`.

Same hazard elsewhere: 30 `re/match` sources define a `bool FUN_<rva>` or declare one. A cross-TU mismatch (`bool`
definition, `int` declaration) is a hazard only in the VS2022 build. Not audited in this row (named in Next).

### 1.3 Builds, check, arms
- VS2022 (`MASHED_NO_DEPLOY=1`): Build OK, rva-lint 322 known, NEW 0. Object build (`MASHED_DDKOBJ=1`): Build OK.
- `ddkobj.py check`: IDENTICAL 1563/1601, DIFFERENT 22, not-in-map 16, units linked 1551/1766, refusals 11. The DIFFERENT
  set is the same 22 rvas as the base (RESULT_PORT-TOP-AUDIOSTOP-5A61B0 sections 1.4 and 3): no newly DIFFERENT unit.
  0x005add60 and 0x005ada90 IDENTICAL.
- Arms (test-slot after the 10 min memory wait, mashed_lock taken and released, ran local):

  | arm | build | stop | return address | log sha256 (16) |
  |---|---|---|---|---|
  | vs_init_audio | VS2022 | **0x005A9AB0** after 16.5 s | 0x0023AE3E = `_PortTop_Bound_005a9970+0x9e` (`mashed_re.map`, symbol at 0x0023ada0) | 7e70d89eb7089e4d |
  | ddk_init_audio | object | **0x005A9AB0** after 16.3 s | 0x00048BAF = `_PortTop_Bound_005a9970+0x8f` (`mashed_re_ddk.map`, symbol at 0x00048b20) | dbb01bfd159c0d0e |

  - Both: `[OT_TIMER] STOP: the matched 0x00000000 called original 0x005A9AB0`. The VS2022 build no longer reaches 0x005a6110.
  - **Self-tests PASS in both arms:** portstate-image PASS (0 of 8 failed, PortStateSeed 272/272), OT_CODEPTR PASS
    (0 of 367 failed), code-trap PASS (0 of 6 failed, 459 pages).

## 2. Checkpoint 2: 0x005a9ab0 (provisional), 0x005a9c50 (admitted), 0x005a9be0 (match)

### 2.1 0x005a9ab0 source
The old source (72.3%, `005a9ab0_before.c`) had two differences from the original that change values, not only bytes:
- The +0x4c clear: the old loop was `puVar4 = puVar4 + 1; *puVar4 = 0;` x6, which writes words [1..6] of the buffer. The
  original is `push edi` 0x005a9b49 / `mov edi,[esi+0x4c]` 0x005a9b4a / `mov ecx,6` 0x005a9b4d / `xor eax,eax` 0x005a9b52 /
  `rep stosd` 0x005a9b54: words [0..5]. New source: `for (i = 6; i != 0; i--) { *p = 0; p++; }`.
- The list link: the original loads `[list+0x10]` into edx (0x005a9bbe) before the `[esi+0x1c]` store (0x005a9bc7) and
  stores that value to `[esi+0x18]` (0x005a9bca). The old source loaded it after the +0x1c store (equiv.py
  `ret-memory` at 0x005a9bbe). New source: `iVar1 = *piVar2;` before the +0x1c store.
- The +0x50 clear is one `*(__int64 *)p = 0` (original `xor ecx,ecx` 0x005a9ba2, two stores 0x005a9ba4 / 0x005a9ba6).
- match.py: 67.9% (regs 74.1%, ops 90.2%, 107/112 ins). Residual: param_1 is kept in ebp in the original
  (0x005a9ab2 `mov ebp,[esp+0xc]`), the DDK spills it to its stack slot; a local copy of param_1 scored 65.2%.
- equiv.py (orch/integration copy, no diff on this branch): `EQUIV-IF-CALLERS-IGNORE(ECX) [REG,LAYOUT,FRAME,VALUE-FLOW]
  callers 17, scan says ignore: 1/17`. The ignoring caller is 0x005a99fa (0x005a9970, the init path). 0x005a7fc3 and
  0x005a8009 read, 14 unknown. A conditional verdict with reading callers is not an admission (L-126): PROVISIONAL.tsv
  row (status provisional) and a decision row (section Decision). Not C4.

### 2.2 0x005a9c50 admitted, 0x005a9be0 match
- 0x005a9c50 (`re/match/s25/005a9c50.c`, unchanged): match.py 96.7% (59/60 ins, residual: one unfolded `or ecx,eax`
  named in its header). equiv.py: `EQUIV [LAYOUT,VALUE-FLOW]`. equiv_dyn.py cross-check: default `SAME same=16 inc=0
  diff=0`, seeded `SAME same=16 inc=0 diff=0`. ADMITTED.tsv row (source commit c9ff1a67c, tool commit 1ed7ceb1a). Not C4.
- 0x005a9be0: hooks.csv C4 c4=match, match.py 100% BYTE-IDENTICAL (98 B). Binds as a match body.
- On this path 0x005a9970 passes param_2 = 0 (`re/match/s30/005a9970.c`), so bits 4 / 8 of +0xc stay clear and
  0x005a9c50 does not call 0x005aa3d0 (NOT-PROVEN, 86.0%). 0x005aa3d0 and 0x005aa330 are new named stops.

### 2.3 Binding
`audiotick_bind_gen.py`: `PROV_BIND |= {'005a9ab0'}`. Roots unchanged (PORT-TOP-AUDIOSTOP-5A61B0's `roots.txt`).
Plan: 264 bind (229 match, 16 admitted, 12 provisional, 7 replay), 64 reuse, 35 stops. Against the base plan: 0x005a9ab0
stop -> bind, 0x005a9be0 / 0x005a9c50 new binds, 0x005aa330 / 0x005aa3d0 new stops. Wrappers that changed:
`PortTop_005a9970.cpp`, `PortTop_005a62b0.cpp` (0x005a9ab0 bound), `PortTop_AudioStops.cpp`. New TUs
`PortTop_005a9ab0.cpp`, `PortTop_005a9be0.cpp`, `PortTop_005a9c50.cpp` (newfiles.txt +3, rsp +3, allowlist +0).
`bindings_update.py`: BINDINGS.tsv updated 34, added 4. The generator rewrote the other wrappers with LF endings
only; those were not committed.

### 2.4 Builds, check, arms
- VS2022: Build OK, rva-lint 322 known, NEW 0. Object build: Build OK.
- `ddkobj.py check`: IDENTICAL 1564/1603, DIFFERENT 23, not-in-map 16, units linked 1553/1769, refusals 11, provisional
  kept VS2022 205 units (204 + 0x005a9ab0). DIFFERENT = the base 22 + **0x005a9c50** (`_PortTop_Bound_005a9c50`, first
  difference +0x75), newly linked by this checkpoint: the admitted 96.7% body, whose residual is the unfolded
  `or ecx,eax`. 0x005a9be0 IDENTICAL.
- Arms:

  | arm | build | stop | return address | log sha256 (16) |
  |---|---|---|---|---|
  | vs_init_audio | VS2022 | **0x005AA560** after 2.8 s | 0x0023AFDD = `_PortTop_Bound_005aa480+0x5d` (`mashed_re.map`, symbol at 0x0023af80) | 9f153efd1e2fd519 |
  | ddk_init_audio | object | **0x005AA560** after 2.4 s | 0x00048D19 = `_PortTop_Bound_005aa480+0x59` (`mashed_re_ddk.map`, symbol at 0x00048cc0) | 44aaa462f02ca3fd |

  - 0x005aa480 is table entry 7 (slot 0x00632be0); its source calls 0x005aa560 at line 59
    (`re/match/s30/005aa480.c`). So entry 6 (0x005a9970) returned nonzero in both builds.
  - **Self-tests PASS in both arms:** portstate-image 0 of 8 failed, OT_CODEPTR 0 of 367 failed, code-trap 0 of 6 failed.

## 3. Checkpoint 3: 0x005aa560 (provisional)
- Source `re/match/s25/005aa560.c` unchanged: match.py 79.8% (regs 79.8%, ops 91.7%, 184/193 ins), residual named in its
  header (prologue / argument-load order, register allocation).
- equiv.py: `NOT-PROVEN call-args: region ours +0x30 / orig 0x005aa593 call 0x5aad40 arg 0: (esp0+-0x20c) | (esp0+-0x218)
  (fallback: stack address inside lin)`. 0x005aad40 is `mov eax,[0x7dcd20]` 0x005aad40 / `ret` 0x005aad45: it takes no
  arguments, so the compared "arg 0" is the stack pointer at the call. It differs by 0xc because the original pushes
  ebx / ebp / esi / edi (0x005aa574 / 0x005aa57c / 0x005aa57d / 0x005aa57e) before that call. This is a tool limit, not a
  located difference. Listed provisional (PROVISIONAL.tsv). Not C4.
- Binding: `PROV_BIND |= {'005aa560'}`. Plan 272 bind (235 match, 16 admitted, 13 provisional, 8 replay), 64 reuse,
  37 stops. Changes against checkpoint 2: 0x005aa560 stop -> bind; new binds 0x005aa030, 0x005aa7e0, 0x005aa900,
  0x005aa9c0, 0x005aabe0, 0x005adef0 (hooks.csv C4 c4=match) and 0x005aa7a0 (C4 c4=frida-replay); new stops 0x005a9e60,
  0x005a9f10 (0x005aa7a0's callees) and 0x005aac00. Wrappers changed: `PortTop_005a6280.cpp`, `PortTop_005a7f70.cpp`,
  `PortTop_005a8e70.cpp`, `PortTop_005aa480.cpp`, `PortTop_005b8570.cpp`, `PortTop_005b9e30.cpp`,
  `PortTop_AudioStops.cpp`; 8 new TUs; newfiles.txt +8, rsp +8, allowlist +3 (ORIGTOP-ONLY CROSS-TARGET for
  0x005aa030 / 0x005aa9c0 with `Util/PromoLoop_sessionB.cpp`, 0x005aa7a0 with `Lane2/L2_005aa7a0.cpp`); BINDINGS.tsv
  updated 35, added 10.
- Builds: VS2022 Build OK, rva-lint 325 known, NEW 0. Object build Build OK.
- `ddkobj.py check`: IDENTICAL 1570/1610, DIFFERENT 24, not-in-map 16, units linked 1560/1777, provisional kept VS2022
  206 units. DIFFERENT = checkpoint 2's 23 + **0x005aa7a0** (first difference +0x16). It is newly linked by this
  checkpoint and its C4 is by frida-replay (IDENTICAL-FOR-2724), not by byte match: match.py 82.1% (25/28 ins),
  relocheck BAD only on the 0x005a9f10 call it makes (now a named stop). The six match callees are IDENTICAL.
- Arms:

  | arm | build | stop | return address | log sha256 (16) |
  |---|---|---|---|---|
  | vs_init_audio | VS2022 | **0x005AA510** after 3.1 s | 0x0023CF96 = `_PortTop_Bound_005aa560+0xe6` (`mashed_re.map`, symbol at 0x0023ceb0) | 56fc6d7cb2159a79 |
  | ddk_init_audio | object | **0x005AA510** after 2.6 s | 0x0023AE36 = `_PortTop_Bound_005aa560+0xe6` (`mashed_re_ddk.map`, symbol at 0x0023ad50) | 85df95eeced4e078 |

  - 0x005aa560 calls its param_5 (`(*param_5)(param_1,param_6,puVar4,uVar5)`), which 0x005aa480 passes as
    `FUN_005aa510`. Both builds run the VS2022 body of the provisional 0x005aa560, hence the same +0xe6.
  - **Self-tests PASS in both arms:** portstate-image 0 of 8 failed, OT_CODEPTR 0 of 367 failed, code-trap 0 of 6 failed.

## Arms command
`ARMS_STEP=PORT-TOP-AUDIOINIT-SPLIT-5A9970 ARMS_ROOT=D:/MashedWT/PORT-TOP-AUDIOINIT-SPLIT-5A9970_root ARMS_EXTRA="MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1" test-slot py -3.12 <ORCH>/re/orchestrator/mashed_lock.py run --label PORT-TOP-AUDIOINIT-SPLIT-5A9970 -- py -3.12 verify/portorigtop_20261113/audiotick_stops_arms.py vs_init_audio ddk_init_audio`
Private root: robocopy of `D:/MashedWT/PORT-TOP-AUDIOSTOP-5A61B0_root`, its old `mashed_re.log` removed.

## Raw data (`D:/MashedWT/PORT-TOP-AUDIOINIT-SPLIT-5A9970_data/`, sha256)
Checkpoint 1:
- `cp1/vs_init_audio.log` 7e70d89eb7089e4d52b54d1fce45756aa21f215fac0396d7056cd0da6733718c
- `cp1/ddk_init_audio.log` dbb01bfd159c0d0e839779bcd9f3655332c826f623a730c0de7ca56618efc933
- `arms1.out` e49d3caf5f0558209ff62b7d84c21c72c739e486253d49fca8b35775a1a9f736
- `build1_vs.log` 88480385788ab0fcba8c6ba05e2be262347a98ad6b631f3cd1c38a587121dad5
- `build1_ddk.log` de9a78ec526021ae2aca41b35bc0de42ec2f266c1d7ae816205bd748b4cc6bdb
- `ddkcheck1.log` 2f1659990ec3991013d4f2ff649bef5767d6d72e8217f7707a24769d94a61141
- `005add60_before.c`, `add60_match_bytes.txt`, `add60_relocheck.txt`, `refresh_dry.txt`

Checkpoint 2:
- `cp2/vs_init_audio.log` 9f153efd1e2fd519cdeda10de5e5d4d1cc38d1320fe42861064c23c2a88a7f1f
- `cp2/ddk_init_audio.log` 44aaa462f02ca3fd2cfc8df3af4f55967474889ef991e59690abe47a2f2797e4
- `arms2.out` 5daffd2fe02b6cf2c7bd0c552948cff609b9b6914676513c35c8e7b89677ebf8
- `build2_vs.log` 43f362ebf3e00b5e1403e669c549e8f1a365a8af0e170d8ab9da42d3c0f63af7
- `build2_ddk.log` 4392cd741e4db7b9c93890b7ddc655117d7dd5f5cf033a06084b237a2e5a9f32
- `ddkcheck2.log` 025d784c1b14de0399f634aa5e34f1522ef79426f112b101df8fe05c90e87d3e
- `005a9ab0_before.c` 5115bcf9ab285ddb4a36def384877f6dfd257dea9cd1eede6c79f0c8cc32e030
- `9ab0_match.txt` 05149402a77fc13fbc563aa8fae0e79c9ebee91a0353d96129679ac65b6618af
- `equiv_9ab0.txt` d15898782a81b5a1c7fd8d8dbe4cbfefb9631bb23412029eecd4308bbd9dc03c
- `equiv_9c50.txt` 37101b3058798e21c42c4768381e48da23a57c4569a6da36e2228a6eb5b0605f
- `9c50_xcheck_default.txt`, `9c50_xcheck_seeded.txt` a8dfd9519991cc8c03d4ddb2a49569e61ac97d16b107922f7b93961ede9659fc (same text)
- `plan1_dry.txt` b1b387d445f6609443ac7cd2e793fa1fe2735dfd9262b4a5bc8e4933ba28bbce
- `plan1_write.txt` 06afc1782db18c0c0714d616652b463b72c3d9e6176327d8e719ac241a7d9e93
- `exeidx.json` 555f29d122118cce8e1e99bd2b656e5e02295477ee859c1bcca39b5e1965e3e3 (the regenerated index; the committed copy stays the base's)

Checkpoint 3:
- `cp3/vs_init_audio.log` 56fc6d7cb2159a79110fa907fe5b7376b87edd3c0724f48611552515acf53d6b
- `cp3/ddk_init_audio.log` 85df95eeced4e078a898c6905fde84c54c9e3c62289fba9b449e022b5a390fa9
- `arms3.out` 83cb815198feb0b3f34b976f363fcfefa62484e806a31f75afcde94e7a138583
- `build3_vs.log` 4c807ee03dbf8acf654f27a02f1f09d1aa88dfb5ff3a4d2e1e001fd176132975
- `build3_ddk.log` 85dfd54feeb411de3997c9fe558d21c3629087e306e02e317862a016ffe465f3
- `ddkcheck3.log` 52473cdbf8fd7aee7ddee7eba7cf93a6fec9700ef44c166c330108e97fff577e
- `plan2_dry.txt` 23d0db561ed78d6839da01169ca456b08c594ca3def16f5b77d66749621daadf
- `plan2_write.txt` 4c0f5fb23bc2716ca79646169b6a1b9a9db1a51c0972b9cacf475a643b004d7b
- `equiv_a560_before.txt` 158a74205bee6897792305b5162aec5a3b73ad8f19db20c7dc67fb882f82feab
- `a7a0_match.txt` a1b62187fa0d24b4c03d3d61f27af91cf5e6d003f5dc2972c2cc0547c6e302c2
- `exeidx3.json` 7cf41a02f4301c839507018e61baefd447b5e03829aa7bcd564505d65e0fbff2
