# RESULT PORT-TOP-AUDIO-5A8F30-STOPS: the init-arm stops from 0x005A8F30 on

## Verdict: DECISION (2 checkpoints GREEN; both builds now stop at 0x005AEF30, the audio worker-thread start, census order 42)

- **Self-test result: PASS** in every arm, both builds, both checkpoints (portstate-image 0 of 8 failed, OT_CODEPTR 0 of 367
  failed, code-trap 0 of 6 failed, pages 459/459).
- Not reached: 0x0045d460. Every later init row is behind 0x005aef30, which starts a real thread (section 2.4, Decision),
  so RO-AUDIOINIT-45D460-REACH is not answered.
- Not merged with orch/integration yet (the branch is based on 4c57c7375, which contains orch/integration 215d5bbb9).
- No newly DIFFERENT unit in the object build: DIFFERENT 25 = the base's 26 minus 0x005a6340, which is byte-identical
  now (section 1.5).
- Base: orch/PORT-TOP-AUDIO-5AA3D0-STOPS @ 4c57c7375 (contains orch/integration 215d5bbb9). Top-down row: both builds,
  ddkobj check, knob boot (init arms), self-tests. Gates: not run (top-down row). No menu walk: no menu code changed.
  equiv.py = tool commit 1ed7ceb1a, no diff on this branch.

## 1. Checkpoint 1: 0x005a8f30 and the bit-length macro family, the 0x005bbb20 tables

### 1.1 0x005a8f30 (census order 23): source from the listing, BYTE-IDENTICAL
- The original expands a nested bit-length macro at every use of its argument:
  `HB32(v) = (v & 0xffff0000) ? HB16(v >> 16) + 16 : HB16(v)`, `HB16(w) = (w & 0xff00) ? DAT_00617ff8[w >> 8] + 7 :
  DAT_00617ff8[w] - 1`, with `v = M = ((short)(1 << FUN_005aee20(0x28)) != 0) ? (1 << FUN_005aee20(0x28)) & 0xffff : 1`.
  Evidence in the listing: 14 calls to 0x005aee20 between 0x005a8f4b and 0x005a913c; the `+7` / `+0x10` pair
  (0x005a9011 / 0x005a9014) is HB16(...) + 16; `sar 0x18` / `sar 0x10` / `sar 8` at 0x005a9007 / 0x005a9065 / 0x005a90fc;
  the `(v >> 16) & 0xff00` test reads byte 3 of v from the spill slot (`mov ah,[esp+0xf]` 0x005a8fd0). The branches Ghidra
  removed as unreachable are the arms the macro expansion keeps.
- `re/match/s25/005a8f30.c`: 30.4% (raw g2s) -> `match.py --bytes` 100.0%, 168/168 ins, 589 B BYTE-IDENTICAL; relocheck
  OK checked=24; equiv.py `EQUIV [BYTE-IDENTICAL]`. Tail stores in listing order with the +0x40 or right after +0x28
  (Ghidra's order scored 98.8%). The NOT-PROVEN masked-shift form of the old source is gone (the shift count is the int
  return of 0x005aee20, no `& 0x1f`).
- The same macro makes six more constructors byte-identical (same descriptor call / macro / store tail shape):

  | rva | source | before | now | relocheck |
  |---|---|---|---|---|
  | 0x005a6340 | re/match/s25/005a6340.c | 82.4% admitted (volatile spill) | 604 B BYTE-IDENTICAL | OK 27 |
  | 0x005a8060 | re/match/s25/005a8060.c | 40.0% | 744 B BYTE-IDENTICAL | OK 43 (CloseHandle import unchecked) |
  | 0x005b9f30 | re/match/s27/005b9f30.c | 29.2% | 610 B BYTE-IDENTICAL | OK 27 |
  | 0x005c7a90 | re/match/s28/005c7a90.c | 32.0% | 617 B BYTE-IDENTICAL | OK 27 |
  | 0x005c6cc0 | re/match/s28/005c6cc0.c | 40.6% | 603 B BYTE-IDENTICAL | OK 26 |
  | 0x005c8120 | re/match/s28/005c8120.c | 32.0% | 618 B BYTE-IDENTICAL | OK 29 |

  0x005a6340 leaves ADMITTED.tsv (its row is removed; it binds as a byte match now). Previous sources are in the data
  folder (`<rva>_before.c`).

### 1.2 The tables 0x005bbb20 walks (Ghidra had no functions)
0x005bbb20 (bound, C4) calls 0x005ad080 on three tables: (0x006351ec, 4) at 0x005bbb20 / 0x005bbb22, (0x00635194, 9) at
0x005bbb33 / 0x005bbb35, (0x006351dc, 2) at 0x005bbb46 / 0x005bbb48; 0x005ad080 returns 0 when an entry returns 0, and
0x005bbb20 then calls 0x005bbb70 (0x005bbb5f). The census rows 29 to 53 (0x005b9f30, 0x005b8680, 0x005c7a90, 0x005c6cc0,
0x005a8060, 0x005c8120, 0x005b74c0, 0x005b6be0, 0x005bce80) are the constructors these entries call. All 15 entries got
a pc_funcs.csv row (size from the listing to the `ret` before the nop pad) and a hooks.csv C1 row through
`re/tools/c1_add.py` (CHANGELOG lines with it).

- **0x006351ec (4 entries)** 0x005aee40, 0x005b7fe0, 0x005b7f80, 0x005b7f20: `if (FUN_005ad570(&obj) == 0) return 0;
  FUN_005ad540(&obj, &desc, 0); FUN_005ad420(&obj, t1, n1, t2, n2); return &obj;` 68 B each, BYTE-IDENTICAL, relocheck OK 10.
- **0x006351dc (2 entries)** 0x005bb850 (150 B, BYTE-IDENTICAL with a pointer local for `[eax+0x54]`; the double-deref
  spelling scored 88.4%), 0x005c9230 (101 B, BYTE-IDENTICAL). relocheck OK 15 / 12.
- **0x00635194 (9 entries)** 0x005b9ea0, 0x005b85e0, 0x005c7a00, 0x005c6c20, 0x005a7fe0, 0x005c80a0, 0x005b7440,
  0x005b6b80, 0x005bce20: same shape as 0x005a8ea0 (create through 0x005a9ab0, constructor, 0x005aa200 table load,
  0x005a9d00 on each failure). Best 77.1% to 91.1%, with the residual of 0x005a8ea0: the DDK cross-jumps the identical
  failure tails (`push esi / call 0x005a9d00 / add esp,4 / xor eax,eax / pop esi / ret`) and the early `pop esi / ret`,
  which the original keeps apart. equiv.py on all nine: `EQUIV-IF-CALLERS-IGNORE(EAX) [LAYOUT] callers 0, scan says
  ignore 0/0` (indirect only; the walker 0x005ad080 tests the return). Conditional verdict with a reading caller: listed
  PROVISIONAL.tsv and bound, decision requested (section Decision). Not C4.

  | rva | slot | ctor | match.py |
  |---|---|---|---|
  | 0x005b9ea0 | 0x00635194 | 0x005b9f30 | 77.6% 41/49 |
  | 0x005b85e0 | 0x00635198 | 0x005b8680 | 79.2% 45/53 |
  | 0x005c7a00 | 0x0063519c | 0x005c7a90 | 77.6% 41/49 |
  | 0x005c6c20 | 0x006351a0 | 0x005c6cc0 | 79.2% 45/53 |
  | 0x005a7fe0 | 0x006351a4 | 0x005a8060 | 91.1% 43/45 |
  | 0x005c80a0 | 0x006351a8 | 0x005c8120 | 91.1% 43/45 |
  | 0x005b7440 | 0x006351ac | 0x005b74c0 | 77.1% 40/48 |
  | 0x005b6b80 | 0x006351b0 | 0x005b6be0 | 88.6% 33/35 |
  | 0x005bce20 | 0x006351b4 | 0x005bce80 | 88.6% 33/35 |

- `match.py --flags` overrides do not reproduce the default score (44.1% with the default flag string), so a flag
  search does not test the tail split. [UNCERTAIN] line PORT-TOP-AUDIO-5A8F30-STOPS (a).

### 1.3 Promotions
`re/promotions/PORT-TOP-AUDIO-5A8F30-STOPS.tsv`: 13 rows (the 7 constructors of 1.1 and the 6 table entries of 1.2), dry
run `batch: 13 pass, 0 refused` (`c4_dry2.txt`). No `--apply` on the branch.

### 1.4 Binding
- `audiotick_bind_gen.py`: `PROV_BIND |=` the nine 0x00635194 entries; `STOP_FORCE |= {'005aef30'}`: 0x005a8060 now binds
  and calls 0x005aef30 (0x005a8328), which calls `__beginthread` (0x005c2e79, no exe body) and starts the second thread
  (census order 42); it stays a stop until the init stops are done.
- Roots: `verify/portorigtop_20261113/audio_5a8f30/roots.txt` = the 67 base roots + the 15 table entries (83 with the
  base's other entries). Plan: 356 bind (305 match, 26 provisional, 17 admitted, 8 replay), 69 reuse, 78 stops.
- `codeptr_inventory.py --write`: slots 0x00635194..0x006351b4, 0x006351dc / 0x006351e0, 0x006351ec..0x006351f8 bound
  (31 slots bound, was 16). rsp +50, allowlist +3 (ORIGTOP-ONLY CROSS-TARGET 0x005af010, 0x005b0b90, 0x005b0f10, bodies new
  to the closure that also exist in Lane2 / Util TUs), BINDINGS.tsv +87.

### 1.5 Builds, check, arms
- VS2022: Build OK, rva-lint 333 known, NEW 0. Object build: Build OK.
- `ddkobj.py check`: IDENTICAL 1643/1689, DIFFERENT 25, not-in-map 21, units linked 1639/1869, refusals 11, provisional
  kept VS2022 219 units + 5 fns. DIFFERENT = the base list (RESULT_PORT-TOP-AUDIO-5AA3D0-STOPS 3.4, 26 units, the same
  tree) minus 0x005a6340. All 13 new byte-matched units IDENTICAL (`bytecheck1.tsv`).
- Arms:

  | arm | build | stop | return address | log sha256 (16) |
  |---|---|---|---|---|
  | vs_init_audio | VS2022 | **0x005B8680** after 10.4 s | 0x0023FB01 = `_PortTop_Bound_005b85e0+0x41` (`mashed_re.map`, symbol at 0x0023fac0) | b5fa6d03547a4bba |
  | ddk_init_audio | object | **0x005B8680** after 8.4 s | 0x0023E781 = `_PortTop_Bound_005b85e0+0x41` (`mashed_re_ddk.map`, symbol at 0x0023e740) | 9372d292e9ba1492 |

  - Both builds passed 0x005a8f30, 0x005b34f0 (0x005a8ea0's last callee), the four 0x006351ec entries and the first
    0x00635194 entry 0x005b9ea0 (0x005b9f30, 0x005aa200, 0x005bc800, 0x005a9de0), then entered 0x005b85e0, which calls
    0x005b8680 at 0x005b861a (census order 34, C2, no source at this point).
  - **Self-tests PASS in both arms:** portstate-image 0 of 8 failed, OT_CODEPTR 0 of 367 failed (188 slots, 31 bound),
    code-trap pages 459/459, read / write ok.

## 2. Checkpoint 2: 0x005b8680, 0x005b74c0, 0x005b6be0 (macro family), stop at 0x005aef30

### 2.1 Sources
Same constructor shape as 1.1 (generator spelling, stores in listing order):

| rva | source | before | now | relocheck |
|---|---|---|---|---|
| 0x005b8680 | re/match/s27/005b8680.c (new) | no source | 604 B BYTE-IDENTICAL | OK 27 |
| 0x005b74c0 | re/match/s27/005b74c0.c | g2s source | 611 B BYTE-IDENTICAL | OK 28 |
| 0x005b6be0 | re/match/s27/005b6be0.c | g2s source | 611 B BYTE-IDENTICAL | OK 28 |

Promotion TSV now 16 rows. 0x005bce80 (census 53, folded form: `movsx ecx,[0x617ff9]` / `dec ecx` at 0x005bce9d, the
macro on the constant 1) is best 73.9%: the original keeps 0x7b8 in eax (0x005bce92) and ors it (0x005bceaf), ours uses
immediates. Not reached yet, not bound.

### 2.2 Binding
- Plan: 373 bind (322 match, 26 provisional, 17 admitted, 8 replay), 70 reuse, 88 stops; rsp +17, allowlist +1
  (ORIGTOP-ONLY DUP-IN-TARGET 0x005ad2e0 with `Audio/AudioQueuePop.cpp`), BINDINGS.tsv +28. The code pointers the three
  constructors store bring their own closures (0x005b8be0, 0x005b7880, 0x005b7010, 0x005b70e0 and callees).
- The first VS2022 link failed on two Ghidra thunk names: `re/match/s25/005ab180.c` names its body `thunk_FUN_005ab620`,
  and `re/match/s27/005b7880.c` calls `thunk_FUN_005b1500` (0x005b15d0 is `jmp 0x5b1500`, called at 0x005b789a). Generator
  `EXTRA_PRE` defines for both (same form as 0x005aafe0 / 0x005ae330).

### 2.3 Builds, check, arms
- VS2022: Build OK, rva-lint 334 known, NEW 0. Object build: Build OK.
- `ddkobj.py check`: IDENTICAL 1660/1706, DIFFERENT 25 (the checkpoint 1 list exactly), not-in-map 21, units linked
  1656/1886, refusals 11. 0x005b8680 / 0x005b74c0 / 0x005b6be0 IDENTICAL. No newly DIFFERENT unit.
- Arms:

  | arm | build | stop | return address | log sha256 (16) |
  |---|---|---|---|---|
  | vs_init_audio | VS2022 | **0x005AEF30** after 2.3 s | 0x0023F0DC = `_PortTop_Bound_005a8060+0x18c` (`mashed_re.map`, symbol at 0x0023ef50) | 15f9da1bf56b6dc6 |
  | ddk_init_audio | object | **0x005AEF30** after 2.3 s | 0x0004B4BD = `_PortTop_Bound_005a8060+0x2cd` (`mashed_re_ddk.map`, symbol at 0x0004b1f0) | 054fede33c32ddcc |

  - Both builds passed 0x005b85e0 (0x005b8680, 0x005aa200, 0x005bbbb0), 0x005c7a00 (0x005c7a90, 0x005c96e0), 0x005c6c20
    (0x005c6cc0, 0x005c9680), then entered 0x005a7fe0 -> 0x005a8060, which calls 0x005aef30 at 0x005a8328 after the two
    0x005aeea0 calls and 0x005aef00 (0x005a8315). 0x005aef30 is the forced stop of 1.4.
  - **Self-tests PASS in both arms:** portstate-image 0 of 8 failed, OT_CODEPTR 0 of 367 failed (31 bound), code-trap
    0 of 6 failed.

### 2.4 What 0x005aef30 does (the decision below)
- 0x005aef30 stores param_2 at param_1+8 (0x005aef3d), calls `__beginthread(0x005aef70, param_1[4], param_1)`
  (0x005aef41 / 0x005aef46, target 0x005c2e79), returns 0 on -1 (0x005aef4e), else stores the handle at param_1[0] and calls
  `SetThreadPriority(handle, param_1[3])` (import slot 0x005cc0bc, call 0x005aef5e).
- Thread entry 0x005aef70 (40 B, no Ghidra function): calls `param_1[1](param_1[2])` (0x005aef78); when that returns
  non-zero, `ReleaseSemaphore(*ret, 1, 0)` (0x005cc094) and `Sleep(-1)` (0x005cc034). Scratch source BYTE-IDENTICAL
  (`thread_scratch_scores.txt`; not installed, not bound).
- param_1[1] = 0x005a84d0, passed by 0x005a8060 to 0x005aef00 (`push 0x5a84d0` at 0x005a8301). 0x005a84d0 (131 B, no
  Ghidra function) loops until DAT_007dcb4c != 0 (tests at 0x005a84d0 / 0x005a8536): `WaitForSingleObject(DAT_007dcae0,
  -1)` (0x005a84f1), walk the list at 0x007dcad8 calling 0x005aab00 and 0x005aab70 on each node - 0x14
  (0x005a8501..0x005a851f; both callees bound, C4), `ReleaseSemaphore(DAT_007dcae0, 1, 0)` (0x005a852c), `Sleep(0x11)`
  (0x005a8530); then `ReleaseSemaphore(DAT_007dcb50, 1, 0)` (0x005a854d) and returns 0. Scratch source 91.7% (48/48 ins,
  register residual).
- Binding 0x005aef30 therefore needs an exe `__beginthread` and starts a real second thread that runs this loop every
  0x11 ms on the list the main thread fills. 0x0045d460 is not reachable without passing 0x005aef30: 0x005a8060 is reached
  from entry 5 of the 9-entry table 0x00635194, and 0x005ad080 stops at the first entry that returns 0.

## Decision
- **L-173** (filed by the lead): the nine 0x00635194 entries (table in 1.2), `EQUIV-IF-CALLERS-IGNORE(EAX)` with the
  walker 0x005ad080 reading EAX, same class as L-171 (0x005a8ea0). Listed provisional and bound meanwhile.
- **Requested now (this worker does not file it): the audio worker-thread start 0x005aef30** (section 2.4). Options:
  (a) bind it: an exe `__beginthread` for 0x005c2e79 (the CRT row, C4 libid, waived-platform-crt, no exe body today) plus
  sources for 0x005aef70 (byte-identical in scratch) and 0x005a84d0 (91.7%, would be provisional), so the knob path starts
  the real worker thread; (b) keep 0x005aef30 a stop: the init arms cannot pass census order 42 and RO-AUDIOINIT-45D460-REACH
  stays open; (c) a default-off knob stand-in for 0x005aef30 that returns param_1 without starting a thread (PREREG), so the
  main-thread init can reach 0x0045d460 with no worker thread. Recommendation: (c) first to answer
  RO-AUDIOINIT-45D460-REACH, then (a) as its own row.

## Lock status at build/run time
Times -0300; the line is printed in the same command right before the step.

| checkpoint | step | time | status |
|---|---|---|---|
| 1 | (waiting) | 13:21:22 | `HELD merge-train:20261116_130150`, `PRIORITY-HOLD`: waited in an until-loop to 13:33:22 (`free`, `PRIORITY-LINGER`) |
| 1 | VS2022 build | 13:34:01 | `free 'mashed_machine'`, `PRIORITY-LINGER ... 58s more` (no HOLD, no SOLO) |
| 1 | object build | 13:38:31 | `free 'mashed_machine'` |
| 1 | arms | 13:47:08 | `free 'mashed_machine'`; test-slot waited for memory, then `mashed_lock.py run` held the lock |
| 2 | VS2022 build (try 1, link failed, 2.2) | 14:03:49 | `free 'mashed_machine'` |
| 2 | VS2022 build (try 2) | 14:05:53 | `free 'mashed_machine'` |
| 2 | object build | 14:07:19 | `free 'mashed_machine'` |
| 2 | arms | 14:13:27 | `free 'mashed_machine'`; test-slot waited for memory 570 s, then `HELD 'mashed_machine' pid=13112` |

## Arms command
`ARMS_STEP=PORT-TOP-AUDIO-5A8F30-STOPS ARMS_ROOT=D:/MashedWT/PORT-TOP-AUDIO-5A8F30-STOPS_root ARMS_EXTRA="MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1" test-slot py -3.12 <ORCH>/re/orchestrator/mashed_lock.py run --label PORT-TOP-AUDIO-5A8F30-STOPS -- py -3.12 verify/portorigtop_20261113/audiotick_stops_arms.py vs_init_audio ddk_init_audio`
Private root: robocopy of `D:/MashedWT/PORT-TOP-AUDIO-5AA3D0-STOPS_root`, its old `mashed_re.log` removed.

## Raw data (`D:/MashedWT/PORT-TOP-AUDIO-5A8F30-STOPS_data/`, sha256)
Checkpoint 1:
- `cp1/vs_init_audio.log` b5fa6d03547a4bba5bf939f23d249f60c1a2759dde621b0b0b8f14c73a6ed77e
- `cp1/ddk_init_audio.log` 9372d292e9ba14920bf5376d51d18c18da3678cacd9b2a24f6f1643ba13a3a9d
- `arms1.out` a2fb2017ee706707f3a0cbbbcee3c4bfc078e9dabe308b8e64c5677e0b0bf294
- `build1_vs.log` 432ca8e01109adb7798607cbc75677af1c8c07831d3c7d144e6495d246cbf112
- `build1_ddk.log` 9b0e160e83cabb5845812e6c62fbc0797c5d889fb02acd14d8448b6374e9d8b5
- `ddkcheck1.log` f7b4c1327d6eb8ccf2282fa3e5164f2280281ad9182962653c0d1525edf8bc4d
- `bytecheck1.tsv` 0ec7e228def9c60a08c5514485a8b0063e35aea453be24e9651df813c7dcba47
- `plan1_dry2.txt` a30a1035ef9bdb895265530cf0f17e3ae920825cb29321d2675446967f5398fe
- `plan2_dry.txt` 883bceb0b66c6e686f0f57a312a863121dc8cc45cfb7e57c2ee3768cd40177f2
- `plan2_write.txt` 9fba9adbf55e2b706ad0910fad6047853005d3c40b4db122b8ed0c18495faff9
- `codeptr2.txt` 4a9df00bed6d82a2dd0244434a4d3e0b6e07d2d0548b605680cceb302bd574b9
- `equiv_wrappers.txt` d2ebc2410ecd1f26a3c4b007ba7ca7a72d702f52edb2cb3c92965b71353effcf
- `c4_dry1.txt` d04bd2854d8734c4924a62b318c1091a20006a8664608b12ce8f991f49e6beed
- `c4_dry2.txt` cebc9f703f2254fbacd1648db2dd7c9083ee03cf4b118035ecfc66795bec19a2
- `005a8f30_before.c` b5772acf200dd3c81e2b4f93c47b7e52f9ca42fa0b1ab09885a505d0adb1b55c
- `005a6340_before.c` d22b71b6d91a01a8d908d55a3c9ef56f3878114a8c1f40fcd30847665c6c8c92
Checkpoint 2:
- `cp2/vs_init_audio.log` 15f9da1bf56b6dc60b7e5838c552a7d2e77ef295a063bb73db0f0fd85d2cf35f
- `cp2/ddk_init_audio.log` 054fede33c32ddccd8c017b433fc98693b5568001e589159069af43e9f636fe0
- `arms2.out` 6e47b3aef3eff08426a82eb4e2b1e029a9ac88be550d6d3dc00d7226780762a0
- `build2_vs.log` f170ed2c2f28b53fe245ffc69123c7d23ec30594821431fcb69ea6cac7be678d
- `build2_ddk.log` 39ce5689ab8ba1c0360980081d38b8f8dbdb85b3c6b776626c16e3f9e146d986
- `ddkcheck2.log` dbf767263cc361a96b9e283162c91c72ff2b34d3d029649c39e85d1813ca03a8
- `bytecheck2.tsv` 8d8062096e9ac119c4d43d7e908f3136d57a65a68da36185a12fd66d6cf21df3
- `plan3_dry.txt` a4caa836a72f97fe5c79433183bad9825802fad14615b04af58effbe8c47931f
- `plan3_write.txt` eb0257c5b1ce2b79d7fa405974968c1cdf13415d9869f969fcb9a545b0021768
- `codeptr3.txt` 4a9df00bed6d82a2dd0244434a4d3e0b6e07d2d0548b605680cceb302bd574b9
- `005b74c0_before.c` bc7489423a5bc242171973c81b5b6f204216416a83368a337604a40779a38f11
- `005b6be0_before.c` 2d26b60fd48a0a01b78019efd39300055fe345f35308742d754dea335990a9f6
- `lockstatus.txt` (checkpoints 1 and 2) 6d20b72687f2e8cea922de31bd6357f0827243237c287de33ff93187d1a6e7ee
