# RESULT MATCH-S02-RC (redo under L-68), GREEN, tracker text only

Branch `orch/MATCH-S02-RC` off `orch/MATCH-RC-CALLERLESS-FIX` @ d88708571. The old branch (448906ae8) was renamed
`orch/MATCH-S02-RC-old-448906ae8` and its commits are not reused. No game run, no source or build change.

## Verdict

**0 rows go to C4 by `match`.** Ruling L-68 (Mariano 2026-10-31): a match row is C4 only when `mashed_re.exe` calls the
swapped-in body. I recounted the exe callers of all 19 swapped RVAs (script over every TU in `mashedmod/exe_sources.rsp`
plus `.h/.inc`, symbol name or RVA, comments and `RH_ScopedInstall` excluded, `#ifndef MASHED_STANDALONE` blocks checked):

- **18 of 19: exe-callers=0.** Raw-address calls (`reinterpret_cast<...>(0x0040e340u)`) in `Vehicle/VehicleCouplingBridge.cpp`
  (also in `exe_sources.rsp`, .asi-meaningful only), `Vehicle/Integrate2.cpp` (#else branch, .asi), `Race/ScoringHooks.cpp` and
  `Vehicle/PhysicsChainHooks.cpp` (asi-only) target the original image address, not the swapped body, so they are not callers.
  `Gameplay/Thresholds_ah4.cpp` (GetLiveCarCount) is under `#ifndef MASHED_STANDALONE`. Hits for 0x0040b6d0, 0x0040b250 and
  0x00411d60 are comments or a log string.
- **1 of 19 has exe callers: 0x0040e370** (IsCarSlotActive, 4 TUs: D3d9Render/TrackRenderer.cpp, Race/CarRegistrant.cpp,
  Race/HeadingAssist.cpp, Race/PathProgStep.cpp). It stays C3: the exe also holds a second body, the naked `TG_40e370` in
  `Ai/AiTargetClosest_naked.inc` (call site 0x00414ad5), so the swapped body is not the only body. Unblock: replace
  `TG_40e370` with a call to `IsCarSlotActive`, then re-classify route match (one source edit, not a tracker leg).

The brief expected 2 of 19 with a caller (the old RESULT also found 1). I found no second one. If a second row is meant,
the candidate is 0x0040e340, but its only symbol-level caller is the guarded `Thresholds_ah4.cpp`.

## What changed in `hooks.csv` (19 rows)

- `file` and `exe_file` name `Match/MatchS02_<rva>.cpp` (the body moved there when MATCH-S02 merged).
- Level and status unchanged on every row. `notes` gets the L-68 entry with the match.py / relocheck / sha256 / gate evidence.
- 11 C3 rows (0x0040ad20, ad30, b250, b430, b620, b6b0, b7a0, b7b0, b810, d250, 0x00411d60): stay C3, note
  `match evidence: identical, swapped, exe-callers=0; C4 when a caller lands (L-68 ruling)`.
- 3 C4 rows (0x0040b6c0, b6d0, e340): stay C4 on the **frida route only**; no `c4=match` route (no exe caller). Notes say the
  frida evidence measured the previous body.
- 2 C4 rows (0x0040bb70, bb90): same, and the matched body binds callee 0x004c5c00 through a raw-address forwarder (stub).
- 2 C3 rows (0x0040e3a0, 0x0040dfc0): stay C3, raw-address forwarder callees (stubs), exe-callers=0.
- 1 C3 row (0x0040e370): as above.
- `re/analysis/CHANGELOG.md`: 19 lines below `<!-- ENTRIES -->`.

Evidence reused (unchanged, source files untouched by this branch): match.py 100.0% and relocheck OK for all 19
(`verify/matchs02_20261031/results.tsv`, `relocheck.tsv`, `swaps.tsv`), merge-train gates of run 20261031_131735 that merged
MATCH-S02: G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3. Gates not re-run (no code change).
