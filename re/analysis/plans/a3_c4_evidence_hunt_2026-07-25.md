# A3 — Tier-1 C4 evidence hunt (2026-07-25)

Produced read-only by an account2 child session spawned from the account3 parent; verdicts
reviewed here. Answers item **A3** of `re/analysis/plans/account3_handoff_2026-07-24.md`
(tracker_health T1.2 = 7 C4 rows with an early status; T1.3 = 9 C4 rows with a blank
`frida_diff` column; union = 9 unique RVAs).

**Headline:** 7 of 9 have real, on-disk, hook-INSTALLED canonical-scenario evidence — the
`frida_diff` column is simply unpopulated (a hygiene defect, not an evidence defect).
2 of 9 cite an artifact that does not exist on disk.

---

## Verdicts

| RVA | name | verdict | evidence |
|---|---|---|---|
| 00404320 | PerModeRenderMachine | **KEEP-C4** | `log/install_observe_r1b_20260609.txt:24` (batch 5) |
| 00492770 | MainLoopInit | **KEEP-C4** | same log `:27` (batch 6) |
| 00431ae0 | FUN_00431ae0 | **KEEP-C4** | same log `:27` |
| 00431af0 | FUN_00431af0 | **KEEP-C4** | same log `:27` |
| 00431b00 | FUN_00431b00 | **KEEP-C4** | same log `:27` |
| 004c5800 | RwTexDictionarySetCurrent | **KEEP-C4** | same log `:24-25` |
| 004c5820 | RwTexDictionaryGetCurrent | **KEEP-C4** | same log `:25` |
| 0040b6d0 | FUN_0040b6d0 | **AMBIGUOUS** | `CHANGELOG.md:358` cites `log/c4_racediff_result.json` — **file absent** |
| 0046bce0 | FUN_0046bce0 | **AMBIGUOUS** | `CHANGELOG.md:352` cites `log/c4_racediff_result.json` — **file absent** |

## The 7 KEEP-C4 rows — what the evidence actually proves

`log/install_observe_r1b_20260609.txt` (1886 B, 2026-06-09) is the R1-B re-validation run per
`re/analysis/C4_REVALIDATION.md`. Header L5–9 records, for 85/85 hooks across 6 runs:

- first byte at each RVA `== 0xE9` → **inline-JMP live**;
- `.asi` manifest `installed=1`;
- canonical boot-to-menu, **25 s survival**, no crash-handler hit.

This is a canonical scenario with the hook **installed** — it clears the CLAUDE.md C3/C4 line
(hook-bypassed synthetic A/B is C3 at best). **Caveat, stated plainly:** it is an
*install + survival observation*, not a per-RVA OFF-vs-ON behavioural diff. For these leaf
setters/getters and dispatch shims that was the accepted R1-B bar. **[UNCERTAIN]** whether the
current campaign intends to hold C4 to a stricter behavioural-diff bar; if it does, these 7
become AMBIGUOUS on the same footing as the two below. That is a reviewer decision, not an
evidence question.

**Action if KEEP stands:** populate the `frida_diff` column with
`log/install_observe_r1b_20260609.txt` for all 7, and fix the T1.2 status-column defect
(`new`/`mapped` alongside `C4`). Both via `re-classify`.

## The 2 AMBIGUOUS rows — precisely what is missing

Both were promoted C3→C4 on 2026-06-19 by the C4-CAMPAIGN Phase2 in-race racediff
(`re/frida/canonical_c4_racediff.py`), recorded in `CHANGELOG.md:352,358` as `off==on;
JMP live`. That is the **correct shape** of C4 evidence — so this is not a clean DEMOTE.
But the sole cited artifact `log/c4_racediff_result.json` **does not exist**; a whole-repo
search for `*racediff*` returns only the tool `.py`. The rows' own comment fields are stale,
carrying only the C2→C3 `c3_batch_ac` / `c3_batch_ad` synthetic-diff text (C3-grade at best,
and those logs were never saved either — only the batch *prompt* files survive).

**Resolution path:** account3 re-runs `py -3.12 re/frida/canonical_c4_racediff.py` for
`0x0040b6d0` and `0x0046bce0` to regenerate `log/c4_racediff_result.json`, then writes the
pointer into the `frida_diff` column via `re-classify`. Until then neither row has a citable
C4 basis. Corroborates `re/analysis/plans/b5e_c4_campaign_2026-07-24.md:89-90,95`, which
reached the same missing-artifact conclusion from the stale comment field.

## Artifact existence audit

| artifact | exists | covers |
|---|---|---|
| `log/install_observe_r1b_20260609.txt` | **YES** | 00404320, 00492770, 00431ae0/af0/b00, 004c5800/5820 |
| `log/c4_racediff_result.json` | **NO** | cited by CHANGELOG:352,358 for 0046bce0 & 0040b6d0 |
| `log/c4_install_observe_19batch_20260724.log` | YES | 19 *other* RVAs — none of the 9 targets |
| `log/batches/c3_batch_ac.txt` / `ad.txt` | prompt-only | 0040b6d0 / 0046bce0 — prompts, not results |
