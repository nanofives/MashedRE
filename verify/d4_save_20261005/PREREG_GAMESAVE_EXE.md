# PRE-REGISTRATION — give `Save/GameSave.cpp`'s four rows exe-side bodies (Option A)

**Committed UNRUN. 2026-10-05.** D4 save. Anchor verified: `MASHED.exe.unpatched`
`BDCAE093…3C0E`, `launch.exe` `0150…8DA2`.

## 1. Scope, and the deviation stated up front

`Save/GameSave.cpp` is `asi_sources.rsp`-only and holds four **C4 `impl`** rows with an **empty
`exe_file`**: `0x004099e0` `SaveStatusClear`, `0x00404e50` `SaveLoad`, `0x00404f50` `SaveWrite`,
`0x00404f80` `SaveFileExists`.

**A verbatim port is a 12-TU closure** (measured: 17 callout targets, 14 without an exe body, pulling
in RenderWare stream functions, a driver dispatch, a texture loader and `Save/SettingsDialog.cpp`,
because `SettingsAndIO.cpp` bundles the two gamesave file wrappers with RW stream code and a dialog).
**That is NOT what this leg does.**

**THE DEVIATION, stated plainly and not to be discovered later.** In the exe build only
(`/DMASHED_STANDALONE`), the three callees become **standalone equivalents, not ports**:

| original callee | row | exe-build substitute |
|---|---|---|
| `0x004b3b70` `FileReadWrapper_i3` | C4, `SettingsAndIO.cpp`, no exe body | read the whole file with the exe's own file primitives |
| `0x004b3bb0` `FileWriteWrapper_i3` | C4, `SettingsAndIO.cpp`, no exe body | write the whole buffer |
| `0x00550b00` `VfsFileExists` | C4, `GameSaveVFS.cpp`, no exe body | existence test |

Also substituted, because both addresses are blank-mapped in the standalone: the save buffer
`0x00803358` (private `0x24FA0` storage) and the filename string `0x005cc8e0` (private literal).
`0x008a95a0`, the save-status global, likewise.

**Consequences registered now:**

- **No C-level is claimed for the three callees in the exe**, and their rows are not touched. They
  stay C4 at the RVA, earned on the `.asi`, with an empty `exe_file`.
- **The four `GameSave.cpp` rows get an `exe_file`, and NO C-level moves** either. Their C4 was
  earned on the `.asi` copy against the original's callees; the exe copy calls substitutes, so the
  existing C4 does **not** transfer to it. Precedent: `CarDropNonRenderAtomics` is recorded as *"a
  measured data-driven equivalent of the original's selection, not a verbatim port … No `hooks.csv`
  row and no C-level promotion follows from it"*.
- **The `.asi` build must be unaffected.** Every existing definition stays inside
  `#ifndef MASHED_STANDALONE`, textually unchanged.
- **The bodies will be PRESENT, not LIVE.** `RH_ScopedInstall` resolves to the no-op
  `Stubs/HookSystemNoOp.cpp:19` in the exe, so nothing calls these until `Race/GameFlow.cpp` is
  routed through them. **That routing is explicitly OUT of scope here** and needs its own gate.

## 2. Filename safety

The standalone must never write the original's save. The exe-build filename resolves to
`getenv("MASHED_SAVE_PATH")` if set, else **`mashed_re_gamesave.bin`** — matching
`Race/GameFlow.cpp`'s existing choice and its rule *"**NEVER** `original/gamesave.bin`"*. The
self-test in §3 sets `MASHED_SAVE_PATH` to its own scratch path so it cannot clobber a real progress
file.

## 3. Gates

**G-BUILD.** `mashedmod\build.bat` produces **both** `mashed_re.exe` and `mashed_re_dev.asi` with
exit 0 and no new warnings attributable to this TU. A failure stops the leg.

**G-ASI-TEXT.** The `.asi` path must be provably untouched: the four function bodies and their four
`RH_ScopedInstall` lines must be **byte-identical** to the committed versions, verified by
`git diff` showing only additions outside them plus the `#ifndef`/`#endif` wrapper. Threshold:
**0 modified lines** inside the four existing function bodies.

**G-INERT.** The default exe build must be unchanged, because the new bodies have no callers. Scored
with the declared instruments over the same 220-call window, against this build's own committed
baseline (`verify/d3_arm_20261005/BASELINE_DISTANCE.md`):

- `ai_speed_env.py --check` — `launch` **1426.4 / 2053.0 / 2055.2** and `ft_median_m0`
  **2550.6 / 2053.0 / 2278.2**, (e) **PASS 3/3**. Any digit moving on any car = **FAIL**.
- `ai_ctrl_window.py --check` — (b) failing bands **exactly 13 of 30** (car 1: 5, car 2: 4, car 3: 4).
  More than 13 = **FAIL**.
- `git diff --stat` and `git status --porcelain` on `re/tools/ai_ctrl_window.py` and
  `re/tools/ai_speed_env.py` both **empty** — the bands are not moved, and it is proven not asserted.

**G-SMOKE.** A **default-OFF** self-test, `MASHED_SAVE_SELFTEST=1`, exercises the four bodies without
touching the default path:

1. `SaveFileExists()` on a path that does not exist → must return **0**.
2. Fill the private buffer with a deterministic pattern, `SaveWrite()`, then check the file on disk is
   **exactly 151,456 bytes** (`0x24FA0` — note `GameSave.cpp:31`'s comment currently says 150,432,
   which is wrong and is corrected in this leg as a comment-only fix).
3. `SaveFileExists()` → must now return **1**.
4. Zero the buffer, `SaveLoad()`, and the buffer must equal the pattern on **all 151,456 bytes**.
5. `SaveStatusClear(0x1234)` then read back the status storage → must read **0x1234**.

**PASS requires all five, with the byte-comparison reported as `N of 151456 matching`.** A partial
pass is a FAIL and is reported as such. **Registered limitation:** this proves the four bodies are
self-consistent in the exe. It proves **nothing** about whether the original accepts the file — that
is `PREREG_SAVE.md`'s KA-ACCEPT, still VOID pending a corrected address.

## 4. Not in scope

- Routing `GameFlow.cpp` through these functions (the "present, not live" gap).
- The 345-byte save tail / U-3559.
- Any C-level move, in either direction.
- The 12-TU faithful closure.
- Re-running KA-ACCEPT.
