# RESULT PORT-TRACKER-FILECOL (2026-11-09)

Branch `orch/PORT-TRACKER-FILECOL` from `orch/integration` 4abc2ce60. The re-classify skill's hooks.csv transaction,
restricted to the `file` cell: no confidence, status, `exe_file` or `port` edit by hand, and one CHANGELOG line per
row (`Cn->Cn  file column only`, the convention of the 2026-11-08 entries for 0x004325c0 / 0x00430830 / 0x004309b0).

## 1. Answer

**GREEN. 18 rows changed, 3 already right.** After the change `backfill_exe_file.py` gave 0 rows changed and
`backfill_port.py` reported "hooks.csv already current" on this tree's build (rva-lint NEW=0). `tracker_lint.py --base
orch/integration` reports `OK -- hooks.csv 6155 rows ... pre-existing=3 NEW=0`.

## 2. Rule

A row's `file` moves to the matched TU when all three hold:

1. `exe_file` names exactly one `Match/*.cpp` TU.
2. That TU is in `mashedmod/asi_sources.rsp` as well as in `mashedmod/exe_sources.rsp`, so it is the body in both
   targets.
3. The current `file` names a different `mashedmod/` source.

The old TU then holds no body for the RVA. rva-lint would report a second body as a NEW duplicate, and its one
allowlisted finding is 0x004177b0, which is not on this list. Rows whose `file` names an analysis note
(`re/analysis/...md`), or whose matched TU is exe-only, keep their `file`: that is the existing convention (`file` = the
note, or the `.asi` body), not a stale cell. Tool: `filecol/filecol_candidates.py` gives `filecol/FILECOL_CANDIDATES.tsv`,
and `filecol/filecol_apply.py` applies it, dry run by default and idempotent.

## 3. Rows

### From PORT-STATE-02 (6 asked)

| rva | name | old file | new file |
|---|---|---|---|
| 0x0040e480 | CarSlotStateSet | Ai/CarSlotState.cpp | Match/MatchS02_0040e480.cpp |
| 0x0040ce80 | PtrTable5f2770Get | Util/PromoLoop_round49.cpp | Match/MatchS02_0040ce80.cpp |
| 0x004769d0 | ParticleEmitter_SetVelocity | Render/RfxEmitter.cpp | Match/MatchS12_004769d0.cpp |
| 0x00476ae0 | Batch476ae0 | Render/RfxEmitter.cpp | Match/MatchS12_00476ae0.cpp |
| 0x00476b30 | FUN_00476b30 | Render/RfxEmitter.cpp | Match/MatchS12_00476b30.cpp |
| 0x0046dd80 | Float61313cGet | already Match/MatchS11_0046dd80.cpp | no change |

### From PORT-STATE-01 (2 asked)

0x00430830 and 0x004309b0 already name `Match/MatchS07_00430830.cpp` / `Match/MatchS07_004309b0.cpp` (CHANGELOG
2026-11-08). No change.

### Taken from PORT-TOP-04B..04G (13)

The 04B..04F RESULTs hand back no explicit `file` cell (04G has no RESULT yet). Their port-column sections list rows
that became `called` through a matched TU, and these 13 of them meet the rule in section 2:

| rva | name | old file | new file |
|---|---|---|---|
| 0x0045b350 | RwInitNullStub | Input/DirectInput.cpp | Match/MatchS09_0045b350.cpp |
| 0x0042b930 | MenuAlphaGet | Frontend/MenuGetters.cpp | Match/MatchS07_0042b930.cpp |
| 0x00426ba0 | HudDrawEnabled | HUD/HudDispatch.cpp | Match/MatchS06_00426ba0.cpp |
| 0x004274d0 | LangIndexSeedFromCli | Frontend/SkeletonAndScatter_t6.cpp | Match/MatchS06_004274d0.cpp |
| 0x0042f510 | Vehicle0HandleGet | Boot/FrameDispatch.cpp | Match/MatchS07_0042f510.cpp |
| 0x0042b8b0 | ScreenWidthGet | Frontend/MenuButtonDetect.cpp | Match/MatchS07_0042b8b0.cpp |
| 0x0042b8c0 | ScreenHeightGet | Frontend/MenuButtonDetect.cpp | Match/MatchS07_0042b8c0.cpp |
| 0x0041f320 | Car::GetState | Util/PromoLoop_round37.cpp | Match/MatchS04_0041f320.cpp |
| 0x00428590 | ViewportInit | Boot/SubsystemInit.cpp | Match/MatchS06_00428590.cpp |
| 0x00431b10 | BootParamSet2 | Boot/PromoLoop_round12.cpp | Match/MatchS07_00431b10.cpp |
| 0x00431b40 | Set67eaa8 | Util/PromoLoop_round28.cpp | Match/MatchS07_00431b40.cpp |
| 0x0042b890 | RenderWidthSet | Render/RenderSubmit_o4.cpp | Match/MatchS07_0042b890.cpp |
| 0x0042b8a0 | RenderHeightSet | Render/RenderSubmit_o4.cpp | Match/MatchS07_0042b8a0.cpp |

0x00476ae0 / 0x00476b30 are named in both lists and are counted once. 0x0042b8b0 / 0x0042b8c0: `exe_file` also lists `Frontend/MenuButtonDetect.cpp`, which holds only a comment block for them (`MenuButtonDetect.cpp:192-211`, "the body ... moved to Match/MatchS07_0042b8b0.cpp"). Its `// 0x0042b8b0` tag line is what the exe_file scanner reads, the same effect PORT-TOP-04E reported for 0x00403640. The `file` move is right. The derived `exe_file` keeps the extra entry until that comment is reworded, which is a code change and not in this row. Not taken: rows whose `exe_file` is an exe-only
TU (`PortTop_*.cpp`, `Boot/SubsystemInit.cpp` for 0x004924f0, `D3d9Render/RwWorldStream.cpp`), rows with two `exe_file`
TUs (0x004cbd30, 0x004cc050, 0x004c19f0, 0x00556cc0, 0x00556cd0), and rows whose `file` is an analysis note. Those
`file` cells follow the existing convention.

Every row stays at C4. `exe_file` and `port` are unchanged (both backfills: 0 rows).

## 4. Checks

- Build of this tree (`MASHED_NO_DEPLOY=1`): Build OK, rva-lint `1 finding(s): CROSS-TARGET=1 | allowlisted=1 NEW=0`.
- `scripts/backfill_exe_file.py`: `rows already correct: 6151   rows whose old value changed: 0`. The 4 WARN rows are
  the pre-existing short or long records (0x00404830, 0x004464c0, 0x0041f360, 0x00420870), untouched.
- `scripts/backfill_port.py`: `rows changed: 0 ... hooks.csv already current`.
- `re/orchestrator/tracker_lint.py --repo . --base orch/integration`: `OK ... pre-existing=3 NEW=0`.
- `hooks.csv`: 18 lines changed, every other byte kept. `re/analysis/CHANGELOG.md`: 18 lines inserted below
  `<!-- ENTRIES -->`.

## 5. Commits

| commit | content |
|---|---|
| this commit | hooks.csv (18 file cells), CHANGELOG (18 lines), `filecol/filecol_candidates.py`, `filecol/filecol_apply.py`, `filecol/FILECOL_CANDIDATES.tsv`, this RESULT |

No game launch in this row.
