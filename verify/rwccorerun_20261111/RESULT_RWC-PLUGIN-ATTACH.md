# RESULT RWC-PLUGIN-ATTACH: plugin attach on the matched RW core (L-124 (C) step 3)

Run 39, 2026-11-12. Branch `orch/RWC-PLUGIN-ATTACH`, based on `orch/RWC-CORE-RUN` e1205e9a2 (run 38, engine lifecycle
on the core), so that branch's progress lands with this one. Same knob (`MASHED_RWC_CORE`, default OFF, read only with
`MASHED_ORIG_TOP=1`) and the same four arms as `PREREG_RWC-CORE-RUN.md`. Top-down row: G-CTRL / B / 12 captures not run
(WORKER_RULES). No tracker edit.

**Verdict (run 40): GREEN for what landed, with the Im2D comparison not done.** Branch `orch/RWC-PLUGIN-ATTACH` @
6e4430812 (plus the RESULT commit). Self-test: **PASS** in every core arm (RwTexDictionaryCreate binding self-test 0 of
11 checks failed, in vs_core / ddk_core / vs_frame / ddk_frame), and the RWC-DEVICE self-test PASS (0 fail) in
both frame arms.

- Plugin attach 0x00493640 runs on the RW core and returns 1. **RwEngineStart now returns 1 on the core** with
  every plugin attached (Init, Attach, Open, Start, Stop, Close, Term all succeed), in both builds.
- New sub-knob `MASHED_RWC_FRAME` (only with `MASHED_RWC_CORE=1`, default OFF): the bring-up 0x00493710 returns 1
  with its tail on the core, DisplayInit 0x004921d0 runs with 71 of 88 librw bindings sent to their core bodies,
  and **the first frame is presented on the RW core** (RwRasterShowRaster 0x004c7730 = the raster, 1 Present), in
  both builds (exit 0x5257F001).
- Not done: the frame has no Im2D draw yet (the loading-disc draw 0x00428450 has no body on the top-down path), so
  the Im2D comparison against the original and the matched frame render 0x00492e90 remain. See section 7 and the
  Handoff.
- Default and knob-OFF paths unchanged: vs_off vs ddk_off 1134 / 1134 lines, 0 differ, 600 frames each. Top-down
  row, so G-CTRL / B / 12 captures not run (WORKER_RULES). No tracker edit.

## 1. Plugin attach 0x00493640 on the core

The run-38 WIP (`attach_wip_run38.patch`) applied: `MashedRw_AttachPlugins_00493640` (`LibRw/RwApiBind.cpp`) calls
`RwCore_AttachPlugins` under the knob, which runs the matched `re/match/s15/00493640.c` (67.9%, provisional) compiled
as `RwCore_00493640`, between RwEngineInit and RwEngineOpen as in 0x00493710. No librw binding added (L-124).

First knob boot with only the four generator gaps fixed (`vs_core` 07:20, closure 747):

```
[RWC_CORE] RwEngineInit 0x004c32b0(0, 0, 0x200000) = 1; RwGlobals [0x007d3ff8] = 007D3EC8, engine status +0x124 = 1
[RWC_CORE] RenderwareAttachPlugins 0x00493640() = 1 on the RW core; [0x007dc57c] (PTank plugin offset, 0x00534ac4) = 0x80
[RWC_CORE] RwEngineOpen 0x004c30b0(hwnd=0269047C) = 1; IDirect3D9 [0x007d4108] = 0178FBC8
[RWC_CORE] RwTexDictionaryCreate 0x004c5890 call 1 on the RW core (RwCore_004c5890) = 0177E1BC; ... self-test PASS (0 of 11 checks failed, 0 failing calls so far)
[RWC_CORE] STOP: rva 0x004E7EF0 called, no body on the RW core path (no source, no hand body); exit 0x52570070
```

So 0x00493640 and its 14 + 3 attaches return 1 on the core. RwEngineStart then runs the module constructors the
attaches registered through 0x004c2d90, and the first of them, 0x004e7ef0 (module 0x501, registered by 0x004e5d30), had
no body: none of the 42 constructors / destructors registered by the attaches (`0x004e36a0` .. `0x00557e90`, 21
modules) has a re/match source.

## 2. The four generator gaps (RESULT 38.5), fixed in `gen_rwcore_tus.py`

| gap | fix |
|---|---|
| 1. `FUN_4e4e60` names (no leading zeros, `re/match/rwl02/004e5d30.c`) | name regex takes 6-8 hex digits, `rva_of()` zero-fills |
| 2. stops called through `__stdcall` declarations (`_004eede9@8`, `_004ecf29@12`, `_004ecf53@12`) | `SIG`: the convention and argument bytes a source declares or defines for a name; a stop is emitted with them |
| 3. named callees (`RpAtomicRegisterPlugin` in 0x0052e310 / 0x00534a80 / 0x0053d0b0 / 0x0053eaa0) | `NAMED`: `// MATCH: 0x<rva> <Name>` labels with no hex in the name, plus hooks.csv `Rw* / Rp* / Rt*` names (`RpWorldForAllWorldSectors` 0x004e5c70 in 0x00538bd0) |
| 4. fix-ups `RwCore_004ec839` / `_004ec915` / `_004ec9cd` / `_004ecaab` | these are function starts: `__stdcall` d3dx bodies (`re/match/s21/004ec839.c` etc., 100%). The seed declaration now carries the `SIG` convention. Not data inside functions |

Also: `s_Invalid_combination_type_0061811c` in `re/match/s19/004c51a0.c` as `MGLOBALA(char, ..., 0x1c, 0x0061811c)` (25
bytes + pad; score 14.7 unchanged).

Further generator fixes found on the way: a source's own definitions are bound even outside the RW bands (0x0052daf0
entered as a fix-up target); EXT RVAs are terminal (0x0045b350 `RwInitNullStub` had been compiled again from its
source, LNK2005); `defs_of` takes the MATCH label when the label is defined in the file (`re/match/s24/00550b00.c` has
the helper `GetFS` between its MATCH line and `FUN_00550b00`); `_strncpy` and `qsort` (0x005c24e0) and `__chkstk`
(0x004a3440) bound to CRT symbols.

## 3. Lifted bodies: `lift_rwcore.py`

`lift_rwcore.py` transcribes a closure RVA with no source verbatim from the anchored disassembly into a naked
`__asm` body (`RwCore/RwCore_Lifted_NN.cpp`), callees and data bound like the generated TUs. Under L-137 (Mariano
2026-11-12) a lifted body is scaffolding only: listed in `PROVISIONAL_RWC.tsv`, knob path only, never C4 and never
`port=body`, and a matched or hand-written source must replace it. No frida-replay promotion is filed for any lifted
body.

Closure at 2cca29c6c (RwEngineStart = 1, before the frame knob; final closure in 3.2): **1400 RVAs: 418 match100, 229 provisional sources, 620 lifted, 16 hand,
10 exe bodies, 107 logged stops** (`LIFT_REFUSED.tsv`: 81 labels inside a known function, 19 indirect jumps or calls
through tables, 7 unresolved targets). `PROVISIONAL_RWC.tsv` lists the 865 non-proven bodies (229 + 620 + 16).

### 3.1 Run 40: code pointers Ghidra never made functions

The first run-40 boot (pre-fix tree, `vs_core_run40_1.log`) passed RwEngineInit, the attach and RwEngineOpen, then in
RwEngineStart:

```
[RWC_CORE] AV at 00532770 (exe+0x522770) write 00532770; eax 00532770 ... esi 00623C30
```

`mapsym.py` on the stack: `RwCore_EngineStart` -> 0x004c2fb0 -> 0x004d8000 -> lifted 0x0052e3d0 -> 0x0052ffd0 ->
0x005327d0 -> 0x004d4380, which called 0x00532770 raw. 0x005327d0 passes `DAT_00623c30` (the RxNodeDefinition of
`nodeD3D9SkinAtomicAllInOne.csl`) to 0x004d4f90, and the dword at 0x00623c40 holds 0x00532770, the node body. None of
these callbacks is in `function_inventory.csv`, `match_ledger.tsv` or `re/match`, so the strict seed rule
(fix-up only to a known function start) left them raw.

Fix (`gen_rwcore_tus.py`): a strict-range dword also becomes a fix-up when its target is 16-byte aligned, the byte
before it is 0x90 / 0xcc / 0xc3, it lies inside no known function body, and its low 16 bits are not zero.
`CODEPTR_RULE.tsv` lists all 189 in-.text candidates: **27 accepted** (0x00623c34 .. 0x00624058: the node definitions
and the PVS / world-sector / patch / UV-anim callback tables), 162 refused. The low-half rule refuses 0x0061b8d0 =
0x004c0000, which sits in a table of 16-bit pairs (0x0070007f 0x00720071 ..). The new targets joined the closure
(1273 -> 1400 rows). Four raw g2s sources they pulled in do not compile as C++ under VS2022 and are lifted instead
(`LIFT_INSTEAD`: rwl03/00530650, rwl03/00530c00, rwl04/0054ba20, rwl04/0054bda0).

### 3.2 Run 40: the librw binding RVAs, and four lifter forms

For the frame knob every RW-band RVA a librw binding stands for (`LibRw/*.cpp` `MashedRw_<Name>_<rva>`) is a
closure root, plus 0x004b3eb0 (the TXD-from-memory loader DisplayInit calls; `PortTop_Port_004b3eb0` is a librw
path). Final closure (`RwCore_Bodies.tsv`): **1567 RVAs: 491 match100, 267 provisional sources, 680 lifted, 16 hand,
10 exe bodies, 103 logged stops**. `PROVISIONAL_RWC.tsv`: 963 rows (267 + 680 + 16). `LIFT_REFUSED.tsv`: 76 labels
inside a known function, 24 unresolved call targets, 3 other.

Generator and lifter changes this needed, each from a named stop or build error:

| change | evidence |
|---|---|
| extents end at the next known function start | match_ledger lists 0x004c2620 with 1425 bytes (function_inventory 1040, ending at the start 0x004c2a30), so the camera update callbacks 0x004c2bb0 / 0x004c2b80 (ret + nop before each, both open with `mov eax, [0x007d3ff8]`) were refused as labels |
| only the C++ branch of `#ifdef __cplusplus` counts as a definition | re/match/rwl01/004c1d30.c defines a C stand-in of 0x004c1a70 under `#else`; the copy symbol was unresolved at link |
| `_strstr` bound to `RwCoreCrt_strstr` | 0x005507b0 calls it by name (LNK2019) |
| switch tables lifted as compare chains | stop at 0x004df030 (`jmp dword ptr [edi*4 + 0x4df0fc]`) and 0x004ca160 (`xor edx, edx; mov dl, byte ptr [eax + 0x4cab20]`): the bound `cmp X, N` / `ja` and the optional byte index table are read from the anchored image, the jump becomes `cmp X, k` / `je L_<target>`. 14 of the 15 jump-table refusals cleared; 0x0053ed20 (`jmp dword ptr [eax*4 + 0x53f134]` at 0x0053edc3) keeps a shape the walk-back does not match and stays a stop |
| IAT calls through `RwCoreIat_<slot>` (`RwCore/RwCore_Iat.cpp`, 9 slots, each = this exe's import of the same name: GetClientRect, AdjustWindowRectEx, SetWindowLongA, SetWindowPos, RegOpenKeyA ..) | 0x004c9290 / 0x004ca160 / 0x004fbd78 called `[0x005cc21c]` / `[0x005cc00c]`; the OS is the declared platform boundary |
| tail jumps through a run-time pointer (`jmp dword ptr [reg + small disp]`) emitted as is | stop at 0x004e3850 (`jmp dword ptr [eax + 0x10]`, a module callback), same footing as `call [reg + disp]` |
| 12 raw g2s sources that fail VS2022 C++ lifted instead | `LIFT_INSTEAD_RUN40.txt` (build_vs12 .. build_vs25 logs) |
| 3 callees of re/match/rwl02/004e7420.c (0x005c4670 / 0x005c47b0 / 0x005c47e0, sources in s28) in XFOLLOW | UNBOUND non-RW in the generator |

`route_bindings.py` inserts one line per binding, `if (RwCore_FrameRoute()) return reinterpret_cast<ret (__cdecl*)(..)>
(&<core symbol>)(args);` tagged `[RWC-FRAME]` (idempotent). `ROUTED.tsv`: **71 routed** (47 match100, 16 provisional,
6 lifted, 2 logged stops), 17 not routed (10 the engine knob already routes, 7 game-side RVAs outside the closure:
0x0052cb00 / 0x0052d530 BMP read / write, 0x004b3f90, 0x0048fef0, 0x004b5300, 0x004b5190, 0x004b52f0).
RwRasterShowRaster 0x004c7730 goes through `RwCore_FrameShowRaster` (RwCore_Knob.cpp), which logs the frame, stops,
closes and terminates the engine on the core and ends the process with 0x5257F000 | (result != 0).

rva-lint: 180 known (D4 burn-down 5, ORIGTOP-ONLY 175). The growth added 21 ORIGTOP-ONLY pairs and one untagged
D4 entry, `DUP-IN-TARGET:004c0ad0` (RwCore_004c0ad0 plus four renamed copies `RwCoreCopy_<src>_004c0ad0` the matched
sources carry, distinct symbols, knob path only; five marked TUs is not of the ORIGTOP-ONLY class).

## 4. Builds

| build | result |
|---|---|
| default VS2022 (`build_vs29.log`, tree 6e4430812) | Build OK, rva-lint OK (180 known, 0 new) |
| object build `MASHED_DDKOBJ=1` (`build_ddk3.log`) | Build OK, link rc 0, `mashed_re_ddk.exe` |
| `ddkobj.py check` (`BYTECHECK.tsv`, 1319 rows) | **IDENTICAL 1090, DIFFERENT 215, not-in-map 14.** RwCore rows: match100 495 IDENTICAL (9 not-in-map), provisional 57 IDENTICAL / 208 DIFFERENT, exe 2 IDENTICAL. **No 100% source of the core is DIFFERENT.** Three 100% game sources are DIFFERENT, 0x00493900 (+0x13), 0x00493710 (+0x24) and 0x004929d0 (+0x3), at the same offsets in run 38 (e1205e9a2) and run 39 (dadd79960): not from this row. Against run 39's 787 rows: 0 changed. The run-40 midpoint file (fd0f8c33a, 1204 rows): 4 rows moved not-in-map -> result (0x004eb9e0, 0x004ebb00, 0x004ec130 IDENTICAL, 0x004cb330 DIFFERENT), 0 changed otherwise |
| ddkobj refusals | 20 units `fail:static-register-convention` / `fail:wrapper-redefines-globals` (game sources, e.g. re/match/s13/0047c270.c, s12/004769f0.c), the same mechanism as before this row; they keep their VS2022 bodies |

## 5. Knob boots

Run 40 `vs_core` of 2cca29c6c (`vs_core.log`; data seed 44 ranges, 190 code-pointer fix-ups, 0 faults):

```
[RWC_CORE] RwEngineInit 0x004c32b0(0, 0, 0x200000) = 1; RwGlobals [0x007d3ff8] = 007D3EC8, engine status +0x124 = 1
[RWC_CORE] RenderwareAttachPlugins 0x00493640() = 1 on the RW core; [0x007dc57c] (PTank plugin offset, 0x00534ac4) = 0x80
[RWC_CORE] RwEngineOpen 0x004c30b0(hwnd=001707F2) = 1; IDirect3D9 [0x007d4108] = 04691E00
[RWC-DEVICE] CreateDevice(adapter 0, type 1, flags 0x00000054) -> 0x00000000
[RWC_CORE] RwTexDictionaryCreate 0x004c5890 call 1 on the RW core (RwCore_004c5890) = 018C94AC; ... self-test PASS (0 of 11 checks failed, 0 failing calls so far)
[RWC_CORE] RwEngineStart 0x004c2fb0() = 1
[RWC_CORE] bring-up probe done: RwEngineStop 0x004c2f60() = 25560712; returning 0 from RwEngineStart ...
[RWC_CORE] RwEngineClose 0x004c3040() = 1
[RWC_CORE] RwEngineTerm 0x004c3270() = 1
```

The module registry that RwEngineStart walks holds 38 nodes (dumped by the VEH in the AV log of the first run-40 boot,
`vs_core_run40_1.log` lines 101-138: ids 0x40F, 0x401 .. 0x412 core, 0x501 .. 0x50F world, 0x102 .. 0x1B7 toolkits and
plugins, 0x4 last). In the passing boot RwEngineStart returns 1 with all of them registered, and RwEngineStop returns
0x01860688 (the RwGlobals block, nonzero) after the module destructors (0x004d8060). The knob ends the boot there by design
(the rest of the top-down path still calls librw bindings and the hand frame). The full engine lifecycle with every
plugin attached now runs on the RW core.

### 5.1 Final arms, tree 6e4430812 (`arms_run40_final.out`)

| arm | exit | result |
|---|---|---|
| vs_off | 0 after 600 frames | knob off, librw path |
| ddk_off | 0 after 600 frames | knob off; vs_off vs ddk_off **1134 / 1134 lines, 0 differ** (`logcmp.py`) |
| vs_core | 0xFFFFFFFF (by design) | Init / Attach / Open / Start = 1 on the core, probe, Stop, failure path, Close = 1, Term = 1 |
| ddk_core | 0xFFFFFFFF (by design) | same as vs_core on the object build |
| vs_frame | **0x5257F001** | Start = 1, 0x00493710 returns 1, DisplayInit on the core, Reset forced windowed 640x480, RWC-DEVICE selftest PASS, RwRasterShowRaster 0x004c7730 = raster, `presents 1`, Stop / Close / Term on the core |
| ddk_frame | **0x5257F001** | same as vs_frame on the object build |

The frame-knob log (`vs_frame_firstframe.log`) between Start and the Present:

```
[ORIG_TOP] 0x00493710 RW engine bring-up (matched, RW calls on librw) returned 1
[ORIG_TOP] 0x00428450 loading-disc draw (32, -32): missing body, not drawn
[ORIG_TOP] 0x00493700 called: missing body
[RWC-DEVICE] Reset: asked windowed 1 640x480 refresh 0 -> forced windowed 1 640x480 refresh 0
[ORIG_TOP] 0x004936f0 called: missing body
[RWC-DEVICE] selftest PASS (0 fail): backbuffer=640x480 windowed=1 getters=640x480 GetDirect3D=wrapper QI=wrapper client=640x480
[RWC_CORE] RwRasterShowRaster 0x004c7730(raster 08F290B8, dev 00000000, flags 0x1) = 08F290B8 on the RW core: first frame presented
[RWC_CORE] probe: ... presents 1
```

(The 0x00493710 log text still says "RW calls on librw": that string is fixed in PortTop_00493710.cpp. Under the frame
knob the calls go to the core.) 0x00493700 and 0x004936f0 are the callbacks 0x00493710 registers through 0x004cbc60 /
0x004cbc80. The librw shims only stored them. The core driver calls them around the device Reset, and the top-down
path has no body for either (logged no-ops).

Frame-knob iteration (`frame_iter.sh`, logs `vs_frame_<n>.log`): AV in librw `rw::TexDictionary::create` from
`PortTop_Port_004b3eb0` (fixed: 0x004b3eb0 lifted and routed), then stops 0x004df030, 0x004c9290, 0x004e3850 and
0x004ca160 (each fixed by the lifter row above), then the first Present.

## 6. Self-test

**PASS.** `RwCoreBind_TexDictionaryCreate_004c5890` self-test "0 of 11 checks failed, 0 failing calls so far" in
vs_core, ddk_core, vs_frame and ddk_frame. RWC-DEVICE self-test "PASS (0 fail)" in vs_frame and ddk_frame (backbuffer
640x480, windowed, getters 640x480, GetDirect3D and QueryInterface return the wrapper, client 640x480). The knob-OFF
arms do not run either self-test.

## 7. Next blocker and follow-up rows

1. **Im2D on the first frame (the rest of this row's step 3).** 0x00428450 (loading-disc draw, source
   `re/match/s06/00428450.c`) has no body on the top-down path. Under `MASHED_RWC_FRAME` it can join the closure
   like 0x004b3eb0 and draw through the core's Im2D (RwGlobals device block +0x30 / +0x34). Then capture the
   original's first-frame Im2D call for the same disc (`re/frida/menu_draw_burst.py` shape) and compare vertex for
   vertex. Proposed row: `RWC-FIRST-FRAME-IM2D`.
2. **The matched frame render 0x00492e90 on the core.** Its 26 callees run the title layer, menus and HUD through
   hand stand-ins (`PortTop_StandIn_00492e90` is the hand RenderFrame). It needs the T1 stand-in rows first. Moving
   the frame-knob endpoint from the first Present to frame N is the measure.
3. **0x00493700 / 0x004936f0**, the callbacks the core driver calls around the device Reset, have no exe body.
4. **17 bindings not routed**: 7 game-side RVAs outside the closure (BMP read / write 0x0052cb00 / 0x0052d530,
   0x004b3f90, 0x0048fef0, 0x004b5300, 0x004b5190, 0x004b52f0) and 2 routed to logged stops (0x00513f90,
   0x00514330). Each is a stop the frame knob names when it is reached.
5. **Lifted bodies (L-137)**: 680 transcriptions are scaffolding. The 103 remaining stops (`LIFT_REFUSED.tsv`) are
   mostly labels inside known functions (76) and unresolved call targets (24).
6. `match_ledger.tsv` `bytes_n` overstates 0x004c2620's extent (1425 against 1040): a ledger note for whoever
   reruns the match on that source.

## 8. Process notes

- Merge-train PRIORITY-HOLDs ran almost continuously from 07:32 to 11:03 (four trains). Builds waited for a gap each
  time. One `build.bat` was started at 07:39 while a hold was shown (the status and the build ran in the same command);
  it stopped at the rva-lint step after a few seconds, before any compile. Every later build waited for the hold to clear.
- Raw data `D:/MashedWT/RWC-PLUGIN-ATTACH_data` (sha256 in `RAW_SHA256_RWC-PLUGIN-ATTACH.txt`).

## Handoff (run 40 end, 2026-11-12)

Run 39's handoff (reboot safe point at 5e542495c) is closed: every step it listed ran in run 40.

State: branch `orch/RWC-PLUGIN-ATTACH`, worktree `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\RWC-PLUGIN-ATTACH`,
private root `D:/MashedWT/RWC-PLUGIN-ATTACH_root`, raw data `D:/MashedWT/RWC-PLUGIN-ATTACH_data` (sha256 of the logs
cited here: `RAW_SHA256_RWC-PLUGIN-ATTACH.txt`). The branch is mergeable as it stands: every change is on the knob
path (`MASHED_ORIG_TOP=1` plus `MASHED_RWC_CORE=1`, and `MASHED_RWC_FRAME=1` for the routing), both builds pass, and the
knob-OFF arms are unchanged.

To continue (section 7, item 1):
1. Add 0x00428450 to `ROOTS` / `XFOLLOW` in `gen_rwcore_tus.py` and route its top-down caller to the core body under
   `RwCore_FrameRoute()` (as 0x004b3eb0: `route_bindings.py` matches `PortTop_Port_<rva>`; the missing-body stand-in in
   the PortTop TU needs the same one-line route).
2. `bash verify/rwccorerun_20261111/frame_iter.sh <next build number>` (regenerates, routes, builds until it passes
   with lift-instead / ORIGTOP-ONLY handling, waits out merge-train holds, boots vs_frame and prints the end). Fix the
   stop it names, repeat.
3. When the disc draws: log the Im2D calls on the core (the device block +0x30 / +0x34 entries) and capture the
   original's first-frame disc draw for a vertex-for-vertex comparison.

## 9. Run 41 restack: PROVISIONAL.tsv (run-40 L0 L-137 check)

`re/match/PROVISIONAL.tsv` did not exist on this branch (it was cut before PROV-DIVERGED-18 landed it on orch/integration).
The file is now orch/integration's version (29 rows) plus the rows of `PROVISIONAL_RWC.tsv`, in its 12-column format
(`status provisional`, `flags` `-`, replay columns `not run`, evidence = this RESULT). Text only: no build, no gate, no game run,
`hooks.csv` untouched, no promotions TSV.

| Added | Count | Marking |
|---|---|---|
| lifted (`RwCore/RwCore_Lifted_NN.cpp`) | 680 | `SCAFFOLDING (L-137)` in `diagnosis`, equiv `n/a (lifted)` |
| generated provisional sources | 266 | equiv `NOT-PROVEN` |
| hand (`RwCore/RwCore_Hand.cpp`) | 16 | equiv `n/a (hand)` |
| total | 962 | file is 993 lines (comment, header, 29 + 962 rows) |

Flags for reviewers:
- 0x004c51a0 is in both lists. Kept integration's `refused` row (DIVERGED@1, 14.7%) and did not add the RWC `provisional` row
  (963 - 1 = 962 added). The RWC core knob still routes `RwCore_004c51a0` to that source: a refused body may not run, so it needs a source
  or a hand body (follow-up row).
- Executed lifted members (L-141): the run keeps no per-function execution log. The AV stack in section 3.2 shows lifted 0x0052e3d0
  executing on the EngineStart path, so it needs a matched or hand source. Which other lifted members executed is unmeasured.
- Merge: `re/match/PROVISIONAL.tsv` is added on this branch and exists on integration, so the train sees add/add on that file. Integration's
  29 rows are the common prefix; resolve by taking integration's file and appending the 962 added rows.
