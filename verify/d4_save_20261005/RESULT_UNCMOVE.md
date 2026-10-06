# RESULT — 34 rows moved, all 6 gates PASS. The "1362" was mine and was wrong.

**RAN 2026-10-05.** Pre-registration `PREREG_UNCMOVE.md`, committed **unrun** at `d537ccc7`.
**No C-level moved, no band moved, no row's content edited.**

---

## 1. The adjudication the task asked for, and what it found

The task was "adjudicate the 45 live-`Blocks` rows, then move the 1362". **Both numbers were mine
and both were wrong**, and finding that out *was* the adjudication.

| rule | count | verdict |
|---|---|---|
| marker **anywhere in the row** (what I used) | **1396** | **broken** — rows routinely *mention* another row's resolution |
| marker in the **Type cell** (the real convention) | **18** | still unsafe, see below |
| **struck-through ID** `~~U-NNNN~~` | **34** | the only unambiguous signal |

The whole-row rule **over-counted by 1378**. The derived "1309 none-ish / 45 live" split was equally
void — and the "45" was further corrupted by reading `cell[6]` as `Blocks` when rows containing
unescaped pipes have 8, 9, 11, 21 and in one case 65 cells. `Blocks` is the **last** cell.

**Type-cell scoping does not rescue it.** `U-9191`'s Type cell matches the marker **because I wrote
the word "RESOLVED" into its narrative this morning**, while the row is explicitly still open and is
the phase's live blocker. A bulk move on either rule would have filed the open blocker as resolved.

**So the headline finding is negative and worth more than the move: there is no reliable automatic
rule for the resolved-in-prose rows.** Any future sweep of the remaining ~3065 needs a different
signal or a human pass.

## 2. What was moved

**34** Active rows with a struck-through ID, **verbatim**, to the end of the Resolved section. No
cell rewritten — that section's real convention is the 7-column Active shape (86 of its 95 rows).

`Blocks` adjudication, done before the pre-registration: **24** read `none` / `nothing` / `—` /
`resolved` / `~~resolved~~`; **8** read a variant of *"Blocks no function's C3"*, itself a statement
that nothing is gated; **3** (`U-9021`/`U-9022`/`U-9023`) hold a doc path rather than a gate.

**One flagged and still moved: `U-9052`** — ID struck, but `Blocks` records *"Action 8's mechanism
is pinned but its meaning is not"*. The author closed the row; the residue is preserved in the cell
and named here so the decision is visible rather than buried.

## 3. Gates — 6 of 6 PASS

| gate | result |
|---|---|
| **G-CONSERVE** — multiset of all U-row lines byte-identical before/after | **PASS** |
| **G-COUNT** total unchanged | **PASS** — 3194 → 3194 |
| **G-COUNT** Active −34 | **PASS** — 3099 → 3065 |
| **G-COUNT** Resolved +34 | **PASS** — 95 → 129 |
| **G-NONROW** — every non-row line unchanged | **PASS** |
| **G-NOMOVE-OPEN** — `U-9191` still in Active | **PASS** |

`git diff --stat`: **34 insertions, 34 deletions** — exactly the move, zero content churn. CRLF
intact: 3315 CRLF, **0** bare LF.

G-CONSERVE is the one that matters: identical multisets mean the same lines exist, only their
positions changed. Any rewrite, truncation or re-quoting would have failed it.

## 4. Also corrected

The Resolved-section header block I added at `f2b2ddcb` cited **1362 / 1309 / 45**. All three are
replaced with the measured figures and with the explicit warning that neither the whole-row nor the
Type-cell rule is sufficient, naming `U-9191` as the counter-example.

## 5. Not done

- The remaining **~3065** Active rows, including the ~18 Type-cell-marked ones. **No rule tested
  today supports moving them**, and that is the leg's substantive conclusion rather than an
  omission.
- No `Blocks` cell was cleared, no row's content edited, no C-level touched.
