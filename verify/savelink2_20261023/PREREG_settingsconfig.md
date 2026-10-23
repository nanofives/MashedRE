# PREREG: savelink2 (b) slice 3, the video-config cluster as shared bodies (Save/SettingsConfig.cpp)

Written before any arm. Build: exe `537dff8f`, dev .asi `5802fe3a` (work/savelink2, uncommitted on 582887690). Lint 0 NEW.

## Change

- Save/SettingsConfig.cpp holds one body each of ConfigLogError 0x004963e0, ConfigLogDebug 0x00496400, ConfigFilenameGet
  0x00498910, ConfigLoad 0x00498950 and ConfigSave 0x004989b0, and it joins exe_sources.rsp. Save/SettingsCfg.cpp
  (ConfigSave, which called its siblings by address) is removed.
- The disassembly found no functional defect. Two non-faithful details are made byte-faithful: ConfigLoad's extra third
  argument to FUN_004a4541 (a 2-argument fopen), and ConfigLogDebug's handle re-read at 0x0049643c.
- Exe: dead code, with its own CRT and private storage.
- Recorded, not changed: disable_log patches 0x004963e7 (JMP), 0x00496400 (RET) and the log opener 0x00496490
  (`xor eax,eax / ret`). So the handle 0x00772fbc stays NULL in the patched reference, and the installed hooks return
  early as the patched bytes do.

## Arms (private root D:/MashedWT/savelink_root, fresh .asi deployed there, sandbox disabled)

- **C1 sweep.** `MASHED_ORIG_DIR=D:/MashedWT/savelink_root/original py -3.12 re/frida/nav_champ_probe.py --plan settingssweep`.
  The SettingsSweep export is called at mode-select. Each case runs the unpatched-original trampoline twice (noo), then
  the shared body (ndiff).
- **G gates.** Exe `537dff8f`, `run_arms.py H_DEF-4 B_DEF_K0 B_DEF_K1 B_DEF_K26`.

## Predictions

- **P1.** `SWEEP fn=settings cases=22 noo=0 ndiff=0 logged=9 cfg_restored=1`. 22 = 2 handle states x (3 ConfigLogError +
  3 ConfigLogDebug + 1 ConfigFilenameGet + 2 ConfigLoad + 2 ConfigSave). The 9 logging cases all have a handle:
  - ConfigLogError "x\n" and "\tFAILED\n" (2)
  - three ConfigLogDebug formats (3)
  - ConfigLoad present and missing (2)
  - ConfigSave writable and read-only (2), since the "Saving" line comes before the open
- **P2.** SettingsSweep returns 0. The root's videocfg.bin sha256 is the same before and after the run.
- **P3.** The LIVE line reports the live call counts of the five hooks before the sweep. Any value is allowed; it shows
  whether the boot path calls them after install.
- **P4 (G).** G-CTRL bacfc747 / 8dd1c004 / 6f9932b6 and B f9deb9b9 / d3601fcc / 9da9a26a, unchanged.

## Decision rule

- All hold: commit. Levels are kept, file column -> Save/SettingsConfig.cpp for 0x004989b0 only.
- P1 or P2 fails: report the first failing case, restore videocfg.bin from the pre-run copy, and do not commit.
- P4 fails: stop for Mariano.
