# B5c — RWP-3.7 per-tick integrator SUBSET (clean-room port) — 2026-07-15

Lane B5c (ROADMAP §WS-B, D1 Option A; RE_MASTER_PLAN §7 item 10). Inherits B5a
(`B5a_SYSTEM2_PLATING_2026-07-14.md`) + B5b (`B5b_RWP37_QHULL_VENDOR_2026-07-14.md`).
Ghidra pool0, read_only, anchor `MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`.
NO-GUESSING: every constant/offset cites the live Ghidra address.

## 0. Scope correction (ratified with the user 2026-07-15)

Two load-bearing errors were inherited from the B5a note (OU#4) and the B5c brief; both are
corrected here from the live decomp before any port:

1. **`0x007e9de0` is NOT proxy-body state.** `DAT_007e9de0` is a **scalar ring write-index**
   (`DAT_007e9de0 += 1; if (0xff < DAT_007e9de0) DAT_007e9de0 = 0;`). `&DAT_007e9de4 /
   007ea1e4 / 007ea5e4 / 007ea9e4 / 007ec9e4` are a **256-slot on-screen event/collision
   MARKER draw-stream** (per slot: type tag literal 6/8/9/10, RGBA colour `DAT_007f1030` /
   `0xff0000ff` / `0xffff0080`, id, 8-word payload, 16-float transform), written by
   `FUN_0047def0` (0x0047def0) and `FUN_0047d640` (0x0047d640) — both Ghidra-labelled
   *"per-frame on-screen event-marker emitter"*, gated on game-mode `FUN_0040e350() ∉ {1..5}`.
   The "stride 0x404" in B5a was the counter(0x4)+array(0x400) spacing, not a body record.
   The real proxy-body state is **pointer-indirect**: `body = FUN_0057c210(DAT_006c9a78[i])`;
   its world-matrix slot is refreshed by `FUN_0055dff0 → FUN_0055b800`.

2. **The B5a-named helpers are NOT the integrator; the true step is the whole solver island.**
   `FUN_0055dff0/0055ac00/0055deb0/0055c000/0057c210` are **coupling-side helpers** driven by
   `FUN_0047eb30` (the B5d coupling driver). The actual per-tick step is
   `FUN_0047ea40 → FUN_0047e9c0`, and `FUN_0047e9c0` is a **12-stage RWP solver pipeline**
   (`FUN_0055e200/0055fe50/0055fea0/0055ff90/0055ff70/00561e60/00561040/00561e80/00561390/
   00561c50` + the two marker passes) whose transitive closure is essentially the entire
   ~165 KB `RwpConstraints`/`RwpPartition` island (broadphase `FUN_0055a1f0`, island-partition
   `FUN_00560260`, CCD/impact `FUN_00561390`, constraint iterate `FUN_00573670`/`0056b9d0`).
   That contradicts the brief's *"port on demand, not the whole island."*

**Ratified scope (narrow subset):** port the 5 named helpers + `FUN_0055b800` + the trivial
wrappers `FUN_0055e200`/`FUN_0047ea40`; DEFER the full `FUN_0047e9c0` solver pipeline to a new
lane. Acceptance = the REAL pointer-indirect body-matrix state, not the `0x007e9de0` ring.

## 1. Ported functions (verbatim) → `Collision/RwpIntegrator.cpp`

| RVA | Our symbol | Role | Notes |
|---|---|---|---|
| `0x0057c210` | `RwpBodyTableLookup` | `return *(DAT_007dc8d8 + key)` body accessor | binds real `DAT_007dc8d8` (game global) |
| `0x0055deb0` | `RwpWorldSolverHandle` | `return *(world+4)`; 2nd `dt` arg ignored by body | dt = `DAT_0061331c` = 0.05 |
| `0x0055ac00` | `RwpShapeActiveBitSet` | shape-active bit set/clear at `owner+0x5c` | leaf, no abs writes |
| `0x0055b800` | `RwpBodyMatrixRefresh` | copy src mtx → body world-matrix slot, translate, matrix→quat | calls RW-math `004c52f0`/`004c51a0`/`00546b10` (extern) |
| `0x0055dff0` | `RwpBodyRefreshGate` | active-bit gate → tail-call `0x0055b800` | **pre-branch-args resolved**, see §2 |
| `0x0055c000` | `FUN_0055c000` (kept name) | GJK support map (vtable slot `shape+0x5c+0x14`) | extern-C name; called by CollisionBodyCreate.cpp |
| `0x0055e200` | `RwpSolverContextSet` | `*ctx = value` | trivial |
| `0x0047ea40` | `RwpSceneStepWrapper` | `FUN_0047e9c0(ctx,a2); return ctx` | `FUN_0047e9c0` DEFERRED (extern) |

All registered runtime-toggleable via `RH_ScopedInstall(...)` (inert on the exe via
`HookSystemNoOp`; inline-JMP under the `.asi` for the diff-original A/B).

## 2. `FUN_0055dff0` pre-branch-args resolution (disasm 0x0055dff0..0x0055e042)

Ghidra recovered `FUN_0055dff0(int param_1)` calling `FUN_0055b800()` with no args — the
pre-branch-args pitfall ([[feedback_ghidra_prebranch_args]]). Disassembly shows it is
`FUN_0055dff0(body, idx)`:
```
EDX = body = [ESP+4]
if ([body+0x24]==0) ret                                   ; owner
CX = [body+0x20]                                           ; sidx
if (([ *(owner+0x60) + (sidx>>5)*4 ]) & (1<<(sidx&0x1f)) == 0) ret   ; active bit
EAX = idx = [ESP+8]; EDX = M = [body+0x10]
EAX &= 0xffff; ECX = idx*3; e = M + ECX*4 (= M + idx*0xc)
[ESP+8] = *(e+8); [ESP+4] = *(e+4); JMP 0x0055b800         ; tail call
⇒ FUN_0055b800( *(M+idx*0xc+4), *(M+idx*0xc+8) )
```
The coupling driver `FUN_0047eb30` calls `FUN_0055dff0(body, 0)` (idx 0).

## 3. `FUN_0055dec0` conflict — RESOLVED (NO-GUESSING)

`CollisionBuildDeps.h`/B5a treated `0x0055dec0` as a 4-arg "register body into world"
`FUN_0055dec0(world,node,-1,2)`; `Util/UtilLeaves_ab6.cpp` ports it as the 3-byte leaf
`return *param_1`. **Both are correct simultaneously:** `FUN_0055dec0` (body 0x0055dec0..
0x0055dec6) is literally `return *param_1`; `FUN_0047d3c0` *calls* it with 4 args but cdecl
means the extra 3 (`node,-1,2`) are pushed-and-ignored. The "register body" label was
mis-attributed — the call returns `*world` and `FUN_00559c40` consumes it. Definition placed
in `RwpBuildExterns.cpp` (`return *world`, extra args discarded); the `UtilLeaves_ab6` port
remains canonical.

## 4. Build-integration (closes B5b residue)

`Collision/RwpIntegrator.cpp`, `Collision/RwpBuildExterns.cpp`, `Collision/CollisionBodyCreate.cpp`,
`Collision/PhysicsWorldBuild.cpp` added to `build.bat` (exe) + `asi_sources.rsp` (.asi).
`QhullBridge.cpp` is compiled in ISOLATION with `/I deps\qhull-2002.1\src` into per-target
`.obj` (qhull ships generic `io.h`/`mem.h`/`stat.h` that would shadow CRT headers of the other
~200 TUs) and linked into both targets.

`RwpBuildExterns.cpp` resolves the un-ported RWP externs from `CollisionBuildDeps.h`:
FUNCTIONS as RVA-forwarding thunks (faithful under the `.asi`; present-but-uncalled on the
standalone exe until the load-time build chain is wired, B5e); DATA globals as documented
placeholder storage (read-only cone/scale constants seeded with real `.rdata` values; runtime
STATE globals zero). `FUN_0055c000` is provided by the port (RwpIntegrator.cpp), not thunked.

Build result: **both targets built clean (exit 0, 2026-07-15)** — `mashed_re.exe`
(1,154,560 B) + `mashed_re_dev.asi` (719,360 B, deployed to `original\`). The 4 Collision
TUs + `QhullBridge_{exe,asi}.obj` link into both targets. Closes B5b's build-integration
residue.

## 5. Cone-table "runtime writer" for `_DAT_005cf240` — RESOLVED (there is none)

`reference_to 0x005cf240` = **exactly 1 reference: a READ at `0x00481e14`, ZERO writes** in
the whole binary. The premise "a runtime writer populates it" was FALSE. The real cause was
an **operand-size mis-read** inherited by B5b:
```
00481e14  FLD  qword ptr [0x005cf240]   ; dd 05 40 f2 5c 00 — 8-byte DOUBLE load
00481e1a  FSQRT
00481e1c  FDIVR float ptr [0x005cf23c]  ; 1.78 / sqrt(qword)
```
The 8 bytes at `0x005cf240..47` = `00 00 00 00 | 00 00 5e 40` = the double **120.0**
(`0x405e000000000000`). Ghidra typed `_DAT_005cf240` as a *float* = only the low dword (0.0),
which produced the "`_`-overlap" warning and the bogus `sqrt(0.0)→+inf→skip` story. Real:
`step = 1.78 / sqrt(120.0) = 0.16249 < π` ⇒ the cone-build loop RUNS and fills ~120 dirs
(cap `0x78` at `0x00481eb0`), matching B5b's captured n=117. **Fix applied** in
`CollisionBodyCreate.cpp` (inline `const double = 120.0`; retired the mis-typed float extern).
`_DAT_006ce818` (cone count) is written ONLY by `FUN_00481e00` itself (`0x00481f08`), no
external writer. `_DAT_005cf238` (`0x00481f40 FMUL m32fp`) is a genuine 4-byte float denormal
`0x047a0001` — a real (if odd) constant, not a mis-read; left as-is (type-1 shape scale).
⇒ The cone table is self-populating at load; the [UNCERTAIN] is closed with no writer to port.

## 6. Acceptance — canonical-scenario run, ONLY the 8 ported hooks installed

The ported helpers call the game's real RW-math (`FUN_004c52f0/004c51a0/00546b10` via the
RVA-forwarding thunks under the `.asi`), so ON vs OFF is structurally bit-identical unless a
control-flow transcription errs. Acceptance used the CLAUDE.md-endorsed hot-path method
(behavioral observation, no Interceptor): install ONLY the 8 ported hooks and run the canonical
race.

`py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --hold 30 \
   --hooks 0x0055dff0,0x0055b800,0x0055ac00,0x0055deb0,0x0057c210,0x0055c000,0x0055e200,0x0047ea40`

Result (2026-07-15, pid 25360, exit 0): reached **RACE RUNNING (phase 3)**; car spawned with
`grounded=4` (all 4 proxy bodies); over 30 s the vehicle was physically simulated with evolving
velocity (`[374.9,0,-1223.2] → [2322.9,-23.2,2510.9] → … → [-734.1,-2.0,-761.2]`), rounds
advanced (spawnFired 12→16), **no crash / freeze / physics regression** with the ported
coupling helpers replacing the originals. Since these hooks are the per-tick coupling surface
(FUN_0047eb30 drives them ~240×/s), a wrong transcription would visibly break driving — a clean
30 s race is strong behavioral evidence that the ports reproduce the originals.

**C-level:** the 7 previously-C1 helpers → **C2** (disassembled, shape documented, verbatim
transcription, build-verified BOTH targets, canonical-run clean). `0047ea40` stays C2. This is
NOT claimed C4: per `re/CONFIDENCE.md` C4 needs a per-field `diff-original` CSV — a no-regression
run is not a bit-diff. The C4 upgrade (per-field bit-identical body-matrix-slot diff) is naturally
delivered by **B5d**, which drives these helpers from the coupling bridge `FUN_0047eb30` and
integration-diffs the body state live. Cited here as C3-supporting behavioral evidence.

## 7. Status — B5c DONE (narrow subset)
- Scope correction ratified (0x007e9de0 = marker ring, not body state; named helpers are
  coupling-side, not the solver). Verbatim ports written + hooked (RwpIntegrator.cpp).
- Build-integration: 4 Collision TUs + isolated QhullBridge obj into build.bat + asi_sources.rsp;
  BOTH targets build clean. `FUN_0055dec0` conflict resolved.
- Cone-table [UNCERTAIN] closed: `_DAT_005cf240` is the double 120.0, no runtime writer; port bug
  fixed.
- Acceptance: canonical race clean with only the 8 hooks installed (C2 + behavioral).
- DEFERRED to a new lane: the full 12-stage `FUN_0047e9c0` RWP solver pipeline (the whole island).
- Next: **B5d** — coupling bridge `FUN_0047eb30` verbatim (drives this subset; provides the C4
  live body-state integration diff).
