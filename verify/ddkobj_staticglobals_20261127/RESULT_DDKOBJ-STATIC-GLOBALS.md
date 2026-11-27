not met, stop moved from 0x0047ab30 (AV exe+0x61741, write [0+0x1EC]) to the race-load stop 0x004E99B0 (exit, caller 0x004b3c60), after COURSE.LUA ran to its end and textures.txd + graph.bsp loaded, on the object build (measured on a87ce200f and on the RACE-PIZ-DEVICE prep base; the 7aae3a0f5 head re-run is the open step, section 0)

# RESULT DDKOBJ-STATIC-GLOBALS (run 56, tool row, top-down kind) — IN PROGRESS, HANDOFF FOR THE LIVING-PC REBOOT

## 0. Handoff (reboot coordinator 14:10, Windows token leak)

State at the stop:
- Branch `orch/DDKOBJ-STATIC-GLOBALS` @ f6441c8fb (tool commit, rebased onto orch/integration **7aae3a0f5**, the head L0 named).
  Worktree `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\DDKOBJ-STATIC-GLOBALS`. Ledger row: working.
- The worktree's `mashedmod/build` is still the a87ce200f build (not yet rebuilt on 7aae3a0f5).
- Untracked helper `re/tools/ddkobj/_headctrl_ddkobj.py` = `git show HEAD~1:re/tools/ddkobj/ddkobj.py` (the head's tool, for the
  control build). Delete it before committing; never commit it.
- Raw data: `D:/MashedWT/DDKOBJ-STATIC-GLOBALS_data` (`r1/` = RACE-PIZ-DEVICE prep base 461a34f27, `r2/` = a87ce200f).
  Root `D:/MashedWT/DDKOBJ-STATIC-GLOBALS_root` (original/ + staged image 81CC7775).

Next, on 7aae3a0f5 (only these checks count), in order, each after `mashed_lock.py status` and `test-slot --status` (C: > 25 GB):
1. `cmd /c "set MASHED_NO_DEPLOY=1&& set MASHED_DDKOBJ=1&& mashedmod\build.bat"` > `$D/build.log`.
2. Head-tool control: `py -3.12 re/tools/ddkobj/_headctrl_ddkobj.py build --units all --link --out mashedmod/build/ddkobj_headctrl`
   then `... check --out mashedmod/build/ddkobj_headctrl` > `$D/headctrl_build.log`.
3. `py -3.12 verify/ddkobj_staticglobals_20261127/compare_base.py mashedmod/build/ddkobj mashedmod/build/ddkobj_headctrl D:/MashedWT/DDKOBJ-STATIC-GLOBALS_data/piz_base > verify/ddkobj_staticglobals_20261127/compare_base.txt`; also compare `different_rvas` with L0's head list (IDENTICAL 2455/2562, DIFFERENT 79).
4. `py -3.12 re/tools/ddkobj/ddkobj.py selftest`, `py -3.12 re/tools/bind/porttop_bind.py selftest` (into this folder).
5. Knob boot kb_vs3 and ONE arm RO_ROLOADRWC-sg3 (commands in section 5), copy BYTECHECK.tsv / ddkobj_summary.json here,
   frida list after, fill sections 3-6 with the 7aae3a0f5 numbers, delete `_headctrl_ddkobj.py`, commit, `roadmap.ps1 set
   DDKOBJ-STATIC-GLOBALS review -Evidence verify/ddkobj_staticglobals_20261127/RESULT_DDKOBJ-STATIC-GLOBALS.md`.

## 1. Cause and change (re/tools/ddkobj/ddkobj.py)

35 re/match sources declare an original global `DAT_<addr>` as a C file-static (`static T DAT_x;` under `#ifndef __cplusplus`,
pilot finding 2: the static is what reproduces the original's register use) and map it with `MGLOBAL(T, DAT_x, 0x<addr>)` in the
C++ view. In the DDK object the static is a STATIC-class symbol in `.bss` (`_DAT_006bf1cc`, sec 4, for 0x0047ab30), and every
reference is a DIR32 relocation to it, so the linked body read a private zero copy. All 35 are zero-initialised
(`inventory.py`).

Change (process_unit, step 1, definitions): a STATIC, non-function symbol whose plain name is `DAT_<8 hex>`:
- source MGLOBAL address == the name's address, in 0x00400000..0x00a00000: a new undefined external `__ddkabs_<addr>` (the same
  absolute symbol MGLOBAL externs already use) takes every relocation outside `.debug*` (`coffedit.repoint`), the private
  definition is renamed `<name>__ddkstatic<rva>` (dead). Recorded per unit as `static_globals` (name, addr, refs).
  `check` then compares those dwords exactly (not masked, `abs_data_refs_exact`), so an IDENTICAL row proves each one
  equals the original's address.
- no MGLOBAL for the name (the port view keeps a private copy too): `fail:static-global-unmapped`, VS2022 body, not fatal.
- MGLOBAL at another address: `fail:static-global-address-mismatch`, VS2022 body, not fatal.

Selftest fixtures (`_ST_SRC`): `stat` (a scalar and an array static, MGLOBAL / MGLOBALA at their own address), `statnomg`,
`statbad`; 6 checks (bound names and reference counts, no relocation left on the private copies, array addends 8 / 12 kept,
the two refusals not fatal). `_ST_MGPRE` gained the MGLOBALA forms.

## 2. The 35 sources and their verdict (a87ce200f build; same on 461a34f27)

| source | static name(s) | unit (wrapper) | verdict | refs bound | BYTECHECK (abs exact now / before) |
|---|---|---|---|---|---|
| s04/0041c380.c | DAT_0063cde4 | PortTop_0041cb80 | bound | 3 | 0x0041c380 IDENTICAL 4/2; 0x0041cb80 IDENTICAL 4/3 |
| s06/00429290.c | DAT_0067d960 | MatchS06_00429290 and PortTop_00429290 | bound | 1 each | 0x00429240 IDENTICAL 1/0; 0x00429290 IDENTICAL 0/0 |
| s10/00461650.c | DAT_0068f4c4 | PortTop_00461650 | bound | 5 | IDENTICAL 88/83 |
| s10/00463590.c | DAT_006904e0 | PortTop_00463590 | bound | 3 | IDENTICAL 8/5 |
| s10/004661f0.c | DAT_0069058c | PortTop_004661f0 | bound | 2 | IDENTICAL 62/60 |
| s12/00475a60.c | DAT_0069160c | PortTop_00475a60 | bound | 2 | IDENTICAL 5/3 |
| s12/00475c10.c | DAT_00691614 | PortTop_00475c10 | bound | 2 | IDENTICAL 8/6 |
| s12/004769a0.c | DAT_00692528, DAT_0069252c, DAT_00692530 | MatchS12_004769a0 | bound | 1+1+1 | IDENTICAL 4/1 |
| s12/004769d0.c | DAT_006924dc, DAT_006924e0 | MatchS12_004769d0 | bound | 1+1 | IDENTICAL 3/1 |
| s12/00476d00.c | DAT_006924dc | MatchS12_00476d00 | bound | 2 | IDENTICAL 13/11 |
| s13/0047ab30.c | DAT_006bf1cc | PortTop_0047ab30 | bound | 5 | IDENTICAL 5/0 |
| s13/0047abd0.c | DAT_006bf1cc | PortTop_0047abd0 | bound | 6 | IDENTICAL 6/0 |
| s13/0047ac80.c | DAT_006bf1cc | PortTop_0047ac80 | bound | 1 | IDENTICAL 1/0 |
| s13/0047ad40.c | DAT_006bf1cc | PortTop_0047ad40 | bound | 4 | IDENTICAL 4/0 |
| s13/0047c0f0.c | DAT_006c2fec | PortTop_0047c0f0 | bound | 3 | IDENTICAL 7/4 |
| s14/00489890.c | DAT_00703110 | PortTop_00489890 | bound | 1 | IDENTICAL 1/0 |
| s15/0048e5f0.c | DAT_007668f4 | PortTop_0048e5f0 | bound | 2 | IDENTICAL 13/11 |
| s01/004036a0.c | DAT_00636b74 | PortTop_00403910 | refused `fail:static-global-unmapped` (no MGLOBAL: the port keeps a private copy too). Before: refused `fail:externalized-static-register-convention`, so not linked either way | - | - |
| s10/00462950.c | DAT_0068fc80, DAT_0068fc84 | PortTop_00462950 | `skip:provisional` (unchanged) | - | - |
| pilot/a7_0041c380.c, s01/004034a0.c, s09/004527f0.c, s09/00459400.c, s15/00491c00.c, s15/00491f00.c, s16/00498a00.c, 00498c00.c, 00498d20.c, 00498d60.c, 00498e40.c, 00498ea0.c, 00498f60.c, 00499170.c, 004991f0.c, 00499400.c (16) | see inventory.py | none | not a unit (no exe wrapper includes it) | - | - |

17 linked sources (18 units, s06 has two wrappers): 17 bound, all IDENTICAL. 1 unit refused (was refused before). 1 provisional.
16 not included by the exe.

match.py --bytes + relocheck.py on all 35 (`scope_match.tsv`; no source edited by this row): 27 BYTE-IDENTICAL + relocheck
OK, 7 partial sources with relocheck OK, 1 (s10/00462950.c, provisional) not byte-identical and relocheck BAD, all unchanged
from the head. Every one of the 17 bound sources is BYTE-IDENTICAL with relocheck OK.

## 3. ddkobj check (a87ce200f; to be repeated on 7aae3a0f5)

Against the head-tool control (HEAD~1 ddkobj.py on the same default objects) and against RACE-PIZ-DEVICE's object build:
IDENTICAL 2455/2562, DIFFERENT 79, not-in-map 28, units linked 2437/2713 on both sides; per-function result changes 0, newly
DIFFERENT none, new not-in-map none; dual body 33 and the 7 provisional functions unchanged. One unit status change:
s01/004036a0.c (refusal reason, see section 2). The DIFFERENT list equals the train's head list (drain 2, 79, 0 new, 0 gone).
Against L0's older head check `log\orch\run56_headcheck\` (1379467a6): the only newly DIFFERENT is 0x0047a320, replay-C4,
classified by DDKOBJ-ADDR-REGISTERED. Listed unit -> 100% neighbour pairs that fell back to VS2022: none new.

## 4. Other checks (a87ce200f unless said)

| check | result |
|---|---|
| build both (`MASHED_NO_DEPLOY=1 MASHED_DDKOBJ=1`) | Build OK, link rc 0 (r1 461a34f27, r2 a87ce200f) |
| ddkobj selftest | OK, 0 failures, 50 PASS (6 new) |
| binder selftest | PASS, 0 FAIL |
| knob boot kb_vs1 / kb_vs2 (VS2022 exe, MASHED_ORIG_TOP=1 MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=600) | rc 0, `WinMain 0x00492370 returned 0 after 600 frames` |
| arm RO_ROLOADRWC-sg1 (461a34f27, exe dd64ccfa) / sg2 (a87ce200f, exe 5a50e5fa) | exit 5151152 = 0x004E99B0 after 7 s / 6 s: COURSE.LUA runs with no `Unable to find` and no AV, `[RW_FS] 0x004cc230(2, 1, "textures.txd"): 1577932 bytes`, `0x004cc230(2, 1, "graph.bsp"): 594516 bytes`, then `rva 0x004E99B0: race-load stop (PORT-OT-RACE), call 1, callers 0x004b3c60, return 0005A1ED, exits`. meta: warp n 2402 tick 300, race.n 0 / reached false, selftest 22196 checks 0 fails; OT_TIMER self-test 2990 checks 0 failed |
| race frame body on a knob-on arm | no |
| hash moved | none run (row kind: no G-CTRL / B / 12x4) |

0x004E99B0 is one of the all-knobs gate rows held by decision C (not claimed here).

## 5. Commands

`D=D:/MashedWT/DDKOBJ-STATIC-GLOBALS_data`, `O=<orch-wt>`:
`PORTTOP_OUT=$D test-slot bash -c "py -3.12 $O/re/orchestrator/mashed_lock.py run --label DDKOBJ-STATIC-GLOBALS -- py -3.12 verify/porttop_20261108/knob_boot.py kb_vsN D:/MashedWT/DDKOBJ-STATIC-GLOBALS_root 300 MASHED_ORIG_TOP=1 MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=600"`;
`A4_OUT=$D A4_ROOT=D:/MashedWT/DDKOBJ-STATIC-GLOBALS_root A4_EXE=<wt>/mashedmod/build/ddkobj/mashed_re_ddk.exe A4_RO_CKPT=0,1,17,18,299,301,600 test-slot bash -c "py -3.12 $O/re/orchestrator/mashed_lock.py run --label DDKOBJ-STATIC-GLOBALS -- py -3.12 verify/savelink_20261023/run_arms.py RO_ROLOADRWC-sgN"`.

## 6. Frida, raw files

frida-<hash> folders in %LOCALAPPDATA%\Temp: 5 before (`frida_before.txt`), 5 after sg1 (`frida_after.txt`, same names). This row
uses no Frida, made none, deleted none.

Raw (`D:/MashedWT/DDKOBJ-STATIC-GLOBALS_data/r1`, 461a34f27): build.log 4455becd..., kb_vs1.log 66fa9fef...,
RO_ROLOADRWC-sg1.log 13709983..., sg1/meta.json 11c28436..., sg1/ticks.bin 877fc536..., mashed_re_ddk.exe dd64ccfa...,
mashed_re.exe 77c68084.... `r2` (a87ce200f): sg2 exe 5a50e5fa, meta.json 9b660d1a, ticks.bin c013667d (full sha256 to be
listed with the 7aae3a0f5 run).
