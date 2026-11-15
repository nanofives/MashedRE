# RESULT DDKOBJ-RETARGET-493710 and DDKOBJ-RETARGET-493900 (2026-11-15)

**Verdict: GREEN for both rows.** In the object build, 0x00493710 and 0x00493900 now link the DDK compile of their
100% sources, and `ddkobj.py check` reports both **IDENTICAL** (0 differing bytes). Both builds compile. Knob boots pass
on both builds. **Self-test result: `ddkobj.py selftest` = OK, 0 failures, 31 PASS** (`retarget/selftest.log`).

Row kind: top-down (PortTop wrappers, one `re/match` source, a BINDINGS note). Default gates (G-CTRL, B, 12 captures
x 4) not run, per WORKER_RULES "Row kinds": the knob is off on those hashes. No menu code changed, so no menu walk.
Branch `orch/DDKOBJ-RETARGET-493710` from `orch/integration` @ 198109d50. Both rows share this branch and RESULT.

## 1. Changes

| file | change |
|---|---|
| `mashedmod/src/mashed_re/Match/PortTop_00493900.cpp` | includes `re/match/topm01/00493900.c` instead of `re/match/s15/00493900.c`. Same 4 bindings (the topm01 source has the same MATCH-S15 preamble and callee names). |
| `mashedmod/src/mashed_re/Match/PortTop_00493710.cpp` | includes `re/match/pilot/a2_00493710.c` instead of `re/match/s15/00493710.c`. New bindings for the names the pilot source uses and the s15 draft did not: `StubA`/`StubB` -> `GetRaceEndTrigger` (rva 0x005c9d00), `StubC` -> forwarder `PortTop93_StubC_005c9d00` (calls `GetRaceEndTrigger(0)`: in C++ one C-linkage name cannot carry the source's two prototypes `int(void*)` and `int(void)`), `FUN_004951e0` -> `PortTop_Thunk_004951e0` (forwards to the existing missing stub `PortTop_Missing_00495780`, returns 0, the source discards the value), `printf` -> `PortTop_Crt_004a2cbd` (was `FUN_004a2cbd`; parameter now `const char*` to agree with the source's prototype), `DAT_007719d8`/`DAT_007719dc` -> `(*reinterpret_cast<int*>(k_...))` with `static const std::uintptr_t k_007719d8 = 0x007719d8u` (and `..dc`), the form `ddkobj.py` classifies `@samedata` (its selftest fixture "same"). Removed: the `FUN_005c9d00`, `thunk_FUN_00495780` and `FUN_004a2cbd` defines (names only the s15 draft used). |
| `re/match/pilot/a2_00493710.c` | its two `extern int DAT_007719d8; extern int DAT_007719dc;` lines are wrapped in `#ifndef MATCH_PORT` (the form of ddkobj's selftest fixtures), so the port's original-address defines do not have to form a declarator. The DDK compile (C, `MATCH_PORT` undefined) sees the same text: match.py re-run after the edit gives the same 100% / BYTE-IDENTICAL / relocheck OK (section 2). |
| `verify/porttop_20261108/BINDINGS.tsv` | line 5 (0x00493900) and line 53 (0x00493710): "the only source" replaced by the source now compiled, its score, and the row id. (The task named them the other way round. In the file, line 5 is 0x00493900 and line 53 is 0x00493710.) |

Not touched: hand-path code, RW bindings (`LibRw/RwApiBind.cpp`; no new librw binding, L-124), `hooks.csv`,
DDKOBJ-GFFU-DUP-4929D0, DDKOBJ-PROV-MIXED-UNIT, L-140.

Same arguments as before at the RW binding: `MashedRw_EngineInit_004c32b0(int memFuncs, int arenaSize)` (2
parameters, cdecl) now receives the pilot source's `(StubB(&buf), StubC(), 0x200000)`. Its 2 parameters read
`StubB`'s and `StubC`'s results, both 0 (`GetRaceEndTrigger` is `xor eax,eax; ret`). The s15 draft passed 2 arguments,
both 0 as well (RwApiBind.cpp:122-124 comment). The third argument is pushed and caller-popped. The RwApiBind comment
("passes two arguments") now describes the s15 draft. It was left as is, since that file is out of scope.

## 2. Source checks (`retarget/match_relocheck.txt`)

| source | match.py --bytes | relocheck.py |
|---|---|---|
| `re/match/pilot/a2_00493710.c` (before and after the guard edit) | 100.0% (regs 100, ops 100), 120/120 ins, 430 B, 0 differing unmasked bytes, BYTE-IDENTICAL | OK, checked=47, unchecked=5 (`_StubA,_StubB,_StubC,_printf`) |
| `re/match/topm01/00493900.c` | 100.0% (regs 100, ops 100), 122/122 ins, 448 B, BYTE-IDENTICAL | OK, checked=36, unchecked=2 (`@__security_check_cookie@4,___security_cookie`) |

## 3. Builds

- VS2022: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` -> `=== Build OK ===`, no warning or error naming
  either wrapper (`build_vs.log`). `mashed_re_dev.asi` not deployed.
- Object build: `cmd /c "set MASHED_DDKOBJ=1&& set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` -> link rc=0,
  `mashed_re_ddk.exe`, `=== Build OK ===` (`build_ddk.log`).
- `py.exe -3.12 re/tools/ddkobj/ddkobj.py check` (`retarget/ddkobj_check.txt`): IDENTICAL 1243/1275, DIFFERENT 17,
  not-in-map 15, units linked 1222/1501, refusals 9 (externalized-static-register-convention=5,
  wrapper-redefines-globals=4). **0x00493710 and 0x00493900 are no longer in the DIFFERENT list.**

`retarget/BYTECHECK_493710_493900.tsv`:

| rva | exe symbol | size | relocs masked | abs data refs exact | diff bytes | result | src |
|---|---|---|---|---|---|---|---|
| 0x00493900 | `_Orig_00493900` | 448 | 14 | 24 | 0 | IDENTICAL | re/match/topm01/00493900.c |
| 0x00493710 | `_Orig_00493710` | 432 | 50 | 2 | 0 | IDENTICAL | re/match/pilot/a2_00493710.c |

Unit bindings (`retarget/units_493710_493900.txt`, from `ddkobj_build.json`): both units `ok`, no dropped refs. In
a2_00493710: `DAT_007719d8` / `DAT_007719dc` -> `__ddkabs_007719d8` / `__ddkabs_007719dc` (data-abs), `StubA` / `StubB`
-> `_GetRaceEndTrigger` (in-set), `StubC` -> `_PortTop93_StubC_005c9d00`, `printf` -> `_PortTop_Crt_004a2cbd`,
`FUN_004951e0` -> `_PortTop_Thunk_004951e0` (wrapper-bound), every RW callee -> its `MashedRw_*` shim, as the
VS2022 view binds them. In topm01/00493900: 15 globals data-abs, `strtok` / `_stricmp` wrapper-bound,
`BootPtr773818Get` in-set, /GS cookie pair to the CRT.

The other 15 DIFFERENT rows are outside these two rows. They include the units RESULT_DDKOBJ-DIFFERENT-REASONS
classifies, plus 0x005538a0, 0x00554200, 0x00554940 and 0x005572c0 as they stand on orch/integration @ 198109d50.
[UNCERTAIN] This row did not classify those four. Its change does not touch their units.

## 4. Knob boots

Private root `D:/MashedWT/DDKOBJ-RETARGET-493710_root` (robocopy of `original\`), `verify/porttop_20261108/knob_boot.py`
with `KNOB_EXE` per arm, each run through `test-slot` (outside) and `mashed_lock.py run --label DDKOBJ-RETARGET-493710`
(inside), 3000 frames (`MASHED_DET_FRAMES=3000`). All ran locally (no `[test-hub]` line).

| arm | exe | env | result |
|---|---|---|---|
| vs_off | VS2022 `mashed_re.exe` | knob OFF | rc 0 after 37.4 s, `B17-SUMMARY reached-main-loop chrome=YES thunks=6/6` |
| vs_on | VS2022 | `MASHED_ORIG_TOP=1` | rc 0 after 32.9 s, `0x00493710 RW engine bring-up (matched, RW calls on librw) returned 1`, `WinMain 0x00492370 returned 0 after 3000 frames` |
| ddk_off | object build `mashed_re_ddk.exe` | knob OFF | rc 0 after 31.7 s, `B17-SUMMARY reached-main-loop chrome=YES thunks=6/6` |
| ddk_on | object build | `MASHED_ORIG_TOP=1` | rc 0 after 48.2 s, `0x00493710 ... returned 1`, `WinMain 0x00492370 returned 0 after 3000 frames` |

On the knob path, the 0x00493710 call sequence in the log (cfglog lines `Calling RwEngineInit` .. `Calling
RwEngineStart`, two `printf:` lines, then `0x00493600`, `0x005584c0`) is the pilot source's order. Diffed with 8-hex
tokens masked, the vs_on and ddk_on logs differ in 3 lines only: the `[portstate-seed] mem=` hash, the cfglog
timestamp, and one `[x87] PC_24 after device` line that vs_on prints and ddk_on does not. [UNCERTAIN] Why that x87
line differs between the two builds was not looked into. Both arms run their 3000 frames. 0x00493900 logs nothing of
its own, so these logs show no line for it. WinMain 0x00492370 (BINDINGS.tsv line 5, level 1 under 0x00492370)
calls it on the knob path, and both knob-ON arms return 0 after 3000 frames.

The `printf` line reads `Using video mode 0 : 3840x2160`. That is the `MashedRw_EngineGetModeInfo_004c2ed0` shim's
value on this machine. It is what the s15 draft printed through the same shim too: same callee and arguments, so
this row does not change it.

## 5. Process notes

- The first VS2022 build was cut short by a `| head` on the tee'd output (SIGPIPE ended the build). It was re-run with
  the whole output going to the log. That second run is the one reported.
- The four-arm boot loop waited about 11 min on test-slot's memory gate (selftest log 23:39, vs_off log 23:52 after a 37 s run) before it launched (the machine was over the
  24 GB budget). It started once memory freed up.

## Files

Committed (`verify/ddkobj_20261114/retarget/`): `match_relocheck.txt`, `ddkobj_check.txt`,
`BYTECHECK_493710_493900.tsv`, `units_493710_493900.txt`, `selftest.log`, `RAW_SHA256.txt`.

Raw (not committed), `D:/MashedWT/DDKOBJ-RETARGET-493710_data/`, SHA-256 in `retarget/RAW_SHA256.txt`:
`build_vs.log`, `build_ddk.log`, `vs_off.log`, `vs_on.log`, `ddk_off.log`, `ddk_on.log`, `knob_boots.txt`,
`mashed_re_vs.exe`, plus `mashedmod/build/ddkobj/mashed_re_ddk.exe` and `BYTECHECK.tsv` in the worktree.
