# RESULT BIND-CRT-STDIO-CALLERS (2026-11-13, roadmap runs 41-42): GREEN

Follow-up to BIND-CRT-STDIO (`verify/portorigtop_20261109/RESULT_BIND-CRT-STDIO.md`). The original's log-file
cluster now runs on the `MASHED_ORIG_TOP` knob path from its byte-matched sources, bound to the BIND-CRT-STDIO CRT
bodies (`Match/PortTop_CrtStdio.cpp`). This switches the write overlay and the DDK-faithful `_ctime64` on for the
first time: before this row the bindings were inert. Top-down row (WORKER_RULES "Row kinds"): no file the default path
executes changed. Every new TU is `// RVA-LINT: ORIGTOP-ONLY` under `MASHED_STANDALONE`. The worker ran both builds, the
ddkobj check, the knob boots and the self-test, and left G-CTRL / B to the merge train.
Base: WIP `63e45fa88` (run 41) merged with `orch/integration` @ `145dec4d0` (merge `789a49018`, no conflict).

## 1. What is bound

| rva | function | source (match.py, DDK `/O2 /GS /Ob1`) | was on the knob path | now |
|---|---|---|---|---|
| 0x00496490 | log open (WM_CREATE) | `re/match/s16/00496490.c` 100% 35/35 | `PortTop_Missing_00496490` (returned 0) | `Orig_00496490`, `Match/PortTop_00496490.cpp` |
| 0x00496470 | log close (WM_DESTROY) | `re/match/s16/00496470.c` 100% 8/8 | `PortTop_Missing_00496470` (no-op) | `Orig_00496470`, `Match/PortTop_00496470.cpp` |
| 0x004963e0 | ConfigLogError | `re/match/s16/004963e0.c` 100% 9/9 | `PortTop_CfgLogError` stand-in | `Orig_004963e0`, `Match/PortTop_004963e0.cpp` |
| 0x00496400 | ConfigLogDebug | `re/match/s16/00496400.c` 100% 24/24 | `PortTop_CfgLogDebug` stand-in | `Orig_00496400`, `Match/PortTop_00496400.cpp` |

Callers moved to the matched names:
- 0x00499820 (WndProc): `Match/PortTop_00499820.cpp`
- 0x00499890 (OS-version log): `Match/PortTop_00499890.cpp`
- 0x00492370 (WinMain): `Match/PortTop_00492370.cpp`
- 0x00493710: `Match/PortTop_00493710.cpp`
- 0x00402750: `Match/PortTop_00402750.cpp`
- 0x004935a0 (texture read): `Match/PortTop_004ce790.cpp`

The two stand-ins are deleted.

Callee adapters (in `PortTop_00496490.cpp`):

| callee | bound to |
|---|---|
| 0x004a4541 fopen | `MashedCrt_fopen_004a4541`: `"wt"` 0x005d0004 opens `<MASHED_ORIGTOP_WRITE_DIR, default origtop_rw>/mashed.log` and never `original/` |
| fputs 0x004a4254 | `MashedCrt_fputs_004a4254`, plus a `[ORIG_TOP] cfglog:` mirror into `mashed_re.log` (a port diagnostic that replaces the stand-ins' lines) |
| fclose 0x004a4368 | `MashedCrt_fclose_004a4368` |
| _ctime64 0x004a43b9 | `MashedCrt_ctime64_004a43b9` (DDK day padding) |
| _ftime64 0x004a43d2, sprintf 0x004a2b60, vsprintf 0x004a42c5 | the exe CRT |

Source edit: `re/match/s16/00496490.c` got typed callee prototypes so the same text also compiles as C++ (varargs
sprintf, `void *` handle, const strings). match.py still scores it 100% 35/35.

Reference note: the reference MASHED.exe carries the disable_log patch (0x004963e7 JMP, 0x00496400 RET, 0x00496490),
so its 0x00772fbc stays 0. Only these four functions read or write 0x00772fbc. `MASHED_ORIGTOP_WRITE_DIR=off` gives
the reference's disable_log behaviour on the knob path: 0x00496490 returns 0 and the other three write nothing.

## 2. Checks

| check | result |
|---|---|
| default build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, .asi not deployed. mashed_re.exe sha256 `858a4e4e0cb0cfc108ce61e5555ed05aadfd8c9086a4c9b292c34a30573080b8` |
| object build (`MASHED_DDKOBJ=1`, NO_DEPLOY) | `=== Build OK ===`, sha256 `94e177a72ce99c96dc3b2e1ee208f863af854e1e6c4f280a4ed2867b98f5d158`. Refusals 4 (wrapper-redefines-globals), none in this row |
| `ddkobj.py check` | IDENTICAL 1149/1179, DIFFERENT 11, not-in-map 19, units linked 1130/1397. Integration head (`INTEGRATION_LOG.md`, 2026-11-13 00:04): 1145/1175 with the same 11 DIFFERENT (0x00401690 0x0042f530 0x004332a0 0x0045a3a0 0x00475a00 0x004929d0 0x00493710 0x00493900 0x00498810 0x004c0c20 0x004c0f00), units 1126/1393. The +4 are this row's units `_Orig_004963e0` / `_Orig_00496400` / `_Orig_00496470` / `_Orig_00496490`, all IDENTICAL |
| **self-test** `verify/portorigtop_20261112/run_logcluster_selftest.bat` (compiles the four TUs and `PortTop_CrtStdio.cpp` unchanged into a test exe) | **15/15 PASS** on the merged head (`selftest_output.txt`). Covers: open returns 1 and sets 0x00772fbc; close clears it; header = 19-char ctime + CRLF CRLF with the day zero-padded; error then debug line; `original/mashed.log` not created; writers after close and a second close are no-ops; mirror content; reopen truncates; `off` returns 0, leaves 0x00772fbc 0, writes nothing |
| rva-lint | OK, 195 known, 0 new |
| captures / G-CTRL / B | not run: top-down row, no menu code changed. The merge train runs the default gates |

## 3. Knob boots (`MASHED_ORIG_TOP=1`, `MASHED_DET_FRAMES=3000`, private root `D:/MashedWT/BIND-CRT-STDIO-CALLERS_root`, each under `mashed_lock.py run`)

| arm | exe | rc | WinMain | overlay `origtop_rw/mashed.log` | `cfglog:` lines |
|---|---|---|---|---|---|
| def_on_rw0 | default | 0 | returned 0 after 3000 frames | 1109 B | 21 |
| ddk_on_rw0 | object | 0 | returned 0 after 3000 frames | 1109 B | 21 |
| def_on_rw0_wdoff (`MASHED_ORIGTOP_WRITE_DIR=off`) | default | 0 | returned 0 after 3000 frames | none | 0 |
| ddk_on_rw0_wdoff (`MASHED_ORIGTOP_WRITE_DIR=off`) | object | 0 | returned 0 after 3000 frames | none | 0 |
| def_on_rw1 (`MASHED_ORIG_TOP_RWMOD=1`) | default | 0 | returned 0 after 3000 frames | 1140 B | 21 |
| ddk_on_rw1 (`MASHED_ORIG_TOP_RWMOD=1`) | object | 0 | returned 0 after 3000 frames | 1140 B | 21 |

- The overlay-off arms log `[BIND-CRT-STDIO] fopen("mashed.log", "wt") refused: write mode, MASHED_ORIGTOP_WRITE_DIR=off, returns NULL` once and write no log, which is the disable_log form.
- `original/mashed.log` was absent after all six boots.
- The default and object builds write the same `mashed.log` text below the date header, on both rw0 and rw1.
- The header is `Fri Nov 13 02:55:45` + CRLF CRLF. The rw1 log has one more line (`Searching piz for fgdc20.txd`), from the RWMOD path's extra load.

Raw files (not committed), sha256:
```
caddac21fb4aebb6c62ab2c525f86b5fd107d1f7e75f5039f64ffc86746dab8c  D:/MashedWT/BIND-CRT-STDIO-CALLERS_data/r42/boots/def_on_rw0.log
5e285238d9a8164a0a8a0c3a99d694176ce0a943dede99b5f6ddae9482abebbd  D:/MashedWT/BIND-CRT-STDIO-CALLERS_data/r42/boots/def_on_rw0.mashed.log
cc1f39f25102908680c80e52deb2b14d1465d312ae14f207f769828d06a6d891  D:/MashedWT/BIND-CRT-STDIO-CALLERS_data/r42/boots/ddk_on_rw0.log
eaaccde83f6eee39da761c9cc2d91facf9f2280f3748410703e7008c171ee98d  D:/MashedWT/BIND-CRT-STDIO-CALLERS_data/r42/boots/ddk_on_rw0.mashed.log
a840673ce0273ed40035ac59d0fe5897ac3a84183f82d8a04adacf29bc844488  D:/MashedWT/BIND-CRT-STDIO-CALLERS_data/r42/boots/def_on_rw0_wdoff.log
02a15030e509b76d5e23f0b69621994e3201611d22296e26e8f5cd13dc262d70  D:/MashedWT/BIND-CRT-STDIO-CALLERS_data/r42/boots/ddk_on_rw0_wdoff.log
d2a276ace43676ca0093fdeba285fd9377b32a3bbda17fc342522b3dd0abf036  D:/MashedWT/BIND-CRT-STDIO-CALLERS_data/r42/boots/def_on_rw1.log
219c05ff8f2a3228ad6351855b73395854f2a35f2e3a47aa8ca31d66007f2952  D:/MashedWT/BIND-CRT-STDIO-CALLERS_data/r42/boots/def_on_rw1.mashed.log
d07f1aed39d7e16699c8694ee03b4d789657aad82c566621294c3d43f7255d3d  D:/MashedWT/BIND-CRT-STDIO-CALLERS_data/r42/boots/ddk_on_rw1.log
a92ab71714e388ae6018426066827357d9489ed6729477911e5dd01c01931f83  D:/MashedWT/BIND-CRT-STDIO-CALLERS_data/r42/boots/ddk_on_rw1.mashed.log
```
Build logs, `BYTECHECK.tsv` and `ddkobj_summary.json`: `D:/MashedWT/BIND-CRT-STDIO-CALLERS_data/r42/`. Boot script:
`D:/MashedWT/BIND-CRT-STDIO-CALLERS_data/r42/boots.sh`.

No promotions: the four functions' C4 status is a match-route matter and is unchanged by this row. No tracker edit.

## Next

- **0x00498510** (controller defaults, contcfg load, `PortTop_Missing_00498510` in `Match/PortTop_InputPort.cpp:131`) is
  the other fopen caller. Its source `re/match/s16/00498510.c` is 95.3%, so it needs an `equiv.py` admission or a
  `PROVISIONAL.tsv` listing before it can be bound (L-126).
- The video-config cluster (`Match/PortSave01_Config.cpp`) still binds its CRT callees to `SettingsCfg*`. Moving it to the
  `MashedCrt_*` names is the last part of the BIND-CRT-STDIO "Next".
