# RESULT: savelink2 (b) slice 3, the video-config cluster as shared bodies

PREREG: `PREREG_settingsconfig.md`. Build: exe `537dff8f`, dev .asi `5802fe3a`. Evidence:
`D:/MashedWT/savelink2_data/leaf_ab/C1c.*` (registered run) and `C1d.*` (diagnostic), `D:/MashedWT/savelink2_data/arms_cfg.txt`.

**Verdict: P1 FAILED as registered (4 ndiff), with the cause identified as the boot-patch guard and not the bodies.** A
post-hoc diagnostic run (C1d) gives exactly the registered P1 numbers. P2 and P4 held. Under the PREREG's decision rule the
code is not committed without Mariano.

## Harness fixes before the registered run (no prediction involved)

- C1 / C1b: the probe returned "no module". `nav_champ_probe.py`'s boot forces `MASHED_RE_NO_AUTO_HOOK=1` and sets no
  `MASHED_RE_DEV`, so mashed_re_dev.asi never loaded. The settingssweep plan now sets scenario_launch.py's `--hooks` env
  (`MASHED_RE_DEV=1`, `MASHED_HOOK_ONLY=<the five RVAs>`, NO_AUTO_HOOK dropped).
- The probe's boot copies `scripts/canonical/videocfg_windowed.bin` over the root's videocfg.bin. The pre/post hashes
  are taken around that (f87df3c8b03bab30 both times).

## C1c (registered)

`SWEEP fn=settings cases=22 noo=0 ndiff=4 logged=6 cfg_restored=1`. LIVE: err 22, dbg **0**, name 0, load 1, save 0.

- Cases 1-18 (all ConfigLogError, ConfigLogDebug and ConfigFilenameGet cases, plus ConfigLoad/ConfigSave without a
  handle): noo 0, ndiff 0. This includes the unpatched ConfigLogDebug trampoline with a log handle, at 6/12/16 bytes.
- Cases 19-22 (ConfigLoad / ConfigSave with a log handle): the original path wrote no "Loading/Saving video cfg" line.
  The shared body wrote it (36 / 33 bytes).

Cause: HookSystem's boot-patch guard (Core/HookSystem.cpp:64-84, `kBootPatched`) **refuses 0x00496400**, because
disable_log's RET is its first byte. So the original ConfigLoad / ConfigSave `call 0x00496400` reached the patched RET.
The shared bodies call the unpatched ConfigLogDebug. The difference is the boot patch, not either body.

## C1d (post-hoc diagnostic, `MASHED_HOOK_ALLOW_BOOTPATCH=1`)

`SWEEP fn=settings cases=22 noo=0 ndiff=0 logged=9 cfg_restored=1`, SettingsSweep returned 0. LIVE: err 22, dbg 154,
load 1. With the 0x00496400 hook installed, both sides call the same ConfigLogDebug. The log opener 0x00496490 stays
patched, so the handle stays NULL and the 154 boot calls do nothing. These are the registered P1 numbers exactly.

## P2, P4

- videocfg.bin f87df3c8b03bab30 before and after every run. No `videocfg.bin.sweep_hidden` or `leaf_cfglog.tmp` was
  left behind.
- Gates (exe 537dff8f): H_DEF-4 bacfc747 / 8dd1c004 / 6f9932b6. B f9deb9b9 / d3601fcc / 9da9a26a. Unchanged.
- Lint 0 NEW.

## Correction to the PREREG's "recorded" note

The PREREG said the installed hooks override both disable_log sites. Measured:
- 0x00496400 is refused by the guard.
- **0x004963e0 installs**: its JMP covers 0x004963e0..e4, and the patched JE is at 0x004963e7, outside that range. So
  the ConfigLogError hook bypasses disable_log's neutralization of ConfigLogError. With the opener patched, the handle is
  NULL and the 22 boot calls write nothing.

This interaction has existed since the hook was re-enabled (2026-05-24), and it is invisible today.

## Commit hygiene

The staged removal of Save/SettingsCfg.cpp went into the PREREG commit (the same as slice 1's RwStream.cpp).
