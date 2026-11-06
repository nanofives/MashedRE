# RESULT MATCH-S16, 2026-11-02 (merge, gates and re-classify 2026-11-06)

**Verdict 2026-11-06: GREEN.** After merging orch/integration (adf5ff69b): 41 swapped rows, 41/41 byte-identical and
relocheck OK, exe reach unchanged (0 exe callers, 0 C4 candidates), gates equal the merge_train refs (G-CTRL
8284be46/ba407a16/31fe043e, B 1d5bf4f9 x3), 41 rows re-classified tracker-only. Section "2026-11-06" at the end. The
2026-11-02 text below is the static and port leg as first written; two of its 43 swaps (0x00496900, 0x00497450) are
dropped in favour of the GFFU bodies already on integration.

**Verdict: BLOCKED. Built on this branch. Gates not run because the display is unavailable (D3DERR_NOTAVAILABLE on
LIVING-PC); the lead's task was static work only.** The slice is 0x004944b0..0x0049b300: 128 game functions, no
library rows inside.

- **115/128** sources are byte-identical under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178,
  `/O2 /GS /Ob1`, C, x87). Every one of the 115 has `relocheck.py` OK in its best source file (`relocheck.tsv`).
- **43** of them are now the port's only body. 40 are wrapper TUs `Match/MatchS16_<va8>.cpp`; 3 are in-file bodies
  (0x00494f40 and 0x00494fd0 in `00495080.c`, 0x00497190 in `004971b0.c`). 5 are built into both `mashed_re.exe` and
  the .asi; 38 are .asi only.
- **C4 candidates: 0.** None of the 43 swapped bodies has a direct exe caller (L-68), so all stay at their level with
  the L-68 note.

Branch: `orch/MATCH-S16`, cut from orch/integration 3c6510ee6. MATCH-S09, S10, S11, S13 and S15 are not merged there,
and nothing here depends on them.

Insertion anchors are S16-only:
- The rsp block sits after `"Boot\VideoConfig.cpp"`. S09 uses `PickupPoolSpawn`, S10 `FloatSliderStep`, S11
  `DrawQuadPrimitives`.
- The `x87_tus.txt` block sits after `ViewportScaledRectDraw`.
- The two new `re/console/pc_funcs.csv` rows are 0x004991f0 (454 B) and 0x00499820 (97 B). Their sizes come from the
  pool clone's functions (`decomp_pc.py`, slot Mashed_pool0, `exact_entry` true).

## Match (static leg)

| | count |
|---|---|
| slice functions | 128 (`slice_rvas.txt`, from `tools/mkslice.py`; matches the `verify/matchscope_20261031/slices.tsv` row) |
| raw converter (g2s) at 100% | 36 |
| hand-matched to 100% (helpers h1-h6, `helper_notes.txt`, `helper_assign.txt`) | 79 |
| below 100% or no source | 13 (table below) |

The 115 include several rows counted by the whole-symbol bytes rule, where Ghidra's size stops before the epilogue:
0x004955d0, 0x004972b0, 0x00498a00 and 0x00494f40.

| rva | size | best % | regs % | class | residual (helper notes) |
|---|---|---|---|---|---|
| 0x004944c0 | 864 | 59.2 | 54.4 | c | C++ EH frame with inline smart-pointer dtors (thiscall, C++ object code) |
| 0x00494820 | 594 | 37.0 | 52.6 | c | C++ EH frame + thiscall; C++-mode experiment 44.3% |
| 0x00494b65 | 78 | - | - | c | wrong boundary: the real entry is 0x00494b50 (99 B). `00494b65.c` holds FUN_00494b50, byte-identical, but 0x00494b50 has no pc_funcs row or hooks row |
| 0x00495870 | 1544 | 98.4 | 98.4 | b | one `fxch` before y*scale in the normalise branch; held static callee in `00495fe0.c` (ESI arg) |
| 0x004967e0 | 284 | 98.8 | 98.8 | b | one `and eax,1` after `shr 7` not reproduced |
| 0x004969a0 | 612 | 66.0 | 78.0 | b | frame 0x9c vs 0xa0, FP/global-load order; held static callee in `00496c10.c` |
| 0x00497b60 | 1135 | 75.6 | 97.1 | b | register permutation only (hDlg / GetDlgItem / ShowWindow registers) |
| 0x00498510 | 653 | 95.3 | 95.3 | b | one `mov ecx,0x80` placed earlier in a struct copy |
| 0x00498810 | 69 | 86.7 | 100.0 | b | 2 register choices swapped (static callee, Camera* in EAX) |
| 0x00498ea0 | 183 | 91.1 | 97.8 | b | `lea ebx` scheduling, base/index swapped in 3 loads |
| 0x00499400 | 653 | 90.1 | 92.3 | b | inner loop global-load order and counter copy through edi |
| 0x00499d90 | 779 | 43.9 | 43.9 | a | the original unrolls the record loop x4, 9176 unrolls x8 (same as S01 0x00401690) |
| 0x0049b300 | 6 | - | - | c | `jmp [IAT]`, a linker import thunk; no source |

Classes: (a) compiler-version codegen, (b) source shape not found in budget, (c) tool limit.

Holds and source notes:
- Files that hold a static callee (pilot finding 1, register-passed args) are listed in `helper_notes.txt`.
- h5 wrote one shared text into each of the video-dialog files and the 0x00498910 group. In every swapped file, each
  `// MATCH:` line other than the RVA's own was converted to `// HELD:` (`#ifndef MATCH_PORT`).
- `00499ba0.c` is not swapped. Its length for the `DAT_00773818` array is not read from the binary: it uses 0x100,
  marked `[UNCERTAIN]` in the file.

## Swap (port leg)

`swaps.tsv` has one row per swapped RVA: wrapper, targets, x87, port symbol, install, source blob, match, relocheck
and old body. The helper reports are `D:/MashedWT/MATCH-S16_data/swap_w1.txt` .. `swap_w4.txt`.

| targets | rows |
|---|---|
| exe + asi | 5: 0x00498bc0 VideoGetRenderWidth, 0x00498bd0 VideoGetRenderHeight, 0x00498bf0 DisplayActiveFlagGet, 0x00499690 WindowMsgPump, 0x004996f0 WindowShow |
| asi only | 38: 0x004944b0, 0x00494a80, 0x00494f30, 0x00494f40, 0x00494fd0, 0x00495080, 0x00495120, 0x00495270, 0x00495350, 0x00495520, 0x00495530, 0x004955b0, 0x00495790, 0x004957a0, 0x00495830, 0x00496900, 0x00496910, 0x00496920, 0x00496930, 0x00496d60, 0x00496db0, 0x00497000, 0x00497190, 0x004971b0, 0x00497450, 0x00498be0, 0x00498d20, 0x00498d60, 0x00498f60, 0x00499170, 0x004991f0, 0x00499710, 0x00499720, 0x00499730, 0x00499740, 0x004997b0, 0x0049a2e0, 0x0049a740 |

**Lint and build checks**
- `py -3.12 scripts/lint_rva_bodies.py` reports `2 finding(s): CROSS-TARGET=1, DUP-IN-TARGET=1 | allowlisted=2 NEW=0`,
  unchanged from before the swaps.
- `bodies_post.tsv`: each swapped RVA has exactly one body.
- `py -3.12 scripts/lint_inline_asm_abs.py` reports OK (1173 files).
- `re/tools/dual_copy_allowlist.txt` is unchanged: no swapped RVA had an entry.

**x87.** One entry was added to `x87_tus.txt`: `MatchS16_00495080`. Its old body was naked asm that called the
original image's x87 code at 0x00494fd0. The other float wrappers, `MatchS16_00497000` and `MatchS16_00495350`, stay
SSE, because their old bodies (`Util/PromoLoop_sessionB.cpp`, `Frontend/IntroSplash.cpp`) were SSE TUs. This is the
S11 rule: keep each body's codegen mode. No physics TU is in this slice.

**Behaviour notes the gates or a Frida diff must cover**
- **0x00495080 (with in-file 0x00494f40 and 0x00494fd0).** The old body called the original 0x00494fd0 by address.
  The gamma-ramp math and the device vtable calls (+0x1c/+0x58/+0x54) now run as port code (VS2022 x87). The ramp
  values are bit-identical only if VS2022 rounds the float casts the way the DDK does. This is the one swap whose
  output the old body does not pin, so a `diff-original` run is owed before any promotion.
- **0x00494a80.** `Lane2/L2_00494a80.cpp` (asi only) keeps an active `RH_ScopedInstall(L2_FUN_00494a80, 0x00494a80)`
  that the body scanner does not count. The new `MatchS16_00494a80` has install=0 and is called only by
  `MatchS16_00495350`. Two implementations of one RVA now live in the .asi. Follow-up: retire the Lane2 copy or move
  the install.

**Matched-source edits** (C++ compat; the DDK bytes are unchanged, and `match.py` 100% and `relocheck` OK were re-run
after each edit):
- Win32 declarations under `#ifndef MATCH_PORT`: 00495120.c, 00495530.c, 00499690.c, 004996f0.c, 004997b0.c.
- `unsigned int` return under `__cplusplus`: 004955b0.c.
- `MATCH_STATIC` plus stand-in caller: 004971b0.c and the five dialog files.
- Callee prototypes typed: 00496d60.c, 00496db0.c.
- HELD conversion: 00498d20.c, 00498d60.c, 00498f60.c, 00499170.c, 004991f0.c.

**Raw-address forwarders** (the old body reached the original by address and there is no port body):
- 0x00495080 -> 0x004a2d30 (CRT pow)
- 0x00496d60 / 0x00496db0 -> 0x004d4170, 0x004d4dd0, 0x004eb3c0 / 0x004eb9d0, 0x004d4f90, 0x004d4380, 0x004d41e0
- 0x00495530 -> 0x0049b300, 0x004987b0
- 0x00494a80 -> 0x004c77c0, 0x004944c0
- 0x00495350 -> 0x004671a0, 0x00493fd0, 0x00494460, 0x00494480, 0x00499710, 0x004c1be0
- 0x00498d60 -> 0x00498a00 (EBX forwarder)
- 0x004971b0 -> 0x004a2b60 (variadic), 0x004a4541 (kept raw so the `fix_fopen` cave runs on the game-CRT FILE*),
  0x004a49cf, 0x004a4368

**9 byte-identical rows NOT swapped (NOT-SAFE)**

| rva | old body | reason |
|---|---|---|
| 0x004963e0 | Save/SettingsConfig.cpp ConfigLogErrorCore (exe+asi) | exe arm reads a private `s_logHandle` and uses the exe CRT; MGLOBAL 0x00772fbc falls inside mashed_re.exe's own .data in the exe |
| 0x00496400 | Save/SettingsConfig.cpp ConfigLogDebugCoreV | same; the port symbol takes (fmt, va_list), the matched function is variadic |
| 0x00498910 | Save/SettingsConfig.cpp ConfigFilenameGetCore | exe arm writes a private `s_filenameBuf`, not 0x007731e8 |
| 0x00498950 | Save/SettingsConfig.cpp ConfigLoadCore | exe arm uses a private `s_settings`, `_fsopen` and its own literals; exe caller VideoCfgFirstRun.cpp:154 |
| 0x004989b0 | Save/SettingsConfig.cpp ConfigSaveCore | same; exe caller VideoCfgFirstRun.cpp:189 |
| 0x00498a00 | Boot/VideoCfgFirstRun.cpp ModeLabel (exe) | C++ `ModeLabel(char*, const VideoMode&)` over a std::vector entry |
| 0x00498e40 | Boot/VideoCfgFirstRun.cpp DefaultModeFind (exe) | reads a port `std::vector<VideoMode>`, not the 0x007731f8/0x00773408 tables |
| 0x00494ef0 | Boot/Teardown.cpp ThunkVideoStateGet (exe+asi) | matched is `jmp 0x00493f70`, whose only port body (VideoStateFlagGet) is asi-only; becomes SAFE when that body links into the exe |
| 0x004950b0 | Util/UtilMid.cpp QpcTimeScaledTo3Mhz (asi) | **port fidelity bug, filed as MATCH-FIX-4950B0** (below) |

**63 byte-identical rows with no port body**, not swapped (a new body with no caller is not C4, L-68): 0x00494ac0,
0x00494b20, 0x00494bc0, 0x00494c60, 0x00494c80, 0x00494ee0, 0x00494f00, 0x00494f10, 0x00494f20, 0x00495110,
0x00495150, 0x004951e0, 0x004951f0, 0x00495280, 0x004952f0, 0x004954f0, 0x00495580, 0x004955c0, 0x004955d0,
0x00495780, 0x00495e80, 0x00495fe0, 0x00496010, 0x00496040, 0x004960e0, 0x00496100, 0x00496320, 0x00496370,
0x004963d0, 0x00496470, 0x00496490, 0x00496530, 0x00496940, 0x00496970, 0x00496c10, 0x00496ce0, 0x00496d00,
0x00496d20, 0x00496e40, 0x00496ec0, 0x00497060, 0x00497230, 0x004972b0, 0x00497310, 0x00497470, 0x004974b0,
0x00497b10, 0x004982a0, 0x004987b0, 0x00498860, 0x00498b60, 0x00498c00, 0x00499820, 0x00499890, 0x00499ba0,
0x00499cc0, 0x00499ce0, 0x0049a0a0, 0x0049a0e0, 0x0049a150, 0x0049a730, 0x0049aae0, 0x0049ab10.

The four gap rows from `verify/match_topdown_20261101/gap_value.tsv` are all matched:
- 0x004991f0 (VideoSettingsDlgProc) and 0x00498f60 (VideoDialogInit) are swapped, asi only.
- 0x004974b0 and 0x00497b60 have no port body; 0x00497b60 reached 75.6% (regs 97.1%).

## Port bugs found

- **MATCH-FIX-4950B0** (ledger row added, MATCH, wave 4). The .asi body `QpcTimeScaledTo3Mhz` (Util/UtilMid.cpp,
  0x004950b0) computes QPC*3000000/freq with unsigned 64-bit math and returns 0 when freq (0x00771e70) is 0. The
  original uses signed `__int64` (`__allmul` / `__alldiv`) with no guard (`re/match/s16/004950b0.c`, 100%). The two
  results differ once the product passes 2^63, which is about 85 h of uptime at a 10 MHz QPC. The row asks for the
  matched body plus an L-n decision on keeping the zero-freq guard.

## Exe callers (L-68)

Binary witness on this build: `py -3.12 verify/matchs16_20261102/tools/exe_reach.py mashedmod/build/mashed_re.exe
mashedmod/build/mashed_re.map _VideoGetRenderWidth _VideoGetRenderHeight _DisplayActiveFlagGet _WindowMsgPump
_WindowShow` (`exe_reach.txt`).

| rva | symbol | direct exe sites | reach from WinMain |
|---|---|---|---|
| 0x00498bc0 | VideoGetRenderWidth | 0 | no (addr_taken = install table only) |
| 0x00498bd0 | VideoGetRenderHeight | 0 | no |
| 0x00498bf0 | DisplayActiveFlagGet | 0 | no |
| 0x00499690 | WindowMsgPump | 0 | no |
| 0x004996f0 | WindowShow | 0 | no |
| 38 asi-only rows | | none (not in the exe) | |

**No C4 candidates.** All 43 swaps stay at their level, with `notes` += `match evidence: identical, swapped,
exe-callers=0; C4 when a caller lands (L-68 ruling)`. This goes through re-classify after the merge and is not
applied here.

## Gates

**Not run, by instruction: the display is unavailable.** LIVING-PC fails `CreateDevice` with D3DERR_NOTAVAILABLE, and
the lead's task was static work only (no gates, no game runs).

Owed before review:
- `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this exe, compared with
  `re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref`.
- The merge train's `-Full` pass, because `mashedmod/src` is in `full_set_paths`.

The exe-side swaps are five plain getters/Win32 wrappers with no exe caller. The gate scenario reaches them only if
the original image calls the installed bodies, which happens in the .asi, not the standalone.

## Build

`set MASHED_NO_DEPLOY=1&& mashedmod\build.bat` reported Build OK. The .asi was not deployed:
`original\mashed_re_dev.asi` is still dated 2026-10-24.

| artifact | sha256 |
|---|---|
| exe | `add5add632618270dbf1f109d1a7e83d66279f5d3aeb104c9b4ae8438f97fbeb` |
| asi | `569506a95aa613bb4d50172c052b9114497c0a035ad819edaa28f8e2a12fe0a8` |

## Raw files (not committed; `D:/MashedWT/MATCH-S16_data`)

| file | sha256 |
|---|---|
| decomp.json (decomp_pc.py --port --callees, 128 entries, slot Mashed_pool0) | 8cd2e7551fafbe81810b8fb2faac688513fdb3c1e26f9c2ae768d1d65c740158 |
| g2s/results.tsv (raw converter scores) | 2a1a5286e10eb8f16bbbc7c7c9c05f9fb1080a3ca6b155c2ffc88e00e48487c8 |
| build1.log (Build OK) | 37195fbf143559674c4024195c38f14d0c3b82b36cce71133d2c8dc68fbd2f2e |
| bodies_pre.tsv (pre-swap body scan) | 970ce0b8ce2a695cd1ba914975ffacc945e22875289e6b3cc9fbc7719384bc92 |
| bodies_post.tsv (post-swap body scan) | 51b513d7ebd47aab7fe25d411e081fb92ad3e25fdefdffea29ee9d610a0390c6 |
| rc.tsv (= committed relocheck.tsv) | 24e79b759485fe69d389182d59f2bc4116637e95ee921378786f8d49845e51b6 |
| g2s/, notes/h1..h6.md, scratch/, swap_w1..w4.txt | raw converter output, helper notes and reports, scratch |

## 2026-11-06: merge, re-check, gates, re-classify

**Merge.** Worktree re-created from `orch/MATCH-S16` @ b3cfd98cd, then
`git -c merge.autoStash=false merge orch/integration` (adf5ff69b, which carries MATCH-S09, S11, T13-SEED-WRITER and
PORTSTATE-GAMEFLOW-FU). Three conflicts, resolved by hand:
- `Frontend/IntroSplash.cpp`: kept the S16 move of 0x00495350 to `MatchS16_00495350.cpp`. Integration's side (S11)
  had only renamed the `OrigVehicle0Get(0)` call inside the old body to `BackBuffRasterOpen(0)`. The S16 wrapper
  still reaches 0x004671a0 through its original address (asi only), which runs S11's installed body there.
- `Util/PromoLoop_round24.cpp` and `Util/PromoLoop_sessionB.cpp`: took integration's side. MATCH-PORTSTATE-GAMEFLOW-FU
  already moved 0x00496900 SlotActiveThunk and 0x00497450 Pred497450 into `Match/MatchGFFU_00497450.cpp`, built into
  exe and asi. S16's `MatchS16_00496900.cpp` and `MatchS16_00497450.cpp` (asi only) would have been second bodies, so
  both were deleted and taken out of `asi_sources.rsp`. `re/match/s16/00496900.c` (100%, relocheck OK) is the
  byte-matched form of GFFU's hand forwarder. The two rows' hooks.csv cells still name the PromoLoop files: ledger row
  MATCH-GFFU-TRACKER-497450.
- rsp files, `x87_tus.txt` and `re/console/pc_funcs.csv` merged with no conflict and no duplicate lines.

S16 swaps after the merge: **41** (5 exe+asi, 36 asi only).

**Build.** `set MASHED_NO_DEPLOY=1&& mashedmoduild.bat`: Build OK, .asi not deployed.

| artifact | sha256 |
|---|---|
| exe | `4fc371522b755f7f9f6f349a7cf1532f285f4388fe4c97f8fa0f73eda34a7d5b` |
| asi | `8ffcb98f9061aa0cf80abfe982a744b95ecf046f593a776c667ee18cb4df3695` |

- `py -3.12 scripts/lint_rva_bodies.py`: `1 finding(s): CROSS-TARGET=1 | allowlisted=1 NEW=0`.
- `py -3.12 scripts/lint_inline_asm_abs.py`: OK (1253 files).

**Re-check of the 41 swapped rows** (`recheck_postmerge.tsv`): `match.py <src> --bytes` reports BYTE-IDENTICAL
(relocations masked) for 41/41, and `relocheck.py <src>` reports OK for 41/41. 0x00494f40 is 85% by Ghidra's
instruction span and byte-identical over the 133 B whole symbol, as on 2026-11-02.

**Exe reach** (`exe_reach_postmerge.txt`, same command as above on the post-merge exe): the five exe+asi symbols still
have 0 direct exe sites and no WinMain reach (address taken by the install table only). The 36 asi-only rows are not
in the exe. **C4 candidates: 0.**

**Gates** (`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, exe 4fc37152, private root
`D:/MashedWT/MATCH-S16_root`, test slot outside, `mashed_lock.py run --label MATCH-S16` inside). Refs read from
`re/orchestrator/merge_train.json` on orch/integration adf5ff69b at gate time.

| gate | this exe | ref | |
|---|---|---|---|
| G-CTRL H_DEF step / gates / ptrace | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e | equal |
| B_DEF_K0 / K1 / K26 step | 1d5bf4f9 / 1d5bf4f9 / 1d5bf4f9 | 1d5bf4f9 x3 | equal |

All four arms exited 0 and self-exited (H_DEF 133 s). `full_set_paths` (Vehicle/, Ai/, Collision/) are not touched by
this branch.

**Tracker transaction (re-classify, applied by `tools/reclass.py`).** 41 hooks.csv rows: `file` =
`mashedmod/src/mashed_re/Match/<wrapper>` (the in-file bodies 0x00494f40 / 0x00494fd0 name `MatchS16_00495080.cpp`,
0x00497190 names `MatchS16_004971b0.cpp`), `exe_file` = the same for the 5 exe+asi rows and unchanged for the 36
asi-only rows, `notes` += `MATCH-S16 2026-11-06: match evidence: identical, swapped, exe-callers=0; C4 when a caller
lands (L-68 ruling); match.py <src> 100% ...; relocheck OK ...; src sha256 ...; port body ... (only body); <this
RESULT>`. Confidence and status cells unchanged (32 C3, 5 C2, 4 C4). The four rows already C4 by an earlier route
(0x00494f30, 0x00498bc0, 0x00498bd0, 0x00498bf0) keep it, with the note that the match route adds no level. 0x00495080
and 0x00494a80 carry the owed items below in their notes. 41 CHANGELOG lines (`Cn=Cn notes/file only`) below
`<!-- ENTRIES -->`. `py -3.12 re/orchestrator/tracker_lint.py --repo . --base orch/integration`: OK, NEW=0.
No UNCERTAINTIES or STUBS rows were created or closed.

**Follow-up ledger rows filed:**
- MATCH-S16-FU-L2-494A80: `Lane2/L2_00494a80.cpp` keeps an active install of 0x00494a80 next to the matched
  `MatchS16_00494a80.cpp` (install=0) in the .asi.
- MATCH-S16-FU-GAMMA: a diff-original run for 0x00495080, whose gamma-ramp math now runs as VS2022 x87 port code.
- MATCH-GFFU-TRACKER-497450: hooks.csv cells for 0x00496900 / 0x00497450 (see Merge).
- MATCH-FIX-4950B0 (filed 2026-11-02) stands.

### Raw files 2026-11-06 (not committed; `D:/MashedWT/MATCH-S16_data`)

| file | sha256 |
|---|---|
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv, B_DEF_K1.step.csv, B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |
| gates_run.log | 5d10f30ed00bda48ea1e46709ddd6e0a2afc810179a2cdd02c9651f2a7e5ddac |
| build2.log (Build OK) | 6e7c32c1d2b9ed4f6e8a08034c47a7b12ea64aeb6e774603c28710d9a2d688e8 |
| postmerge_recheck.tsv (full match.py lines) | 0348d5c2da8a95708f65e1f1cb8d279fe5468085d50cce24e1a9c59767a4f809 |
