# RESULT PORT-TOP-AUDIOINIT-SPLIT-5A9970: the init-arm build split, then the stops after it

## Verdict: in progress (checkpoint 1 done)

Base: orch/PORT-TOP-AUDIOSTOP-5A61B0 @ c3b0b6356. Branch orch/PORT-TOP-AUDIOINIT-SPLIT-5A9970. Top-down row: both builds,
ddkobj check, knob boot, self-tests. Gates: not run (top-down row). No menu walk: no menu code changed.

## Stops passed (one line each)

| # | stop | how it was passed | checkpoint |
|---|---|---|---|
| 1 | build split (VS2022 0x005AAF50 / object 0x005A9AB0) | first divergence located: `0x005add60`'s source returned `bool`; VS2022 C++ set only AL, so `0x005ada90`'s `== 1` failed in the VS2022 build only. Source return type -> `int`, still BYTE-IDENTICAL + relocheck OK; both builds now stop at **0x005A9AB0** | checkpoint 1 |

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
