# RESULT MATCH-S06-FU, 2026-11-02

**Verdict: GREEN.** The two NOT-SAFE rows of MATCH-S06 are swapped. PREREG `PREREG_MATCH-S06-FU.md`, committed
alone as `1347114da` before the code (`7158c0dc6`) and before any arm. Scored in the PREREG's order: 1 S PASS, 2 K PASS,
3 G0 PASS, 4 M PASS, 5 R PASS.

- **0x004282a0 `MenuMenusBA`** (exe + .asi): the byte-matched source `re/match/s06/004282a0.c` is now the one body of
  both targets (`Match/MatchS06_004282a0.cpp`, x87 TU). The naked `Frontend/MenuMeasure.cpp` is deleted. The exe calls
  it (3 direct sites, path from `WinMain`), the 19 measured returns are bit-identical to the baseline exe, and the 9
  returns the original was probed for are equal to the original's. **C4 candidate by the `match` route** (text below).
- **0x004274e0 `LocalizationFileLoad`** (.asi only, as before): the byte-matched source `re/match/s06/004274e0.c` is the
  .asi's one body (`Match/MatchS06_004274e0.cpp`). The one behavioural difference from the old body (language index 5
  opens `USA.dat`) runs only with **`MASHED_LOC_USA_DAT=1`, default OFF**. No exe body, no exe caller: stays C2 (L-68).
  Turning the knob on by default is a decision row, not filed here (the knob only affects the dev .asi, see below).

## Why the S06 note called 0x004282a0 NOT-SAFE, and how this swap avoids it

The exe side of D4-DC-82a0's shared body read port-owned cells (`GetMenuMessage` over `g_msg_dat`, the `g_font`
glyph sum, the derived divisor `s_menuMeasureDiv`). The matched source names the original's globals through the S06
preamble (`MGLOBAL` = a reference to the original address). In `mashed_re.exe` those addresses are not the original's
data: the exe's own image (ImageBase 0x00010000) covers 0x00400000..0x00A00000 (`exe_main.cpp` WEDGE TRAP note), so
`[0x0067d830]` in the exe is some exe variable. A plain wrapper would have read that and changed every menu width.

`Match/MatchS06_004282a0.cpp` therefore skips the source's preamble (`MATCH_PREAMBLE`) and defines its own `MGLOBAL`
that binds each global through a per-target cell, and maps each callee name to a per-target function:

| source name | .asi | exe |
|---|---|---|
| `DAT_0067d838` | `[0x0067d838]` | `g_menuBa_FontCell` -> zero dword (the exe's measure ignores the font argument) |
| `DAT_0067d830` | `[0x0067d830]` | `g_menuBa_DivCell` -> `s_menuMeasureDiv` (`DeriveMenuMeasureDiv`) |
| `FUN_00427780` | `FontText_StringTableLookup` (S06 matched body) | `MenuBa_ExeLookup` (id passed through) |
| `FUN_004277a0` | `TextCtrlCodeRemapBody` (S06 matched body) | `MenuBa_ExeRemap` (`GetMenuMessage(id, dst, 0x200)`) |
| `FUN_005554d0` | raw forwarder to `0x005554d0` (its port body is under `#if 0`) | `MenuBa_ExeMeasure` (`ExeMenuMeasureGlyphs`) |
| `DAT_007d3ff8` | `[0x007d3ff8]` | private zero (declared by the source, not read by the body) |

The exe's three callee stand-ins are the same logic as the old naked-ABI thunks (`ExeMenuMeasureLookup` /
`ExeMenuMeasureRemap` / the `g_menuBa_Measure` cell), now plain cdecl C functions. `MeasureMenuMessage` keeps its PC_24
scope. One body for the RVA in both targets, so no CROSS-TARGET pair comes back (`[rva-lint]` NEW 0).

**Exe codegen check** (`D:/MashedWT/MATCH-S06-FU_data/exe_g1/mashed_re.exe`, `_MenuMenusBA` at 0x00181a50): the body
reads the font and divisor through the two reference cells (`mov eax,[0xf1c5d4]; push [eax]`, `mov eax,[0xf1c5d0];
fdiv [eax]`), never an original address. VS2022 emits the two literals as `fmul qword`: `[0x25b348]` =
0.07079999893903732 (f32 bits `3d90ff97`, the original's `[0x005cd5fc]`) and `[0x25b3d8]` = 640.0 (`44200000`,
`[0x005cd618]`), both exact float values, so at PC_24 each product rounds as the original's `fmul dword` does. The
order is the original's: `scale * 0.0708` rounded to the float argument, call, `fdiv` divisor, `fmul` 640. Arm M is the
measurement of that.

## Arms

### 1. S static: PASS (`static_S.txt`, `build_lint.txt`)

```
0x004274e0 _FUN_004274e0: 100.0% (regs 100.0%, ops 100.0%)  38/38 ins  flags=/O2 /GS /Ob1 tc=ddk
   bytes: 148 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
0x004274e0  OK  checked=12  unchecked=0
0x004282a0 _FUN_004282a0: 100.0% (regs 100.0%, ops 100.0%)  26/26 ins  flags=/O2 /GS /Ob1 tc=ddk
   bytes: 122 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
0x004282a0  OK  checked=7  unchecked=2  unchecked: @__security_check_cookie@4,___security_cookie
```
`set MASHED_NO_DEPLOY=1&& mashedmod\build.bat`: Build OK, `.asi` not deployed. `[rva-lint]` 3 known, NEW 0.
`[asm-lint]` OK.

### 2. K knob reach: PASS

`MASHED_LOC_USA_DAT` occurs 0 times in `mashed_re.exe` and once in `mashed_re_dev.asi` (`grep -c -a`).

### 3. G0 default gates: PASS

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on the branch exe (`A4_EXE` = `exe_g1`),
private root `D:/MashedWT/MATCH-S06-FU_root`, test slot + `mashed_lock.py run --label MATCH-S06-FU` (plain waiter, no
solo). The first attempt booted nothing useful: `run_arms.py` does not create `A4_OUT`, so its copy step failed; the
runner now creates the folder and the arms were re-run.

| arm | file | sha8 | ref (`merge_train.json`) |
|---|---|---|---|
| H_DEF | step.csv / gates.csv / ptrace.log | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e |
| B_DEF_K0, K1, K26 | step.csv | 1d5bf4f9 x3 | 1d5bf4f9 x3 |

Every arm exit 0, self-exited (H_DEF 317 s, 13962 step lines; B arms 17-21 s, 762 lines). No gate hash moved, so no
DECISION row.

### 4. M measure log: PASS (`measure_base.txt`, `measure_g1.txt`)

`tools/run_measure.py`: each exe booted with `MASHED_GOTO=32 MASHED_DBG_MEASURE=1 MASHED_SETTINGS_DEFAULTS=1`,
cwd = `MASHED_ROOT` = the private root. Baseline exe = this branch's base `158fe7b71` built unchanged
(`exe_base`, sha256 `4834848c...`), branch exe = `exe_g1` (sha256 `ce024788...`). Both runs PASS (19 `measure:` lines
each, PIDs 22840 / 22372, killed by PID).

- The two files are byte-identical (sha256 `0adee4e04fed6065...` both).
- The 9 `table` lines equal the original's returns in `verify/scr19_parity_20261012/arrow_fix/measure_table.txt`:
  0x59: 29.79607582 / 30.64739418 / 25.53949356; 0x1b2: 45.17469788 / 46.46540833 / 38.72116852; 0x1b1: 75.93193817 /
  78.10141754 / 65.08451843 (scales 0.7 / 0.72 / 0.6). The toggle arrow x is 411 (bits `43cd8000`), the original's 411.

### 5. R exe reach: PASS (`exe_reach.tsv`)

`MenuMenusBA` 0x00181a50: 3 direct sites, reaches `WinMain`:
`_MenuMenusBA <- MeasureMenuMessage <- RenderFrame <- _WinMain@16`.

## Not measured (as declared in the PREREG)

The knob-ON path of 0x004274e0. It runs only in the dev `.asi` inside `MASHED.exe` with language index 5, which comes
from `DAT_007719e8` (launcher handshake, copied by 0x004274d0). The step does not deploy the .asi and no recipe drives
a USA launch. The ON path is the byte-matched source, which is the static claim. The OFF path equals the old body by
construction: the entry returns 0 for index 5 before the body, and indices 0..4 / default are the same strings, calls
and stores in both.

## Re-classify text (for the lead, through re-classify after the merge; not applied here)

- **0x004282a0** `MenuMenusBA`: C3 -> **C4**, route `match` (c4=match), file and exe_file
  `mashedmod/src/mashed_re/Match/MatchS06_004282a0.cpp` (was `Frontend/MenuMeasure.cpp`). notes append:
  `match evidence: identical (re/match/s06/004282a0.c 23ff5a8f05fb886d, 100%, ddk /O2 /GS /Ob1), relocheck OK checked=7
  unchecked=2 (__security_check_cookie, __security_cookie), swapped in both targets (replaces the naked D4-DC-82a0 body;
  globals and callees bound per target, the exe keeps its own cells), exe-callers=3 (MeasureMenuMessage <- RenderFrame
  <- WinMain), measure log 19/19 bit-identical to the pre-swap exe and 9/9 equal to the original's probe, gates
  unchanged (MATCH-S06-FU)`. Per CONFIDENCE "How the Write-set leg relates": the callees in the exe are port stand-ins
  (`GetMenuMessage`, the `g_font` glyph sum), so the row is not evidence for 0x00427780 / 0x004277a0 / 0x005554d0.
  CHANGELOG: `2026-11-02  0x004282a0  MenuMenusBA  C3->C4  route match (c4=match), exe-callers=3
  (Match/MatchS06_004282a0.cpp); evidence: verify/matchs06fu_20261102/RESULT_MATCH-S06-FU.md`.
- **0x004274e0** `FUN_004274e0` / `LocalizationFileLoad`: stays **C2**. file `Match/MatchS06_004274e0.cpp` (.asi only),
  exe_file empty. notes append: `match evidence: identical (re/match/s06/004274e0.c 59d2d25807a8707a, 100%), relocheck OK
  checked=12, swapped (.asi only), exe-callers=0; index 5 (USA.dat, 0x005cd5ac) behind MASHED_LOC_USA_DAT (default OFF);
  C4 when a caller lands (L-68 ruling) (MATCH-S06-FU)`. relocheck resolving `s_USA_dat_005cd5ac` is the operand witness
  for **U-0869** ("case 5 USA.dat, xref unconfirmed"): the original's case-5 arm pushes 0x005cd5ac. The lead can close
  U-0869 through re-classify on that evidence.

## Rebase notes (for the merge train)

rsp edits are two lines each and sit in the `MatchS06_*` block only: `Frontend\MenuMeasure.cpp` removed from both,
`Match\MatchS06_004282a0.cpp` inserted after `MatchS06_00428140` in both, `Match\MatchS06_004274e0.cpp` inserted after
`MatchS06_004274d0` in `asi_sources.rsp`. `x87_tus.txt`: one line after `MatchS06_00428140`. No `MatchS07_*` line is
touched.

## Raw files (not committed; `D:/MashedWT/MATCH-S06-FU_data`)

```
4834848c59fbae4d3bb56d2101d57baa2e14ccdbb1e4906b62047e0d17612b9d  exe_base/mashed_re.exe (base 158fe7b71)
12cc546773443bf81d989f861aff98ee7e1ff8f1c124970527c860a0b383826f  exe_base/mashed_re.map
ce024788085dbc551bb7bcff5f907079b6783fadfbd0c7a9c0cd7836f6d11e05  exe_g1/mashed_re.exe (7158c0dc6)
2cdc32583b7410323cf78b0c56eb6def96d0a9a69290275324d61ee60ba470ca  exe_g1/mashed_re.map
40a6ac82d7c593a2945e0c6257a221ac49797a3be098a763e3abb43debbd5ac1  exe_g1/mashed_re_dev.asi
10a828736fa902ac929b4c02aa009ecd61a3d1373bd9f45abab09350dfcc67e0  build_base.log
236922ecd23d40a17ee06ea96801696890fafc2b7a80b0d2dbabb1e9a079a6e7  build1.log
7bf7f606829990a280f341a9f6ed8304230cb06aa485898776cd79f45b045100  measure/base.log
e57512d48613d1327a2e8b385adb9bcfbd3917cb5f715394d37b4303c2d92593  measure/g1.log
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  arms_g1/H_DEF.step.csv
ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb  arms_g1/H_DEF.gates.csv
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3  arms_g1/H_DEF.ptrace.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g1/B_DEF_K0.step.csv, B_DEF_K1.step.csv, B_DEF_K26.step.csv
```
