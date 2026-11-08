# RESULT PORT-RWP-01: RW-Physics 3.7 reachability + matched swaps (2026-11-08)

Branch `orch/PORT-RWP-01` off `orch/integration` `ea666edab`. Worker: claude2 L2. Lane: PLAN.md section 8 (L-98).

## Verdict

**The exe's default race path executes no RenderWare-Physics-3.7 body.** `called 0` is correct, not a scanner miss.
The exe does not run the original's RWP proxy-body world at all. Its race physics is its own vehicle model
(`VehiclePhysics_StepCar` / `VehiclePhysics_StepCarsOuter` < `UpdateCar` < `RenderFrame` < `WinMain`), which ports
`FUN_00470c70` from `0x00470e1a` on and leaves out the `CALL FUN_0047eb30` at `0x00470e15`
(`Vehicle/VehiclePhysicsRun.cpp:1006` lists StepBegin's coverage as `0x00470e1a..0x00470f16`).

So **0 of the 16 matched-source rows were swapped**. The brief limits swaps to rows the default path executes, and a
swap of an unreached body would not move the port column. No file under `mashedmod/` changed, so the default gates
were not re-run (see Gates).

## 1. Reachability (REACH.tsv, ABS_CALLS.tsv)

Method: `reach.py` imports `scripts/backfill_port.py`'s own graph (rel32 call/jmp onto map symbols + every base
relocation), symbol resolution and WinMain walk, then adds the reverse side: the exe callers of each body (CRT
initialisers excluded, that is where the no-op `RH_ScopedInstall` registrations sit) and the tops of each body's caller
closure. `abs_calls.py` covers the edge class that graph cannot see: a dword literal in `.text` equal to an RWP RVA
(`reinterpret_cast<fn>(0x0057c210u)(...)`), which targets the original's `.text` and faults in the exe if it runs.

Build: this worktree, `mashedmod\build.bat` with `MASHED_NO_DEPLOY=1`, Build OK, rva-lint OK (1 known, 0 NEW).

| RWP rows with an exe body | 129 (73 of them C4) |
|---|---|
| reached by WinMain | **0** |
| unreached, resolved to a map symbol | 126 |
| unresolved (no map symbol, no reachable enclosing caller) | 3 (`0x0055deb0`, `0x0055e200`, `0x00563840`) |
| RWP RVA literals in exe `.text` | 151 (RVA, holder) pairs, **0** holders reached by WinMain |

What each row does in the original (column `original_job`, all 129 rows):

| original job | rows | exe entry for that job | reached? |
|---|---|---|---|
| solver island: `FUN_0047eb30` -> `FUN_0047ea40` (`0x0047f0c9`) -> `FUN_0047e9c0`, 12-call stage sequence | 101 | `RwpSceneStepWrapper` (MatchS13, 0x0047ea40) -> `FUN_0047e9c0` (RwpSolverCore23) | no: `RwpSceneStepWrapper`'s only reference is its initialiser |
| bridge helpers, called from `FUN_0047eb30` per tick | 4 | `VehicleCouplingBridge` (0x0047eb30) | no: only its initialiser references it |
| world build, `FUN_0047d3c0` (load time, at `0x0047eb9f` inside the bridge) | 1 | none (`0x0047d3c0` is C2 with no exe body) | - |
| no exe caller and no relocated exe pointer to it | 21 | - | no |
| unresolved | 2 | - | - |

The chain is cut twice in the exe, either cut is enough:
1. Nothing in the exe frame calls `VehicleCouplingBridge` (the exe's `FUN_00470c70` analogue skips `0x00470e15`).
2. `VehicleCouplingBridge_impl` reaches the RWP world through absolute original addresses, not exe names:
   `0x0055deb0`, `0x0047ea40`, `0x0047d3c0`, `0x0055dff0`, `0x0055ac00`, `0x0057c210`, `0x00562460`, plus game/RW
   callees `0x0040e340 0x0040e350 0x00426060 0x0046cb30 0x0046d4a0 0x0046d4d0 0x0046d510 0x004233e0 0x004a3384
   0x004c0e50 0x004c3df0 0x004c45f0`. None is one of the 6 RVA thunks (`exe_main.cpp:9727`), so the body would land in
   unmapped or zero-filled pages if it ran. And the world `DAT_006ce274` is never built in the exe (zero-filled .data, no `FUN_0047d3c0` body).

### Scanner (backfill_port.py): no missed edge on the default path

- The absolute-literal edge class is real but every holder is unreached (ABS_CALLS.tsv), so `called 0` stands.
  Proposal (not applied): a `--abs-literals` report in backfill_port.py that lists reached symbols holding a literal
  equal to a hooks.csv RVA outside the 6 thunks. On today's tree it would list nothing for RWP; it would catch a future
  wiring that runs an absolute call in the exe.
- Inlined extern wrappers show as caller-closure tops (`FUN_00561390`, `FUN_0055ff70`, ...): `/O2` inlines the extern
  `FUN_x` wrapper into `FUN_0047e9c0_impl`, which then passes `&FUN_x_impl` to `ShadowAB::RunTracked` (a relocation
  edge). The island is connected inside the exe, it just has no entry from WinMain. No verdict changes.
- 2 C4 rows resolve to an `_impl` name with no map symbol (`0x0055deb0` -> `RwpWorldSolverHandle_impl`,
  `0x0055e200` -> `RwpSolverContextSet_impl`), while the `RH_ScopedInstall` names in the same TU
  (`RwpWorldSolverHandle_RegAbi`, `RwpSolverContextSet`, `RwpIntegrator.cpp:475/480`) do have symbols, both unreached.
  Proposed fix: in `resolve()`, when the scanned name ends in `_impl` and has no symbol, also try the name registered
  with the same RVA by `RH_ScopedInstall` in that TU. Verdict unchanged (`body`).

## 2. Swaps (swaps.tsv, relocheck.tsv)

All 16 matched sources (`re/match/rwl05`, `re/match/rwl06`) pass `re/tools/match/relocheck.py` (OK, 16/16). None
swapped:

| reason | rows |
|---|---|
| NOT-REACHED (no WinMain path) | 16/16 |
| hand body also carries a ShadowAB harness | 15 (all but `0x0055bd70`) |
| hand body is `__declspec(naked)` asm (`RwpSolverBroadphase3.cpp:604`, tail JMP) | 1 (`0x0055bd70`) |

They become swappable once the RWP world is wired into the exe (decision below). The ShadowAB wrappers would go with
the hand body in that swap.

Flip candidates: none (no swap made).

## 3. Port column before / after

`py -3.12 scripts/backfill_exe_file.py` (0 changes) then `py -3.12 scripts/backfill_port.py --check` (rows changed 0):
whole tree none=5311 body=587 called=253. RWP C4 rows: body 73, none 99, called 0, before and after.

## 4. Gates

Not run: the branch changes no file under `mashedmod/` (diff = `verify/portrwp_20261108/` only), so the exe is built
from the same source as `orch/integration` `ea666edab`. Build OK and rva-lint OK were run on this worktree.

## 5. Decision for the lead (blocks PORT-RWP-02 too)

To make any RWP row `called`, the exe must run the RWP world, which it does not today:
- call `VehicleCouplingBridge` from StepBegin at the `0x00470e15` position (before `0x00470e1a`),
- bind the bridge's absolute callees (list in section 1) to exe names, including `0x0047ea40` ->
  `RwpSceneStepWrapper` and `0x0055deb0` -> `RwpWorldSolverHandle`,
- port the world build `FUN_0047d3c0` (C2, no exe body, qhull path, L-73) so `DAT_006ce274` exists.

This changes behaviour: the bridge's readback writes each car's render matrix from the proxy body
(`FUN_0046d4d0` at `0x0047f1a1`, "both render buffers"), so car pose on screen, and anything reading those buffers,
would come from the RWP world instead of the exe's own model. Recommendation: a lane PORT-RWP-WIRE behind a default-OFF
knob with a PREREG, gated per step, and its default flip as a decision row. PORT-RWP-02 ("the 98 none rows the exe path
needs") has no rows today: the exe path needs none of them until that wiring lands.

## Files

- `REACH.tsv` (129 rows), `reach.py`
- `ABS_CALLS.tsv`, `abs_calls.py`
- `swaps.tsv`, `swaps.py`, `relocheck.tsv`
- Raw: `D:/MashedWT/PORT-RWP-01_data/build0.log` (baseline build log), SHA-256 f24c592af884ab08b0247f0ba10c55baab9fefaaf916d9769f95943f532c5a60.
