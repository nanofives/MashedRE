# RESULT DDKOBJ-CELLS (2026-11-12): the 5 units link as DDK objects, 7/7 functions IDENTICAL

PREREG: `verify/ddkobj_20261112/PREREG_DDKOBJ-CELLS.md` (commit ffe2c9ca8, alone, before any arm). Branch
`orch/DDKOBJ-CELLS` from `orch/integration` @ a616fcc3b. Code commit a308caef4. Row kind: top-down (wrappers, the exe
bindings they read, `re/tools/ddkobj/`).

## Verdict

**All 5 units the spike refused as `fail:wrapper-redefines-globals` now link as DDK objects.** Route: the first option
in the row title. The cells moved to the original addresses. No hand code reads a cell any more.

| arm | result |
|---|---|
| 1 `ddkobj.py selftest` | **OK, 0 failures** (25 checks, 2 new: `mgaddr` accepted, `mgcell` still refused) |
| 2 object build + `ddkobj.py check` | units linked 859/864. The 7 functions of the 5 units: **7/7 IDENTICAL**. No 100% source is DIFFERENT. Refusals 4, all `wrapper-redefines-globals`, all outside this row (section 3) |
| 3 boots, 3000 frames | object build OFF rc 0 (34.8 s), ON rc 0 (28.3 s), VS2022 ON rc 0 (31.5 s). Both ON: `WinMain 0x00492370 returned 0 after 3000 frames` |
| 4 knob-ON log diff, object vs VS2022 | **0 of 1145** `[ORIG_TOP]`/`[RW_BIND]` lines differ. 8 of 177 other lines differ, all intro-movie frame counts (timing, the same class as RESULT_DDKOBJ-REGCONV section 3) |
| 5 12-capture walk, OFF/ON/OFF/ON | object build **4/4 class E**, VS2022 **4/4 class E**, 12/12 equal each, freeze `pinned pos=55 hash=28538c74` |

Self-test result: `ddkobj.py selftest` OK, 0 failures. Default gates not run: top-down row (WORKER_RULES). The default
exe's hand path does run changed code (section 2), so the merge train's gates are the check that matters for it.

## 1. Byte check of the 5 units (`BYTECHECK_cells.tsv`)

| rva | exe symbol | source | size | abs data refs exact | result |
|---|---|---|---|---|---|
| 0x004769f0 | `_ParticleEmitter_SetColour` | re/match/s12/004769f0.c | 32 | 3 | IDENTICAL |
| 0x004282a0 | `_MenuMenusBA` | re/match/s06/004282a0.c | 128 | 2 | IDENTICAL |
| 0x004963e0 | `_ConfigLogError` | re/match/s16/004963e0.c | 32 | 1 | IDENTICAL |
| 0x00496400 | `_ConfigLogDebug` | re/match/s16/00496400.c | 112 | 2 | IDENTICAL |
| 0x00498910 | `_ConfigFilenameGet` | re/match/s16/00498910.c | 64 | 5 | IDENTICAL |
| 0x00498950 | `_ConfigLoad` | re/match/s16/00498910.c | 96 | 4 | IDENTICAL |
| 0x004989b0 | `_ConfigSave` | re/match/s16/00498910.c | 80 | 3 | IDENTICAL |

Whole build (`ddkobj_summary_cells.json`): IDENTICAL 816 / 904, DIFFERENT 78, not-in-map 10. 76 of the 78 DIFFERENT
rows are in `DIFFERENT_scores.tsv` (all below 100%). The 2 not in that list, scored here with `match.py`:
0x00498810 (re/match/s16/00498810.c) **86.7%**, 0x004332a0 (re/match/s08/004332a0.c) **97.3%**. So no 100% source is
DIFFERENT. The PREREG bar holds.

## 2. What changed

**Tool** (`re/tools/ddkobj/ddkobj.py`): a wrapper `#define MGLOBAL(T, n, a) static T &n = *(T *)(a)` or
`#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)` is accepted (`MGLOBAL_ADDR_RE`, whitespace-insensitive,
only these two forms). Each global then links as `__ddkabs_<addr>`, the binding the tool already used for the preamble's
own MGLOBAL. Any other MGLOBAL form is still `fail:wrapper-redefines-globals`.

**Wrappers and exe bindings:**

| unit | before (exe) | now (both targets) |
|---|---|---|
| 0x004769f0 | `MATCH_CELL_DAT_00613290 (0x00613290u)` etc.: cells already at the original addresses | the preamble's address form. Text only, same code |
| 0x004282a0 | `DAT_0067d838` -> `g_menuBa_FontCell` (a private zero), `DAT_0067d830` -> `g_menuBa_DivCell` (private, `DeriveMenuMeasureDiv`), `FUN_00427780` -> `MenuBa_ExeLookup` (returned the id), `FUN_004277a0` -> `MenuBa_ExeRemap` (`GetMenuMessage(id, dst, 0x200)`) | `[0x0067d838]` / `[0x0067d830]` at their addresses (both BSS in the original). `MeasureMenuMessage` stores `DeriveMenuMeasureDiv()` at `[0x0067d830]` when it is zero. `FUN_00427780` -> `MenuBa_ExeLookup` returns the id's `[u16 len][chars]` record in `g_msg_dat` (the shape 0x00427780 returns from `0x0066d828`), or a zero-length record outside the table. `FUN_004277a0` -> `TextCtrlCodeRemapBody` (the S06 body) |
| 0x004963e0, 0x00496400, 0x00498910/50/b0 | `SettingsCfgGlobal(a)`: private log handle, filename and settings buffer, plus private copies of 5 .rdata strings when the port-state seed is OFF | `0x00772fbc`, `0x007731e8`, `0x00773208` at their addresses (all BSS). With the seed OFF, the 5 strings are written to their original addresses during `Save/SettingsConfig.cpp`'s dynamic initialization. `SettingsCfgGlobal` is removed |

Why 0x004282a0 needed more than its cells: its source keeps a C-only static copy of 0x004277a0 (`re/match/s06/004282a0.c`,
`MATCH-HELD`), and the DDK object remaps through that in-object copy. So the lookup slot must return a real record, as
the original's does. With the old binding the DDK body would have read the id as a pointer.

**Value equivalence on the default path:**

- Every moved cell is BSS in `MASHED.exe.unpatched` (checked with pefile: `.data` raw size 0x4d000, the cells past it), so
  the arena starts at zero, as the private cells did. On the knob-OFF path nothing else writes `0x0067d838`,
  `0x00772fbc`, `0x007731e8` or `0x00773208..` (grep of `mashedmod/src` and `re/match`. The knob path's log opener 0x00496490
  is `PortTop_Missing_00496490`, which returns 0, so the handle stays NULL).
- Lookup + S06 remap against `GetMenuMessage`: the same characters through the same control-code map (8..14). Records in
  all 7 language tables in `Font36.piz` (ENGLISH, FRENCH, GERMAN, ITALIAN, JAPANESE, SPANISH, USA, 677 ids each): max
  length 238 (FRENCH) < 511 (the old truncation), 0 odd or out-of-range offsets. The 4 VS2022 walks are class E.

**Knob-ON difference, expected and not a bar (PREREG):** the s16 matched readers of `0x00773208..` the exe compiles
(for example `re/match/s16/00498d60.c`, `00499170.c`) now see what `ConfigLoad` loaded. Before, `ConfigLoad` filled a
private copy they could not see. The knob-ON boots and walks pass. [UNCERTAIN: no knob-ON log of the integration head was
diffed against this branch, so whether any `[ORIG_TOP]` line moved because of this is not measured. This row's log diff
compares the object build against VS2022 of the same tree.]

## 3. Refusals left (not this row)

The build refuses 4 units, all `fail:wrapper-redefines-globals` through `@portdata` data defines in hand-path wrappers:

| source | wrapper | data defines |
|---|---|---|
| re/match/s03/h2_00414180.c | SubStateMachine.cpp | `DAT_005f2d80` -> `kCdRows` (a port table) |
| re/match/s01/00404e80.c | GameSaveBuffer.cpp | 9 (`DAT_007f0a40` -> `kTrackTable`, `DAT_00803358` -> `kSaveBufBase`, ...) |
| re/match/s01/00404e50.c | GameSave.cpp | `s_gamesave_bin_005cc8e0`, `DAT_00803358` |
| re/match/s01/00404e00.c | CarMaterialTables.cpp | `DAT_00639cc8` -> `Tables().body`, `DAT_00636c08`, 2 name tables |

They joined the refusal list after the spike (5 then, 9 in RESULT_DDKOBJ-REGCONV). The same route applies (cells to the
original addresses). These wrappers are hand-path files (save, car tables, sub-state machine), so they need their own
row. Check against the ROADMAP v5 freeze list before claiming it.

## 4. Files

Committed in `verify/ddkobj_20261112/`: this RESULT, `PREREG_DDKOBJ-CELLS.md`, `BYTECHECK_cells.tsv`,
`ddkobj_summary_cells.json`, `LOGDIFF_cells_on.txt`, `CAPS_cells.txt`, `run_arms_cells.sh`.

Raw (`D:/MashedWT/DDKOBJ-CELLS_data`, root `D:/MashedWT/DDKOBJ-CELLS_root` with `original` a junction to
`D:/MashedWT/PORT-TOP-04J_rootK/original`), sha256:

```
60ee94f8ded64187b6c067938a386a34a07b271e7ec187ab850542f882707f89  build_default.log
1de1880808c4271b4a35c21b89846ebe0015075a608250a9759e8257d774efd4  build_ddkobj.log
3808cfca26a857c7ab999ba494d719803a6f2bed8e01fbd3df39d52720fc7b61  selftest.log
b4180f728f612816e9ff2de3622ba6fb9c4bd6c1748c26fe300dbdc367928b68  ddk_off.log
80469698fd1e1986c25e9b54cd9450019c91451ed341487e324200fdbb6d027c  ddk_on.log
d3bae61d30c1766519f1df6bc191e0803a13428163c83b95692f2b5cc0b9e71a  vs_on.log
722eeaaebf15c03a67628561c4355a7575e884bd4bc13ae8caf017cc7f05d4ca  arms_boots.log
9aba4cb43891a07dc2505fd9bd373283241e6728051f22767eac489edfcf11f1  arms_walk.log
f9730048be09da246ff3261fd54493395fd8281cee06ded4c8c6cbb955a7fb94  mashed_re_ddk.exe (worktree mashedmod/build/ddkobj)
5502cb710b96aae94247525a59dbbf3d12bdc2a06cecc4f886e942ab4447d650  mashed_re.exe (worktree mashedmod/build)
```

Every game launch went through `mashed_lock.py run` inside `test-slot`. No PRIORITY-HOLD or SOLO was open at either
build or at any launch. The script's `wait_quiet` checked before each launch. No tracker edits, no promotions.
