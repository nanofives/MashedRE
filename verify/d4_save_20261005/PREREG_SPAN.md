# PRE-REGISTRATION — the 345 bytes are the CHAMPIONSHIP SPAN, not an un-serialized tail

**Committed UNRUN. 2026-10-05.** D4 save. Supersedes the framing of `PREREG_SAVE.md` / `RESULT_SAVE.md`.

## 0. Why this replaces the boot test the user asked for, and what is kept

The user asked to re-register KA-ACCEPT against the corrected address `*0x008a94a8`. Before doing
that I read `re/tools/gamesave_edit.py`'s header, which **invalidates the premise both the earlier
prereg and my own result were built on**:

> `re/tools/gamesave_edit.py:9-14` — *"file size `0x24FA0`, magic `0xDEADBEEF` at +0; **span
> `0x24A40`, 13 rows x `0x30` bytes, 12 dword columns each**; col 1 (`+0x04`) = challenge-cup launch
> gate; col 3 (`+0x0c`) = cup membership marker (=2 on the 4 Bronze-cup rows 0..3); col 4 (`+0x10`) =
> track "known" flag (=2 on every row in the shipped save); col 11 (`+0x2c`) = quick-battle launch
> gate"*

**My measured divergence starts at `0x24a44` = `0x24A40` + 4 = row 0, column 1**, and the repeating
value `2` I found at a **`0x30` stride** is exactly this row stride. So the 345 differing bytes are
**progression state** — unlock gates, cup membership, track-known flags — and **not** the
"un-serialized tail, shipped with non-zero defaults" that `re/tools/gamesave_parse.py`'s header
describes.

**This also confirms the `[UNCERTAIN]` lead I filed in `RESULT_SAVE.md` §3** from the other
direction: `0x008a94ac` reads **`0x24A3C`**, four bytes below the span base `0x24A40`.
`gamesave_parse.py`'s region table (profile `0x0004`+`0x2443C`, "tail" at `0x24440`) is therefore
**wrong about where the profile ends**, by `0x600`.

**So the acceptance question changes shape.** It is not "will the original tolerate a zeroed tail"
but "the two files differ in the progression span, so what progression does each one describe".
**That is answerable offline, from committed files, with no game run** — and it is a sharper answer
than a boot test, which could only ever have said "it booted".

**What is NOT abandoned:** the in-game test remains available and is still the only way to show the
original *behaves* correctly on our file. It is deferred to its own leg, better targeted once the
semantic difference is known.

## 1. Instrument

Offline decode of both committed files using **`gamesave_edit.py`'s documented layout only** — no new
layout guesses:

- span base `0x24A40`, 13 rows, stride `0x30`, 12 dword columns per row.
- named columns: **1** (`+0x04`) challenge-cup launch gate, **3** (`+0x0c`) cup membership,
  **4** (`+0x10`) track-known, **11** (`+0x2c`) quick-battle launch gate.
- Unnamed columns are reported as raw dwords and **given no semantic name** (NO-GUESSING).

Files: `original/gamesave.bin` (reference, read-only) and `mashed_re_gamesave.bin` (the standalone's,
read-only). **Nothing is written. `original/` is not modified.**

## 2. KA-LAYOUT — validate the layout on the REFERENCE file before reading anything across

The decode is only trustworthy if the documented invariants hold on the shipped save. All three are
scored on `original/gamesave.bin`:

- **col 4 (`+0x10`) == 2 on all 13 of 13 rows** — the doc's *"=2 on every row in the shipped save"*.
- **col 3 (`+0x0c`) == 2 on rows 0..3** (the 4 Bronze-cup rows) — **4 of 4**.
- **col 1 (`+0x04`) and col 11 (`+0x2c`) non-zero on row 0 ONLY** — the doc's *"only row 0 set"*, so
  **exactly 1 of 13** rows non-zero in each of those two columns.

**All three must pass.** If any fails, my span base or stride is wrong, the leg is **VOID**, and no
cross-file number is reported. This is the passing-baseline rule: a decode that cannot reproduce the
reference file's documented invariants may not be used to judge another file.

## 3. G-SPAN — what actually differs, and where

Reported, with counts and both denominators:

- **G-REGION.** Of the **345** differing bytes (and **169** differing dwords), how many fall inside
  the span `0x24A40..0x24CB0` (`0x270` bytes) and how many fall **outside** it. Both counts printed.
  I expect a substantial share outside, because the divergence runs to `0x24f5b` while the span ends
  at `0x24CB0` — so this gate is descriptive and has **no pass/fail**; it bounds what the span
  explains.
- **G-PROG.** Per-row table of the four named columns on both files, plus the derived reading:
  how many of 13 tracks each file describes as **launch-gated** (col 1), **cup member** (col 3),
  **known** (col 4) and **quick-battle gated** (col 11).
- **G-VERDICT**, fixed now:
  - **CONTENT-DIFFERENCE** if the original's span is populated per KA-LAYOUT and the standalone's
    named columns are zero on **>= 12 of 13** rows. Reading: the format matches and the standalone's
    save simply describes *less progression* — the original would accept it as a near-empty profile,
    not reject it.
  - **FORMAT-DIFFERENCE** if the standalone's span is **non-zero but inconsistent** with the layout —
    e.g. col 4 takes values other than `{0, 2}` on any row — which would mean our writer is producing
    structurally wrong rows rather than merely empty ones.
  - **INCONCLUSIVE** otherwise, with the table printed.

## 4. What this leg cannot say

- **Nothing about whether the original BEHAVES correctly on our file.** No game is run. A
  CONTENT-DIFFERENCE verdict means the bytes are structurally fine, **not** that the original loads
  and plays correctly — that still needs an in-game leg.
- **Nothing about the bytes outside the span.** G-REGION counts them; it does not identify them, and
  no name is given to any unnamed column or to the region beyond `0x24CB0`.
- **No C-level moves**, no band moves, no code changes.
- **U-3559 is not resolved.** If a meaningful share of the 345 bytes lies outside the span, the
  "who writes that region" question survives in reduced form and should be re-scoped rather than
  closed.
