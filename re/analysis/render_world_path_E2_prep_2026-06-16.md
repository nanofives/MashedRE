# WS-E / E2-prep — RxPipeline submit path + BSP→RpWorld loader (RVA-cited)

Session 2026-06-16, **Mashed_pool12** (read-only; pre-assigned pool6 was poisoned
by a stuck channel lock — fell back per ghidra-pool skill). Anchor verified this
session: `original/MASHED.exe.unpatched` SHA-256
`bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e` == CLAUDE.md.
Every RVA below was decompiled/disassembled this session via Ghidra MCP on
`Mashed_pool12`. NO-GUESSING: mechanical facts are cited to the address; names of
RW APIs are marked with their basis and `[UNCERTAIN]` where the symbol is stripped.

This note pays down the three OPEN items the E1 map
(`render_world_path_E1_2026-06-16.md` §9) flagged as prerequisites for the B-FULL
verbatim world-render port the user ratified (2026-06-16):

1. the **RxPipeline node graph / submit path** below `FUN_004d40d0`,
2. the **BSP→RpWorld world-stream loader**,
3. the **exact culling predicates** `FUN_004f0900` and `FUN_004c1b40`.

Decision context: the user chose "RE-prereq first (decompile)" this session — no
port code is written; this map is the source-of-truth every verbatim line below
the +0x48 atomic render callback needs.

> **UPDATE (same session, residuals):** items 1 (world-stream loader) and 2
> (RxPipeline node graph) are now **CLOSED** — see **§9** (`RpWorldStreamRead`
> `FUN_004e99b0` + sector readers) and **§10** (the node graph = single
> all-in-one nodes `FUN_004eb3d0` world / `FUN_004ea6d0` atomic; pipeline-create
> `FUN_004f0810`). Remaining open: §7 items 3–5 (`FUN_004f0900` target, world-B
> semantics, verification) + the node-body internals (port-time, non-blocking).

---

## 1. Culling predicates (E1 §4 leaves)

### 1.1 `FUN_004c1b40` — camera frustum-sphere test — FULLY DECOMPILED
`FUN_004c1b40(int param_1 /*camera*/, float *param_2 /*sphere xyz+r*/)`
(0x004c1b40..0x004c1ba7). Mechanically:
```
result = 2;                              // rwSPHEREINSIDE (all-inside default)
plane  = (float*)(camera + 0x94);        // frustum plane array
for (i = 6; i != 0; --i) {               // 6 frustum planes
    d = plane[2]*sphere[2] + plane[0]*sphere[0] + plane[1]*sphere[1] - plane[3];
    if (sphere[3]  <  d) return 0;        // fully outside this plane → rwSPHEREOUTSIDE
    if (-sphere[3] <  d) result = 1;      // straddles → rwSPHEREBOUNDARY
    plane += 5;                           // stride 5 floats = 20 bytes per plane
}
return result;                           // 2=inside, 1=boundary, 0=outside
```
- **camera+0x94 = frustum-plane array, 6 entries × 0x14 bytes.** Each entry =
  `{ RwV3d normal (3 floats @ +0..+8); RwReal distance (+0xc); +0x10 skipped }`.
  The +0x10 dword (`plane[4]`) is the RW `RwFrustumPlane.closestX/Y/Z` byte triple
  — read by the full RW test but NOT by this simple sphere variant.
- `param_2` (sphere) = 4 floats: center.xyz (+0..+8), radius (+0xc).
- This is **RwCameraFrustumTestSphere** (mechanics certain; the RW name is the
  standard one for this exact 6-plane sphere classification — `[UNCERTAIN: exact
  symbol]` since the binary is stripped, but the {2,1,0} return is rwSPHERE*).
- Producer of the sphere arg in `FUN_004e5680`: `RpAtomicGetWorldBoundingSphere`
  (Ghidra already recognises this name).

### 1.2 `FUN_004f0900` — per-sector visibility test — MECHANISM, target runtime-bound
Disassembly (0x004f0900..0x004f090a, 3 instructions):
```
MOV EAX,[0x007d3ff8]        ; EAX = RwGlobals ptr (G)
MOV ECX,dword ptr [EAX+0x4] ; ECX = *(G+4)  = the world/sector-TYPE object  (call it O)
JMP dword ptr [ECX+0x68]    ; tail-jump to the fn-ptr at O+0x68, sector arg forwarded
```
- It is a thunk: `FUN_004e5680` calls `FUN_004f0900(sector)`; the sector arg is
  forwarded on the stack to the function pointer at **`(*(RwGlobals+4)) + 0x68`**.
- **`*(RwGlobals+4)` = the world/sector-type object `O`** — the SAME object
  `FUN_004e5190` reads at `(*(RwGlobals+4))+0x6c` for the world default pipeline
  (§3.2). So `O = { +0x68: sectorVisibilityTestFn, +0x6c: worldDefaultPipeline }`.
- Concrete target of `O+0x68` is a runtime-installed fn-ptr (set during engine/
  driver init); **`[UNCERTAIN — needs the writer of RwGlobals+4 / O+0x68]`**. To
  resolve: find where `*(RwGlobals+4)` is assigned and trace `O+0x68`. This is the
  only predicate not statically bottomed-out this session.

---

## 2. The RxPipeline execute dispatch — `FUN_004d40d0`

`FUN_004d40d0(pipeline, ioBlock, flush)` @ 0x004d40d0..0x004d416e (E1 §8 / prior
`render_c0_promote_c/0x004d40d0.md`). The node-graph entry is the indirect call:
```
(**(code**)( *(int**)(pipeline + 8) + 4 ))( *(int**)(pipeline + 8), &DAT_00911ad0 );
```
- `*(int**)(pipeline+8)` = **P**, the pipeline's first executable node block.
- `*P` = P's vtable; `*P + 4` = **vtable[1]** = the node's execute entry.
- Called `node->vtable[1](node, &io)` where `&io = &DAT_00911ad0` — the staged I/O
  block (resource at `_DAT_00911ac0`, object at `_DAT_00911ad0`, heap/cluster mgr
  at `_DAT_00911ad4 = DAT_007d4710`). Set/cleared around the call (latched globals,
  classic RW-SDK arg passing — the node reads fixed addresses, not call args).
- Indirect → Ghidra lists no callee for it; the node functions are NOT statically
  reachable from here. They are determined by **which pipeline** is passed.
  Resolving the node-function identities = decompile the pipeline-CREATE (§5 next).
- Two helper callees (per-frame heap mgmt, NOT geometry): `FUN_004e1710`
  (pre-flush, guarded by `flush != 0 && *(DAT_007d4710+0x18) != 0`), `FUN_004e2030`
  (error-recovery on `ioBlock+0x14` when status `>1`).

### Render-heap / cluster reset — `FUN_004d4dd0`
`FUN_004d4dd0(int *param_1)` @ 0x004d4dd0..0x004d4ef2 — the per-execute resource/
cluster manager (reached via the heap path, not a node). Allocates
`*(RwGlobals + DAT_00911ae0 + 0x38) * 0xb4`-byte (180) staging via the RW resource
allocator `(*(RwGlobals + 0x108))(size, 0x1030409)`, then walks `param_1[1]`
cluster entries (stride **0x28** = 40 B): calls each cluster's `+0x14` callback,
decrements its refcount at `+0x3c`, and calls the `+0xc` destructor when it hits 0.
This is RxHeap/cluster bookkeeping — context for the submit, not the node graph.

---

## 3. The two engine default pipelines (atomic vs world)

Both the atomic path (`FUN_004e5f90`, E1 §8) and the world-sector path
(`FUN_004e5190`) resolve a pipeline then call `FUN_004d40d0(pipe, obj, 1)`.

### 3.1 Atomic render callback — `FUN_004e5f90` (atomic+0x48 default)
```
pipe = *(atomic + 0x6c);                          // atomic's own pipeline override
if (pipe == 0) pipe = *(RwGlobals + DAT_00911ae0 + 0x3c);   // engine default ATOMIC pipeline
FUN_004d40d0(pipe, atomic, 1);
```

### 3.2 World-sector render callback — `FUN_004e5190` (world+0x68 default)
`FUN_004e5190(uint world)` @ 0x004e5190..0x004e51d7:
```
if (*(short*)(world + 0x84) == 0) return world;   // empty-world guard
pipe = *(int*)(world + 0x7c);                      // world's own pipeline override
if (pipe == 0) {
    pipe = *(int*)( *(int*)(RwGlobals+4) + 0x6c );  // world-TYPE default (object O, §1.2)
    if (pipe == 0)
        pipe = *(int*)(RwGlobals + DAT_00911ae0 + 0x40);  // engine default WORLD pipeline
}
FUN_004d40d0(pipe, world, 1);
```
**Key structural fact: world geometry and atomic geometry use SEPARATE default
pipelines** — `plugin+0x3c` (atomic) vs `plugin+0x40` (world-sector) — both
executed by the same `FUN_004d40d0`, both bottoming into the 3D submit leaf (§4.2).

### 3.3 The D3D9 driver plugin block — `RwGlobals + DAT_00911ae0`
`DAT_00911ae0` = the **D3D9 driver's RwGlobals plugin offset**. Set once at plugin
registration: the ctor at **0x004d8530** does `MOV [0x00911ae0], EAX` (EAX = the
offset arg from `RwEngineRegisterPlugin`) then `CALL 0x004d3db0`. 41 readers across
the RxD3D9 band 0x004d3xxx–0x004f4xxx; the single writer is 0x004d8534.

Plugin-data constructor **`FUN_004d3db0`** @ 0x004d3db0..0x004d3e81 (idempotent via
`DAT_007d470c`):
- `DAT_007d4710 = FUN_004e17e0(DAT_00618410)` — creates the render heap / cluster
  mgr (`DAT_007d4710` = the `_DAT_00911ad4` mgr in §2).
- `*(plugin+0x00) = thunk_FUN_004cc820(0x34, DAT_00618418, 4, DAT_0061841c, &DAT_007d46e8, 0x40409)`
  — a 0x34-byte object at plugin+0.
- `*(plugin+0x38) = DAT_00618414`; `FUN_004e1e70(plugin+0x04)`;
  `*(plugin+0x34) = 0`; `*(plugin+0x30) = 0`.
- **Does NOT write +0x3c or +0x40** → the default ATOMIC (+0x3c) and WORLD (+0x40)
  pipelines are built later, at device OPEN (the §5 next target).

Plugin-block field map (offsets relative to `RwGlobals + DAT_00911ae0`):
| off | written by | meaning (mechanical) |
|---|---|---|
| +0x00 | FUN_004d3db0 | 0x34-byte object (raster/state cache root) |
| +0x04 | FUN_004d3db0 (FUN_004e1e70 init) | sub-object init in-place |
| +0x30,+0x34 | FUN_004d3db0 = 0 | counters/null |
| +0x38 | FUN_004d3db0 = DAT_00618414; read in FUN_004d4dd0 ×0xb4 | per-cluster stride count |
| **+0x3c** | (device OPEN, §5) | **default ATOMIC pipeline** (read FUN_004e5f90) |
| **+0x40** | (device OPEN, §5) | **default WORLD-sector pipeline** (read FUN_004e5190) |

---

## 4. The D3D9 submit leaves (`IDirect3DDevice9::DrawIndexedPrimitive`)

Both `DrawIndexedPrimitive` call sites (device vtbl **+0x148**) in the driver band
0x004d0000–0x004f5000, found by instruction search (`CALL [ECX+0x148]`):

### 4.1 Im2D indexed submit — call @ **0x004dc10e** (HUD/menu E4 path)
Inside the `fpIm2DRenderIndexedPrimitive` region (device-table entry @ 0x004dbc90,
E1 §7). Submit context (0x004dc0c0..0x004dc138):
- `REP MOVSB` copies verts into a managed VB; `EAX = *0x007d4110` (IDirect3DDevice9*),
  `ECX = *EAX` (vtable).
- Primitive type from a **RW→D3D primitive-type table @ 0x005d8d8c** indexed by the
  prim-mode arg (`MOV EDX,[EDX*4 + 0x5d8d8c]`).
- Vertex/index state globals: `0x007d709c`, `0x007d70a4`, `0x007d70a8`.
- This is the 2D/Im2D leaf — **E4 (HUD/menu) submit**, out of E1/E2 world scope but
  cited for completeness of the device table.

### 4.2 3D / atomic submit — call @ **0x004e1007** (world + car geometry path)
The 3D primitive leaf the world-sector and atomic pipelines bottom into. Submit
context (0x004e0fc0..0x004e101f):
- `EAX = *0x007d4110` (device), `ECX = *EAX` (vtable); pre-call
  `CALL [EDX+0x1a0]` = device **SetIndices** (vtbl +0x1a0) and `CALL 0x004d53b0`.
- Vertex/index state globals: `0x007d7100`, `0x007d7108`, `0x007d7110`;
  `0x006181cc`, `0x006181d8` (cached stream/decl handles).
- Enclosing function is FPO/undefined (no Ghidra boundary); body spans
  ~0x004e0e00–0x004e12xx. `[UNCERTAIN: exact function start]` — find via the
  pipeline-create's submit-node fn-ptr (§5).

**The node-function identities (instance / transform-cull / light / submit) are the
one piece NOT cracked this session.** They are installed by the pipeline-CREATE at
device OPEN (the code that writes `plugin+0x3c`/`+0x40`). The submit node's body
ends at the 0x004e1007 `DrawIndexedPrimitive`. See §6 next target.

---

## 5. The BSP→RpWorld world-stream loader — FULLY CHAINED

The track-load path, top to the RW stream reader (all verified this session):

```
FUN_00426e10(trackIdx)                    track-load orchestrator; ends with DAT_0066d704 = 1
 │   builds "toastart/tracks/<name>" path, opens the .piz (FUN_00495280),
 │   reads COURSE.LUA (FUN_004260e0), parses it (FUN_0047a020 → &DAT_00644378)
 └ FUN_00479330(&DAT_00646e58, &DAT_00644378, &DAT_00644158)   Course::CreateFromDescription (C2)
    │   param_1 = course/world-A struct base = &DAT_00646e58
    │   *(course + 0x105d0) = FUN_0042a6b0(texdic)        load course TXD
    │   *(course + 0x105d4) = FUN_0042a640(param_2+0x140) ← load the BSP WORLD  (= RpWorld*)
    │   collision BSPs   → course+0x10090.. via FUN_0042a640 (flag &= ~0x40 = no-render)
    │   clumps/DFF       → FUN_0042a5d0 ; splines → FUN_0042a7f0 ; UVA → FUN_0042a740 ;
    │   anims/cam-path   → FUN_0042a860 ; lights → RpLightCreate + RpWorldAddLight(course+0x105d4,…)
    └ FUN_0042a640(name)                  BSP world-file loader entry
       │   builds filename (+ext), locates in piz/VFS via FUN_0042a530
       └ FUN_004b3c60(name)               world stream-read wrapper
          ├ FUN_004cc230(2, 1, name)      RwStreamOpen (mode 2 = read)        [name UNCERTAIN]
          ├ FUN_004cc5e0(stream, 0x0B, …) RwStreamFindChunk(rwID_WORLD)  ← literal chunk id 0x0b
          ├ FUN_004e99b0(stream)          RpWorldStreamRead → RpWorld*    ← THE BSP→RpWorld builder
          └ FUN_004cc160(stream, 0)       RwStreamClose
```

### The world handle arithmetic (closes E1's "no-writer" mystery)
`&DAT_00646e58` is the **course/world-A struct base** (a large struct; fields up to
`course+0x10638`). The two globals E1 called "world A / world B handles" are
sub-fields of it, written via `base + offset` (register-relative) — which is why
`reference_to` found no direct symbol write:
- **`DAT_0065742c` = 0x00646e58 + 0x105d4 = course+0x105d4 = the RpWorld\* (world-A).**
  Written by `*(course+0x105d4) = FUN_0042a640(...)` in `FUN_00479330`.
- **`DAT_00656ee8` = 0x00646e58 + 0x10090 = course+0x10090 = collision-BSP[0] handle.**
  So `FUN_00426670`'s "world-B" arm (`DAT_0066d700 != 0`) renders the collision
  BSP. `[UNCERTAIN: the game-mode that selects world-B / why collision is rendered]`.

`FUN_004e99b0` already has a prior note (`bucket_004ddfb0/0x004e99b0.md`); it is the
deepest builder — the RpWorldSector tree + per-sector vertex/atomic construction
(sector+0x38 atomic lists, etc.) lives there. **Decompiling FUN_004e99b0 +
its recursion is the world-loader port's first concrete target (§6).**

---

## 6. Consolidated struct-offset map (what the verbatim port must reproduce)

All offsets cited to the function that reads/writes them this session.

**RpWorld** (handle = `course+0x105d4` = `DAT_0065742c`):
- `+0x68` render-callback slot — set/get by `FUN_004e5820`/`FUN_004e5840` (E1 §3);
  default `FUN_004e5190`.
- `+0x7c` world's own pipeline override (FUN_004e5190).
- `+0x84` (short) empty-world guard / sector-or-atomic count (FUN_004e5190).
- `+0xc` current-camera slot — written by `FUN_004e4320` as
  `*(DAT_007d716c + world + 0xc) = camera` (the per-object extension base
  `DAT_007d716c`); also `world+4` non-zero → `FUN_004c0e50(world+4)`.

**RpWorldSector** (walked by `FUN_004e5680`):
- `+0x38` intrusive atomic linked-list head (`link=*(sector+0x38)`; node `link[2]` =
  atomic; terminate when `link == sector+0x38`).

**RpAtomic** (RpAtomicCreate @ 0x004e67b0, E1 §8; traversal in FUN_004e5680):
- `+0x02` (byte) flags; bit `0x4` = render-enabled (rpATOMICRENDER).
- `+0x44` frame; `+0x48` **render callback** (default `FUN_004e5f90`).
- `+0x60` (short) per-frame render stamp — compared to `RwGlobals+2`; set after draw.
- `+0x6c` atomic's own pipeline override (FUN_004e5f90; 0 → engine default +0x3c).

**RwCamera** (`*(RwGlobals)` = the active camera in FUN_004e5680):
- `+0x18` beginUpdate callback (FUN_004c1a00, E1 §1).
- `+0x94` 6 × RwFrustumPlane (0x14 each); plane = normal.xyz(+0..8)+dist(+0xc)
  (FUN_004c1b40).

**RwGlobals** (`*0x007d3ff8` = pointer G):
- `+0x02` (short) per-frame render-stamp counter (FUN_004e5680).
- `+0x04` world/sector-TYPE object `O` = `{+0x68 sectorVisTest, +0x6c worldPipeline}`
  (FUN_004f0900, FUN_004e5190).
- `+0x10` RwDevice (E1 §7); `+0x108` resource allocator fn-ptr (FUN_004d4dd0);
  `+0xcc`/`+0xd4` string/path builder fn-ptrs (FUN_00426e10, FUN_0042a640).
- `+ DAT_00911ae0` D3D9 plugin block (§3.3); `+0x3c` atomic pipe, `+0x40` world pipe.

**Globals**: `DAT_007d4710` render heap/cluster mgr; `DAT_007d4110`/`DAT_007d4120`
IDirect3DDevice9*; `DAT_0066d704` world-loaded (set 1 in FUN_00426e10; cleared in
FUN_00426b40/FUN_00426e10-teardown); `DAT_0066d700` world A/B selector
(set in FUN_004266f0); prim-type table `0x005d8d8c`.

---

## 7. Open / next targets (precise, before any port code)

In dependency order for the B-FULL world-render port:

1. **[CLOSED 2026-06-16 — see §9]** World-loader port root: `FUN_004e99b0`
   (RpWorldStreamRead) + its sector readers `FUN_004ea220` (plane-sector tree) and
   `FUN_004e9e40` (atomic-sector geometry) decompiled; RpWorld/RpWorldSector build
   + `sector+0x38` list construction mapped.
2. **[CLOSED 2026-06-16 — see §10]** Pipeline-CREATE: `FUN_004f0810` (master) →
   `FUN_004f43f0` (atomic, plugin+0x3c) / `FUN_004f4340` (world, plugin+0x40). Both
   are single all-in-one nodes: world body `FUN_004eb3d0`
   (`nodeD3D9WorldSectorAllInOne`), atomic body `0x004ea6d0`
   (`nodeD3D9AtomicAllInOne`). Remaining: full decompile of the two node bodies
   (the internal instance/light/submit → 0x004e1007) — port-time work, not blocking.
3. **`FUN_004f0900` concrete target** — find the writer of `RwGlobals+4` (object O)
   and trace `O+0x68` to the real sector-visibility function.
4. **World-B semantics** — which game-mode sets `DAT_0066d700 != 0` so the collision
   BSP (`course+0x10090`) renders as the world.
5. **Verification (WS-H1)** — once a render slice runs, `diff-original` the generic
   leaves with the inline-JMP live: `FUN_004c1b40`, `FUN_004e5680`, `FUN_004e5f90`,
   `FUN_004e5190`, `FUN_004d40d0`, `FUN_004b3c60`, `FUN_004e99b0`.

## 8. Function index (all verified this session on Mashed_pool12)
| RVA | role |
|---|---|
| 0x004c1b40 | RwCameraFrustumTestSphere (full decomp) |
| 0x004f0900 | per-sector vis-test thunk → `(*(RwGlobals+4))+0x68` |
| 0x004d40d0 | RxPipelineExecute dispatch (node = pipeline+8 → vtable[1]) |
| 0x004d4dd0 | render-heap / cluster reset |
| 0x004d3db0 | D3D9 plugin-data constructor (heap + 0x34 obj; not +0x3c/+0x40) |
| 0x004d8530 | D3D9 plugin registration ctor (writes DAT_00911ae0 offset) |
| 0x004e5f90 | atomic render callback → default ATOMIC pipe (plugin+0x3c) |
| 0x004e5190 | world-sector render callback → default WORLD pipe (plugin+0x40) |
| 0x004dc10e | DrawIndexedPrimitive — Im2D submit (E4) |
| 0x004e1007 | DrawIndexedPrimitive — 3D submit (world+car, E1/E2) |
| 0x00426e10 | track-load orchestrator (sets DAT_0066d704=1) |
| 0x00479330 | Course::CreateFromDescription |
| 0x0042a640 | BSP world-file loader entry |
| 0x004b3c60 | world stream-read wrapper (RwStreamFindChunk 0x0B) |
| 0x004e99b0 | RpWorldStreamRead (BSP→RpWorld builder) — NEXT to decompile |
| 0x004cc230 / 0x004cc5e0 / 0x004cc160 | RwStreamOpen / FindChunk / Close |
| 0x004e4320 | world begin = `*(DAT_007d716c+world+0xc)=camera` |
| 0x00426b40 / 0x004266f0 | world teardown (flag=0) / A-B selector setter |
| **0x004ea220** | RpPlaneSector recursive reader (rwID_PLANESECTOR 10) |
| **0x004e9e40** | RpWorldSector geometry reader (rwID_ATOMICSECTOR 9); builds sector+0x38 list |
| 0x004f3e90 | material-list reader (world+0x10) |
| 0x004e1b60 / 0x004e1c90 | RpWorld/sector plugin-extension (binMesh) reader |
| 0x004e5300 / 0x004e5700 | RpWorldInstance / RpWorldDestroy (Ghidra-named) |
| 0x004cbd30 / 0x004cc790 | RwStreamRead / RwStreamReadReal(endian) |
| **0x004f0810** | master D3D9 default-pipelines create (zeroes plugin+0x3c..+0x5c) |
| **0x004f43f0** / **0x004f4340** | atomic / world default-pipeline create |
| 0x004f07d0 / 0x004f07a0 | atomic (plugin+0x3c) / world (plugin+0x40) pipeline setter |
| 0x004eb3c0 / 0x004eb9d0 | atomic / world node-definition providers |
| **0x004ea6d0** | nodeD3D9AtomicAllInOne body (atomic node execute) |
| **0x004eb3d0** | nodeD3D9WorldSectorAllInOne body (world node execute) |
| 0x004d4170 / 0x004d4380 / 0x004d41e0 | RxPipelineCreate / Unlock / Destroy |
| 0x004d4dd0 / 0x004d4f90 | RxPipelineLock / RxPipelineNodeAddFragment |

---

## 9. RpWorldStreamRead build — `FUN_004e99b0` (residual #1 closed)

`FUN_004e99b0(stream)` @ 0x004e99b0..0x004e9e33 — builds the RpWorld from the
rwID_WORLD chunk. Sequence (every callee verified):
1. `FUN_004cc5e0(stream, 1, &len, &ver)` RwStreamFindChunk(**rwID_STRUCT=1**);
   version gate `0x34fff < ver < 0x37003`.
2. `FUN_004cbd30(stream, &hdr, len)` RwStreamRead → 0x40-byte world header (flags
   `local_1c`, bbox 3 floats × scale `_DAT_005cc33c`, counts, format `local_20`).
3. Computes the RpWorld+inline-sector alloc size from the format flags
   (`base DAT_0061864c + DAT_006189e4*numMat + …`; +normals if flag 0x10, +prelight
   if flag 8, +texcoord sets, +tris), allocates via `(*(RwGlobals+0x108))(size,
   0x3000b)`, zero-fills.
4. Inits the RpWorld struct (offsets below), `FUN_004e5280` (sub-init).
5. `FUN_004cc5e0(stream, **8**, …)` rwID_MATLIST → `FUN_004f3e90(stream, world+0x10)`
   reads the material list to `world+0x10`.
6. Sector tree: if `hdr[0]==0` → `FUN_004cc5e0(stream, **10**,…)` rwID_PLANESECTOR →
   **`FUN_004ea220`** (recursive); else `FUN_004cc5e0(stream, **9**,…)`
   rwID_ATOMICSECTOR → **`FUN_004e9e40`**. Root stored at **`world+0x1c`**.
7. `FUN_004e51e0` + `FUN_004e5820(world, 0)` set `world+0x68` = default render
   callback (`FUN_004e5190`). Per-material `FUN_004e8090`.
8. `FUN_004d8000` + `FUN_004e1b60(&DAT_0061864c, stream, world)` read the world's
   rwID_EXTENSION plugins (binMesh native data → `DAT_007d7278`).
9. **`RpWorldInstance(world)` @ 0x004e5300** — instances geometry into the device
   (the resident VB/IB the node bodies submit). Returns world.

**RpWorld struct** (handle = world-A `DAT_0065742c`):
`+0x00`(byte)=7 rpWORLD type; `+0x03`=1; `+0x08`=format flags; `+0x0c`=2;
**`+0x10`=material list**; `+0x14`=numMaterials; **`+0x1c`=root sector**;
`+0x20`=numTexCoordSets; `+0x2c/+0x34/+0x3c`=intrusive list heads (self-linked);
`+0x44..+0x4c`=bbox×scale; `+0x50..+0x67`=6-float bbox/origin; `+0x68`=render cb.

**RpPlaneSector** (`FUN_004ea220`, 0x18 bytes): `+0x00`=plane type/axis; `+0x04`=
split value; **`+0x08`=left child**, **`+0x0c`=right child** (each a plane-sector→
recurse, or atomic-sector→leaf, chosen by the hdr left/right flags); `+0x10/+0x14`
=left/right bound values.

**RpWorldSector** (`FUN_004e9e40`, size `DAT_006189e4`): `+0x00`=0xffffffff (leaf
tag); `+0x04`=triangles ptr (numTris×8 = 4×u16 v0,v1,v2,mat); `+0x08`=vertex
positions (numVerts×0xc); `+0x0c`=normals (if format 0x10); `+0x30`=prelight
(if format 8, RGBA×numVerts); `+0x10..`=texcoord-set ptrs; `+0x34`=mesh/binMesh
list (read by the node body `FUN_004eb3d0`); **`+0x38`=intrusive atomic-list head
(self-linked empty at load)** ← the list `FUN_004e5680` walks; `+0x40`=2nd list
head; `+0x60..`=bbox (6 floats); `+0x80`(short)=matListWindowBase;
`+0x82`(short)=numVertices; `+0x84`(short)=numTriangles. **Triangle u16 order +
geometry layout are byte-faithful to the standalone's `Track::World` parse.**
Pre-0x36002 streams byte-swap the triangle words (loop @ ~0x004e9f80).

Note: at load the `sector+0x38` atomic list is an empty self-link; movable atomics
(cars, props) are linked in later when added to the world — static world geometry
is rendered by the world-sector NODE (§10), not via the +0x38 atomic list.

---

## 10. The RxD3D9 node graph — single all-in-one nodes (residual #2 closed)

The default pipelines are built by **`FUN_004f0810`** (the RxD3D9 pipelines-create,
reached at device OPEN): it zeroes `plugin+0x3c..+0x5c`, then calls `FUN_004d8560`
(`return 1` stub), **`FUN_004f43f0`** (atomic pipe), **`FUN_004f4340`** (world pipe);
on failure `FUN_004f43b0`/`FUN_004f4470`/`FUN_0045b350` tear down.

Each create is identical in shape (standard RW): `RxPipelineCreate` `FUN_004d4170`
→ set `pipe+0x2c=2` → `RxPipelineLock` `FUN_004d4dd0` → node-definition provider →
`RxPipelineNodeAddFragment` `FUN_004d4f90(locked, 0, nodeDef)` → `RxPipelineUnlock`
`FUN_004d4380` → save factory copy + install:
- **World** (`FUN_004f4340`): nodeDef = `FUN_004eb9d0()` =
  `&nodeD3D9WorldSectorAllInOne.csl` desc @ **0x00618768**; save `plugin+0x58`
  (factory), set `plugin+0x40` via `FUN_004f07a0`.
- **Atomic** (`FUN_004f43f0`): nodeDef = `FUN_004eb3c0()` =
  `&nodeD3D9AtomicAllInOne.csl` desc @ **0x00618708**; save `plugin+0x54`, set
  `plugin+0x3c` via `FUN_004f07d0`; then `FUN_004f46a0` + `FUN_004fa5f0`.

**Both pipelines are a SINGLE fused "all-in-one" node** — NOT a multi-node
instance→cull→light→submit chain. The RxNodeDefinition (`.csl`) layout decoded from
the two descriptors:
| desc off | world @0x618768 | atomic @0x618708 | RxNodeDefinition field |
|---|---|---|---|
| +0x00 | 0x00618748 | 0x006186e8 | name ptr ("nodeD3D9{WorldSector,Atomic}AllInOne.csl") |
| +0x04 | **0x004eb3d0** | **0x004ea6d0** | **nodeBody = the node EXECUTE fn** |
| +0x08,+0x0c | 0 | 0 | nodeInit / nodeTerm |
| +0x10 | 0x004eb520 | 0x004ea970 | pipelineNodeInit |
| +0x2c | 0x1000 | 0x1000 | pipelineNodePrivateDataSize |

So `FUN_004d40d0`'s `pipeline+8 → P->vtable[1](P, &io)` resolves to these node
bodies: an atomic renders via `FUN_004ea6d0`, a world sector via `FUN_004eb3d0`.
Both take `(self, &io)` with `*io` = the object (verified from the prologues:
`MOV ECX,[ESP+4]; MOV EAX,[ESP+8]; … MOV reg,[EAX]`). `FUN_004eb3d0` reads
`sector+0x82` (numVertices) and walks the mesh list at `sector+0x34`, bottoming into
the 3D submit leaf (§4.2, DrawIndexedPrimitive @0x004e1007). Both node bodies are
FPO/undefined in Ghidra (never auto-disassembled) — their full internal
decompile (instance / transform / light / per-mesh submit) is the next port-time
task, but is no longer a *blocker*: the node identities, contract, and submit leaf
are all pinned.

### Port consequence
The B-FULL world render = port (a) `RpWorldStreamRead` §9 to build the structs,
(b) the traversal §1–§3 (already mapped), (c) the two all-in-one node bodies
`FUN_004eb3d0`/`FUN_004ea6d0` as the geometry submit, (d) the RW D3D9 device-table
leaves they call (render-state set 0x004d7480, the 3D submit @0x004e1007). No
multi-node RxPipeline machinery needs porting beyond Create/Lock/AddNode/Unlock/
Execute — the graph is one node per object type.
