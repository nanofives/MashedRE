not met, stop moved from the COURSE.LUA open (AV exe+0x1E824, RwCore_005509b0+0x4, "Unable to find COURSE.LUA in piz") to the COURSE.LUA script's Setup_Fog handler 0x0047ab30 (AV exe+0x61741, write [0+0x1EC]) on the object build; CUT wall count 108 (of 303 functions the original first runs after the 0x0047a020 enter)

# RESULT RACE-PIZ-DEVICE (run 56, race-path row, top-down kind)

Branch `orch/RACE-PIZ-DEVICE`, worktree `.worktrees\RACE-PIZ-DEVICE`. Prep stacked on RACE-VFS-COURSELUA 028c09d3c, then
merged with orch/integration 2637e5b37 (RACE-VFS-COURSELUA merged; merge commit 36d800fec, one conflict in the bulk
moved.tsv ledger, resolved by taking integration's copy and re-adding this row's line with `porttop_bind.py moved`).
Every build, check, selftest and arm counted below ran on the merged tree. PREREG 2f826d16e (committed alone, before any
arm). Worker claude-opus-5-5 high on claude3 (moved from claude2 for Ghidra MCP). G-CTRL / B / 12x4: not run (row kind).
No new knob, no default change. `git diff orch/integration HEAD -- re/tools/bind/porttop_bind.py` is empty (RULE 1).

## 0. Verdict

| item | result |
|---|---|
| done-means (decision B) | **load length not above 3** (`race.n` 0, `race.reached` false on both object-build arms). **CUT wall count delivered** (section 4): **108 walls** among the 303 functions the original first executes after the first enter of 0x0047a020 (CourseDescInit, the caller of the COURSE.LUA load) and before the race anchor |
| stop, object build | **moved** from the COURSE.LUA open (RACE-VFS-COURSELUA vfs3: `Unable to find COURSE.LUA in piz`, AV exe+0x1E824) to `PortTop_Bound_0047ab30` +0x21 (exe+0x61741, write 0x000001EC, ecx 0), called by Lua (`callCclosure` <- `luaD_call` <- `luaV_execute` <- `lua_dobuffer` <- `PortTop_Bound_0047b8a0` <- `PortTop_Bound_0047b8d0`): COURSE.LUA is now opened from the track piz, read, and executed by the Lua VM up to its first `Setup_Fog` call |
| cause of the new stop | a ddkobj class, not this row's code (section 3): `re/match/s13/0047ab30.c` declares DAT_006bf1cc as a C file-static, the DDK object reads that private zero copy, not 0x006bf1cc, which the VS2022 0x0047a020 body sets to 0x00644158. 17 DDK units of this object build share the class |
| beyond it (VS2022 build, diagnostic only) | COURSE.LUA runs to its end, 0x00479330 loads `textures.txd` (1,577,932 bytes from the piz), stop at `Rwl_StreamFindChunk` +0xf5 (hand librw stream reader, write AV) under PortTop_Bound_0042a6b0 / 0x00479330 |
| race frame body on a knob-on arm | **no** |
| hash moved | none run (row kind); ddkobj DIFFERENT count 79 = L0 head 79 (section 5) |

## 1. Change

1. **Sources** (`re/match/s18/`, all `// FLAGS: /O2 /GS /Ob1`, DDK): the 7 piz: device methods, written from the
   disassembly (Ghidra pool slot Mashed_pool10, read-only). Static register-argument helpers 0x004b6b00 / 0x004b6b20 /
   0x004b6b90 are copied into the sources that call them, as `re/match/s18/004b6940.c` does for 0x004b6710.
2. **hooks.csv**: 7 C1 rows (`re/tools/c1_add.py`, render, identity = the method slot 0x004b6640 stores each address
   in). `re/promotions/RACE-PIZ-DEVICE.tsv`: the 5 byte-matched rows, route match (applied by the train; dry run below).
3. **re/match/PROVISIONAL.tsv** + 0x004b6640, 0x004b6c00, 0x004b6cd0 (status provisional, equiv verdict, caller scan).
4. **Profile data** (raceload): roots + 0x004b6640, the 7 methods, 0x004b6570, 0x004b6940, 0x004b67a0, 0x004b6770,
   0x004b65e0, 0x004b67e0, 0x005507a0; rules `prov_bind` 0x004b6640 / 0x004b6c00 / 0x004b6cd0 and three raceload-only
   CRT aliases to hand bodies in `Match/PortTop_00402750_Pieces.cpp`: 0x004a34b0 -> std::strncpy, 0x004b6b10 and
   0x004b302f -> _stricmp. Generated with the tool: 15 new `Match/PortTop_<rva>.cpp`, raceload plan / newfiles,
   `PortTop_RaceLoadStops.cpp` (+1 stop 0x005c1d63 _fseek, reached only in fopen mode, mode flag is 0 on every arm),
   `exe_sources.rsp` (+15), `dual_copy_allowlist.txt` (+2 ORIGTOP-ONLY: 0x004b65e0, 0x004b67a0). Bulk profile (RULE 3,
   regenerated with the tool): `Match/PortTop_004b6590.cpp` now reuses PortTop_Bound_004b67a0 instead of its named stop,
   `PortTop_BulkStops.cpp` -1 stop, bulk plan.json, moved.tsv +2 lines (0x004b67a0 stop -> reuse with cause, totals).
5. **Hand file** `Match/PortTop_00402750_Pieces.cpp`, only when `RwCore_FrameRoute()` (MASHED_RWC_CORE=1 +
   MASHED_RWC_FRAME=1, both default OFF): `PortTop_PizFs_Init_004b6540` also runs PortTop_Bound_004b6640(flag) (0x004b6540
   is a jmp to 0x004b6640); `PortTop_PizFs_Mount_00495280` runs PortTop_Bound_004b6570(exe path) after the exe reader has
   read the whole archive and closed it; `PortTop_PizFs_Unmount_004952f0` first runs PortTop_Bound_004b67a0 (the
   target of the thunk 0x004b6590, which 0x004952f0 always calls). Off that route nothing changes (kb_vs1, section 5).
6. `re/orchestrator/UNCERTAIN_REGISTER.md` + 1 line (strncpy identity).

Deviations on the knob path, listed: (a) the mount chain gets the exe path (`original/toastart/...`), not the 0x00402b70
form the original passes, so the name copies at 0x008ab7e0 / 0x008ad8a0 differ from the original; (b) 0x004b6610 (the
callback store behind 0x004b6560) is not bound, so [0x007d3e5c] stays 0 and 0x004b6940 skips the progress callback the
original calls on each mount (0x00429290 recorded by the hand SetIdle).

## 2. Bodies, with class (RULE 9: match and replay apart)

| rva | what (slot of the 0x004b6640 record) | source | match.py --bytes | relocheck | equiv.py (tool 1ed7ceb1a) | class | object build |
|---|---|---|---|---|---|---|---|
| 0x004b6630 | +0x14, &DAT_008cd9c0[n], stride 0x20078 | s18/004b6630.c | BYTE-IDENTICAL 16 B | OK 1/0 | n/a | match (pending promotion) | IDENTICAL |
| 0x004b6f90 | +0x44 eof | s18/004b6f90.c | BYTE-IDENTICAL 36 B | OK 2/0 | n/a | match (pending) | IDENTICAL |
| 0x004b6f60 | +0x2c close | s18/004b6f60.c | BYTE-IDENTICAL 46 B | OK 2/0 | n/a | match (pending) | IDENTICAL |
| 0x004b6ba0 | +0x4c exists | s18/004b6ba0.c | BYTE-IDENTICAL 81 B | OK 6/0 | n/a | match (pending) | IDENTICAL |
| 0x004b6ed0 | +0x38 seek (EDX:EAX from an 8-byte local) | s18/004b6ed0.c | BYTE-IDENTICAL 131 B | OK 4/0 | n/a | match (pending) | IDENTICAL |
| 0x004b6c00 | +0x28 open | s18/004b6c00.c | 83.1% | not run | NOT-PROVEN branch-condition (counter reload after the byte store) | provisional | skip:provisional, VS2022 body |
| 0x004b6cd0 | +0x30 read | s18/004b6cd0.c | 88.7% | not run | NOT-PROVEN unsupported (rep movsd, variable count) | provisional | skip:provisional, VS2022 body |
| 0x004b6640 | the piz: install | s18/004b6640.c | 60.5% | not run | NOT-PROVEN call-regargs | provisional | skip:provisional, VS2022 body |
| 0x004b6570, 0x004b6940, 0x004b67a0, 0x004b6770, 0x004b65e0, 0x004b67e0, 0x005507a0 | mount / unmount chain | existing C4 sources | (C4 c4=match rows) | - | - | match (C4) | IDENTICAL each |

Counts: match 5 new (pending the train) + 7 existing C4 bound, replay-C4 0, provisional 3. `c4_reclass.py batch --tsv
re/promotions/RACE-PIZ-DEVICE.tsv` (dry run): 5 PASS, 0 refused. Residues (in the PROVISIONAL rows): 0x004b6c00 keeps the
ring counter DAT_008ad864 in a register where the original re-reads it after the byte store to 0x008ab89f + n*0x40, and
has two loop-head alignment pads the original lacks (flagscan best /O2 /Ob1 and /Ogty 83.1%); 0x004b6cd0 keeps the
refill destination in a register where the original holds it in [esp+0x20] (0x004b6e29).
Listed unit -> 100% neighbour that fell back to VS2022: 0x004b6c00 (holds a copy of 0x004b6b00, C4) and 0x004b6cd0 (holds
copies of 0x004b6b00 and 0x004b6b90, C4, and 0x004b6b20, C2); those copies are file-local, the C4 rows' own units are linked.

## 3. The stops, measured

**Object build** (`mashed_re_ddk.exe` 05e260cc, arms RO_ROLOADRWC-piz1 / RO_ROLOADRWCC-piz1, same result on both):
- `[PIZ_FS] 0x004b6640 piz: file system (provisional body) installed: [0x007d3e54] 04C40900, [0x007dc75c] 5 file systems`
  (RACE-VFS-COURSELUA: 4).
- every mount: `0x004b6570(<path>) original mount chain returned 1`, `[0x008ad9a8]` = the exe reader's count for that
  piz (font36 26, sfx 30, powerups 36, Panel 15, perm 26, training 43 twice), default `[0x007dc76c]` = the piz: record;
  every unmount puts the default back (018483F0).
- `cfglog: Searching piz for COURSE.LUA` with no `Unable to find`; then AV exe+0x61741 = `PortTop_Bound_0047ab30` +0x21,
  write 0x000001EC, ecx 0; stack (map `mashedmod/build/ddkobj/mashed_re_ddk.map`): callCclosure, luaD_call, luaV_execute,
  luaD_call, f_call, luaD_runprotected, lua_call, lua_dobuffer, PortTop_Bound_0047b8a0, PortTop_Bound_0047b8d0.
- Cause: `re/match/s13/0047ab30.c:27-31` declares `static char *DAT_006bf1cc;` in its C branch (`#ifndef __cplusplus`).
  The DDK object of that unit (BYTECHECK IDENTICAL) reads its own zero BSS copy. The writer 0x0047a020 (provisional,
  VS2022 body, `re/match/s12/0047a020.c` MGLOBAL) stores 0x00644158 at 0x006bf1cc (the call in `re/match/s06/00426e10.c:111`
  passes the array DAT_00644158). Measured why the source keeps the static: with `MGLOBAL(char *, DAT_006bf1cc,
  0x006bf1cc)` in C the three s13 handlers drop to 91.1% / 92.2% / 84.6% and relocheck BAD (the original keeps the value
  in a register across the Lua calls, i.e. it is a file-static of 0x0047a020's translation unit). Sources restored.
- Class size (`static_global_units.py`, `static_global_units.txt`): 35 re/match sources hold a static `DAT_<addr>`
  global; 17 of them are linked as DDK units in this object build, among them 0x00429290 (DAT_0067d960), 0x0047ab30 /
  0x0047abd0 / 0x0047ac80 / 0x0047ad40 (DAT_006bf1cc, the COURSE.LUA handlers) and 0x0047c0f0 (DAT_006c2fec). Each reads
  a private zero copy in the object build. A ddkobj fix (bind a static data symbol named `DAT_<addr>` to its address, or
  refuse the unit) is a tool row, outside this row's ownership: not done here.

**VS2022 build** (`mashed_re.exe` 65d3d914, arms RO_ROLOADRWC-piz1vs / RO_ROLOADRWCC-piz1vs, diagnostic, not the oracle
build): COURSE.LUA runs to its end, `0x00426e10` goes on to 0x00479330, `Searching piz for textures.txd`, `[RW_FS]
0x004cc230(2, 1, "textures.txd"): 1577932 bytes from piz -> memory stream`, then AV exe+0x1316C5 =
`mashed_re::D3d9Render::Rwl_StreamFindChunk` +0xf5 (write 0x0025C888), callers PortTop_Bound_0042a470 / Orig_004b3d80 /
PortTop_Bound_00479330 / PortTop_Bound_0042a6b0. That is the hand librw stream path (L-124: the RW core route takes no new
librw binding), the wall after the ddkobj one.

## 4. CUT wall count (decision B second form)

Method: the RO-AUDIOINIT-DEPTH census (verify/audioinit_depth_20261127/), one arm `c1` on the original
(`cut_census.py`): the ref1 recipe unchanged (detorig.py, `original\MASHED.exe` sha256 b9977dab..., c8 counter list
count_list8.txt sha256 c35a34f6..., 6,445 counters), root `D:\MashedWT\RACE-PIZ-DEVICE_census_root` (robocopy of
T2-RACEPATH-CENSUS_root, then `original\`), snaps 0x00426e10:enter (n 3532, tick 300), 0x0047a020:enter (n 3537, tick
300), 0x0047b8d0:enter (n 175, tick 0: the boot powerups.lua). Window: counter 0 at the first enter of 0x0047a020
(CourseDescInit, single caller 0x00426e10 at 0x00426e10's COURSE.LUA call, which calls 0x0047b9b0 -> 0x0047b8d0, the
open of the exe's old stop) and above 0 at the race anchor (`prov_anchor.json`). Join `cut_join.py` on this tree
(binder plan.json files after this row, hooks.csv, re/match, pending re/promotions). Table `cut_walls.tsv`.

| class (303 functions first run after the 0x0047a020 enter, up to the race anchor) | functions | bytes |
|---|---|---|
| bound (bind / reuse line in a plan.json) | 195 | 24,936 |
| C4 unbound | 64 | 11,804 |
| source, not C4 | 31 | 17,394 |
| no source | 12 | 2,857 |
| no row | 1 | 181 |
| **CUT walls (not bound)** | **108** | **32,236** |

By subsystem, the 108: third-party-library 81 (RenderWare-Physics-3.7 44, renderware 18, msvc-crt 14, lua-4.0 4,
qhull 1), gameplay 9, render 5, util 3, track 3, boot 2, ai / frontend / particle / vehicle 1 each, no row 1
(0x004222f0, 181 B, bulk stop). The census race_load phase (counter after the warp above 0 at the anchor) holds 1,179
functions on this list.

Limits, listed: `bound` means an exe body exists, not that an arm executed it (the object build stops inside the first
Lua handler, so most of the 195 have not run on the exe); the counters count every thread; 0x0047a020's own prefix
(0x00478cb0, 0x004b6520, 0x0047b9b0, 0x0047b860, 0x00440bc0 and callees, 0x0047b8d0) is inside the window when first run
there, and all ran on the exe before its stop; functions with no counter (118 pc_funcs entries per RO-AUDIOINIT-DEPTH)
are invisible, so the walls are a floor; the static-global ddkobj class (section 3) is not counted as a wall (those units
are bound) though it stops the object build at 0x0047ab30.

## 5. Checks run

| check | result |
|---|---|
| mashed_lock.py status before every build, check, selftest, arm | free, or HELD by RO-AUDIOINIT-45D460-REACH-5 (not PRIORITY-HOLD / SOLO) before the merged-tree builds; build.bat waited by itself |
| test-slot --status / C: free before builds | printed before the first and the merged-tree builds: C: 65 GB, 77 GB, 70 GB free (never under 25) |
| match.py --bytes, relocheck.py on the new sources | section 2 (5 BYTE-IDENTICAL + OK; 3 provisional) |
| equiv.py (merged tool 1ed7ceb1a, unchanged by the merge) | 0x004b6c00, 0x004b6cd0, 0x004b6640 NOT-PROVEN (equiv_*.txt) |
| c4_reclass.py batch --tsv re/promotions/RACE-PIZ-DEVICE.tsv (dry run) | 5 PASS, 0 refused |
| binder selftest, prep tree | FAIL 5 (bulk plan stale after 0x004b67a0 bound), then PASS after regenerating bulk + moved |
| binder selftest, merged tree 36d800fec | **PASS** (selftest_merged1.txt) |
| VS2022 build, merged tree (build_vs2) | `=== Build OK ===`; rva-lint 662 known, NEW 0 |
| object build MASHED_DDKOBJ=1, merged tree (build_ddk2) | `=== Build OK ===`, mashed_re_ddk.exe 05e260cc |
| ddkobj.py check (in build_ddk2) | IDENTICAL 2455/2562, DIFFERENT 79, not-in-map 28, linked 2437/2713, refusals 17 (13 + 4), provisional kept VS2022 259 units + 7 fns, dual body 33, fn-addr refs ok 66 bad 0. L0 head: 2442/2549, DIFFERENT 79 |
| newly DIFFERENT units | none of this row's; against log\orch\run56_headcheck\ (1379467a6) the one new DIFFERENT is 0x0047a320, which came with the merge (DDKOBJ-ADDR-REGISTERED, replay-C4) |
| knob boot VS2022 exe (MASHED_ORIG_TOP=1 MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=600, kb_vs1) | `WinMain 0x00492370 returned 0 after 600 frames`; no 0x004b6640 / 0x004b6570 line (RWC route off) |
| arms RO_ROLOADRWC-piz1 / RO_ROLOADRWCC-piz1 (object build, staged image 81CC7775...) | exit 0xC0000005 after 9 s / 6 s, stop section 3, meta selftest 16647 checks 0 fails, warp n 2402 tick 300, race.n 0 |
| arms RO_ROLOADRWC-piz1vs / RWCC-piz1vs (VS2022 exe, diagnostic) | exit 0xC0000005 after 6 s each, stop section 3 |
| census arm c1 (original) | race anchor N 3632, dn_warp 108 (ref1 108), then the known post-anchor census crash (exit 6, as RO-AUDIOINIT-DEPTH d1) |
| frida-<hash> folders in %LOCALAPPDATA%\Temp | before 6 (names in D:/MashedWT/RACE-PIZ-DEVICE_data/frida_dirs_before.txt), after the census 7, 1 new: frida-5debad932e0058de05786ad9520ffc7a, created 13:08:34, inside this row's lock window (census prov_install.json 13:08:35): deleted. After: 6 |

## 6. Follow-ups (for L0 / lead)

- **DDKOBJ-STATIC-GLOBALS (tool row):** 17 linked DDK units hold a file-static `DAT_<addr>` (list in
  static_global_units.txt) and read a zero copy in the object build. It stops the race load at 0x0047ab30 and also touches
  0x00429290 (DAT_0067d960, the title tick) and 0x0041c380. Fix in ddkobj.py (bind the static data symbol to its address,
  or refuse the unit), then re-run RO_ROLOADRWC on the object build.
- Next wall after it (VS2022 diagnostic): the texture-dictionary stream read under 0x00479330 / 0x0042a6b0 runs on the
  hand librw stream reader (`Rwl_StreamFindChunk`); on the RW core route that read needs the matched RW stream path (L-124).
- Matching residues: 0x004b6c00 (83.1%), 0x004b6cd0 (88.7%), 0x004b6640 (60.5%).
- 0x004b6610 binding (the mount progress callback) and the 0x00402b70 path form for the mount chain (deviations a, b).

## 7. Evidence

Committed (`verify/racepiz_20261127/`): this RESULT, PREREG_RACE-PIZ-DEVICE.md, equiv_004b6640.txt, equiv_004b6c00.txt,
equiv_004b6cd0.txt, cut_census.py, cut_join.py, cut_walls.tsv, static_global_units.py, static_global_units.txt.

Raw (not committed), `D:/MashedWT/RACE-PIZ-DEVICE_data`:

| file | sha256 |
|---|---|
| arms/RO_ROLOADRWC-piz1.log / RO_ROLOADRWCC-piz1.log | c0819a56052584765f4a46f027fad7e0319bf8b82abb35246300f16bc3435c77 / 6f1494a8bc75cf4670b1908da856a7076b9f87c6be940cfe44cb8174716ff103 |
| arms/RO_ROLOADRWC-piz1/meta.json / RO_ROLOADRWCC-piz1/meta.json | e9ea3249b0e406319504f2dbab7ad9b366b160bd01707a7f800a6b553b65460f / ff6191faf869e2526ea6c31e53efaa6a0ae7ef36f700baabe083be1c6d469f6d |
| arms/RO_ROLOADRWC-piz1vs.log / RO_ROLOADRWCC-piz1vs.log | 5903de6253ada4174b7f16e05a750ed67d648bd620d106b055b840b91f46e11f / a43e69e60157fd99eb5794bf09ed0d8388e56caa4f0feb3b9b67768fdad253ae |
| kb_vs1.log | 79a07df91a70010381db03b69374aee0934bef9224d3f430855a4da6f828a81b |
| build_vs2.log / build_ddk2.log | 72b5fc698c732d8a1e7956896bbb225213ddd161aaeb36dddf51c70596ebd925 / 6b217702597b6a2a9d9b5e8ce12aa6e09ab4c49554f85616f51382c03d302bc1 |
| selftest_merged1.txt | 43e6930b9b01fa970388e9d2d0ba6c20b9837dc3105d88e51de50b136c29f3e4 |
| census/c1/prov_anchor.json / census/c1.log / census/c1/meta.json | 4043240e0ec2b0b1b848a1e4d41ec86cd95128796b601d1f1ff0c6fb2d8f8fbb / a368d5c404fd58563eb563ff89a6e836a2f8aa4a4124b382f8bbdd39cc77dd89 / 6e5eaf9a59f745690a9040435dc5305828827dcaf32344b07960ecd688627c89 |
| worktree mashedmod/build/ddkobj/mashed_re_ddk.exe | 05e260ccc1cd53b11bfb64d830524117cf87d69f449b7af19960c171d9632fc5 |
| worktree mashedmod/build/mashed_re.exe | 65d3d914485ec21a98a8b30bb6174fd64776c8b245212aa23d93b3d631f8f0d9 |
| worktree mashedmod/build/ddkobj/BYTECHECK.tsv | 5e4e5e0f790ad2ddc5a9763eacd15ae93cd51916c3690539bf7ef47abf385178 |
| root original/mashed_image.bin | 81cc77758ffe93ac0b681eb1fed3d63912d25e7d059503af6d4f871dea9afcd8 |

## 8. Unfinished scope (listed, not implied)

- Load length above 3: not reached (race.n 0).
- The ddkobj static-global class: found and sized, not fixed (tool row).
- The texture-dictionary stream wall (VS2022 diagnostic): not worked.
- Matching 0x004b6c00 / 0x004b6cd0 / 0x004b6640 to 100%: residues listed, not closed.
- strncpy identity for the raceload alias: [UNCERTAIN], register line.
