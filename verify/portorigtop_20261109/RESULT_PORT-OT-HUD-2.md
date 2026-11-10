# RESULT PORT-OT-HUD-2 (2026-11-10): GREEN, the effect pair 0x004210b0 / 0x004210f0 and the RWMOD-gated 0x00555830 / 0x00556e40 on matched bodies under MASHED_ORIG_TOP

L2 worker `cmwajfmms25dzle1ca6nsdgms` for lead `cmw7xu1960f37p41557tefpnd`, L-111 (a) step 1. Branch `orch/PORT-OT-HUD-2`,
**rebased onto orch/integration `8e45017e1`** (after the PORT-OT-RENDER / PORT-OT-OTHER drain) at the lead's request:
- `b91c2aa0d` = the code commit `8565ea326` replayed. Two conflicts, both unions: `mashedmod/exe_sources.rsp` (OTHER's
  4 TUs and my 7) and `re/tools/dual_copy_allowlist.txt` (GAMEPLAY's 6 ORIGTOP-ONLY lines and my 3).
  Match/PortTop_0040bbb0.cpp auto-merged.
- `91b417f89`: the first rebased build failed with C2084 at LibRw/RwApiBind.cpp:1550. PORT-OT-IMAGE-FMT (`dd2cb7e8c`) had
  added the same `MashedRw_RwModKnob` accessor (same body, in an extern "C" block). My copy is dropped, so RwApiBind.cpp is
  unchanged against orch/integration. The BINDINGS updater now chains the three updater tips on orch/integration (below).
- This RESULT commit.

The pre-rebase history (HUD + RWG-MODULES + integration `086427678`, GREEN on exe `5b2b11e8`) is kept as
`backup/PORT-OT-HUD-2-prerebase` (`8bf4fd43a`). Every result below is on the rebased head.

## Verdict

On the MASHED_ORIG_TOP path:

- **0x004210b0 (effect init) and 0x004210f0 (its teardown) run matched bodies together.** 0x004210b0 builds the 0x40-byte
  record at 0x0063e548 through `Orig_004770c0` (flags 0x817, capacity 0x80, texture "smoke"). 0x004210f0 now calls
  `Orig_004768c0` on it. The stand-in `PortTop_StandIn04C_004768c0` had one caller left (0x004210f0) and is removed, so
  0x004768c0's BINDINGS row is `matched`.
- **0x00555830 and 0x00556e40 have exe bodies and run only with `MASHED_ORIG_TOP_RWMOD=1`.** Their call sites in 0x00427620
  are selectors (below). On today's knob path 0x00555830's body runs with RWMOD on, on a zero handle (its `param_1 == 0`
  branch, returns 0). 0x00556e40's body does not run: its four handles are 0, because the init 0x00427ca0 that writes them
  is still a stand-in, and the body reads `*(handle + 0x74)` with no null check.

Rows that now have a matched exe body on this path: 0x004210b0, 0x00421080, 0x004b6480, 0x00555830, 0x00556e40,
0x005c4d30, 0x005c4d50 (7, port none -> called), plus 0x004768c0's second caller. One new stand-in: 0x00553e80 (no matched
source), reached only for a non-zero 0x00555830 argument.

## What changed

| rva | before | now | TU |
|---|---|---|---|
| 0x004210b0 | `PortTop_StandIn04G_004210b0` (in 0x0040bbb0) | `Orig_004210b0` | new Match/PortTop_004210b0.cpp |
| 0x00421080 | (not reached) | `Orig_00421080` | new Match/PortTop_00421080.cpp, **ORIGTOP-ONLY** CROSS-TARGET vs .asi-only Match/MatchS05_00421080.cpp |
| 0x004b6480 | (not reached) | `Orig_004b6480` | new Match/PortTop_004b6480.cpp, **ORIGTOP-ONLY** CROSS-TARGET vs .asi-only Input/DirectInput.cpp |
| 0x004210f0 | calls `PortTop_StandIn04C_004768c0` | calls `Orig_004768c0` | Match/PortTop_004210f0.cpp |
| 0x004768c0 | stand-in from 0x004210f0, matched from 0x00413bb0 | matched from both | stand-in deleted from Match/PortTop_StandIns04C.cpp |
| 0x00555830 | `PortTop_StandIn04C_00555830` | selector: `Orig_00555830` with RWMOD, else the stand-in | new Match/PortTop_00555830.cpp |
| 0x00556e40 | `PortTop_StandIn04C_00556e40` | selector: `Orig_00556e40` with RWMOD and a non-zero handle, else the stand-in | new Match/PortTop_00556e40.cpp |
| 0x005c4d30 | (not reached) | `Orig_005c4d30` | new Match/PortTop_005c4d30.cpp, **ORIGTOP-ONLY** CROSS-TARGET vs .asi-only Util/PromoLoop_sessionB.cpp |
| 0x005c4d50 | (not reached) | `Orig_005c4d50` | new Match/PortTop_005c4d50.cpp (no other body in either target) |
| 0x00553e80 | (not reached) | stand-in `PortTop_StandInOTH2_00553e80` (logs, returns 0) | in Match/PortTop_00555830.cpp |

Matched sources, all unedited by this step. `match.py --bytes` and `relocheck.py` on this tree:

| rva | source | bytes | relocheck |
|---|---|---|---|
| 0x004210b0 | re/match/s05/004210b0.c | 55 B BYTE-IDENTICAL | OK 7/7 |
| 0x00421080 | re/match/s05/00421080.c | 43 B BYTE-IDENTICAL | OK 3/3 |
| 0x004b6480 | re/match/s18/004b6480.c | 89 B BYTE-IDENTICAL | OK 0/0 |
| 0x005c4d30 | re/match/s28/005c4d30.c | 17 B BYTE-IDENTICAL | OK 0/0 |
| 0x005c4d50 | re/match/s28/005c4d50.c | 69 B BYTE-IDENTICAL | OK 2/2 |
| 0x00555830 | re/match/s24/00555830.c | 215 B BYTE-IDENTICAL | OK 7/7 |
| 0x00556e40 | re/match/s24/00556e40.c | 67 B BYTE-IDENTICAL | OK 3/3 |

No re/match source was edited, so no `#ifndef MATCH_PORT` guard was needed. The C++ conversions the raw text makes are
taken by forwarders in the PortTop TUs:
- 0x004210b0 passes a string literal to an `undefined4` parameter and `&DAT_0063e548` (undefined1*) to `uint*`. C++
  overloads `PortTop_Call0040bb30_004210b0(const char*)` and `PortTop_Call004770c0_004210b0(void*, ...)` take them (the
  Match/PortTop_00413b80.cpp pattern).
- 0x004210f0 passes `&DAT_0063e548` to `void*`. `PortTop_004210f0_Call004768c0(void*)` forwards to `Orig_004768c0(int)`.
- 0x00555830 declares `Entry20 *FUN_005c4d30(int)` and `int FUN_005c4d50(int)` against the s28 prototypes
  `undefined4 (undefined4*)` / `undefined4 (int*)`. Forwarders are defined after the source text so they can use its `Entry20`.
- 0x004210b0 declares `FUN_004b6480(byte*, uint)` against the s18 `(unsigned char*, int)`, and 0x00556e40 declares
  `void FUN_004c5a60(int)` against the exe `int MashedRw_TextureDestroy_004c5a60(int)`. One forwarder each.
- Every callee name is an object-like `#define FUN_x Name`.

### Callees, all bound by name (no raw original address)

- 0x004210b0: 0x0040bb30 `Search40bb30` (Match/MatchS02_0040bb30.cpp, both targets), 0x004770c0 `Orig_004770c0`
  (Match/PortTop_004770c0.cpp, PORT-OT-HUD), 0x00421080 `Orig_00421080`, 0x004b6480 `Orig_004b6480`.
  - Flags 0x817 = 0x800 | 0x10 | 4 | 2 | 1. In re/match/s12/004770c0.c this maps to PTank flags 0x10000001 | 4 | 2 | 0x20 |
    0x80000 = 0x10080027, allocates p[4] / p[5] / p[6] / p[8] through 0x00474db0, and calls one optional setter, 0x00476c10
    (`Batch476c10`, Render/RfxEmitter.cpp, which writes the plugin block +0xd8..+0xe4 from `s00613294` = (0, 0, 1, 1) in the
    exe and sets bit 0x80000 at +0x40).
- 0x004210f0: 0x004768c0 `Orig_004768c0`.
- 0x00555830: 0x005c4d30 `Orig_005c4d30`, 0x005c4d50 `Orig_005c4d50`, 0x004c5a60 `MashedRw_TextureDestroy_004c5a60`
  (LibRw/RwApiBind.cpp), 0x00553e80 the stand-in, RwGlobals +0x10c (librw free) and +0x11c (`RwGlobalsFreeListFree`,
  registered only with RWMOD).
- 0x00556e40: 0x004c5a60 `MashedRw_TextureDestroy_004c5a60`, RwGlobals +0x11c.
- 0x005c4d50: RwGlobals +0x10c. 0x00421080, 0x004b6480, 0x005c4d30: no callees.

### How "only with RWMOD" is enforced

- The matched bodies of 0x00555830 and 0x00556e40 have one caller in the exe: the matched 0x00427620
  (Match/PortTop_00427620.cpp, AppDestroy 0x00402a40, MASHED_ORIG_TOP only). Their `FUN_x` names there bind to two selectors
  in that TU, `PortTop_00427620_Call00555830` and `PortTop_00427620_Call00556e40`. Nothing else references
  `Orig_00555830` / `Orig_00556e40`.
- The selectors read `MashedRw_RwModKnob()` (LibRw/RwApiBind.cpp, added by PORT-OT-IMAGE-FMT for 0x00493600). It returns the same
  `RwModKnob()` that decides whether `MashedRw_InstallRwGlobals` registers +0x118 / +0x11c, so the matched body runs exactly
  when +0x11c is bound.
- 0x00556e40's selector also needs a non-zero handle. The body's first read is `*(param_1 + 0x74)` (re/match/s24/00556e40.c),
  and the four handles DAT_0067d83c / 0067d840 / 0067d844 / 0067d848 are written only by 0x00427ca0
  (re/match/s06/00427ca0.c), which is `PortTop_StandIn04E_00427ca0` on this path. This is the same class as PORT-OT-HUD's
  0x004768c0 on a zero record. The matched body runs from the selector once 0x00427ca0 swaps in.
- 0x00555830 has its own `param_1 != 0` test, so its selector checks only the knob.
- Each selector logs its first call with the path it took. The log lines are below.

### Reachability (the ORIGTOP-ONLY rule)

Every new body is reached only through matched code entered from the matched WinMain 0x00492370 (MASHED_ORIG_TOP):
- 0x004210b0 / 0x00421080 / 0x004b6480: from the matched 0x0040bbb0 (from 0x00402750).
- 0x004210f0 -> 0x004768c0: from the thunk 0x00421590 in AppDestroy 0x00402a40.
- 0x00555830 / 0x00556e40 / 0x005c4d30 / 0x005c4d50 / 0x00553e80: from the matched 0x00427620 in AppDestroy 0x00402a40.

The three TUs whose RVA has a second, .asi-only body carry `// RVA-LINT: ORIGTOP-ONLY` and an `ORIGTOP-ONLY` allowlist
line (keys from `lint_rva_bodies.py --seed`): 0x00421080, 0x004b6480, 0x005c4d30.

## Acceptance

Machine rules:
- The build ran from a loop that waited until `mashed_lock.py status` showed no PRIORITY-HOLD / SOLO and no
  `%TEMP%\mashed_machine_priolinger_*.lock` was `held: true` with a live pid.
- Every launch ran as `test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label PORT-OT-HUD-2 -- ...'`.
- Private roots, each with `original` = junction to `D:/MashedWT/PORT-TOP-04J_rootK/original` (the PORT-OT-HUD
  arrangement): `D:/MashedWT/PORT-OT-HUD-2_root` (boots, captures) and `D:/MashedWT/PORT-OT-HUD-2_rootA` (gates). `A4_OUT` =
  `D:/MashedWT/PORT-OT-HUD-2_data2/arms`, created beforehand (HB-RUNARMS-AOUT-MKDIR).

| check | result |
|---|---|
| syntax check | `cl /Zs /EHa /W3 /O2 /std:c++17 /DMASHED_STANDALONE` on the 7 new and 5 edited Match TUs: no error |
| build (`MASHED_NO_DEPLOY=1`, tree `91b417f89`) | `=== Build OK ===`, `[skip] MASHED_NO_DEPLOY set: mashed_re_dev.asi NOT deployed to original\`. mashed_re.exe sha256 `77c8eea58859c6c25fc4369a3f3b32a4a0a14aa1896da5948994d0869888e88b` |
| knob-ON boot, RWMOD off (`knob_boot.py knob_on_1 <root> 300 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | `knob_on_1: exited rc=0 (0x00000000) after 45.5 s`. :248 `RpPTankAtomicCreate(128, 0x10080027 -> 0x10080027, 0)` = the 0x004210b0 record (capacity 0x80, flags 0x817 mapped as above). The six other 0x10080027 creates at :221-235 (capacity 64 / 80) are PORT-OT-OTHER's particle emitters. :440 `RpAtomicDestroy(PTank record 10B51258)` = its teardown. :602 `rva 0x00555830(0x00000000): stand-in (RWMOD off)`, :604 `rva 0x00556e40(0x00000000): stand-in (RWMOD off)`. No stand-in line for 0x004210b0 or 0x004768c0. :769 `WinMain 0x00492370 returned 0 after 3000 frames` |
| knob-ON boot, RWMOD on (same + `MASHED_ORIG_TOP_RWMOD=1`) | `knob_on_rwmod: exited rc=0 (0x00000000) after 50.1 s`. :304 the same capacity-128 create (record 0F7F27E8). :331 `RwGlobals [MASHED_ORIG_TOP_RWMOD]: modules registered (...)`. :501 `RpAtomicDestroy(PTank record 0F7F27E8): material texture 0F6306F8 released, record freed`, :502 `RwTextureDestroy(0x0F6306F8)`, :503-506 four `RwGlobals +0x10c free(...)` = p[4] / p[5] / p[6] / p[8] (the teardown 0x004210f0 -> 0x004768c0). :663 `rva 0x00555830(0x00000000): matched body (RWMOD on)`, :664 `rva 0x00556e40(0x00000000): stand-in (RWMOD on, handle 0: 0x00427ca0 init is a stand-in)`. :829 `WinMain 0x00492370 returned 0 after 3000 frames` |
| 12 captures, RWMOD off (`CAPFLAKE_DATA=<data2> CAPFLAKE_ROOT=<root> capflake_runs.py nav1 OFF@dflt ON@dflt OFF@dflt ON@dflt`) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. The E list on this tree carries walk_04 `ba34030d1b7e2fe4` (post-ICON reference, L-112 (a)) |
| 12 captures, RWMOD on (`MASHED_ORIG_TOP_RWMOD=1` in every run's env, arm `nav_rwmod`) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. walk_04 = `ba34030d1b7e2fe4` in all 8 runs. No other capture moved |
| G-CTRL (H_DEF) | `8284be46/67931834/31fe043e` = merge_train.json `gctrl.ref` |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 = `b.ref` |
| rva-lint (`scripts/lint_rva_bodies.py`) | `D4 burn-down=2  ORIGTOP-ONLY=26 (class deleted at the ORIG_TOP flip; 27 marked TU(s))`, `OK -- no NEW duplicate bodies (28 known)`. orch/integration `8e45017e1` alone: ORIGTOP-ONLY 23 (this step adds 3) |

## BINDINGS and the port column

`verify/portorigtop_20261109/update_bindings_othud2.py` now runs the three updater tips on orch/integration first:
`update_bindings_otgameplay.py`, `_otvehicle.py` and `_otother.py`. OTHER runs RENDER, and each tip runs
`update_bindings_othud.py` and the FRONTEND / OT-BOOT / 04J chain. No updater chains GAMEPLAY or VEHICLE, so chaining
only one tip would skip rows.
- Checked first: those three alone reproduce the committed BINDINGS.tsv byte for byte. None of their FIXED rows is one of
  this step's 9.
- Then it writes the 9 rows: 451 rows (447 before, +4: 0x00421080, 0x004b6480, 0x005c4d30, 0x005c4d50), matched 190 -> 198,
  stand-in 73 -> 70 (0x004210b0, 0x004768c0, 0x00555830, 0x00556e40 leave it, 0x00553e80 joins it from kind `rw`).

`backfill_exe_file.py` (`old value changed: 0`) then `backfill_port.py` on this build: `none=5131 body=552 called=472`, 7 rows
changed, all this step's: 0x004210b0, 0x00421080, 0x004b6480, 0x00555830, 0x00556e40, 0x005c4d30, 0x005c4d50 none -> called.
0x004768c0 was `called` before (from 0x00413bb0 and OTHER's callers) and stays `called`. No drift row.

## Left for later

- 0x00427ca0 FontText_HudInit: its swap would give 0x00556e40 non-zero handles (and 0x00555830 a non-zero 0x0067d838). It is
  not in the base runnable set (PORT-OT-HUD's table). The exe copy forwards to raw Rt2d font addresses (stand-in reason in
  Match/PortTop_00402750_Pieces.cpp).
- 0x00553e80: no matched source. It runs only for a non-zero entry obj of a non-zero 0x00555830 argument, so it matters only
  after 0x00427ca0.

## Raw files (D:/MashedWT/PORT-OT-HUD-2_data2, not committed)

| file | sha256 |
|---|---|
| build.log | a55c1e3d32c3beb05aae75a8532dc250b466a568c0cb138c0ada502488f920df |
| knob_on_1.log | 6d994c5b63d6bca0ec4464ae57a26f5a9199d53fee1ff0744c600d33671c8250 |
| knob_on_rwmod.log | 0d79fba38a5867fb964bf3a543e4ec8da33fb09ba2ea79a2db8660a08df45c9f |
| runs.tsv (both capture arms) | f560646e9825f27b38517ec0c4d851cd17931fc8944763b7fbaa1d1204796438 |
| cap_nav1.out | 853d70ae85e0bffbe0486696fe747069157447e4e505675aac3a046c456918c2 |
| cap_rwmod.out | 52f72371f59882001bbbc152b414d5b3e5ba71884e3d8b4537a27e3108217f45 |
| nav1/r01_OFF_pindflt.log | 5383f9871c50fd39109d316dbf200d3f67836d4aad91c5621f495dcd4fe9e3c0 |
| nav1/r02_ON_pindflt.log | b7b47155cd5fcd32acd28754e34a7328fe085710e344bb8634826940268594a3 |
| nav1/r03_OFF_pindflt.log | 14915582a426f0732acd8f270ffbd54a52ff5a1b53ce8d2bbab90c3b31b13485 |
| nav1/r04_ON_pindflt.log | 2a675c427858d705f4c173b178ee66955c9edc1f3b3b04c771dd376e49b9aa21 |
| nav_rwmod/r01_OFF_pindflt.log | 7a301000bad1dc49469d05dd24f5a9251d11b65454188f4b857918c65bbb3fbd |
| nav_rwmod/r02_ON_pindflt.log | 2b56e15cb50623fe2c1951a1c2da0481d7484ed9bfee71c666264e23441238a1 |
| nav_rwmod/r03_OFF_pindflt.log | 24b9027a65cb0127e2a1c63c082042a784955501eb3fa491bfa59d337d2512ef |
| nav_rwmod/r04_ON_pindflt.log | 1f2070e2fbcc1babfab2d3d6b03db6de3177814dbf5732ca26ee3809160ddf9e |
| nav1/r0{1..4}_*/walk_04_*.bmp, nav_rwmod/r0{1..4}_*/walk_04_*.bmp | ba34030d1b7e2fe4... (all eight equal) |
| gates_run.log | ae9859dc08bfc1a72f3fd774e522c635d343218442afa490045d3541ed096397 |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv, B_DEF_K1.step.csv, B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |

The pre-rebase runs (exe `5b2b11e8`, same verdicts, walk_04 `4d53d93f87f0332d` then) are in `D:/MashedWT/PORT-OT-HUD-2_data`.
They are not cited above.
