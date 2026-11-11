# RESULT RWC-CORE-RUN: RW engine bring-up on the matched RW core (L-124 (C) step 2)

Verdict: **RED.** The row's exit (first menu frame rendered by the RW core, draw calls compared with the original) is
not reached. On the RW core, RwEngineInit and RwEngineOpen succeed. RwEngineStart creates the D3D9 device through the
RWC-DEVICE wrapper. It then faults inside the module constructor of plugin ID 0x40a, in a provisional body (0x004d4380,
17.2%). Both builds fail at the same place. The texture-module freelists exist at that point (section 4).

Branch `orch/RWC-CORE-RUN`, stacked on `orch/RWC-DEVICE` 7b64f91ea and `orch/RWC-DRIVER` 29a753e89, neither merged
yet. The second one is needed for the 0x004c7a70 source. PREREG `68d18009d` was committed alone, before the arms.

Gates: G-CTRL / B not run (top-down row, WORKER_RULES). No tracker edit.

## 1. What landed

| file | what |
|---|---|
| `verify/rwccorerun_20261111/gen_rwcore_tus.py` | generator. Computes the closure from the engine roots (Init / Open / Start / Stop / Close / Term, the driver setters 0x004c9f50 / 0x004c9eb0 / 0x004c9f60) plus every code pointer in the RW core's initialized `.data` (0x00617f00..0x00618600: the device block 0x00618220 and two more). It follows every `(thunk_)FUN_` / `LAB_` name in each source, writes one exe-only TU per source (`RwCore/RwCore_<rva>.cpp`, the source compiled verbatim as C++ under MATCH_PORT, callees and code pointers bound by `#define`), the missing stops, the data seed and the `exe_sources.rsp` block |
| `mashedmod/src/mashed_re/RwCore/` | 235 generated TUs; `RwCore_Missing.cpp`, 91 stops that log the RVA and end the process with `0x5257xxxx`; `RwCore_DataSeed.inc`; `RwCore_Knob.cpp`; `RwCore_Crt.cpp`, the CRT slots RW installs (sprintf .. sscanf, realloc, pow); `RwCore_Hand.cpp`, 13 bodies written from the disassembly |
| `LibRw/RwApiBind.cpp` | engine bindings Init / Open / Start / Stop / Close / Term and the three driver setters first check `RwCore_Enabled()` (`MASHED_RWC_CORE`, default OFF). RenderwareAttachPlugins does nothing on that path. The librw bindings stay in the tree as the fallback (L-124). No new librw binding |
| `RwCore_Knob.cpp` | applies the data seed and code-pointer fix-ups before RwEngineInit and passes 0x200000 as RwEngineInit's third argument. That argument is the dword pushed at 0x00493737 and left on the stack for the call at 0x00493743. After a good Start the knob would probe, run RwEngineStop and return 0, so that 0x00493710 runs its own failure path. It also has a first-AV logger: fault address, the stack words inside the exe image, and the engine probe |
| `re/tools/dual_copy_allowlist.txt` | 54 lines: ORIGTOP-ONLY pairs (RwCore body against a hand or .asi body of the same RVA), plus 2 untagged lines for 0x004d5480 (below) |

Closure (`RwCore_Bodies.tsv`, 348 RVAs):

| kind | count | what |
|---|---|---|
| match100 | 144 | 100% sources |
| provisional | 91 | NOT-PROVEN sources (L-126 (6)), listed in `PROVISIONAL_RWC.tsv`, knob path only, never C4 by this status |
| hand | 13 | `RwCore_Hand.cpp`, from the disassembly, also provisional: 0x004dd140 Direct3DCreate9 import -> `MashedD3D9Wrap_Direct3DCreate9`; 0x004ccef0 / 0x004ccf00 malloc / calloc; 0x004c35b0 / 0x004c35d0; 0x004cca80 (freelist allocator, the 0x007d3fe0 slot); 0x004cfdd0 / 0x004cfdf0 / 0x004d7d70; the four CPUID checks 0x004dcf90 / 0x004dcff0 / 0x004dd050 / 0x004dd0b0 |
| exe | 9 | existing exe bodies of the same RVA: RwInitNullStub-style leaves, RwStr*, RwV3dTransformPoint / Vector, Orig_004d7ff0 |
| missing | 91 | no source and no hand body: mid-function `LAB_` targets, the texture callbacks 0x004c61b0 / 0x004c7200 / 0x004c7490, the Im2D entries 0x004db5b0 / 0x004db840 / 0x004dbc90, and the `_rwcseg` SSE routines 0x005cb000 / 07f / 0ef / 160 / 2a0. Those five are forced missing: the original has them as hand asm, and their sources are raw g2s at 6-24% with register-argument signatures. None was called in the arms |

Source edits made so the sources compile as C++ (verified with `match.py` on each file, before and after):

| source | edit | score |
|---|---|---|
| s19/004c32b0.c (RwEngineInit) | the 32 code-pointer names were `MGLOBAL(undefined1, FUN_x, ...)`, which passed a byte as each plugin constructor. Now they are declared functions passed as addresses (the original's `push imm32`) | 37.7% -> **62.1%** (201/203 ins, was 265) |
| s19/004c5dd0, s19/004c7600, s20/004cde20, s21/004d8000, s21/004d8060 | `typedef int code();` in the guarded `__cplusplus` form already used by other sources | 100 -> 100 |
| rwd01/004c7a70.c, s21/004d7c60.c, s21/004d8fa0.c | pointer casts on 3 lines | 93.2 / 100 / 100, unchanged |
| rwl01/004df120.c | `extern char s_..._00618510[]` -> `MGLOBALA(char, ..., 0x34, 0x00618510)` (52 bytes, the string at 0x00618510 in the anchored binary) | 46.3, unchanged |

`re/analysis/match_ledger.tsv` still lists 0x004c32b0 at 37.7. This row does not write the ledger.

0x004d5480 (100%) is compiled from its own source and again inside `s21/004d5bc0.c` and `s21/004d7480.c`, which carry
the copy for their own codegen. Each copy is renamed `RwCoreCopy_<tu>_004d5480`. The lint counts three ORIG_TOP bodies,
which is outside the ORIGTOP-ONLY class, so these are 2 untagged allowlist lines (D4 burn-down 2 -> 4).

## 2. Builds

| build | result |
|---|---|
| default (`MASHED_NO_DEPLOY=1`, VS2022) | Build OK, `mashed_re.exe` 7e2db176... |
| object build (`MASHED_DDKOBJ=1`) | Build OK, `mashed_re_ddk.exe` 8fae8ea0...; units 818 (parent 583), DDK objects 775 (parent 540). Every RwCore unit links as a DDK object. Refusals: the same 20 `fail:` lines as the spike |
| `ddkobj.py check` | IDENTICAL 689, DIFFERENT 97, not-in-map 3 (`BYTECHECK.tsv`). The parent's 551 rows (`verify/rwcdevice_20261111/BYTECHECK.tsv`) are equal row by row: 0 changed. The 235 new rows: match100 144 IDENTICAL; provisional 89 DIFFERENT and 2 IDENTICAL (0x004c90a0 and 0x004d5bc0: RWC-DRIVER took both to 100%, and the ledger still lists 68.5 / 88.6). No 100% source is DIFFERENT |
| `scripts/lint_rva_bodies.py` | OK, 0 new |

## 3. Knob boots

Private root `D:/MashedWT/RWC-CORE-RUN_root` (junction `original` -> `D:/MashedWT/PORT-TOP-04J_rootK/original`). The arms
ran as one launch of `run_arms.sh`, inside `mashed_lock.py run` inside `test-slot`. The common env was
`MASHED_ORIG_TOP=1 MASHED_WIN_POS=primary-bl MASHED_DET_FRAMES=600`.

| arm | exe | env | result |
|---|---|---|---|
| vs_off | VS2022 | common | rc 0, 8.0 s, 600 frames |
| vs_core | VS2022 | + `MASHED_RWC_CORE=1` | rc 0xC0000005 after 0.7 s |
| ddk_off | object build | common | rc 0, 11.4 s, 600 frames |
| ddk_core | object build | + `MASHED_RWC_CORE=1` | rc 0xC0000005 after 0.5 s |

Knob OFF (`logcmp.py`):
- vs_off vs ddk_off: 1133 / 1133 lines, 0 differ.
- vs_off vs RWC-DEVICE `vs_on_off.log` / `ddk_on_off.log`: 1133 / 1133, 1 differs, the last line ("returned 0 after 600
  frames" here against 3000 there). This row's arms run 600 frames, by the PREREG.

Knob ON, step by step (`KNOB_LINES.txt`, the same in both builds except pointers):

| step | result |
|---|---|
| data seed | 7 ranges, 9 code-pointer fix-ups, 0 faults. The device block 0x00618224 now holds the exe address of the 0x004c7a70 body |
| RwEngineInit 0x004c32b0(0, 0, 0x200000) | **1**. [0x007d3ff8] = 0x007D3EC8 (the original's static RwGlobals), status +0x124 = 1 |
| RenderwareAttachPlugins 0x00493640 | not run on this path (no plugin attaches) |
| RwEngineOpen 0x004c30b0 | **1**. The driver calls `Direct3DCreate9(31)` through the wrapper; IDirect3D9 [0x007d4108] nonzero |
| HardwareSetVideoMode 0x004951f0 | stand-in as before (returns 1, no mode set) |
| RwEngineStart 0x004c2fb0 | CreateDevice runs: asked windowed, forced windowed 640x480, hr 0, device [0x007d4110] nonzero. Then the plugin registry init 0x004d8000 runs the module constructors in registration order (0x00493710's RwEngineInit list: 0x40f, 0x401, 0x40d, 0x402 .. 0x409, 0x412, 0x40a, 0x40b) |
| **first failing step** | the constructor of module 0x40a, 0x004cd850 (100%) -> 0x004e18f0 (100%) -> **0x004d4380 (provisional, 17.2%, 1,411 B)** -> 0x004d4bf0 (100%) reads a bad pointer (`AV ... read 6E275940`). The object build has 0x004d4f90 (provisional, 28.3%) between 0x004d4380 and 0x004d4bf0. The frames are resolved from `mashed_re.map` / `mashed_re_ddk.map` with `mapsym.py` |

0x004e18f0 calls 0x004d4170, 0x004d4dd0 (63.2%), 0x004e1290, 0x004d4f90 (28.3%) and 0x004d4380 (17.2%), then 0x004cd500
on the result. The two lowest are raw g2s sources. [UNCERTAIN] which of 0x004d4dd0 / 0x004d4f90 / 0x004d4380 produces the
bad pointer: the logger records only the fault and the stack. No missing stop fired before the fault.

Two probe labels in `RwCore_Knob.cpp` are wrong:
- "+0x10 fpSystem" reads the RwDevice copy's gammaCorrection (0x3F800000). fpSystem is +0x14.
- "(expect 007D3EC8)" applies only before RwEngineOpen. After Open, [0x007d3ff8] is a heap block
  (vs 0x01623F58, ddk 0x01857F50).

The runs used this text. Fixing it changes no computed value.

## 4. For RWC-BIND-4C5890: the texture-module freelist after this bring-up

The texture module (ID 0x408, constructor 0x004c5f60, `re/match/rwl01/004c5f60.c`) registers before 0x40a and had run
when the fault hit. The probe in the fault handler reads:
- module offset [0x007d4054] = 0x3F4, instances [0x007d4058] = 1;
- texture freelist [RwGlobals + 0x3F4 + 8] = 0x007D405C;
- texdict freelist [RwGlobals + 0x3F4 + 0xC] = 0x007D4030;
- current dictionary [+0x10] = [0x007d4080] = a heap pointer (vs 0x08E4128C, ddk 0x01886594).

So **the freelist exists** once RwEngineStart reaches the module constructors. 0x004c5f60 has already called 0x004c5890
(RwTexDictionaryCreate) through it, on the RW core, and stored the default dictionary at 0x007d4080 (0x004c5f60.c,
`DAT_007d4080 = FUN_004c5890()`). In this closure 0x004c5890 is the 100% source `re/match/s19/004c5890.c` compiled as
`RwCore_004c5890`. [UNCERTAIN] whether the freelists survive the rest of RwEngineStart: the boot ends at the 0x40a fault.

## 5. What remains before the row's exit

1. Module 0x40a's constructor chain: source work on 0x004d4380 (17.2%), 0x004d4f90 (28.3%) and 0x004d4dd0 (63.2%), or
   hand bodies against RW 3.x structs (L-124's middle path). Then the rest of RwEngineStart. The remaining constructors
   (0x40b and the RwEngineStart tail) have not run yet.
2. RenderwareAttachPlugins 0x00493640 (67.9%) and its plugin attaches on the core path, including RpWorld, MatFX and
   PTank. Not in this closure.
3. The rest of the top-down path after RwEngineStart still calls librw bindings (DisplayInit camera, raster, texdict).
   Under (C) those become RW core bodies too. RW-SIZING counts 668 executed RW functions outside RW-Physics. This
   closure covers 348 RVAs of the engine bring-up.
4. The frame on the top-down path is the hand `RenderFrame` stand-in (`Match/PortTop_00492370.cpp:240-247`). The
   original's frame render 0x00492e90 (100% source) and the menu code under it must run before "first menu frame
   rendered by the RW core" can be measured.
5. The draw-call comparison point: `re/frida/menu_draw_burst.py` hooks the RwGlobals Im2D slot (+0x30) on the original.
   With the RW core running, the same slot in the exe is the comparable point. Today's `MASHED_DBG_DRAWSTREAM` sees only
   the B15 bridge.

## 6. Raw data

`D:/MashedWT/RWC-CORE-RUN_data`, sha256 in `RAW_SHA256.txt`: the 4 arm logs, the final build log, the arm driver
transcript and the two exes. Earlier build logs build1..6 and the diagnostic boots vs_core / vs_core2 (before the
probe in the fault handler) are in the same folder, not hashed.
