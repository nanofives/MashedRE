# claude2 (Accenture) autonomous-loop queue

Canonical state for the self-paced work loop. **Each iteration:**
1. Read this file + `git log --oneline -6` + CHANGELOG head to see what's done.
2. Pick the **top OPEN lane** that is fully doable with **read/write/build/commit only**.
3. Do ONE bounded unit → build → run the relevant regression gate → commit locally.
4. Mark the lane DONE here (with the commit hash) + record any MCP-need to
   `re/analysis/d11057_claude2_handoff_2026-07-03.md`.
5. Schedule the next wake-up (or STOP if no OPEN unblocked lane remains — do NOT
   invent risky work).

**Hard limits (skip a lane if it needs any of these):** Ghidra/Frida MCP · guessing an
RE fact (constant/offset/mapping not in a cited note) · modifying a C3/C4-verified hook
without a `diff-original` · hand-editing trackers (hooks.csv/STUBS/UNCERTAINTIES/DEFERRED/
CHANGELOG — leave to `re-classify` on account3) · writing under `original/` · `git push`.

**Regression gate:** `re/analysis/standalone_menu_sm/harvest/build_navsm_test.bat`
(exit 0 = GREEN) after any `MenuNavSM.{h,cpp}` change; `mashedmod\build.bat` after any
exe/asi source change.

---

## Lanes

- [DONE `bd92c046`] **it1 — menu-SM regression harness + stale screen-8 test fix.**
- [DONE `86aab145`] **it2 — build.bat LNK4042 dedupe.** Removed 12 duplicate exe source
  lines; Build OK; exe-link LNK4042 13→1. (Remaining 1 = the `.asi` MixedC3Sweep
  same-basename collision → new lane it7.)
- [RE-SCOPED → Fable] **it3 — D-11057 config-edit live wiring.** Deferred as a worker lane:
  it would land *inert* code (a no-op until Fable fills the `ed40[row]→selector` table from
  the Ghidra dump — and filling it here = guessing, violates NO-GUESSING). Better applied by
  Fable in one shot alongside the ed40[] harvest (hand-off #3). The confirmed primitive
  (`Nav_ConfigEditWrap`) + tests already landed (commit a8924d1b).
- [DONE `166ad210`] **it4 — video-playback subsystem scoping doc.**
  `re/analysis/video_playback_scoping_2026-07-03.md`: recommend `deferred-not-needed` for
  the standalone S-DoD (MpegVideoTexture already plays frontend.mpg; legacy DirectShow chain
  has no standalone consumer). Verbatim diff + small.mpg trace = MCP-gated → hand-off.
- [DONE `2493db3a`] **it5 — D-11056 rule-5 reader survey + array resolution.**
  `re/analysis/d11056_rule5_reader_survey_2026-07-03.md`: reader chain mapped; synthesized a
  provisional answer (rule-5 total = KTC_NewCopter copter array, not a pickup array); Fable's
  remaining MCP set narrowed to 2 xrefs + 2 decodes (hand-off #5).
- [OPEN] **it8 — Fable-scoping survey: D-11057 config-edit ed40[] + increment handler
  (hand-off #3).** Mirror the it5 rule-5 survey: from `frontend_config_screens_REmap_
  20260614.md` + the `FUN_0043dfd0.c` harvest + the d11057 confirm note, enumerate exactly
  what Fable must pull to fill the s18/s24 `ed40[row]→selector` map + `ea94` range + the
  increment handler (0x00440283..0x00440820), so it activates `Nav_ConfigEditWrap`. Pure
  read/draft; directly shrinks the MCP tail. Higher value than it6 — do first.
- [OPEN] **it6 — extend `build_menustr_test.bat` / add a badges_test gate** to run+gate
  like navsm (build-only today). Low value; do late.
- [OPEN] **it9 — combined standalone unit-test gate.** A `run_all_tests` script that
  builds+runs the existing harnesses (navsm, Race/tests/ruleengine_test, racemodes_test)
  and gates on FAIL — one regression command. Safe infra; medium value.
- [DONE `f16d3d8f`] **it7 — .asi MixedC3Sweep obj collision.** Diagnosed: only Render+Audio
  MixedC3Sweep.cpp were in `asi_sources.rsp`; Render (compiled first) was overwritten by
  Audio before link, so its obj was always dropped (its only hook, RwFreeListCreate, is
  MASS-DISABLED anyway). Removed the redundant Render entry → BYTE-IDENTICAL linked .asi,
  LNK4042 13→0. File kept in tree; needs a unique obj name if re-enabled.

Append new OPEN lanes as they're discovered; never remove the hard-limits block.
