# RESULT BIND-CRT-STDIO (2026-11-11, roadmap run 37): GREEN

The original's stdio and time CRT entry points are bound to the exe CRT on the MASHED_ORIG_TOP path
(`Match/PortTop_CrtStdio.cpp`, exe-only, `// RVA-LINT: ORIGTOP-ONLY`). This is a top-down row: no file the default
path executes changed, so per WORKER_RULES ("Row kinds") the worker ran both builds, the ddkobj check, the knob boots
and the self-test, and left G-CTRL / B to the merge train. Base: `016739dd3` (the handoff) merged with
`orch/integration` @ `778b8b91d`.

## 1. Bindings (8 rows, kind `crt`, `update_bindings_bindcrtstdio.py` through `bindings_rows.apply`)

| rva | function | identity | exe binding |
|---|---|---|---|
| 0x004a4541 | fopen | libid fopen.obj, identical-unlisted | `MashedCrt_fopen_004a4541`: path rule + write overlay (section 2) + fix_fopen contract |
| 0x004a4368 | fclose | libid fclose.obj, identical | `std::fclose` |
| 0x004a49cf | fread | libid ambiguous with _fwrite. Code: calls `__fread_lk` 0x004a48e6 | `std::fread` |
| 0x004a4b22 | fwrite | libid ambiguous with _fread. Code: calls `__fwrite_lk` 0x004a4a1b | `std::fwrite` |
| 0x004a4254 | fputs | libid differs (prologue order, 0.82). Code: lock, strlen, _stbuf, __fwrite_lk, _ftbuf, 0 / -1 | `std::fputs` (0 / EOF on both CRTs) |
| 0x004a3b84 | _vsnprintf | libid vsnprint.obj | `__stdio_common_vsprintf` with the UCRT legacy bits (section 3) |
| 0x004a43b9 | _ctime64 | libid identical x4. Code: localtime 0x004a9858 reads a 64-bit time | `_ctime64` + DDK day padding (section 3) |
| 0x004aa00c | _tzset | libid tzset.obj, identical | `_tzset` + mirror of `_timezone` / `_daylight` / `_dstbias` into 0x00616670 / 74 / 78 with DDK values (section 3) |

FILE objects belong to the exe CRT. None of the 16 matched callers dereferences a FILE field. They only pass the
pointer back into these functions, so the DDK `_iobuf` layout never sees an exe `FILE*`.

**Reach today:** the bindings are inert on the knob path, like BIND-CRT-HEAP's. Every fopen caller on the
`MASHED_ORIG_TOP` path is still a missing stub or has its own forwarder:
- 0x00496490 (log open) is `PortTop_Missing_00496490` (Match/PortTop_00499820.cpp:16).
- 0x00498510 (controller defaults, contcfg load) is `PortTop_Missing_00498510` (Match/PortTop_InputPort.cpp:131).
- The video-config cluster (`Match/PortSave01_Config.cpp`) binds its CRT callees to the `SettingsCfg*` functions.
- `Match/MatchS16_004971b0.cpp` is asi-only.

None of the 12 boot logs has a `[BIND-CRT-STDIO]` line, and no `origtop_rw/` directory was created in the boot root.
A later row that binds one of those callers by `#define FUN_004a4541 MashedCrt_fopen_004a4541` switches them on.

## 2. Write-mode policy (decided): a write overlay beside `original/`

- **Write modes** (any mode with `w`, `a` or `+`) open `<overlay>/<name>`. That covers `"wb"` 0x005d00d8 for the
  contcfg%d.bin / videocfg.bin writers 0x00497230 / 0x004982a0 / 0x004989b0, and `"wt"` 0x005d0004 for the
  mashed.log opener 0x00496490.
  - `<overlay>` is `MASHED_ORIGTOP_WRITE_DIR`, default `origtop_rw`, relative to the cwd. That cwd is the root that
    holds `original/`, the directory the exe already writes `videocfg.bin` / `mashed_re_gamesave.bin` into. On a LAN
    install it is beside the exe.
  - The directories are created as needed.
  - `a` / `+` first copy the `original/` file into the overlay when the overlay has none.
- **Read modes** open the overlay copy when it exists, else `original/`. A config the game wrote reads back on the
  next boot.
- **`MASHED_ORIGTOP_WRITE_DIR=off`** (or `0`) gives the old behaviour: write modes return NULL and log one line.
- `original/` is never opened for writing, so the shared reference install stays the reference.
- The overlay keeps the knob path's files apart from the hand path's own `videocfg.bin` / `mashed_re_*`. That is why
  the policy is an overlay and not "write beside the exe" under the same names.
- `origtop_rw/` is added to `.gitignore`.
- fopen keeps the fix_fopen contract: a result below 0x10000 is returned as NULL.

## 3. exe CRT against DDK CRT (`_tzset` vs `__tzset`, plus `_ctime64` and `_vsnprintf`)

Each probe is one C source built twice: with the XP DDK libcmt (`tools/vc7ddk`, the original's CRT, `/MT`) and with
the VS2022 UCRT (the exe's CRT, `/MT`). Sources and outputs are in `verify/bindcrtstdio_20261111/`.

| probe | finding | binding action |
|---|---|---|
| `tz_probe.c`, 7 TZ settings (unset = system zone "Argentina Standard Time", PST8PDT, EST5EDT, ART3, CET-1CEST, GMT0BST, JST-9) | `_timezone`, `_dstbias`, `_tzname`, localtime in Nov and Jun: equal on all 7. **`_daylight` differs when TZ names a DST zone:** DDK stores the zone name's first character (PST8PDT 80 'P', EST5EDT 69, CET-1CEST 67, GMT0BST 66), UCRT stores 1. Equal (0) for unset TZ, ART3 and JST-9. **`_ctime64` text differs:** DDK `Tue Nov 09`, UCRT `Tue Nov  9` | mirror writes `_tzname[1][0]` to 0x00616674 when TZ is set and daylight is non-zero. `_ctime64` turns a space at character 8 into `0` |
| `edge_probe.c`, `_ctime64` on -1, 0, 86399, year 3000/3001 bounds, 2^44 | -1 gives NULL on both. 0 .. 3001 differ only in the day padding. 2^44 - 1 and 2^44 print garbage on the DDK (no range check) and NULL on the UCRT | out of the game's domain (callers pass the current time). Recorded, not emulated |
| `pf_probe.c` / `pf_probe_ucrt.c`, 26 `_vsnprintf` cases | UCRT default differs on `%e` / `%E` / `%g` (2-digit exponent), `%.0f` of 2.5 (2 vs DDK 3), inf / nan text. With `LEGACY_VSPRINTF_NULL_TERMINATION \| LEGACY_MSVCRT_COMPATIBILITY \| LEGACY_THREE_DIGIT_EXPONENTS` (no STANDARD_ROUNDING), **25 of 26 equal** | binding calls `__stdio_common_vsprintf` with those bits. The one left is `%a`: the DDK does not implement it and prints `a` |

[UNCERTAIN] `_daylight` for a **system** zone that has DST (TZ unset): this machine's zone (Argentina) has none, so
only the no-DST system case was observed (0 on both). Both CRTs derive it from `GetTimeZoneInformation`. Comparing it
needs a run on a machine whose zone observes DST, or a run under a changed system zone.

**Self-test** (`run_selftest.bat`, compiles `PortTop_CrtStdio.cpp` unchanged into a test exe;
`MASHED_CRT_TZ_BASE` points the mirror at a buffer, the exe keeps 0x00616670): **22/22 PASS**
(`selftest_output.txt`). It covers the following:
- Overlay write and read-back, `original/` untouched.
- Append copy-on-write, nested dirs, a missing file returning NULL, the env override and `off`.
- `_vsnprintf` against the DDK: exponent, rounding, truncation, inf.
- The `_tzset` mirror against the DDK (PST8PDT 28800 / 80 / -3600, JST-9 -32400 / 0).
- `_ctime64` against the DDK text.

## 4. Builds, object build, boots

| check | result |
|---|---|
| default build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, .asi not deployed. mashed_re.exe sha256 `5f76ebfdb102997f1a85d55266a39648423fecf95cd012ed6a123fa68a050017` |
| object build (`MASHED_DDKOBJ=1`, NO_DEPLOY) | `=== Build OK ===`, 20 units refused by ddkobj (static-register-convention / wrapper-redefines-globals, unchanged, none in this row). sha256 `d4c51cb013efb69b70c330c1d10f3374312e7b7980133fcfed7a7d10cb998f8b` |
| `ddkobj.py check` | IDENTICAL 541, DIFFERENT 8, not-in-map 3. Equal to the DDKOBJ-SPIKE baseline (`verify/ddkobj_20261111/RESULT_DDKOBJ-SPIKE.md:92-94`: 7 sources below 100% + 0x00414060 operand order). This row adds no matched unit |
| knob boots, `MASHED_ORIG_TOP=1`, 3000 frames, private root `D:/MashedWT/BIND-CRT-STDIO_root` | 6/6 rc 0, `WinMain 0x00492370 returned 0 after 3000 frames`: default RWMOD off, RWMOD on x2, object build RWMOD off / on, default with `MASHED_ORIGTOP_WRITE_DIR=off` |
| captures / G-CTRL / B | not run: top-down row (WORKER_RULES "Row kinds"), no menu code changed. The merge train runs the default gates |
| rva-lint | OK, 35 known, 0 new |
| port column | `scripts/backfill_port.py`: 0 rows changed. fopen stays `body` (hand Save/FsOpen.cpp), the other 7 stay `none` like every `crt` binding (L-67) |
| runnable re-measure | `origtop_size.py`: 1,075 (1,074 at the handoff + 1 from the integration merge, `bind:bound` 51 -> 52). SIZE.tsv updated |

Raw files (not committed), sha256:
```
563446d4a61cb6e069ef1752ea10eba1872cde37edd3304761765c002e93ef2e  D:/MashedWT/BIND-CRT-STDIO_data/boots2/def_on_rw0.log
8367f1d2fccf3bc47ec121b78fe98402cdc34d97b5d4b7d2b82965763a4c13db  D:/MashedWT/BIND-CRT-STDIO_data/boots2/def_on_rw1.log
085f7941bc84dce98d4678502331fb042120f2ef95845f016b5ccb63bb5d8a39  D:/MashedWT/BIND-CRT-STDIO_data/boots2/def_on_rw1b.log
4a81fbb1d7e1af7d954e3fc7eddde769fc2f0a8c6858dcc03b1d92250d3384c6  D:/MashedWT/BIND-CRT-STDIO_data/boots2/ddk_on_rw0.log
4f0dd3fefbf914caa3537534d5cbce55dff591b9e3b00a19518a336cdad03d49  D:/MashedWT/BIND-CRT-STDIO_data/boots2/ddk_on_rw1.log
3f5df0a5a3acc5259d7241d26636b1eff5849a849a3b26bb517245c80a77ef0b  D:/MashedWT/BIND-CRT-STDIO_data/boots2/def_off_w.log
```
The first boot set (`boots/`, before the section 3 fixes) also gave 6/6 rc 0.

## Next

Bind the callers: `PortTop_Missing_00496490` and `PortTop_Missing_00498510`, then move the video-config cluster's
`SettingsCfg*` to the `MashedCrt_*` names on the knob path. After that the overlay and the DDK-faithful formatting
take effect.
