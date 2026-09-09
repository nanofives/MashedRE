# TT-11 shadow A/B — first live run, and how it was proven non-vacuous (2026-09-09)

`Core/ShadowAB.h` generalises the hand-written B5c in-process A/B. This note records the
first armed run of the generalised header and, more importantly, **why "48/48 OK" is not a
vacuous result** — which took two attempts to establish.

## The run

    # one hook installed, ShadowAB armed, warped into a live race
    MASHED_SHADOW_AB=1 py -3.12 re/frida/scenario_launch.py --hooks 0x0057c210 --hold 15

`original/shadow_ab.log` (the log lands in the process CWD, matching the 13 existing
`original/*_selftest.log` files):

    [--] fn=RwpBodyTableLookup PATCHBYTE installed=e9 uninstalled=8b A/B-IS-REAL
    [0]  fn=RwpBodyTableLookup rva=0057c210 idx=1075 ndiff=0 OK
    ... 48 samples, all ndiff=0 ...

**48 samples, 48 bit-identical, 0 SKIP.** `idx=1075` shows the hook was found in the registry.

## Why the first "48/48 OK" was NOT yet evidence

If `HookSystem::Uninstall` had silently failed, the "call the original" step would have
re-entered **our own inline-JMP**, and the harness would have compared the port against
itself. That is trivially equal — a perfect, meaningless 48/48. The sample count and the
hook index do not rule it out.

### Attempt 1 — corrupt the port. FAILED, and the failure is worth keeping.

Injected `+1` into `RwpBodyTableLookup_impl` and re-ran, expecting `ndiff=1` on every sample.
Result: **no log at all and no race**. The function returns a physics body pointer, so `+1`
misaligns it and the solver dies before the first sample is ever written. A value-corruption
control is unusable for this function class — it kills the process it is meant to measure.
(Weak side-evidence that the port's output really does flow into the game, but nothing more.)

### Attempt 2 — read the patch byte inside the window. DECISIVE.

The header now samples the byte at the RVA on both sides of the uninstall, once per function:

    installed=e9      our inline JMP is in place
    uninstalled=8b    the original prologue is back

Cross-checked against the anchored binary: `original/MASHED.exe.unpatched` at `0x0057c210`
begins `8b 44 24 04 8b 0d d8 c8` — first byte **0x8b**, exactly what the uninstalled window
observed. So the "original" call demonstrably executed the ORIGINAL code.

The check is cheap, harmless, and now runs on **every** function on its first sample, so every
future shadow-A/B log carries its own proof-of-realness rather than relying on this one note.
A `SUSPECT-no-restore` line appears instead if the bytes ever say otherwise.

## What the result does and does not say

- **Does:** on 48 live in-race calls with real arguments and real state, our
  `RwpBodyTableLookup` returned bit-identical results to the original, and the comparison was
  demonstrably against the original.
- **Does not:** 48 is the per-function sample cap (`kDefaultSamples`), not the call count — the
  function fires far more often, so this is a sample, not exhaustive. One function, one
  scenario, one track. `.asi` only, so it says nothing about standalone runtime
  (memory `feedback_c4_verifies_logic_not_standalone`). And per the lane's standing limits, a
  clean shadow A/B is promotion *evidence* to be weighed by `re-classify`, never an automatic
  C-level.

## Cost, which is the point of TT-11

Adding this function to the lane is now:

    SHADOW_AB_COUNTER(ab, "RwpBodyTableLookup", 0x0057c210u, ShadowAB::kPhaseRace);
    return ShadowAB::Run(ab, RwpBodyTableLookup_impl, key);

Two lines, against ~13 lines of hand-rolled arm/uninstall/compare/log per site before. No
Frida, no `arg_type`, no synthetic call, no per-function boot — and, given the Defender ASR
block on TTD recording, no external instrumentation binary either.
