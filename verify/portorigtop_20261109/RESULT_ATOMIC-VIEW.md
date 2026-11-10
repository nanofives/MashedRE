# RESULT ATOMIC-VIEW (2026-11-10): part (i) GREEN, entry 7's 0x0040cda0 on its matched body through the RW-layout atomic view; part (ii) split out

L2 worker `cmwb3bps015lwsc1bjg9b2bc5` for lead `cmw7xu1960f37p41557tefpnd`. Branch `orch/ATOMIC-VIEW`, rebased onto
`orch/integration` `c85b80fbd`. Between the `af2144c3b` base and `c85b80fbd`, `git diff -- mashedmod/` is empty, so the
build measured below is the build of the rebased tree.

## Scope

- **(i) Done.** Type-table entry 7's 0x0040cda0 runs its matched body, and so does its callback chain
  0x0040bb10 -> 0x0040bae0 -> 0x0040baa0, plus 0x004672d0.
- **(ii) Split (proposed row ATOMIC-VIEW-2).** Entry 1's 0x004548e0 (DepthCharge) needs about 8 more librw bindings,
  and none of them exists in the exe yet:
  - RpAtomicClone 0x004e69a0 on the 0x004b3f90 view
  - RpAtomicSetFrame 0x004e7e30 for a librw atomic (the exe's Orig_004e7e30 serves the RW-layout PTank records)
  - RwFrameAddChild 0x004c1040 (C2)
  - for the 0x004548a0 tree (0x004547c0, 0x00454170): 0x004c15c0, 0x004c1210, 0x004e4440 RpAtomicGetWorld and
    0x004e43b0
  That is a second subsystem-sized commit, so per the brief it stops here. Entry 1 stays the logging stand-in.

## What changed

| rva | before | now | TU |
|---|---|---|---|
| 0x0040cda0 | stand-in `PortTop_PWI_StandIn_0040cda0` | `Orig_0040cda0` via `PortTop_PWI_Call0040cda0` | new Match/PortTop_0040cda0.cpp = re/match/s02/0040cda0.c (BYTE-IDENTICAL 86 B, relocheck OK 5/1 unchecked `_RpClumpForAllAtomics`, bound by name) |
| 0x0040bb10 | (not reached) | `Orig_0040bb10` | new Match/PortTop_0040bb10.cpp = re/match/s29/0040bb10.c (C4, MATCH-40BB10) |
| 0x0040bae0 | (not reached) | `Orig_0040bae0` | new Match/PortTop_0040bae0.cpp = re/match/s02/0040bae0.c (BYTE-IDENTICAL 44 B, relocheck OK 3/0) |
| 0x0040baa0 | (not reached) | `Orig_0040baa0` | new Match/PortTop_0040baa0.cpp = re/match/s02/0040baa0.c (BYTE-IDENTICAL 63 B, relocheck OK 3/0) |
| 0x004672d0 | (not reached) | `Orig_004672d0` | new Match/PortTop_004672d0.cpp = re/match/s11/004672d0.c (BYTE-IDENTICAL 45 B, relocheck OK 4/0) |
| 0x004e66d0 at this call site | n/a | `MashedRw_ClumpForAllAtomicsAsViews_004e66d0` | LibRw/RwApiBind.cpp |
| 0x0053fb70 RpMatFXAtomicEnableEffects | n/a | `MashedRw_MatFXAtomicEnableEffects_0053fb70` | LibRw/RwApiBind.cpp |
| 0x0053fbb0 RpMatFXMaterialSetEffects | n/a | `MashedRw_MatFXMaterialSetEffects_0053fbb0` | LibRw/RwApiBind.cpp |
| 0x0053fd30 RpMatFXMaterialSetupEnvMap | n/a | `MashedRw_MatFXMaterialSetupEnvMap_0053fd30` | LibRw/RwApiBind.cpp |

All five sources were re-checked with `match.py --bytes` and `relocheck.py` this session.

re/match/s02/0040baa0.c gained `#ifdef __cplusplus` prototypes. They take the material as `int *` (the source passes a
pointer, which C++ will not convert to `uint`). The DDK text in the `#else` branch is unchanged, and the source is
still BYTE-IDENTICAL. This follows the 004039c0 precedent from POWERUPINIT-A.

### The view (extended, not forked)

- **The problem:** 0x0040bae0 reads the geometry at RW atomic +0x18. librw keeps it at +0x14.
- **The variant:** `MashedRw_ClumpForAllAtomicsAsViews_004e66d0` walks the clump exactly as the ForAllAtomics binding
  does (RW order, stop on a 0 return), but hands each atomic over as `NewAtomicView(atomic, clump)`. That is the same
  MashedRwAtomicView, and the same constructor, used by 0x004b3f90 and by TRIPOOL's 0x0048fef0. Its `geometry` field
  at +0x18 points to the view's `geomView`.
- **Reuse:** the existing 0x004e89a0 binding (TRIPOOL) already resolves a geometry view to its librw geometry, and it
  hands out librw `rw::Material` pointers.
- **No material view needed:** the only material field 0x0040baa0 reads is +0x00, the texture, and librw has it at
  +0x00 too (existing static_assert). The view itself is unchanged.
- **New binding:** `MashedRw_MatFXAtomicEnableEffects_0053fb70` resolves a view to its librw atomic.

### Builders of what the bodies read

- **DAT_006905b0** (camera record, +0x04 = frame) is built by the matched 0x00467110, through 0x004921d0 and
  RwCameraCreate:
  - POWERUPINIT-C's knob_on_1.log:181 creates record 08F558A8, which is the value at AppDestroy.
  - The 0x0045bae0 type-table loop runs later, at :364.
  - This row's logs show it set when 0x0040cda0 runs: `DAT_006905b0 = 08D5DDA8 (frame 08DD8D38)` (knob_on_1.log:416).
- **DAT_0067ecb0** (MenuAlphaGet) is 33 at that call, not 3. So 0x004672d0 takes the camera branch, and the
  0x0042f510 branch is not taken.
- **DAT_00636ad0** is 0, so the 0x0040cda0 body runs past its guard.
- **The texture** comes from 0x0040bb30("shine"), which searches FX.TXD through the TexDictionaryFindNamedTexture
  binding and returns a librw texture.
- **The forwarder** runs the matched body only when DAT_0067ecb0 != 3 and DAT_006905b0 and its +0x04 are non-zero.
  Otherwise it logs a stand-in (the 0x004768c0 precedent). Every run took the matched branch.

### Observed: 0x0053fd30 not reached

On every call 0x0040baa0 found the material's texture slot (+0x00) at 0. So it took the `0x0053fbb0(material, 0)` arm,
and 0x0053fd30 was not called (knob_on_1.log:418, 423, 428). [UNCERTAIN] What is missing is whether the original's
oilslicker.dff material also has no texture at this point. The texture is resolved by the clump stream read: RW's
0x004e7420 in the original, librw's Clump::streamRead in the exe, through the ClumpStreamRead binding. An
original-side observation of the material +0x00 after 0x0042a5d0("oilslicker.dff") would settle it. That is a
property of the clump-read binding, not of these bodies.

## rva-lint

All 5 new TUs carry `// RVA-LINT: ORIGTOP-ONLY`. No new finding: `35 known, NEW=0`, `ORIGTOP-ONLY=33 (51 marked TU(s))`.
No RVA here has another body in either target.

## Acceptance

Machine rules as in POWERUPINIT-C:
- Builds waited until no priolinger lock was held:true with a live pid and no SOLO lock was held.
- Every launch was `test-slot` around `mashed_lock.py run --label ATOMIC-VIEW[-gates]`.
- The roots were the private `D:/MashedWT/POWERUPINIT-C_root` / `_rootA`.

| check | result |
|---|---|
| syntax | `cl /Zs ... /DMASHED_STANDALONE` on the 5 new TUs, Match/PortTop_004576f0.cpp and LibRw/RwApiBind.cpp: no error |
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, mashed_re.exe sha256 `b8917db6976591eca61f97e619a49993490514cc41e0d1cb7342e741bbcfe8b7` |
| knob-ON boot, RWMOD off | `exited rc=0 (0x00000000) after 61.8 s`, `WinMain ... returned 0 after 3000 frames`. 4 x `rva 0x0040cda0(...): matched body` (:416, :421, :426, :431), each followed by `0x0053fb70 -> rw::MatFX::enableEffects`, `0x0053fbb0(material, 0)` and `0x004e66d0(clump) as views: 1 atomics` |
| knob-ON boot, RWMOD on | `exited rc=0 (0x00000000) after 61.1 s`, the same 4 matched calls (:477..:492) |
| 12 captures, RWMOD off (`capflake_runs.py nav1 OFF@dflt ON@dflt OFF@dflt ON@dflt`) | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. Both ON logs: 4 matched 0x0040cda0 calls, `WinMain returned 0 after 578 frames` |
| 12 captures, RWMOD on (`nav_rwmod`) | 4/4 class E eqE=12, the same evidence in both ON logs |
| walk_04 | `ba34030d1b7e2fe4` in all 8 rows. No capture moved. runs.tsv sha256 f560646e…, byte-identical to POWERUPINIT-A / C |
| G-CTRL (H_DEF) | `8284be46 / 67931834 / 31fe043e`, unchanged |
| B (K0 / K1 / K26) | `1d5bf4f9` x3, unchanged |
| rva-lint | OK, 35 known, 0 new |

**Default path.** Everything new runs only from the entry-7 forwarder on the MASHED_ORIG_TOP path, and the new bindings
only from these TUs. The seed is untouched. The unchanged gates are consistent with that.

## BINDINGS and the port column

`verify/portorigtop_20261109/update_bindings_atomicview.py` first runs `update_bindings_powerupinit_c.py` (the chain
tip on `c85b80fbd`), which reproduces the 494 rows. Then it writes 8 rows plus one note:
- 0x0040cda0: stand-in -> matched.
- Matched: 0x0040bb10, 0x0040bae0, 0x0040baa0, 0x004672d0.
- Bound: 0x0053fb70 / 0x0053fbb0 / 0x0053fd30.
- A note on 0x004e66d0 for the call-site variant.

Result: 494 -> 500 rows; matched 237 -> 242, stand-in 65 -> 63, bound 49 -> 52.

`backfill_exe_file.py`: 5 rows given an exe_file. Then `backfill_port.py` on this build: `none=5096 body=552
called=521`, 6 rows changed, all none -> called: 0x0040cda0, 0x0040bb10, 0x0040bae0, 0x0040baa0, 0x004672d0 and
0x0042f510. The last one is Vehicle0HandleGet, now reachable from 0x004672d0's other branch.

## Raw files (D:/MashedWT/ATOMIC-VIEW_data, not committed)

| file | sha256 (first 16) |
|---|---|
| build.log | 8315551c5d624e10 |
| mashed_re.exe | b8917db6976591ec |
| knob_on_1.log / knob_on_rwmod.log | 89c949dfae657fa3 / d2592c673614a86b |
| runs.tsv | f560646e9825f27b |
| capflake_nav1.stdout / capflake_rwmod.stdout | a354683a82ee96b2 / 0216ff646adb01bf |
| arms.stdout | b2796f4f1b69e68f |
| arms/H_DEF.step.csv / gates.csv / ptrace.log | 8284be46 / 67931834 / 31fe043e |
| arms/B_DEF_K0, K1, K26 .step.csv | 1d5bf4f9 (all three) |

## Next

- **ATOMIC-VIEW-2:** entry 1 (0x004548e0), with the librw bindings listed under Scope (ii), on the same view.
- **Optional:** an original-side read of the oilslicker material texture, to settle the [UNCERTAIN] above.
