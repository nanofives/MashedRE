# RESULT dcmix: the mixed dual-copy residue (2026-10-22)

Branch `work/dcmix` from `fcb790811`. Private roots `D:\MashedWT\dcmix_root` (original-side A/B) and `D:\MashedWT\dcmix_root13`
(13-track replay); evidence `D:\MashedWT\dcmix_data` (`MOVED_OFF_GIT.txt`). Final exe `7ea8a2e2` / asi `301ee157`. No knob was added
or changed, so there is no PREREG: every exe change is knob-free and every reference came back unchanged (section 3).

## Verdicts

| entry | static (checked against `MASHED.exe.unpatched`) | what changed | status |
|---|---|---|---|
| 1. `0x0055dec0` | IDENTICAL: `mov eax,[esp+4]; mov eax,[eax]; ret` (`0x0055dec0..0x0055dec6`). 31 call sites; `FUN_0047d3c0` pushes 4 args (`0x0047d5e3..0x0047d5ee`), the leaf reads only the first | one body, `Util/UtilLeaves_ab6.cpp:UtilDeref55dec0`; `Collision/RwpBuildExterns.cpp:FUN_0055dec0` deleted, `PhysicsWorldBuild.cpp` calls the shared one | FOLDED |
| 2. `0x0055e200` | not a pair: `Collision/RwpSolverCore23.cpp:140` is an `extern` declaration. The scanner's first pass took it for a body (section 4) | scanner fixed; the citation in `RwpIntegrator.cpp` corrected (it cited `0x0055e205`, mid-instruction) | FOLDED (false finding) |
| 3. `0x0040e180` | same pair from the same inputs; tie rule `<=`, loop order, out order (INNER, OUTER) equal | one body, `Race/MostSepPair.cpp` (`RaceCamera::MostSeparatedPair`) in both rsp files, the magnitude is the only per-target input (exe `Vec3Magnitude`, asi the original `0x004c3ac0`, as each called before). The .asi export `MostSepPair_0040e180` fills `RaceCamCar[4]` from the original's getters and calls it | FOLDED, in-process A/B clean (section 2) |
| 4. `0x0042d3e0` | not a pair: already one body since dualcopy4 (`Frontend/MenuEntryZero.h:MenuEntryZeroBody`). The finding bound the `// 0x0042d3e0` comment in `MenuNavSM.cpp` to a `static_assert` line. `RecordsZero` calls the one body and then clears `pad20`, `pad30` and entry 30 on purpose (documented there, still open) | scanner fixed, key dropped | FOLDED (false finding) |
| 5. `0x00408ad0` | DIFFERENT. The original is a load, `fld [v*0x30c + 0x8a96ec]` (`0x00408ada`). `TrackRenderer::RacePct` (and its inline copy in `FillRaceCamCars`) computes `fmod(progress, n) / n * 100` on gate count | none | NOT knob-free, reported (decision 1) |
| 6. `0x00410510` | DIFFERENT on one path. Rule 7 (jump table `0x00410844` index 3 -> `0x0041075a`) counts in `edi` only, and its no-match exit (`0x00410790 jmp 0x410622`) leaves `esi`, the score-scan winner, untouched. `RuleEngine.cpp:187` sets `winner = 0` there | none | NOT knob-free, reported (decision 2) |
| 7. `0x0045baa0` | the .asi body is the original (count `[0x5f9bd8]`, base `0x5f9998`, stride `0x40`, key at `+0`, entry or 0, `count <= 0` -> 0). The exe returned an index / -1 over its own table | one body, `Powerup/PowerupTypeSearch.cpp` in both rsp files; the exe `Lookup` maps the entry back to the index (9 distinct codes, so the same entry); the .asi `Search45baa0` passes the original's table | FOLDED, path1 GREEN |

Lint `scripts/lint_rva_bodies.py`: **11 findings, all allowlisted, 0 NEW** (was 18). Seven lines dropped from
`re/tools/dual_copy_allowlist.txt` (`0x0055dec0` x2, `0x0055e200` x2, `0x0042d3e0`, `0x0040e180`, `0x0045baa0`).
`scripts/test_rva_body_scan.py` **8/8** (two new checks: a declaration is not a body, NEG 4 fails on the old scanner; a one-line
body ending in `);` still anchors).

## 2. Evidence on the original (.asi `301ee157`, private root)

**`0x0040e180`, in-process A/B** (`ab_leaf.sh`, `MASHED_LEAF_SELFTEST=1`, `scenario_launch.py --hooks 0x0040e180 --cars 4`). Per call:
the original twice through a trampoline (re-runs `SUB ESP,0x3C; PUSH EBX; PUSH EBP`, resumes at `0x0040e185`), then the shared
body on the getters' values; the game gets the shared body's pair. Logs `D:\MashedWT\dcmix_data\leaf_ab\*.leaf.log`.

| run | calls | noo | ndiff | pairs with distinct indices |
|---|---|---|---|---|
| `t0` track 0, 60 s | 13000 | 0 | 0 | 10405 |
| `wz_full` Warzone, 120 s | 32000 | 0 | 0 | 20921 (every call after ~21000 has equal indices: the one-survivor tail) |

Pairs seen: (0,3) (0,1) (0,2) (1,3) (1,2) (3,3). Not shown by these logs: a call with a car whose `FUN_0046cbb0` dead flag is
set while it is still alive and active (the log does not record the per-car inputs).

**`0x0045baa0`, path1** (`early_window_leaf_diff.py search_45baa0`, worktree .asi, `esi_global_search` seeds the table): **GREEN, 3
cases, 0 mismatches** (`D:\MashedWT\dcmix_data\diff_search_45baa0.csv`). `run_diff.py` refuses this arg_type (no handler in
`diff_template.js`), the early-window harness is where it lives.

`0x0055dec0`: the .asi code is unchanged (`UtilDeref55dec0` was already the installed body); only the exe's caller moved.

## 3. Gates on the exe (recipe F), final exe `7ea8a2e2`, knob-free

| gate | reference (dcleaf) | final |
|---|---|---|
| G-CTRL x2 (`H_DEF-1/-2`) | `c3bfcef3 / 4ed4849b / 4c3548a1` | **equal, x2** |
| B K0 / K1 / K26 | `4985eb3b / b6e3d073 / 6717c15e` | **equal** |
| (b) envelopes `vsum.py` | 9/9 | **9/9** (`vsum_b_def.txt`) |
| (e) `ai_speed_env.py --check` K0 | PASS 3/3, launch 1425.7 / 2052.5 / 2055.0 | **PASS 3/3, same values** (`e_check_def.txt`) |
| D2 `score_d2_v2.py port`, 3 runs alone | PASS 3/3 | **PASS 3/3**: slip 0.1900 / 0.2550 x3, drvmed 1964.25 / 1965.67 / 1964.25, H1 398/400 (`score_d2_def.txt`) |
| 13 tracks `run13.py` + `cites_rres.json` | 39 frames, 878 rows, 0 RED | **score table identical** to dcleaf's, gates PASS (`tracks_def_score.txt`) |

## 4. Scanner fix (`scripts/rva_body_scan.py`)

The first pass meant to reject a declaration (`...);`) but the check was dead whenever a `{` appeared within the next 6 lines
(the inner `if "{" not in blob` made the `;` test a no-op). It now rejects a candidate whose text before the first `{`, with `//`
comments stripped, ends in `;`. An earlier draft tested the tail after the last `)` instead: that dropped real one-line bodies
(`f(x) { return g(x); }`) and kept the `0x0055e200` declaration because of the `)` in its trailing comment. Finding-list diff
old -> new scanner on this tree: exactly `0x0042d3e0` [exe] and `0x0055e200` [exe] + [asi] removed, nothing else.

## 5. Trackers: NOT applied, proposed (re-classify on your go)

| RVA | proposal |
|---|---|
| `0x0045baa0` | C2 -> C3: the demotion reason (two return contracts, .asi-only evidence) is gone; path1 GREEN on the one body. `file = exe_file = Powerup/PowerupTypeSearch.cpp` |
| `0x0040e180` | C2 -> C4: hook live, canonical race x2, clean in-process A/B. Coverage gap: the dying-but-alive input is not shown (section 2). `file = exe_file = Race/MostSepPair.cpp`. hooks.csv:889's "exe uses std::sqrt" note is stale since dualcopy4 |
| `0x0055dec0` | C3 kept; `file = exe_file = Util/UtilLeaves_ab6.cpp` |
| `0x0055e200` | C4 kept; note the signalling-NaN difference (below) |

**`0x0055e200` note:** the original copies through x87 (`fld [esp+8]` / `fstp [eax]`), the port copies the 32 bits. Equal for every
non-NaN single; x87 quiets a signalling NaN. The 4 callers pass `0x358637bd` (`0x00449e28`, `0x00449e7d`) or `FUN_0047e9c0`'s
argument 2 (`0x0047e9c9`). Not changed.

## 6. Decisions for Mariano

1. **`0x00408ad0` (entry 5).** The faithful value is the live slot `0x008a96ec + v*0x30c`, which the exe already writes (the default
   bridge, `TrackRenderer.cpp:8175`) and already reads in three places (`LivePct`, `SpectatorDistances.cpp:72`,
   `AiStandalone.cpp:1693`). Switching `RaceCamCar::race_pct` (camera / `EliminationCheck`) and `ProgBehind` (team-play elimination)
   to it changes default-build values. Options: (a) knob + PREREG lane, flip on the gates; (b) rename `TrackRenderer::RacePct` to a
   port-only helper with no `0x00408ad0` tag and file the stand-in as a U-row, leaving values as they are.
2. **`0x00410510` (entry 6).** Making rule 7 faithful (drop `winner = 0`, `RuleEngine.cpp:187`) changes the exe only on rule 7 at a
   segment end where no car's metric is above 3.0 but the score scan found a winner. Knob + PREREG lane, or leave for the rules lane?
3. Apply the section-5 tracker proposals (`0x0040e180` to C4 with the coverage gap, `0x0045baa0` to C3)?
4. Not merged. The shared `original/mashed_re_dev.asi` is not redeployed.

Left out of this lane as the kickoff said: `0x004282a0`, `0x004177b0` (deliberate splits), `0x004cc050`, `0x004cbd30` (render
streams), `0x004c39b0`, `0x004c4680`, `0x00532b80`, `0x004c4d20` (math pairs with a D2 / draw-file cost).
