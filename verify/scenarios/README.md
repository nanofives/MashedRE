# Canonical Verification Scenarios

A **scenario** is a reproducible sequence of game inputs that drives a specific code path. C4 promotions reference a scenario name; the Frida-diff CSVs are filed under that scenario.

Scenarios are stable IDs — once a scenario is documented, it doesn't change shape (only its inputs may be tightened for determinism). New behaviors get new scenarios.

## Layout

```
verify/scenarios/
├── README.md                           (this file)
├── 001-boot-to-menu/
│   ├── inputs.md                       (key sequence + expected timings)
│   ├── frida_setup.js                  (instruments specific to this scenario)
│   └── reference.csv                   (committed; the original-behavior baseline)
├── 002-single-race-egypt-bullet/
│   └── ...
└── 003-frontend-walkthrough/
    └── ...
```

## Scenario naming

`NNN-<short-slug>` — 3-digit zero-padded ordinal + kebab-case description. Once allocated, never renamed.

## Scenario life cycle

1. **Define** — add `verify/scenarios/NNN-<slug>/inputs.md` describing the input sequence (intro skip, menu navigation, race start, etc.) precisely enough that a fresh run produces the same byte trace.
2. **Bake reference** — run the scenario against unmodified `MASHED.exe` with the scenario's Frida script; commit the resulting `reference.csv` as ground truth.
3. **Use** — `diff-original` skill compares a hooked run against `reference.csv`. Identical = pass.
4. **Re-bake** — only when Ghidra's view of the binary changes structurally (e.g., a struct gets a new field), the reference may need re-baking. Document the reason in a commit message.

## Required scenarios for milestones

| Milestone | Scenarios needed |
|---|---|
| Phase 4 (first hook) | 001-boot-to-menu (sanity), one leaf-function-specific scenario |
| Phase 5 frontend sweep | 003-frontend-walkthrough |
| Phase 5 audio sweep | TBD |
| Phase 5 vehicle sweep | 002-single-race-egypt-bullet (or equivalent fixed track/vehicle) |
| Phase 6 / P-DoD | full playthrough scenarios for every track and vehicle |

## Determinism requirements

A scenario's reference CSV must be byte-identical across runs of the same `MASHED.exe`. To ensure this:

- **Seed RNG** — find Mashed's RNG init (Phase 3) and force a known seed in the Frida instrument.
- **Fix frame timing** — hook the `QueryPerformanceCounter` / `timeGetTime` calls and return monotonically increasing canned values.
- **Disable system time reads** — same hook pattern.
- **Skip intros** — every scenario starts post-intro to avoid MCI video timing variance.

If a scenario can't be made byte-identical (hardware-dependent quirks), document the *fields to ignore* in `inputs.md` and have the diff tool mask them.
