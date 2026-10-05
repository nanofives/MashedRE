# RESULT — the four `GameSave` functions are now LIVE on the standalone's save path. 5 of 5 gates PASS.

**RAN 2026-10-05.** Pre-registration `PREREG_WIRE.md`, committed **unrun** at `efb267f2`. Anchor
verified before arming. Follow-on to `RESULT_GAMESAVE_EXE.md` (`54919660`), which left the bodies
present but not live.

**The first behaviour-changing edit of this session, and it passed every gate.**

---

## 1. Gates

| gate | threshold | measured | verdict |
|---|---|---|---|
| **G-BUILD** | both targets exit 0 | `mashed_re.exe` + `mashed_re_dev.asi`, `=== Build OK ===` | **PASS** |
| **G-NOREG** | (e) two gated stats unchanged 3/3; (b) exactly 13 of 30 | digit-identical, 13 of 13 | **PASS** |
| **G-BYTES** | 151456 of 151456 matching, both files 151456 | **151456 of 151456**, sizes **151456 / 151456** | **PASS** |
| **G-LIVE** | `GameSave_LastReadBytes() == 151456` | **151456** | **PASS** |
| **G-ROUNDTRIP-LIVE** | all `kAreaCount` areas restored | **13 of 13** | **PASS** |

### G-LIVE is the one that mattered

```
G-LIVE   GameSave_LastReadBytes=151456 (want 151456) -> load WENT through SaveLoad
```

That counter is written **only** inside the exe-build `gFileRead`, so a non-zero value is positive
evidence the load went through `SaveLoad` rather than past it. This gate exists because this same
session found a byte-faithful `0x00469df0` and a fully written `BodyOrient_OmegaFromAngVel` both
sitting with **zero call sites** — "linked" is not "called", and the gate was written so it would
read `0` if the wiring were inert.

### G-NOREG, digit by digit

| stat | baseline (car 1/2/3) | measured |
|---|---|---|
| `launch` | 1426.4 / 2053.0 / 2055.2 | **1426.4 / 2053.0 / 2055.2** |
| `ft_median_m0` | 2550.6 / 2053.0 / 2278.2 | **2550.6 / 2053.0 / 2278.2** |
| (e) | PASS 3/3 | **PASS 3/3** |
| (b) failing bands | 13 of 30 | **13**, same band names |

Band files proven unedited (`git diff --stat` and `git status --porcelain` both empty). Capture
`W1.csv`.

### G-BYTES and G-ROUNDTRIP-LIVE, verbatim

```
G-BYTES  SaveWrite vs fwrite: 151456 of 151456 matching, sizes 151456 / 151456 (want 151456)
G-ROUNDTRIP-LIVE  13 of 13 areas restored (unlock+trophy)
RESULT 3 of 3 gates passed
```

G-BYTES is a **single-build A/B** — one image written through `Save::SaveWrite` and through a direct
`fwrite`, compared in the same process — so it cannot be confounded by anything else changing between
builds. The previous leg's primitive self-test was re-run in the same process and still reports
**5 of 5**.

## 2. What changed

`Race/GameFlow.cpp`'s save and load no longer do their own file I/O:

| | before | after (exe build) |
|---|---|---|
| image | GameFlow's own file-static `img[kSaveSize]` | **`GameSave`'s `g_saveBuf`** via `GameSave_BufferPtr()` — one image, modelling the original's single buffer at `0x00803358` |
| write | `fopen`/`fwrite`/`fclose` | **`Save::SaveWrite()`** (`0x00404f50`) |
| load | `fopen`/`fread` | **`Save::SaveFileExists()`** (`0x00404f80`) then **`Save::SaveLoad()`** (`0x00404e50`) |
| status | untouched | `SaveStatusClear(0)` (`0x004099e0`) runs inside both, as in the original |

**All four functions are now on the live path.** The `.asi` arm is unchanged and still uses the old
code, guarded by `#ifndef MASHED_STANDALONE`.

**`SaveFileExists` before `SaveLoad` is more faithful, not less.** The original's `SaveLoad` discards
its I/O result and always returns 0 — which is exactly why `0x00404f80` exists for callers to test
beforehand.

**The `n == kSaveSize` gate is preserved, not dropped.** `SaveLoad` cannot report a byte count, so
rather than weaken the gate to magic-only — under which a **truncated** file carrying a valid magic
would parse a partly-zero span — the exe-build substitute records its count and exposes
`GameSave_LastReadBytes()`. Exe-build-only, part of the declared deviation, **no C-level claimed**.
It then doubles as G-LIVE's witness.

**A structural note worth keeping:** before this, the standalone had **two** save images — GameFlow's
and `GameSave.cpp`'s — and the ported writer had no caller at all. It now has one, which is what the
original has.

## 3. Risks that were named in advance, and how they landed

- **"The save bytes could change."** They did not: G-BYTES is byte-exact at 151456 of 151456.
- **"`MASHED_SAVE_PATH` now redirects the REAL save."** Confirmed — that is now true, and it is a
  widened blast radius. The default literal is unchanged (`mashed_re_gamesave.bin`) and
  `GameFlow`'s rule *"NEVER `original/gamesave.bin`"* still holds.
- **"The self-test deletes its own scratch file."** Still a trap if someone points
  `MASHED_SAVE_PATH` at a real save and enables the self-test. Not guarded in code; recorded here.
- **No save file was clobbered by this leg**, verified by hash after the runs:
  `original/gamesave.bin` `BD18788182B2343E…` (unchanged, and `original/` has no git modification),
  `mashed_re_gamesave.bin` `5983B387A5FC7293…` (unchanged). No stray `*.fwriteref` or self-test
  artifacts left in the repo.

## 4. What this still does NOT establish

- **Nothing about whether the ORIGINAL accepts the standalone's save.** `PREREG_SAVE.md`'s KA-ACCEPT
  remains **VOID** pending the corrected address (`*0x008a94a8`, not `0x00803358`). The file the
  standalone writes is still 345 tail bytes different from the original's.
- **Nothing about the 345-byte tail** / **U-3559**, whose writer is still unidentified.
- **No C-level moved.** The four rows keep the C4 they earned on the `.asi`; the exe copy calls
  substitutes and no C4 is claimed for it. G-BYTES/G-LIVE/G-ROUNDTRIP-LIVE are behavioural
  self-consistency checks on the standalone, **not** Frida diffs against the original.
- **No band moved**, and D4 is not closed — it has other items.
