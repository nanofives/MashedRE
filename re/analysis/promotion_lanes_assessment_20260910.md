# Promotion lanes — what else can move rows, and how many (2026-09-10)

Sizing pass after the first shadow-lane sweep (`shadow_lane_20260910.md`). Every number below
is measured this session from `hooks.csv`, the operand sweeps and a dry run of `shadow_gen.py`;
nothing here moves a C-level. Two items are decisions for the owner, marked **[DECISION]**.

## Where the rows are

| pool | rows | has compiled port | note |
|---|---|---|---|
| C4 | 184 | all | |
| C3 | 966 | **725** (.cpp in `file`) | 987 registered `RH_ScopedInstall` rows at C3 in the operand sweep, 818 PASS+installed |
| C2 | 3,928 | **~14** (+185 counted by the sweep) | 2,917 note-only, 982 empty `file` |
| C1 | 821 | – | |

Reading: **C2→C3 is bottlenecked on writing ports** (about 3,900 rows have no code), while
**C3→C4 is bottlenecked on evidence** (725 rows have code and no canonical-scenario diff).
The shadow lane produces exactly the second kind of evidence, so that is where the volume is.

## Lane 1 — C3→C4 through the shadow A/B  **[DECISION: rubric wording]**

Dry run of `shadow_gen.py` over the 818 PASS+installed C3 ports
(`re/parity/matchdiff_sweep_c3.csv`, regenerated offline this session):

| result | n | by subsystem |
|---|---|---|
| WOULD_GENERATE (return-value site, automatic) | **387** | render 98, audio 72, frontend 50, gameplay 41, util 35, ai 25, vehicle 16, input 9, hud 8, boot 6 |
| NEEDS_REGION (void port) | 293 | render 65, audio 49, util 46, frontend 41, gameplay 24 |
| ALREADY (site exists) | 39 | |
| SKIP | 99 | 97 are `__declspec(naked)` register-ABI ports — a C++ wrapper cannot express them |

What the lane gives per row is stronger than the Frida synthetic A/B the rubric names: the hook
is **installed** (PATCHBYTE `E9`), the inputs are the **game's own** at the real call site in a
canonical race, and the comparison is bit-exact. Memory `feedback_no_overclaiming_c_levels`
says C4 demands "a canonical-scenario run with the hook actually installed" — that is this.

The blocker is one sentence in `re/CONFIDENCE.md` L37: *"the `diff-original` skill has produced
a clean Frida CSV diff"*. The shadow report is not a Frida CSV. Options:
- (a) amend L37 to "a clean original-vs-port diff on a canonical scenario with the hook
  installed (Frida `diff-original` CSV, or `shadow_ab_report.py` CLEAN with `A/B-IS-REAL`)";
- (b) keep C4 Frida-only and treat shadow CLEAN as C3 evidence only (today's 44 stay the ceiling).
Under (a), the realistic first slice is the 387 minus audio (72): audio runs on its own thread,
and the Uninstall/Install window is only safe single-threaded (`ShadowAB.h` LIMITS). Frontend
and hud sites need `--phase menu`/`any` rather than the race gate. Expected reach is well under
100% (29 of 94 sites were unreached this session); budget 20–30 boots for the first pass.
"No stubs in the body" still applies per row and is checkable from `STUBS.md`.

## Lane 2 — decomp→port factory for the 3,900 unported C2 rows

Nothing generates a port today; the RWP island (K1–K24) was transcribed by hand from Ghidra
decomp, then verified by the very lane that now exists. The pieces for a factory are present:
`re/tools/decomp_pc.py` (headless batch decomp, no MCP), the plate notes (shape, args, globals),
`shadow_gen.py` (verification wrapper), `shadow_batch.py` (evidence). Missing: the transcriber
(`decomp2port.py`: Ghidra C → MSVC-compilable verbatim C++ with `undefined4` typedefs, absolute
globals, `extern "C" __cdecl` + `RH_ScopedInstall`, raw-RVA thunks for unported callees), and a
**reachability filter** so nobody ports code the canonical race never runs (113 of 210
pre-screened candidates were `never`). Cost per row after the tool exists: one Sonnet
transcription + a build + a share of a boot. Yield expectation from the pre-screen: ~1/3 of
candidates reachable in a race; return-value leaves verify automatically, void ones need a
region.

## Lane 3 — remove the region/idempotency problem: page-level write tracking

Both `Run()`'s idempotency blind spot (7 of 10 DIVERGENT this session) and `RunRegion`'s need
for a hand-verified span (293 + 55 void ports) go away with one mechanism: before running the
original, mark all committed writable pages read-only, catch write faults in a vectored
exception handler (record page + pre-image, re-enable, continue); after the call, the touched
set IS the region. Restore it, run the port under the same tracking, compare touched pages.
No spec, no idempotency assumption, and it also covers the frontier's 43 MUTATOR_LANE rows.
Costs: protection flips over tens of MB per sample (ms each; 48 samples is fine), must exclude
the current stack and the harness's own pages, other threads' writes show up as noise (audio is
already off under `MASHED_MUTE=1`). Roughly a day of work in `Core/ShadowAB.h`; no new
dependency. Precondition for scaling Lane 1 into the 293 void C3 ports.

## Lane 4 — cheap wins already possible

- **Phase gate for non-physics sites.** Everything generated so far gates on `kPhaseRace`; the
  frontend/util/hud C2 sites that read NO_SAMPLES may fire at menu/load. Regenerate those with
  `--phase any` and re-boot: 1 boot, up to ~10 rows.
- **RunRegion re-test of the aliasing pair** (`0x00577be0`, `0x00577cb0`) and the promotion of
  the 5 gate-unchecked region CLEAN rows (kickoff A/C).
- **Scenario reach**: `--boost`, `--mode 2`, driving input for the 29 unreached sites.

## Lane 5 — coverage instrument (TT-4)  **[DECISION: new tool]**

A DynamoRIO `drcov` run of the original over the canonical scenarios would rank all 3,900
unported C2 rows by "executes in a race", replacing the per-24 Interceptor pre-screen (one boot
per chunk, 160 boots for the pool). DynamoRIO is **not installed**; installing it is a new
tool decision. Unicorn 2.1.4 *is* installed (Python), which makes TT-3 emulator-differential
feasible for teardown/one-shot rows without asking, but Lane 3 covers most of the same ground
in-process and cheaper.

## Recommendation

1. Decide Lane 1's rubric wording. If (a): generate the 315 non-audio C3 sites, run
   `shadow_batch.py` with `--cars 4 --hold 60` in two phase groups, promote CLEAN rows to C4
   through `re-classify` with the same transaction shape as today's.
2. Build Lane 3 (page-level tracking) next — it unblocks 348 void ports across C2/C3 and
   retires the idempotency caveat.
3. Then Lane 2, gated by a reachability filter, is the only route that grows C3 by hundreds.
