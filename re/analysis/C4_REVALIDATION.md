# C4 Re-validation Tracker

Created 2026-06-09 from the project audit (`re/analysis/AUDIT_2026-06-09.md`).
This is the dedicated tracking file for the broken-loader C4 evidence debt
(2026-05-15..24 window; see memory `project_loader_broken_9d_audit` and commit
`e798bdad` mass-disable). **Decision (user, 2026-06-09): re-validate via the
install-observe canonical harness; reconfirm what passes, demote what fails.**

## Protocol (per row)

1. Confirm the row's hook has a registered, installable reimpl (`hooks_registry.py`
   entry + `RH_ScopedInstall` present). If not → demote C4→C2 immediately
   (precedent: 22 such demotions 2026-06-05, `scripts/reclassify_c4_revalidate_demote.py`).
2. Run the install-observe canonical harness (validated 2026-06-05; throughput 4–9/day
   demonstrated 06-05..06-08): hook installed (verify 0xE9 at RVA), canonical scenario
   exercised, diff/observation evidence captured to `log/`.
3. PASS → clear the suspect tag, append post-fix evidence string to hooks.csv notes via
   `re-classify`. FAIL → demote with loud CHANGELOG line per `re/CONFIDENCE.md`.
4. All mutations through `re-classify` only.

## Population A — 74 tagged rows (C4-EVIDENCE-SUSPECT, still C4)

Authoritative list = live query, do not snapshot here:

```
grep "C4-EVIDENCE-SUSPECT" hooks.csv   # 96 tagged total: 74 still C4, 22 already demoted C2
```

The 22 already-demoted rows still carry stale tags — clear tags on demoted rows during
Phase R0 cleanup so the grep converges to the open population.

## Population B — 24 untagged escapees (audit finding, 2026-06-09)

These rows are C4 with in-window 2026-05-23 evidence (`C3->C4 2026-05-23 c4-lift`,
scenarios `boot-to-menu-canonical-r3` / `mass-canonical-wave2` / `texture_loader_d3_cont1`)
but were NOT touched by tagging commit `6e359fe8` and show no post-fix evidence in notes.
Audit verified 5 of the 24 against the tagging-commit diff; **verify each row's notes
column before acting** (NO-GUESSING).

```
00428590  00431ae0  00431af0  00431b00
004921d0  00492270  004938c0  00495120
00495270  00498c00  004c2d90  004c5800
004c5820  004c5a00  004c5ae0  004c5b50
004c5c80  004c9eb0  004c9f50  004c9f60
004cbc60  004cbc70  004cbc80  004cc820
```

Action (Phase R0): confirm each row's evidence is in-window, then add the
`C4-EVIDENCE-SUSPECT` tag so Populations A and B merge into one queryable set.

## Population C — borderline Vec3 trio

`0x004c3ac0` Vec3Magnitude, `0x004c3b30` FastSqrt, `0x004c3b90` FastInvSqrt — C4 since
2026-05-11 (commit `3f4a33f3`, "canonical-scenario observation"), untagged. Evidence file
`log/observe_hooks_at_menu.txt` is timestamped **2026-05-15 00:57 — day one of the
broken-loader window**. Whether the observation ran before or after the loader broke is
not established. Action: re-run these three through the harness regardless (they are cheap
scalar leaves; re-validation costs less than adjudicating the timestamp).

## Exit criterion

Zero `C4-EVIDENCE-SUSPECT` tags in hooks.csv AND zero C4 rows whose only canonical
evidence is dated 2026-05-15..24. Every row either reconfirmed (post-fix evidence string
in notes) or demoted (CHANGELOG line). Track progress below.

## Progress log

| Date | Rows processed | Reconfirmed | Demoted | Evidence |
|---|---|---|---|---|
| 2026-05-26 | 19 (frontend Phase A) | 9 full + 10 install-only | 2 → C3 | `log/sweep_suspect_c4_frontend_run.txt` |
| 2026-06-05 | 25 | 3 | 22 → C2 (no installable reimpl) | CHANGELOG lines 3170–3195 |
| 2026-06-06 | 1 | 1 (RwMatrixScale) | 0 | `boot_to_menu_install_observe_2026-06-06` |
| | | | | |
