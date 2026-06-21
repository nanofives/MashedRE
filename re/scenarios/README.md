# Captured playthrough scenarios (machine-replayable layer)

A **captured scenario** is a human-guided playthrough recorded *passively and
in-process* so it can later be **replayed deterministically without the human** —
to re-reach a deep game state and run installed-hook Frida diffs against it (the
canonical-scenario / C4 evidence the `re/analysis/C4_REVALIDATION.md` backlog is
gated on), and to know *which functions a given part of the game exercises*.

This is the machine layer. It complements `verify/scenarios/` (the human-readable
`inputs.md` + reference-CSV framework). Relationship:

| Layer | Path | Artifact | Author |
|-------|------|----------|--------|
| Human nav doc | `verify/scenarios/NNN-<slug>/inputs.md` | prose key-sequence + timings | hand-written |
| **Machine capture** | `re/scenarios/NNN-<slug>/` | replayable frame-indexed trace + coverage | recorded |

## Why this exists

My autonomous button-pressing harnesses only reach a thin slice of the game, so
whole branches never fire. Documented example
(`re/analysis/phys_c4_evidence/COVERAGE_SCENARIO_FINDINGS_2026-06-17.md`): the
A5/A6a/A6b **airborne** branches are transcribed verbatim but **NOT REACHED**
because *"the canonical Quick-Battle Arctic arena is geometrically flat"* — the
cars never lift a wheel. A human who knows the game picks a ramp track and jumps
trivially. Guided play → real coverage of branches I structurally cannot reach.

## Capture rules (non-negotiable)

- **NO OS-level input injection** — banned, see `re/frida/CANONICAL_INPUT_DESIGN.md`
  (a 2026-06-06 incident leaked keystrokes into the user's active window). The
  recorder only **reads** the DirectInput buffer dinput8 fills from the real
  keyboard; the human's physical presses are what get captured.
- **Spawn + explicit-PID + kill-only-that-PID.** Never blanket-kill MASHED by name
  (multiple sessions run concurrently). The recorder spawns its own MASHED and
  kills only that PID.
- Record against the **original `MASHED.exe`** (the diff reference) for canonical
  behavior; the same recorder works on `mashed_re.exe` to check the port reaches
  the same states.

## Directory format

```
re/scenarios/NNN-<slug>/
├── meta.json          # name, binary, exe SHA-256 anchor, date, areas, env knobs, frame count
├── input_trace.tsv    # frame, t_ms, dik_csv, names_csv   (delta-encoded: one row per change)
├── coverage.tsv       # rva, first_frame, first_t_ms, name, subsystem, confidence
└── notes.md           # what the human navigated, narration, anchored to the SHA
```

- `input_trace.tsv` is **delta-encoded**: a row is written only when the set of
  down keys changes. The replayer holds the last state until the next row. Keyed
  by `frame` = the GetDeviceState(cb=256) call index, so replay is deterministic.
- `coverage.tsv` lists the first frame each candidate RVA fired (one-shot). The
  candidate set is *targeted* per session (e.g. the airborne-physics group, or a
  mode's dispatch functions), not the whole image.

## Tooling

```powershell
# Record a guided session (you play the spawned window; I capture passively).
py -3.12 re/frida/record_session.py --name ramp-airborne --mode physics --seconds 180

# Generic area coverage (stock MASHED) with a candidate RVA set.
py -3.12 re/frida/record_session.py --name topdog-mode --cov @cov_topdog.txt --seconds 150

# Replay a captured trace deterministically (no human) — re-reach the state,
# optionally with hooks installed for an A/B diff.
py -3.12 re/frida/replay_session.py re/scenarios/001-ramp-airborne

# Autonomous pipeline self-test (in-process synthetic inject + record-back; no human).
py -3.12 re/frida/record_session.py --selftest-roundtrip
```

## Naming

`NNN-<short-slug>` — 3-digit zero-padded ordinal + kebab-case description, same
convention as `verify/scenarios/`. Once allocated, never renamed.
