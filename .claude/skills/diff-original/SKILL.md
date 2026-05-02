---
name: diff-original
description: Run a Frida-driven side-by-side trace of reversed-vs-original behavior to verify a hook reproduces the original function. Use after the hook-author skill has produced an implementation, or any time the user asks to "verify the hook", "diff against original", "trace this function", "is my reimplementation correct".
---

# diff-original — Frida behavioral diff

Verifies that a reimplemented function in `mashedmod` produces observably identical behavior to the original at the same RVA. Inspired by TD5RE's `frida_race_trace.js` workflow.

## Concept

For a hook at RVA `0x00xxxxxx`:

1. Attach Frida to a clean run of `original\MASHED.exe` (no DLL injected).
2. Place an `Interceptor.attach` at `module.base + 0x00xxxxxx` that logs `args[0..N]`, key memory reads, and the return value to `log/diff_original_<RVA>.csv`.
3. Stop. Inject the modded DLL and run the same scenario.
4. Place the same Frida instrument at `module.base + 0x00xxxxxx` (now jumps into `CFoo::Bar`); log to `log/diff_modded_<RVA>.csv`.
5. `diff -u` the two CSVs. Identical = hook accepted. Diverging row = first regression — investigate.

## Hook invocation

The Frida script lives in `re/frida/diff_function.js` (template). When invoked, the skill:

1. Asks the user (or infers from context) the RVA, function signature, and which arguments to dump.
2. Generates a tailored copy under `re/frida/diff_<sym>.js`.
3. Runs **two** Frida sessions, one against original, one against modded build, with the same scripted inputs (lap/track/menu navigation).
4. Diffs the resulting CSVs and reports first divergence with line numbers.

## Standard scenarios

Pre-canned input scripts (to be added under `re/frida/scenarios/`):

- `intro_skip.js` — mash through intros to main menu
- `single_race.js` — auto-launch a 1-lap race on a fixed track/vehicle, dump per-frame state
- `frontend_walkthrough.js` — exercise menu nav for HUD/font hooks

## Trust hierarchy

1. Frida output of the **actual original binary** — ground truth.
2. Ghidra decompilation — usually correct but sometimes loses semantics.
3. Reversed source — the thing being verified, never the reference.

If Frida and Ghidra disagree, Frida wins.

## When NOT to use

- For pure refactoring of already-verified hooks → no diff needed.
- For build/compile errors → fix the build first.
- For functions with non-deterministic output (RNG, time-based) → seed deterministically first; otherwise the diff is meaningless.

## Acceptance criteria

A hook is considered accepted only when:
- The CSV diff is byte-identical for the agreed scenarios, OR
- All differences are in fields explicitly marked as ignored (timestamps, pointer values, frame counters), AND
- No crash, hang, or visible visual regression in a manual playthrough of the affected feature.
