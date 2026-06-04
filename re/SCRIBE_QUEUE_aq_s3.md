# SCRIBE_QUEUE fragment — batch_aq session 3 (aq_s3)

Author-only promote-c2 pass (render campaign 1/~3, third slice
0x004e1290-0x004e2420 — the dense small-function cluster on the
0x004e1xxx-0x004e2xxx pages). Outcome: **ALL-SKIP — zero C1->C2 plates
authored.** Every one of the 26 candidates decodes as a vendored
RenderWare 3.x CORE primitive (library-confirm rule applied per RVA from
live decomp, not by address screen). No bucket dir was created (nothing to
plate). Central re-classify reclasses all 26 OUT to
third-party-library[renderware], kept C1.

## Queued

2026-06-03  aq_s3  bucket=re/analysis/bucket_render_004e1290_004e2420  confidence=C1->C2  rvas=NONE  library_skip=004e1290:renderware,004e12a0:renderware,004e12b0:renderware,004e13f0:renderware,004e14a0:renderware,004e15f0:renderware,004e1670:renderware,004e1710:renderware,004e1780:renderware,004e17e0:renderware,004e18f0:renderware,004e1960:renderware,004e1990:renderware,004e19f0:renderware,004e1a90:renderware,004e1ac0:renderware,004e1b00:renderware,004e1b30:renderware,004e1b60:renderware,004e1c90:renderware,004e1ce0:renderware,004e1d20:renderware,004e1e70:renderware,004e2030:renderware,004e2090:renderware,004e2420:renderware  note=ALL 26 candidates are vendored RW 3.x core (RwHeap allocator family + CSL/D3D9 instance-pipeline construction + RwPluginRegistry stream-callback machinery + extension stream codec + CSL pipeline connect/validate; subsystem_observed=third-party-library[renderware] for every RVA); 0 plated; needs_function_create=NONE (all 26 had function objects and decompiled cleanly); actual pool slot=Mashed_pool9 (pre-assigned; opened read-only cleanly at the FLAT mashed_pool location, no LockException; program_close issued)

## Notes for the sweep

- **Count**: 26 RVAs read end-to-end against live decomp (read-only,
  Mashed_pool9, session 111197e6f19f4b2db6381831b1ca133f, image_base
  0x00400000). **0 plates authored — `rvas=NONE` is deliberate** (nothing for
  the sweep to plate from this session; the deliverable is the `library_skip`
  list for central re-classify).

- **POOL SLOT**: pre-assigned **Mashed_pool9** opened read-only on the FIRST
  attempt (program_open, flat `mashed_pool\` project_location, project_name
  Mashed_pool9, no LockException). `program_close` issued at end. Marker
  `.pool_slot_aq_s3`.

- **needs_function_create = NONE.** All 26 returned a function object and
  decompiled cleanly.

- **Drift check**: all 26 are their own first-field `render,C1` rows in
  hooks.csv at session start (anchored `^<rva>,` grep). 18 point at existing
  batch-v-s3 C1 plates in `re/analysis/bucket_004ddfb0/`
  ("renderware-d3d9-driver"), 6 at batch-t-s3 plates in
  `re/analysis/bucket_004e1ce0/` ("RW scenegraph CORE"), 1 (004e1b60) at
  `re/analysis/texture_loader_d3/0x004e1b60.md` ("RW plugin extension stream
  reader") — the verdict below deepens those readings, not restarts. None
  already >=C2; none drift-skipped.

- **LIBRARY-CONFIRM VERDICT — all 26 are four coherent RenderWare 3.x core
  modules: (A) the RwHeap small-block allocator, (B) CSL/D3D9
  instance-pipeline construction, (C) the RwPluginRegistry
  stream-callback/extension codec, (D) the CSL pipeline connect/validate
  pass.** Evidence lines:

  1. **Engine-instance fn-table DAT_007d3ff8 with 0x...0409 memhints
     throughout module A.** Raw alloc `**(DAT_007d3ff8+0x108)` with memhint
     0x1040409 in 004e14a0 + 004e17e0 (header alloc memhint 0x40409); raw
     free `**(DAT_007d3ff8+0x10c)` in 004e1780 (x3) + 004e14a0 + 004e17e0;
     realloc `**(DAT_007d3ff8+0x110)(ptr, n*8, 0x1030409)` in 004e13f0 —
     exactly the header's RW-idiom list. Debug message channel
     `**(DAT_007d3ff8+0xc4)` with library string
     `s_Heap_resized_from__d_to__d_bytes_006185f8` in 004e14a0.
  2. **RW error path everywhere.** FUN_004d7ff0 + FUN_004d8480 error raises
     with RW error codes 0x80000013 (heap OOM, 004e13f0/004e14a0),
     0x80000004 (stream version, 004e1b60), 0x80000016/0x80000018
     (004e1e70), and pipeline-validate codes 0x1d/0x1e/0x1f/0x20 (004e2090).
  3. **Module-object .data anchors on the 0x00618xxx page** (header idiom):
     004e1290 returns `&PTR_s_ImmInstance_csl_00618558`; 004e12a0 returns
     `&PTR_s_nodeD3D9SubmitNoLight_csl_006185b8` — named CSL pipeline-node
     descriptor getters.
  4. **RW 3.5–3.7 stream version window.** 004e1b60 reads chunk type 3
     (extension) via FUN_004cc5e0 and rejects versions outside
     0x35000..0x37002; 004e1d20 writes chunk headers via
     FUN_004cc580(..., 0x37002, 10) — the RW 3.7.0.2 anchor batch_t already
     recorded for this page.
  5. **Callees/callers land exclusively in the RW band.** Pipeline ctors
     004e18f0/004e19f0 call FUN_004d4170/004d4dd0/004d4f90/004d4380/004d41e0
     (pipeline alloc/lock/add-node/unlock/destroy) + FUN_004cd500/004cd550
     (current-pipeline setters, the Im2D/submit page); 004e2090 tail-calls
     FUN_004e2650/004e2930/004e2b60/004e2bf0 (s4's slice); 004e2030 frees
     into the RW heap global DAT_007d4710 via 004e12b0; 004e2420 allocs via
     FUN_004d4250.
  6. **Zero game-state access.** No candidate reads/writes any Mashed game
     global (no 0x0068xxxx+ entity pools, no application-page data). Data
     refs are exclusively the RW statics DAT_007d3ff8/DAT_007d4710/
     DAT_00911ae0 and the 0x00618xxx module-object/string page.

- **Per-RVA mechanical verdicts** (all read from live decomp this session):

  | RVA | ~size | mechanical role (RW core) |
  |---|---|---|
  | 004e1290 | 5b | getter: returns &PTR_s_ImmInstance_csl_00618558 (CSL node descriptor) |
  | 004e12a0 | 5b | getter: returns &PTR_s_nodeD3D9SubmitNoLight_csl_006185b8 (CSL node descriptor) |
  | 004e12b0 | 318b | RwHeap free: coalesce freed cell (header at -0x20) with prev/next free neighbours (in-use test = +0xc nonzero on header links); 4-way merge cases; updates free-slot array (count +0x14, base +0xc) or registers new span via 004e13f0 |
  | 004e13f0 | 166b | free-slot array grower: cap +0x10 += 0x20, realloc `**(DAT_007d3ff8+0x110)(arr, cap*8, 0x1030409)`; on move re-aim back-pointers (+0xc of each span header); OOM -> error 0x80000013, cap rolled back; returns next slot ptr, count +0x14 incremented |
  | 004e14a0 | 330b | RwHeap alloc: size aligned +0x1f & ~0x1f; backward best-fit scan of slot array; miss -> new block max(req+0x60, *heap) via `**(DAT_007d3ff8+0x108)(sz+0x8b, 0x1040409)`, align base 0xffffff80, chain at +8, format via 004e15f0, debug msg `Heap resized from %d to %d bytes` (006185f8) via +0xc4; carve via 004e1670; OOM error 0x80000013 |
  | 004e15f0 | 127b | block format: build 0x20-byte boundary cells at block start + end-0x20 (copy loop 8 dwords), self-link, register interior span in slot array (004e13f0), chain to previous block's tail cell |
  | 004e1670 | 145b | span carve: remainder <0x100 -> take whole span (swap-remove slot, count-1); else split: new boundary cell at payload+size, relink neighbours, slot keeps remainder; returns header+0x20 (payload); marks in-use (+0xc=0) |
  | 004e1710 | 103b | heap reset: zero slot count +0x14, re-format every chained block via 004e15f0 (head block last), zero +0x18; returns 1/0 |
  | 004e1780 | 89b | heap destroy: free slot array +0xc, walk block chain +4 freeing each, free header — all via `**(DAT_007d3ff8+0x10c)` |
  | 004e17e0 | 269b | heap create: header 0x1c alloc (memhint 0x40409); first block max(max(req,0x400) aligned 0x20, 0x80) + 0x8b alloc (memhint 0x1040409), base aligned 0xffffff80; init fields (min-block-size *h, block chain +4, flags +0x18=1->0); format via 004e15f0; full rollback on failure |
  | 004e18f0 | 102b | pipeline create (ImmInstance): FUN_004d4170 alloc, +0x2c=1, FUN_004d4dd0 lock, FUN_004d4f90(lock, 0, 004e1290()) add node, FUN_004d4380 unlock/build; *out=pipe, FUN_004cd500(pipe) set current; cleanup FUN_004d41e0 on failure |
  | 004e1960 | 37b | pipeline destroy: FUN_004cd500(0); FUN_004d41e0(*p); *p=0 |
  | 004e1990 | 87b | submit-pipeline table teardown: FUN_004e08b0(); FUN_004cd550(0, n) for n=3,5,4,1,2; FUN_004d41e0(*tbl); zero 5 table slots |
  | 004e19f0 | 151b | submit-pipeline table create: same ctor shape as 004e18f0 but node = 004e12a0() (nodeD3D9SubmitNoLight); assigns one pipeline to all 5 table slots, FUN_004cd550(pipe, 3/5/4/1/2); tail FUN_004e0920() |
  | 004e1a90 | 41b | path-absolute predicate: leading '\\' OR `[A-Za-z]:` -> 1 else 0 (RW filesystem primitive; no state) |
  | 004e1ac0 | 54b | RwPluginRegistry stream-callbacks set: walk entry list at reg+0x10 (id [2], next [0xc]) for plugin_id; set [3]/[4]/[5] (read/write/size cbs); return entry offset [0]; miss -> 0xffffffff |
  | 004e1b00 | 41b | same walk; set [6] (rights cb); miss 0xffffffff |
  | 004e1b30 | 41b | same walk; set [7]; miss 0xffffffff |
  | 004e1b60 | 299b | plugin extension stream read: FUN_004cc5e0(stream, 3, &len, &ver); ver outside 0x35000..0x37002 -> error 0x80000004; per sub-chunk FUN_004cc400 header, dispatch entry read-cb [3] by id else FUN_004cc050 skip; then invoke every entry's cb [6] on the object |
  | 004e1c90 | 69b | invoke plugin entry cb [7] for plugin_id; return registry if cb !=0 else 0 |
  | 004e1ce0 | 61b | extension size sum: per entry, size-cb [5] > 0 accumulates +0xc+size (chunk header overhead) |
  | 004e1d20 | 195b | plugin extension stream write: total via [5] loop; FUN_004cc580(stream, 3, total, 0x37002, 10) header; per entry with cb [5]>0 and write-cb [4]: sub-chunk header FUN_004cc580(stream, id, size, 0x37002, 10) + cb [4] |
  | 004e1e70 | 234b | device-info get/init: null arg -> error 0x80000016; if `*(DAT_007d3ff8+0x124)==3` copy 11 dwords from DAT_007d3ff8+DAT_00911ae0+4 to out; else out must BE that block (error 0x80000018) and gets defaults {7,2,5,6,0,1,1,2,-1,0,-1} |
  | 004e2030 | 91b | instance-table reset: `*(*(tbl+4)+0x10)=1`; per entry (base tbl+0x14, stride 0x1c, count tbl[1]w): if +0x14 nonzero -> free buffer +4 via 004e12b0(DAT_007d4710, buf) unless flag bit1; zero entry; *tbl(w)=0 |
  | 004e2090 | 901b | CSL pipeline connect/validate (reverse node walk, stride 0x28): per-node dup-output check (errors 0x1f null / 0x1e dup), cluster-table alloc 004e2420(004d4310(pipe), n), cluster propagation via 004e2500 (find) / 004e2470 (add) / 004e2530 (mark), mode-1 mismatch error 0x1d, OOM error 0x20; tail 004e2650 -> 004e2930 -> 004e2b60 -> 004e2bf0 (s4 slice) |
  | 004e2420 | 77b | cluster-table ctor: FUN_004d4250(0x14) header + FUN_004d4250(n*0x24) array; {count=0, cap=n, 0, array, owner=param_1} |

- **For central re-classify**: reclass-OUT all 26 to
  `third-party-library[renderware]`, kept C1 (library residue, not
  reimplementation targets — same treatment as ao_s1/ao_s2/ap rows). Do NOT
  rename on master (none renamed; read-only session).
- **U-IDs / S-IDs**: none minted (author-only; nothing plated, so no inline
  [UNCERTAIN] markers created; reserved range U-8900..9199 untouched).
- **render C1 still drains by 26** via the reclass-OUT (458 -> 432 from this
  session's share).
- Files in the atomic commit: this fragment only. No hooks.csv writes, no
  re-classify, no master-Ghidra mutation (read-only MCP throughout).
