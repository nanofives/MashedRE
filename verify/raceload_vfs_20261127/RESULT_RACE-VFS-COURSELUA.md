# RESULT RACE-VFS-COURSELUA (run 56): G1 met by the wall count only (282 walls + 7 no-row functions on the original's race load); load length not above 3 (race.n 0); stop not moved (still the COURSE.LUA open), its cause moved from "no file system" to "no piz: device"

Branch `orch/RACE-VFS-COURSELUA` from orch/integration 1379467a6. Worker claude-opus-5-5 on claude2. Row kind: race-path row
(top-down: profile data, generated outputs, one hand file on the knob path, one PROVISIONAL.tsv row). PREREG aec67db10
(committed alone, before the arms). G-CTRL / B / 12x4: not run (row kind). No default changed, no new knob, no tracker edit,
no promotion file. `git diff orch/integration HEAD -- re/tools/bind/porttop_bind.py` is empty (RULE 1).

## 0. Verdict

| item | result |
|---|---|
| done-means (decision B) | **wall count delivered** (section 4): of the 1330 functions the original executes in the race_load phase (re/coverage/reach.tsv), 812 bound, 236 library, **282 walls** (hand 38, C4 unbound 85, source not C4 76, no source 83), plus **7 no-row** functions the census cannot see (the piz: device methods) |
| load length (object build, MASHED_ORIG_TOP) | **not above 3**: `meta.json` `race.reached` false, `race.n` 0 on both arms (RO_ROLOADRWC-vfs2, RO_ROLOADRWCC-vfs2) |
| stop | **not moved**: same AV `exe+0x1E824` (`RwCore_005509b0+0x4`) read `00000038`, eax 0, after `cfglog: Unable to find COURSE.LUA in piz` (arm_log_extract.txt). Its cause is narrowed: the RW core FS manager now holds 4 file systems, none is `piz:` |
| what now runs (measured) | `0x00495780 HardwareInstallFileSystem (matched, RW core FS) returned 1: [0x007dc75c] 4 file systems, default [0x007dc76c] 017F9E58` (RO_ROLOADRWC-vfs2.log:92). Before this row: `missing (0x004955d0 unported ...)`, count 0 at the COURSE.LUA open (RACE-LOAD-LUA-2 section 4) |
| stop met and crossed on the way | 0x00551460 (RtFS path join, called by the RtFS exists method 0x005512d0): the first arm after the FS install stopped there (`[OT_TIMER] STOP: ... called original 0x00551460`, RO_ROLOADRWC-vfs1.log:181, return address in `_PortTop_Bound_005512d0`). Bound as a listed provisional body (section 2), the vfs2 arms run past it to the COURSE.LUA open |
| next wall | the `piz:` file system (section 3): 0x004b6640 (C2, 60.5%, equiv NOT-PROVEN call-regargs) and its 7 methods with no source and no hooks.csv row, plus the original piz mount chain in place of the exe's hand reader |
| race frame body on a knob-on arm | **no** (race.n 0) |
| hash moved | none run (row kind); ddkobj DIFFERENT set unchanged |

## 1. Change

1. `re/tools/bind/state/raceload/roots.txt` + 0x00495780, 0x004955d0, 0x00550790, 0x00551460 (with comment lines).
   `re/tools/bind/state/raceload/rules.tsv` + `prov_bind 00551460` (one rule, one comment).
2. Generated with the tool (`write --profile raceload --roots-only`, `sync --profile raceload --step RACE-VFS-COURSELUA`,
   `write --profile audio`, `bindings --profile audio`): `Match/PortTop_{00495780,004955d0,00550790,00551460}.cpp`,
   raceload plan.json / newfiles.txt, audio plan.json, `Match/PortTop_005512d0.cpp` and `Match/PortTop_00550c50.cpp`
   (`#define FUN_00551460 PortTop_AStop_00551460` -> `PortTop_Bound_00551460`), `Match/PortTop_AudioStops.cpp` (the
   0x00551460 stop removed), `exe_sources.rsp` (+4), `dual_copy_allowlist.txt` (+1, 0x00550790 ORIGTOP-ONLY CROSS-TARGET with
   `Util/PromoLoop_round17.cpp`), `verify/porttop_20261108/BINDINGS.tsv` (1 row updated). `Match/PortTop_BulkRefs.cpp`
   unchanged against orch/integration. 0x00551190 / 0x00550430 reuse the exe bodies of PORT-TOP-AUDIO-HWINIT-4627F0.
3. Audio state, one line, granted by L0 in the ownership table (diff):
   ```
   --- a/re/tools/bind/state/audio/rules.tsv
   +++ b/re/tools/bind/state/audio/rules.tsv
   +alias	00551460	PortTop_Bound_00551460	Match/PortTop_00551460.cpp
   ```
   Nothing else in `re/tools/bind/state/audio/` changed except the generated plan.json.
4. Hand file `Match/PortTop_00493710.cpp`: `PortTop_Thunk_004951e0` (the call 0x00493710 makes for 0x004951e0 = `jmp 0x00495780`)
   runs `PortTop_Bound_00495780` when `RwCore_FrameRoute()` (MASHED_RWC_CORE=1 + MASHED_RWC_FRAME=1, both default OFF), the
   predicate under which RtFSManagerOpen already runs `RwCore_00550350` (LibRw/RwApiBind.cpp, [RWC-FRAME]). Off that route the
   missing stub runs as before (kb_vs1.log:98).
5. `re/match/PROVISIONAL.tsv` + 0x00551460 (status provisional, equiv line, caller scan), section 2.
6. `re/orchestrator/UNCERTAIN_REGISTER.md` + one line (section 3).

## 2. Bodies bound, with class (RULE 9: match and replay apart)

| rva | name | source | class | match.py --bytes | relocheck | object build |
|---|---|---|---|---|---|---|
| 0x00495780 | HardwareInstallFileSystem | re/match/s16/00495780.c | match (C4) | BYTE-IDENTICAL 12 B | OK checked 1 | IDENTICAL (BYTECHECK.tsv) |
| 0x004955d0 | FUN_004955d0 | re/match/s16/004955d0.c | match (C4) | BYTE-IDENTICAL 419 B | OK checked 7, unchecked 7 (CRT cookie + 5 kernel32/user32 imports) | IDENTICAL |
| 0x00550790 | FsManager7dc76cSet | re/match/s24/00550790.c | match (C4) | BYTE-IDENTICAL 10 B | OK checked 1 | IDENTICAL |
| 0x00551460 | FUN_00551460 | re/match/rwl04/00551460.c | **provisional** (C2, 37.9%) | not identical (match ledger REFUSED, 111 bytes) | not run | skip:provisional, VS2022 body kept (provisional kept 255 -> 256) |

Counts: match 3, replay-C4 0, provisional 1. 0x00551460: `equiv.py` (tool commit 1ed7ceb1a) `NOT-PROVEN unsupported: instruction
repne scasb al, byte ptr es:[edi]` (equiv_00551460.txt), not a conditional verdict. Caller scan (re/match, mashedmod/src): 2 callers,
both read the return (zero test): 0x005512d0 (re/match/s24/005512d0.c:32) and 0x00550c50 (re/match/s24/00550c50.c:50, returns 5
on 0). Its PROVISIONAL.tsv row carries both lines. Not C4 by this status (L-126 / L-128).

**Binding numbers (RO_ROLOADRWC-vfs2):**
- executing: 0x00495780, 0x004955d0 (log line 92), the reused 0x00551190 / 0x00550430 (the count went 0 -> 4), 0x00550790 (default
  device set, `[0x007dc76c]` 017F9E58), 0x00551460 (the vfs1 stop at this call is gone on vfs2 and the arm reaches the COURSE.LUA
  open). Call counts not measured.
- reachable from a bound caller, not executed: none new.
- pre-bound keep-alive: 0 (Match/PortTop_BulkRefs.cpp unchanged).
- bindable rows left unbound, with reason: the original piz mount chain 0x00495280 / 0x004b6570 / 0x004b6940 (C4 match each,
  `load_walls_race_load.tsv` class c4_unbound) stays on the hand reader `PortTop_PizFs_Mount_00495280`: replacing it changes every
  piz consumer on the knob path, and it opens nothing without the piz: device below. 0x004b6640: C2 (section 3).

## 3. The next wall: the piz: file system (measured stop, cause read from sources)

At the original's COURSE.LUA-site open (0x0047b910) the default device is `piz:` (RACE-LOAD-LUA-2, vfs_boot1.jsonl). On the exe the
log reads `cfglog: Searching piz for COURSE.LUA` / `Unable to find COURSE.LUA in piz` and VFS_Open returns 0 on a default device
that is a disk file system. The piz: device:

| rva | what (decomp_pc.py --create, pizdev_decomp.txt) | hooks.csv | source | census |
|---|---|---|---|---|
| 0x004b6640 | RwMalloc 0x58 (RwGlobals +0x108), 8 method slots, DAT_007d3e50 / 007d3e54 / 008cd9f8 / 008cda20 / 008eda70 / 008eda98, 0x00551330(p, 2, ..), 0x00550430(p) | C2 | re/match/s18/004b6640.c 60.5%, equiv NOT-PROVEN call-regargs at orig 0x004b6640 call 0x00551330 (equiv_004b6640.txt) | reached_menu 1 |
| 0x004b6630 | returns &DAT_008cd9c0 + n * 0x20078 (slot +0x14) | no row | none | not in census |
| 0x004b6c00 | open: name into 0x008ab860 ring, stricmp against the 0x0090dac0 entry table (DAT_008ad9a8 entries, stride 0x80) (slot +0x28) | no row | none | not in census |
| 0x004b6f60 | close (slot +0x2c) | no row | none | not in census |
| 0x004b6cd0 | read, 0x20000 window (slot +0x30) | no row | none | not in census |
| 0x004b6ed0 | seek, modes 1 / 2 / 3 (slot +0x38) | no row | none | not in census |
| 0x004b6f90 | eof (slot +0x44) | no row | none | not in census |
| 0x004b6ba0 | exists: stricmp over the entry table (slot +0x4c) | no row | none | not in census |
| 0x004b6b10 | __stricmp | C2 | none | reached 1 |

The 7 methods are outside `re/analysis/function_inventory.csv` (Ghidra-missed). Their callees 0x004b6b00, 0x004b6b20, 0x004b6b90 are
C4 with sources. The methods read the original piz globals (0x0090dac0 table, DAT_008ad9a8, DAT_007d3e64 / 0x007d3e68 window), which
only the original mount chain 0x004b6940 writes; the exe's hand reader writes none of them (RACE-LOAD-LUA-2 section 4).
[UNCERTAIN] which installer each of the original's 6 devices comes from (register line): the exe's measured 4 equal 6 minus piz:
minus the audio init's 0x004624c0 device under a reading of the sources, not a watch.

Crossing it needs: hooks.csv rows + sources for the 7 methods (match or provisional), 0x004b6640 provisional (or matched), the
original mount chain bound on the RW core route beside or instead of the hand reader (a knob-path choice), and an arm. That is match
and source work, not a data-only binding, so this row stops here.

## 4. Wall count (decision B second form)

Tool `verify/raceload_vfs_20261127/load_walls.py` (committed). Inputs: `re/coverage/reach.tsv` (the original's INT3 census, phase
race_load = warp poke DAT_00771968 = 2 until phase 3, the race anchor), the binder's exe-body index on this row's tree
(`porttop_bind.py index`, 2845 rvas from 3030 TUs of exe_sources.rsp), hooks.csv, re/match sources, PROVISIONAL / ADMITTED.
Table: `load_walls_race_load.tsv` (one row per function, class, source, exe body).

| class (race_load phase, 1330 functions) | count |
|---|---|
| bound (an exe TU compiles its re/match source) | 812 |
| lib (Lua / CRT / zlib / libpng / qhull, bound by symbol through the lib / crt routes) | 236 |
| hand (only a hand exe body) | 38 |
| C4 unbound (bindable data rows) | 85 |
| source, not C4 (provisional 25, admitted 4, none 47) | 76 |
| no source | 83 |
| **walls** (hand + C4 unbound + source not C4 + no source) | **282** |
| no row (not in the census, listed by hand: the 7 piz: methods) | 7 |

On the head index (before this row) the same count was 284 (bound 810, C4 unbound 86, source not C4 77): this row bound two race_load
functions, 0x00550790 (C4) and 0x00551460 (provisional). Walls by subsystem are printed by the tool: third-party-library 140 of the 282
(71 of them [renderware]), render 45.
Limits, listed: the census has no call order, so the 282 are all unbound functions of the load, not only those after the stop (the
exe passed hand and lib bodies before it); `bound` means an exe body exists, not that the arm executed it; the census arms hooks.csv
VAs only (functions with no row are invisible, so the no-row list is a floor). Phase variant `--phase race_not_menu` (1519 functions)
is printed by the tool for reference.

## 5. Checks run

| check | result |
|---|---|
| mashed_lock.py status before every build, check, selftest, arm | free each time except one build start: HELD by DDKOBJ-ADDR-REGISTERED (not PRIORITY-HOLD / SOLO), build ran |
| C: free before builds | df: 35 GB, 55 GB, 52 GB (L0 disk hold at 21 GB respected: no build or arm while held). test-slot --status printed once, before the first build (not before the two rebuilds) |
| VS2022 build (build_vs1 / vs2 / vs3) | `=== Build OK ===`; rva-lint 659 known, NEW 0 |
| object build MASHED_DDKOBJ=1 (build_ddk1 / ddk2 / ddk3) | `=== Build OK ===`, mashed_re_ddk.exe 0f11d850... |
| ddkobj.py check (final) | IDENTICAL 2363/2529, DIFFERENT 78, not-in-map 88, units linked 2408/2698, refusals 34 (13 + 21), provisional kept VS2022 256 units + 7 fns, dual body 33 |
| vs head check log\orch\run56_headcheck\ (2360/2526, DIFFERENT 78, linked 2405/2694) | IDENTICAL +3 (0x00495780, 0x004955d0, 0x00550790), DIFFERENT set identical (0 new, 0 gone), provisional kept +1 (0x00551460) |
| newly DIFFERENT units to classify | none. Listed unit -> 100% neighbour pairs that fell back to VS2022: none (0x00551460 is alone in its TU) |
| match.py --bytes + relocheck.py on the included sources | section 2 (3 BYTE-IDENTICAL + OK; 0x00551460 provisional) |
| equiv.py | 0x00551460 NOT-PROVEN (unsupported repne scasb); 0x004b6640 NOT-PROVEN (call-regargs) |
| binder selftest (final tree) | **PASS** (selftest_row.txt). Intermediate FAILs, fixed by regenerating with the tool: wrappers + BulkRefs stale after the hand-file edit (FAIL 2), BINDINGS.tsv after the audio plan change (FAIL 1) |
| knob boot VS2022 exe (MASHED_ORIG_TOP=1 MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=600) | rc 0, `WinMain 0x00492370 returned 0 after 600 frames`, FS install still the missing stub off the RWC route (kb_vs1.log:98) |
| arms vfs1 without image (invalid, kept as `_noimage`) | `[portstate-image] REFUSED(missing)`; stop 0x00551460 |
| arms vfs1 with staged image (extract_image.py, sha256 81CC7775...) | both exit 0x00551460 (5575776) after 1 s, stop 0x00551460 |
| arms vfs2 (final) | both exit 0xC0000005, 7-9 s, the COURSE.LUA stop; meta selftest 22196 checks 0 fails, race.n 0, warp n 2402 tick 300 |
| frida-<hash> folders in %LOCALAPPDATA%\Temp | counts only before: 698 (PowerShell, 10:3x, L0 disk-hold reply) and 3 (bash glob, before the first arm); no name list was taken before. After: 6, names saved to D:/MashedWT/RACE-VFS-COURSELUA_data/frida_dirs_after.txt. **Deleted 0**: this row started no Frida process (its tools were build.bat, ddkobj.py, porttop_bind.py, match.py, relocheck.py, equiv.py, decomp_pc.py on Ghidra headless, knob_boot.py, run_arms.py), so no folder is attributable to it |

## 6. Follow-ups (for L0 / lead)

- Piz: file system row (section 3): hooks.csv rows + sources for 0x004b6630, 0x004b6c00, 0x004b6f60, 0x004b6cd0, 0x004b6f90,
  0x004b6ed0, 0x004b6ba0; 0x004b6640 provisional; the original mount chain on the RW core route; then the ROLOADRWC arm.
- The audio alias line is in this branch (L0 grant); no separate row needed.
- L-114 (b) 0x0047b9b0 hand runner: still cannot retire (no arm reaches lua_dobuffer).

## 7. Evidence

Committed (`verify/raceload_vfs_20261127/`): this RESULT, PREREG_RACE-VFS-COURSELUA.md, load_walls.py, load_walls_race_load.tsv,
selftest_row.txt, equiv_00551460.txt, equiv_004b6640.txt, arm_log_extract.txt.

Raw (not committed), `D:/MashedWT/RACE-VFS-COURSELUA_data`:

| file | sha256 |
|---|---|
| arms/RO_ROLOADRWC-vfs2.log | 8a64c905673b174b5b3dfd74a151b0a94a126e25e2347cbefe0057cbbb0dbb70 |
| arms/RO_ROLOADRWCC-vfs2.log | ec647bb5e0ce98bbb38cc887e2fda0f63782a931649154a940911a6e7c9e5ece |
| arms/RO_ROLOADRWC-vfs2/meta.json | 88dda92bda679f9e0fa1b485b26d074488496f57b9ae5dfa7e8f12066f942d28 |
| arms/RO_ROLOADRWCC-vfs2/meta.json | 485c685a8cddff4959d4ff7755a484710b49858f4fe54e3f03197bc73a82f7d5 |
| arms/RO_ROLOADRWC-vfs1.log | 92d8c1c140c4fb9b4d927a2a14141dfeb4014cb1db9aa56101438f6598b030a9 |
| arms/RO_ROLOADRWC-vfs1_noimage.log | a044638fd5adfb8e16c9ed878e5677ceea787b056d6d2838a327b60211ca8925 |
| kb_vs1.log | 4292fcfb444ec5404ec47894d417c1e2c1ac3f5b2e9944899022e61108bcb674 |
| build_vs3.log / build_ddk3.log | fe35232627ac0d59b67c6150f285f77907375ed6a2ec4017dd626141e5eed92d / dced60edf9f55a634b18190d15c35f7a03cbe8585f7237252cdf84de06a40241 |
| ddkobj_check3.txt | cbb866dd144aec2a428b9a43f1b5d745356067656622f5d50eaf827f9a07742d |
| pizdev_decomp.txt (decomp_pc.py --create --port, pool slot 0) | 2ddbd7d41ab13db7a90797573e95530a8614cce12a8aec0dcc8c52a0438cba51 |
| worktree mashedmod/build/ddkobj/mashed_re_ddk.exe | 0f11d85068a4639d7dae094863ec1d87fc2a9e3278a4aa985515eeba9eb945a6 |
| worktree mashedmod/build/mashed_re.exe | c9934b02bcc8e30746a8add154dc0c1002904f298248994bf9ced3a8cd8fc403 |
| root original/mashed_image.bin | 81cc77758ffe93ac0b681eb1fed3d63912d25e7d059503af6d4f871dea9afcd8 |

## 8. Unfinished scope (listed, not implied)

- Load length above 3: not reached (race.n 0).
- COURSE.LUA open: still fails (piz: device, section 3).
- 0x00551460 replay / in-situ check: not run (provisional only).
- The original-side device-per-installer watch: not run (register line).
