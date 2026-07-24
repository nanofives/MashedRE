# Tracker health sweep — 2026-07-24 (account2 Lane A, READ-ONLY)

Cross-tracker consistency audit of `hooks.csv` + `STUBS.md` + `UNCERTAINTIES.md` +
`DEFERRED.md`. Generator: `tracker_health.py` (scratchpad one-off, not committed). **All fixes
route through the `re-classify` skill on account3** — this doc is analysis only.

**Method notes that make the numbers trustworthy (learned mid-sweep, cost several false starts):**
- `frida_diff` is an **evidence-pointer field** (log/json path, `green`, `pending`) — NOT an
  enum. Do not test it for `==ok`.
- `hooks.csv` **status vocab has drifted** from the header comment. Real values:
  `mapped, impl, new, ported, verified, unmapped, disassembled, disasm, analyzed, hooked, stub,
  deferred`. A C4 row is legitimately `impl` OR `verified` (95/84).
- All three MD trackers split into `## Active …` then `## Resolved/Cleared (audit trail)`. The
  trail reuses Active IDs by design and often lacks `~~`; open/dup counts are only meaningful
  **within the Active section**. (A section-blind pass reported DEFERRED as 672 open / 69 dup
  IDs; section-aware it is 43 open / 0 collisions.)
- `conf C0/C1 + has analysis file` is the **library-residue convention** (797 rows = the
  third-party RenderWare band), not a defect.

---

## TIER 1 — CONFIRMED defects (actionable)

**T1.1 — 13 functions listed TWICE under VA vs file-offset RVA convention, with disagreeing
metadata.** A `000xxxxx` file-offset row normalizes to the same VA as a `004xxxxx` row but
carries a different name/subsystem/confidence. Canonical example:
`00095280 OpenPizFile / util / C2` **vs** `00495280 FUN_00495280 / render / C2` (both → VA
0x00495280). Affected VAs: `00495280 004952f0 004b6570 004b6710 004b6770 004b67e0 004b6940
005504d0 00551090 00551190 005512b0 005514e0 00551180` (piz / RtFS family). **Fix:** merge each
pair to one canonical row (keep the semantic name, drop the `FUN_`/file-offset twin).

**T1.2 — 7 C4 rows still tagged an early-stage status** (contradiction — C4 is the top
confidence): `0040b6d0 00431ae0 00431af0 00431b00 0046bce0 004c5800 004c5820`.

**T1.3 — 9 C4 rows with an empty `frida_diff` evidence pointer** (C4 gate requires
canonical-scenario evidence; the pointer is blank): `00404320 0040b6d0 00431ae0 00431af0
00431b00 0046bce0 00492770 004c5800 004c5820`. (Overlaps T1.2 — those rows are C4 with neither
a resolved status nor an evidence pointer; highest-priority to re-examine or demote.)

**T1.4 — status vocabulary drift.** (a) The `hooks.csv` header comment
(`mapped|wip|stubbed|impl|verified|wontfix`) is stale vs the real vocab above — update it.
(b) Un-normalized synonyms: `disasm`×15 vs `disassembled`×15, `stub`×2 vs `stubbed`. Pick one
spelling each.

**T1.5 — 32 dead `file` paths** (column points to a nonexistent path). Two clusters:
`re/analysis/library_residue/qhull.md` (~23 qhull rows → the note doesn't exist) and
`mashedmod/src/mashed_re/Util/StateAccessors.cpp` (0x0042b8d0 → TU moved/deleted), plus a few
`audio_dsound` plates. Repoint or clear.

**T1.6 — STUBS.md: 7 ID collisions in the Active section** (distinct rows sharing one ID):
`S-0340×2, S-0341×2, S-1441×2, S-1480×5, S-1481×3, S-1482×3, S-1485×2`. E.g. `S-1480` is 5
different hud passthroughs (RVAs 005c4d30 / 00552d10 / 00552df0 / 00552da0 / 00552e40).
Renumber so each row has a unique ID (matters for the census + any per-ID tooling).

**T1.7 — STUBS.md: 11 struck rows still sitting in the Active section** (should be moved to
`## Resolved stubs (audit trail)`), and the census header (**1,109 open / 147 struck**) no
longer matches the section-aware actual (**1,088 active-open / 173 audit-trail**). Refresh the
census line and relocate the struck rows.

---

## TIER 2 — CANDIDATE re-review queues (NOT a clean work list — heavily caveated)

These count open rows whose referenced RVA is now C3/C4. **Do not treat as a work backlog.** The
account2 workflow memory documents that these coarse joins massively over-count: careful passes
over the STUBS variant found only single-digit real strikes (the rest are library passthroughs,
live-by-design trampolines in verbatim/thunk TUs, or inlined callees). Same caution applies here.

- **UNCERTAINTIES.md: 751 open** uncertainties whose `Where`-RVA is now C3/C4. Largest lane —
  a function reaching C4 *may* have answered its open uncertainties, but a C4 can still carry
  genuine open DAT-value/offset uncertainties. Candidate re-review for account3, per-row.
- **STUBS.md: 398 open** whose called-RVA is now C3/C4 — expect a very low real-strike yield
  (see memory).
- **DEFERRED.md: 12 open** (Active section) referencing a now-C3/C4 RVA — small enough to hand-check.
- Minor: UNCERTAINTIES Active-section ID collisions (6): `U-4183 U-4184 U-4189 U-4191(×3)
  U-4192 U-4195`.

---

## TIER 3 — Informational (not defects)

- **Dangling RVA refs** (RVA in tracker but absent from `hooks.csv`): UNCERTAINTIES 1015,
  STUBS 59, DEFERRED 2. Sampled → overwhelmingly **mid-instruction addresses** (odd offsets like
  `…d6/…ad/…1f`) and **data-segment citations** (`0067xxxx` / `0089xxxx` DAT_ addresses). This is
  expected under the NO-GUESSING rule (uncertainties cite exact instruction/data addresses); not
  a defect. Only worth scanning if a *function-entry* RVA (even, `.text`, aligned) appears here.
- **UNCERTAINTIES has 2,983 open active rows** — a very large backlog. Expected given
  deterministic-RE generates many per-function DAT/offset uncertainties; flagged as a scale
  signal, not an error. Most are non-blocking (`Blocks: none`).

---

## Suggested fix order for account3

1. T1.2 + T1.3 (16 C4 rows) — confidence-integrity, smallest + highest-severity.
2. T1.1 (13 duplicate function rows) — one-time de-dup, removes a whole false-positive class.
3. T1.6 + T1.7 (STUBS IDs + census) — cheap hygiene, unblocks accurate S-DoD counting.
4. T1.4 + T1.5 (vocab + dead paths) — mechanical, route to a cheap model.
5. TIER 2 only if a promotion round wants a re-review queue — expect low yield.
