# B5e — FUN_0047e9c0 solver-island clean-room port: plan + cluster queue — 2026-07-15

Lane B5e (ROADMAP §WS-B, D1 Option A; RE_MASTER_PLAN §7 item 10; solver-island gate answered
**Option A — full clean-room island**, fd3a7428). Inherits B5a..B5d
(`B5d_COUPLING_BRIDGE_2026-07-15.md`). Worktree `.worktrees/b5e-solver-island`
(branch `r7/b5e-solver-island`), Ghidra pool0, read_only, anchor
`MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`.
NO-GUESSING: every constant/offset below cites the live Ghidra address.

## 0. Identity and end-state

`FUN_0047ea40 → FUN_0047e9c0` is the per-tick RenderWare-Physics-3.7 world step (the "solver
island"). B5c/B5d left it reachable only through the RVA thunk `FUN_0047e9c0` in
`Collision/RwpBuildExterns.cpp` — faithful under the `.asi`, dead on the standalone exe. B5e
ports the island clean-room (our own decomp only; no Rwp source exists) so the standalone calls
no MASHED code.

**Lane-end acceptance** = whole-loop per-field body-state bit-identity diff (the deferred B5d §4
C4 diff folded in — commit 08595848): canonical bridge-driven race, per-field diff of the proxy
body state via `body=FUN_0057c210((&DAT_006c9a78)[i])`; linear fields bit-identical, angular
fields carry the accepted ≤1-ULP x87 floor (`project_phys_chain_float10_methodology`).

**Per-cluster acceptance (thunk-swap A/B)**: port N functions → `RH_ScopedInstall` each at its
RVA (runtime-toggleable) → under the `.asi` the original island's calls route into the ports via
inline-JMP → re-run the canonical bridge-driven QuickRace
(`py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --hold 35 --hooks <bridge+B5c8+cluster RVAs>`)
clean. Ports of not-yet-ported island callees stay RVA-forwarding thunks until their cluster lands.

## 1. Island census (this session's Ghidra pool0 walk)

Static transitive call closure from `0x0047e9c0`, exclusions: marker passes
`FUN_0047d640`/`FUN_0047def0`; library bands zlib/libpng `0x516000-0x529fff`, CRT
`0x5c0000-0x5c8000`, qhull `0x57c5b0-0x5a5820`, D3DX9 PSGP `0x4ec000-0x4fc9e0`.

- **137 functions, 82,611 bytes** (matches the B5d §5 sizing walk). Full table:
  `re/analysis/b5e/island_dag.tsv` (per fn: size, internal/external callees, indirect-call flag,
  island-caller count). Verbatim decomp text per function: `re/analysis/b5e/decomp/FUN_00xxxxxx.c`
  (committed 89bbf488 — later cluster sessions port from these files; re-verify against live
  decomp only on [VERIFY-DISASM] flags).
- **65 leaves** (no island-internal callees). **23 functions contain indirect calls** (vtable /
  fn-ptr dispatch): 004c4600, 004c52f0, 00546b10, 0055a1f0, 0055bae0, 0055bd70, 0055bd80,
  0055c000, 0055c230, 0055c2d0, 0055e050, 0055fe50, 00560260, 00561040, 00561280, 00561390,
  0056f350, 005729a0, 00575c60, 00575fe0, 00578610, 0057a660, 0057a9a0.
- **10 largest** (rva:bytes): 00570090:10500 (constraint/LCP core), 00576880:4958, 00560260:3537
  (island partition), 005729a0:3138, 00578ff0:2899, 0056d3f0:2375, 0056dd40:2357, 00561390:2239
  (CCD/impact), 0055a1f0:1957 (broadphase), 0056f350:1918.
- **Excluded edges** (3, `island_excluded_edges.tsv`): root → the two marker passes;
  `FUN_00564310 → _rand@0x005c229b` (CRT band — port note: `_rand` is CRT `rand()`; the port
  must call the SAME generator state as the original for bit-identity [UNCERTAIN — resolve at
  FUN_00564310's cluster: which RNG state the original reads]).
- `__chkstk@0x004a3440` is inside the closure (called by FUN_005646c0) — stack-probe intrinsic;
  handled by the compiler in the port, not transcribed.

## 2. Vtable / indirect-dispatch reach

Resolved (full evidence: `re/analysis/b5e/island_vtable_reach.md`, targets table
`island_vtable_targets.tsv`, 104 rows). **12 dispatch tables in 4 families**:

1. **RWP volume descriptors** — `*(shape+0x5c)` object, 10 fn-ptr slots each; 9 tables:
   Sphere `0x5e4f50`, Box `0x5e4fe0`, Cylinder `0x5e51c8`, Trilist `0x5e52a8`, Capsule
   `0x5e5338`, Grid `0x5e54c0`, Aggregate `0x5e5900`, Triangle `0x5e5db0`, Null `0x5e5e50`.
   Install sites cited per table in the reach note (e.g. Box at `0x0055c815` in `FUN_0055c810`;
   Triangle installed island-internally by `FUN_00575fe0` at `0x00576344`). **Grid and Aggregate
   have zero references — dead types in this build** (no port needed).
2. **`rwpOBJTYPEBODY` object table `0x0062403c`** — installed by body-creator `FUN_0057c300` at
   `0x0057c31a`; slot `+0x1c` runtime-overridden to `FUN_0057c2b0` by registrar `FUN_0057c270`
   (plugin offset `DAT_007dc8d8`). Resolves the `FUN_0055a1f0`/`0055e050`/`00561390` obj[0]
   dispatches (→ `0x57c3a0`/`0x57c3f0`/`0x57c590`/`0x57c2b0`). `rwpOBJTYPERAGDOLL` @`0x632b68`
   is never installed (0 refs).
3. **Scene callbacks** (`FUN_00560260`'s `code*` params) — installed in scene ctor
   `FUN_0055f800`: `+0x404=0x56b310` always; `+0xf4`/`+0xf8` = `0x56a450`/`0x56adb0` vs
   `0x569140`/`0x5697f0` branching on `scene+0x58==2` (sites `0x55fd8e/9c/a6/b5/bf`).
4. **RwMatrix module slot** (`FUN_004c4600`/`004c52f0`) — single writer `0x004c44f1` stores
   `0x004c40e0`; no other writer among all `DAT_007d4028` readers.
   `FUN_00546b10` resolves internally to `FUN_00546c50`/`00546cb0` (both already in the 137).

**Reach beyond the 137:** 78 unique targets across all slots (~26.1 KB, span-estimate upper
bounds for the ~69 Ghidra-undefined ones); the subset on slots the island actually calls is
**48 targets / ~18.0 KB**. **Real port surface ≈ 185 fns / ~101 KB.** 5 `[UNCERTAIN]` items in
the reach note (undefined-target extents, a possible `param_12` call in `FUN_00560260`,
descriptor tail fields, the 0x5cf9e8 game-vtable family ruled out, an unreferenced continuation
table near `0x5e5ab8`).

## 3. Cluster queue (the session plan for the rest of the lane)

<!-- FILL: cluster table from worker draft, locally verified -->

## 4. Cluster 1 (this session)

<!-- FILL: what was ported, thunks retired, build + canonical race evidence -->

## 5. Status

- Island census exported + committed (89bbf488).
- OPEN: vtable reach (§2), cluster queue (§3), cluster 1 port (§4).
