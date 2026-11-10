# RESULT PORT-OT-HUD (2026-11-10): GREEN, the HUD pair 0x00413b80 / 0x00413bb0 and three matched callees on their bodies under MASHED_ORIG_TOP

L2 worker `cmwa9vsl31tf5le1ceow80ck6` for lead `cmw7xu1960f37p41557tefpnd`. Branch `orch/PORT-OT-HUD` from orch/integration
`1be11e86d`, with `orch/PORT-OT-FRONTEND` (`3167e6545`, carries PORT-OT-BOOT `a90c0355a`) merged
(`git -c merge.autoStash=false`, one CHANGELOG conflict resolved as a union: FRONTEND's four 2026-11-10 lines above
orch/integration's 2026-11-09 lines). First commit = HB-CAPFLAKE-WALK04-REF (RESULT_HB-CAPFLAKE-WALK04-REF.md).

## Verdict

The HUD pair **0x00413b80 (init) / 0x00413bb0 (teardown)** runs its matched bodies on the MASHED_ORIG_TOP path, with
the three matched callees it needs as exe-only copies: **0x004770c0**, **0x00474db0**, **0x004768c0**. The pair
builds and destroys the 0x40-byte record at 0x0063bd50 (a PTank atomic for the "VehicleIcons" texture). Five rows now
run matched code that ran a stand-in or nothing before. The other five HUD stand-ins stay, each with a reason below.

Yield, plainly: 2 HUD rows (both 1-line wrappers) plus 3 render/vehicle callee rows. Nothing in the exe reads
0x0063bd50, so the knob path draws nothing new. The value is the port column and two fewer stand-ins.

## Scope: how the HUD rows were found

- `origtop_base_dump.py --dump-base` on this tree (SIZE.tsv restored afterwards, it belongs to PORT-ORIGTOP-SIZE):
  39 rows with subsystem `hud` in the base runnable set (1,004 rows in all), 4 of them `stand-in` in BINDINGS (0x00413b80, 0x00413bb0,
  0x00555830, 0x00556e40), the rest already matched or with no BINDINGS row (under a stand-in ancestor).
- BINDINGS.tsv stand-ins whose hooks.csv subsystem is `hud`: 7 = the 4 above + 0x00427ca0, 0x00404820, 0x00552b90.
  This matches the lead's FRONTEND pre-scan list.
- OT_MEASURE.tsv lists the base rows with a RwGlobals or seed caveat. 0x00413b80 / 0x00413bb0 are not in it (no
  caveat). Their callees: 0x004768c0 `OK` (+0x10c only), 0x00474db0 `OK` (+0x108 only), 0x004770c0 seed `OK`
  (0x006132c4 / 0x006132c8 seeded).

## What changed (one commit)

| rva | before | now | TU |
|---|---|---|---|
| 0x00413b80 | `PortTop_StandIn04G_00413b80` (in 0x0040bbb0) | `Orig_00413b80` | new Match/PortTop_00413b80.cpp |
| 0x004770c0 | (not reached: its only knob-path caller was the 0x00413b80 stand-in) | `Orig_004770c0` | new Match/PortTop_004770c0.cpp, **ORIGTOP-ONLY** DUP-IN-TARGET vs Render/RfxEmitter.cpp `Create` |
| 0x00474db0 | (not reached) | `Orig_00474db0` | new Match/PortTop_00474db0.cpp, **ORIGTOP-ONLY** CROSS-TARGET vs .asi-only Match/MatchS11_00474db0.cpp |
| 0x00413bb0 | `PortTop_StandIn04C_00413bb0` (in 0x0040bd00) | `Orig_00413bb0` | new Match/PortTop_00413bb0.cpp, **ORIGTOP-ONLY** CROSS-TARGET vs .asi-only Render/PromoLoop_round256.cpp |
| 0x004768c0 | `PortTop_StandIn04C_004768c0` | `Orig_004768c0` from 0x00413bb0 only | new Match/PortTop_004768c0.cpp |

Callee check (the BOOT / FRONTEND lesson). Every callee binds by name to an exe symbol, no raw original address:
- 0x00413b80: 0x0040bb30 `Search40bb30` (MatchS02_0040bb30.cpp), 0x004770c0 `Orig_004770c0`, 0x00476cb0 `Batch476cb0`
  (MatchS12_00476cb0.cpp, the matched source on the original record layout).
- 0x004770c0: 0x004b6520 `Orig_004b6520`, 0x00534b60 `MashedRw_PTankAtomicCreate_00534b60` and 0x004c0b30
  `MashedRw_FrameCreate_004c0b30` (LibRw/RwApiBind.cpp), 0x004e8090 `Orig_004e8090`, 0x004e7e30 `Orig_004e7e30`,
  0x00474db0 `Orig_00474db0`, and the six optional setters 0x00476ae0 `Batch476ae0`, 0x00476b30 `Set476b30`, 0x00476b90
  `Batch476b90`, 0x00476be0 `Batch476be0` (MatchS12_*.cpp), 0x00476c10 `Batch476c10`, 0x00476c60 `Batch476c60`
  (RfxEmitter.cpp). Flags 0x37 call none of the six (re/match/s12/004770c0.c:92-97 test bits 0x80..0x1000).
- 0x00474db0: RwGlobals +0x108 (librw malloc, LibRw/RwApiBind.cpp:1215), 0x004b6520 `Orig_004b6520`.
- 0x00413bb0: 0x004768c0 `Orig_004768c0`.
- 0x004768c0: 0x004e7e30 `Orig_004e7e30`, 0x004c0c20 `MashedRw_FrameDestroy_004c0c20`, 0x004e6920
  `MashedRw_AtomicDestroy_004e6920` (the PTank-record case frees the record and drops the texture count 0x004e8090
  raised), RwGlobals +0x10c (librw free, LibRw/RwApiBind.cpp:1216), 0x004b6520 `Orig_004b6520`.

Why 0x004770c0 needed its own copy: the exe body of that RVA is the hand `Rfx::Create(Emitter&, ...)`
(Render/RfxEmitter.cpp:247), on a port `Emitter` struct, not the original 0x40-byte record. Passing 0x0063bd50 to it
would write a different layout past the record. The PTank record the PORT-TOP-04I shim returns has every field
0x004770c0 writes: geometry +0x18 -> materials +0x20 -> material[0], the frame at +0x04, and the block at +0x70
(= [0x007dc57c], set by the 0x00493640 shim) with room to +0x100 for the +0x04 / +0x40 / +0xa4 / +0xa8 / +0xac stores.

Why 0x004768c0 keeps its stand-in for 0x004210f0: that caller passes 0x0063e548, a record built by 0x004210b0, which is
still `PortTop_StandIn04G_004210b0`. With the record zero, the matched body's first read is `*(0 + 4)`. BINDINGS keeps
the row as `stand-in` and names both symbols.

Reachability. Every new body is reached only through the matched 0x0040bbb0 (init, from 0x00402750) and the matched
0x0040bd00 (teardown, from AppDestroy 0x00402a40), both entered only from the matched WinMain 0x00492370
(MASHED_ORIG_TOP). No hand code reads 0x0063bd50 (grep over mashedmod/src: only comments and the .asi-only
PromoLoop_round256.cpp / PromoLoop_round69.cpp).

Source edits, each re-checked with the DDK:
- re/match/s03/raw_00413b80.c, raw_00413bb0.c: the old typedef header (C++ rejects `typedef ... bool` and
  `wchar_t`) replaced with the MATCH-S02 preamble already used by raw_0041a3d0.c, and `extern undefined1 DAT_0063bd50;`
  became `MGLOBAL(undefined1, DAT_0063bd50, 0x0063bd50);` (extern under C, as before).
- re/match/s12/004768c0.c: `typedef int code();` became `int code(int)` under `__cplusplus` only (the same pattern as
  re/match/s11/00474db0.c). C text unchanged.
- `match.py --bytes` after the edits: 0x00413b80 43 B, 0x00413bb0 12 B, 0x004768c0 211 B, all `BYTE-IDENTICAL`
  (same as before the edits). `relocheck.py`: 0x00413b80 OK 6/6, 0x00413bb0 OK 2/2, 0x004768c0 OK 11/11.
- C++ conversions the raw sources make: `&DAT_0063bd50` to `uint*` / `int`, a string literal to `undefined4`. Each
  callee name binds to a C++ overload taking the pointer (the Match/PortTop_00420d00.cpp pattern). 0x004770c0 and
  0x004768c0 bind through `extern "C"` forwarders with the source prototypes (the MatchS14_004881d0.cpp pattern).

Other files: `mashedmod/exe_sources.rsp` +5 TUs. `re/tools/dual_copy_allowlist.txt` +3 `ORIGTOP-ONLY` lines (the
`lint_rva_bodies.py --seed` keys). BINDINGS.tsv through the new `update_bindings_othud.py` (runs
`update_bindings_otfrontend.py` first): 433 rows, matched 157 -> 161, stand-in 95 -> 93, 2 rows added (0x004770c0,
0x00474db0).

## HUD rows left, with the reason

| rva | name | reason |
|---|---|---|
| 0x00555830 | FUN_00555830 | OT_MEASURE `needs binding`: null RwGlobals call slot +0x11c |
| 0x00556e40 | FUN_00556e40 | OT_MEASURE `needs binding`: null RwGlobals call slot +0x11c |
| 0x00552b90 | FontSys_Shutdown | not in the base runnable set |
| 0x00427ca0 | FontText_HudInit | not in the base runnable set |
| 0x00404820 | FUN_00404820 | not in the base runnable set |

## Acceptance

Machine rules: the build waited for merge train `20261110_030142` (PRIORITY-HOLD pids 18764 / 22184, then a SOLO
window) to clear. `%TEMP%\mashed_machine_priolinger_18764.lock` read `held: false` at build time. The game runs then
queued behind train `20261110_033214`'s SOLO. Every launch ran as `test-slot bash -c 'py -3.12
<orch-wt>/re/orchestrator/mashed_lock.py run --label PORT-OT-HUD -- ...'`. Private root `D:/MashedWT/PORT-OT-HUD_root`
(`original` = junction to `D:/MashedWT/PORT-TOP-04J_rootK/original`, the same as PORT-TOP-KNOBON-CAPFLAKE_rootK).

| check | result |
|---|---|
| syntax check before the build | `cl /Zs /EHa /W3 /O2 /DMASHED_STANDALONE` on the 5 new and 3 edited TUs: no error |
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `[skip] MASHED_NO_DEPLOY set: mashed_re_dev.asi NOT deployed to original\`. mashed_re.exe sha256 `dc988db58e16d63c6d4399868b4b81726c379c035646eec95f273501246ca548` |
| knob-ON boot (`knob_boot.py knob_on_1 <root> 300 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | `knob_on_1: exited rc=0 (0x00000000) after 30.4 s`. knob_on_1.log:226 `RpPTankAtomicCreate(20, 0x100000A7 -> 0x100000A7, 0) -> PTank record 0F7AD730` (cap 0x14, flags 0x37 mapped by 0x004770c0 to 0x10000001 \| 4 \| 2 \| 0x20 \| 0x80). :439-445 the teardown: `RpAtomicDestroy(PTank record 0F7AD730): material texture 016D1370 released, record freed`, `RwTextureDestroy(0x016D1370)`, then 5 `RwGlobals +0x10c free(...)` = the five arrays flags 0x37 allocate (p[4..8]). No stand-in line for 0x00413b80 / 0x00413bb0 (65 stand-in lines in FRONTEND's knob_on_1.log, 63 here). :420 `rva 0x004768c0: stand-in, not run` is the 0x004210f0 call, as planned. Last line :621 `WinMain 0x00492370 returned 0 after 3000 frames` |
| 12 captures, OFF / ON / OFF / ON, pin default (`CAPFLAKE_DATA=D:/MashedWT/PORT-OT-HUD_data CAPFLAKE_ROOT=<root> capflake_runs.py nav1 OFF@dflt ON@dflt OFF@dflt ON@dflt`) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. The 12 hashes are equal in all four runs, knob ON and OFF. With the HB-CAPFLAKE-WALK04-REF list, walk_04 `4d53d93f87f0332d` is now counted as E |
| G-CTRL (H_DEF) | `8284be46/67931834/31fe043e` = merge_train.json `gctrl.ref` |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 = `b.ref` |
| rva-lint | `D4 burn-down=2  ORIGTOP-ONLY=8 (class deleted at the ORIG_TOP flip; 9 marked TU(s))`, `OK -- no NEW duplicate bodies (10 known)`. Before: burn-down 2, ORIGTOP-ONLY 5 |

The first gates run stopped on a harness error: `run_arms.py` copies its outputs into `A4_OUT` without creating it
(`FileNotFoundError` at run_arms.py:294 after H_DEF exited). The arms were re-run after `mkdir`. Not a finding.

### Port column deltas

`backfill_exe_file.py` (`rows given an exe_file: 5`, `old value changed: 1`), then `backfill_port.py`:
`none=5177 body=548 called=430`, 42 rows changed.

This step:

| rva | port |
|---|---|
| 0x00413b80, 0x00413bb0, 0x00474db0, 0x004768c0 | none -> called |
| 0x00476cb0 | body -> called (called by 0x00413b80) |
| 0x00476c10, 0x00476c60 | body -> called: static reachability through the 0x004770c0 forwarders. Flags 0x37 never call them at run time |
| 0x004770c0 | called (unchanged), `exe_file` gains Match/PortTop_004770c0.cpp next to Render/RfxEmitter.cpp |

The other 35 changed rows are FRONTEND's own row 0x00428390 and the 34 drift rows FRONTEND listed (BOOT's rows and
earlier merged bodies, all none -> called, 0x004026d0 and 0x004b64e0 body -> called): this tree's hooks.csv came from
the merge with orch/integration's cells for them, and the backfill on this build writes them again.

### Raw files (D:/MashedWT/PORT-OT-HUD_data, not committed)

| file | sha256 |
|---|---|
| build.log | 92d86dea3d3caba090485ba0f85a2129ae6b448798041c34394d05318fd6b09b |
| knob_on_1.log | 98bd4ae1e7ed419323b33ec35130b17cebca96b6049b6f9acb0b98e92d2b1299 |
| runs.tsv | 84966e87b0dd95e296ffd3bfc456ce1d2f8ef1019b26462f3308e12c26a1cdcc |
| nav1/r01_OFF_pindflt.log | c7db4c8ebbc3ccebcc6bf8aaa3d28c15620d49d85636736a3fe0e10088b4d272 |
| nav1/r02_ON_pindflt.log | 915802a09557699a8e47086b6330530f65a1fd4b43583062d7cf603a9de8f7e1 |
| nav1/r03_OFF_pindflt.log | 44c5d0bc68222cf7acde2e375f27d0b291a30b86940513da6ce3a9b41307309c |
| nav1/r04_ON_pindflt.log | 80d77e317f5a812dcb72a28475b943238ce96a0c231546c4340dcb8e19890519 |
| nav1/r0{1,2,3,4}_*/walk_04_colourselect.bmp | 4d53d93f87f0332d... (all four equal) |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv, B_DEF_K1.step.csv, B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |

## Next

- 0x004210b0 / 0x004210f0 (effect init / teardown of the record 0x0063e548) are the same shape as this pair: 0x004210b0
  calls 0x00421080, 0x004770c0 and a bit-array clear. With `Orig_004770c0` and `Orig_004768c0` now in the exe, that
  pair can swap next (then 0x004768c0's last stand-in caller goes and its BINDINGS row becomes `matched`).
- The two +0x11c rows (0x00555830, 0x00556e40) wait on the RwGlobals slot binding (PORT-OT-RWG-SLOTS).
- Harness: `run_arms.py` could `os.makedirs(OUT, exist_ok=True)` before the first copy.
