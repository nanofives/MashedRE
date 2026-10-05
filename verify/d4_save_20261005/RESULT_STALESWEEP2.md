# RESULT — the sweep is reliable now, and the `status:` column is NOT needed

**RAN 2026-10-05.** Follow-on to `RESULT_STALESWEEP.md`, whose tool failed its own known-answer
check. **No schema change was made. No C-level moved, no band moved.**

---

## 1. Why the column was not added

The task was "add the `status:` column and make the sweep reliable". I surveyed the file first, and
**`UNCERTAINTIES.md` already carries two status mechanisms** — I had not looked:

| mechanism | rows |
|---|---|
| a **Resolved section** (`\| ID \| Type \| Resolved date \| Resolution \|`, line 3187) | **60** of 3125 — **~98 % unused** |
| a **marker in the Type cell** (`~~structural~~ **RESOLVED …**`) | **1362** ACTIVE rows |
| resolved **in prose only**, no marker | **170** ACTIVE rows |
| no resolution signal | 1533 |

**Adding a third mechanism alongside two existing ones would make the file harder to read, not
easier**, and it is a 3125-row rewrite. The honest finding is that **the Resolved section is the
abandoned convention** — 1362 marker-resolved rows are still filed under Active rather than moved.

So the column is **not done**, and I am putting that decision back to the owner with the numbers
rather than executing a large irreversible edit I now believe is the wrong shape. `CLAUDE.md`'s
"stop and ask" covers exactly this.

## 2. The sweep was made reliable without any schema change

The first version read **only** the Type marker, so it classified U-3559 as open and missed the case
it was built for. It now classifies every row into **section / marker / prose / open** and reports
the first three as **separate buckets with their confidence stated**.

**Known-answer check, re-run against the pre-fix `gamesave_parse.py` (`e3e60fec~1`):**

| | before | after |
|---|---|---|
| hits on the archetype | **0 — FAIL** | **3 — PASS**, including **U-3559** |

The prose bucket is where U-3559 lives, and dropping it silently was the whole bug.

## 3. A systematic noise class, measured and filtered

`scripts/reclassify_batch_*.py` carry lines like *"Mint U-IDs from U-8000 for every bare
`[UNCERTAIN]` marker in the 156 plates"* — **mint-time provenance**, not a claim that a row is open.
That was **9 of 19 hits**, nearly half the output. Now skipped by default;
`--include-batch-scripts` restores them.

## 4. Adjudication of what remains

Scoped to tools and harnesses (the dangerous class — these drive measurements):

- **TRUE POSITIVE, fixed: `re/frida/cam_frame_writer_watch.py:1`.** Its first line — the tool's
  stated purpose — reads *"U-9058. Find WHO writes the RwCamera frame matrix"*. **U-9058 was RESOLVED
  on 2026-08-30, the same day it was filed:** the frame basis is written by `Camera::InitWithMatrix`
  (`0x00442a20` @ `0x00442a4e`). A header note now records the answer and points at the row. This is
  the second instance of the `gamesave_parse.py` pattern, though **not** a live bug — no wrong
  constant, just an obsolete purpose line.
- **Superseded framing, deliberately left: `probe_save_globals.py:3`** (U-3558). Adjudicated in
  `RESULT_U3559.md` — `0x7F1038 + 0x24 = 0x7F105C`, so both name the same address.
- **Still-open sub-questions, correctly cited:** `hooks_registry.py` U-1552, U-5551, U-6621 — rows
  resolved on one axis while the code cites a different, still-open detail.
- **Self-references:** the two tools that now *discuss* U-9058 and U-8000 flag themselves. A known
  false-positive class, not worth special-casing.

## 5. What this does not claim

- **A clean run still does not mean "no stale citations".** The prose detector is deliberately
  narrow; rows that state an answer in wording it does not match are still missed.
- The 1533 "no resolution signal" rows are **unclassified, not verified open**.
- Only tools/harnesses were adjudicated. `mashedmod/src` has ~45 further hits, unreviewed.
- No tracker row was edited and no C-level moved.
