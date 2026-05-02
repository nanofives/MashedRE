# Function Confidence Rubric

Every row in `hooks.csv` carries a confidence label `C0..C4`. Promotion is one-way and gated by **observable evidence**, never by feeling. The `re-classify` skill is the only thing that should mutate a function's confidence — do not edit `hooks.csv` by hand without consulting the rubric.

## Levels

### C0 — Unknown
- The function exists in Ghidra (it has an RVA) but nothing else is established.
- May not even have a name better than `FUN_00xxxxxx`.
- Default state for newly-discovered functions during Phase 1 sweep.
- **Promotion to C1 requires:** at least one of (a) a meaningful name from imports/exports/RTTI, (b) a string xref that hints at purpose, (c) a caller chain that anchors it to a known subsystem entry point.

### C1 — Located
- The function has a tentative purpose drawn from name/strings/xrefs but the body has not been read.
- Belongs to a named subsystem (audio, AI, render, …).
- Calling convention is identified.
- **Promotion to C2 requires:** the decompilation has been read end-to-end and the **shape** of the function (number of args, return type, what it reads/writes) is documented in `re/analysis/<subsystem>/<func>.md`. NO-GUESSING applies — fields and constants are recorded as raw addresses + values, not as semantics.

### C2 — Disassembled
- The decomp has been read and mechanically transcribed: every memory access, every branch, every callee is enumerated.
- Sign and width of every operand is correct (signed/unsigned, 8/16/32/64-bit).
- Field offsets cited (`+0xNN`) for every struct access, even if the struct is not yet named.
- **Promotion to C3 requires:** semantics are inferred and verified. The function has a real name. Every field has a name (or is explicitly marked `[UNCERTAIN]` with the marker recorded in `UNCERTAINTIES.md`). At least one caller and one callee are themselves at C2 or higher (no semantic inferences from islands).

### C3 — Understood
- The function's purpose is stated in plain prose at the top of `re/analysis/<subsystem>/<func>.md`, with citations.
- A reimplementation has been written in `mashedmod/src/mashed_re/`.
- Build is clean.
- The function is hooked through `RH_ScopedInstall` and runtime-toggleable.
- **Promotion to C4 requires:** the `diff-original` skill has produced a clean Frida CSV diff between original and modded behavior on at least one canonical scenario. No stubs in the implementation.

### C4 — Verified
- All F-DoD criteria satisfied (see `ROADMAP.md`).
- The function is canonical reference material; lower-confidence callers can lean on it.

## Demotion rules

A function is **demoted** automatically when:
- A diff regression is observed (C4 → C3 until re-verified).
- A struct it depends on changes shape (any function touching that field at C3 → C2).
- An `[UNCERTAIN]` it depends on is upgraded with new contradicting evidence.

Demotions are loud — emit a row in `re/analysis/CHANGELOG.md` so we don't lose track of regressions.

## Anti-patterns

- Promoting on "this is probably what it does." Ban.
- Skipping levels (C0 → C3). Ban — every level's evidence must be on file.
- Promoting an island. If callees are C0 and the function reads an unknown global, it cannot be C3.
- Promoting because the build compiles. C3 requires reasoning about behavior; C4 requires Frida diff. Compile-passing alone gets you nothing.
