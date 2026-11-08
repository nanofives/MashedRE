# RESULT PORT-SAVE-01 (2026-11-08)

Port axis D4, lane A swap batch SAVE (plan: `verify/portplan_20261108/PLAN.md` section 4, lanes.tsv rows `save`).
Branch `orch/PORT-SAVE-01` off `orch/integration` @ `80d933fa7`.

**Verdict: GREEN.** 6 rows swapped to their byte-matched sources in both targets, 2 rows were already the
matched body (MATCH-S01), 1 row NOT-SAFE (0x00410510). Port column: all 9 rows `called` before and after.
Default gates on the worktree exe: G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3, all equal to the `merge_train.json` references.

## Rows

| rva | name | outcome | source | port before | port after |
|---|---|---|---|---|---|
| 0x00404f50 | SaveWrite | already swapped (MATCH-S01, 2026-10-31): `Save/GameSave.cpp` compiles `re/match/s01/00404e50.c` (which holds 0x00404e50 / 0x00404f50 / 0x00404f80). No change | re/match/s01/00404e50.c | called | called |
| 0x00404f80 | SaveFileExists | already swapped, as above | re/match/s01/00404e50.c | called | called |
| 0x004963e0 | ConfigLogError | swapped | re/match/s16/004963e0.c | called | called |
| 0x00496400 | ConfigLogDebug | swapped | re/match/s16/00496400.c | called | called |
| 0x00498910 | ConfigFilenameGet | swapped | re/match/s16/00498910.c | called | called |
| 0x00498950 | ConfigLoad | swapped | re/match/s16/00498910.c (= 00498950.c text) | called | called |
| 0x004989b0 | ConfigSave | swapped | re/match/s16/00498910.c (= 004989b0.c text) | called | called |
| 0x00550bc0 | VfsStreamGetType | swapped | re/match/s24/00550bc0.c | called | called |
| 0x00410510 | Race::EvaluateResult | **NOT-SAFE**, not touched (see below) | re/match/s02/00410510.c | called | called |

The lanes.tsv source for 0x00404f50 / 0x00404f80 is `re/match/s01/raw/*.c`. Those raw files name the original's
literal `"gamesave.bin"` and `&DAT_00803358`. The swapped body in the tree is the same matched text from
`re/match/s01/00404e50.c` with `s_gamesave_bin_005cc8e0` bound to `GameSaveFilename()` (`mashed_re_gamesave.bin`,
MASHED_SAVE_PATH / MASHED_SAVE_ORIGINAL) and the buffer to the port's `g_saveBuf`, so the exe keeps writing
`mashed_re_gamesave.bin` beside itself. Nothing to do on this branch.

`swaps.tsv` (this folder) lists the 6 swaps in the MATCH-S01 layout (source and wrapper git blob ids). `relocheck.tsv`
is `re/tools/match/relocheck.py` on the four source files, run on this branch: all OK. `match.py` on the same files
(DDK cl 13.00.9176 / c2 13.00.9178, each file's recorded flags):

```
0x004963e0 _FUN_004963e0: 100.0% (regs 100.0%, ops 100.0%)  9/9 ins  flags=/O2 /GS /Ob1 tc=ddk
0x00496400 _FUN_00496400: 100.0% (regs 100.0%, ops 100.0%)  24/24 ins  flags=/O2 /GS /Ob1 tc=ddk
0x00498910 _FUN_00498910: 100.0% (regs 100.0%, ops 100.0%)  10/10 ins  flags=/O2 /GS /Ob1 tc=ddk
0x00498950 _FUN_00498950: 100.0% (regs 100.0%, ops 100.0%)  30/30 ins  flags=/O2 /GS /Ob1 tc=ddk
0x004989b0 _FUN_004989b0: 100.0% (regs 100.0%, ops 100.0%)  27/27 ins  flags=/O2 /GS /Ob1 tc=ddk
0x00550bc0 _FUN_00550bc0: 100.0% (regs 100.0%, ops 100.0%)  3/3 ins  flags=/O2 /Ob1 /G5 tc=ddk
```

## How the config cluster is bound (0x004963e0 / 0x00496400 / 0x00498910 / 0x00498950 / 0x004989b0)

- **TUs.** `004963e0.c` is compiled at the end of `Save/SettingsConfig.cpp`. `00496400.c` and `00498910.c` (one file
  holds 0x00498910 / 0x00498950 / 0x004989b0, the same text as `00498950.c` and `004989b0.c`) are compiled in the new
  `Match/PortSave01_Config.cpp` (added to both rsp files and `x87_tus.txt`). They cannot share one C++ TU:
  `004963e0.c` defines `FUN_004963e0(char *)` and `00498910.c` declares it `(const char *, ...)`, two extern "C"
  declarations of one name with different types. The original's C TUs were separate too, and the call is cdecl with
  one pointer argument either way.
- **Callees, all by name.** `fputs`, `FUN_004a42c5` (vsprintf), `FUN_004a4541` (fopen), `FUN_004a49cf` (fread),
  `FUN_004a4b22` (fwrite), `FUN_004a4368` (fclose) -> extern "C" `SettingsCfg*` functions in SettingsConfig.cpp that
  call the same `Host*` functions the replaced hand cores called: the exe's CRT in the exe (`_fsopen(.., _SH_DENYNO)`,
  `std::fputs`, ...), the game CRT push/ret thunks in the .asi (unchanged). `FUN_004963e0` -> `ConfigLogError`,
  `FUN_00496400` -> `ConfigLogDebug`, `FUN_00498910` -> `ConfigFilenameGet`. No body makes an absolute-address call in
  the exe.
- **Globals.** The matched preamble's `MGLOBAL` / `MGLOBALA` are predefined to go through `SettingsCfgGlobal(addr)`:
  the original address in the .asi (what the preamble does by default), and in the exe the same storage the hand cores
  used: the private log handle, 16-byte filename copy and 0x200-byte settings buffer, and the exe's copies of the five
  strings (`"Loading video cfg from %s\n"` 0x005d013c, `"Saving video cfg to %s\n"` 0x005d0158, `"\tFAILED\n"`
  0x005cfe70, `"rb"` 0x005cf010, `"wb"` 0x005d00d8). The exe's mapping of the original .rdata is zero-filled
  (exe_main.cpp `kMashedDataBase` comment, "zeroed memory"), so binding these strings to their original addresses
  would have opened `videocfg.bin` with mode `""` in the exe. This is a binding of the same data the hand cores used,
  not a change to it: **flag for the lead**, since the plan lists port-side rebinds as NOT-SAFE. The rebind it means
  (a body reading a rebased record) is the case where the matched body would read different memory than the hand
  port. Here both read the same exe-private storage. If the lead reads the rule strictly, drop commit
  `6136322d7`'s config part and keep the VfsStreamGetType swap.
- **File name.** The matched body's own literal `"videocfg.bin"`, CWD-relative: the name and path the hand core used
  in both targets (`kFilename`, now removed as unused). The exe still writes `videocfg.bin` beside itself.
- **Inlining.** The recorded flags are `/Ob1` (only `inline`-marked functions are inlined). VS2022 `/O2` inlined
  `ConfigFilenameGet` into `ConfigLoad` / `ConfigSave`, which left the 0x00498910 symbol with no call edge and dropped
  its port column to `body`. `#pragma auto_inline(off)` around the two includes restores the original's call
  structure (`CALL 0x00498910` at 0x00498951 / 0x004989b1) and the row is `called` again.
- **Callers.** `Boot/VideoCfgFirstRun.cpp` (exe only) calls `ConfigLoad` / `ConfigSave` (extern "C") instead of the
  removed `ConfigLoadCore` / `ConfigSaveCore`. `Match/MatchS06_0042a530.cpp` binds `FUN_00496400` to `ConfigLogDebug`
  by name; its variadic forwarder to `ConfigLogDebugCoreV` is gone.
- **.asi sweep.** `SettingsSweep()` (exported, Frida-driven, `re/frida/nav_champ_probe.py --plan settingssweep`) now
  runs the matched exports as the new side of each case. The installed entries were thin wrappers that counted live
  calls (`g_live[5]`) around the cores. The matched bodies are installed directly and carry no counter, so the
  sweep's `LIVE` line now prints only `cfg0=`. The sweep was not re-run on this branch (asi-side, needs the original
  booted with the .asi deployed, which this worker must not do).
- **rva-lint.** The .asi trampoline comments `// 0x00496400 SUB ESP...` and `// 0x004963e0..0x004963e7 replayed...`
  were read by the body scanner as a second body of those RVAs (DUP-IN-TARGET) and bound 0x004963e0 to
  `LogErrorOrig`. They now start with "Original entry replay:". `dual_copy_allowlist.txt` unchanged (no key went:
  these RVAs had no allowlisted dual copy).

## 0x00550bc0 VfsStreamGetType

`re/match/s24/00550bc0.c` compiled in `Save/VfsStream.cpp` in place of the hand body (`return *(int*)(ctx + 8)`). No
globals, no callees. The exe caller `Match/MatchS09_00451ce0.cpp` already binds `FUN_00550bc0` to `VfsStreamGetType`
by name.

## NOT-SAFE

| rva | name | reason |
|---|---|---|
| 0x00410510 | Race::EvaluateResult | Port-side rebind. The exe body is `EvaluateResultCore(EvalResultInputs&, ...)` (Race/EvalResultCore.cpp:10), driven in the exe by `SnapshotInputs` (Race/RuleEngine.cpp:150-179), which feeds the port snapshot (`p.timeAttack`, `c.score[]`, `c.alive[]`, `p.timer`, ...) and makes the side-effect callbacks no-ops (`OnEnter`, `OnPlayer0Flag`, `OnConclude`). The matched body (re/match/s02/00410510.c) reads and writes 11 globals at their original addresses (`DAT_0063b90c`, `DAT_0063ba8c`, `DAT_007f0fcc`, `DAT_007f0fd0`, `DAT_005f2770`, ...) and calls 15 RVAs (0x00405890, 0x0040b6d0, 0x0040e340, 0x00417730, 0x00417740, 0x00426c00, 0x0042f500, 0x0042f6a0, 0x00448700, 0x0044df80, 0x00458f80, 0x0045bed0, 0x0045bf30, 0x0046c7b0, 0x0046cbb0). Swapping it would move the exe's race-end evaluation from the port snapshot onto the original's globals, which the exe does not maintain the same way. Same finding as MATCH-S02 (`verify/matchs02_20261031/RESULT_MATCH-S02.md`, "Not swapped"). No commit for this row. |

## Checks

| check | result |
|---|---|
| build (`MASHED_NO_DEPLOY=1 mashedmod\build.bat`, both targets) | OK. `original\mashed_re_dev.asi` not deployed |
| rva-lint | OK, no new duplicate bodies (1 known CROSS-TARGET, allowlisted, unrelated) |
| exports | both targets export ConfigLogError / ConfigLogDebug / ConfigFilenameGet / ConfigLoad / ConfigSave / VfsStreamGetType / SaveWrite / SaveFileExists (pefile) |
| backfill_exe_file + backfill_port | exe_file of 0x00496400 / 0x00498910 / 0x00498950 / 0x004989b0 -> Match/PortSave01_Config.cpp. port: 9/9 `called`. Totals after: none=5311 body=587 called=253 |
| hooks.csv `file` column | not changed. It still names Save/SettingsConfig.cpp for the four rows now compiled in Match/PortSave01_Config.cpp. `c4_reclass.py batch` skips rows already at C4 and the skill does not hand-edit `file`, so this is left to the lead / a re-classify pass |
| gates | exe `mashedmod/build/mashed_re.exe` sha256 `29c526f3...` (commit `6136322d7`), `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` under `test-slot` + `mashed_lock.py run --label PORT-SAVE-01`, private root `D:/MashedWT/PORT-SAVE-01_root`. G-CTRL `8284be46/67931834/31fe043e` (ref same), B `1d5bf4f9/1d5bf4f9/1d5bf4f9` (ref same). All four arms exit=0, self-exited. No flip, so no bisect was needed |

## Raw data (not committed)

Folder `D:/MashedWT/PORT-SAVE-01_data/b1` (runner log `D:/MashedWT/PORT-SAVE-01_data/b1_run.log`):

| file | sha256 |
|---|---|
| H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| B_DEF_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| B_DEF_K1.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| mashed_re.exe (gated build) | 29c526f31360510d36bc91a9d25de7d12e39159a3765fe41d6ebfb07c1f62bbd |
