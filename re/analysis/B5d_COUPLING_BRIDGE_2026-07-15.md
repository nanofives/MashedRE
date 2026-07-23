# B5d — coupling bridge FUN_0047eb30 verbatim port + live drive of the B5c subset — 2026-07-15

Lane B5d (ROADMAP §WS-B, D1 Option A; RE_MASTER_PLAN §7 item 10). Inherits B5a/B5b/B5c
(`B5c_RWP37_INTEGRATOR_SUBSET_2026-07-15.md`). Ghidra pool0, read_only, anchor
`MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`.
NO-GUESSING: every constant/offset/signature below cites the live Ghidra address.

## 1. What landed

`Vehicle/VehicleCouplingBridge.cpp` — clean-room **verbatim** transcription of the once-per-tick
chain→rigid-body coupling driver `FUN_0047eb30` (`0x0047eb30..0x0047f1db`, ~1700 b). Built into
BOTH targets (added to `build.bat` + `asi_sources.rsp`); `mashed_re.exe` 1,157,120 B +
`mashed_re_dev.asi` 721,920 B, exit 0, no warnings on the TU (`log/b5d_build_2026-07-15.log`).
Registered runtime-toggleable via `RH_ScopedInstall(VehicleCouplingBridge, 0x0047eb30)` (inert on
the exe via HookSystemNoOp; inline-JMP under the `.asi`).

Full body of the driver (verbatim, from the live decomp + full disassembly
`0x0047eb30..0x0047f1db`):
1. `if (DAT_006ce274==0) return false` (world-present gate, 0x0047eb45).
2. `if (FUN_0040e350()!=6) _DAT_006cad48=0` (0x0047eb55).
3. **One-shot build gate** `if (DAT_0086caa0==0x7b)`: `u=FUN_00426060()`; if
   `DAT_00773920==1 || (4<DAT_00773920<7)` → `FUN_00562460(*(int*)0x0080332c)`;
   `FUN_0047d3c0(DAT_006ce274,u)`; **`DAT_0086caa0=0` (cleared at 0x0047eba7)**.
4. dt clamp: `if (_DAT_005cc9a0 < DAT_0061331c) DAT_0061331c=0.05` (0x3d4ccccd, 0x0047ebad).
5. `_DAT_006cad48++`; `_DAT_007f0f8c=(float)(FUN_0040e340()+5000)` (0x1388).
6. **Per-car forward loop** (cursor `p=&DAT_00881f68`, stride 0x341 floats = 0xd04 b, index `i`,
   until `(int)p >= 0x00885378`): live gate `p[-0x26d]==0 && i<FUN_0040e340()`; body =
   `FUN_0057c210((&DAT_006c9a78)[i])`; PD-spring writes body linVel (gain `_DAT_005ccd6c`=20.0,
   lookahead `_DAT_005cf014`=0.0002 → net 0.06 s); heading-align writes body angVel (atan2→deg
   `FUN_004233e0`, acos `FUN_004a3384`, `_DAT_005cc98c`=57.2958); commit `FUN_004c45f0`/
   `FUN_004c0e50`/`FUN_0055dff0(body,0)`/`FUN_0055ac00(*(body+0x24),body,1)`; mode 3|5 teleport
   body←render; `i≥playerCount` drops body `+0x34 = 0xc61c4000` (−10000.0).
7. **Vendor solver step**: `ctx=FUN_0055deb0(DAT_006ce274,DAT_0061331c)`; `r=FUN_0047ea40(ctx)`.
8. **Readback loop** (cursor `q=&DAT_008816d8`, stride 0x341, index `j`, until
   `(int)q >= 0x00884ae8`): gate `q[-0x49]!=0`; transform `−CoM·1.0` by body matrix; subtract
   from body-matrix translation; `FUN_0046d4d0(j, buf)` → BOTH render buffers.
9. `return r != 0` (SETNZ, 0x0047f1cd).

All 20 callees are dispatched to their **real RVAs via thunks**. Under the `.asi` the bridge
therefore calls the real game helpers; and because the B5c hooks install an inline-JMP at
`0x0057c210 / 0x0055dff0 / 0x0055ac00 / 0x0055deb0 / 0x0047ea40`, the bridge's RVA calls to those
addresses transparently **route into our B5c ports** when the B5c hooks are installed — the C4
exercise. Everything stays runtime-toggleable.

## 2. NO-GUESSING corrections carried by this port

1. **Readback gate is `record+0x14`, NOT `+0x1c`.** `vehicle_coupling.md:95` said the readback
   half runs "per car where +0x1c != 0". The disasm at `0x0047f0e0` is
   `MOV EAX,[EBP+0xfffffedc]; TEST EAX,EAX; JZ` with `EBP=0x008816d8` ⇒ gate address
   `0x008816d8-0x124 = 0x008815b4 = record+0x14` — the SAME live gate the forward loop reads at
   `0x0047ebf3` (`[EDI-0x9b4]`, `EDI=0x00881f68`). Polarity is **opposite**: forward processes
   iff `==0`, readback iff `!=0`. Fixed in the port; `vehicle_coupling.md` line 95 to be amended.
2. **`FUN_0055deb0` + `FUN_0047ea40` ARE called inside the bridge** (after the per-car loop,
   `0x0047f0c0`/`0x0047f0c9`) — resolves the worker-brief `[UNCERTAIN]` seam. Confirmed live.
3. **Pre-branch-arg resolutions** (all from disasm, [[feedback_ghidra_prebranch_args]]): the four
   render helpers are 2-arg — `FUN_0046d4a0(out*,idx)`, `FUN_0046cb30(out*,idx)`,
   `FUN_0046d510(out*,idx)`, `FUN_0046d4d0(idx,src16*)` (decompiled `0x0046d4a0/cb30/d510/d4d0`).
   `FUN_004c3df0` is always 4-arg `(dst,src,count,mtx)`. `FUN_004c45f0(mtx)` / `FUN_004c0e50(int)`
   are 1-arg. Ghidra dropped these as no-arg where the args were pushed before a shared branch.

## 3. Acceptance — canonical QuickRace, bridge + the 8 B5c hooks installed (behavioral / C3)

`py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --hold 35 \
   --hooks 0x0047eb30,0x0055dff0,0x0055b800,0x0055ac00,0x0055deb0,0x0057c210,0x0055c000,0x0055e200,0x0047ea40`

Result (2026-07-15, pid 30268, exit 0, `log/b5d_canonical_bridge_2026-07-15.log`): reached
**RACE RUNNING (phase 3)**; car spawned `grounded=4` (all 4 proxy bodies); over 35 s the vehicle
was physically simulated with evolving velocity (`[358.5,0,-1302.2] → [-83.7,0.2,331.1] →
[2328.2,-23.2,2478.8]`) then the round-advance idle pattern (`grounded=0/vel=0`) — identical shape
to B5c's acceptance, **no crash / freeze / physics regression**. Because `FUN_0047eb30` is the
per-tick coupling surface and now drives the 8 B5c helpers ~240×/s through their inline-JMP hooks,
a wrong transcription of the PD law would visibly break driving — a clean 35 s bridge-driven race
is strong behavioral evidence the port reproduces the original.

**C-level (honest, no overclaim):** this is a **canonical-scenario run with the hooks installed
and driving the subset** → supports **C3** for the bridge `0x0047eb30` and the 8 B5c helpers
(behaviorally-verified integration). It is **NOT C4**: per `re/CONFIDENCE.md` C4 needs a per-field
`diff-original` CSV (bit-identical body-state), not a no-regression run. The per-field diff is the
remaining B5d step (§4).

## 4. Remaining for C4 — per-field body-state integration diff (next focused session)

Deterministic A/B: run the canonical race twice with the launcher's drive injector (`--spike-
telemetry` forces accel/steer), (A) stock original (no `--hooks`) vs (B) bridge + 8 B5c hooks, and
per-field diff the proxy-body state read via the pointer-chain
`body=FUN_0057c210((&DAT_006c9a78)[i]); linVel=*(*(*piVar2+0x10)+8)+piVar2[1]*0x20 [0..2];
angVel=same+0x10; matrix=*(*(body+0x10)+8)[0xc..0xe]` (NOT `0x007e9de0`). Requires adding a direct
body-state reader to the scenario agent + replay-deterministic inputs (`project_replay_deterministic_clock`).
Target bit-identity; the linear PD half is float-only and expected bit-identical; the **angular
half** (atan2→deg + acos, kept 80-bit in ST0 across a cancelling subtract in the original — MSVC
has no 80-bit C type) carries the accepted **≤1-ULP → few-ULP** x87 floor per field, to be
measured and documented with addresses.

## 5. STOP-AND-ASK — the deferred 12-stage solver island (FUN_0047e9c0) sizing

`FUN_0047ea40 → FUN_0047e9c0` is the deferred RWP world step. `B5e CANNOT ship on RVA thunks`
(the standalone calls no MASHED code by construction), so this island must be clean-room ported.
Sizing (transitive static-call closure from `0x0047e9c0`, Ghidra walk 2026-07-15):

- **137 first-party physics functions, ~80.7 KB** of code (excluding the 2 non-physics
  marker-emitter passes `FUN_0047d640`/`FUN_0047def0`, 4.9 KB). **Near-zero library content**
  (1 band-hit, 34 B) — it is essentially all bespoke RWP code.
- This is a **lower bound**: the solver dispatches shape support/contact virtuals **indirectly**
  (vtable, e.g. `FUN_0055c000` GJK support), which a static-call closure does not capture — the
  real port surface is ≥137 fns / ≥81 KB.
- 12 direct stages: markers ×2 + `FUN_0055e200/0055fe50/0055fea0/0055ff70/0055ff90/00561040/
  00561390/00561c50/00561e60/00561e80`. Anchors: broadphase `FUN_0055a1f0` (2.0 KB), island
  partition `FUN_00560260` (3.5 KB), CCD/impact `FUN_00561390` (2.2 KB), constraint core
  `FUN_00570090` (**10.5 KB** — the single largest), plus `FUN_00576880` (5.0 KB),
  `FUN_005729a0` (3.1 KB), `FUN_00578ff0` (2.9 KB).

**Options (architecture gate — for the USER to decide, not this lane):**
- **(A) Clean-room the full island** — bit-identical RWP solver. ~137+ functions / ~81+ KB; by far
  the largest single-lane port in the project (comparable to the entire renderer effort). Buys true
  C4 collision/contact physics standalone.
- **(B) Reduced-but-faithful solver subset** — port integrate + a simplified contact resolution
  that reproduces grounded driving without the full LCP/CCD/partition island (the philosophy the
  standalone `VehiclePhysicsRun.cpp StepCar` reduction already uses, `vehicle_coupling.md:118-139`).
  Much smaller; **not bit-identical** → behaviorally-faithful (C3-class) only.

## 6. Status — B5d PORT done; C4 diff + solver gate open
- Verbatim `FUN_0047eb30` ported + built into both targets + hooked; canonical race clean with the
  bridge driving the 8 B5c hooks (behavioral / C3).
- Corrections: readback gate `+0x14` (not `+0x1c`); solver calls confirmed in-bridge; pre-branch
  args resolved.
- OPEN (next session): the per-field bit-identity body-state diff → C4 (§4).
- STOP-AND-ASK: the FUN_0047e9c0 solver-island architecture gate (§5) — sized, awaiting the user's
  A-vs-B decision before B5e.

## 7. C4 mechanism realized + the thunk-callee gate RULING (2026-07-23, account3)

The §4 per-field body-state diff was realized as an **in-process bit-identity self-test**
(the A4 `PhysicsChainHooks` pattern, but reaching the original via `HookSystem::Uninstall`→
call-RVA→`Install` — single-threaded physics, no bespoke naked-asm). `Collision/RwpIntegrator.cpp`
now carries a `_impl` + self-test wrapper for the 6 A/B-able B5c fns, gated on session-phase
`0x00771968 == 3` (race-only; the leaf helpers also fire during load-time world build
`FUN_00481e00`, where the Uninstall/Install churn is unsafe pre-spawn).

**RESULT so far:** `RwpBodyMatrixRefresh` (0x0055b800) = **64/64 `ndiff=0` GREEN** (0x40 world-
matrix slot + 0x10 quaternion slot bit-identical vs original, canonical QuickRace, inline-JMP
live; commit 63d10c5c, `log/phys_c4_b5c_matrixrefresh_GREEN.log`). The 5 remaining leaves are
built + gated (commit cd1d5f14) but await a fresh GREEN run — this session's boot state degraded
after many spawn/kill cycles (`project_boot_hang_directshow_intro`; needs a reboot).

**RULING — RVA-thunk callees do NOT block C4** (asked+decided 2026-07-23):
A port reaches C4 even though its body calls helpers via RVA thunks (`RwpBuildExterns`) **iff the
callee is identified + mapped + C2+ classified** (a real reversed function), NOT an unreversed
placeholder. Grounds: (1) CONFIDENCE.md L32 C4 gate = "clean Frida CSV diff on a canonical
scenario; No stubs in the implementation" — a *stub* (STUBS.md) is a placeholder into a
NOT-YET-REVERSED fn; `RwpBodyMatrixRefresh`'s callees `FUN_004c52f0` (RwMatrixCombine, render C2),
`FUN_004c51a0` (RwMatrixTranslate, render C2), `FUN_00546b10` (vehicle C2) are all mapped+ported+C2,
so the gate is met; (2) the in-process A/B *exercises* the callees (both orig and port call them),
so the port's control-flow + arg-marshaling is verified bit-for-bit; (3) precedent: A4
`VehicleControlUpdate` 0x00470670 is C4 with RVA-thunk callees in its body; (4)
`feedback_c4_verifies_logic_not_standalone` — C4 attests .asi logic bit-identity, standalone
callee-reachability is a separate track.

**Scope of the ruling:** unblocks C3→C4 for `RwpBodyMatrixRefresh` + the 5 pure-leaf B5c fns
(leaves have no callees → trivially clean body) **once each is GREEN-verified**. **Excluded** (stay
C3, different reasons): `RwpSceneStepWrapper` 0x0047ea40 (tail-calls the DEFERRED, unreversed solver
`FUN_0047e9c0` — that IS an unreversed-island call, and it is not a clean per-frame A/B) and
`FUN_0055c000` (load-time-only, not per-frame A/B-able). A callee that is an unmapped/C0 placeholder
WOULD block C4.
