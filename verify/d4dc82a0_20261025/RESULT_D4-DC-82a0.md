# D4-DC-82a0: 0x004282a0 MenuMenusBA vs MeasureMenuMessage, consolidated (W0-10)

**Verdict: GREEN.** `0x004282a0` has one body, `mashedmod/src/mashed_re/Frontend/MenuMeasure.cpp`, linked into
both targets. Allowlist row `CROSS-TARGET:004282a0` removed (8 entries to 7), rva-lint `NEW=0`. Measure table
9/9 bit-equal, menu draw lists unchanged against the baseline exe, .asi A/B GREEN, default gates unchanged.

Branch `orch/D4-DC-82a0`, base `orch/integration` `b5c84688e`. Builds `MASHED_NO_DEPLOY=1`, `original\` not
touched, the shared `mashed_re_dev.asi` not redeployed.

| commit | what |
|---|---|
| `7a77cf641` | `MASHED_DBG_MEASURE` also logs the 9 measure-table calls (debug log only). Baseline exe built here |
| `893a1e771` | the consolidation |
| this commit | evidence, `hooks.csv` row 004282a0 (file column + note, level kept C3), CHANGELOG |

## 1. What changed

The .asi's naked `MenuMenusBA` (the C3-evidenced copy, re-ported from the bytes 2026-10-12) moved unchanged into
`Frontend/MenuMeasure.cpp`, which is in `asi_sources.rsp` and `exe_sources.rsp`. The 3 callees and 2 data reads
it took at absolute addresses now go through cells declared in `Frontend/MenuMeasure.h`. Each target fills them:

| cell | original's site | .asi (`Frontend/MenuMenusB.cpp`) | exe (`exe_main.cpp`) |
|---|---|---|---|
| `g_menuBa_Lookup` | `call FUN_00427780` @0x004282c2, result in EAX | `0x00427780` | returns the id in EAX |
| `g_menuBa_Remap` | `call FUN_004277a0` @0x004282cb, EAX src, EBX buffer | `0x004277a0` | `GetMenuMessage(id, EBX, 0x200)` over `g_msg_dat` (it applies the FUN_004277a0 remap) |
| `g_menuBa_Measure` | `call FUN_005554d0` @0x004282ec, ST0 | `0x005554d0` | the old `g_font` glyph sum loop, `* size` (0x0055559f) |
| `g_menuBa_FontCell` | `mov edx,[0x0067d838]` @0x004282dd | `0x0067d838` | a zero dword (unused by the exe measure) |
| `g_menuBa_DivCell` | `fdiv [0x0067d830]` @0x004282f1 | `0x0067d830` | `DeriveMenuMeasureDiv()` = `0x3f7fffff` (U-9680) |

The two `.rdata` multipliers are now the TU's own floats. Bits read from `original/MASHED.exe.unpatched`:
`[0x005cd5fc]` = `0x3d90ff97` = `0.0708f`, `[0x005cd618]` = `0x44200000` = `640.0f`.

Compiled body, both binaries (capstone): the same 23 instructions, `sub esp,0x404` .. `ret`, the only additions
to the 2026-10-12 sequence being one `mov edx,[edx]` and one `mov eax,[cell]` for the two indirect data reads.

`exe_main.cpp:MeasureMenuMessage` keeps its signature and callers. It is now the exe's caller of the shared
body and scopes x87 `_PC_24` around the call (restoring the previous precision after). Reason: the body is x87,
so it computes at the calling thread's precision, and the original's game thread runs this at PC_24 (every
return of the 2026-10-12 probe is float-exact). That reproduces the old SSE2 per-op float rounding: section 3.

## 2. .asi side: the shared body vs the original's own

Private root `D:\MashedWT\D4-DC-82a0_root` (WORKER_RULES), `MASHED_RE_NO_AUTO_HOOK=1`, .asi `9aa11d7a`.

- `asi_measure_probe.py` / `asi_measure_probe.json`: one boot, screen 19 pushed, then the original's
  `0x004282a0` and the .asi export `MenuMenusBA` called with the same arguments, 10 ids x 7 scales
  (ids 0x59 0x1b2 0x1b1 0x5a 0x25f 0x36 40 1 5 20, scales 0.7 0.72 0.6 0.8 0.9 1.0 8.0). 70 calls on the game
  thread (inside one onEnter of 0x0042e3a0), 70 from the Frida JS thread (not the game's precision control).
  **140/140 bit-equal** (returns as doubles), `[0x0067d830]` = `0x3f7fffff`.
- `run_diff.py menu_menus_ba` through `run_diff_private.py` (same harness and negctl fixture, only the exe
  path moved to the private root): **GREEN 12/12** `diff_menu_menus_ba.csv`, self-baseline **GREEN 12/12**
  `diff_menu_menus_ba_selfbaseline.csv`. These are the 12 vectors of the 2026-10-12 re-earn, including the
  demotion vector (id 40, size 8.0, width 800).

Probe history: the first two boots in the private root AV'd during CRT init (exit `0xC0000005`, no dump), the
heap-inversion class `scripts/setup_mashed_compat.ps1` documents. The root's exe path carried only
`DWM8And16BitMitigation`. I set the shared install's layer string on that one path for the runs and restored
`DWM8And16BitMitigation` after. The other `D:/MashedWT/*_root` paths in the HKCU Layers key carry the same short value, so
original-side Frida work in a private root needs this step. Not filed as a harness row (HB) from here.

## 3. Exe side: measure table and menu parity

Baseline exe `30a1fa7b` (`7a77cf641`), consolidated exe `49c8635e` (`893a1e771`). `run_sa.ps1` = the arrow_fix
recipe with `MASHED_ROOT` = the private root, `MASHED_SETTINGS_DEFAULTS=1`, `MASHED_DBG_MEASURE=1`.

**Measure table** (`measure_table.py` / `measure_table.txt`). The 9 calls of
`verify/scr19_parity_20261012/arrow_fix/measure_table.txt` against the original's committed game-thread returns
(3 boots): **9/9 bit-equal** for the baseline exe and the consolidated exe. The full measure log (18 calls plus
the autosave arrow x, `sa32_*.log`) is identical line for line between the two exes. The log only fires on the
autosave row (screen 32), not on screen 19.

**Draw lists.** Same flags as arrow_fix (`--scale-b 1 --exclude-tex 9 --map <map> --rotate-a 0x42e65a`) against the
same committed original captures (`orig1_settled`, `orig2_settled`, `after_fix/orig_scr30`, `after_fix/orig_scr32`).

| | consolidated exe | arrow_fix committed (2026-10-13) |
|---|---|---|
| scr19 orig1 strict | 376 match, 168 checker, 0 other | 376, 168, 0 |
| scr19 orig2 strict | 376, 168, 0 | 376, 168, 0 |
| scr30 strict | 220, 168, 8 | 220, 168, 8 |
| scr32 strict | 196, 168, 0 | 196, 168, 0 |
| scr32 tol 0.01 | 192, 336, 8 | 188, 336, 16 |

"checker" = rows whose four vertex colours are all `0x00000000` / `0xff808080`, the animated background
(`classify_diff.py`, `classify_strict.txt`, `classify_tol.txt`). The 8 scr30 rows are the two original quads at
(288, 64) and (464, 64) from 0x0042e80c, missing in the committed diff too. The scr32 tol 0.01 difference is the
autosave arrow: the committed run drew it at 410.07, this run at 411 = the original. The baseline exe gives the
same 192 / 336 / 8 (`diff_scr32_base_*`), so that change predates this lane.

The `--tol-anim 4` verdicts do not reproduce the committed counts (orig1: 388 matched here, 544 committed). Every
non-matched row in those runs is a checker row (`classify_tol.txt`), so the difference is the background's
animation phase at the captured frames. Not used as acceptance.

**Baseline vs consolidated, same recipe** (`cmp_drawstream.py`, ignores the return-address list `r`, which moves
with the code layout): scr19 956 draws, scr32 648 draws, 168 differing each, all 168 checker vertex-only, 0 other
(`cmp_sa19_base_vs_after.txt`, `cmp_sa32_base_vs_after.txt`).

## 4. Gates and lints

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, exe `49c8635e`, `A4_ROOT` = the private
root (`gates_RUNLOG.txt`):

- G-CTRL `bacfc747 / 8dd1c004 / 6f9932b6` (H_DEF step / gates / ptrace)
- B `f9deb9b9 / d3601fcc / 9da9a26a` (K0 / K1 / K26)

`scripts/lint_rva_bodies.py`: 7 findings, 7 allowlisted, NEW=0. `scripts/lint_inline_asm_abs.py`: OK.

**Lock note for the lead.** The gate job's `mashed_lock.py run` broke `D3-CF-K357`'s hold as STALE
(`pid=18904 alive=True age=1801s`, the hard max-age rule), so H_DEF and the B arms may have overlapped a
D3-CF-K357 run. My hashes came out exact. If D3-CF-K357 ran a wall-clock-bound arm around 02:55-03:08 on
2026-10-25, it may need a re-run.

## 5. Trackers

Through re-classify: `hooks.csv` row `004282a0` file and exe_file columns -> `Frontend/MenuMeasure.cpp`, dated note
appended, level kept **C3** (the gate's reimplementation, build, `RH_ScopedInstall`, toggle and caller/callee
criteria hold: caller `0x0042ed70` C3, callee `0x004277a0` C3). CHANGELOG entry prepended. No UNCERTAINTIES or
STUBS rows created or closed.

## 6. Files

Scripts: `run_sa.ps1`, `run_all_sa.ps1`, `asi_measure_probe.py`, `run_diff_private.py`, `run_all_asi.ps1`,
`measure_table.py`, `classify_diff.py`, `cmp_drawstream.py`. Captures: `sa{19,30,32}_after.json/.log`,
`sa{19,32}_base.json/.log`. Diffs: `diff_*.txt/.json`. Binaries stay on `D:\MashedWT\D4-DC-82a0_data\{base,after}`.
