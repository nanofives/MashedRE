I now have everything needed. All 31 files are read, the load-time semantics of `RH_ScopedInstall` are settled, and the hooks.csv confidence rows are counted. Here is the D0.7 triage for Save/, Gameplay/, Input/, Particle/.

---

## Confirmation: none of these are in the exe list

`build.bat` (lines 131ÔÇô332) links exactly these directories into `mashed_re.exe`: Piz, Rws, Txd, Track, D3d9Render, Ai, Vehicle, Collision, Math, Powerup, Race, Audio, Compat, Stubs, Frontend, HUD, Boot, Util, Camera, Physics, GameState, Render. **No file from Save/, Gameplay/, Input/, or Particle/ appears.** All 31 reach only the `.asi` via `asi_sources.rsp` ÔÇö except two that reach neither (see below). Confirmed genuinely absent.

## The load-time verdict hinges on `RH_ScopedInstall`

`Core/HookSystem.h:37-43` expands `RH_ScopedInstall(F, RVA)` to a file-scope object whose ctor calls `HookSystem::Register(RVA, &F, name)`. In the **exe** that symbol resolves to `Stubs/HookSystemNoOp.cpp:19` ÔÇö a **no-op that never touches the RVA** (it's passed as an integer, not dereferenced, and no patch is written). Therefore a file-scope `RH_ScopedInstall` is **not** class C. Class C would require a file-scope initializer that *dereferences* a MASHED address at load (the `static const GUID g = *(GUID*)0x005d09dc;` pattern from `build.bat:101`).

Two verification greps across all four dirs returned **zero** hits for `= *ÔÇª` file-scope dereference initializers and **zero** for `call_original`. Every RVA touch in these files is either a `reinterpret_cast` pointer **value** at file scope (no deref) or lives inside a **runtime** function body.

## Result: 0 class-C files. 2 class-A, 29 class-B.

### Classification table

Counts are hooks.csv rows whose **file column names the .cpp** (the task's criterion). "Runtime-RVA" = body reads/writes fixed MASHED addresses or jumps to game VAs at call time ÔÇö load-safe, but the export is inert/AVs in the standalone until those addresses are mapped.

| File | Class | C4 | C3 | Blocking construct | Justification |
|---|---|---|---|---|---|
| **Save/** | | | | | |
| SettingsAndIO.cpp | B | 5 | 0 | ÔÇö | file-scope only `static const ÔÇª = reinterpret_cast`(:53-76,:183-244); naked thunks + Win32 at runtime |
| GameSave.cpp | B | 4 | 0 | ÔÇö | 3 game fn-ptrs (:21-23) are values; deref only in bodies |
| SettingsConfig.cpp | B | 4 | 0 | ÔÇö | `#pragma optimize off`; naked CRT/game thunks (:52-94), all runtime |
| SettingsDialog.cpp | B | 3 | 0 | ÔÇö | constexpr IDs + inline-asm RVA calls in bodies only |
| SettingsCfg.cpp | B | 1 | 0 | ÔÇö | naked game-VA thunks (:62-103), runtime |
| FsOpen.cpp | B | 1 | 0 | ÔÇö | wraps CRT `_fsopen` only; **no MASHED address at all** |
| CareerEvents.cpp | B | 2 | 0 | ÔÇö | fn-ptrs (:167,:171) are values; globals derefed only in bodies |
| GameSaveVFS.cpp | B | 2 | 0 | ÔÇö | constexpr addrs; vtable/global derefs runtime-only |
| GameSaveBuffer.cpp | B | 2 | 0 | ÔÇö | constexpr addrs; all deref in bodies (memcpy) |
| Race_Guard.cpp | B | 1 | 0 | ÔÇö | constexpr addrs; 2 global reads in body |
| RwStream.cpp | B | 1 | 0 | ÔÇö | fn-ptr values (:96-108); vtable deref runtime |
| VfsStream.cpp | B | 1 | 1 | ÔÇö | operates on caller ctx ptr; **no fixed MASHED address** |
| ReplayThunk.cpp | B | 1 | 0 | ÔÇö | load-safe, but **cross-TU link dep on `ReplaySave` (Vehicle/Replay.cpp ÔÇö NOT in exe)** |
| ReplayGetSize.cpp | B | 1 | 0 | ÔÇö | fn-ptr value (:59); 1 raw-RVA log call in body |
| ReplayTimeFormat.cpp | B | 0 | 1 | ÔÇö | **pure compute, zero MASHED addresses** |
| ReplayGetTimeAtIdx.cpp | B | 0 | 1 | ÔÇö | 1 raw-RVA call (`TimeFormat` 0x00411350) in body |
| tests/gamesave_format_test.cpp | **A** | 0 | 0 | ÔÇö | **self-test with `main()` (:51)**; includes `GameSaveFormat.h`; never in any build list |
| **Gameplay/** | | | | | |
| ScoreMasks_ah3.cpp | B | 3 | 1 | ÔÇö | constexpr addrs; 1 raw-RVA + 1 by-name export (`VehicleSlotGetter`, in exe) |
| Thresholds_ah4.cpp | B | 2 | 0 | ÔÇö | fn-ptr value (:25); by-name exports resolve; runtime deref only |
| RangeTable_ah1.cpp | B | 1 | 5 | ÔÇö | anon-ns inline fns (:22-29) return pointer values; runtime RVA call `FUN_00426090` |
| SubsystemARecordKey.cpp | B | 0 | 1 | ÔÇö | globals read in body only; self-contained otherwise |
| SubsystemARecordFind.cpp | B | 0 | 1 | ÔÇö | globals read in body only |
| SparseGrid_ag.cpp | B | 0 | 2 | ÔÇö | anon-ns pointer-value getters (:9-11); deref in bodies |
| GameplayLeaves_ad3.cpp | B | 0 | 1 | ÔÇö | constexpr addrs; globals derefed in bodies |
| **Input/** | | | | | |
| DirectInput.cpp | B | 0 | 5 | ÔÇö | `#pragma optimize off`; naked game-VA thunks; Win32; self-linkable |
| DInput.cpp | B | 0 | 1 | ÔÇö | load-safe; **cross-TU dep on `ConfigLogDebug`+`FsopenSafe` (Save)** |
| MemsetInline_ag1.cpp | B | 0 | 1 | ÔÇö | **pure transform, zero MASHED addresses** |
| MixedC3Sweep.cpp | **A** | 0 | 0 | ÔÇö | **empty** (:22-23 "not compiled into the ASI"); documents a DEFERRED fn; not in rsp; unlocks nothing |
| **Particle/** | | | | | |
| ParticleLeaves_ad4.cpp | B | 0 | 0ÔÇá | ÔÇö | operates on caller ptr/vtable + `_InterlockedDecrement`; **no fixed MASHED address** |
| ParticleLeaves_ad5.cpp | B | 0 | 0ÔÇá | ÔÇö | **pure const-return, zero MASHED addresses** |
| ParticleBurst_ac1.cpp | B | 0 | 0ÔÇá | ÔÇö | 1 raw-RVA call (`0x004864f0`) in body |

ÔÇá **Tracker note:** all 5 Particle functions (0x0049c690, 0x0049f180, 0x0049f2b0 ÔåÆ ad4; 0x0049fa40 ÔåÆ ad5; 0x00486610 ÔåÆ ac1) **are C3 in hooks.csv** (lines 2215/2730/2734/2743/2066) but their file column points at the `re/analysis/particle_promote_*` note, **not the .cpp**, so the strict "file column names it" count is 0. Their real confidence is C3.

## Ordered add-back list (class-B, by C4 unlocked desc; ties ÔåÆ fewest external deps)

"External deps" = runtime raw-RVA touchpoints + cross-TU link deps.

1. **Save/SettingsAndIO.cpp** ÔÇö 5 C4
2. **Save/GameSave.cpp** ÔÇö 4 C4 (3 fn-ptr calls)
3. **Save/SettingsConfig.cpp** ÔÇö 4 C4 (5 CRT/game thunks)
4. **Gameplay/ScoreMasks_ah3.cpp** ÔÇö 3 C4 (globals + 1 raw RVA)
5. **Save/SettingsDialog.cpp** ÔÇö 3 C4 (Win32 + 3 inline-asm RVA calls)
6. **Save/GameSaveBuffer.cpp** ÔÇö 2 C4 (globals only, 0 game-fn calls)
7. **Gameplay/Thresholds_ah4.cpp** ÔÇö 2 C4 (by-name exports resolve + 1 raw RVA)
8. **Save/CareerEvents.cpp** ÔÇö 2 C4 (2 raw-RVA calls)
9. **Save/GameSaveVFS.cpp** ÔÇö 2 C4 (many globals + vtable + callback)
10. **Save/FsOpen.cpp** ÔÇö 1 C4 (**CRT-only, 0 MASHED addr**)
11. **Save/Race_Guard.cpp** ÔÇö 1 C4 (2 global reads)
12. **Save/VfsStream.cpp** ÔÇö 1 C4 + 1 C3 (caller-ctx only)
13. **Gameplay/RangeTable_ah1.cpp** ÔÇö 1 C4 + 5 C3 (1 raw RVA + globals)
14. **Save/ReplayGetSize.cpp** ÔÇö 1 C4 (1 raw-RVA log call)
15. **Save/RwStream.cpp** ÔÇö 1 C4 (3 fn-ptrs + vtable global)
16. **Save/SettingsCfg.cpp** ÔÇö 1 C4 (5 CRT/game thunks)
17. **Save/ReplayThunk.cpp** ÔÇö 1 C4 ÔÇö **link-blocked**: needs `ReplaySave` (Vehicle/Replay.cpp not in exe); add that first or defer
18. **Input/DirectInput.cpp** ÔÇö 5 C3 (self-linkable)
19. **Particle/ParticleLeaves_ad4.cpp** ÔÇö 3 C3ÔÇá (no fixed MASHED addr)
20. **Gameplay/SparseGrid_ag.cpp** ÔÇö 2 C3 (globals only)
21. **Input/MemsetInline_ag1.cpp** ÔÇö 1 C3 (**pure, 0 deps**)
22. **Save/ReplayTimeFormat.cpp** ÔÇö 1 C3 (**pure, 0 deps**)
23. **Particle/ParticleLeaves_ad5.cpp** ÔÇö 1 C3ÔÇá (**pure, 0 deps**)
24. **Gameplay/SubsystemARecordKey.cpp** ÔÇö 1 C3 (globals only)
25. **Gameplay/SubsystemARecordFind.cpp** ÔÇö 1 C3 (globals only)
26. **Gameplay/GameplayLeaves_ad3.cpp** ÔÇö 1 C3 (globals only)
27. **Save/ReplayGetTimeAtIdx.cpp** ÔÇö 1 C3 (1 raw RVA)
28. **Particle/ParticleBurst_ac1.cpp** ÔÇö 1 C3ÔÇá (1 raw RVA)
29. **Input/DInput.cpp** ÔÇö 1 C3 ÔÇö **needs Save/SettingsConfig.cpp + Save/FsOpen.cpp** (cross-TU); add after #3 and #10

## Counts per class

- **Class C (blocked static-init): 0**
- **Class A (unlinked by intent): 2** ÔÇö `Save/tests/gamesave_format_test.cpp` (self-test `main`), `Input/MixedC3Sweep.cpp` (empty, not compiled anywhere)
- **Class B (safe to add ÔÇö no boot-crash): 29** (16 Save + 7 Gameplay + 3 Input + 3 Particle)
- Total in scope: 31 files. C4 rows unlocked (file-column): **35** (Save 29, Gameplay 6, Input/Particle 0); plus **17 C3** file-column + **5 C3** Particle note-path.

## Two caveats the add-back must not ignore

1. **Load-safe Ôëá functional.** Because `RH_ScopedInstall` is a no-op in the exe, adding any class-B file can't crash boot and won't auto-install anything ÔÇö but the exports there deref **fixed MASHED absolute addresses** (0x004xxxxx code / 0x006xxxxxÔÇô0x008xxxxx data) at runtime. In the standalone (based 0x10000) those addresses are unmapped, so any export that runs will AV. The genuinely useful, work-out-of-the-box adds are the **zero-MASHED-address** files: `FsOpen`, `VfsStream`, `MemsetInline_ag1`, `ReplayTimeFormat`, `ParticleLeaves_ad4`, `ParticleLeaves_ad5`. Everything else compiles and links but is inert until D0.7 neutralizes its RVA tunnels.
2. **Cross-TU link blockers** (link errors, not boot crashes): `ReplayThunk.cpp` ÔåÆ `ReplaySave` (Vehicle/Replay.cpp, absent); `DInput.cpp` ÔåÆ `ConfigLogDebug`+`FsopenSafe` (Save). Order the batch so providers land first.

No files were written.
