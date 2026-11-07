# RESULT C4R-ROADMAP (run 35, 2026-11-07)

Verdict: **GREEN** (text only, no game run, no build, no gates).

Decision: L-93 (Mariano 2026-11-07, option B), `log/orch/DECISIONS.md`.

## What changed

| File | Change |
|---|---|
| `ROADMAP.md` (v3, 3,144 lines) | moved with `git mv` to `re/analysis/archive/ROADMAP_v3_2026-08-15.md`, content unchanged (same pattern as v2 -> `re/analysis/archive/ROADMAP_v2_2026-06-09.md`), so every v3 line number still resolves in the archive |
| `ROADMAP.md` (v4, 265 lines) | new, two axes |
| `CLAUDE.md` | section "Roadmap, DoD, and trackers" only: current-plan paragraph and the `ROADMAP.md` bullet. "Confidence promotion" and `re/CONFIDENCE.md` untouched (C4R-RUBRIC owns them) |
| `re/NEXT_SESSION.md` | new START HERE block on top pointing at v4, the archive, and the pre-L-93 C4 wording in the older blocks |

## Links kept working

- v4 opens with an "Old citations" note: `ROADMAP.md §D0..§D5`, `ROADMAP.md:<line>`, "Trajectory correction",
  "Open decisions", "Workstream ledger", "Honest baseline", "D3 amendment 2026-10-09" written before 2026-11-07
  mean the v3 archive. The same note is in `re/NEXT_SESSION.md` and `CLAUDE.md`.
- Code comments that cite "ROADMAP.md §D3" (`re/tools/ai_ctrl_stats.py:13`, `re/tools/ai_ctrl_window.py:20,24,80`,
  `re/tools/b_envelopes.py:1`) were not edited (text-only step, no code edits); the v4 note names them.
- `DEFERRED.md` rows citing ROADMAP sections (D-11071, D-11075, D-11060) not edited (tracker, re-classify only);
  covered by the same note.

## v4 outline

1. Old-citations note, source decision L-93.
2. Why v4 exists: C4 measured the port; L-93 survey numbers; SimGolf precedent; RE axis vs port axis;
   superseded clauses (L-68, L-73, L-84, L-89, libid port-use); **the project DoD is the port axis**.
3. Baseline 2026-11-07: ladder C4 254 / C3 1,069 / C2 4,580 / C1 66 (+32 untagged, recounted from `hooks.csv`),
   game 3,373 / library 2,596, 1,796 of 2,157 at 100%, 462 libid, expected about 1,875 and about 3,350 C4,
   port column baseline from C4R-PORTCOL, allowlist 1 live line (recounted), gate refs pointer to
   `re/orchestrator/merge_train.json` (G-CTRL `8284be46 / 67931834 / 31fe043e`, B `1d5bf4f9` x3 on 2026-11-07).
4. Default-build rule, kept, scoped to the port axis.
5. Definition of Done: F-DoD / S-DoD / P-DoD for the RE axis (C4 per `re/CONFIDENCE.md`, M5 routes, waivers,
   match ledger) and for the port axis (`port = called`, one body per RVA, evidence covers the exe copy, v3 S-DoD
   criteria 1-6 incl. L-22 criterion 5 scope, allowlist empty, playthrough, WS-I out, CRT/libpng/zlib out of the
   port count), D-Gate. Neither axis reads the other's column.
6. Closed phases D0-D3, a few lines each with evidence paths, detail in the archive.
7. RE axis M1 (C4R-RUBRIC, C4R-PORTCOL, C4R-ROADMAP), M2 (C4R-MATCHBATCH), M3 (S17, S18, S20..S28 match-only;
   S21..S28 titles predate L-93, the row note wins), M4 (C4R-LIBID), M5 (MATCH-S14-FU, MATCH-RWCORE-G5,
   MATCH-PCFUNCS-REGEN, d3dx9 / DirectShow to file, Frida route + HB-RUNDIFF-MULTIOBS, MATCH-9466 blocked),
   final sweep C4R-MATCHBATCH-2.
8. Port axis D4 (open rows table from the ledger: D4-SAVE-U3560, D4-DC-77b0, 8 D4-DC-INC-*, D4-DC-4d20-GRID-FLIP,
   D3-CF-D11079, D4-UNC-PHYS-3, D4-ST5LEG2-MODE7 -> D4-FLIP-ST5-LEG2, D3-CF-MODEXIT / -D11078 / -R7INPUT, the nine
   matched-body swap rows, MATCH-PAUSE-OVERLAY, HB-4, HB-RUNDIFF-MULTIOBS, gate D4-SDONE; "a swap moves the port
   value, not the C-level") and D5 (shipping checklist with state: done items, D5-SHIM / -STEP3-REMOVE, D5-DEPS,
   D5-KNOBS blocked on D4-SDONE).
9. How the work runs: ledger, worker rules, conductor, decisions, gate refs, trackers, v3 process rules,
   workstreams pointer.

## Notes for the conductor

- `d3dx9 / DirectShow` in M5 has no ledger row yet ("to file" in v4).
- MATCH-S21..S28 ledger titles still read "body swap, build, gates"; their notes say match-only (L-93). A
  title refresh with `roadmap.ps1` would remove the ambiguity.
- The v4 numbers marked "expected" are L-93's estimates; C4R-MATCHBATCH and C4R-PORTCOL should overwrite the
  baseline table rows they own.
