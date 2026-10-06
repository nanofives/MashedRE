# U-9186 measure-inert-first: porting the two early-return branches would be INERT — do NOT wire it

**MEASURED 2026-10-06.** No game run, no build, no code change, no C-level moved, `original/`
untouched. Static analysis of the committed port plus original decompilation
(`re/tools/decomp_pc.py` against a read-only pool slot; master Ghidra not opened).

## The task and the outcome

U-9186 says the surviving AI over-speed is a COMMAND defect: the original lifts/brakes on
149 of 660 window calls where the port holds full throttle, via three unported branches of
`FUN_00416250`. The two **portable** ones (the mode-7 branch is structurally blocked — no
world-object list) are:

- **BRANCH 1** — `FUN_00414a70 == 2` → `ctrl[4]=0; ctrl[5]=0xff; return;` (lift + brake),
  36 of the 149 calls. Original site `0x00416405`.
- **BRANCH 2** — `FUN_004148b0 != 0 && FUN_00416060 != 0` → `ctrl[5]=0xff; ctrl[0]=0;
  ctrl[1]=0; return;` (brake + zero steer), 64 of the 149 calls.

**The measure-inert-first step says: wiring either branch now would ship dead code.** Both
read per-car race-progress / rank state that the standalone never produces. Verbatim
transcriptions would read zeroed image-pad memory and either never fire or fire on garbage.
**No code was written** — this is the refusal the discipline exists to produce.

## The evidence, each input traced to "no standalone writer"

The standalone VirtualAlloc-maps `0x00500000..0x009fffff` as **blank zeroed** memory
(`exe_main.cpp:518-519`), so every original global below reads **0** unless the port writes
it. None of them is written:

| input | read by | standalone writer | value in standalone |
|---|---|---|---|
| `0x008989b0[v]` (per-car reference distance / progress) | `RefDist` = `FUN_00442cc0`, gating BRANCH 2's chain | **none** — producer `FUN_00442a60` (`Spectator::ComputeDistances`, **C2, unported**) | **0** |
| `0x008a96e8 + v*0x30c` (per-car progress block) | `FUN_00408a50` = closest-in-race input (BRANCH 1) | **none found** | **0** |
| `0x0089a364` (leader index) | `FUN_004148b0` (BRANCH 2) | **never written** (only a const def in the asi-only `AiLeaderTimer.cpp:50`) | **0** |
| `0x0089a4c4 / 0x0089a4c8 + v*0x74` (catch-up counters) | `FUN_004148b0` | **only zeroed** in `AiStandalone.cpp:1578-1581`, `:1747`; the incrementer is `FUN_004148b0` itself (asi-only) | **0** |

**The port already documents this**, verbatim at `AiStandalone.cpp:704-709`:

> `// Standalone inputs with no standalone writer, read from the image-pad exactly where`
> `// the original reads them (so 0 here) …`
> `//   0x008989b0[v] (FUN_00442cc0; written by the unported FUN_00442a60),`

And the gate helpers themselves are not even in the shipping binary:
`FUN_00416060` (`AiTargeting.cpp` / `AiLineOfSight.cpp`), `FUN_004148b0` (`AiLeaderTimer.cpp`),
`FUN_004150e0` (`AiWallLateral.cpp`) are all in **`asi_sources.rsp` only**; `FUN_00414a70` and
`FUN_00414c30` are C2 doc-only (no `.cpp`). The orchestrator `ControlStep` hardcodes
`int mode = 0` and stubs the whole targeting chain (`AiStandalone.cpp:828, 844, 846`), and the
host's `ai_target_enable()` is a literal `return 0;` stub (`TrackRenderer.cpp:103`), so the
branches are not reached at all today.

### What each branch would actually do on the zeroed state (so "inert" is precise, not vague)

- **BRANCH 2:** `FUN_004148b0` reads `RefDist(v) == 0` for every car, the leader index `0`,
  the catch-up counters `0`; its catch-up accumulate path is gated on
  `_DAT_005cd0a8 < RefDist(leader)` with `RefDist(leader) == 0` → false → **returns 0 → branch
  never fires.**
- **BRANCH 1:** `FUN_00414a70` computes "closest in race" from `FUN_00408a50(car)` = 0 for all
  four cars → every car is equidistant at 0 → the selection is degenerate and does not return
  the `2` that fires the brake. **Does not reproduce the original's 36 calls.**

The `s_host` AI interface (`AiStandalone.h:36-64`) exposes **no** opponent-progress, rank,
leader, or catch-up accessor — only own position/velocity/heading, `veh_type`, `veh_f32`,
`held_powerup`. So there is nothing to feed the gates even by rebinding.

## The prerequisite, named — this is where a real fix starts

The branches cannot be made live by "wiring the TU". The upstream requirement is a
**per-car race-progress producer** in the standalone:

1. Port `FUN_00442a60` (`Spectator::ComputeDistances`, C2) — it writes the per-car reference
   distances at `0x008989b0`, which `RefDist`/`FUN_00442cc0` and the whole LeaderTimer /
   closest-in-race chain read. Today it is unported and the array stays zero.
2. With live progress at `0x008989b0`, re-run the inert check: do `FUN_00414a70` /
   `FUN_004148b0` then return their firing values at a rate approaching the original's 36 / 64?
   (A default-OFF counter on each gate, no `ctrl` change, before any behaviour wiring —
   memory `count-it-before-designing-a-witness`, `zero-of-n-needs-a-coverage-check`.)
3. Only then port the two branches, behind a default-ON knob, gated on (e)/(b)/over-speed.

The catch-up counters (`0x0089a4c4/4c8`) are self-produced by `FUN_004148b0` once it reads
live progress, so they are not a separate prerequisite; the leader index `0x0089a364` has its
own producer to trace if BRANCH 2 is pursued.

## Bottom line

U-9186's over-speed is real and command-caused, but its fix is **blocked on an unported
upstream producer** (`FUN_00442a60` → `0x008989b0`), not on wiring the branches. Wiring them
now would add inert code to the shipping binary that reads zeroed image-pad state — the exact
anti-pattern `ROADMAP.md:235-240` and the ASI-only triage warn against. **No C-level moved, no
code shipped; the finding is the refusal plus the named prerequisite.**
