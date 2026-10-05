# RESULT — `Save/GameSave.cpp` now builds into `mashed_re.exe`. All four gates PASS.

**RAN 2026-10-05.** Pre-registration `PREREG_GAMESAVE_EXE.md`, committed **unrun** at `13610a9f`.
Anchor verified before arming.

**The first leg of this session to produce a working result.** Four of four gates pass, nothing was
retracted, and no gate was re-posed after the fact.

---

## 1. Gates

| gate | threshold | measured | verdict |
|---|---|---|---|
| **G-BUILD** | both targets exit 0 | `mashed_re.exe` **and** `mashed_re_dev.asi` built, `=== Build OK ===` | **PASS** |
| **G-ASI-TEXT** | 0 modified lines inside the four existing bodies | the only removed lines are the `kGameSaveSize` comment and its definition, moved above the `#ifndef` with an **identical value** `0x24fa0u` | **PASS** |
| **G-INERT** | (e) two gated stats unchanged on 3/3; (b) exactly 13 of 30 | see below | **PASS** |
| **G-SMOKE** | all 5 steps | **5 of 5**, round-trip **151456 of 151456 matching** | **PASS** |

### G-INERT, digit by digit against this build's own committed baseline

| stat | baseline (car 1/2/3) | measured | |
|---|---|---|---|
| `launch` | 1426.4 / 2053.0 / 2055.2 | **1426.4 / 2053.0 / 2055.2** | identical |
| `ft_median_m0` | 2550.6 / 2053.0 / 2278.2 | **2550.6 / 2053.0 / 2278.2** | identical |
| (e) verdict | PASS 3/3 | **PASS 3/3** | |
| (b) failing bands | 13 of 30 (5 / 4 / 4) | **13 of 30 (5 / 4 / 4)**, same band names | |

`git diff --stat` and `git status --porcelain` on `re/tools/ai_ctrl_window.py` and
`re/tools/ai_speed_env.py` are both **empty** — the bands were not moved, proven not asserted.
Capture: `N1.csv`.

### G-SMOKE, verbatim

```
step1 exists_before=0 (want 0)
step2 on_disk=151456 (want 151456)
step3 exists_after=1 (want 1)
step4 roundtrip 151456 of 151456 matching
step5 status=0x1234 (want 0x1234)
RESULT 5 of 5 steps passed
```

Default-OFF (`MASHED_SAVE_SELFTEST`), with `MASHED_SAVE_PATH` pointed at scratch so it cannot clobber
a progress file, and it removes its own scratch file. `MASHED_SAVE_SELFTEST_OUT` →
`selftest.txt`.

## 2. The deviation, as registered

In the exe build (`/DMASHED_STANDALONE`) three callees and three storage locations are
**standalone equivalents, not ports**:

| original | replaced by |
|---|---|
| `0x004b3b70` `FileReadWrapper_i3` (C4, no exe body) | `fopen`/`fread` |
| `0x004b3bb0` `FileWriteWrapper_i3` (C4, no exe body) | `fopen`/`fwrite` |
| `0x00550b00` `VfsFileExists` (C4, no exe body) | `fopen` probe, normalised `{0,1}` |
| buffer `0x00803358` | private `g_saveBuf[0x24FA0]` |
| filename `0x005cc8e0` | `getenv("MASHED_SAVE_PATH")` else `mashed_re_gamesave.bin` |
| status `0x008a95a0` | private `g_saveStatus` |

**The four functions' control flow is unchanged** — each still calls its primitive, then
`SaveStatusClear(0)`, then returns 0; `SaveFileExists` still normalises to `{0,1}` as the original's
`NEG/SBB/NEG` does. Only the primitives differ.

**Why a verbatim exe port was not attempted:** its transitive closure is **12 TUs**, with 17 callout
targets of which 14 have no exe body, pulling in RenderWare stream functions, a driver dispatch, a
texture loader and `Save/SettingsDialog.cpp` — because `SettingsAndIO.cpp` bundles the two gamesave
file wrappers with RW stream code and a Win32 dialog.

## 3. Trackers — `exe_file` set, NO C-level moved

Four rows gained `exe_file = mashedmod/src/mashed_re/Save/GameSave.cpp`: `0x004099e0`
`SaveStatusClear`, `0x00404e50` `SaveLoad`, `0x00404f50` `SaveWrite`, `0x00404f80` `SaveFileExists`.

**All four keep C4 `impl`, and that C4 still refers to the `.asi` copy only.** It does **not** transfer
to the exe copy, which calls substitutes. Recorded in the TU header so a future reader cannot mistake
it. Precedent: `CarDropNonRenderAtomics`, *"a measured data-driven equivalent … no C-level promotion
follows from it"*.

**No C-level is claimed for the three substituted callees** either; their rows are untouched and keep
an empty `exe_file`.

**hooks.csv churn avoided, and this nearly went wrong.** A `csv.DictWriter` round-trip re-quoted 32
unrelated rows — 36 insertions for a 4-row change. That was **reverted** and redone as a line-level
append, giving **4 insertions / 4 deletions**, 5932 rows intact, CRLF preserved.

## 4. Comment-only correction made in passing

`GameSave.cpp`'s size comment said *"0x24fa0 — gamesave buffer size 150,432 bytes"*. **`0x24FA0` is
151,456**; 150,432 would be `0x24BA0`. The constant was always right; only the gloss was wrong. The
corrected figure is corroborated on disk — `original/gamesave.bin` and `mashed_re_gamesave.bin` are
both exactly 151,456 bytes, and G-SMOKE's `on_disk=151456` confirms it at runtime.

## 5. What this does NOT establish

- **The bodies are PRESENT, not LIVE.** `RH_ScopedInstall` resolves to the no-op
  `Stubs/HookSystemNoOp.cpp:19` in the exe, so **nothing in the standalone's call graph calls these
  four functions.** Only the default-OFF self-test exercises them. Routing `Race/GameFlow.cpp`
  through them is the next step and was explicitly out of scope.
- **Nothing about whether the original accepts the standalone's save.** `PREREG_SAVE.md`'s KA-ACCEPT
  is still **VOID** pending a corrected address (`*0x008a94a8`, not `0x00803358`).
- **Nothing about the 345-byte save tail** / U-3559.
- **No C-level moved**, in either direction, and **no band moved**.
- G-SMOKE proves these four bodies are **self-consistent in the exe**. It is not a Frida diff against
  the original and no C4 claim rests on it.

## 6. Artifacts

`selftest.txt` (G-SMOKE), `N1.csv` + `N1_t08/t30/t60.png` (G-INERT capture), and the one-line
`exe_sources.rsp` addition.
