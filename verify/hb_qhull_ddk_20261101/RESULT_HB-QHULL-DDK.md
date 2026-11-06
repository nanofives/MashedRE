# RESULT HB-QHULL-DDK: vendored qhull 2002.1 built with the XP DDK compiler

Date 2026-11-01. Branch `orch/HB-QHULL-DDK` (from `orch/integration` @ `0e3d71a75`). Worker on claude2.
Task: Mariano 2026-11-01, try building `mashedmod/deps/qhull-2002.1` with XP DDK cl 13.00.9176 / c2 13.00.9178
(`tools/vc7ddk`) at `/O2 /G5 /Ob2 /GF`. Propose the `build_qhull.bat` diff (not applied), link-check both targets,
match-check the 116 qhull rows by libid. No promotion in this step.

## Verdict

- **Links: yes.** `mashed_re.exe` and `mashed_re_dev.asi` both link with the DDK-built `qhull_2002_1.lib`, through
  `mashedmod\build.bat` unchanged (`MASHED_NO_DEPLOY=1`, `original\mashed_re_dev.asi` untouched, still the 2026-10-24
  file). The lib needed two extra switches over the plain flags: `/Zl` and `legacy_stdio_definitions.lib` merged into
  the archive (section 2).
- **Match: 116 of 116 identical.** Every one of the 116 LIB-IDENTITY qhull rows is identical to its library function in
  the port's own lib (`re/tools/match/libmatch.py`), and so are the 6 rows MATCH-LIB added later: 122 of 313
  `third-party-library[qhull-2002.1*` rows (116 C2 + 6 C1), 0 ambiguous, 152 differ, 39 not compared.
  `identical_rows.tsv` lists them.
- **Diff:** `verify/hb_qhull_ddk_20261101/build_qhull_bat_ddk.diff` (43 changed lines, `git apply --check` OK on
  `orch/HB-QHULL-DDK`). Running the patched .bat itself (scratch copy) gives the same per-row libmatch result as the
  driver used for the link check.
- **Blocker for the planned C4 by libid: the port never calls qhull.** Neither target executes any qhull function
  (section 3). Under the callerless ruling (L-68, `re/CONFIDENCE.md` `libid`: "Identity without the port using the
  source is not C4"), the 116 rows cannot be promoted after the diff is applied, gates or not. They become C4 by libid
  once a ported caller of `0x0047d3c0` lands.
- **Applying the diff changes code layout in both targets** (section 4): it drops 188,864 bytes (0x2e1c0) of exe dead qhull code that the
  VS2022 build links in. The `-Full` gates must run after the diff is applied, before any merge.

## 1. Build and match

Compiler: `C:\Users\maria\Desktop\Proyectos\Mashed\tools\vc7ddk\bin\cl.exe` (13.00.9176, c2 13.00.9178), headers
`tools\vc7ddk\inc\crt`, objects carry `@comp.id` 28/9178. Flags `/nologo /c /W0 /O2 /G5 /Ob2 /GF /Zl`
(`/O2` already contains `/Ob2 /GF` in 13.00). Archive by VS2022 `lib.exe`, link by VS2022 `link.exe`.

```
py -3.12 re/tools/match/libmatch.py --objs mashedmod/deps/qhull-2002.1/qhull_2002_1.lib \
    --subsystem "third-party-library[qhull-2002.1" --tsv verify/hb_qhull_ddk_20261101/qhull_ddk_lib_libmatch.tsv
qhull DDK /O2 /G5 /Ob2 /GF /Zl (port lib, worktree) rows 313 identical 122 (ambiguous 0) differs 152
    not-compared 39 obj-functions 589 unused 315 identical-at-unlisted 0
```

`obj-functions 589` counts the merged `legacy_stdio_definitions.lib` members too (366 without them, same 122).
Set compare against `verify/libidentity_20261030/raw/qhull_O2_G5.tsv`: the 116 identical rows there are all identical
here, each with the same library function name. The 6 new ones are that run's `identical-unlisted` addresses
(`0x00585360 0x00585d20 0x00585d50 0x0059dd50 0x0059e720 0x005a5790`), which MATCH-LIB gave C1 rows.
`/Zl` was checked separately: the lib without it gave the same 122.

## 2. Link fixes (both are in the diff, build.bat is unchanged)

1. `LNK1104: cannot open 'LIBC.lib'` (exe link). Every DDK object carries `.drectve`
   `/DEFAULTLIB:"LIBC" /DEFAULTLIB:"OLDNAMES"`. VS2022 ships no single-threaded `LIBC.lib`. Fix: `/Zl` (omit
   default-library names). It changes no code byte (libmatch counts above).
2. `LNK2019: _sprintf` in `_qh_option` (global.obj), `.asi` link only. The DDK `stdio.h` calls `sprintf` as an
   external. The UCRT defines it inline in its headers, so the external lives in `legacy_stdio_definitions.lib`. The exe
   linked anyway because `VideoCfgFirstRun.obj` emits an external `_sprintf` COMDAT. Fix: merge
   `legacy_stdio_definitions.lib` into `qhull_2002_1.lib` on the `lib` line.

CRT imports of the DDK objects, all resolved by the VS2022 static CRT: `___mb_cur_max __fltused __ftol __iob
__isctype __pctype __setjmp3 _calloc _clock _exit _fgets _floor _fprintf _fputs _free _freopen _localtime _longjmp
_malloc _qsort _sprintf _strchr _strncat _strncmp _strncpy _strpbrk _strrchr _strstr _strtod _strtol _time _toupper`.
[UNCERTAIN] ABI parity of three of them with the UCRT, untested because no qhull code runs (section 3):
`__iob` (DDK `stderr` = `&_iob[2]` with a 32-byte `FILE`; the exe map resolves `__iob` to `libucrt:_file.obj`),
`_time` / `_localtime` (DDK `time_t` is 32-bit), and the ctype macros (`__pctype`, `___mb_cur_max`, `__isctype`).
On the bridge path these are off: `QhullBuildHull` passes `file` (0 from `ConeCastHullBuild`) as both `outfile` and
`errfile`, the option string is `"qhull s Pp"`, and in the library TUs `stderr` / `stdout` / `stdin` / `time` /
`localtime` appear only in error, trace and option (`QR`, `T*`) paths (`global.c:1176,1219,1259,1530`,
`io.c:527`, `mem.c:11`, `merge.c:3466`, `qhull.c:443,465`). They need a check once a caller lands.

## 3. The port never calls qhull (callerless, L-68)

Port call chain, from source: `VehiclePhysicsWorldCreate` 0x0047d3c0 (`Collision/PhysicsWorldBuild.cpp`, C2) ->
`PhysicsCollisionBodyCreate` 0x004826d0 (`Collision/CollisionBodyCreate.cpp:40`, C2) -> `ConeCastHullBuild` 0x00481e00
(`:54`, C2) -> `QhullBuildHull` 0x0057ca30 (`Collision/QhullBridge.cpp:231`, C2) -> `qh_new_qhull` and the rest.
Nothing in `mashedmod/src/mashed_re` calls `VehiclePhysicsWorldCreate` (only its declaration,
`Collision/CollisionBuildDeps.h:30`). The other `QhullBuildHull` caller is `b5b_qhull_selftest.cpp`, a separate
self-test exe.

Byte evidence (`presence.py`: each qhull lib function of 24 bytes or more, relocation sites masked, searched in the
linked image):

| image | qhull functions present |
|---|---|
| `MASHED.exe.unpatched` (control) | 119 of 360 |
| `mashed_re.exe`, DDK lib | 0 of 360 |
| `mashed_re_dev.asi`, DDK lib | 0 of 360 |

The DDK maps list 5 `qhull_2002_1:` entries, all data (`_qhmem`, `_qh_rand_seed`, `firstcall` in `qh_new_qhull`, two
`__real@` constants), and no `QhullBridge_*.obj`, `CollisionBodyCreate` or `VehiclePhysicsWorldCreate` function.

So the rule's second condition fails for all 116: the port links the matching source, but `mashed_re.exe` does not use
it. Promotion waits for a ported caller of 0x0047d3c0 that the default build executes. Then the 116 (and the 6 C1 rows,
after their own C2 step) qualify by libid with this lib, plus the merge-train gates.

## 4. Layout change: dead qhull code (0x2e1c0 bytes in the exe) leaves both targets

The same worktree relinked with today's VS2022 lib (the unmodified `build_qhull.bat`, `/Ox /arch:IA32 /fp:precise`,
no `/Gy`):

| section | DDK lib | VS2022 lib |
|---|---|---|
| exe `.text` vsize | `0x1c912f` | `0x1f72ef` (+`0x2e1c0`) |
| exe `.rdata` vsize | `0x5bd52` | `0x5cc10` |
| asi `.text` vsize | `0x1082fe` | `0x139ece` (+`0x31bd0`) |
| `qhull_2002_1:` lines in exe map | 5 | 1,890 (`_qh_new_qhull` at `0x00570e70`, `_qh_initflags` `0x00571fe0`) |

The VS2022 objects have one `.text$mn` per object, so the reference from `QhullBridge_exe.obj` pulls whole objects in
and `/OPT:REF` cannot drop them. The DDK objects are function-level COMDATs (`/O2` implies `/Gy`), so the unreferenced
code goes. No executed code changes, but every function linked after the qhull block moves. Memory
`sim-trajectory-is-code-layout-sensitive`: dead-code layout has flipped roundabout / Storm / Highway trajectories
before, and TRAINING G-CTRL has not. So run the merge-train `-Full` gates (`gctrl.ref` and `b.ref` in
`re/orchestrator/merge_train.json`) after the diff is applied, before merging.

## 5. Applying the diff (Mariano)

```
git apply verify/hb_qhull_ddk_20261101/build_qhull_bat_ddk.diff
del mashedmod\deps\qhull-2002.1\qhull_2002_1.lib
```

Delete the old lib in every checkout that has one: `build.bat` rebuilds qhull only when the lib is missing
(`build.bat:57-61`), so a stale VS2022 lib would stay linked. `publish.sh` builds from a clean `git archive`, so it
rebuilds. The patched .bat stops with `[ERROR] XP DDK compiler not found` on a PC without `tools\vc7ddk`
(gitignored), on purpose: falling back to VS2022 would ship non-matching bytes. `MASHED_DDK` points it at another DDK
root. LIVING-PC needs its own copy of `tools\vc7ddk` before it can build.

## Gates

Not run in this step. The gates come after the diff is applied (section 4).

## Files

Committed (`verify/hb_qhull_ddk_20261101/`):
- `RESULT_HB-QHULL-DDK.md` (this file)
- `build_qhull_bat_ddk.diff`: the proposed `build_qhull.bat` change
- `qhull_ddk_lib_libmatch.tsv`: libmatch per-row output, 313 rows
- `identical_rows.tsv`: the 122 identical rows (116 + 6)
- `libmatch_summary.txt`: the libmatch summary lines (driver lib and the patched .bat's own lib)

Raw, not committed: `D:/MashedWT/HB-QHULL-DDK_data/` (SHA-256 in its `SHA256SUMS.txt`):

| file | SHA-256 |
|---|---|
| `ddk_bins/mashed_re.exe` | `c3a7224a867cb8f7e1e0ed7d075613deeca5b5d4e77738a7118468ef760dc40a` |
| `ddk_bins/mashed_re_dev.asi` | `401950ccfa80adaa3a28278dd6784ecc8fb883919f4c56b06d282ddbf459c4bb` |
| `ddk_bins/mashed_re.map` | `4df60415eb746044e08452f8a9358242dff99bf232ed712aa0c12a3db8f5766a` |
| `ddk_bins/mashed_re_dev.map` | `4b9541597b09b07e29e483972d8b822445c1255958e88dbca217ada56ba5717f` |
| `ddk_bins/qhull_2002_1.lib` | `057b90eb6ab2c16afc2e070e938e654d11af070532975da638197beb7eda2686` |
| `vs_bins/mashed_re.exe` | `155912e6aaba1687c494d4c83d73157cfb29e627fda4a73e3a0c5bc8190f2a50` |
| `vs_bins/mashed_re_dev.asi` | `e5436311ea8efe77d32b24944f6ad877d7e9f623035f8c60c2c32992b0e9034c` |
| `vs_bins/mashed_re.map` | `1520ac82dd047c18e3d53329015fdd87a255eb1f7690a0d6b625297c6895c272` |
| `vs_bins/qhull_2002_1_vs2022.lib` | `79bab0ca59726dd7984e4c44f4100f5143d964de466c882d6cb44b19c5c18779` |
| `build_full.log` (LNK1104 LIBC) | `afc89454d490756b1389517313c6719f08681bd04ef3a936b1f6abd3018f3185` |
| `build_full2.log` (asi LNK2019 `_sprintf`) | `84b20b2898476013f57f0fc46ef0cdc8eee834fd0a36176443ae7d1693c93a03` |
| `build_full3.log` (both link) | `f88ee02eb37fc889d0f23a0bd950dcfa34cd23ee6f03ca66ff81ee9cc98fa769` |
| `build_vs.log` (VS2022 lib relink) | `4c42629b0b3498c86bab9f7e5efd97b5216e53257894e83643963682c46591b7` |
| `build_qhull.proposed.bat` | `8a2b30a88207d41cd2397eecc2d7b87e09da749fd1b06c8f0bc9a6cb37351643` |
| `build_qhull_ddk_driver.bat` (link-check driver) | `2be3b3c6d149cb3ba55283baac4dc644e2fe80a3cedc1a9610eb2fc0912ddd77` |
| `presence.py` | `f315c9bdb61e22b830b38d05712bb32aceada1074b034df6abd73b4d53df3c70` |
