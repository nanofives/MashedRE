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

Drafted by the account2 worker from `island_dag.tsv` edges + `hooks.csv` + the thunk files
(2026-07-16, coverage arithmetic verified: 128 clustered + 8 DONE + 1 intrinsic = 137 ✓);
K1 pinned by this session. **No cluster has a forward dep** — every island callee lands in the
same or an earlier cluster, so at each swap point the ported code calls only ported code or
the original (via absolute-address reads / RVA thunks).

DONE-ALREADY (excluded): 004c3ac0 (C4), 004c3b30 (C4), 004c45f0, 004d7ff0, 004d8480 (C3),
0055ac00, 0055c000, 0055e200 (B5c C3). Intrinsic: 004a3440 `__chkstk` (compiler-provided).

| id | members (rva:size) | B | fns | deps | ind |
|---|---|---|---|---|---|
| **K1** ✅ | 005601f0:109 00563e70:239 00563f60:221 00564040:187 005641b0:351 00564310:663 00565120:60 00565160:67 005651b0:70 00565200:94 00565550:22 00565ef0:171 00565fa0:167 00566200:411 005667c0:109 00566830:163 005675d0:91 005684c0:149 00568560:143 | 3487 | 19 | — | — |
| K2 | 004c4600:111 004c51a0:323 004c52f0:378 00546b10:214 00546bf0:93 00546c50:93 00546cb0:93 | 1305 | 7 | DONE | 3 |
| K3 | 0055a1f0:1957 0055a9a0:397 0055abb0:73 0055ae50:27 0055b750:173 0055bae0:133 0055bd70:14 0055c0f0:318 0055c2d0:175 | 3267 | 9 | K1 | 4 |
| K4 | 005646c0:1465 00564c80:1179 00565260:747 | 3391 | 3 | K1 | — |
| K5 | 0055ab30:117 0055e050:44 0055bb70:115 0055bd80:85 0055c230:154 | 515 | 5 | K2 K3 K4 | 3 |
| K6 | 0056bb80:337 0056bce0:260 0056bdf0:135 0056be80:534 0056c0a0:618 0056c310:614 | 2498 | 6 | DONE | — |
| K7 | 0056c580:861 0056c8e0:434 0056caa0:1262 0056cf90:216 0056d070:724 | 3497 | 5 | — | — |
| K8 | 0056d350:158 0056d3f0:2375 0056dd40:2357 0056ed60:463 0056e680:1756 | 7109 | 5 | — | — |
| K9 | 0056ef30:133 0056efc0:83 0056f020:113 0056f0a0:328 0056f1f0:338 0056fad0:186 00567c00:49 | 1230 | 7 | — | — |
| K10 | 0056f350:1918 0056fb90:770 0056fea0:483 | 3171 | 3 | K2 K3 K9 | 1 |
| K11 | 00567f00:1460 00567c60:670 005685f0:431 00568dd0:505 00568fd0:354 | 3420 | 5 | — | — |
| K12 | 00570090:10500 (constraint/LCP core, own cluster) | 10500 | 1 | K1 K9 K10 | — |
| K13 | 00560260:3537 (island partition, own cluster; 16-callee fan-in) | 3537 | 1 | K1 K6 K7 K8 K9 K10 K12 | 1 |
| K14 | 00577be0:196 00577cb0:513 00577ec0:1497 005784a0:366 | 2572 | 4 | — | — |
| K15 | 00576880:4958 (own cluster) | 4958 | 1 | K1 K14 | — |
| K16 | 005735f0:126 00575120:208 005751f0:181 00576640:563 00579b50:165 00579c00:328 00579d50:243 00579e50:134 00579ee0:866 0057ae20:8 | 2822 | 10 | — | — |
| K17 | 00575b60:250 00578b20:176 00578bd0:213 00578cb0:212 00578d90:185 0057a250:1038 0057a660:824 00575fe0:1632 00578ff0:2899 | 7429 | 9 | K1 K2 K3 K16 | 2 |
| K18 | 005757d0:170 00575c60:890 00574ad0:1615 00578610:1287 0057a9a0:1035 0056b7a0:546 | 5543 | 6 | DONE K1 K2 K3 K5 K16 K17 | 2 |
| K19 | 00578e50:105 0057adb0:105 005752b0:680 | 890 | 3 | K15 K18 | — |
| K20 | 00575880:732 00575560:618 | 1350 | 2 | K3 K17 K19 | — |
| K21 | 00561280:265 00568990:675 005729a0:3138 0056ba30:244 0056bb30:67 | 4389 | 5 | K1 K2 K3 K5 K6 K15 K16 K18 K20 | 2 |
| K22 | 00573670:544 0056b9d0:95 | 639 | 2 | K3 K5 K16 K18 K21 | — |
| K23 | 0055fe50:77 0055fea0:203 0055ff70:28 0055ff90:603 00561040:566 00561390:2239 00561c50:517 00561e60:31 00561e80:31 (the 9 remaining stage drivers) | 4295 | 9 | DONE K1 K3 K5 K6 K11 K13 K21 K22 | 3 |
| K24 | 0047e9c0:118 (root; retires the RwpBuildExterns `FUN_0047e9c0` thunk) | 118 | 1 | DONE K23 | — |
| KV1 | scene callbacks: 0x56b310, 0x56a450/0x56adb0 vs 0x569140/0x5697f0 (+ callees, e.g. 0x56aae0/0x56ac40) — installed by scene ctor `FUN_0055f800` | ~ | ~10 | after K13 | n/a |
| KV2 | body object table 0x0062403c targets: 0x57c3a0/0x57c3f0/0x57c590/0x57c2b0 + registrar FUN_0057c270 | ~ | ~5 | any | n/a |
| KV3 | volume-descriptor slot fns (7 live tables: Sphere/Box/Cylinder/Trilist/Capsule/Triangle/Null) — ~48 targets ~18 KB total incl. KV1/KV2; exact per-table lists in `island_vtable_targets.tsv` | ~18KB | ~48 | any | n/a |

Per-cluster island-callee→cluster edge detail: worker output archived at
`re/analysis/b5e/cluster_plan_worker_2026-07-16.md`. KV1..KV3 are NOT needed for per-cluster
`.asi` A/B acceptance (ported code reads the REAL tables via absolute addresses, so dispatch
lands in original code until KV lands) — they ARE needed before lane-end standalone truth.
Thunk retirement: K2 retires the `FUN_004c52f0/004c51a0/00546b10` RVA thunks
(RwpBuildExterns.cpp:62-64); K24 retires `FUN_0047e9c0` (line 65). The other ~20 thunks target
non-island RVAs and stay.

## 4. Cluster 1 — K1 ported (19 leaves, 3,487 B) → `Collision/RwpSolverLeaves1.cpp`

Clean-room verbatim transcription of the 19 K1 leaves from `re/analysis/b5e/decomp/` (worker
first-draft, locally verified + corrected), built into BOTH targets (`build.bat` +
`asi_sources.rsp`): `mashed_re.exe` 1,161,728 B + `mashed_re_dev.asi` 726,528 B, exit 0
(`log/b5e-solver-island/b5e_cluster1_build.log`; 2 benign warnings are the decomp's own
unsigned-negate and precedence forms, cross-checked against disasm). All 19 registered
runtime-toggleable via `RH_ScopedInstall`.

NO-GUESSING verifications done against live pool0 disasm before accepting the draft:
1. **Calling convention**: all 19 bodies end in plain `RET` (0xC3, caller cleanup) ⇒ `__cdecl`
   (byte reads at each body_end; e.g. `FUN_00564310` ends 0x005645a6).
2. **`PTR_DAT_005ceabc` is a FLOAT, not a pointer** — `FCOM float ptr [0x005ceabc]` at
   `0x005667e2`; bits `0x00800000` = 1.17549435e-38 (FLT_MIN threshold). Ghidra data type was
   wrong; port binds `*(float*)0x005ceabc`.
3. **Ghidra `ABS()` = x87 FABS** (`0x0056628a` ff. in FUN_00566200; `0x0056683d/44/4c` in
   FUN_00566830) ⇒ ported as `fabsf` (sign-bit clear, exact incl. −0.0), NOT a ternary.
4. **Ghidra `SQRT()` = x87 FSQRT** (`0x005667f7`) on an 80-bit ST0 chain ⇒ `sqrtl`; carries the
   accepted ≤1-ULP x87 floor (MSVC long double is 64-bit) — [X87]-tagged in source.
5. **`FUN_005684c0` "pointer into undefined4" stores are plain 32-bit `MOV`s**
   (`0x005684eb..0x00568549`) ⇒ ported with value casts, layout identical.
6. **`FUN_00564310` calls CRT `rand` at `0x005c229b`** — ported via a fn-ptr to the ORIGINAL
   static-linked CRT so the RNG state stays shared under the `.asi` A/B (standalone rebind is a
   lane-end item, flagged in source).
7. **`FUN_00566830` bit-select expression** matches disasm exactly
   (`SHL/OR` chain `0x0056688d..0x005668ae`, second index uses the UPDATED first index).

**Acceptance state: BLOCKED-ENV, not run.** The canonical bridge-driven QuickRace
(`--hooks` bridge + B5c 8 + all 19 K1 RVAs) could not run this session: the desktop was
locked/display asleep — `diag.py probe-render` returned `hr=0x8876086A` (D3D9-NOTAVAIL), and
MASHED self-exits (−1) before the menu with NO crash dump, canonical `.asi` control identical.
Environment recovery already applied (repatch_original, setup_mashed_compat, diag doctor —
all green except render). The worktree `.asi` is built and ready; the canonical `.asi` backup
is at `original\mashed_re_dev.asi.pre-b5e-cluster1`. **First action next session (with an
awake display): deploy the worktree `.asi` and run the §0 per-cluster acceptance command.**

## 5. Status

- Island census exported + committed (89bbf488); vtable reach resolved + committed (bce5a9ce).
- Cluster queue K1..K24 + KV1..KV3 ratified into §3 (worker-drafted, coverage-verified).
- K1 ported + built both targets; canonical-race acceptance BLOCKED-ENV (display asleep) — run
  it before claiming K1 swap-clean; only then re-classify the 19 leaves C1→C2.
- OPEN next: K1 acceptance race → re-classify → K2 (RW-math quat/matrix, retires 3 thunks).
