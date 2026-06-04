# SCRIBE_QUEUE fragment — batch_ar session 5 (ar_s5)

Author-only promote-c2 pass. ALL-SKIP session: every candidate decoded as
vendored RenderWare 3.x toolkit code (library-confirm rule); zero plates
authored. Central re-classify reclasses the library_skip list to
third-party-library[renderware], kept C1.

## Queued

2026-06-03  ar_s5  bucket=re/analysis/bucket_render_00549640_005515a0  confidence=C1->C2  rvas=NONE  library_skip=00549640:renderware,00549900:renderware,00549970:renderware,005499f0:renderware,00549a50:renderware,00549a90:renderware,00549aa0:renderware,00549b20:renderware,0054a120:renderware,0054b340:renderware,0054b630:renderware,0054b6d0:renderware,0054b740:renderware,0054b8c0:renderware,0054ba20:renderware,0054bda0:renderware,0054c400:renderware,0054c4f0:renderware,0054c870:renderware,0054ceb0:renderware,0054cfa0:renderware,0054d090:renderware,0054d4d0:renderware,0054dbd0:renderware,0054e520:renderware,0054eb90:renderware,0054f410:renderware,0054f8d0:renderware,0054fd60:renderware,00550130:renderware,00550350:renderware,00550400:renderware,005504d0:renderware,00550520:renderware,00550580:renderware,00550670:renderware,00550740:renderware,00550750:renderware,005507a0:renderware,00550a20:renderware,00550bd0:renderware,00551090:renderware,00551190:renderware,005512b0:renderware,00551330:renderware,00551410:renderware,00551460:renderware,00551510:renderware,00551550:renderware,005515a0:renderware  note=subsystem_observed=third-party-library[renderware] for all 50; actual pool slot Mashed_pool14 (pre-assigned, opened read-only cleanly first try); 0 plates; 0 needs_function_create (all 50 have function objects)

## Notes for the sweep

- **Count**: 50 candidates, 50 library_skip[renderware], 0 plates. All 50 were
  C1 in hooks.csv at session start (anchored `^<rva>,` grep — no drift, none
  already >= C2). All 50 have function objects in the pool14 clone; every
  decomp completed clean (no timeouts, no needs_function_create).
- **Pool slot**: pre-assigned Mashed_pool14 opened read-only cleanly on the
  first attempt (FLAT mashed_pool\ location, program_name="MASHED.exe",
  session eb7fbb74...); program_close done at session end.
- **Evidence per cluster** (addresses from pool14 clone):
  - **RpPatch plugin toolkit, 00549640..0054f410 (27 RVAs)** — the rtpatch
    Bézier-patch module:
    - `00549640` — plugin registrar for plugin ID **0x123**: calls
      `FUN_004c2d90(0x18,0x123,...)`, Ghidra-named
      `RpAtomicRegisterPlugin(0xc,0x123,...)`, then
      `FUN_004e7da0/4e7dd0/4e7df0` (stream-callback registration, incl. the
      RW error pair `FUN_004d7ff0`/`FUN_005c9d00` as read/write stubs) and
      `FUN_004e8f50/4e8f80/4e8fb0` (geometry-plugin registration); stores
      plugin offsets in `DAT_007dc720/724/728`.
    - `00549970 / 005499f0 / 00549a50 / 00549a90` — module init/teardown:
      zeros + rebuilds pipeline globals `DAT_007dc73c..748` (00549a50 builds
      an RxPipeline tagged `&PTR_s_PatchAtomic_csl_00623d98` — the
      "PatchAtomic.csl" pipeline-name string — via
      `FUN_004d4170/4d4dd0/4d4f90/4d4380`), destroys them via `FUN_004d41e0`;
      00549970 reads device caps from `FUN_004cbb60()` (+0x1e flag bit,
      +0xc4 version word vs 0x101).
    - `00549900` — writes 6 dwords into the engine-instance plugin block at
      `DAT_007dc720 + DAT_007d3ff8` (range remap using `_DAT_005cc320`).
    - `00549aa0 / 00549b20 / 0054a120` — patch-atomic instancing: flag-bit
      remap into `FUN_004e8c70` (geometry create), per-patch mesh
      build/refresh writing through `DAT_007dc728` plugin offset, engine
      alloc `(**(DAT_007d3ff8+0x108))(n,0x30123)` (memhint **0x30123** =
      object class | plugin 0x123), tessellation driver calling the
      evaluator family below. 00549b20 reads
      `*(byte*)(DAT_00912118 + 0x2d + DAT_007d3ff8)` (engine-instance field;
      DAT_00912118 is an RW module offset slot, .bss).
    - `0054b340 / 0054b630` — vertex-normal smoothing over shared-position
      buckets (angle/cos thresholds, `FUN_004c3ac0` Vec3Magnitude callee,
      `_DAT_005cc320`/`DAT_005d757c` float consts) + its uint16-array append
      helper; alloc/free exclusively via engine fn-table +0x108/+0x10c with
      memhint 0x30123.
    - `0054b740 / 0054b8c0 / 0054b6d0` — patch-mesh stream read / stream
      write / stream-size: RW stream primitives `FUN_004cc790` (read+skip),
      `FUN_004cc770`/`FUN_004cc6e0` (write), `FUN_004cc5e0` (FindChunk),
      `FUN_004f3e90`/`FUN_004f3d40`/`FUN_004f3ce0` (material-list
      read/write/size), 0x14-byte header {flags, numCtrlPoints, numQuad,
      numTri, numTexCoordSets} with 0xc/0x4/0x8/0x44/0x2c record strides.
    - `0054ba20 / 0054bda0 / 0054c400 / 0054c4f0 / 0054c870 / 0054ceb0 /
      0054cfa0 / 0054d090 / 0054d4d0 / 0054dbd0 / 0054e520 / 0054eb90 /
      0054f410` — the pure-math tessellation kernel: forward-difference
      matrix builders (3-comp xyz / 4-comp rgba twins; consts
      `_DAT_005cc31c`, `_DAT_005cd0a0`, `_DAT_005ccac8`), forward-difference
      seeders, position/normal/color/UV grid evaluators (byte-quantized
      RGBA variant uses ROUND), bilinear quad evaluators, and the two
      triangle-strip index generators. Zero globals beyond float constants;
      no game state.
  - **RW texture-dictionary stream readers, 0054f8d0 / 0054fd60 / 00550130
    (3 RVAs)** — `0054f8d0` reads a 4-byte {count,flags} header then loops
    `FUN_0054fd60` (simple path) or the complex LOD-raster path: image read
    `FUN_004cee90`, raster create `FUN_004c77c0`, format negotiation
    `FUN_004d5340`, mip lock/unlock `FUN_004c7860`/`FUN_004c7600`, texture
    create `FUN_004c5a00`, name/mask set `FUN_004c5ae0`/`FUN_004c5b50`,
    dict-add `FUN_004c5bc0`, plugin-data read `FUN_004e1b60(&DAT_00618138,…)`
    (0x0061xxxx module list = RW idiom); engine alloc memhint **0x101b3**;
    alloc-failure raises `FUN_004d7ff0(0x80000013,…)` + `FUN_004d8480`
    (RW error-stack idiom). `00550130` is the RwTexture stream reader with
    the RW 3.5-3.7 stream **version window check 0x35000..0x37002** and
    filter/address-mode nibble fixup on texture+0x50. (Prior plates
    texture_loader_d2/d3 noted these as RW-shaped; library now confirmed.)
  - **RtFS file-system manager toolkit, 00550350..00551550 (19 RVAs)** —
    mashed.log's `RtFSManagerOpen` module. Module globals
    `DAT_007dc754` (FS list head) / `758` (capacity) / `75c` (count) /
    `760` (init guard) / `764` (last error) / `768` (error callback fn ptr) /
    `76c` (default FS); critical section `DAT_009124e0`
    (Initialize/Enter/LeaveCriticalSection — .bss RW module state, not game
    state); ALL string ops via engine fn-table `DAT_007d3ff8`
    +0xcc (strcpy) / +0xe8 (stricmp) / +0xf0 (strcmp) / +0xf4 (strlen):
    - `00550350` init, `00550400` capacity check (errfn code 5),
      `005504d0`/`00550520`/`00550750` find-FS-by-name/exists/by-mount
      walkers (errfn 6/7), `00550740` returns `&DAT_007dc754`,
      `005507a0` returns default FS.
    - `00550580 / 00550670` — file-open on FS instance (free-slot scan under
      the critical section, async-callback fields +0x28/+0x30/+0x34, vtbl
      call +0x28) and open-by-path (parses leading "mount:" prefix,
      falls back to default FS).
    - `00550a20 / 00550bd0` — fgets-equivalent over FS vtbl +0x30/+0x38
      (CRLF collapse, seek-back) and a vtbl +0x40 tail-call thunk.
    - `00551090 / 00551190 / 005512b0 / 00551330 / 00551550` — the win32
      async-I/O backend: `GetOverlappedResult` poll with error codes
      0x3e4/0x3e5 (ERROR_IO_INCOMPLETE/PENDING), FS-object factory
      (0x5c-byte object + 0x60-stride slot array, memhint **0x401be**,
      fills vtbl with `LAB_00550c30..LAB_005514e0` neighbors),
      slot-state poll, register-FS (name-collision check, mount-name copy;
      RtFSManagerRegister shape), and the user-callback completion
      dispatcher.
    - `00551410 / 00551460` — path helpers: skip-past-':' and
      prefix+suffix concat with '/'/'\\' separator normalization.
    - `00551510` — 2-case dispatcher calling fn ptrs at obj+0x20/+0x24 with
      obj+0x50: no globals at all; sits inside the RtFS band and matches its
      vtbl-callback shape (prior plate rw_engine_teardown_d2 noted all-indirect
      dispatch). [UNCERTAIN] no caller-side evidence captured this session
      tying it to a specific RtFS event pair; library verdict rests on band
      position + zero game-state + RW callback shape.
    - `005515a0` — plugin registrar for plugin ID **0x135**: `FUN_004c2d90(0,
      0x135,...)` + `FUN_004e80c0(0x28,0x135,...)` + `FUN_004e80f0(0x135,...,
      FUN_00551840,...)`; stores handle in `DAT_009124f8`. (Its stream
      callback FUN_00551840 is an ar_s6 candidate.)
- **No game-state reads/writes** in any of the 50: no 0x006xxxxx-0x009xxxxx
  game globals (the only 0x009xxxxx touches are `DAT_009124e0` — the RtFS
  module's own CRITICAL_SECTION — and `DAT_009124f8`/`DAT_00912118`, RW
  module plugin-offset slots in .bss), no entity pools, no application-page
  callers passing game data observed.
- **No U-IDs / S-IDs minted** (author-only). One bare [UNCERTAIN] recorded
  above (00551510 event-pair identity); it is NON-BLOCKING — the RVA is
  library_skip regardless.
- No hooks.csv / STUBS / UNCERTAINTIES / DEFERRED / shared SCRIBE_QUEUE
  writes. Master Ghidra untouched (read-only pool14 clone session,
  program_close done).
