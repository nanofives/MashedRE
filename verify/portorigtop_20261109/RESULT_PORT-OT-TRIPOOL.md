# RESULT PORT-OT-TRIPOOL (2026-11-10): GREEN, the Tri.dff slot table 0x0048ff20 / 0x0048ffd0 / 0x00490000 swapped onto matched bodies under MASHED_ORIG_TOP

L2 worker `cmwb2cbqb126esc1b0xhgr59c` for lead `cmw7xu1960f37p41557tefpnd`, L-111 step 1, item 2 of
RESULT_PORT-OT-RENDER-2.md. Branch `orch/PORT-OT-TRIPOOL`. It was first cut from orch/POWERUPINIT-A `eadfbfdbe`, because A had
not merged yet. It was then rebased onto orch/integration `05ea6cb1e` (POWERUPINIT-A and RENDER-2 merged), per the lead.
The rebase had no conflicts. Worktree `.worktrees/PORT-OT-TRIPOOL`. Ran on claude3 (Ghidra MCP account). The two
callback matches below needed only the anchored binary and `re/console/pc_funcs.csv` sizes, so no Ghidra write was made.

## Yield

| rva | before | now | TU |
|---|---|---|---|
| 0x0048ff20 | `PortTop_StandIn04G_0048ff20` | `Orig_0048ff20` via `PortTop_TriPool_Call0048ff20` | new Match/PortTop_0048ff20.cpp = re/match/s15/0048ff20.c |
| 0x0048fed0 | no hooks.csv row, no source | `Orig_0048fed0`, **new matched source**, C1 -> C4 (match) | new Match/PortTop_0048fed0.cpp = re/match/s15/0048fed0.c |
| 0x0048fef0 | (not reached) | `Orig_0048fef0` via `PortTop_TriPool_Call0048fef0` (RW-layout atomic view) | new Match/PortTop_0048fef0.cpp = re/match/s15/0048fef0.c |
| 0x0048fee0 | no hooks.csv row, no source | `Orig_0048fee0`, **new matched source**, C1 -> C4 (match) | new Match/PortTop_0048fee0.cpp = re/match/s15/0048fee0.c |
| 0x004e89a0 | no binding | `MashedRw_GeometryForAllMaterials_004e89a0` (kind `rw`) | LibRw/RwApiBind.cpp |
| 0x0048ffd0 | `PortTop_StandIn04C_0048ffd0` | `Orig_0048ffd0` via `PortTop_TriPool_Call0048ffd0` | new Match/PortTop_0048ffd0.cpp = re/match/s15/0048ffd0.c |
| 0x00490000 | `PortTop_StandIn04C_00490000` | `Orig_00490000` via `PortTop_TriPool_Call00490000` (compiled and bound, not run: 0x0040cfd0 runs in no check) | new Match/PortTop_00490000.cpp = re/match/s15/00490000.c |

File callees (step 4): 0x004b6520, 0x004b65a0, 0x004b65b0, 0x004b3b70 and 0x004b3e40 reuse POWERUPINIT-A's TUs (`Orig_<rva>`).
0x004cbd30 is `PortTop_RwStreamRead_004cbd30` inside `Orig_004b3b70`. 0x004b68f0 is A's piz binding. No new TU was needed for
them, and every callee binds by name, with no raw-address call.

Nothing on this chain stays a stand-in. The old `PortTop_StandIn04C_0048ffd0` / `_00490000` / `PortTop_StandIn04G_0048ff20`
definitions stay unreferenced, as RENDER-2 left its stand-ins.

## Step 1: the two callbacks (U-4034)

Capstone on `original/MASHED.exe.unpatched`:

```
0048fed0  8b442404  mov eax, [esp+4]      ; atomic
0048fed4  8b4c2408  mov ecx, [esp+8]      ; data (the slot)
0048fed8  894108    mov [ecx+8], eax      ; slot +0x08 = atomic
0048fedb  c3        ret                   ; returns the atomic (non-zero: RpClumpForAllAtomics walks on)
0048fee0  8b442404  mov eax, [esp+4]      ; material
0048fee4  8b4c2408  mov ecx, [esp+8]
0048fee8  89410c    mov [ecx+0xc], eax    ; slot +0x0c = material, the only writer of 0x0086a4ac.. (RENDER-2 item 2)
0048feeb  c3        ret
```

`re/console/pc_funcs.csv`: both are 12 B (0x0048fed0..0x0048fedb, 0x0048fee0..0x0048feeb). They are passed at 0x0048ff89
(push, then call 0x004e66d0 at 0x0048ff93) and at 0x0048ff01 (`push 0x48fee0`, then call 0x004e89a0 at 0x0048ff07). This
settles U-4034: `&LAB_0048fed0` is a callback, not a jump table.

| source | match.py --bytes | relocheck |
|---|---|---|
| re/match/s15/0048fed0.c (new) | 12 B whole symbol, BYTE-IDENTICAL, insn 100% (4/4) | OK checked=0 unchecked=0 |
| re/match/s15/0048fee0.c (new) | 12 B whole symbol, BYTE-IDENTICAL, insn 100% (4/4) | OK checked=0 unchecked=0 |
| re/match/s15/0048ff20.c (MATCH_PORT guard added) | 170 B BYTE-IDENTICAL | OK checked=12, unchecked=2 (GS cookie helpers, as before) |
| re/match/s15/0048ffd0.c (MATCH_PORT guard added) | 41 B BYTE-IDENTICAL | OK checked=3 |
| re/match/s15/00490000.c (MATCH_PORT guard added) | 32 B BYTE-IDENTICAL | OK checked=2 |

Flags `/O2 /GS /Ob1`, the s15 preamble. Tracker: two rows inserted (C1, mapped, file = the source). Then
`c4_reclass.py batch --tsv verify/portorigtop_20261109/TRIPOOL_C4.tsv --apply` moved them `[1/2] 0x0048fed0 match PASS
byte-identical 12 B, relocheck OK (0/0)` and `[2/2] 0x0048fee0 match PASS ...` to C4. `uncertainty_resolve.py` struck U-4034.

### Source guards (DDK text unchanged, `#ifndef MATCH_PORT`)

- 0048ff20.c: under MATCH_PORT, `s_Tri_dff_005cf4f8` is a static copy of the 8 bytes at 0x005cf4f8 (`Tri.dff\0`, read from
  the anchored image). The exe does not load the original `.rdata`, and its image pad reads 0 there.
- 0048ffd0.c, 00490000.c: under MATCH_PORT, the 0x004e8090 prototype takes `undefined4`, because the call passes the slot
  word (C++ rejects the implicit `unsigned` -> `int *`). The TU forwarder `PortTop_TriPool_Call004e8090` casts it for
  `Orig_004e8090`.

## Step 2: 0x004e89a0 bound to librw

The disassembly of 0x004e89a0 caches count = geometry +0x24 (0x004e89aa) at [esp+0x14] (0x004e89af). For i < count it calls
cb(materials[i], data), with materials = geometry +0x20 (0x004e89bd / 0x004e89c1). It stops when cb returns 0 (0x004e89cc)
and returns the geometry (0x004e89d7). The binding is the same walk over librw `Geometry::matList` in list order, returning
the pointer it was given. It is a binding, not the 76.7% body (class (b), not L-115).

## Step 3: the RW-layout atomic view (extended, not forked)

0x0048fef0 reads the geometry at atomic +0x18 (0x0048fef5) and ORs 0x40 into geometry +0x08 (0x0048fef8). The atomic
comes from slot +0x08, which is the librw `rw::Atomic` the RpClumpForAllAtomics binding hands to 0x0048fed0. librw has its
geometry at +0x14.

- `MashedRwAtomicView` is unchanged. Its `MashedRwGeometryView` gains `flags` at +0x08 (static_assert) and an exe tail
  `rw::Geometry* lib`.
- The view builder is now one helper, `NewAtomicView(atomic, clump)`, used by both the 0x004b3f90 binding and the new
  `MashedRw_AtomicView_0048fef0(atomic, clump)`.
- The 0x0048ff20 TU defines `FUN_0048fef0` as `PortTop_TriPool_Call0048fef0`. That forwarder builds the view from slot
  +0x08 and slot +0x04, then runs `Orig_0048fef0(view, slot)`.
- **One owner of the flags: the librw geometry.** The view's +0x08 is a copy. The 0x004e89a0 binding resolves a geometry
  view to its `lib` and writes the copy back before it walks the materials. 0x0048fef0 calls it right after the OR
  (0x0048ff07), so that is the first point the store can land. Layout facts are asserted: librw `Geometry::flags` is at
  +0x08, `Geometry::MODULATE` = 0x40 (rpGEOMETRYMODULATEMATERIALCOLOR), and `Material::texture` is at +0x00 (0x004e8090
  reads it).
- Measured in every knob-ON run: `RpGeometryForAllMaterials(view ... -> geometry ...): flags 0000000F -> 0000004F (view
  +0x08), 1 materials`.

## Builders and owners (the teardown lesson)

| state | builder | readers on this path |
|---|---|---|
| slots 0x0086a4a0..0x0086ae1f (30 x 0x50, BSS = the zero image pad) +0x00 / +0x04 / +0x08 / +0x0c | 0x0048ff20 (and its callbacks), at boot from 0x0040bbb0 | 0x0048ffd0 (+0x04, +0x0c), 0x00490000 (+0x0c) |
| the 30 Tri.dff clumps | 0x0048ff20 via 0x004b3e40 (RpClumpStreamRead binding) | destroyed by 0x0048ffd0 (RpClumpDestroy binding), nobody else |
| geometry flags | librw (Clump::streamRead) | 0x0048fef0 through the view, written back by the 0x004e89a0 binding |

No other exe code reads or writes 0x0086a4a0..0x0086ae1f on this path. A grep over mashedmod/src finds three other files:
- Match/MatchS15_004904d0.cpp compiles 0x004904d0 into the exe as `UtilZeroTable86a4a0`, which zeroes slot +0x00. Nothing
  in the exe calls it: its only reference is an `RH_ScopedInstall` for the .asi.
- Util/L2_00490490.cpp is .asi-only (not in exe_sources.rsp).
- Match/MatchT01G_004111c0.cpp names the light list at 0x0086ae20, past the slot range. 0x00490020 /
0x00490380 / 0x00490490 / 0x004904d0, the other users of the slots (RENDER-2 item 2), have no exe caller on this path. The
0x0048ffd0 and 0x00490000 forwarders run the matched body only when all 30 slots hold a clump and a material, because both
bodies read those words without a null check. They log either way.

## Acceptance

Machine rules:
- **Builds:** every build ran after two clear reads 20 s apart: no live `held:true` `%TEMP%\mashed_machine_priolinger_*.lock`
  and no SOLO lock. The first build waited about 14 min behind merge train 20261110_161211 (pids 19220 / 6080).
- **Launches:** every launch was `test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label
  PORT-OT-TRIPOOL[-gates] -- ...'`, test-slot outside and mashed_lock inside. Only spawned PIDs were touched.
- **Roots:** private roots `D:/MashedWT/PORT-OT-TRIPOOL_root` (boot, captures) and `_rootA` (gates), each with `original` =
  junction to `D:/MashedWT/PORT-TOP-04J_rootK/original`. `A4_OUT` = `D:/MashedWT/PORT-OT-TRIPOOL_data/arms`.

The measured build is on the rebased tree. After it, only the header comment of Match/PortTop_0048ff20.cpp changed (the
grep wording below), and rva-lint was re-run OK.

| check | result |
|---|---|
| syntax check | `cl /Zs /EHa /W3 /O2 /DMASHED_STANDALONE` on the 6 new and 3 edited Match TUs, and RwApiBind.cpp with the librw includes: no error |
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `.asi NOT deployed`. mashed_re.exe sha256 `fe284c4b5998cf709bf488805d681c4f19e40f976fb631020f7c233efeb3ccfb` |
| knob-ON boot (`knob_boot.py knob_on_1 <root> 300 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | `exited rc=0 (0x00000000) after 49.2 s`, :1030 `WinMain 0x00492370 returned 0 after 3000 frames` |
| what the boot shows | :275 `0x004b68f0("Tri.dff") binding: found, length 664` (SFX.piz entry 0x298, inside the 0x400 buffer). :276 664 bytes from the piz into a memory stream. :278 / :280 the flags write-back. :309 `rva 0x0048ff20: matched body, 30 slots at 0x0086a4a0: (clump, atomic, material) set (30, 30, 30)`. :672 `rva 0x0048ffd0: matched body (30 slots built)`, then 30 `RpClumpDestroy` of the slot clumps before the next AppDestroy callee (0x00477870) |
| 12 captures, OFF / ON / OFF / ON, pin default (`CAPFLAKE_DATA=<data> CAPFLAKE_ROOT=<root> capflake_runs.py nav1 OFF@dflt ON@dflt OFF@dflt ON@dflt`) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. walk_04 `ba34030d1b7e2fe4` in all four rows of runs.tsv. No capture moved. Both ON logs show the flags write-back, 0x0048ff20 (30, 30, 30), 0x0048ffd0 matched, and `WinMain ... returned 0 after 578 frames` |
| G-CTRL (H_DEF) | `8284be46 / 67931834 / 31fe043e` = merge_train.json `gctrl.ref` |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 = `b.ref` |
| rva-lint (`scripts/lint_rva_bodies.py`) | `34 finding(s) ... allowlisted=34 NEW=0`, `D4 burn-down=2 ORIGTOP-ONLY=32 (33 marked TU(s))`, `OK -- no NEW duplicate bodies (34 known)` |

The first lint run flagged two false call-through hits (DUP-IN-TARGET 0x0040bbb0 and 0x0048fef0), from comment lines that
start with `// 0x00...` above a definition (the POWERUPINIT-A lesson). The comments were reworded. Nothing went into the
allowlist: the six new TUs are the only exe bodies of their RVAs, and the .asi has none, so no ORIGTOP-ONLY pair arises.

Default path: every change runs only on the MASHED_ORIG_TOP path. The new TUs are reached only from the matched
0x0040bbb0 / 0x0040bd00 / 0x0040cfd0. The view helper and the 0x004e89a0 binding have no default-path caller. The 0x004b3f90
binding builds the same view as before, now with geometry +0x08 filled. The gates above are unchanged.

## BINDINGS and the port column

`verify/portorigtop_20261109/update_bindings_tripool.py` runs `update_bindings_powerupinit_a.py`, then applies the FIXED
table of `update_bindings_otrender2.py` (its main is not called), then this row's rows.
- `--check-base`: `base chain reproduces BINDINGS.tsv byte for byte: True` against the committed file of `05ea6cb1e`
  (LF-normalised).
- Rows: 0x0048ff20, 0x0048ffd0 and 0x00490000 go stand-in -> matched. New matched rows: 0x0048fed0, 0x0048fef0,
  0x0048fee0. New `rw` row: 0x004e89a0 RpGeometryForAllMaterials. The notes RENDER-2 gave 0x0048ff20 / 0x00490000 (why kept)
  are replaced by this row's.
- Result: 477 -> 481 rows, matched 218 -> 224, stand-in 73 -> 70, rw 29 -> 30.

`backfill_exe_file.py`: 6 rows given an exe_file, `old value changed: 0`. Then `backfill_port.py` on this build gives
`none=5105 body=552 called=500` and `rows changed: 6`, all this row's:
- none -> called: 0x0048ff20, 0x0048fef0, 0x0048ffd0, 0x00490000.
- new -> called: 0x0048fed0, 0x0048fee0.

0x00490000 counts as `called` by the static graph (0x0040cfd0 calls it), but no check here runs it. 0x004e89a0 stays
`none`, as a binding.

## Raw files (D:/MashedWT/PORT-OT-TRIPOOL_data, not committed)

| file | what |
|---|---|
| build.log | final build |
| knob_on_1.log / .stdout | the boot above |
| runs.tsv, capflake_nav1.stdout, nav1/r01..r04 logs | the 12 captures |
| arms.stdout, arms/H_DEF.*, arms/B_DEF_K*.step.csv | the gates (hashes above) |

## Next

- 0x00490020 / 0x00490380 / 0x00490490 / 0x004904d0 (the slot users that draw / update): they need a caller on the knob
  path first. They read slot +0x00..+0x4c and pass +0x0c to 0x004e8090, which works on the librw material now.
- 0x00490000 runs once 0x0040cfd0 runs in a check (RESULT_PORT-OT-VEHICLE.md).
