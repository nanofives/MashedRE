# RESULT PORT-OT-RENDER-2 (2026-11-10): GREEN, 0x00475a00 (L-115 near-match body) and 0x00475d90 swapped under MASHED_ORIG_TOP, 0x00490000 kept, 0x0041a980 not reachable

L2 worker `cmwb0d3fw0velsc1bg4xhhcf8` for lead `cmw7xu1960f37p41557tefpnd`, L-111 step 1, the three RENDER leftovers of
RESULT_PORT-OT-RENDER.md. Branch `orch/PORT-OT-RENDER-2` from orch/integration `7bed7a482`, worktree
`.worktrees/PORT-OT-RENDER-2`.

## Yield

| item | rva | verdict |
|---|---|---|
| 1 | 0x00475a00 | **swapped** onto the 93.1% source as an L-115 near-match body (port axis only, hooks C2 unchanged) |
| 1 | 0x00475d90 | **swapped** onto its matched body (BYTE-IDENTICAL 184 B), runs at AppDestroy on the two records 0x00475a00 builds |
| 2 | 0x00490000 | kept. Writer of 0x0086a4ac found (callback 0x0048fee0 under 0x0048ff20), and that chain cannot run on matched bodies |
| 3 | 0x0041a980 | not run, and nothing on the knob path can make 0x0040cf80's test pass |

## Item 1: 0x00475a00 and 0x00475d90

### 0x00475a00 has no byte match

`re/match/s12/00475a00.c` is the best source: 93.1% (regs 100%, ops 100%, 29/29 instructions). The pilot
(`re/match/pilot/a3_00475a00.c`, verify/matchpilot_20261030/results.tsv:32) tried 9 source forms and 126 permuter compiles,
MATCH-S12 113 more. This session tried 6 new forms (char* globals with a shifted offset, a fused store, a `- -o` sum,
`(char*)ptr + o*4` with pointer globals, `((T*)o)[g/4]`): 93.1%, 93.1%, 93.1%, 89.7%, 89.7%, 68.4%. None closes it. The
pilot filed it as a class-(a) candidate, a compiler-version difference (the recorded DDK is 13.00.9176 / 9178, the retail
9466 the original used is not available). 0x00475ab0's note (re/match/s12/00475ab0.c) records another 9176-vs-original
codegen difference in the same group (x8 loop unroll where the original has x4).

Lead decision L-115 = (c): a body whose only residual is the SIB base/index order may run under MASHED_ORIG_TOP. It counts
on the port axis only, never C4.

### The proof that the two compute the same

`match.py re/match/s12/00475a00.c --bytes`: `91 B whole symbol, 2 differing unmasked bytes (first at +0x41)`.
`relocheck.py`: `OK checked=9 unchecked=0`. The two bytes, from the anchored `original/MASHED.exe.unpatched` (capstone)
and match.py `--show`:

| address | original | DDK 9176 |
|---|---|---|
| 0x00475a3f (SIB at 0x00475a41) | `89 04 17` `mov dword ptr [edi + edx], eax` | `89 04 3a` `mov dword ptr [edx + edi], eax` |
| 0x00475a47 (SIB at 0x00475a49) | `89 34 07` `mov dword ptr [edi + eax], esi` | `89 34 38` `mov dword ptr [eax + edi], esi` |

Every other byte is equal, so the register values going into each store are the same: edi = DAT_0069160c << 2
(0x00475a28 / 0x00475a31), edx = DAT_00691604 (0x00475a39), eax = the 0x00474db0 result at 0x00475a3f and DAT_00691610
(0x00475a42) at 0x00475a47, esi = param_2 (0x00475a0b). In each pair only the SIB base and index fields swap:
- Scale is 1 in both (SIB ss = 00) and there is no displacement (ModRM mod = 00, r/m = 100), so the effective address is
  base + index in both, and addition commutes: the same address.
- Neither form has EBP or ESP as the base, so both use the default DS segment: the same segment.
- The source register (eax, esi) and the operand size (dword) are the same: the same stored value.
- The instruction length is the same (3 bytes), so nothing after it moves.

### Builders (the teardown lesson)

| record | builder | on this path |
|---|---|---|
| DAT_00691600 = 2, arrays DAT_00691604 / 08 / 10 / 14 | 0x004759b0 (matched, Match/PortTop_004759b0.cpp) | `FUN_004759b0(2)` in 0x0040bbb0 (re/match/s02/0040bbb0.c:87) |
| records DAT_00691608[0..1], DAT_00691604[0..1], DAT_00691610[0..1] | 0x00475a00 | `FUN_00475a00(tex, 0x18)` / `(tex, 0x10)`, textures `"scorch"` / `"wfall"` via 0x0040bb30 (0040bbb0.c:88-91) |
| record +0x04 (read by 0x004768c0 with no null check) | 0x004770c0 (matched, Match/PortTop_004770c0.cpp): RpPTankAtomicCreate binding, the RW-layout MashedRwPTankAtomic record | called by 0x00475a00 with flags 0x80c |

0x00475d90 frees exactly these: 0x004768c0 on each record, RwGlobals +0x10c on DAT_00691604[i] (the 0x00474db0(cap, 0x50)
block), then the four arrays. Shared state: no other exe TU reads or writes 0x00691600..0x00691617 (grep over
mashedmod/src). No other body of either RVA in the source tree, so no ORIGTOP-ONLY marker and no allowlist line.

### What changed

- `Match/PortTop_00475a00.cpp` (new): includes re/match/s12/00475a00.c, callees through forwarders with the source's
  prototypes: 0x004770c0 -> `Orig_004770c0`, 0x00474db0 -> `Orig_00474db0`. The header carries the L-115 proof above.
- `Match/PortTop_00475d90.cpp` (new): includes re/match/s12/00475d90.c, 0x004768c0 -> `Orig_004768c0`.
- `re/match/s12/00475d90.c`: `typedef int code();` is now `#ifdef __cplusplus typedef int code(int); #else ... #endif`,
  the form re/match/s12/004768c0.c uses (C++ rejects the one-argument call through `int()`). CRLF kept. `match.py --bytes`
  after: `184 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL`, relocheck `OK checked=18`.
- `Match/PortTop_0040bbb0.cpp`: `#undef` / `#define FUN_00475a00 Orig_00475a00` in the rebind block below the stand-in list
  (the SI definition stays, unreferenced). `Match/PortTop_0040bd00.cpp`: `#define FUN_00475d90 Orig_00475d90`.
- `Match/PortTop_004759b0.cpp`: the comment that said 0x00475d90 stays a stand-in now says it runs.
- `mashedmod/exe_sources.rsp`: +2 TUs after `Match\PortTop_004d7ff0.cpp`.
- BINDINGS.tsv (below): 0x00475a00 kind **`near-match`** (new kind, L-115, not `matched`), 0x00475d90 `matched`.

## Item 2: 0x00490000 kept

0x00490000 (re/match/s15/00490000.c) calls 0x004e8090(*p, 0) for p = 0x0086a4ac, step 0x50, to 0x0086ae0c: the +0x0c word of
the 30 0x50-byte slots at 0x0086a4a0 (BSS: .data raw size 0x4d000, the slots are at offset 0x2804a0). 0x004e8090
(`Orig_004e8090`, RpMaterialSetTexture) reads `*param_1` first.

**Every reference to 0x0086a400..0x0086ae1f in .text** (capstone over every dword in that range): the slot table is used by
0x0048ff20, 0x0048ffd0, 0x00490000, 0x00490020, 0x00490380, 0x00490490 and 0x004904d0. Of those, the stores to +0x0c come
from:

```
0048ff20  (called by 0x0040bbb0, boot)          per slot i < 30 (0x0048ff6b..0x0048ffae):
  0048ff7e  call 0x4b3e40           slot+4 = clump read from the "Tri.dff" buffer
  0048ff93  call 0x4e66d0           RpClumpForAllAtomics(clump, 0x0048fed0, slot)
  0048ff9d  call 0x48fef0           0x0048fef0(slot+8, slot)
0048fed0  mov [ecx+8], eax  (0x0048fed8)          callback: slot+8 = atomic
0048fef0  mov eax,[esi+0x18] / or [eax+8],0x40 / call 0x4e89a0(geometry, 0x0048fee0, slot)
0048fee0  mov [ecx+0xc], eax  (0x0048fee8)        callback: slot+0xc = material   <- the only writer of 0x0086a4ac
```

The other users only read +0x0c, each disassembled from its entry: 0x00490020 writes +0x00 (0x00490300) and +0x18..+0x4c
(0x004901c5..0x00490294) and reads +0x0c at 0x00490337 / 0x00490343 / 0x0049035c / 0x00490368, passing it to 0x004e8090 and
storing through it (`mov [ecx+4], edx`). 0x00490380 writes +0x00, +0x14, +0x18, +0x34, +0x3c, +0x4c. 0x00490490 reads +0x00 /
+0x04. 0x004904d0 writes only +0x00 (0x004904e0). So 0x0086a4ac is non-zero only after 0x0048ff20's per-slot build.

**Why that chain does not run here, each reason sufficient:**
- 0x0048fed0 and 0x0048fee0 have no source under re/match and no hooks.csv row (the same gap RESULT_PORT-OT-OTHER-2.md
  gave for 0x0048ffd0).
- 0x004e89a0 (RpGeometryForAllMaterials) is 76.7% (re/match/rwl02/004e89a0.c: the count cached in a dead stack slot). That
  is not the L-115 class, and BINDINGS has no rw binding for it.
- 0x0048fef0 (matched 100%) reads RW 3.x `atomic + 0x18` as the geometry. The knob path's RpClumpForAllAtomics binding
  (`MashedRw_ClumpForAllAtomics_004e66d0`, LibRw/RwApiBind.cpp:865) hands callbacks the librw `rw::Atomic`, whose geometry
  is at +0x14 (RwApiBind.cpp:791-794). An RW-layout atomic view like `MashedRwAtomicView` would be needed.
- 0x0048ff20's file callees 0x004b65a0 (-> 0x004b68e0), 0x004b65b0 (-> 0x004b68f0) and 0x004b3b70 (-> 0x004cbd30) have no
  BINDINGS row. 0x004b3b70 has only the hand body Save/FileWrappers.cpp:204. 0x004b3e40 is matched (100%) but not
  compiled in the exe.

On this path 0x0048ff20 is a stand-in (knob_on_1.log:219 `rva 0x0048ff20: stand-in, not run`), so `*0x0086a4ac` is 0 and
the first 0x004e8090 read would be `*(0)`. Its caller 0x0040cfd0 also does not run in any check here (RESULT_PORT-OT-VEHICLE.md).
The teardown 0x0048ffd0 (RpMaterialSetTexture(slot+0xc, 0), RpClumpDestroy(slot+4), 0x0048ffd0..0x0048fff8) has the same
dependency and stays too (knob_on_1.log:482).

## Item 3: 0x0041a980 cannot run on the knob path

0x0040cf80 (re/match/s02/0040cf80.c) calls 0x0041a980 only when 0x0041f320(i) != 0 for some i < 4. 0x0041f320 returns
`[0x0063d830 + i*4]` (0x0041f324). The table is BSS (.data offset 0x53830 > raw size 0x4d000), so it starts at 0.

Every reference to 0x0063d830..0x0063d83f in .text:

| site | function | store |
|---|---|---|
| 0x0041f324 `mov eax, [eax*4 + 0x63d830]` | 0x0041f320 | read |
| 0x0041eca6 `mov [ebp*4 + 0x63d830], esi` | 0x0041ec00 | **0**: esi is zeroed at 0x0041ec70 `xor esi, esi` and not written again before 0x0041eca6 |
| 0x00420836 `mov dword ptr [ebx*4 + 0x63d830], 1` | 0x00420420 | **1**, the only non-zero store |

0x00420420 is 79.8% (re/match/s05/00420420.c, hooks C2: no matched body). Its only caller is 0x0040d110 (matched), whose
only caller chain is 0x0040d270 <- 0x0040d440 <- 0x004929d0 (ghidra_funcfacts refs). On the knob path 0x004929d0 is bound to
the hand `GameModeMachine_Tick` (BINDINGS `port`, Match/PortTop_00492370.cpp:222-225) and 0x0040d440 is `missing`.
0x0041ec00 is called from 0x0040cf80 itself (after the test) and 0x0041ffb0 (a stand-in in AppDestroy). No exe TU writes
0x0063d830 (grep: only the reader Match/MatchS04_0041f320.cpp). So no route on the knob path makes the test pass, and the
knob-ON boot has no 0x0041a980 or 0x00422140 line. Making it run needs a race finish through the matched game flow and a
matched 0x00420420, which is outside this row.

## BINDINGS chain

`verify/portorigtop_20261109/update_bindings_otrender2.py` runs `update_bindings_othud2.py` once, then applies the FIXED
tables of `update_bindings_otother2.py` and `update_bindings_otpickuppool.py` in that order (their `main` is not called,
since each re-runs othud2), then its own rows.
- `--check-base` first: `base chain reproduces BINDINGS.tsv byte for byte: True` against the committed blob of `7bed7a482`
  (the checkout is CRLF under core.autocrlf, the scripts write LF, so it compares LF-normalised).
- Then 6 rows: 0x00475a00 stand-in -> **near-match** (`Orig_00475a00`, note cites L-115 and both SIB bytes), 0x00475d90
  stand-in -> matched, and new reasons in the notes of 0x004759b0 (teardown now runs), 0x00490000 and 0x0048ff20 (kept,
  item 2) and 0x0041a980 (not run, item 3). Result: 452 rows, matched 202 -> 203, stand-in 67 -> 65, near-match 0 -> 1.
- Note for whoever merges second: PORT-OT-POWERUPINIT is fixing the same chain into othud2 -> otother2 -> otpickuppool. On
  rebase, chain this script's FIXED after that one.
- `near-match` is a new kind. `re/tools/port_closed_sets.py` and `origtop_size.py` treat a kind outside their known sets as
  not satisfied (open / expanded), which is the right reading for a non-matched body.

## Acceptance

Machine rules: the build started after two clear reads 20 s apart (no `%TEMP%\mashed_machine_priolinger_*.lock`,
`mashed_lock.py status` = `free 'mashed_machine'`). Every launch ran as `test-slot bash -c 'py -3.12
<orch-wt>/re/orchestrator/mashed_lock.py run --label PORT-OT-RENDER-2[-gates] -- ...'` (test-slot outside, mashed_lock
inside). Only spawned PIDs were touched. Private roots `D:/MashedWT/PORT-OT-RENDER-2_root` (boot, captures) and `_rootA`
(gates), each with `original` = junction to `D:/MashedWT/PORT-TOP-04J_rootK/original`. `A4_OUT` = `D:/MashedWT/PORT-OT-RENDER-2_data/arms`, created first.

| check | result |
|---|---|
| syntax check | `cl /Zs /EHa /W3 /O2 /DMASHED_STANDALONE` on the 2 new and 3 edited TUs: no error (one existing C4996 fopen warning in PortTop_0040bbb0.cpp) |
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `[skip] MASHED_NO_DEPLOY set: mashed_re_dev.asi NOT deployed to original\`. mashed_re.exe sha256 `d627f84efbfc281dcf2d7e27cc7e0129047c7db290a6df2afc663011a55a7170` |
| knob-ON boot (`knob_boot.py knob_on_1 <root> 300 MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`) | `exited rc=0 (0x00000000) after 61.2 s`, :810 `WinMain 0x00492370 returned 0 after 3000 frames`. No `0x00475a00` / `0x00475d90` stand-in line. :226-227 `RpPTankAtomicCreate(24, 0x1008000A ...) -> PTank record 0F71AB28` and `(16, 0x1008000A ...) -> 0F71BAD8` (the two 0x00475a00 records, flags 0x80c). :512-520 0x00475d90 at AppDestroy: `RpAtomicDestroy(PTank record 0F71AB28)` and `(0F71BAD8)`, each with its texture release and three RwGlobals +0x10c frees, then :521-524 four frees (the arrays). 27 creates, 27 destroys |
| 12 captures, OFF / ON / OFF / ON, pin default (`CAPFLAKE_DATA=<data> CAPFLAKE_ROOT=<root> capflake_runs.py nav1 OFF@dflt ON@dflt OFF@dflt ON@dflt`) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. The 12 hashes are equal in all four runs (runs.tsv, 1 distinct hash list). walk_04_colourselect `ba34030d1b7e2fe4` in all four. No other capture moved. Both ON logs: the two cap-24 / cap-16 creates, no stand-in line of either RVA, 27 creates and 27 destroys |
| G-CTRL (H_DEF) | `8284be46/67931834/31fe043e` = merge_train.json `gctrl.ref` |
| B (K0 / K1 / K26) | `1d5bf4f9` x3 = `b.ref` |
| rva-lint (`scripts/lint_rva_bodies.py`) | `29 finding(s) ... allowlisted=29 NEW=0`, `D4 burn-down=2  ORIGTOP-ONLY=27 (class deleted at the ORIG_TOP flip; 28 marked TU(s))`, `OK -- no NEW duplicate bodies (29 known)` |

### Port column

`scripts/backfill_exe_file.py` (`rows whose old value changed: 0`), then `scripts/backfill_port.py` on this build:
`none=5125 body=552 called=478`, `rows changed: 2`, both this row's: 0x00475a00 (C2) and 0x00475d90 (C4) none -> called,
exe_file `Match/PortTop_00475a00.cpp` / `Match/PortTop_00475d90.cpp`. The C column is unchanged (0x00475a00 stays C2). No
drift row.

## Kept, with the reason

| rva | reason |
|---|---|
| 0x00490000 | item 2: its slot +0x0c is written only by the callback 0x0048fee0 under 0x0048ff20, and that chain needs two unsourced callbacks, a 76.7% unbound 0x004e89a0, an RW-layout atomic view for 0x0048fef0 and four file-callee bindings |
| 0x0048ff20 | matched, but its per-slot build is the chain above. Swapping it alone stores librw atomics that 0x0048fef0 reads at RW 3.x offsets |
| 0x0048ffd0 | same dependency as 0x00490000 (reads slot +0x0c and +0x04) |
| 0x0041a980 | already on its matched body (PORT-OT-RENDER). Not reachable: item 3 |

## Raw files (D:/MashedWT/PORT-OT-RENDER-2_data, not committed)

| file | sha256 |
|---|---|
| build.log | e27b76bb15243a322ad2f8bef43b546fd6a50026b62004d420151e799cfe48ae |
| mashed_re.exe (this build) | d627f84efbfc281dcf2d7e27cc7e0129047c7db290a6df2afc663011a55a7170 |
| knob_on_1.log | 1967685512c132bf188a1aa14af330440f61f10a8c6c6bf83776dcfb41ade8a4 |
| knob_on_1.stdout | 605a5ff0d2a89ce90ca97e7c850748aac3261b65d7f59810d8571ee0ee26cfcb |
| runs.tsv | 57330649dd80ec34fbcbb541d4200d97cfe6376ecad59ca71d2e3a696bc0f656 |
| capflake.stdout | b0ae67f43e977458917c1a809bc38af21410c0b8a6585cca5d31bb831c9e6a96 |
| nav1/r01_OFF_pindflt.log | bac8d0a22fd30fce5dbd18c40400e30ccd844243d0d67a7caab38b4cde33f062 |
| nav1/r02_ON_pindflt.log | c9d1e0a77e45b1ced94564094f22cce3baf7d4acd23c69314921ff8721a07aa0 |
| nav1/r03_OFF_pindflt.log | 1d8ac9b2900dcabf74d7fa1eeb65c02a2e4b9790df9c46620951f1acfa6924c9 |
| nav1/r04_ON_pindflt.log | d9fc724bf5aa2be93e51be80c373b169024db152998800eafae9218ddded69ae |
| arms.stdout | 9973f9d62b9dff35bae3d63ee0c21259ea23680a4892defcc22563f93f421e95 |
| arms/H_DEF.step.csv / gates.csv / ptrace.log | 8284be46.. / 67931834.. / 31fe043e.. |
| arms/B_DEF_K0, K1, K26 .step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |

## Next

- 0x00490000 / 0x0048ff20 / 0x0048ffd0 as one row (proposed `PORT-OT-TRIPOOL`): match the two 3-instruction callbacks
  0x0048fed0 / 0x0048fee0 (new hooks.csv rows through re-classify), bind 0x004e89a0 to librw (`rw::Geometry` material
  list) as RpClumpForAllAtomics is bound, give 0x0048fef0 an RW-layout atomic view, bind the file callees 0x004b65a0 /
  0x004b65b0 / 0x004b3b70 / 0x004cbd30, compile 0x004b3e40, then swap the three together.
- 0x0041a980 needs a race finish through the matched game flow (0x004929d0 matched, MASHED_PORTSTATE_GAMEFLOW_FU) and a
  matched 0x00420420 (79.8% today). That is a game-flow row, not RENDER.
