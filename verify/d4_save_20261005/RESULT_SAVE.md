# RESULT — KA-ACCEPT is VOID: my instrument was wrong, and my own registered control caught it

**RAN 2026-10-05.** Pre-registration `PREREG_SAVE.md`, committed **unrun** at `9e62d991`, amended
**unrun** at `cdad7cd1` (the `MASHED_ROOT` copy replacing backup/restore). Anchor verified before
arming.

**No C-level moved. No band moved. No code written. No TU linked. `original/` never written.**

---

## 1. `original/` was not touched, as registered

| file | SHA-256 (first 32) | state |
|---|---|---|
| `original/gamesave.bin` before | `BD18788182B2343E5203EB983FDDD8BD` | — |
| `original/gamesave.bin` after | `BD18788182B2343E5203EB983FDDD8BD` | **unchanged** |
| `original/gamesave.bin.refbak_20260830` | `BD18788182B2343E5203EB983FDDD8BD` | untouched |
| the standalone's `mashed_re_gamesave.bin` | `5983B387A5FC72936368790F1B5FAD5A` | read only |

Two full install copies were made under a scratch path and the subject save was written **only** to
the copy, launched via `MASHED_ROOT`. `original/` was read, never written.

## 2. KA-ACCEPT: **VOID** on the control, exactly as the gate provided for

**G-MAGIC required `[0x00803358] == 0xDEADBEEF` on >= 4 of 4 samples on BOTH arms, with the
explicit clause that a failure on the CONTROL voids the leg rather than condemning arm S.**

Measured, **arm C (the shipped save)**: `[0x00803358] == 0` on **7 of 7** samples over 21.7 s.
Zero of seven read the magic.

**So the leg is VOID and arm S was NOT run.** Had I skipped the control and run only arm S, I would
have had two arms both reading zero and could have called that "the original accepts it" or "the
original rejects it" with equal and equally false confidence.

G-BOOT incidentally passed on arm C (7 samples >= 4, process alive, `ph=3` throughout), so the
install copy and the `MASHED_ROOT` redirection both work. That part of the harness is sound.

## 3. Why it was wrong — post-VOID diagnosis, declared as such

`0x00803358` is the **serialization buffer**, and the address I took it from is a *write* site:
`re/tools/gamesave_parse.py`'s header cites `0x00404F37` `MOV DWORD PTR [0x803358], 0xDEADBEEF` as
the "first-write RVA". **A buffer that is filled during a save/load call is empty at an arbitrary
sampling moment**, and the run was mid-race (`ph=3`, `spawnFired=3`), not at a profile-load moment.
I treated a write-site address as if it were live state.

One diagnostic run (arm C, `--peek`, no Interceptor) gives the corrected targets:

| address | value | reading |
|---|---|---|
| `0x008a94a8` | **77,777,432 = `0x04A26E98`** | a live heap pointer — the doc says the profile block is `REP MOVSD` **from `*DAT_008A94A8`**, so the live profile is at `*0x008a94a8`, not at `0x00803358` |
| `0x008a94ac` | **150,076 = `0x24A3C`** | stable across all samples; sits immediately adjacent to a length |
| `0x00803358` | 0 | the serialization buffer, empty between save/load calls |

**A lead for whoever re-registers this, marked `[UNCERTAIN]` and not acted on:** `0x24A3C` is **8
bytes below `0x24a44`**, which is exactly where the disk divergence between the two saves begins. If
`0x008a94ac` is the serialized length, then the serialized region ends at ~`0x24A40` and the 345
differing bytes lie **beyond it** — which would support the doc's claim that the tail is not written
by `SerializeToBuffer`, while making its stated boundary (`0x24440`, i.e. profile size `0x2443C`)
wrong by `0x600`. **I did not verify this and it must not be relied on.**

**The corrected instrument** is the indirect peek form `--peek` already supports: `@<abs>` for an
absolute address read out of an earlier peek, and `i<rvaA>+<rvaB>+<off>` for a base-plus-offset
chain (`scenario_launch.py:2348-2357`). Targeting `*0x008a94a8 + <offset>` reads the live profile.
Re-registration needed; the gates themselves can stand unchanged.

## 4. What still stands from the offline work

Unaffected by the VOID, because it is a file comparison and needs no running game:

- Both saves are **151,456 = 0x24FA0** bytes with identical `0xDEADBEEF` magic.
- **0 differing bytes** in the documented profile region; **345** in the tail, all within
  `0x24a44..0x24f58` — **169 divergent dwords**.
- The tail is **structured authored state, not padding**: 168 non-zero dwords of 728 in the original,
  a `0x30`-byte-stride record array, `0x01010101` flag words x40, the float `0x3F7FEBA4` (~0.99968)
  x13, and `0xffffffff` sentinels.
- The standalone's 5 non-zero tail bytes look **incidental** — two of them (`0x24a50`, `0x24a80`,
  both `02`) happen to match the original.
- **Refuted cheaply:** that the four asi-only `Settings*.cpp` TUs own the tail. Zero references to
  the save buffer or tail offsets, and their RVAs are boot / video-config / file-IO
  (`0x004a4541` is the `fix_fopen` site).
- **The real lead stands:** `Save/GameSave.cpp` is the only asi-only `Save/` TU referencing the save
  buffer, and it holds the whole file-I/O layer as **four C4 `impl` rows with an empty `exe_file`** —
  `0x00404e50` `SaveLoad`, `0x00404f50` `SaveWrite`, `0x00404f80` `SaveFileExists`, `0x004099e0`
  `SaveStatusClear`. It addresses the buffer as the original's absolute `0x00803358`, blank-mapped in
  the standalone, so it is a **NEEDS-STORAGE** port and the `PickupPoolSpawn` `#ifdef` recipe applies.
- **A factual error in a C4 TU's comment, owed as a comment-only fix:** `Save/GameSave.cpp:31` reads
  *"0x24fa0 — gamesave buffer size 150,432 bytes"*. `0x24FA0` is **151,456**; 150,432 is `0x24BA0`.

## 5. What is NOT concluded

- **Nothing about whether the original accepts the standalone's save.** That question is exactly as
  open as before this leg, and the 345-byte tail hypothesis is untested.
- **No C-level moves**, in either direction. In particular `GameSave.cpp`'s four C4 rows are
  untouched; this leg tested a file and an address, not those functions.
- **D4 is not advanced**, and "link the Save TUs" is still neither confirmed nor refuted as the right
  description of the work — though §4's `GameSave.cpp` finding makes it more plausible than the
  offline comparison alone suggested.

## 6. Own-work defects this leg

1. **The instrument error** (§3): a write-site address used as live state. The registered control
   caught it, which is the one thing that went right.
2. The first `PREREG_SAVE.md` asked the user for permission to write `original/gamesave.bin` under a
   backup/restore discipline, when `MASHED_ROOT` already made that unnecessary
   (`scenario_launch.py:29-31`, memory `mashed-root-asset-ab`). Found and amended **before** running,
   but it should have been found before the first commit.
