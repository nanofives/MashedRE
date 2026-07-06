# D-11056 rule-5 READER-side survey + array resolution (claude2, 2026-07-03)

Companion to `d11056_collectible_feed_survey_2026-07-03.md` (feeder side — who calls the
registrar) and `d11056_fable_ghidra_findings_2026-07-03.md` (Fable's Ghidra results). This
one takes the **reader side** — what the rule-5 win predicate actually reads for its total
— and **synthesizes the two prior notes to answer the open "which array" question**, then
pins the *minimal* Ghidra set Fable still needs. Read/draft only (no MCP); all RVAs cited
from the existing plates + the ported `Race/RuleEngine.{h,cpp}`.

Binary anchor: MASHED.exe SHA-256 (unpatched) `BDCAE093…`.

## The rule-5 reader chain (CONFIRMED from ported decomp)

- **`FUN_00405890` (0x00405890)** — rule-5 win predicate, ported verbatim as
  `RuleEngine::CollectAllDone`:
  `if (DAT_0063a5d0 == 0) return false; return DAT_0063a5d4 == DAT_0063a5d0;`
  i.e. `collectTotal != 0 && collectDone == collectTotal`.
  - **total = `DAT_0063a5d0`**, **done = `DAT_0063a5d4`**.
  - Callers (from the plate): `FUN_00410510`, `FUN_00410d10` (the rule dispatch /
    SegmentCheck+EvaluateResult, ported in RuleEngine), `FUN_00429e10`, `FUN_00446520`,
    `FUN_00464a50`.
- **`DAT_0063a5d0` (total)** — incremented by **`FUN_00405780`** (the copter/waypoint-
  object registrar), which per the Fable findings note is called **only by `KTC_NewCopter`
  (0x0047b720)** at track-KTC-parse time.
- **`DAT_0063a5d4` (done)** — incremented by **`FUN_004064c0`** (per-object completion
  updater) when `(100.0f - record[+0x5c]) * 0.01f >= 1.0f`; driven per-frame by
  `FUN_00406ce0` over the `DAT_00639d80` records. The `+0x5c` "touch/collect" trigger
  mechanics are **untraced** (survey open item).
- **Reset** — `FUN_004074a0` zeroes both `DAT_0063a5d0` and `DAT_0063a5d4`.

## Resolving the findings-note open question (SYNTHESIS)

The findings note left open: *"does rule-5's `collectTotal` count the `KTC_NewCopter` copter
array (`DAT_0063a5d0`) or the separate `KTC_AddPickUp` array?"* Combining the reader side
(above) with the feeder side (findings note) answers it **on documented evidence**:

- The rule-5 predicate reads **`DAT_0063a5d0`** (not any pickup-array symbol).
- `DAT_0063a5d0` is incremented **only** by `FUN_00405780` = the **copter/waypoint-object**
  registrar (findings note: "registers one copter object into `DAT_00639d80` and increments
  `DAT_0063a5d0`"), whose **sole** feeder is `KTC_NewCopter`.
- `KTC_AddPickUp` (0x0047b7d0) **does not** call `FUN_00405780` (findings note table), so it
  does **not** touch `DAT_0063a5d0`.

**⇒ Provisional answer: the rule-5 total IS the `KTC_NewCopter` copter/waypoint-object array
(`DAT_0063a5d0`); `KTC_AddPickUp` is a separate system that does NOT feed rule-5.** So the
standalone feed should drive `SetCollectibles(count)` from the **count of `KTC_NewCopter`
commands** parsed at track load, and `OnCollect` from the `FUN_004064c0` completion
condition — matching the existing survey's sketch, now with the source pinned.

[UNCERTAIN] This rests on "`FUN_00405780` is the ONLY writer of `DAT_0063a5d0`", which the
findings note derived from `function_callers` — and that call-graph **missed** the
`KTC_NewCopter→FUN_00405780` edge (unbound-function artifact). So an xref-level check (which
catches computed/unbound refs) is required to be definitive — see below.

## Minimal Ghidra set for Fable (tight scope — 2 xrefs + 2 decodes)

This is all that remains to close D-11056's array question + wire `OnCollect`:

1. **`reference_to 0x0063a5d0`** — confirm the **only writer** is `FUN_00405780` and the
   rule-5 **reader** is `FUN_00405890` (definitive, catches the computed/unbound refs the
   static call graph missed). If a second writer appears, re-open the array question.
2. **`reference_to 0x0063a5d4`** — confirm the only incrementer is `FUN_004064c0`.
3. **decode `FUN_004064c0` (0x004064c0)** — the collect TRIGGER: what sets `record[+0x5c]`
   toward 0 (the per-object "collected" event). Needed to wire `TrackRenderer::OnCollect`
   faithfully (currently untraced — the survey's remaining mechanics gap).
4. **decode `KTC_AddPickUp` (0x0047b7d0)** — confirm which array it populates, to formally
   rule it out as a rule-5 feed (and document whether pickups are a separate non-rule-5
   subsystem). Cheap; closes the "which array" question with certainty.

Everything else (the registrar call path, the per-track KTC_NewCopter parse) is already
mapped in the feeder survey; items 1–4 above are the irreducible MCP tail.
