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

## Batches (several registry hooks at once)

`py -3.12 re/frida/run_diff_parallel.py <hook> [<hook> ...]` runs the **warm pool** by default
(`run_diff_warm.py`, since 2026-10-27): 4 MASHED instances boot once and stream every hook's
force-call diff, instead of one boot per hook. It writes the same `log/diff_<hook>.csv` and uses
`run_diff.py`'s own verdict code (arg_type pre-flight, degenerate guard, return codes). It routes
to the cold `run_diff.py` path by itself, per hook, for anything warm cannot reproduce:
`scenario` hooks, `call_log` hooks (drawstream CSV), `dialog` hooks, registry `cold_only: '<why>'`
or a `COLD_ONLY_ARG_TYPES` handler (process-wide state), and every hook when `--self-baseline`,
`--negctl` or `--x87-cw` is passed. Every warm non-GREEN is re-run cold and the cold verdict is
reported. `--cold` restores one fresh spawn per hook. Hold the machine game lock and a test slot
around the batch (`test-slot bash -c 'py -3.12 re/orchestrator/mashed_lock.py run --label X -- py -3.12 re/frida/run_diff_parallel.py ...'`).

A hook whose fixture leaves process-wide state behind (a later hook on the same instance would
read it) gets `cold_only: '<reason>'` in `hooks_registry.py`; do not write a new runner for it.

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
