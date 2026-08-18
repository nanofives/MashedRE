I have both files in full and the CHANGELOG entries from 2026-08-16 back through 2026-07-31 (31 entries ÔÇö every entry post-dating the master plan's 2026-07-31 rebuild). I also directly counted DEFERRED.md to settle one number. Here is the reconciliation.

---

## Summary verdict

The master plan (`RE_MASTER_PLAN_2026-07.md`) is anchored to a **superseded phase model**. ROADMAP.md is now **v3 (2026-08-15, phases D0ÔÇôD5)**, which archived v2 and explicitly repudiated the "R0ÔÇôR6 closed / active R7" framing the plan is built on. D0 ("tell the truth again") ran and closed on 2026-08-15, invalidating most of ┬º1's numbers and the entire ┬º7 queue ordering. The plan's *technical content* about the two long poles (physics statediff wedge, librw render) largely survives; its *phase scaffolding and priority order* do not.

---

## (1) Defect list ÔÇö master-plan claims contradicted by ROADMAP v3 / CHANGELOG

| # | Master-plan line | Claim | Contradicting citation | Correction |
|---|---|---|---|---|
| D1 | line 3 | "Companion to `ROADMAP.md` (v2, phases R0ÔÇôR8)." | ROADMAP.md:1-5 ÔÇö "Roadmap ÔÇö v3 (2026-08-15). Supersedes v2 (2026-06-09), archived verbatim". v2 no longer governs. | Companion to ROADMAP **v3 (2026-08-15)**; phase vocabulary is **D0ÔÇôD5**, not R0ÔÇôR8. |
| D2 | line 10 | "Active phase: **R7 scaffoldÔåÆverbatim conversion** (R0ÔÇôR6 closed)." | ROADMAP.md:101-104 ÔÇö "v2 claimed 'R0ÔÇôR6 closed'. Its own phase text contradicts that: R4 and R5 both read 'OPENED' ÔÇª v3 records them as open." No R7 exists in v3. | R4/R5 are **open** (mapped to D1/D2). There is no R7. Active work is **D1 (default renderer) and D2 (default physics)**; D0 done 2026-08-15. |
| D3 | line 10 (blockquote) | Phase model is R-phase based. | ROADMAP.md:139-304 defines phases D0ÔåÆD5; CHANGELOG 2026-08-15 "ROADMAP v3 ÔÇª PHASE MODEL REPLACED". | Replace the whole R-phase premise with the v3 default-build rule (ROADMAP.md:46). |
| D4 | line 16 (headline metric) | Uses all-rows **"C3+ = 1,066, 18.1%"** as the headline position. | Numbers themselves match ROADMAP.md:87. But ROADMAP.md:75-81 ÔÇö "Report the first-party number. The headline 18.1% is diluted by 2,215 vendored-library rows." | Not numerically wrong, but **deprecated framing**. Lead with first-party **3,682 rows, 28.8% C3+, 4.9% C4** (ROADMAP.md:77), noting ~1,788 race-closure RVAs undiscovered (ROADMAP.md:83-85). |
| D5 | line 18 | "UNCERTAINTIES 3,003 U-rows." | CHANGELOG 2026-08-15 "113 rows re UNCERTAINTIES Blocks" ÔÇö "**2999 open rows**, 640 asserting a blocker, 70 [UNPROVEN]". | 2,999 open (as of D0.3 final). Minor staleness (3,003 ÔåÆ 2,999). |
| D6 | line 20-36 (┬º1 whole) | "refreshed 2026-07-31" snapshot. | The entire D0 audit (CHANGELOG 2026-08-15 D0.1ÔÇôD0.8) post-dates and recounts every ┬º1 figure. | ┬º1 is one full D0 cycle stale; re-derive from the 2026-08-15 recounts. |
| D7 | line 22 | M1 "**playable whole game** ÔÇö CLOSED 2026-07-13." | ROADMAP.md:15-17,36-40 ÔÇö "`mashed_re.exe`, run with no env vars, does not use most of what has been ported ÔÇª the default build had been quietly exempted from the project's own definition of done"; CHANGELOG 2026-08-15 D0.7 ÔÇö "THE STANDALONE HAS NO SAVE SUBSYSTEM AT ALL." | Closure record stands, but "playable whole game" no longer means the *shipping default* is playable-faithful. See milestone verdict below. |

**Checked and NOT defects (reported so you know they were verified, not skipped):**
- **DEFERRED count** ÔÇö master plan line 17 "672 D-rows (open+struck)" is **corroborated**: a direct count of `D-\d{4}` rows in `DEFERRED.md` returns exactly **672**. It is ROADMAP.md:97 ("43 live rows / 128 struck", i.e. 171) that uses a different, smaller basis. This is a v3-vs-file divergence, not a master-plan error. [UNCERTAIN which basis v3 intends ÔÇö the two docs are not measuring the same thing.]
- **STUBS** ÔÇö master plan line 17 "1,107 S-rows" happens to **equal** the corrected open-count from CHANGELOG 2026-08-15 D0.4 ("1,107 open / 149 struck"). ROADMAP.md:96's "1,072 live" is the figure that was wrong (v3 quoted a stale number; D0.4 fixed it). Master plan is right here.
- **hooks.csv ladder** (line 16: C1 795 ┬À C2 4,005 ┬À C3 881 ┬À C4 185) ÔÇö identical to ROADMAP.md:87. Consistent.
- **B5e merge** (line 26: `021a9f38`, 2026-07-20, K1..K24) ÔÇö matches ROADMAP.md:314. Consistent.
- **Statediff residual / Ring5ab980 / U-6701** (lines 28-32) ÔÇö matches ROADMAP.md:267-269 and CHANGELOG 2026-07-31 (0x005ab980). Consistent.

---

## (2) ┬º7 next-sessions queue verdicts (lines 149-167)

| Item | Line | Verdict | Evidence |
|---|---|---|---|
| 1. librw sizing session | 152-154 | **OVERTAKEN-BY-EVENTS** | Sizing done (`LIBRW_SIZING_2026-08.md`, cited CHANGELOG 2026-08-15 R10b/ROADMAP v3 entries); R10b root-caused 2026-08-01 and closed 2026-08-15; D1 inversion already **measured** and blocked on divergence (ROADMAP.md:218-229; CHANGELOG 2026-08-15 "D1 renderer inversion"). The lane is well past sizing. |
| 2. `entity_field_set` sentinel fix (D6) | 155-156 | **NEEDS-REPHRASING** | Still a valid cleanup, but v3 pushes all promotion/breadth work behind the render+physics defaults into **D4** (ROADMAP.md:292-298); the trajectory-correction (ROADMAP.md:332) flags breadth as over-invested vs verification. Demote to D4-era. [UNCERTAIN whether already implemented ÔÇö no CHANGELOG entry in the read window mentions it.] |
| 3. HUD sweep D-6160..D-6173 | 157-158 | **SURVIVES** (re-homed) | No contradiction. Belongs under D3 (frontend/modes) or D4 (breadth). [UNCERTAIN if done.] |
| 4. Plating drains (D-7000..3, D-0281, D-0245..63/D-9280, D-8140) | 159-160 | **SURVIVES** (lower priority) | No harness, still valid cheap-model volume; but ROADMAP.md:332 explicitly warns breadth outran verification, so this drops below D1/D2/D3. |
| 5. candidate_buckets.json validity + 0x0041c090 C1-vs-C2 conflict | 161 | **SURVIVES** | No contradiction. [UNCERTAIN if resolved ÔÇö not in read window.] |
| Hands-on session (D7): D-11060 + D-11061 + G3 | 163-164 | **SURVIVES** | Still live milestone deferrals (ROADMAP.md:97-98; DEFERRED.md D-11060/61 rows). Prep checklist unchanged. Note D7 said "runs SOON ÔÇö next session the user is present"; it has **not** run. |
| Defect-session lane: statediff wedge, KV C4, WS-A8 | 166-167 | **SURVIVES** | ROADMAP.md:267-269 (statediff wedge D2), :313-314 (WS-A A8 blocked, WS-B C4-verify open). |
| Coordinate-first: VECCAP-2 `FUN_00566200`; U-8991 close | 167 | **SURVIVES / [UNCERTAIN]** | Neither appears in the read CHANGELOG window; status change cannot be confirmed. Missing evidence: any CHANGELOG entry touching 0x00566200 or U-8991 after 2026-07-31. |

---

## (3) Milestone verdicts M1ÔÇôM4 (line 22, lines 59-87)

- **M1 "playable whole game" ÔÇö CLOSED 2026-07-13 (line 22): PARTIALLY OVERTAKEN.** The *closure record* is not reversed, but v3's central finding guts the label: the default `mashed_re.exe` runs the hand-written D3D9 renderer and kinematic scaffold, not the ported librw/physics (ROADMAP.md:20-40), and Save/Audio TUs are not even linked into the exe (CHANGELOG 2026-08-15 D0.7 ÔÇö "THE STANDALONE HAS NO SAVE SUBSYSTEM AT ALL"). "Playable whole game" describes the `.asi`-verified understanding, not the shipping build.

- **M2 "Faithful race" ÔÇö IN PROGRESS (lines 61-73): HOLDS**, now split across v3's **D1 (render) + D2 (physics)**. Sub-item 1 (statediff residual, line 63) SURVIVES ÔÇö ROADMAP.md:267-269. Sub-item 2 (B5d/B5e C4 bit-diff, line 64) SURVIVES ÔÇö ROADMAP.md:314. Sub-item 4 (WS-A8, line 69) SURVIVES ÔÇö ROADMAP.md:313. Sub-item 3 (VECCAP-2, line 66) and sub-item 5 (M2 backlog re-inventory, line 71) remain **[UNCERTAIN]** ÔÇö no confirming/contradicting CHANGELOG entry in the window.

- **M3 "Shipping render via librw ÔÇö D2 DECIDED 2026-07-31" (lines 75-82): GOAL HOLDS, STATUS OVERTAKEN.** The decision (librw is the shipping renderer) is confirmed (ROADMAP.md:317). But "First step is a sizing session ÔÇª the lane is new and unsized" (line 81-82) is false now: sizing done, R10b closed 2026-08-15 (CHANGELOG "R10b residual ÔÇª CLOSED"), and D1 is at the **measured-but-inversion-blocked** stage (ROADMAP.md:218-229; CHANGELOG 2026-08-16 shows the divergence is real, D3D9-side, in-race, faithfulness-vs-original still unadjudicated under U-9039). librw is "the shipping renderer" but **not yet the default** (ROADMAP.md:317).

- **M4 "Ship ÔÇö HUMAN-GATED tail" (lines 84-87): DEFERRAL ITEMS HOLD, FRAMING OVERTAKEN.** D-11060/61/62 remain live (ROADMAP.md:97-98; DEFERRED.md rows). But "None of this is autonomous work ÔÇö schedule it" understates the gap: v3 gates ship (**D5**, ROADMAP.md:300-304) behind D1ÔÇôD4 (default render, default physics, default AI/powerups/modes, then breadth) plus the D0.7 linkage gap. Ship is not merely human-gated; it is D1ÔÇôD4-gated.

---

## (4) Proposed replacement ÔÇö header block (lines 1-15) and ┬º7

Paste-ready, in the file's own style. **Not written to disk.**

### Replacement for lines 1-15

```
# Mashed RE ÔÇö Master Execution Plan (RECONCILED 2026-08-18)

Companion to `ROADMAP.md` (**v3, 2026-08-15, phases D0ÔÇôD5**). ROADMAP defines the *gates*; this
plan defines the *route*. **This is a reconciliation** of the 2026-07-31 rebuild against ROADMAP v3
(prior text in git history ÔÇö `git log -p re/analysis/RE_MASTER_PLAN_2026-07.md`). The 2026-07-31
doc had drifted onto a superseded phase model: it called itself a companion to ROADMAP v2's R0ÔÇôR8
and claimed "Active phase R7 (R0ÔÇôR6 closed)" ÔÇö both repudiated by ROADMAP.md:1-5 (v2 superseded and
archived) and ROADMAP.md:101-104 (v2's own text marks R4/R5 OPEN; v3 records them open; there is no
R7). Per-item history lives in `re/analysis/CHANGELOG.md`; this doc is the strategic index only.

> Phase model: **v3 D0ÔÇôD5**. D0 "tell the truth again" DONE 2026-08-15 (CHANGELOG D0.1ÔÇôD0.8); the
> active long poles are **D1 default renderer** and **D2 default physics**, independent of each
> other. The old R-phases and the M1ÔÇôM4 milestone names in ┬º3 are superseded ÔÇö read them mapped
> onto D-phases (M2 ÔåÆ D1+D2, M3 ÔåÆ D1, M4 ÔåÆ D5).
> Governing rule (ROADMAP.md:46, + the D0.7 linkage clause): **a capability counts only if it runs
> in `mashed_re.exe` with no `MASHED_*` var set AND its TU is linked into the exe.** An env-gated
> opt-in path is a fallback, i.e. not landed.
> Maintenance rule: refresh ┬º1 + ┬º3 after each merged lane; a claim contradicted by ROADMAP or
> CHANGELOG is a bug in THIS file ÔÇö fix it the day it is noticed.
```

### Replacement for ┬º7

```
## 7. Next-sessions queue (reconciled 2026-08-18 against ROADMAP v3; every item leads with a worker leg)

D0 is CLOSED (2026-08-15, all 8 items ÔÇö CHANGELOG D0.1ÔÇôD0.8). The two long poles, **D1 (default
renderer)** and **D2 (default physics)**, are independent; either can advance. Priority order:

**D1 ÔÇö default renderer (invert `MASHED_RENDER_LIBRW`). BLOCKED, not ready to invert:**
1. **Adjudicate librw-vs-D3D9 faithfulness against the ORIGINAL.** The A/B compared the two
   renderers to *each other*, never to the original (CHANGELOG 2026-08-15 "D1 renderer inversion").
   The original-side capture is now possible (`MASHED_CAM_POSE` + the shim's `draw3d.json`); the
   pose reader's +0x4c bug is fixed (it is a direction delta, Xbox twin 0x00446520, CHANGELOG
   2026-08-16) and same-track capture is wired (`Campaign_SetSelectedTrackDev`; on Training,
   D3D9-vs-librw = 1.13%, original-vs-standalone = 90.15% from a higher/further eye, not a frame
   mismatch). *Worker:* collate the campose/track-watcher evidence. *Account3:* the paired original
   capture + adjudication.
2. **Bisect the in-race divergence.** Measured (not filename-inferred): `01_inrace_track` 71.61%,
   `01_action` 21.69%, `round2/3_result` ~69% are all InRace (CHANGELOG 2026-08-16 "CAPMODE moved to
   the capture SINK"). It is D3D9-side, starts at the round-1/round-2 boundary, and is a
   render-state/draw-order problem (the world IS submitted at 13 batches on the orange frames);
   librw is the stable path. Clean bisection point = the round-1/round-2 boundary.

**D2 ÔÇö default physics (invert `MASHED_REAL_PHYSICS`). Defect-session lane ÔÇö coordinate first:**
3. **Statediff residual wedge** (~1/6 boots, second unbisected mechanism; the first, Ring5ab980
   implicit-EAX, fixed 2026-07-31 / U-6701). Caps physics C4 at ~5/6 and blocks A8 and the B5e
   C4-verify campaign (ROADMAP.md:267-269).
4. **WS-A8 velocity/position diff vs original telemetry** on matched inputs, once the wedge falls.

**D3 ÔÇö default AI / powerups / modes (scaffolds the default build still runs):**
5. **WS-C AI** (FUN_00418860 family). Note U-9040 (CHANGELOG 2026-08-16): both the verbatim-band
   and the pure-pursuit steer conventions are DEAD on the default path ÔÇö AI is steered by
   TrackRenderer's own world-space controller ÔÇö so the steer-sign contradiction cannot be resolved
   until the ControlStep path executes.
6. **WS-D powerups** (FUN_0045bba0 dispatcher + 9-entry table; gated on a Ghidra fn-split of
   0x453f60ÔÇô0x45be81) and **WS-G per-mode rules.** *Worker:* map each RVA to standalone status first.

**D4 ÔÇö breadth (ONLY after D1ÔÇôD3):** HUD sweep D-6160..D-6173; plating drains D-7000..3 / D-0281 /
D-0245..63 / D-9280 / D-8140 (cheap-model, no harness); `candidate_buckets.json` validity +
0x0041c090 C1-vs-C2 conflict ÔåÆ `re-classify`; WS-F formats; WS-J audio remainder. **Linkage gap
first:** `Save/` and `Audio/` TUs are absent from `mashed_re.exe` (0 of 17 save, 4 of 25 audio ÔÇö
CHANGELOG 2026-08-15 D0.7); no env var reaches them, so C4-verified save/audio hooks contribute
nothing to the deliverable. Resolve D0 item 7 (asi_sources.rsp vs the exe list) before counting
these subsystems toward P-DoD.

**Next hands-on session (human tail, D5):** D-11060 playthrough + D-11061 recording + G3 cup
place-names Frida session (prep: BOOT_PATCHES boot, d3d9 shim, unlocked desktop, no intro-minimize,
kill-by-PID). Still deferred ÔÇö not yet run.

**Coordinate-first (do NOT pick up cold):** VECCAP-2 `FUN_00566200` [UNCERTAIN ÔÇö status not confirmed
in the reconciliation read window]; U-8991 close via `re-classify`.
```

---

**NO-GUESSING notes / gaps:**
- VECCAP-2 `FUN_00566200`, U-8991, and ┬º7 items 2ÔÇô5 completion status: **[UNCERTAIN]** ÔÇö no CHANGELOG entry between 2026-07-31 and 2026-08-16 (the read window) touches them. Missing evidence: any entry citing those IDs/RVA after 2026-07-31.
- DEFERRED row count: master plan's 672 is corroborated by direct count; ROADMAP.md:97's "43 live / 128 struck" (171) uses a different basis I cannot reconcile from the two docs alone. **[UNCERTAIN]** which basis ROADMAP v3 intends.
- I read CHANGELOG entries back to 2026-07-31 (31 entries). If a specific claim needs an entry older than that, it was outside the requested window.
