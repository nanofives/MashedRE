# PRE-REGISTRATION — move the struck-ID resolved rows, and correct the figures I got wrong

**Committed UNRUN. 2026-10-05.** A mutation of `UNCERTAINTIES.md`, the project's most load-bearing
tracker.

## 1. The population is 34, not 1362. My earlier figure was a regex artefact.

I reported "1362 marker-resolved rows are still filed under Active" and **wrote that figure into
`UNCERTAINTIES.md`'s Resolved-section header** (commit `f2b2ddcb`). It is **wrong**.

My marker regex matched `RESOLVED|CLOSED|RETRACTED|~~` **anywhere in the row**. Rows routinely
*mention* another row's resolution, so the rule caught them. Scoped to the Type cell where the
convention actually puts the marker, the count is **18, not 1396** — the broken rule over-counted by
**1378**.

**And even Type-cell scoping is unsafe.** `U-9191`'s Type cell matches, because *I* wrote the word
"RESOLVED" into its narrative this morning while the row is **explicitly still open and is the
phase's live blocker**. A bulk move on either rule would have filed the open blocker as resolved.

**The one unambiguous signal is a struck-through ID** (`~~U-NNNN~~`) — the author striking the row's
own identifier. There are **34** such rows in the Active section.

## 2. What is moved

The **34** Active rows whose ID cell matches `~~U-\d{3,5}~~`, moved **verbatim** to the end of the
Resolved section, preserving their relative order. **No cell is rewritten** — the Resolved section's
real convention is the 7-column Active shape (86 of its 95 rows), so a verbatim move is
schema-correct.

**Adjudication of their `Blocks` cells, done before writing this:** 24 read `none` / `nothing` / `—`
/ `resolved` / `~~resolved~~`; 8 read a variant of *"Blocks no function's C3"*, which is itself a
statement that nothing is gated; 3 (`U-9021`/`U-9022`/`U-9023`) hold a doc path rather than a gate.
**One is flagged and still moved: `U-9052`**, whose Blocks says *"Action 8's mechanism is pinned but
its meaning is not"* — its ID is struck, so the author closed the row, but the cell records a
residue. It is moved with that text intact and named here so the decision is visible.

## 3. Gates

- **G-CONSERVE.** The **multiset of all U-row lines must be byte-identical before and after** —
  same lines, different positions. This is the primary safety property: it makes any content
  alteration detectable. Threshold: **0 lines added, 0 removed, 0 changed**.
- **G-COUNT.** Total U-rows unchanged; Active count **-34**; Resolved count **+34**.
- **G-NONROW.** Every non-U-row line (preamble, headers, separators, prose) unchanged in content and
  relative order. Threshold: **0 differences**.
- **G-CRLF.** The file stays CRLF throughout.
- **G-NOMOVE-OPEN.** `U-9191` must still be in the **Active** section afterwards. A named,
  specific check, because it is the row the broken rule would have moved.

Any gate failing → `git checkout -- UNCERTAINTIES.md` and report, no partial state.

## 4. Also corrected in the same commit

The header block I added at `f2b2ddcb` cites **1362 / 1309 / 45**. All three are artefacts of the
broken regex and are replaced with the measured figures: **18** Type-cell-marked, **34**
struck-ID, and the explicit warning that neither rule is sufficient because U-9191 defeats both.

## 5. Not in scope

The other ~3065 Active rows. No row's content is edited, no C-level moves, no `Blocks` cell is
cleared. Whether the remaining resolved-in-prose rows should ever be moved is left open — this leg
demonstrates that no automatic rule exists for them.
