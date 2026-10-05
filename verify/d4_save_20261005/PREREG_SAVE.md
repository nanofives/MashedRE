# PRE-REGISTRATION — D4: does the ORIGINAL accept the standalone's `gamesave.bin`?

**Committed UNRUN. 2026-10-05.** D4 ("Link the Save TUs … the standalone must read and write a
`gamesave.bin` the original accepts", `ROADMAP.md:2746-2747`).

Anchor verified before arming: `original/MASHED.exe.unpatched` SHA-256
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`, `original/launch.exe`
`01506209E42C79A4E5BEDB43DCE9FB953F0CA628B26AF9FCD49EE2522DF78DA2`.

## 0. Why the registered task description may already be wrong

The D4 item is phrased as *linking the Save TUs* (14 of 18 `Save/*.cpp` are `asi_sources.rsp`-only).
**An offline comparison done before writing this file suggests that is not the gap.** Measured on the
committed files:

| region | offset | size | differing bytes, original vs standalone |
|---|---|---|---|
| magic | `0x0000` | 4 | **0** — both `0xDEADBEEF` |
| profile block | `0x0004` | `0x2443C` (148,540) | **0** (both all-zero; `original/gamesave.bin` is an unplayed save) |
| **tail** | `0x24440` | `0xB60` (2,912) | **345** |

Both files are **151,456 bytes = 0x24FA0**. `original/gamesave.bin` is byte-identical to its own
`gamesave.bin.refbak_20260830`, so the original has not rewritten it since that backup.

The 345 differing bytes run `0x24a44`..`0x24f5b`, i.e. entirely inside the tail, and the standalone
has **5 non-zero bytes** in the whole tail against the original's populated defaults. The original's
values there are small integers (`01`, `02`, `03`) at 4- and 8-byte strides.

**That is exactly the region `re/tools/gamesave_parse.py` documents as NOT written by
`Save::SerializeToBuffer`, "shipped with non-zero defaults", writer unresolved — `[UNCERTAIN U-3559]`.**

So the hypothesis this leg tests is: **the acceptance goal is 345 tail bytes away, not 14 TUs away.**
If the original tolerates a zeroed tail, D4's save item may already be substantially met and
"link the Save TUs" is the wrong description of the remaining work.

**Registered prediction, so it can be wrong:** I expect the original **loads it without crashing**,
because the magic and the whole profile block already agree. I do **not** predict that the tail is
semantically harmless — that is what G-TAIL tests separately.

## 1. Safety — the one action that writes into `original/`

`CLAUDE.md`: *"Do not move or delete anything in `original\` without explicit user permission."*
KA-ACCEPT requires the original to read a save file, and the original reads `gamesave.bin` from its
own directory, so there is no way to run it without writing that one path.

**Registered discipline, and step 2 is NOT executed until the user approves it explicitly:**

1. `original/gamesave.bin` is copied to `verify/d4_save_20261005/gamesave.orig.bak` and its SHA-256
   recorded **before** anything is written.
2. An independent second backup already exists on disk (`gamesave.bin.refbak_20260830`, verified
   byte-identical to the live file) and is **not touched**.
3. Only `original/gamesave.bin` is written. **No other file under `original/` is created, modified,
   moved or deleted.** `MASHED.exe`, `MASHED.exe.unpatched`, `TOASTART/`, `toastaudio/` are untouched.
4. After the run, `original/gamesave.bin` is restored from the backup and its SHA-256 **re-verified
   equal to the pre-run value**. The leg reports that hash match; if it does not match, that is
   reported as a failure of this leg regardless of what else was measured.
5. If the game is killed mid-write, the restore in (4) still applies and the refbak is the second
   line of defence.

## 2. KA-ACCEPT — does the original load it? The gate that can kill the premise.

**Arms**, each a separate launch of the patched `original/MASHED.exe`:

- **arm C (control)** — `original/gamesave.bin` as shipped. Establishes what "loaded fine" looks
  like on this machine and this patch set.
- **arm S (subject)** — `mashed_re_gamesave.bin` copied over `original/gamesave.bin`.

**Instrument.** `re/frida/scenario_launch.py --peek`, which is *"No Interceptor, no hook, no write"*
(`scenario_launch.py:2338-2343`). The save buffer's documented base is `0x00803358` — the
first-write site is `0x00404F37` `MOV DWORD PTR [0x803358], 0xDEADBEEF`
(`re/tools/gamesave_parse.py` header). Peeked, as `u32` unless noted:

- `00803358:u` — the magic in the **loaded** buffer. Must read `0xDEADBEEF` on both arms.
- four offsets inside the loaded tail region, chosen from the measured divergence span
  (`0x24a44`, `0x24a4c`, `0x24a54`, `0x24a6c` relative to the buffer base), so the arms can be told
  apart in memory rather than only on disk.

**Gates, thresholds and denominators on the same line:**

- **G-BOOT.** The process must reach the main menu and survive **>= 20 s** of peeking on **both**
  arms, measured as: the peek loop produces **>= 4 samples** and the process is still alive at the
  end. A crash, an AV, or zero samples on **arm S** while **arm C** produces >= 4 is a **FAIL** and
  means the original rejects our file.
- **G-MAGIC.** `[0x00803358] == 0xDEADBEEF` on **>= 4 of 4** samples, on both arms. If arm C fails
  this, the instrument or the base address is wrong and the leg is **VOID** — not a finding about
  arm S. (This is the passing-baseline rule: an assert that fails on the control is a false RED.)
- **G-DISTINGUISH.** The four tail peeks must **differ between arm C and arm S** on at least
  **1 of 4** offsets. If they are identical on all four, the original either did not load our file or
  overwrote the tail before the first sample, and **KA-ACCEPT is INCONCLUSIVE** — a boot alone would
  then prove nothing about acceptance.

**A PASS on all three means: the original loads a save whose tail is zeroed, without crashing.** It
does **not** mean the save is semantically correct — see G-TAIL.

## 3. G-TAIL — is the tail DERIVED or AUTHORED? Runs only if KA-ACCEPT passes.

The decisive question for the remaining work. Two possibilities, and they imply opposite tasks:

- **DERIVED** — the original recomputes the tail from the profile block, so the standalone never
  needed to write it and the 345 bytes are not a defect.
- **AUTHORED** — the tail carries state with no other source, so the standalone must write it and
  U-3559 (its writer) must be resolved.

**Test:** after arm S loads, drive the original to a point where it writes the save, then compare the
file it produced against the input.

- **DERIVED** if the original's rewritten file has **>= 300 of the 345** originally-differing byte
  positions restored to the original's values, i.e. it repopulated what we left zero.
- **AUTHORED** if **<= 45 of 345** are restored, i.e. the zeros persist through a save cycle.
- Between 45 and 300: **INCONCLUSIVE**, with the per-offset table reported.

**Registered limitation:** triggering an original-side save needs a race result or a settings change,
and `ROADMAP.md` records autosave firing on race result. If no save write can be triggered within
the run, G-TAIL reports **NOT RUN** rather than guessing, and the DERIVED/AUTHORED question stays
open. **G-TAIL's result does not retroactively change KA-ACCEPT.**

## 4. What this leg does NOT do

- **It does not link any Save TU, and it writes no port code.** `exe_sources.rsp` and
  `asi_sources.rsp` are not edited. The point is to find out whether linking is the right work before
  doing it — 14 TUs whose D0.7 verdict is *"drift but inert … nearly all RVA-tunnelled"*
  (`ROADMAP.md:244-245`) would all land in the NEEDS-STORAGE class of
  `re/analysis/ASI_ONLY_TRIAGE_2026-10-05.md`.
- **It claims nothing about `GameSaveBuffer.cpp`'s C4 rows.** Memory `save-subsystem-two-paths`
  records that TU as a **dead export** with only `g_save_span` + `g_saveCounter` binding, so
  `0x00404ee0` / `0x00404e80` are C4 **at the RVA**, not evidence the exe's path is verified. This
  leg tests a **file**, not those functions, and **no C-level moves** either way.
- **No band moves and no (b)/(e) scorer runs**, because no port code changes.
- **It is not a D4 closure.** D4 is unstarted and has other items.

## 5. Process

One or two launches of the patched `original/MASHED.exe`, `--peek` only, **no Interceptor**. PID
hygiene: track the spawned PID and kill only that; never a blanket kill by name, because other
sessions may have their own `MASHED.exe` running. The restore in §1.4 runs even if the leg aborts.
