# RESULT MATCH-S07, 2026-11-01

**Verdict: GREEN.** Slice 0x0042aeb0..0x00432230 (128 game functions). 114/128 sources are byte-identical
under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`, C, x87), and every one of
the 114 has `relocheck.py` OK. 89 of them are now the port's only body (89 wrapper TUs `Match/MatchS07_<va8>.cpp`):
55 in `mashed_re.exe` (all also in the .asi), 34 in the .asi only. Gates on the merged build: G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9/1d5bf4f9/1d5bf4f9` = merge_train.json refs (unchanged).
**C4 candidates: 10 rows** (9 C3 -> C4, plus 0x0042fe30 already C4 by the frida route gaining the match route).
The other 45 exe swaps have no exe caller reachable from `WinMain` (L-68).

Resumed after the 2026-11-01 14:23 reboot. The pre-reboot session had finished the static leg (helpers h1-h9) and
the swap leg (helpers w1-w8, w6's report lost); its build was cut by the reboot (exit 0x40010004, four zero-byte
x87 objs). This pass: WIP commit of the 141 uncommitted files (2b60680ea), `git -c merge.autoStash=false merge
orch/integration` c82901f6a (clean), rebind of three raw-address callees to the port bodies that now exist (below),
rebuild, gates.

## Match (static leg)

| | count |
|---|---|
| slice functions | 128 (`slice_rvas.txt`) |
| byte-identical, relocheck OK | 114 (`results.tsv`, `relocheck.tsv`; 14 unchecked relocations in total, all imports / CRT helpers) |
| below 100% | 14 (table below; budget spent, permuter run on each) |

Ten of the 114 are identical by the whole-symbol byte rule (Ghidra's size stops before the real end: jump tables,
padding, a tail): `match.py --bytes` prints `BYTE-IDENTICAL (relocations masked)` for them (column `by` = `bytes`
in `results.tsv`).

| rva | size | best % | class | residual (helper notes, `D:/MashedWT/MATCH-S07_data/notes/h*.md`) |
|---|---|---|---|---|
| 0x0042bcb0 | 303 | 84.9 | b | prologue integer scheduling (zero reg ecx vs eax, player load order), 12+ variants and the permuter |
| 0x0042c510 | 687 | 84.2 | b | `total` coalesces into edi; the original initialises it from the spilled count |
| 0x0042d5a0 | 3580 | 98.1 | a | the y CSE spill and the int->float temp swap frame slots 0x18/0x1c, survives named locals, casts, a flat table and the permuter |
| 0x0042e5b0 | 755 | 54.3 | b | register allocation: the original keeps t in a stack slot (frame 4 B larger) |
| 0x0042e8b0 | 801 | 50.8 | b | register allocation: the original spills t, keeps param_1 in memory, pos/size in edi/ebx/ebp |
| 0x0042ed70 | 137 | 89.4 | b | fmul/fld scheduled after the first arg pushes, fxch pair after fsin |
| 0x0042f0c0 | 801 | 84.5 | b | frame order sel/i/black/white, sel stored per arm (the original stores once after the merge) |
| 0x0042f530 | 299 | 96.9 | b | x clamp computed in eax and copied to edi at the join in the original |
| 0x00430250 | 53 | 71.4 | b | block order of the shared `xor eax,eax; ret` |
| 0x00430670 | 226 | 81.6 | b | register allocation in the else-branch loops |
| 0x004307a0 | 117 | 97.1 | a | fcomp scheduled before `pop esi` in the original |
| 0x00431240 | 622 | 98.9 | b | int scratch store/fild scheduled one push later in the original |
| 0x004314b0 | 602 | 94.4 | b | same, plus `push edi` before the x2 fild sequence |
| 0x00431710 | 975 | 85.5 | b | colour locals in swapped slots, width reg / exit-path shape |

Sources: `re/match/s07/<va8>.c`, sha256 (first 16 hex) in `results.tsv`, git blob ids in `swaps.tsv`.

## Swap (port leg)

`swaps.tsv` has one row per swapped RVA: wrapper, targets, x87, port symbol, install, source, blob, match,
relocheck, the removed old body.

| targets | rows |
|---|---|
| exe + asi | 55 (1 of them NEW: 0x0042f020 had no port body before) |
| asi only | 34 (4 of them NEW: 0x0042c220, 0x0042c300, 0x0042c7c0, 0x00430b30) |

- x87 TUs added to `mashedmod/x87_tus.txt` (11): MatchS07_0042aff0, _0042b180, _0042bde0, _0042d300, _0042f8d0,
  _0042fe90, _00430b30, _00431b20, _00431b30, _00431b50, _00431b60.
- `build_objs.ps1` unchanged. `re/tools/dual_copy_allowlist.txt` unchanged against orch/integration.
  `py -3.12 scripts/lint_rva_bodies.py` -> `34 finding(s): CROSS-TARGET=22, DUP-IN-TARGET=12 | allowlisted=34 NEW=0`
  (the allowlist holds exactly 34 keys, so no key went stale).
- 0x00431b20 / 0x00431b50 / 0x00431b60: VS2022 lowers the intrinsic `sin` to `__CIsin`; the old asm bodies ran
  FSIN, so each wrapper binds `sin` to an FLD/FSIN/FSTP helper (no RVA comment, not a body).
- Held static callees (pilot finding 1): files that define another RVA's function in-file for the DDK carry it as
  `// HELD:` under `#ifndef MATCH_PORT` and bind it to that RVA's port body. 0x00430290 holds 0x0042a920 (bound to
  `PostTrophyEvent`).

**Callee rebinds after the merge (this pass).** The swap leg had bound callees with no port body at the time to
`extern "C"` forwarders that call the original address, as the old bodies did. After the merge three of those have
one port body, so the forwarders now call it:
- 0x0040e470 -> `CarSlotStateGet` (`Ai/CarSlotState.cpp`, both targets since D4-DC-INC-ai): wrappers 0x0042aeb0,
  0x0042af50, 0x0042aff0, 0x0042b180, 0x0042b310, 0x0042b540, 0x0042b770, 0x00432080. Same read,
  `[[0x005f2770] + i*4 + 0x34]`.
- 0x0042d3a0 -> `RaceTransitionBufZero` (wrapper 0x00432080) and 0x00431d90 -> `FrontendPanelFlagAdvance`
  (wrapper 0x00431f30): both are this slice's matched bodies.
The other raw forwarders stay as the old bodies had them (0x00472c60, 0x00496900, 0x0040bb50, 0x00427e00,
0x004282a0, 0x00473870, 0x004739f0, 0x004099a0, 0x0040dd60; the swap leg kept the old bodies' calls).

**25 byte-identical rows not swapped.**

| reason | rows |
|---|---|
| old port body diverges from the original (L-68 (a) class: a swap changes port behaviour, so it is a separate decision-backed row, as MATCH-L68-SWAP was for S02) | 0x0042b960 CarSlotInit1P and 0x0042b9e0 CarSlotAssign (exe old body skips FUN_0040e480 through a standalone no-op, the original always calls it), 0x0042d290 MenusLapTimeFmt (old body ignores param_2 and ftols an unloaded ST0), 0x0042f7b0 FrontendCursorUpdate (decrement guard `> 0`, original `!= 0`; exe caller exe_main.cpp:6401), 0x0042ef40 VehicleUnlockFlagGet (unknown param_2 reads byte 0, the original returns 0), 0x00431d00 CarSelectReset (exe arm skips 0x00431b80, asi install commented out "hangs MASHED at boot"), 0x00430b90 ProgressBarSetA (the port would need a MATCH_PORT-only statement for a C++ uninitialised read, so the body would not be the matched text; swap reverted) |
| exe calls a `...For(port state)` form with port-owned values, the matched body reads the original globals | 0x00430830 SplitScreenTrackAssignmentFor (g_game_state.game_mode vs 0x0067e9fc), 0x00430910 MenuOptionSlotGetFor (MenuNavSM.cpp:994), 0x004309b0 FrontendModeIndexFor (exe_main.cpp:2547) |
| register convention | 0x00430760 IsMultiplayerMode (old body naked asm returning in EAX only; the unhooked original caller 0x00433403 keeps ECX live, a compiled body clobbers ECX) |
| no port body today, and a callee has no port body | 0x0042c2a0 (0x004c5930), 0x0042d420 (0x004c1d30, 0x004c0b30, 0x004c0740, 0x004c77c0, 0x00467020), 0x0042d560 and 0x0042f660 (0x0042d420, 0x004c1a40, 0x004c1b10), 0x0042c960 (0x004111c0, 0x00424270; the port also keeps 0x0067ea6c / 0x0067ed4c / 0x0067ecdc as port-owned copies, MenuNavSM.h:100-102, L-72), 0x0042c010 (0x00472b10), 0x0042c090 (0x004273e0), 0x0042d4a0 (0x004c7760), 0x0042c3c0 and 0x0042f400 (0x004a2b60), 0x00430120 (0x00427e00) |
| pair file | 0x00431b80 (one file with 0x00431d00, which is not swapped) |
| not assessed in this pass (w6's report was lost in the reboot) | 0x0042e3a0 MenuChromeShellA, 0x0042fb70 MenuSpriteDispatchB: draw dispatchers whose files hold 0x0042ac00 / 0x0042ac50 / 0x0042b8b0 / 0x0042b8c0, and the exe drives MenuChromeShellA through `Compat/StandaloneRvaThunks.h`. Follow-up swap row |

## Exe callers (L-68)

Binary witness on the merged build: `py -3.12 verify/matchs07_20261101/tools/exe_reach.py mashed_re.exe
mashed_re.map <symbols>` builds the direct `call`/`jmp rel32` graph of the exe from the linker map and walks it from
`_WinMain@16` (`exe_reach.tsv`; `exe_callsites.txt` is S05's one-level count). The address-taken column is `yes` for
every symbol (each one sits in the `/EXPORT` table and its `RH_ScopedInstall` registration), so it carries no signal.

| rva | symbol | direct chain from WinMain |
|---|---|---|
| 0x0042b920 | ConstantGetter22 | PromptStripResolve <- PromptStripAppend <- Frontend::Nav_Init <- WinMain |
| 0x00431b00 | DefaultParam_SetField00 | ExecuteFrontendBootChain <- WinMain |
| 0x00431ae0 | DefaultParam_SetField04 | ExecuteFrontendBootChain <- WinMain |
| 0x00431af0 | DefaultParam_SetField08 | ExecuteFrontendBootChain <- WinMain |
| 0x0042c1d0 | RaceStateArrayZero | ExecuteFrontendBootChain <- WinMain |
| 0x0042d3e0 | MenuEntryArrayInit | ExecuteFrontendBootChain <- WinMain (and VerifyDataSectionWedgeViaReimpl) |
| 0x0042bb60 | MenuTeamBalance | RenderFrame <- WinMain, and WinMain directly (Team Select, exe_main.cpp:6416, :9881) |
| 0x0042fa00 | MenuTeamSelectTick | RenderFrame <- WinMain (Team Select, exe_main.cpp:6376) |
| 0x0042fe30 | RaceEndFlagIfEndMode | PickupPoolSpawn <- PickupField::InitReal <- TrackRenderer::InitPickups <- WinMain |
| 0x0042f6a0 | GetRaceSubMode | RaceEndFlagIfEndMode <- (as above) |
| the other 45 exe swaps | | none reachable. GetDat0067ea64 (9 sites) is called only from S05's callerless PlayerBlock2Field* / PlayerScoreTeamAcc* bodies, FrontendPanelFlagAdvance only from FrontendPageIdDispatch (0 sites) |

A direct chain shows the exe calls the body. It does not show which of these the gate scenario executes (no
per-function execution trace was taken). The callers sit in the boot chain, Nav_Init, track-load pickup spawning,
and RenderFrame's Team Select path.

## Gates (merged build)

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on exe `fea10696`, private root
`D:/MashedWT/MATCH-S07_root`, test slot (acquired after 788 s in the queue) + `mashed_lock.py run --solo`
(2026-11-01, own solo window). Every arm exit 0, self-exited.

| arm | file | sha8 | ref |
|---|---|---|---|
| H_DEF | step.csv / gates.csv / ptrace.log | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e |
| B_DEF_K0, K1, K26 | step.csv | 1d5bf4f9 x3 | 1d5bf4f9 x3 |

The merge train's `-Full` pass (D2 x3, 13 tracks) was not run here: `mashedmod/src` is in `full_set_paths`, so the
train runs it on enqueue.

## C4 re-classify text (apply after the merge, through re-classify; not applied here)

Common evidence for every row below: `match.py re/match/s07/<va8>.c` 100% under `flags=/O2 /GS /Ob1 tc=ddk`,
`relocheck.py` OK with `unchecked=0`, one body (`Match/MatchS07_<va8>.cpp`, both targets, `RH_ScopedInstall`
moved into the wrapper), exe caller chain from `WinMain` (table above), gates unchanged on this build. The `file`
cell moves to the wrapper. Per row (`c4_rows_match.txt` has the full match / relocheck lines and source sha256):

| rva | name | level | match.py | relocheck | old body (removed) |
|---|---|---|---|---|---|
| 0x0042b920 | ConstantGetter22 | C3 -> C4 | 2/2 ins | checked=0 | Frontend/MenuNearLeaves_s6.cpp |
| 0x00431b00 | FUN_00431b00 (DefaultParam_SetField00) | C3 -> C4 | 2/2 ins | checked=1 | Boot/BootLowRvaCluster.cpp |
| 0x00431ae0 | FUN_00431ae0 (DefaultParam_SetField04) | C3 -> C4 | 2/2 ins | checked=1 | Boot/BootLowRvaCluster.cpp |
| 0x00431af0 | FUN_00431af0 (DefaultParam_SetField08) | C3 -> C4 | 2/2 ins | checked=1 | Boot/BootLowRvaCluster.cpp |
| 0x0042c1d0 | FUN_0042c1d0 (RaceStateArrayZero) | C3 -> C4 | 7/7 ins | checked=1 | Boot/GameStateCluster.cpp |
| 0x0042d3e0 | MenuEntryArrayInit | C3 -> C4 | 20/20 ins | checked=2 | Frontend/MenuInit.cpp |
| 0x0042bb60 | FUN_0042bb60 (MenuTeamBalance) | C3 -> C4 | whole symbol 323 B BYTE-IDENTICAL (`--bytes`; insn 95.9% only because Ghidra's size is short) | checked=14 | Frontend/MenuNav.cpp |
| 0x0042fa00 | MenuTeamSelectTick | C3 -> C4 | 56/56 ins | checked=9 | Frontend/MenuNav.cpp |
| 0x0042f6a0 | GetRaceSubMode | C3 -> C4 | 2/2 ins | checked=1 | Util/GameStateGetters.cpp |
| 0x0042fe30 | RaceEndFlagIfEndMode | C4 stays (frida route); add the match route to `notes` | 6/6 ins | checked=2 | Frontend/MenuRaceEnd.cpp |

CHANGELOG line per row: `2026-11-0x  0x<rva>  <name>  C3->C4  route match (c4=match), exe-callers via <chain>;
evidence: verify/matchs07_20261101/RESULT_MATCH-S07.md`.

Where a row's callees or globals are not exercised by the gates, `notes` says so (CONFIDENCE.md, "How the
Write-set leg relates"): 0x0042bb60 / 0x0042fa00 are called from the Team Select (nav screen 16) path in
exe_main.cpp (:6376, :6416, :9881), and these gates give no evidence that path ran.

All other swapped rows (45 exe+asi, 34 asi-only): stay at their level, `notes` += `match evidence: identical,
swapped, exe-callers=0; C4 when a caller lands (L-68 ruling)`. The 17 swapped rows already at C4 by another route
keep it (L-68 demotes only `match`-route C4s); 0x0042fe30 is one of them and is in the table above.

## Build

`set MASHED_NO_DEPLOY=1&& mashedmod\build.bat`, Build OK, .asi not deployed. exe sha256
`fea10696b3559cefc9be43d1f556890bdcc01bda7b50a424c8447f800a8ce9ce`, asi
`b77f4bfe897649fdb7c5f91ca5e61da6d31a19650256083fdcabda68727999ab`.

## Raw files (not committed; `D:/MashedWT/MATCH-S07_data`)

| file | sha256 |
|---|---|
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv, B_DEF_K1.step.csv, B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |
| gates.log, build2.log (exe link, asi link cut by 4 zero-byte objs from the reboot), build3.log (Build OK) | run logs |
| notes/h1.md..h9.md, swap_w*.txt | helper reports (static and swap legs) |
| g2s/, decomp.json, scratch/ | raw converter output, decomp, permuter scratch |
