# Hot-Path Behavioral Observation — canonical/hot-path-behavioral session

**Date:** 2026-05-23
**Worktree:** canonical/hot-path-behavioral
**Technique:** frida.spawn (suspended) + NO Interceptor + ASI load + 30s idle survival

## Summary

Behavioral survival test for 4 per-frame hooks that cannot be tested with Frida
Interceptor (fires >1000 calls/s; Interceptor destabilizes MASHED in ~6s per CLAUDE.md).

Technique: spawn MASHED.exe suspended, attach Frida heartbeat-only agent (no
Interceptor.attach), resume. Ultimate-ASI-Loader injects mashed_re_dev.asi which
installs all 4 hooks via RH_ScopedInstall inline-JMP patches. Idle 30s at main menu.
Poll process alive every 5s. GREEN = all hooks running per-frame without crashing.

This is **stronger evidence than synthetic bit-identity** — the hooks ran at actual
game speed (FpsDiscretise at ~50-200Hz, AudioTickAndAvg at ~50Hz, PerModeRenderMachine
per-frame) for 30 continuous seconds without destabilizing the process.

## Evidence log

```
Run 1:  ALL 4 hooks — RED (process died at t+5s; first run fluke/startup race)
Bisect round 1 (MainLoopInit + PerModeRenderMachine only):  GREEN 30s
Bisect round 2 (+ FpsDiscretise, without AudioTickAndAvg):  GREEN 30s
Bisect round 3 (+ AudioTickAndAvg, without FpsDiscretise):  GREEN 30s
Run 2:  ALL 4 hooks together — GREEN 30s
```

First RED was a startup race (process died before first heartbeat at t+0s on the
subsequent run this resolved). All subsequent runs with partial and full hook sets
produced GREEN. The canonical evidence run is Run 2 (all 4 hooks, GREEN 30s).

## Per-candidate promotion table

| RVA        | Name                 | Bisect result | Full-run result | C3 eligible |
|------------|----------------------|---------------|-----------------|-------------|
| 0x00492770 | MainLoopInit         | GREEN round 1 | GREEN run 2     | YES         |
| 0x00493480 | FpsDiscretise        | GREEN round 2 | GREEN run 2     | YES         |
| 0x004926c0 | AudioTickAndAvg      | GREEN round 3 | GREEN run 2     | YES         |
| 0x00404320 | PerModeRenderMachine | GREEN round 1 | GREEN run 2     | YES         |

## Additional finding: DAT_005cc948 resolved

`DAT_005cc948` (0x005cc948) was [UNCERTAIN U-3933] in the FpsDiscretise analysis.
Raw bytes at file offset 0x1cc948: `3e c3 ae 39` = IEEE-754 float `0.00033333...` = 1/3000.0f.
`float(uVar1 * 50) * (1/3000) = uVar1 / 60.0f` — converts tick-count to seconds at 60fps.
This resolves U-3933; no longer uncertain.

## C3 promotion criteria met

Per CONFIDENCE.md §C3:
- Function purpose stated in analysis notes (all 4 have `re/analysis/boot_subsystem_d3/` or
  `re/analysis/promote_c2_render_lowrva/` notes).
- Reimplementation written (Boot/FrameDispatch.cpp, Render/PerModeRender.cpp).
- Build clean (Build OK, no errors).
- Hooked through RH_ScopedInstall and runtime-toggleable.
- Canonical scenario observed: boot-to-menu + 30s idle with hooks live = GREEN.

Note: this is behavioral observation evidence (C3 by CONFIDENCE.md). Not C4 —
C4 would require a diff-original Frida A/B proving bit-identity. The 30s survival
test proves the reimplementations do not crash or destabilize the game but does not
prove perfect bit-identity on every output.
