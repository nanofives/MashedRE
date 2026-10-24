# Roadmap open-steps seed (2026-10-24)

> Source: account2 worker survey via `delegate.ps1 -Repo Mashed` (claude-opus-5-5, $1.16, 0 denials), 2026-10-24.
> Candidate-grade: rows were read from ROADMAP.md, re/NEXT_SESSION.md (first ~400 lines), re/tools/dual_copy_allowlist.txt and re/HARNESS_BACKLOG.md, not re-checked against code. Phase 0 of ROADMAP_LOOP_PLAN.md verifies each row before it enters the ledger. Rows tagged [UNCERTAIN] are unverified by construction.


I've read the five sources. ROADMAP.md lists **27 open steps** by 2026-10-24. Most dates below come from 2026-10-21..24 entries, but three rows (T-ARCTIC, HB-*, WS-H) cite older text, and nothing in these sources says those are still current.

## (1) Open steps

| id | section | description | status + newest evidence | next action | needs | blocked-on | size |
|---|---|---|---|---|---|---|---|
| D1-R1 | D1-residue | Original-side capture at a matched, frame-synced pose. Pass = `drawlist_diff.py` GREEN or every row cited | OPEN. Fixes so far count toward it but don't close it (ROADMAP.md:284-286, 296-306). The A3/O6 matched-pose gap is a concrete instance (:305-306) | Take the frame-synced original capture and diff it | frida-boot, build | none stated. Owed before D5 (:284, :2228) | ? |
| D1-R2 | D1-residue | The verbatim pose from `Race/RaceCamera.cpp` drives the renderer | OPEN. Pose matches the original to float precision (`verify/d1_camera_20260826/RESULT.md`) but is still discarded: `race_cam_.pos()/.target()` have 0 call sites, and the view comes from the rig at `TrackRenderer.cpp:4097-4123` (ROADMAP.md:543-548). [UNCERTAIN] whether those line numbers still hold | Wire the pose into the renderer | build, frida-boot | none. Gates in-race human review (:581-583) | ? |
| D4-AUDIO-RWS | D4 Audio TUs | AudioRws.cpp: 19 functions left (pool/list family on private zero storage, AudioFmtGlobalScan, AudioContextLookup) | Slice 1 merged. `RESULT_rws1.md`, `POOL_CONSTRUCTOR.md` (NEXT_SESSION.md:3-7, 25) | Pool/list family on private zero storage. AudioContextLookup needs 0x005ac930 first | build, frida-boot | none | M (19 fns) |
| D4-AUDIO-TWINS | D4 Audio TUs | Job twins 0x004625b0 (CharacterBankPath) and 0x005aeb90 (VolToDs) | Waiting on Mariano (NEXT_SESSION.md:23-25) | Ask Mariano | build, frida-boot | user decision | S |
| D4-AUDIO-REST | D4 Audio TUs | The rest of the 21 .asi-only Audio files (585 rows were hook-shaped, ROADMAP.md:2894-2895) | 17 TUs / 59 fns linked (NEXT_SESSION.md:3). [UNCERTAIN] which files beyond AudioRws remain .asi-only; the 21-file list is at :42-43 | Diff the list against the rsp files (`verify/audiolink_20261023/SURVEY.md`) | text-only first, then build, frida-boot | none | ? |
| D4-SAVE-B | D4 Save TUs | Option B: make the file wrappers the exe's save path. Needs exe bodies for 0x004cc230 / 0x004cc160 / 0x005507b0 / 0x00550910, plus a ruling on the failed-write crash | Not started (NEXT_SESSION.md:108-109) | Mariano picks | build, frida-boot | user decision | M |
| D4-SAVE-DLG | D4 Save TUs | SettingsAndIO (3 fns) + SettingsDialog (3 fns): link or leave .asi-only | Not started (NEXT_SESSION.md:110-111) | Mariano decides | text-only / build | user decision | S |
| D4-SAVE-KNOB | D4 Save TUs | `MASHED_SAVE_ORIGINAL` default | Load side does not block a flip. Write target + col-5 trophy cell still do (NEXT_SESSION.md:83-86, 113; `RESULT_knob_progress.md`) | Decision | none | user decision | S |
| D4-SAVE-U3560 | D4 Save TUs | Profile/ghost block, remaining parts: header fields, the loader's `(n+2)x0x24` read, the frontend caller of the restore | NARROWED, `RESULT_ghost_roundtrip.md` (NEXT_SESSION.md:141). A natural ghost needs a TimeTrial lap no unattended harness drives (:151-152) | [UNCERTAIN] not named as a next step | frida-boot | harness gap (:151-152) | ? |
| D4-DC-MATH-39b0 | D4 allowlist | 0x004c39b0, LUT vs exact normalize | Allowlisted (dual_copy_allowlist.txt:58). Feeds D2 | knob default OFF + PREREG, flip on decision (NEXT_SESSION.md:228-230) | build, frida-boot | user decision | ? |
| D4-DC-MATH-4680 | D4 allowlist | 0x004c4680 ortho (WarzonePontoon vs BodyOrientationIntegrate) | allowlist.txt:59. D2 cost | same route | build, frida-boot | user decision | ? |
| D4-DC-MATH-4d20 | D4 allowlist | 0x004c4d20 RwMatrixRotate vs RotateAboutAxis | allowlist.txt:27 | same route | build, frida-boot | user decision | ? |
| D4-DC-77b0 | D4 allowlist | 0x004177b0 AiPreTick vs the RuleEngine fragment | allowlist.txt:45, ROADMAP.md:2933-2939 | AiBankSwitch recipe once its inputs have producers | build, frida-boot | missing input producers (0x008a9648 / 0x008a96ec) | M |
| D4-DC-0510 | D4 allowlist | 0x00410510 RuleEngine vs ScoringHooks | Deliberate split (allowlist.txt:28, NEXT_SESSION.md:231-232) | decision | build | user decision | ? |
| D4-DC-82a0 | D4 allowlist | 0x004282a0 MenuMenusB vs MeasureMenuMessage | DELIBERATE (allowlist.txt:38) | decision | — | user decision | S |
| D4-DC-8ad0 | D4 allowlist | 0x00408ad0 RacePct vs RaceScoreFloatGetBySlot | Kept only for the opt-out `MASHED_NO_RACEPCT_SLOT` (allowlist.txt:26, NEXT_SESSION.md:233) | Delete the opt-out or keep it | build | user decision | S |
| D4-UNC-PHYS | D4 dual-copy | U-9783 / U-9784 (standalone inputs), U-9785 / U-9786 | open (ROADMAP.md:2951, 2958) | [UNCERTAIN] not stated | ? | none | ? |
| D4-WSF | D4 / WS-F | Data formats | "No work since 2026-06-16" (ROADMAP.md:2995) | [UNCERTAIN] none defined | ? | none | ? |
| D4-WSJ | D4 / WS-J | Audio remainder | Same status (ROADMAP.md:2999). [UNCERTAIN] overlap with D4-AUDIO-* | — | ? | none | ? |
| D4-WSH | D4 / WS-H | C4 verification lane | "Continuous; audit stale" (ROADMAP.md:2997) | — | frida-boot | none | ? |
| D4-WSB | D4 / WS-B | B5e per-row C4 campaign | Scope [UNCERTAIN]; needs a `hooks.csv` filter (ROADMAP.md:2991) | grep hooks.csv for B5e rows below C4 | text-only, then frida-boot | none | ? |
| D4-SDONE | D4 gate | Every subsystem S-DONE under the clarified S-DoD, and allowlist empty | ROADMAP.md:2963-2964 | — | all | the rows above | L |
| D3-CF-U9779 | D3 carried | Round end at k=220 (sub-state 7, writer 0x0046ea62); port stays in 6 | ROADMAP.md:2212-2213 | — | ghidra-mcp, frida-boot | none | ? |
| D3-CF-U9782 | D3 carried | FUN_004709a0 retry counter vs StepSubstep | :2214-2215 | — | ghidra-mcp, build | none | ? |
| D3-CF-U9780 | D3 carried | Car 2 vx at k=218, 1 ulp | :2216 | re-check under SUBSTEP_OUTER | frida-boot | none | S |
| D3-CF-U9775 | D3 carried | COLL_LUT faithful path drvmed 1890.05 < floor 1893.63 | :2217-2218 | — | build, frida-boot (D2 arms) | none | ? |
| D3-CF-SNAP | D3 carried | Port-only snapshot/restore kept by SUBSTEP_OUTER | :2219 | — | build | none | ? |
| D3-CF-BOX | D3 carried | Box-state producers FUN_004111c0 / 0040e590 / 00424eb0 / 00422fd0 / 0040be50 unported | :2220-2222 | port | ghidra-mcp, build, frida-boot | none | M |
| D3-CF-RFLAME | D3 carried | R_FLAME 2-query residue (FUN_0045ac40 sort unported) | :2223 | port | ghidra-mcp, build | none | S |
| D3-CF-MODEXIT | D3 carried | Mode exits no oracle run reaches (rules 5 / 7 / 8 / 9 / 10) | :2224-2225 | — | frida-boot (scenario) | harness | ? |
| D3-CF-D11078 | D3 carried | Two-car original capture for U-9730 | :2226 | — | frida-boot | none | ? |
| D3-CF-D11079 | D3 carried | Unify the exe bodies of 0x004c4680 / 0x00532b80 (U-9741) | :2226-2227. 0x00532b80 one body since kfdot (NEXT_SESSION.md:220-222); 0x004c4680 = D4-DC-MATH-4680 | — | build | user decision (4680) | ? |
| D5-SHIM | D5 | exe runs without the dev d3d9 shim, d3d9_real, ASI loader or boot patches | ROADMAP.md:2973-2974 | — | build, run | D4 | ? |
| D5-FIRSTRUN | D5 | Defaults written when no videocfg / contcfg exists | :2975 | — | build | — | ? |
| D5-DEPS | D5 | Runtime deps listed and checked on a clean machine | :2976 | — | run | — | ? |
| D5-PDOD | D5 | P-DoD states that WS-I is out of scope (D-11063) | :2977 | — | text-only | — | S |
| D5-KNOBS | D5 | Delete every default-OFF `MASHED_*` gate on finished ports | :2978-2979 | — | build | D4 | L |
| D5-DEFER | D5 | `DEFERRED.md` holds only justified rows. Dev `.asi` leaves the shipping matrix | :2968-2969 | — | text-only, re-classify | — | ? |
| HB-1 | HARNESS | x87 ST0 float-return handler (6 frontier rows) | NEXT PULL since 2026-07-24 (HARNESS_BACKLOG.md:28-35) | author + verify | frida-boot | none | M |
| HB-2 | HARNESS | Callee-gate cascade scan | :36-38 | mechanical scan | text-only | none | S |
| HB-3 | HARNESS | Course-load scenario verifier (60 C2 track rows) | :44-53 | author | frida-boot | none | M |
| HB-4 | HARNESS | Discovery drains: 1,788 RVAs, 11 SCRIBE queued (as of 2026-07-03) | :54-56 | ghidra-sweep | ghidra-mcp | none | L |
| HB-5 | HARNESS | Parity coverage upkeep | :57-59 | — | frida-boot | none | ? |
| HB-6 | HARNESS | veccap: FUN_00566200 re-transcribe (VECCAP-2), capture_vectors scenario, FUN_005667c0 | :82-86 | — | build, frida-boot | HB-1 (005667c0) | M |
| HB-CHUNK | HARNESS | `--on-chunk` PHASE 2 in `c4_navigate_batch.py` | Untested (:92-123) | re-author against main | frida-boot | none | S |
| HB-ARCTIC | HARNESS | geomlight water discriminator (DFF asset name) | Last note 2026-08-31: scope key not precise enough (:220-231). [UNCERTAIN] current merge state | refine, then 13-track check | build, frida-boot | none | M |

Allowlist counts (`dual_copy_allowlist.txt:26-59`): 7 lines, 4 `DUP-IN-TARGET` + 3 `CROSS-TARGET` + 0 `DUP-INSTALL`. The lint counts 8 findings (5 + 3, NEXT_SESSION.md:225). 0x004c39b0 is listed as "exe+asi" (NEXT_SESSION.md:313), which is the likely cause of the 7-vs-8 difference [UNCERTAIN].

A count mismatch to flag: the D4 header (ROADMAP.md:2900) still says 110 entries. That is the 2026-09-29 seed figure.

## (2) Pending user decisions

- **Job twins** 0x004625b0 / 0x005aeb90: NEXT_SESSION.md:23-25.
- **savelink2 decisions:**
  - option B: NEXT_SESSION.md:108-109
  - dialog helpers: :110-111
  - whether 0x00496400 stays C4 although its hook never installs by default: :112
  - knob default: :113
- **Flip any math pair** (0x004c39b0 / 0x004c4680 / 0x004c4d20): NEXT_SESSION.md:228-230, allowlist.txt:58-59.
- **Deliberate splits** 0x004282a0 / 0x00410510, and the 0x00408ad0 opt-out: NEXT_SESSION.md:231-233.
- **Collision-FX residual:** tuning was "left for a decision" (ROADMAP.md:3095).
- **Next D4 lane:** the savelink2 kickoff tells the next session to ask Mariano which of decisions 1-4 to take, or which other D4 item to pick (NEXT_SESSION.md:121-122).
- **Deploy:** redeploy the shared `original/mashed_re_dev.asi` only when Mariano says (NEXT_SESSION.md:273-274).
- **Merges:** every lane merges only when Mariano says (NEXT_SESSION.md:64-65, 211).
- **Already settled:** the `work/audiolink` merge question at :23 was resolved at :3.

## (3) Serialization constraints

- **D2 arms run alone.** NEXT_SESSION.md:206, 264, 328; memory `d2-arms-are-wall-clock-bound`.
- **Gate re-check after any code change.**
  - Rerun G-CTRL + B on the final exe: NEXT_SESSION.md:62-63, 125-126.
  - References: G-CTRL `bacfc747 / 8dd1c004 / 6f9932b6`, B `f9deb9b9 / d3601fcc / 9da9a26a`. Also (b) 9/9, (e) 3/3, D2 3/3, 13 tracks 878 rows 0 RED (:223-224, 288-289).
  - Each consolidation re-earns its C-level through the gates (ROADMAP.md:2959-2961).
- **Anything that changes what the exe computes** gets knob default OFF + PREREG, and flips only on Mariano's decision (NEXT_SESSION.md:256-258, 306-307).
- **PREREG is committed alone, before its arms.** Check `git diff --cached` first (NEXT_SESSION.md:59, 104).
- **Private roots per lane** (`D:/MashedWT/<lane>_root`, own AppCompat layer). Copy the fresh `.asi` in yourself, because `MASHED_NO_DEPLOY` builds don't refresh it (NEXT_SESSION.md:69-70, 103).
- **Sandbox:** every game launch needs the tool sandbox disabled (:68).
- **Shared `original/`:**
  - Kill only your own PIDs (:76).
  - Concurrent piz loads crash the original. Re-run when that happens and don't report it as a finding (Mashed CLAUDE.md, U-9677).
- **Git index:** the main checkout's index is polled every few seconds. Retry `git add/commit/merge` or commit from the worktree (:73-74).
- **Single-writer tools:**
  - `xtwin.py` uses the single-writer `Mashed_Console` headless project (Mashed CLAUDE.md "Cross-build reference").
  - Master Ghidra writes go through ghidra-sweep only.
  - Trackers are written only via re-classify.
- **Capture windows:** the multi-session skill coordinates them. The D1-R1 original capture needs a frame-synced pose.
- **Account2 can't do most of this.** This worker has no Ghidra MCP, no Frida and no builds, so every row tagged ghidra-mcp, frida-boot or build has to run on account3.
