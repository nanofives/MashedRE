# RESULT RWC-FRAME-BOOT-AV (run 48): GREEN

Worker cmwhjoibo01mtp015nkyp9w3q (claude2, Opus 5.5), lead cmwhjm7s5019tp015txdj5koh. Top-down row: both builds,
`ddkobj.py check`, frame-route and knob-off boots. No G-CTRL, no B, no 12 x 4 captures (the change is reached only under
`MASHED_ORIG_TOP=1 MASHED_RWC_CORE=1`, `RwCore/RwCore_005c47e0.cpp`). Branch `orch/RWC-FRAME-BOOT-AV` = base 830f22e19 +
one fix commit (first measured on 790e30336, then restacked on 830f22e19 as the lead asked; all run-48 numbers below on the
restacked build unless marked).

## Verdict

**GREEN.** The frame-route AV after RW-core present 1 reproduces on the integration head without PORT-TOP-BODY-43DFD0, on
both builds. Cause: a converter arity defect in the provisional source `re/match/s28/005c47e0.c`. After the fix both
frame arms run to present 120 and end with the route's designed exit `0x5257F001`, and knob-off boots are unchanged.

## Control run (step 1 as briefed), head 790e30336, no fix

| arm | build sha256 | result |
|---|---|---|
| vs_frameN (`MASHED_ORIG_TOP=1 MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1 MASHED_RWC_FRAME_N=120`) | mashed_re.exe `dce416cb6ea2f99ffe272dfa1cbafe1dbb5e9149d50b2b9766e5c12e5ab68216` | rc 0xC0000005, 242 log lines |
| ddk_frameN (same, object build) | mashed_re_ddk.exe `13089f1afd4b4daa3a2930b71cd6c13bae7aac35c90d44885fd2b11f8e5fdd17` | rc 0xC0000005, 242 log lines |

## Fault site

From the exe's own first-AV logger (`RwCore_Veh`, `RwCore/RwCore_Knob.cpp:108`) and `catch_av.py`
(verify/porttopbody43dfd0_20261114), offsets resolved against each build's .map (`resolve.py`, this folder):

- VS: `AV at 00213838 (exe+0x203838) read 00000004; eax 00635CF0 ecx 00000000 ... esi 00000000` = `_RwCore_004c1040+0x8`.
- DDK: read of `0x4` at `_RwCore_004c1040+0x6`, eax `0x635cf0`, ecx 0.
- Stack (both): `_RwCore_005c47e0+0x210` <- `_RwCore_004e7420+0x155` <- `_MashedRw_ClumpStreamRead_004e7420+0x39` <-
  `_Orig_004b3bf0` <- `_Orig_00401000` (string `cup.dff` on the stack) <- `_Orig_0040bbb0` <- `_Orig_00402750+0x191` <-
  `_Orig_00492370` <- `_PortTop_WinMain`.

Cause. `re/match/s28/005c47e0.c:38` declared `int FUN_004c1040(void);` and line 120 called `FUN_004c1040();` in the
`DAT_00635cf0 != 0` branch. The original (anchored `MASHED.exe.unpatched`, capstone, `calls_to.py` in this folder):

```
0x005c49ef  mov ecx, dword ptr [0x635cf0]
0x005c49f5  push edi
0x005c49f6  test ecx, ecx
0x005c49f8  mov ecx, dword ptr [ebx]
0x005c49fa  mov edx, dword ptr [ecx + eax*4]
0x005c49fd  push edx
0x005c49fe  je 0x5c4a07
0x005c4a00  call 0x4c1040
0x005c4a05  jmp 0x5c4a0c
0x005c4a07  call 0x4c0f00
```

Both callees take the same two arguments: `(*param_2)[buf[12]]` (edx, pushed last = first argument) and the new frame
(edi). The callee's own source declares `int FUN_004c1040(int param_1,int param_2)` (`re/match/rwl01/004c1040.c:42`) and
reads `*(int *)(param_2 + 4)` (`:50`). With no arguments pushed, `param_2` was a stale stack word (0 here). Not a
regression: neither source nor any `RwCore/` / `LibRw/` file changed between d1e5ac586 and 790e30336 (`git log`).

## Fix (commit 4d1137460 on 830f22e19)

`re/match/s28/005c47e0.c`: the declaration takes `(int p0, int p1)` (the callee source's prototype) and the call passes
`(*(int *)(*param_2 + 4 * buf[12]), iVar4)`, the same pair the `FUN_004c0f00` call on the other branch passes. Header score
and the two provisional tables (`re/match/PROVISIONAL.tsv`, `verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv`) updated
80.0 -> 80.8. 0x005c47e0 stays a provisional NOT-PROVEN body (knob path only, never C4).

| check | before (830f22e19 source) | after |
|---|---|---|
| `match.py re/match/s28/005c47e0.c` (DDK toolchain) | 80.0% (regs 81.2%, ops 87.3%) | **80.8%** (regs 82.0%, ops 89.0%) |
| `match.py --bytes` | 321 differing unmasked bytes of 716, first at +0x28 | 321 of 716, first at +0x28 |
| `relocheck.py` | BAD, checked 27, 15 mismatched | BAD, checked 27, 15 mismatched (same list head: +0x1a3 `_FUN_004c42d0` ...) |

relocheck BAD is the unmatched body's offset shift (both versions), not a new defect.

## Builds and the object-build check (restacked on 830f22e19)

- `cmd /c "set MASHED_NO_DEPLOY=1&& set MASHED_DDKOBJ=1&& mashedmod\build.bat"`: OK. mashed_re.exe
  `709b70fae0bfe24171bed7a8f0f2665cc123fdc94431363c10d4079f7c0810fa`, mashed_re_ddk.exe
  `1ba19a69dc99b6f21d0823113f90cc4d69c4ebe50a66fda6bfb212d06b40c8ca`.
- Fresh base check (830f22e19 built in this worktree before the cherry-pick): IDENTICAL 1547/1590, DIFFERENT 28, not-in-map
  15, units linked 1533/1741, refusals 9 (externalized-static-register-convention 5, wrapper-redefines-globals 4).
- Branch check: the same numbers and the same DIFFERENT list. BYTECHECK.tsv of base and branch are byte-identical
  (sha256 `08bce2dd...a794` both). **No newly DIFFERENT unit.** (0x005c47e0 is below 100% and keeps its VS2022 body in the
  object build.)
- `scripts/lint_rva_bodies.py`: OK, no NEW duplicate bodies (313 known).

## Arms (private root D:/MashedWT/RWC-FRAME-BOOT-AV_root, test-slot outside, mashed_lock inside, `arms.sh` in this folder)

| arm | build | result |
|---|---|---|
| vs_frameN_r48 | VS | rc **0x5257F001** after present 120: `MASHED_RWC_FRAME: boot ends after present 120 on the RW core`, 323 lines, no AV |
| ddk_frameN_r48 | object | rc **0x5257F001**, same lines, no AV |
| vs_frameN_det600 (+ `MASHED_DET_FRAMES=600`) | VS | rc 0x5257F001, same |
| vs_off_r48 (`MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=600`) | VS | rc 0, `WinMain 0x00492370 returned 0 after 600 frames`, 1847 lines |
| ddk_off_r48 | object | rc 0, same, 1847 lines |
| (790e30336 + fix) vs_frameN_fix / ddk_frameN_fix | both | rc 0x5257F001, 323 lines (`catch_av.py` exit stack: `ExitProcess(0x5257f001)` from `_RwCore_FrameShowRaster+0xfb`) |

0x5257F001 is the frame route's own end code after present N (`RwCore_FrameShowRaster`), the value RWC-FRAME-CLUMP run 44
recorded and the PREREG of PORT-TOP-BODY-43DFD0 expects for vs_frameN.

## The RWC-FRAME-CLUMP question (bar item f)

The [UNCERTAIN] was whether CLUMP's arms passed because its `run_arms.sh` COMMON adds `MASHED_DET_FRAMES=600`. Measured
on an exe without this fix (orch/PORT-TOP-BODY-43DFD0 on 830f22e19, `MASHED_PROV_43DFD0` off): `nofix_frameN` and
`nofix_frameN_det600` both end rc 0xC0000005 with `AV at 00218438 (exe+0x208438) read 00000004; eax 00635CF0`.
**`MASHED_DET_FRAMES` is not the reason.** Why orch/RWC-FRAME-CLUMP 9f48df42f passed stays [UNCERTAIN]. Test: vs_frameN on a
9f48df42f build with the `RwCore_Veh` log, then look for the `cup.dff` load under `Orig_0040bbb0` (CLUMP changes
`RwCore_Knob.cpp`, `RwCore_Missing.cpp` and `PortTop_StandIns05.cpp`, which can change which clumps load before present 1).

## Tooling notes

- `catch_av.py` (on orch/PORT-TOP-BODY-43DFD0) now skips the first-chance exceptions `OutputDebugString` raises (Frida type
  `system`). The run-47 version reported three of them from `DbgPrintf_004987b0`, then killed the game before any AV. It
  also reports process exit (code + stack). The Frida 17 API has no static `Module.findExportByName`.
- Builds waited once for a merge-train PRIORITY-HOLD (05:47 to 05:52).

## Raw files (D:/MashedWT/RWC-FRAME-BOOT-AV_data)

| file | sha256 |
|---|---|
| vs_frameN.log (control, VS) | 8069d4d2f3294b57f46251c89e2e5cd4c79f57b8770f64d5508183f27ed0a791 |
| ddk_frameN.log (control, DDK) | 53ab3389c2c624535636e285ffae81f938a22c0d88ad359d534e83b111e57c94 |
| av_ddk_frameN.json | 6b3b3475fe858d088c6478637560d9e757011758f123b2f4e824d40153f80191 |
| vs_frameN_fix.log | e94eb0b4c4145e15c7cb1038485d65e25cba76377a81860e8c56d930cfa7b9e4 |
| ddk_frameN_fix.log | 239e955d376162c66753d4acffbff867eb76921070178814b89da8f005296afc |
| vs_frameN_r48.log | d772492d376b4e3b733f2e2c243ff2b9ae38481b3c297c8609783f1a3828a0d5 |
| ddk_frameN_r48.log | 0b499923c5125d462bfa8e90317c242fb29fc5599f02556979a27545433ef0c0 |
| vs_frameN_det600.log | 866a9e6550a4d2e2e8cba31a10e8d708f580f621f1696d1a4170690655cdc0b6 |
| vs_off_r48.log | aa1a23c236ec9e1d12d643d6463eb6721f1f6ae1a54536351fe204ead2762de3 |
| ddk_off_r48.log | c25aad37bd5e331153c426daffdfa83ae090f951d72df34a99e52351696902e6 |
| ddkcheck_base830.txt = ddkcheck3_r48.txt | e8828bcfdb1c8940c7385bb87a7e2c773fc0874a7657aec9fb51075722b7f75c |
| BYTECHECK_base830.tsv = BYTECHECK_r48.tsv | 08bce2ddaf840554562ded2c92f27df0f6c4d338c12b9657e232d60af327a794 |
| match_005c47e0_before.txt | 3f08c10a96f0df0066fb0cf84d25d475c0375d98fd222bfc7459d96df6d8cc13 |
| match_005c47e0_r48.txt | 5c5be2a5c5f719db451cc10af35fbba9f54b8d187b66b20d5583fd8624a626bd |
| relocheck_005c47e0.txt | de4e44abb3381a17e1dd524b8153cf2654efa2c853e8f3eb4411946f85d0aa16 |
| relocheck_005c47e0_base.txt | 0f0dab23f5db536ac09d753c92c52f516626b3cf75c366ae7433da8ee4027090 |
| slot1.out .. slot5.out | slot outputs (slot4 9bffc5d996d6cbf67a3cd7ca2d4b290988fb94d67a7b42d7f45eef781fcfc06f) |
| (PORT-TOP-BODY-43DFD0_data) nofix_frameN.log, nofix_frameN_det600.log | listed in RESULT_PORT-TOP-BODY-43DFD0.md |
