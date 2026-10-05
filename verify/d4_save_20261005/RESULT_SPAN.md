# RESULT — the span explains only 18 of 345 bytes. My own reframing was over-claimed.

**RAN 2026-10-05.** Pre-registration `PREREG_SPAN.md`, committed **unrun** at `6c27b2ee`. Offline on
committed files, **no game run, no code change, `original/` read-only**. New tool
`re/tools/gamesave_spandiff.py`, output `spandiff.txt`.

---

## 1. KA-LAYOUT — PASS, so the decode is trustworthy

All four documented invariants hold on `original/gamesave.bin`, which validates the span base
`0x24A40` and the `0x30` row stride before anything was read across:

| invariant (`gamesave_edit.py:11-14`) | measured | |
|---|---|---|
| col 4 (`+0x10`) `== 2` on every row | **13 of 13** | PASS |
| col 3 (`+0x0c`) `== 2` on rows 0..3 | **4 of 4** | PASS |
| col 1 (`+0x04`) non-zero on row 0 only | **1 of 13** | PASS |
| col 11 (`+0x2c`) non-zero on row 0 only | **1 of 13** | PASS |

## 2. G-REGION — and this refutes my own pre-registration's headline

`PREREG_SPAN.md` asserted *"the 345 differing bytes are progression state"*. **They are mostly not.**

| | count |
|---|---|
| differing bytes total | **345** (169 dwords), `0x24a44`..`0x24f5b` |
| **inside** the span (`0x24A40`..`0x24CB0`) | **18 of 345** |
| **outside** the span | **327 of 345**, range `0x24cb0`..`0x24f5b` |

**The championship span accounts for 5 % of the divergence.** 95 % lies in a region starting exactly
where the span ends (`0x24CB0`) and running to `0x24f5b` — about `0x2ab` bytes that **neither
`gamesave_parse.py` nor `gamesave_edit.py` documents**.

I was right that the divergence *starts* at span row 0 column 1, and wrong to generalise from its
first bytes to all 345. That is the third successive correction of this same question in one session:

1. `gamesave_parse.py`'s header: *"tail at `0x24440`, not written by `SerializeToBuffer`"* — its
   profile/tail boundary is wrong by `0x600`; the span is at `0x24A40`.
2. **Mine, in `PREREG_SPAN.md`:** *"the 345 bytes are the championship span"* — only **18** are.
3. **Now:** 327 of 345 are in an undocumented region beyond the span.

## 3. G-PROG — what each file says about progression

| column | reference | standalone |
|---|---|---|
| col 1 challenge-cup launch gate | 1 row | **0 rows** |
| col 3 cup membership | 4 rows | **0 rows** |
| col 4 track known | **13 rows** | **2 rows** (0 and 1) |
| col 11 quick-battle launch gate | 1 row | **0 rows** |

The standalone's save is **not empty** — it describes two known tracks, which is a real (old)
progression state; `mashed_re_gamesave.bin` dates from 2026-07-31. Every value it writes in col 4 is
in the documented domain `{0, 2}`.

## 4. G-VERDICT — **INCONCLUSIVE**, as registered, and I am not arguing it down

The registered rule required **>= 12 of 13** subject rows with all four named columns zero for
CONTENT-DIFFERENCE. Measured: **11 of 13** — rows 0 and 1 carry `col4 = 2`. **It misses by one row.**

So the verdict is **INCONCLUSIVE** and stands. What can be said without the gate:
**FORMAT-DIFFERENCE is positively excluded** — the subject's col 4 takes **no** value outside
`{0, 2}` on any row, so the writer is producing structurally valid rows, not malformed ones. That is
a measured exclusion, not a downgraded pass.

**Why the threshold was wrong rather than unlucky:** I set `>= 12` on the assumption that the
standalone's save would be effectively blank. It is a real save with two tracks known, so the correct
discriminator was never "how many rows are zero" — it should have been "does any value fall outside
its documented domain", which is the clause that actually carried information.

## 5. What this establishes, and what it does not

**Established:**

- The span base `0x24A40` / stride `0x30` layout is **validated on the reference file**, and
  `gamesave_parse.py`'s region table is **wrong by `0x600`** about where the profile ends. Worth
  correcting in that tool's header.
- **The real unknown is `0x24cb0`..`0x24f5b`**, 327 of the 345 differing bytes, undocumented by any
  tool in the tree. Structure already measured (`RESULT_SAVE.md` §4, re-confirmed here by offset):
  `0x01010101` flag words x40, the float `0x3F7FEBA4` (~0.99968) x13, `0xffffffff` sentinels, and
  small integers in consecutive dwords. **No name is given to any of it.**
- The standalone writes structurally valid span rows, in-domain.

**Not established:**

- **Whether the original accepts or behaves correctly on the standalone's save.** No game was run.
  This leg says the bytes are structurally sane in the one region that is documented; it says nothing
  about the 327 bytes that are not, and nothing about runtime behaviour.
- **U-3559 is NOT resolved and is now better scoped:** the question is no longer "who writes the
  tail at `0x24440`" but **"what is `0x24cb0`..`0x24f5b` and who writes it"**. That is a smaller,
  sharper question than the row currently records.
- No C-level moved, no band moved, no code changed.
