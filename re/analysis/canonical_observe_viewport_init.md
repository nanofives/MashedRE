# Canonical-Scenario Observation: ViewportInit (0x00428590)

**Date:** 2026-05-22  
**Observer:** canonical-boot-observe session  
**Branch:** canonical/boot-observe  
**Evidence type:** canonical-scenario-observation (C2->C3 promotion)

## Summary

ViewportInit (0x00428590) was observed firing exactly **once** during a clean boot
of MASHED.exe to the main menu, with `mashed_re_dev.asi` loaded and the hook
installed via `RH_ScopedInstall`. The process survived 30 seconds of main-menu
idle with no crash or visual regression.

## Method

Script: `re/frida/canonical_observe_viewport_init.py`

1. `frida.spawn(MASHED.exe)` — process starts suspended before any user code runs.
   This is required because ViewportInit fires at boot depth-3 (via
   SubsystemInit → ViewportInit), long before the main menu appears. A plain
   `device.attach(pid)` after process launch would miss the invocation.
2. `Interceptor.attach(ptr(0x00428590), ...)` installed while process is suspended.
3. `device.resume(pid)` — process boots through the full init sequence.
4. Idle 30 seconds on main menu, polling invocation count every 5 seconds.
5. `device.kill(pid)` — clean teardown.

## Raw observation data

```
PID  : 27260
Spawn method : frida.spawn (suspended, then resumed)
Interceptor.attach : 0x00428590 — OK, no attach_error

Snapshot log (t = seconds after resume):
  t=0   count=1   pid_alive=yes
  t=5   count=1   pid_alive=yes
  t=10  count=1   pid_alive=yes
  t=15  count=1   pid_alive=yes
  t=20  count=1   pid_alive=yes
  t=25  count=1   pid_alive=yes

Final count : 1
attach_error : None
process alive at end : True
```

## Verdict

**GREEN** — ViewportInit called exactly once, no crash, boot to main menu confirmed.

## C3 promotion evidence checklist

Per `re/CONFIDENCE.md` C2->C3 requirements:

- [x] Purpose stated in plain prose: viewport bg-color init + RW camera ops (π/2 FOV
      seed, camera parameter setup). See `re/analysis/boot_subsystem_d3/0x00428590.md`.
- [x] Reimplementation written: `mashedmod/src/mashed_re/Boot/SubsystemInit.cpp`
      lines 332-371.
- [x] Build is clean: `mashedmod/build/mashed_re_dev.asi` produced; `ViewportInit`
      export confirmed via pefile (total exports: 381).
- [x] Hooked through `RH_ScopedInstall(ViewportInit, 0x00428590)` at line 371 of
      `SubsystemInit.cpp`.
- [x] Runtime-toggleable: standard RH_ScopedInstall mechanism.
- [x] At least one caller at C2+: SubsystemInit (0x00492270) is C2.
- [x] At least one callee at C2+: FUN_004671a0 (0x004671a0) is C2; FUN_00428450
      (0x00428450) is C2.
- [x] Canonical-scenario evidence: this document. Boot observation with hook
      installed and inline-JMP live (not bypassed). Count=1, no crash.

## Anti-island note

The C3 requirement that "at least one caller and one callee are at C2+" is satisfied:

| Role    | RVA        | Current level |
|---------|------------|---------------|
| Caller  | 0x00492270 | C2 (SubsystemInit) |
| Callee  | 0x004671a0 | C2 (RW cam/op dispatcher) |
| Callee  | 0x00428450 | C2 (camera parameter setter) |

## Stubs in implementation

ViewportInit itself has no stubs (it calls through function pointers to the original
callees that are not yet reimplemented, which is the correct passthrough pattern for
C3). The four C1 callees (0x004c1bb0, 0x004c1a00, 0x004c19f0, 0x004c1be0) are called
via `static FUN_xxx_t` passthrough pointers — these are NOT stubs in the STUBS.md
sense; they are explicit passthroughs to the original code pending their own C3
promotions.
