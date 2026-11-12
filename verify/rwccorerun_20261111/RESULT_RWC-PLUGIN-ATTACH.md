# RESULT RWC-PLUGIN-ATTACH: plugin attach on the matched RW core (L-124 (C) step 3)

Run 39, 2026-11-12. Branch `orch/RWC-PLUGIN-ATTACH`, based on `orch/RWC-CORE-RUN` e1205e9a2 (run 38, engine lifecycle
on the core), so that branch's progress lands with this one. Same knob (`MASHED_RWC_CORE`, default OFF, read only with
`MASHED_ORIG_TOP=1`) and the same four arms as `PREREG_RWC-CORE-RUN.md`. Top-down row: G-CTRL / B / 12 captures not run
(WORKER_RULES). No tracker edit.

@@VERDICT@@

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

@@LIFT@@

## 4. Builds

@@BUILDS@@

## 5. Knob boots

@@BOOTS@@

## 6. Self-test

@@SELFTEST@@

## 7. Next blocker and follow-up rows

@@NEXT@@

## 8. Process notes

- Merge-train PRIORITY-HOLDs ran almost continuously from 07:32 to 11:03 (four trains). Builds waited for a gap each
  time. One `build.bat` was started at 07:39 while a hold was shown (the status and the build ran in the same command);
  it stopped at the rva-lint step after a few seconds, before any compile. Every later build waited for the hold to clear.
- Raw data `D:/MashedWT/RWC-PLUGIN-ATTACH_data` (sha256 in `RAW_SHA256_RWC-PLUGIN-ATTACH.txt`).

## Handoff (2026-11-12 11:52, LIVING-PC reboot for the token leak)

State at the safe point:
- Branch `orch/RWC-PLUGIN-ATTACH` @ WIP commit 5e542495c plus this RESULT draft. Worktree
  `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\RWC-PLUGIN-ATTACH`. Private root `D:/MashedWT/RWC-PLUGIN-ATTACH_root`
  (junction `original` -> `D:/MashedWT/PORT-TOP-04J_rootK/original`), raw data `D:/MashedWT/RWC-PLUGIN-ATTACH_data`.
- Default VS2022 build of the committed tree: Build OK 11:51 (`build_vs8.log`), rva-lint OK (159 known, 0 new).
- No knob boot of the lifted tree has run yet (the 11:06 attempt sat in the lock queue and was stopped by the tool's
  background limit). The only boot so far is `vs_core.log` 07:20 (pre-lift tree): attach = 1, STOP at 0x004e7ef0.

Next, in order:
1. `test-slot bash -c "py -3.12 ../orch-integration/re/orchestrator/mashed_lock.py run --label RWC-PLUGIN-ATTACH -- bash verify/rwccorerun_20261111/run_arms_attach.sh vs_core"`
   (run_in_background, timeout >= 3 h because of the lock queue). Read the `[RWC_CORE]` lines of
   `D:/MashedWT/RWC-PLUGIN-ATTACH_data/vs_core.log`: expected next step is RwEngineStart running the lifted module
   constructors; the first failure is either a `STOP: rva` (one of the 94 stops, `LIFT_REFUSED.tsv` says why) or an AV
   (the VEH logs fault + stack + registry).
2. Fix forward (a stop: lift it by hand or bind it; an AV: resolve the frame with `mapsym.py`), rebuild, re-boot.
3. Object build `set MASHED_DDKOBJ=1` (no build during a PRIORITY-HOLD), `py.exe -3.12 re/tools/ddkobj/ddkobj.py check`
   (copy `BYTECHECK.tsv`), then all four arms (`run_arms_attach.sh` with no argument) and `logcmp.py` OFF vs OFF.
4. Fill the @@ sections above, raw sha256 list, `roadmap.ps1 set RWC-PLUGIN-ATTACH review -Evidence verify/rwccorerun_20261111/RESULT_RWC-PLUGIN-ATTACH.md`.
