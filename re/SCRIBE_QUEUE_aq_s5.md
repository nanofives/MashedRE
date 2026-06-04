# SCRIBE_QUEUE fragment — batch_aq session 5 (aq_s5)

Author-only promote-c2 pass (render campaign batch 1/~3, fifth slice
0x004e4b90-0x004e5ef0: the RpClump/RpAtomic/RpGeometry page proper per the
batch header — in fact this slice is the **RpLight + RwFrame-destroy + RpWorld
core module band**). Outcome: **ALL-SKIP — zero C1->C2 plates authored.**
Every one of the 26 candidates decodes as a vendored RenderWare 3.x core
primitive (library-confirm rule applied per RVA, not by address screen). No
bucket dir was kept (nothing to plate). Central re-classify reclasses all 26
OUT to third-party-library[renderware], kept C1.

## Queued

2026-06-03  aq_s5  bucket=re/analysis/bucket_render_004e4b90_004e5ef0  confidence=C1->C2  rvas=NONE  library_skip=004e4b90:renderware,004e4bd0:renderware,004e4c00:renderware,004e4c30:renderware,004e4dd0:renderware,004e4ec0:renderware,004e4f70:renderware,004e5020:renderware,004e5190:renderware,004e51e0:renderware,004e5280:renderware,004e5300:renderware,004e55d0:renderware,004e5660:renderware,004e5680:renderware,004e5700:renderware,004e5820:renderware,004e5840:renderware,004e5850:renderware,004e5bf0:renderware,004e5c30:renderware,004e5c70:renderware,004e5cd0:renderware,004e5d00:renderware,004e5d30:renderware,004e5ef0:renderware  note=ALL 26 candidates are vendored RW 3.x core (RpLight module 004e4b90..004e4ec0, RwFrame destroy pair 004e4f70/004e5020, RpWorld module 004e5190..004e5ef0; subsystem_observed=third-party-library[renderware] for every RVA); 0 plated; needs_function_create=NONE (all 26 had function objects and decompiled cleanly); actual pool slot=Mashed_pool11 (pre-assigned Mashed_pool12 threw a REAL LockException at the FLAT mashed_pool location — fell back per header protocol; program_close issued)

## Notes for the sweep

- **Count**: 26 RVAs read end-to-end against live decomp (read-only,
  Mashed_pool11, session 777e1251064a41e59bfc51bb7581b97a, image_base
  0x00400000). **0 plates authored — `rvas=NONE` is deliberate** (nothing for
  the sweep to plate from this session; the deliverable is the `library_skip`
  list for central re-classify).

- **POOL SLOT — FALLBACK USED**: pre-assigned **Mashed_pool12** threw
  `ghidra.framework.store.LockException: Unable to lock project!
  C:\...\mashed_pool\Mashed_pool12` at the FLAT location (correct path per the
  .gpr-location gotcha — this was a REAL LockException, not the nested-path
  mistake). Fell back to **Mashed_pool11** (first stale-lock-tolerant fallback
  per the batch header), which opened read-only on the first attempt
  (program_name MASHED.exe). `program_close` issued at end. Marker
  `.pool_slot_aq_s5` records the fallback. **Flag pool12 as possibly poisoned
  for future assignment** (same shape as the pool0/2/4/5/6/7/8 leaked-lock
  family; see memory feedback_mcp_leaked_project_lock).

- **needs_function_create = NONE.** All 26 returned a function object and
  decompiled cleanly.

- **Drift check**: all 26 are their own first-field `render,C1` rows in
  hooks.csv at session start (anchored `^<rva>,` grep). 24 point at batch_t s3
  C1 plates in `re/analysis/bucket_004e1ce0/` ("RW scenegraph CORE … RW
  3.7.0.2 version anchor; 26 RW renames applied in master Ghidra"); 004e5680
  points at `re/analysis/sky_weather/SESSION_MMMM.md` and 004e5d30 at
  `re/analysis/rw_engine_init_cont1/004e5d30.md`. None already >=C2; none
  drift-skipped. The batch_t s3 RW renames (RpLightCreate, RpLightDestroy,
  RpLightStreamRead, RpLightSetConeAngle, RwFrameDestroy,
  RwFrameDestroyHierarchy, RpWorldCreate, RpWorldDestroy, RpWorldInstance,
  RpWorldForAllAtomics, RpWorldForAllWorldSectors, RwEngineForAllPlugins) are
  already live on master — this session's verdict confirms the same band from
  live decomp.

- **LIBRARY-CONFIRM VERDICT — all 26 are three coherent RenderWare 3.x core
  modules sharing one idiom set: the RpLight module, the RwFrame destroy
  pair, and the RpWorld module.** Evidence lines:

  1. **Named-symbol bracketing.** 12 of the 26 already carry RW API names on
     master from batch_t s3's "RW 3.7.0.2 version anchor" pass (list above).
     004e4dd0 neighbours the ao_s2-reclassed 004e4d90 (per the batch header);
     the RwFrame-destroy pair calls FUN_004d8bd0 — itself reclassed-OUT to
     renderware by ao_s1.
  2. **Engine-instance fn-table DAT_007d3ff8 with 0x300xx/0x40507/0x10xxx
     memhints throughout.** Alloc-hint `**(DAT_007d3ff8+0x118)` with memhint
     0x30012 in 004e4dd0 (RpLightCreate) and 0x40507 in 004e5280 + 004e5850;
     raw alloc `+0x108` with 0x3000b (×2, 004e5850), 0x10006/0x10507 (×3,
     004e5300); free-hint `+0x11c` in 004e4ec0/004e5700/004e5850; raw free
     `+0x10c` saturating 004e5020 (RwFrameDestroyHierarchy, 13 call sites) and
     the destroy paths of 004e5700/004e5850.
  3. **Plugin-offset globals + module object lists.** Plugin offsets
     DAT_007d71a0 (light), DAT_007d71cc (world, used in
     004e5280/004e55d0/004e5700/004e5850), DAT_007d71fc/DAT_007d71d4 (stream
     write, 004e5ef0); module object lists &DAT_0061862c (light, in
     004e4bd0/004e4c00/004e4c30/004e4dd0/004e4ec0), &DAT_006189e4 (sector, in
     004e4f70/004e5020/004e5850), &DAT_0061864c (world, in
     004e5700/004e5850/004e5cd0/004e5d00) via the registry helpers
     FUN_004d7de0/FUN_004d8000/FUN_004d8060/FUN_004e1ac0/FUN_004e1b60.
  4. **RW stream version gates + error machinery.** 004e4c30 gates chunk
     version to 0x35000..0x37002 with a <0x30300 legacy radius→cos(cone)
     conversion path; error raise pattern FUN_004d7ff0(0x80000004)/
     (0x80000013)+FUN_004d8480 in 004e4c30/004e5850 — the standard RWERROR
     idiom seen across every confirmed-renderware page.
  5. **Plugin registrar.** 004e5d30 is the RpWorld-module plugin registrar:
     8× FUN_004c2d90(size, plugin-id 0x501/0x502/0x503/0x504/0x505/0x50a/
     0x507/0x50b, ctor, dtor) then FUN_004e3470 + FUN_004e65c0 + FUN_004f42d0
     module inits (matches its existing C1 plate
     re/analysis/rw_engine_init_cont1/004e5d30.md).
  6. **Zero game-state access.** No candidate reads/writes any Mashed game
     global. Every data ref is the engine-instance block DAT_007d3ff8, a
     plugin-offset global (DAT_007d71xx family + DAT_00911ae0, see flag
     below), a module object list (&DAT_00618xxx/&DAT_0061862c..4c), or the
     library .rdata float pool (DAT_005cc320 1.0f / DAT_005cc108 /
     DAT_005d757c).

- **One borderline citation, mechanically resolved**: 004e5190 reads
  `*(int *)(DAT_00911ae0 + 0x40 + DAT_007d3ff8)` — DAT_00911ae0 is a .bss
  global on a 0x009xxxxx page, but it is used ADDITIVELY against the engine
  instance (offset + engine + 0x40), i.e. the runtime-assigned plugin-offset
  idiom, not a game-state read. 004e5190 is also the default callback that
  RpWorldCreate (004e5850, at 0x004e59xx) and the setter 004e5820 install at
  world+0x68 — entirely intra-library wiring. [UNCERTAIN] which RW plugin owns
  offset slot DAT_00911ae0 — does not affect the library verdict.

- **Name-mismatch flag for central re-classify (report, no claim)**: master
  name `RwEngineForAllPlugins` (004e5660) mechanically dispatches
  FUN_004e47b0(*DAT_007d3ff8, FUN_004e5680, param) where 004e5680 walks ONE
  world's atomic list re-syncing frame-stamped atomics
  (RpAtomicGetWorldBoundingSphere + re-instance vfn atomic+0x48) — i.e. a
  for-all-WORLDS sync dispatch + per-world atomic-resync callback, not a
  plugin iterator. hooks.csv's name for 004e5680
  (`RpWorldForAllAtomicsCallback`) also does not match its mechanics (it is
  not the 004e5c30 callback). Both stay vendored-RW regardless; central pass
  may want to note the rename question, master is NOT touched by this session.

- **Per-RVA mechanical verdicts** (deepening the batch_t s3 C1 plates in
  `re/analysis/bucket_004e1ce0/`; all read from live decomp this session):

  | RVA | ~size | mechanical role (RW core: RpLight / RwFrame / RpWorld) |
  |---|---|---|
  | 004e4b90 | 54b | RpLightSetConeAngle: range-gate DAT_005d757c <= a <= DAT_005cd108, writes -cos(a) at light+0x28, returns light or 0 |
  | 004e4bd0 | 38b | 5-arg thunk -> FUN_004d7de0(&DAT_0061862c, …) (light module object-list helper) |
  | 004e4c00 | 33b | 4-arg thunk -> FUN_004e1ac0(&DAT_0061862c, …) (light module object-list helper) |
  | 004e4c30 | 337b | RpLightStreamRead: chunk find FUN_004cc5e0(type 1), version gate 0x35000..0x37002 else RWERROR 0x80000004, 24-byte payload via FUN_004cbd30, RpLightCreate, <0x30300 legacy radius→cone conv (FUN_004c3b30 sqrt), flags at +2/+3, registry add FUN_004e1b60(&DAT_0061862c) |
  | 004e4dd0 | 143b | RpLightCreate: alloc-hint +0x118 (plugin offset DAT_007d71a0, memhint 0x30012), type byte 3, RGBA 4×1.0f (+0x18..+0x24), err-fn FUN_004d7ff0 at +0x10, self-ring +0x2c/+0x30, frame-stamp short (DAT_007d3ff8+10)-1 at +0x3c, FUN_004d8000(&DAT_0061862c) |
  | 004e4ec0 | 49b | RpLightDestroy: FUN_004d8060(&DAT_0061862c), frame detach FUN_004c0790, free-hint +0x11c (DAT_007d71a0) |
  | 004e4f70 | 165b | RwFrameDestroy: recursive child/sibling tree walk on -1/-2 sentinels; per-frame FUN_004d8bd0(+0x34), unlink rings +0x38/+0x40 via FUN_004e3410/FUN_004e33c0, FUN_004d8060(&DAT_006189e4), FUN_004f0c10(+0x78) |
  | 004e5020 | 364b | RwFrameDestroyHierarchy: same teardown + raw-free +0x10c of child/next/root/parent/8 plugin slots and the frame itself |
  | 004e5190 | 71b | default world-sector instance callback: skip if sector poly-count short +0x84==0; pipeline pick +0x7c else engine-camera *(DAT_007d3ff8+4)+0x6c else plugin slot (DAT_00911ae0+0x40+engine); FUN_004d40d0(pipe, sector, 1) |
  | 004e51e0 | 155b | iterative BSP walk propagating 6-dword extents (world+0x50) down the sector tree (0x18-stride writes at node+0x48, stack depth 64) |
  | 004e5280 | 121b | world-list node alloc (alloc-hint +0x118, plugin offset DAT_007d71cc, memhint 0x40507) + append to engine world ring |
  | 004e5300 | 709b | RpWorldInstance: BSP leaf walk; per-sector instance build FUN_004f0ae0/FUN_004f0c50/FUN_004f3b00, dedup tables alloc +0x108 (memhints 0x10006/0x10507), result at sector+0x78 |
  | 004e55d0 | 129b | find world owning a pointer: walk engine world ring (DAT_007d71cc+4); flat-world flag 0x1000000 range test, else BSP leaf scan for sector identity |
  | 004e5660 | 31b | for-all-worlds dispatch: FUN_004e47b0(*DAT_007d3ff8, FUN_004e5680, p1), returns p1 (see name-mismatch flag) |
  | 004e5680 | 118b | per-world atomic re-sync callback: FUN_004f0900(world); walk ring world+0x38; for atomics with flag bit2 (+2) and stale frame-stamp short +0x60 vs DAT_007d3ff8+2: RpAtomicGetWorldBoundingSphere + FUN_004c1b40 visibility test + re-instance vfn atomic+0x48; restamp |
  | 004e5700 | 288b | RpWorldDestroy: unlink from engine world ring (free-hint +0x11c), per-leaf FUN_004f0c10(+0x78), FUN_004f3b60(world+0x10), RwFrameDestroy or RwFrameDestroyHierarchy on +0x1c by flag bit0 (+3), FUN_004d8060(&DAT_0061864c), raw free |
  | 004e5820 | 21b | set world+0x68 instance callback (NULL -> default FUN_004e5190) |
  | 004e5840 | 8b | get world+0x68 |
  | 004e5850 | 923b | RpWorldCreate: alloc +0x108 (DAT_0061864c size global, memhint 0x3000b) ×2 (world + root sector), RWERROR 0x80000013 on fail, extent copy from 6-dword param, self-rings +0x2c/+0x34/+0x3c, default callback FUN_004e5190 at +0x68, engine-ring node (memhint 0x40507), FUN_004d8000(&DAT_0061864c/&DAT_006189e4), RpWorldInstance; full inline destroy on instancing fail |
  | 004e5bf0 | 57b | for-all iterator over world+0x2c ring (callback(node-0x20, data), early-out on 0) |
  | 004e5c30 | 62b | RpWorldForAllAtomics: indexed array walk *(world+0x10)[0..*(world+0x14)), callback early-out |
  | 004e5c70 | 89b | RpWorldForAllWorldSectors: BSP stack walk (depth 64), callback per leaf (*node < 0), early-out |
  | 004e5cd0 | 38b | 5-arg thunk -> FUN_004d7de0(&DAT_0061864c, …) (world module object-list helper) |
  | 004e5d00 | 33b | 4-arg thunk -> FUN_004e1ac0(&DAT_0061864c, …) (world module object-list helper) |
  | 004e5d30 | 237b | RpWorld plugin registrar: 8× FUN_004c2d90(size, id 0x501..0x50b, ctor, dtor) + FUN_004e3470 + FUN_004e65c0 + FUN_004f42d0; fail if any id <0 |
  | 004e5ef0 | 59b | stream write of plugin-offset dwords: FUN_004cc790(stream, &DAT_007d71fc, 4) + (&DAT_007d71d4, 4) when size param ==8 |

- **For central re-classify**: reclass-OUT all 26 to
  `third-party-library[renderware]`, kept C1 (library residue, not
  reimplementation targets — same treatment as ao_s1/ao_s2 RW-core rows). Do
  NOT rename on master (none renamed; read-only session). Carry the two
  name-mismatch flags (004e5660/004e5680) and the DAT_00911ae0 plugin-offset
  note above.
- **U-IDs / S-IDs**: none minted (author-only; nothing plated; reserved range
  U-8900..9199 untouched).
- **render C1 drains by 26** via the reclass-OUT.
- Files in the atomic commit: this fragment only. No hooks.csv writes, no
  re-classify, no master-Ghidra mutation (read-only MCP throughout).

## Drained

drained-by=sweep-20260603-2220; 0 plates, 0 bookmarks, 0 renames (ALL-SKIP: 26 library_skip, rvas=NONE)
