# PRE-REGISTRATION — route `GameFlow`'s save/load through the four `GameSave` functions

**Committed UNRUN. 2026-10-05.** D4 save, follow-on to `PREREG_GAMESAVE_EXE.md` / `RESULT_GAMESAVE_EXE.md`
(commit `54919660`). Anchor verified: `MASHED.exe.unpatched` `BDCAE093…3C0E`, `launch.exe` `0150…8DA2`.

**This is the first behaviour-changing edit of this session.** The previous leg left the four bodies
**present but not live**; this one gives them call sites.

## 1. The change

`Race/GameFlow.cpp` currently does its own file I/O:

- `SaveProgress()` (`:120-130`) — `BuildImage` into a file-static `img`, then `fopen`/`fwrite`/`fclose`.
- `Campaign_LoadProgress()` (`:253-285`) — `fopen`/`fread` into a file-static `img`, gate on
  `n == kSaveSize && ParseImage(...)`.

Both are rerouted:

| | before | after |
|---|---|---|
| buffer | GameFlow's file-static `img[kSaveSize]` | **`GameSave`'s `g_saveBuf`**, via a by-name accessor — the single image, modelling the original's one buffer at `0x00803358` |
| write | `fopen`/`fwrite`/`fclose` | **`Save::SaveWrite()`** |
| load | `fopen`/`fread` | **`Save::SaveFileExists()`** then **`Save::SaveLoad()`** |
| status | not touched | `SaveStatusClear(0)` runs inside `SaveLoad`/`SaveWrite`, as in the original |

So all four functions become live. The by-name accessor follows the idiom this file already uses
deliberately for the same reason — `GameFlow_SaveCounterPtr()` (`:133-139`) exists so the ported
serializer binds to real engine state "rather than a private duplicate".

**Using `SaveFileExists()` before `SaveLoad()` is more faithful, not less:** the original's `SaveLoad`
**discards its I/O result and always returns 0** (documented at `GameSave.cpp`'s `0x00404e50` plate),
which is precisely why `0x00404f80` exists for callers to test first.

**One semantic detail preserved rather than dropped.** The old gate used `n == kSaveSize`, and
`SaveLoad` cannot report `n`. Rather than weaken the check to magic-only — under which a truncated
file carrying a valid magic could parse a partly-zero span — the exe-build substitute records its
byte count and exposes `GameSave_LastReadBytes()`. **This is exe-build-only and is part of the
already-declared deviation; no C-level is claimed for it.** The gate stays exactly
`bytes == kSaveSize && ParseImage(...)`.

## 2. Gates

**G-BUILD.** Both targets exit 0.

**G-NOREG.** The default build's scored window must not move. Save and load do not run inside the
220-call window, so this is expected to be inert — **expected, therefore measured**, against the
committed baseline: `launch` **1426.4 / 2053.0 / 2055.2**, `ft_median_m0`
**2550.6 / 2053.0 / 2278.2**, (e) **PASS 3/3**, (b) **exactly 13 of 30** failing bands. Any digit
moving on any car = **FAIL**. Band files proven unedited by `git diff --stat` and
`git status --porcelain` both empty.

**G-BYTES.** The write mechanism must be byte-equivalent to the one it replaces. In the default-OFF
self-test: build one image, write it with `Save::SaveWrite()` to path A, write the *same* image with a
direct `fwrite` to path B, and compare. Threshold: **151456 of 151456 bytes matching, and both files
exactly 151456 bytes**. Anything less = **FAIL**. This is a single-build A/B, so it cannot be
confounded by anything else changing between builds.

**G-LIVE.** Proof the four functions are actually on the live path, not merely linked. After a
`Campaign_ReloadFromSave()`, **`GameSave_LastReadBytes()` must equal 151456** — that counter is
written *only* inside the exe-build `gFileRead`, so a non-zero value is positive evidence the load
went through `SaveLoad`. Threshold: **exactly 151456**; `0` means the wiring is not live and the leg
**FAILS** regardless of the round-trip result. (This is the lesson from
`BodyOrient_OmegaFromAngVel` and `0x00469df0`: linked is not called.)

**G-ROUNDTRIP-LIVE.** Through the real GameFlow path, not the primitives:

1. Set a known progression state, `Campaign_SaveNow()`.
2. Clobber the in-memory progression to a different, recognisable pattern.
3. `Campaign_ReloadFromSave()`.
4. The restored `g_progUnlock[]` and `g_progTrophy[]` must equal the values saved in step 1 on
   **all `kAreaCount` entries**, reported as `N of kAreaCount matching`.

**PASS requires all of G-BUILD, G-NOREG, G-BYTES, G-LIVE and G-ROUNDTRIP-LIVE.** A partial pass is
reported as a partial count, not rounded up to a pass.

## 3. Risks named now

- **The save file's bytes could change.** G-BYTES exists for exactly this and is byte-exact.
- **`MASHED_SAVE_PATH` now redirects the REAL save**, not only the self-test, because GameFlow goes
  through `GameSave`'s filename. That is useful for testing and is a **widened blast radius**: a
  user setting it points real progression writes elsewhere. Recorded; the default is unchanged
  (`mashed_re_gamesave.bin`), and `GameFlow`'s rule *"NEVER `original/gamesave.bin`"* still holds
  because the default literal is unchanged.
- **The self-test deletes its own scratch file.** With `MASHED_SAVE_PATH` now shared, running the
  self-test with that variable pointed at a real save would delete it. The self-test sets its own
  scratch path, and this is noted as a trap rather than guarded in code.
- **`original/` is not touched.** No path under it is read, written or deleted by this leg.

## 4. Not in scope

- Whether the ORIGINAL accepts the file (`PREREG_SAVE.md`'s KA-ACCEPT, still **VOID**).
- The 345-byte save tail / **U-3559**.
- Any C-level move. The four rows keep the C4 they earned on the `.asi`; the exe copy still calls
  substitutes and no C4 is claimed for it.
- The 12-TU faithful closure.
