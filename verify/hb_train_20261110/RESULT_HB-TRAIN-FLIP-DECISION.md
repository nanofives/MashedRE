# RESULT HB-TRAIN-FLIP-DECISION

Verdict: GREEN. `merge_train.ps1` now also treats a batch row as a decided flip when its ledger `decision` field
holds an L-n **and** its title names a default turned ON. The decision field alone is too broad (see the counts).

## Rule

`Test-FlipRow` (re/orchestrator/merge_train.ps1) flags a row when any of these holds:

1. id matches `(^|-)FLIP(-|$)` (unchanged)
2. title matches `(?i)\bdecided[ -]flips?\b` (unchanged)
3. new: `decision` matches `^L-\d+$` AND title matches `(?i)\bdefault[ -]ON\b` (`default-OFF` does not match)

The new regexes live in `$DCFG` as `flip_decision_rx` / `flip_decision_title_rx`, overridable from
`merge_train.json` `drain` like the existing two. Only the structured `decision` field and the title are read, never
the note.

## Candidate rules, counted on the live ledger (log/orch/roadmap.json, 2026-11-11, 646 rows)

| rule | rows flagged | beyond the current id/title rule |
|---|---|---|
| current: id `-FLIP` / title "decided flip" | 43 | - |
| decision field non-empty | 100 | 88 |
| decision field `^L-\d+$` | 99 | 87 |
| decision L-n + its DECISIONS.md question names flip / default ON | 65 | 53 |
| decision L-n + title says flip or default ON | 32 | 20 |
| decision L-n + title `flip ... default ON` | 5 | 0 |
| decision L-n + title `flip MASHED_x` / `MASHED_x default ON` | 7 | 1 (HB-FULLCHECK-R17) |
| **decision L-n + title `default ON` (chosen)** | **8** | **2 (D3-CF-COLLLUT, HB-FULLCHECK-R17)** |

Why the decision field alone fails: it stays set after the answer. Of the 100 rows that carry one, 81 are `merged`
and went through the train as ordinary rows (knob default-OFF ports such as PORT-STATE-01..03, D4-DC-77b0-*, policy
carriers C2-ON-KNOB L-115, L115-ADMIT-1 L-117, L115-ADMIT-2 L-118). With that rule every drain would stop at any of
them.

Near-match rows: rows citing L-115/L-117/L-118 in title or note number 9. 6 have no decision field and cannot flag.
The 3 with one (C2-ON-KNOB, L115-ADMIT-1, L115-ADMIT-2) have no `default ON` in the title and do not flag.
169 rows cite some L-n in note or title with an empty decision field. None of them flag.

Run with the real function on the ledger (verify/hb_train_20261110/flipcount_HB-TRAIN-FLIP-DECISION.ps1 extracts `Test-FlipRow` from the branch's script):
`rows 647 old 43 new 45 added: D3-CF-COLLLUT(merged), HB-FULLCHECK-R17(merged)`. Not merged/done and flagged:
D4-DC-4d20-GRID-FLIP (blocked), D4-FLIP-ST5-LEG2 (blocked), PORT-TOP-FLIP (open), HB-TRAIN-FLIP-DECISION (working).
All four are flagged by the existing id rule, not the new one.

Known false positive of the new rule: HB-FULLCHECK-R17 ("first full check of the MASHED_SNAP_POST default ON tree;
no merge"), merged. A false positive only stops a drain, it never merges anything.

## SelfTest

`py -3.12 .../proyectos_test_slot.py pwsh -NoProfile -File <wt>/re/orchestrator/merge_train.ps1 -SelfTest`:
`SELFTEST PASS (0 failure(s))`, 102 PASS lines (run twice, the second after the header comment edit, log selftest_HB-TRAIN-FLIP-DECISION.txt). New checks:

- `flip-decision: decision L-n + 'default ON' title flags, near-match L-n citations / default-OFF / plain rows do not`.
  Cases: decided flip D3-CF-COLLLUT (L-26, hashtable) -> flag; PSCustomObject row with L-92 -> flag; near-match row
  citing L-115/L-117/L-118 in note and title with no decision -> no flag (even with "default ON" in its title);
  L115-ADMIT-2 carrier (decision L-118) -> no flag; PORT-STATE-03 default-OFF knob (L-100) -> no flag; decision `P0-1`
  -> no flag; plain row -> no flag.
- `flip-decision: Get-BatchFlips reads the ledger decision field` (batch Q-1, Q-NEAR, Q-COLL -> Q-COLL only).

Mutation: the old `Test-FlipRow` returns False on the D3-CF-COLLLUT case, so the first new check fails on the old rule.

## Finding for the lead

This row's own id `HB-TRAIN-FLIP-DECISION` matches the existing id rule `(^|-)FLIP(-|$)`. A `-Drain` will refuse a
batch holding it and stop with FLIP. Merge it with a plain (non-drain) pass, or dequeue it from drains.

## Files

- re/orchestrator/merge_train.ps1: `$DCFG` gains two regexes, `Test-FlipRow` gains rule 3, header documents it,
  SelfTest gains the two checks. The orch-integration copy is untouched.
- re/orchestrator/MERGE_TRAIN.md: drain section documents rule 3 and the counts.
