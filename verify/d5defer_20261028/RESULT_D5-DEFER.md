# RESULT D5-DEFER (2026-10-28)

Scope: audit every Active row of `DEFERRED.md` (51 unstruck rows, lines 15-197) against its re-pickup condition.
Text-only: no MASHED.exe, no mashed_re.exe, no Frida, no original-side step. Evidence read: `hooks.csv`, `ROADMAP.md`
(D3 CLOSED block, D5 section), `Core/FaithDefaults.cpp`, `D3d9Render/TrackRenderer.cpp:190-196`, `verify/d5pdod_20261024`.
**No tracker edited.** The re-classify skill defines only function C-level transactions (hooks.csv / UNCERTAINTIES / STUBS /
CHANGELOG); it has no DEFERRED clear operation, and this step moves no C-level. Edits are listed below for the user to
route. Note: ROADMAP.md:119 says "43 live rows"; the file holds 51 unstruck Active rows.

## A. Condition met, clear (39 rows, mechanical, no judgment)
All are "decompile / classify / sweep-session" deferrals. Every RVA now has a hooks.csv row at C2 or higher, so the work
each row deferred is done. None needs the original.

| Rows | Evidence (hooks.csv confidence) |
|---|---|
| D-0245..D-0263 (19) | all 19 RVAs C2 |
| D-0281 | 004954f0 C2, 00498bf0 C4, 00498b60 C2, 0045b350 C3, 00496370 C2, 00496010 C2, 00495580 C2 (ShowCursor is an API) |
| D-6160..D-6168, D-6170, D-6172, D-6173 (12) | C3 or C4 except 0042bcb0 at C2 |
| D-7000..D-7003 (4) | all C2 |
| D-9280 (1 row, 19 RVAs) | all 19 C2 (one C3: 00490490) |
| D-0001 | boot rows at C0/C1 in hooks.csv: 0. Condition ("boot sweep session") has nothing left to sweep |
| D-8140 | debug_overlay bucket: hard negative (retail has no overlay code). Condition is batch-slot planning, and the fanout pipeline is opportunistic only. Unjustified, clear as not-applicable |

Count: 19+1+12+4+1+1+1 = 39 rows (D-8140 included). With D-11072 and D-11074 from B, 41 rows clear and 10 stay Active.

## B. Condition met or description stale, row text needs a rewrite (4 rows)
- **D-11072** (race-position recon): P1, P2, P3 and the registry legs are default ON (`FaithDefaults.cpp:26-33`: PATHPROG,
  TGT3, PATHPROG_LATE). The row's "user fidelity decision before any leg lands" was answered 2026-10-06/09. D3 closed
  2026-10-21 with (b) 9/9. Open items U-9208/U-9209/U-9212/U-9214 not re-checked here. Verdict: clear, or shrink to a
  residue pointer to the carried-forward list at ROADMAP.md:2237.
- **D-11074** (mode 7 broker): M1..M4 all done and default ON (REGISTRY, PICKUPS_ORIG, REG_CARS, REG_CALLBACKS, MODE7). The
  row's own "default flip is a user decision" is satisfied by the D3 close. Clear, leave open U-9215/9217/9218/9219 in
  UNCERTAINTIES.
- **D-11073** (sub-state machine): text "port hardcodes return 6" is stale. `aib_game_sub_mode()` now reads `0x0063ba8c`
  under `SubState_Enabled()` (MASHED_SUBSTATE, absent from FaithDefaults, so default OFF) and returns 7 under
  MASHED_ROUND_END7 (U-9779/81, carried forward non-gating). Phase 3 (fly-in, ~652 frames) is still not ported.
  Rewrite the row to what is left, see C-1.
- **D-11076** (pickup visuals): its re-pickup condition ("pickup pool tick is default ON") is MET (PICKUPS_ORIG default ON).
  The work is therefore actionable now, not deferred by dependency. Glow still waits on rfx emitters. See C-2.

## C. Genuine user calls (verdict DECISION)
1. **D-11073 / MASHED_SUBSTATE**: flip default ON (needs a gate re-score, changes hashes), port phase 3, or wontfix as
   visual-only for v1.0 (D3 closed without it).
2. **D-11076 + D-11077** (IconCube spin and glow; car part 0x12 morph / 0x14 wobble): do before ship, or wontfix as small
   visual parts. Both are D1-residue-tagged although D1-residue closed 2026-10-24.
3. **D-11060 / D-11061** (hand playthrough, continuous recording): need a human at the keyboard and an unlocked desktop. Either
   Mariano does one session (could double as the D5 shipping demo) or both become wontfix.
4. **D-11062** (intro movies): condition is "v1.0 wants intro movies". Ship decision. Standalone boots a static splash.
5. **D-11063** (split-screen): ROADMAP D5 text already treats it as out of v1.0 and d5pdod checked the citation. Recommend
   keep as the documented wontfix row.
6. **D-11078 / D-11079**: user-deferred 2026-10-17, carried forward by D3 close. D-11079 is tied to dual-copy allowlist
   (D4 gate "allowlist empty"), so it cannot stay deferred past D4. D-11078 needs the original (held, reason audio endpoint
   and no original-side runs this pass): needs a two-car original capture.
7. **D-11075** (QoL strand): justified, condition unchanged (user asks, or a port consumer appears). Keep.

## D. Held (re-pickup needs the original)
D-11078 only (two-car capture, U-9730). Nothing else in the Active table needs the original.

## E. Dev .asi leaving the shipping matrix
Not a DEFERRED row. Grep: D-11062 cites the .asi hook harness only as evidence. No row's condition depends on the .asi
shipping. Nothing to change in DEFERRED.md for that clause.

## Proposed edit (not applied)
Move A (39 rows) and D-11072/D-11074 (41 total) to `## Cleared` with date 2026-10-28 and "condition met, evidence: this RESULT". Rewrite
D-11073 and D-11076. Active drops from 51 to 10 rows (D-11060..63, D-11073, D-11075..79: 4+1+5).
