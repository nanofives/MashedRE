# RESULT v2: savelink2 (b) slice 3, the video-config cluster, guard-off test and guard-on control

PREREG: `PREREG_settingsconfig_v2.md`. v1 (registered P1 failed): `RESULT_settingsconfig.md`. Build: exe `537dff8f`, dev
.asi `5802fe3a`, both checked before the arms. Evidence: `D:/MashedWT/savelink2_data/leaf_ab/v2_T.*`, `v2_K.*`.

**Verdict: Q1, Q2 and Q3 hold.** Slice 3 is committed per the v2 decision rule.

| Arm | Result |
|---|---|
| T, guard off (`MASHED_HOOK_ALLOW_BOOTPATCH=1`) | `cases=22 noo=0 ndiff=0 logged=9 cfg_restored=1`, ret 0. LIVE err 22, dbg 154, load 1. |
| K, guard on (control) | `cases=22 noo=0 ndiff=4 logged=6 cfg_restored=1`, ret 4. LIVE dbg 0. ndiff exactly cs19 / cs20 / cs21 / cs22: log 0/36, 8/44, 0/33, 0/33 (original / shared). |
| Q3 | videocfg.bin f87df3c8b03bab30 before and after both arms. 0 leftover files. |
| Gates | exe 537dff8f unchanged since `arms_cfg.txt`: H_DEF-4 bacfc747 / 8dd1c004 / 6f9932b6, B f9deb9b9 / d3601fcc / 9da9a26a. |

Reading:
- With 0x00496400 installed on both sides, the five shared bodies match the unpatched original on every case, including
  the "Loading / Saving video cfg" lines and the "\tFAILED\n" path.
- With the guard on, the only divergence is the four cases whose original path calls the patched RET at 0x00496400.
  The guard-on divergence is a property of disable_log, not of the bodies.

Standing facts recorded in hooks.csv:
- 0x00496400 is refused by the boot-patch guard in every default run.
- 0x004963e0 installs over disable_log's JE. That is invisible while the log opener 0x00496490 stays patched.
