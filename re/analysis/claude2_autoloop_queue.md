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
- [OPEN] **it2 — build.bat LNK4042 dedupe.** The `mashed_re.exe` source list in
  `mashedmod\build.bat` lists ~13 files twice (CarWorldContacts, CarCarContacts,
  ContactProducer, WheelContactSolver, ContactStubs, ForceIntegrator,
  ForceIntegratorStubs, Vec3, RwV3dNormalize, RwV3dTransformPointsCPU, RwMatrixRotate,
  RwMatrixRotateInner, MixedC3Sweep) → LNK4042 warnings. Remove the duplicate lines
  (keep first occurrence), rebuild, confirm `Build OK` + fewer/no LNK4042. Safe hygiene.
- [OPEN] **it3 — D-11057 config-edit env-gated live wiring (Part B advance).** In
  `exe_main` add a LEFT/RIGHT handler for screens 18/24, gated behind `MASHED_CONFIG_EDIT`
  (default OFF), calling `Nav_ConfigEditWrap(ed40[row], dir)`. The `ed40[row]→selector`
  map is a single clearly-marked table (default: all-rows→no-op) for Fable to fill from
  the Ghidra dump (hand-off #3). Gated-off = zero live-behavior risk; build-verify only.
- [OPEN] **it4 — video-playback subsystem scoping doc.** The 4 C2 video rows: from
  existing notes + source, write a scoping doc (deferred-not-needed vs port + rationale)
  to feed the M1 decision. Pure analysis; no wiring.
- [OPEN] **it5 — Fable-facing survey: D-11056 rule-5 collectTotal reader.** From source +
  the d11056 notes, map the candidate call-sites/xrefs that read the rule-5 win-predicate
  total, so Fable's one Ghidra session is scoped. Pure read/draft; reduces Fable tokens.
  (Does NOT resolve the array question — that needs Ghidra — just narrows it.)
- [OPEN] **it6 — extend `build_menustr_test.bat` / add a badges_test gate** to run+gate
  like navsm (build-only today). Low value; do late.

Append new OPEN lanes as they're discovered; never remove the hard-limits block.
