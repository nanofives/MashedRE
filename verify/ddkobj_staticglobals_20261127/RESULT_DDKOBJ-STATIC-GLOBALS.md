not met, stop moved from 0x0047ab30 (AV exe+0x61741, write [0+0x1EC]) to the race-load stop 0x004E99B0 (exit 5151152, caller 0x004b3c60), after COURSE.LUA ran to its end and textures.txd (1,577,932 B) and graph.bsp (594,516 B) loaded, on the object build at orch/integration 7aae3a0f5

# RESULT DDKOBJ-STATIC-GLOBALS (run 56, tool row, top-down kind)

Branch `orch/DDKOBJ-STATIC-GLOBALS`, worktree `.worktrees\DDKOBJ-STATIC-GLOBALS`. Prep on RACE-PIZ-DEVICE 461a34f27 (run r1), rebased
onto a87ce200f (run r2), then onto **7aae3a0f5** (the head L0 named, RACE-PIZ-DEVICE and BIND-LIB-VOIDPROTO merged). Every
number below is from the 7aae3a0f5 run (r3) unless a row says otherwise; r1 and r2 gave the same results. Tool commit f6441c8fb
(ddkobj.py only). No RVA bound, no re/match source edited, no knob, no default change. G-CTRL / B / 12x4 not run (row kind).
The LIVING-PC reboot at 14:49 (token leak) fell between r2 and r3; nothing of this row was running then.

## 0. Verdict

| item | result |
|---|---|
| ddkobj fix | 17 linked sources (18 units) bind their file-static `DAT_<addr>` to the original address; all IDENTICAL with those dwords compared exactly. 1 unit refused with a reason, 1 provisional, 16 sources not included by the exe (section 2) |
| ddkobj selftest | OK, 0 failures, 50 PASS (6 new) |
| object-build check vs the head | IDENTICAL 2455/2562, DIFFERENT 79 (= L0's head numbers and the train's drain-2 DIFFERENT list, 0 new, 0 gone); 0 newly DIFFERENT, nothing to classify |
| edited re/match sources | none edited; the 17 bound sources are BYTE-IDENTICAL with relocheck OK |
| ROLOADRWC arm (object build, staged image) | stop moved (line 1); race.n 0, reached false |
| race frame body on a knob-on arm | no |
| hash moved | none run (row kind) |

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

## 2. The 35 sources and their verdict (7aae3a0f5; identical on a87ce200f and 461a34f27)

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

## 3. ddkobj check (7aae3a0f5)

Against the head-tool control (HEAD~1 ddkobj.py on the same default objects) and against RACE-PIZ-DEVICE's object build:
IDENTICAL 2455/2562, DIFFERENT 79, not-in-map 28, units linked 2437/2713 on both sides; per-function result changes 0, newly
DIFFERENT none, new not-in-map none; dual body 33 and the 7 provisional functions unchanged. One unit status change:
s01/004036a0.c (refusal reason, see section 2). The DIFFERENT list equals the train's head list (drain 2, 79, 0 new, 0 gone).
Against L0's older head check `log\orch\run56_headcheck\` (1379467a6): the only newly DIFFERENT is 0x0047a320, replay-C4,
classified by DDKOBJ-ADDR-REGISTERED. Listed unit -> 100% neighbour pairs that fell back to VS2022: none new.

## 4. Other checks

| check | result |
|---|---|
| build both (`MASHED_NO_DEPLOY=1 MASHED_DDKOBJ=1`), 7aae3a0f5 | Build OK, link rc 0 (also r1 461a34f27, r2 a87ce200f) |
| head-tool control build + check (HEAD~1 ddkobj.py, `mashedmod/build/ddkobj_headctrl`) | IDENTICAL 2455/2562, DIFFERENT 79 |
| ddkobj selftest | OK, 0 failures, 50 PASS (6 new) (`ddkobj_selftest.txt`) |
| binder selftest | PASS, 0 FAIL (`binder_selftest.txt`) |
| knob boot kb_vs3 (VS2022 exe, MASHED_ORIG_TOP=1 MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=600), first boot after the reboot | rc 0, `WinMain 0x00492370 returned 0 after 96 frames`. Same shutdown sequence as kb_vs2; its intro movies completed in 7/7/4/4 frames against 25/26/14/14. the cause of the 96 is not measured; the re-run kb_vs4 gave 600 |
| knob boot kb_vs4 (same, re-run) | rc 0, `WinMain 0x00492370 returned 0 after 600 frames` (kb_vs1, kb_vs2 on r1 / r2: 600 too). This row does not change the VS2022 exe |
| arm RO_ROLOADRWC-sg3 (7aae3a0f5, exe 56271079) | exit 5151152 = 0x004E99B0 after 7 s: COURSE.LUA searched and run with no `Unable to find` and no AV, `[RW_FS] 0x004cc230(2, 1, "textures.txd"): 1577932 bytes from piz`, `0x004cc230(2, 1, "graph.bsp"): 594516 bytes from piz`, then `rva 0x004E99B0: race-load stop (PORT-OT-RACE), call 1, callers 0x004b3c60, return 0005A1ED, exits`. meta: warp n 2402 tick 300, race.n 0 / reached false, selftest 22196 checks 0 fails; OT_TIMER self-test 2990 checks 0 failed. sg1 (r1, exe dd64ccfa) and sg2 (r2, exe 5a50e5fa) gave the same stop |
| before (RACE-PIZ-DEVICE arms piz1, same staged image 81CC7775) | AV exe+0x61741 = PortTop_Bound_0047ab30 +0x21, write 0x000001EC |
| race frame body on a knob-on arm | no |
| hash moved | none run (row kind: no G-CTRL / B / 12x4) |

Attribution: the 0x0047ab30 body is the same DDK bytes before and after (IDENTICAL both times); what changed is that its 5
references to DAT_006bf1cc now link to 0x006bf1cc (abs exact 5, was 0), which the VS2022 0x0047a020 body sets. No control arm
on the head tool was run in this row (the before stop is RACE-PIZ-DEVICE's measured arm on the same image).

0x004E99B0 is one of the all-knobs gate rows held by decision C (not claimed here).

Unfinished scope: 0x004E99B0 (decision C) not worked; s01/004036a0.c stays refused (its port view keeps a private copy too, no
address to bind); the 4 C++ cell-form `wrapper-redefines-globals` refusals are out of scope.

## 5. Commands

`D=D:/MashedWT/DDKOBJ-STATIC-GLOBALS_data`, `O=<orch-wt>`:
`PORTTOP_OUT=$D test-slot bash -c "py -3.12 $O/re/orchestrator/mashed_lock.py run --label DDKOBJ-STATIC-GLOBALS -- py -3.12 verify/porttop_20261108/knob_boot.py kb_vsN D:/MashedWT/DDKOBJ-STATIC-GLOBALS_root 300 MASHED_ORIG_TOP=1 MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=600"`;
`A4_OUT=$D A4_ROOT=D:/MashedWT/DDKOBJ-STATIC-GLOBALS_root A4_EXE=<wt>/mashedmod/build/ddkobj/mashed_re_ddk.exe A4_RO_CKPT=0,1,17,18,299,301,600 test-slot bash -c "py -3.12 $O/re/orchestrator/mashed_lock.py run --label DDKOBJ-STATIC-GLOBALS -- py -3.12 verify/savelink_20261023/run_arms.py RO_ROLOADRWC-sgN"`.

## 6. Frida, raw files

frida-<hash> folders in %LOCALAPPDATA%\Temp: 5 before the first run (`frida_before.txt`), 5 after sg1 (same names), 0 after the
last run (`frida_after.txt`): the 5 went away across the reboot, not deleted by this row. This row uses no Frida, made none,
deleted none.

Raw files, `D:/MashedWT/DDKOBJ-STATIC-GLOBALS_data` (r3 = 7aae3a0f5; `r1/` = 461a34f27, `r2/` = a87ce200f; `piz_base/` =
RACE-PIZ-DEVICE's object-build check):

| file | sha256 |
|---|---|
| build.log | 5d512d96baa8fdfcdd9606bcd55cd294011339fb5c42788a6a0e10eed393b7cb |
| headctrl_build.log | 4966985dd938ea3cd7c104ed382b5aa6a8bf4105dd58d30ff6059d3cc383dca6 |
| kb_vs3.log / kb_vs4.log | 98150f9465ed3658591d9de54aaec7360728829682bf2fbc6ab62ea2761d76f9 / 860aafec008d9fa40db4855819a8f17414d93c380ae771599963170243a505f2 |
| RO_ROLOADRWC-sg3.log | 2aec45736f31e14f2a528ba40e09fec8c3ba81818c3b6b1a4b6e428228d330b3 |
| RO_ROLOADRWC-sg3/meta.json | 58ae3edade719319adf4bf450906a18afdf5472dcc67747b4c26e211e58e8d9f |
| RO_ROLOADRWC-sg3/ticks.bin | 262f44a6b895c58644cf236addb2985481ed659fd1454f323eed5a8ef59e6496 |
| worktree mashedmod/build/ddkobj/mashed_re_ddk.exe | 56271079baff4a18c8d76fc4e52dc1d9107f26f2532f666b4559daca5fb692e4 |
| worktree mashedmod/build/ddkobj_headctrl/mashed_re_ddk.exe | ad1abfc8796fb535cef803c8463fdfa7eb2f0e8ae0742f33f7220d58076a2ae9 |
| worktree mashedmod/build/mashed_re.exe | ba8458fbbc793b9d099a64833d8c281f712816d9f132cbc3269ca939ea294914 |
| worktree mashedmod/build/ddkobj/ddkobj_build.json | 0fbe2570b07a5e030cdf88940ecfdb3d3aeeeaf9eb445cff6829ae62e6a39843 |
| BYTECHECK.tsv (committed here) | 2b7ae62aa6e6b733866347074061fd699b04bd81017a36e2b073e59c10e3c49b |
| root original/mashed_image.bin | 81cc77758ffe93ac0b681eb1fed3d63912d25e7d059503af6d4f871dea9afcd8 |

r1: build.log 4455becd6cd1..., kb_vs1.log 66fa9fefd2a3..., RO_ROLOADRWC-sg1.log 1370998337a4..., exe dd64ccfab841....
r2: exe 5a50e5fa, meta.json 9b660d1a, ticks.bin c013667d.

Committed here: this RESULT, inventory.py, compare_base.py + compare_base.txt, scope_match.py + scope_match.tsv,
ddkobj_selftest.txt, binder_selftest.txt, BYTECHECK.tsv, ddkobj_summary.json, frida_before.txt, frida_after.txt.
The control build `mashedmod/build/ddkobj_headctrl` stays in the worktree, deleted with it.
