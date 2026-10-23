# PREREG v2: savelink2 (b) slice 3, the video-config cluster, sweep with the boot-patch guard off

Written after `RESULT_settingsconfig.md` (registered run C1c: P1 failed, 4 ndiff) and before any v2 arm. The build is the
same as v1: exe `537dff8f`, dev .asi `5802fe3a`, uncommitted tree on the v1 PREREG commit. No code change since v1. The
probe change (the settingssweep hook env) predates C1c.

## Why v2

In C1c, HookSystem's boot-patch guard refused 0x00496400 (disable_log's RET at byte 0). The original ConfigLoad /
ConfigSave paths therefore called the patched no-op, while the shared bodies called the unpatched ConfigLogDebug. To
compare the two bodies on a common callee, the 0x00496400 hook must be installed on both sides. This is
`MASHED_HOOK_ALLOW_BOOTPATCH=1`, the guard's documented experiment switch. The log opener 0x00496490 is not in the hook
set and stays patched, so the live log handle stays NULL. C1d already ran this setup post hoc. v2 registers it.

## Arms (private root D:/MashedWT/savelink_root, .asi 5802fe3a deployed there, sandbox disabled)

- **T (test).** `MASHED_HOOK_ALLOW_BOOTPATCH=1 MASHED_ORIG_DIR=D:/MashedWT/savelink_root/original py -3.12
  re/frida/nav_champ_probe.py --plan settingssweep`
- **K (control, guard on).** The same command without `MASHED_HOOK_ALLOW_BOOTPATCH`, which repeats C1c.

## Predictions

- **Q1 (T).** `SWEEP fn=settings cases=22 noo=0 ndiff=0 logged=9 cfg_restored=1`, and `SETTINGSSWEEP ret=0`.
  `LIVE ... dbg>0 ... load=1`, since ConfigLogDebug is installed and called during boot.
- **Q2 (K).** `cases=22 noo=0 ndiff=4 logged=6 cfg_restored=1`. The ndiff cases are exactly cs19, cs20, cs21 and cs22
  (ConfigLoad / ConfigSave with a log handle). In each, the original's log byte count is shorter than the shared body's
  by the "Loading/Saving video cfg" line. `LIVE ... dbg=0`, `SETTINGSSWEEP ret=4`.
- **Q3.** videocfg.bin sha256 f87df3c8b03bab30 before and after each arm. No `videocfg.bin.sweep_hidden` or
  `leaf_cfglog.tmp` is left in the root.
- Gates: already measured on this exact exe (`arms_cfg.txt`: H_DEF-4 bacfc747 / 8dd1c004 / 6f9932b6, B f9deb9b9 /
  d3601fcc / 9da9a26a). Not rerun, because the exe hash is unchanged.

## Decision rule

- Q1, Q2 and Q3 hold: commit slice 3. hooks.csv notes, and the file column -> Save/SettingsConfig.cpp for 0x004989b0.
  The guard-on divergence is recorded as a property of the boot patch.
- Q1 fails: no commit. Report the first failing case.
- Q2 fails in any way other than the four named cases: report it before any commit. The control would then not be
  explaining what v1 claimed.
