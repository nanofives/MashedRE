# Orchestrator decision queue

Decisions the roadmap loop cannot make (ROADMAP_LOOP_PLAN.md section 7). Each answered row
re-opens the ledger rows it unblocks. Newest batch at the top.

## Phase 0 (filed 2026-10-24 by the Phase 0 builder, open)

Source: seed verification by the account2 worker (`re/orchestrator/roadmap_verify_20261024.md`).

| id | row | question | options | evidence | recommendation |
|---|---|---|---|---|---|
| P0-1 | D1-R1 | ROADMAP still lists D1-R1 open, but its RESULT says it closes (24 citations, 0 RED) and the code is merged. Is R1 closed? | A: accept `RESULT_d1r1` as closing R1, the loop edits the ROADMAP state lines. B: keep it open and name what is missing | `verify/d1r1_20261016/RESULT_d1r1.md:3-6`; `ROADMAP.md:10,284-286,2228-2229,2971-2972` | A, unless you withheld acceptance on purpose |
| P0-2 | D1-R2 | W0-14 says "wire it behind a default-OFF knob". R2 is already wired and default ON (opt-out `MASHED_NO_RACE_CAM_BASIS`), and its RESULT says it closes | A: keep default ON, then your in-race stills review. B: take W0-14 literally and move it behind a default-OFF knob first | `verify/d1r2_20261016/RESULT_d1r2.md:3-4`; `TrackRenderer.cpp:8868-8869, 8999-9000`; `ROADMAP.md:543-548` | A (B re-opens a change that already passed) |
| P0-3 | solo lock | Solo windows are honoured only by code on `orch/integration`. Copies of `mashed_lock.py` on `race/first-frame-parity` ignore them, and the gate runners (`run_arms.py`, `run13.py`) take no lock, so loop workers wrap them in `mashed_lock.py run [--solo]` | A: accept. Solo protects loop work, and the conductor starts a solo step only with no other worker live. B: carry the lock change to `race/first-frame-parity` now (moves that branch) | `re/orchestrator/mashed_lock.py` header; account2 survey `re/orchestrator/MERGE_TRAIN.md` section 6 | A until you next move `race/first-frame-parity` |
| P0-4 | merge train full set | Which cites file and reference table does the 13-track gate compare against? Phase 0 picked the newest lane that ran it | A: `verify/rskyverify_20261016/cites_merged.json` + `verify/rpct_20261022/tracks_flip_score.txt` (configured now). B: name another pair | `re/orchestrator/merge_train.json` `full`; `verify/rpct_20261022/RESULT_rpct.md:65` | A, confirm before the first Vehicle/Ai/Collision merge |

## Wave 0 (asked 2026-10-24, Mariano)

Briefing: account2 worker extraction 2026-10-24 (sources cited per row). Behaviour changes
still ship as a default-OFF knob + committed PREREG; the flip comes back here.

| id | decision | answer | unblocks |
|---|---|---|---|
| W0-1 | Job twin 0x004625b0 CharacterBankPath (`verify/audiolink_20261023/SURVEY.md:364-373`) | **Shared core, thin wrapper.** Output uppercase + backslashes, `original/` prefix moves to the caller `RaceSession.cpp:192`; default-OFF knob + PREREG first | linking `AudioCharacterBankPaths.cpp` |
| W0-2 | Job twin 0x005aeb90 VolToDs (`SURVEY.md:585-599`) | **Redirect behind a knob.** File split first, then VolToDs -> port behind a default-OFF knob + PREREG; flip decided after measurement | moving `AudioLinearToLog` out of the .asi-only file |
| W0-3 | savelink2 option B, file wrappers as the exe save path (`re/NEXT_SESSION.md:97-109`) | **Yes, with a declared NULL guard** on the failed-write path (`RwStreamClose` has none in the original). Needs exe bodies for 0x004cc230 / 0x004cc160 / 0x005507b0 / 0x00550910 | exe saving through the shared wrappers |
| W0-4 | SettingsAndIO + SettingsDialog (`re/NEXT_SESSION.md:110-111`) | **Leave .asi-only.** Dialog 0x65 unreachable in the patched reference (skip_selector), exe has no resources | Save TU link closes at the dialog boundary |
| W0-5 | 0x00496400 keeps C4 although the HookSystem guard refuses its hook in every default run (`re/NEXT_SESSION.md:93-95, 112`) | **Keep C4, note the guard** in hooks.csv. Basis: the guard-off PREREG run had the hook installed, 22/0/0, guard-on control 4 ndiff at disable_log's RET | tracker note via re-classify |
| W0-6 | `MASHED_SAVE_ORIGINAL` default (`re/NEXT_SESSION.md:83-86, 113, 180`) | **Keep OFF.** A default-ON run would overwrite the diffing reference's save | none (closes the question) |
| W0-7 | Math pair 0x004c39b0 LutNormalize vs exact-sqrt RwV3dNormalize (`re/tools/dual_copy_allowlist.txt:58`) | **Yes: knob default OFF + PREREG + D2 re-measure**, then the flip comes back here | allowlist entry, D2 numbers |
| W0-8 | Math pair 0x004c4680 OrthoNormalize vs MatrixOrthoNormalize (`dual_copy_allowlist.txt:59`) | **Yes: knob default OFF + PREREG + D2 re-measure**, then the flip comes back here | allowlist entry, D3-CF-D11079 (U-9741) |
| W0-9 | Math pair 0x004c4d20 RwMatrixRotate vs RotateAboutAxis (`dual_copy_allowlist.txt:27`) | **Yes: knob default OFF + PREREG**; the step first establishes whether the cost lands on D2 or on a draw file, re-measures that, then the flip comes back here | last math-pair allowlist entry |
| W0-10 | Deliberate split 0x004282a0 MeasureMenuMessage (`dual_copy_allowlist.txt:38`) | **Consolidate** (P4: shared core TU, target thunks only) | allowlist entry |
| W0-11 | Deliberate split 0x00410510 RuleEngine vs ScoringHooks (`dual_copy_allowlist.txt:28`, `re/NEXT_SESSION.md:232, 315-316`) | **Consolidate** (P4; input plumbing + end-block side effects behind thunks); full gate set, default race path | allowlist entry |
| W0-12 | 0x00408ad0 `MASHED_NO_RACEPCT_SLOT` opt-out (`dual_copy_allowlist.txt:26`, `re/NEXT_SESSION.md:233, 285-290`) | **Delete the opt-out.** Old trio c3bfcef3 / 4ed4849b / 4c3548a1 stays in git history only | allowlist entry, one D5 knob deletion |
| W0-13 | Collision-FX skid residual, AI slots 2/3 5139 / 5489 vs 241 / 378 (`verify/fe_residuals_20261016/fx/RESULT_FX.md:27-29`, `ROADMAP.md:3095`) | **Derive the gates from the original** (reverse the original's skid emission gates and port them), not a scaffold re-tune | new step D4-FX-GATES |
| W0-14 | D1-R2 verbatim race-camera pose (`ROADMAP.md:543-548, 581-583`) | **Wire it behind a default-OFF knob, then an in-race review with Mariano** (side-by-side stills vs the original); the flip is his | D1-residue closure |
| W0-15 | Redeploy of the shared `original/mashed_re_dev.asi` (`re/NEXT_SESSION.md:273-274, 336-337`) | **Keep the rule: only when Mariano says.** The loop files a redeploy request here | standing rule |
| W0-16 | Push `orch/integration` to GitHub | **Local only.** Mariano pushes when he moves `race/first-frame-parity` | standing rule |

All 13 parked roadmap decisions plus two standing rules are answered. Flips produced by
W0-1, W0-2, W0-7, W0-8, W0-9, W0-14 come back here as new rows with their evidence.
