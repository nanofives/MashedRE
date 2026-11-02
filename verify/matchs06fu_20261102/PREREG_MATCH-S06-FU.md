# PREREG MATCH-S06-FU (committed alone, before any code change or arm)

Branch `orch/MATCH-S06-FU` off `orch/integration` @ `158fe7b71`. Follow-up of the two NOT-SAFE rows of MATCH-S06
(`verify/matchs06_20261101/RESULT_MATCH-S06.md` §"Not swapped: NOT-SAFE"). Both matched sources are already
byte-identical under `re/tools/match/match.py` (ddk, `/O2 /GS /Ob1`) with `relocheck.py` OK
(`verify/matchs06_20261101/results.tsv` rows 48 / 67, `relocheck.tsv` rows 47 / 80). No default is flipped.

## Change 1: 0x004274e0 `LocalizationFileLoad` (asi only), behaviour change behind a default-OFF knob

The matched source `re/match/s06/004274e0.c` becomes the RVA's only body (`Match/MatchS06_004274e0.cpp`, .asi only, as
the old body was). It differs from the old body (`Frontend/SkeletonAndScatter_t6.cpp`) in one case: language index
`DAT_007f0f60 == 5`. The old body returns 0 there without opening a file. The original (= the matched source, relocheck
checked=12) opens `USA.dat` (string at `0x005cd5ac`) through `FUN_004cc230(2, 1, name)` and, on a non-NULL handle, reads
0x10000 bytes into `0x0066d828`, closes, writes 1 to `0x0067d84c`, returns 1.

**Knob `MASHED_LOC_USA_DAT=1`**, default OFF, read once (`GetEnvironmentVariableA`). OFF: the installed entry returns 0
for index 5 before calling the matched body (the old behaviour). ON: the matched body runs for every index. Indices
0..4 and the default case go to the matched body in both settings (same strings, calls and stores as the old body).
The shadow A/B wrapper (`MASHED_SHADOW_AB`) and the install `RH_ScopedInstall(LocalizationFileLoad, 0x004274e0)` move
with it. `mashed_re.exe` does not compile this TU.

## Change 2: 0x004282a0 `MenuMenusBA` (exe + asi), meant to be behaviour-preserving, no knob

The matched source `re/match/s06/004282a0.c` replaces the naked body in `Frontend/MenuMeasure.cpp` as the one body of
both targets (`Match/MatchS06_004282a0.cpp`, x87 TU). The standalone maps no original data: its own image covers
0x00400000..0x00A00000 (`exe_main.cpp` WEDGE TRAP note), so a preamble `MGLOBAL` bound to `0x0067d830` would read the
exe's own `.data`. The wrapper therefore binds the body's two globals and three callees per target, keeping what each
target reads today:

| source name | .asi | exe |
|---|---|---|
| `DAT_0067d838` (font arg) | `[0x0067d838]` | zero dword (`s_menuMeasureFont`) |
| `DAT_0067d830` (divisor) | `[0x0067d830]` | `s_menuMeasureDiv` (`DeriveMenuMeasureDiv`) |
| `FUN_00427780` | `FontText_StringTableLookup` (S06 matched body) | id passed through (as today's `ExeMenuMeasureLookup`) |
| `FUN_004277a0` | `TextCtrlCodeRemapBody` (S06 matched body) | `GetMenuMessage(id, dst, 0x200)` (as today's `ExeMenuMeasureRemap`) |
| `FUN_005554d0` | raw forwarder to `0x005554d0` | `ExeMenuMeasureGlyphs` |

The exe caller `MeasureMenuMessage` keeps its PC_24 scope.

## Arms (standalone built from this branch with `MASHED_NO_DEPLOY=1`; boots in private root `D:/MashedWT/MATCH-S06-FU_root` under `mashed_lock.py run --label MATCH-S06-FU`; explicit PIDs)

- **S static.** `match.py <src> --toolchain ddk` and `relocheck.py` re-run on both sources on this branch. Build OK,
  `[rva-lint]` NEW 0.
- **K knob reach.** Byte search for the knob name `MASHED_LOC_USA_DAT` in the built `mashed_re.exe` and
  `mashed_re_dev.asi`.
- **G0 gates (default env).** `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this branch's exe.
- **M measure log.** Baseline exe (this branch's base `158fe7b71`, built unchanged) and this branch's exe, each booted
  with `MASHED_GOTO=32 MASHED_DBG_MEASURE=1 MASHED_SETTINGS_DEFAULTS=1`, and the `measure:` lines of `mashed_re.log`
  extracted (9 probe lines, 9 table lines, 1 toggle line, `exe_main.cpp` around line 5860).
- **R exe reach.** `verify/matchs06_20261101/tools/exe_reach.py` on this branch's exe + map for `MenuMenusBA`.

## Predictions (scored in this order)

1. **S:** both sources 100% (`004274e0.c`, `004282a0.c`), relocheck OK (unchecked for 004282a0 =
   `__security_check_cookie` / `__security_cookie` only); build OK; `[rva-lint]` NEW 0.
2. **K:** the knob string occurs 0 times in `mashed_re.exe` and at least once in `mashed_re_dev.asi`.
3. **G0:** G-CTRL `8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9 / 1d5bf4f9 / 1d5bf4f9` (`re/orchestrator/merge_train.json`
   `gctrl.ref` / `b.ref` on this base).
4. **M:** the 19 `measure:` lines are byte-identical between baseline and branch exe, and the 9 `table` lines equal the
   original's returns in `verify/scr19_parity_20261012/arrow_fix/measure_table.txt` (`orig` columns) to the 8 printed
   decimals.
5. **R:** `MenuMenusBA` has at least one direct exe call site and a path from `_WinMain@16`.

## Verdict mapping

- GREEN: 1..5 hold.
- RED: 3 fails (a default gate moved; filed as a decision row, not shipped) or 4 fails (the swap changed what the exe
  computes; the 004282a0 swap is reverted and reported).
- 2 failing means the knob reaches the exe: fixed before scoring, reported.
- 5 failing does not block the swap: the row stays below C4 (L-68) and the RESULT says so.

## Not measured (declared now)

The knob-ON path itself. It runs only in the dev `.asi` inside `MASHED.exe` with language index 5, which comes from
`DAT_007719e8` (launcher handshake, copied by 0x004274d0). The .asi is not deployed by this step (WORKER_RULES), and no
recipe drives a USA launch. The knob-ON claim is the static one: the ON path is the byte-matched source.
