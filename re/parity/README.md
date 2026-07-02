# Whole-game parity sweep + C4-targeting pipeline

Automated detector for visual/behavioral differences between the original
`MASHED.exe` and the standalone `mashed_re.exe`, plus a targeting layer that
turns "functions whose output already matches" into a high-yield `run_diff`
queue for C4 promotion. Replaces hunting differences one screen at a time.

Design + rationale: `re/analysis/parity_tooling.md` and the approved plan
(`Whole-Game Parity Sweep + C4-Targeting Pipeline`).

## Pieces

| file | role |
|---|---|
| `scenarios.json` | manifest of comparable states (P0 = settled menu screens; P1-P3 stubbed) |
| `sweep.py`       | dual-side capture orchestrator (drives both binaries to each state) |
| `report.py`      | merges pixel + draw-list diffs into one ranked, emitter-attributed report |
| `target_c4.py`   | clean-match emitters → C4-promotion queue (`C4_TARGETS.md`) |
| `runs/<ts>/`     | per-run output: per-scenario bundles + `index.json` + `report.{json,md}` |

## Run a sweep

```powershell
# 1. build the standalone (needs mashed_re.exe + mashed_re.map)
mashedmod\build.bat

# 2. capture both sides for the menu phase (spawns MASHED + mashed_re.exe;
#    kills only the PIDs it spawns; one Frida session at a time)
py -3.12 re/parity/sweep.py --phase menu --ts run1
#    (or a single scenario: --only menu.s1 ;  preview only: --dry-run)

# 3. rank + attribute the differences
py -3.12 re/parity/report.py re/parity/runs/run1     # -> report.md / report.json

# 4. turn clean matches into a C4 queue (targeting only — NOT C4 evidence)
py -3.12 re/parity/target_c4.py re/parity/runs/run1  # -> re/parity/C4_TARGETS.md

# 5. the actual C4 gate: run_diff with the hook installed in the ORIGINAL
py -3.12 re/frida/run_diff.py <hook>                 # GREEN + re-classify => C4
```

## The loop

```
sweep -> report (ranked, emitter-attributed)
  ├─ RED row, emitter named  -> fix the RE function -> re-sweep that scenario
  └─ clean-match emitter (C2/C3) -> target_c4 -> run_diff -> GREEN -> C4
```

A standalone-vs-original sweep match **never** grants C4 by itself
(`re/CONFIDENCE.md`); `run_diff.py` with the inline-JMP installed in the
original on a canonical scenario remains the sole gate.

## Known calibration: animated chrome (P0)

The menu chrome is **perpetually animated** (arc-wash sweep, preview crossfade,
gradient bands). Two independently-booted captures land at different animation
phases, so the draw-list LCS pairing collapses even though the composition is
the same — proven by diffing the original against *its own* burst (frame 0 vs
frame 3 matches only 77/119 at `tol_anim=4`, but 117/119 at `tol_anim=40`).

Until an animation-phase freeze is added (a debug env that pins the arc/checker/
crossfade frame counter on both sides), the draw2d channel for animated screens
needs a large `tol_anim`, or diff the static chrome only and lean on the pixel
channel for animated regions. Deterministic scenarios (the P3 static-camera
track flythrough — no AI/physics/animation) sidestep this entirely.

## Phase roadmap

- **P0** menu/results, settled (built; reuses the 2D path, zero new C++).
- **P1** boot/loading frames, pixel-only (`boot_frame` driver).
- **P2** in-race 2D HUD (`nav_race` driver; reuses the P0 comparators).
- **P3** in-race 3D world — new `DrawStreamDump_OnDraw3D` (RE) + a
  `0x004e1007` `DrawIndexedPrimitive` burst (original), led by the deterministic
  static-camera flythrough. The biggest divergence and the biggest C4 payoff
  (render subsystem is large and under-promoted).
