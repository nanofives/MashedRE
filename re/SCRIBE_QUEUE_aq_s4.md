# SCRIBE_QUEUE fragment — batch_aq session 4 (aq_s4)

Author-only promote-c2 pass. ALL-SKIP session: every candidate decoded as
vendored RenderWare 3.x core (library-confirm rule); zero plates authored.
Central re-classify reclasses the library_skip list to
third-party-library[renderware], kept C1.

## Queued

2026-06-03  aq_s4  bucket=re/analysis/bucket_render_004e2470_004e4900  confidence=C1->C2  rvas=NONE  library_skip=004e2470:renderware,004e2500:renderware,004e2530:renderware,004e2650:renderware,004e2930:renderware,004e2b60:renderware,004e2bf0:renderware,004e2fa0:renderware,004e2ff0:renderware,004e3160:renderware,004e33c0:renderware,004e3410:renderware,004e3470:renderware,004e4130:renderware,004e41b0:renderware,004e42a0:renderware,004e42d0:renderware,004e4300:renderware,004e4380:renderware,004e4450:renderware,004e45b0:renderware,004e47b0:renderware,004e4810:renderware,004e4860:renderware,004e48e0:renderware,004e4900:renderware  note=subsystem_observed=third-party-library[renderware] for all 26; actual pool slot Mashed_pool14 (pre-assigned pool10 POISONED — see Notes); 0 plates; 0 needs_function_create (all 26 have function objects)

## Notes for the sweep

- **Count**: 26 candidates, 26 library_skip[renderware], 0 plates. All 26 were
  C1 in hooks.csv at session start (anchored `^<rva>,` grep — no drift, none
  already >= C2). All 26 have function objects in Ghidra; every decomp
  completed clean.
- **Pool-slot deviation**: pre-assigned Mashed_pool10 threw a REAL
  LockException at the FLAT location (mashed_pool\, project_name only).
  `Mashed_pool10.lock~` is channel-locked by a live process (Get-Content
  denied: "otro proceso tiene bloqueada una parte del archivo") while
  program_list_open on this session's server = 0 — an in-JVM leaked lock
  (memory: feedback_mcp_leaked_project_lock; the leak coincides with this
  session's first program_open call that failed server-side param validation
  "program_name or program_path is required" — the failed open leaked the
  project lock). Fallback pool13 ALSO channel-locked (stale-looking .lock~
  mtime 2026-06-01 but channel-lock LIVE; my attempt left a fresh
  Mashed_pool13.lock behind at 21:47:10). **Mashed_pool14 opened cleanly
  read-only** and was used for the whole session; program_close done.
  Treat pool10 AND pool13 as poisoned for batch_ar slot assignment; stray
  on-disk locks Mashed_pool10.lock/Mashed_pool13.lock (+ pool13's was created
  by the failed attempt) may need cleanup once their holder JVMs exit.
- **Evidence per cluster** (all addresses from pool14 clone = master sync
  2026-06-03 19:12):
  - `004e2470 / 004e2500 / 004e2530 / 004e2650 / 004e2930 / 004e2b60 /
    004e2bf0` — pipeline-cluster/registry machinery: 0x24-stride entry tables
    (`piVar + 9` walks, `uVar * 0x24` indexing), 0x28-stride node arrays
    (`iVar * 0x28`), alloc-failure path raises RW error via
    `FUN_004d7ff0(0x20)` + `FUN_004d8480(&local)` (RW error-stack idiom),
    helper allocs via `FUN_004d4250`/`FUN_004d42b0`. 004e2bf0 tail-calls
    `FUN_004e3160` (sort) over 0xc-stride records. No game globals.
  - `004e2fa0 / 004e2ff0` — arena allocator: 004e2fa0 carves a 0x20-aligned
    block header (`+0x27 & 0xffffffe0`, min payload 0x20, fields
    {owner, prev=0, next=0, used=0, size}) inside a caller-supplied buffer;
    004e2ff0 frees `param_1-0x20` with neighbour coalescing on the +0x10/+0x4
    flag/size fields. 004e2ff0 is the freelist-free ao_s1 cited as a library
    callee — confirmed.
  - `004e3160` — generic in-place sort (bit-plane partition + insertion
    finish) over caller-described records (base, count, stride, key offset,
    key range); pure utility, no globals at all.
  - `004e33c0 / 004e3410 / 004e4130` — destroy/free helpers: unlink two
    doubly-linked list nodes then free via engine fn-table
    `(**(code **)(DAT_007d3ff8 + 0x11c))(*(DAT_007d7164 [+4] + DAT_007d3ff8),
    p)` — the canonical RW engine-instance freelist-free idiom.
  - `004e3470` — plugin/stream registration init: calls Ghidra-named
    `RpAtomicRegisterPlugin` (0x8,0x509) and `RpClumpRegisterPlugin`
    (0x8,0x509), plus `FUN_004c2d90`/`FUN_004c1cc0`/`FUN_004e4bd0` with
    version word 0x509 and callback triplets; stores returned plugin offsets
    into `DAT_007d716c/7170/7174/7178`; registers stream read/write/size
    callback triplets via `FUN_004e8f80`/`FUN_004f0940`/`FUN_004e5d00`/
    `FUN_004e80f0` for plugin IDs 0x50e/0x510/0x1f; finishes with
    `FUN_004d8560()`. Pure RW module attach.
  - `004e41b0 / 004e42a0 / 004e42d0 / 004e4300` — the stream callbacks
    registered by 004e3470: thin wrappers over stream read `FUN_004f0f10` /
    write `FUN_004f0dc0` / size `FUN_004f10e0` against plugin-offset fields
    (+0x54, +0x78) and `FUN_004e55d0` accessor.
  - `004e4380 / 004e48e0 / 004e4900` — setters on plugin-offset objects:
    write through `DAT_007d7170 + obj` / `obj+0x14` / `obj+0x18..0x24`
    (four floats, sets byte +3 = 1 iff f0==f1==f2 — uniform-RGB flag), each
    calling frame-dirty helper `FUN_004c0e50` when `obj+4 != 0`.
  - `004e4450 RwFrameAddChild / 004e45b0 RwFrameRemoveChild` — Ghidra-named
    (batch_t s3 renames; batch_an confirmed 45b0). Child list splice at
    parent +0x2c / child +0x20/+0x24, hierarchy count +0x24, plugin offset
    `DAT_007d7174`, error path `FUN_004d7ff0(4)` + `FUN_004d8480`.
    [UNCERTAIN] both functions call Ghidra-named `RpClumpForAllAtomics` /
    `RpClumpForAllLights` + `FUN_004e6710` on the CHILD object, and
    `DAT_007d7174` is the offset returned by `RpClumpRegisterPlugin` in
    004e3470 — the existing RwFrame* names may belong to RpWorldAddClump/
    RpWorldRemoveClump instead. Naming question only; library verdict
    unaffected. Master rename decision left to the sweep/re-classify.
  - `004e47b0` — ForAll iterator: walks pointer array at plugin offset
    `DAT_007d716c` (+0 array, +8 count) invoking callback(item, data) until
    it returns 0.
  - `004e4810 RpWorldAddLight / 004e4860 RpWorldRemoveLight` — Ghidra-named.
    List splice at world +0x34/+0x3c chosen by light subtype byte +1
    (<0x80 = +0x3c), plugin offset `DAT_007d7178`, freelist-free idiom in
    the remove path.
- **No game-state reads/writes** (0x006xxxxx-0x009xxxxx) in any of the 26.
  All globals touched are the RW engine instance `DAT_007d3ff8` and plugin
  offsets `DAT_007d7164/716c/7170/7174/7178`.
- **No U-IDs / S-IDs minted** (author-only). One bare [UNCERTAIN] recorded
  above (RwFrameAddChild/RemoveChild vs RpWorldAddClump/RemoveClump naming);
  it is a master-symbol-naming question, NON-BLOCKING — all affected RVAs are
  library_skip anyway.
- No hooks.csv / STUBS / UNCERTAINTIES / DEFERRED / shared SCRIBE_QUEUE
  writes. Master Ghidra untouched (read-only clone session, program_close
  done).
