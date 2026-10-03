# Next session kickoff

Updated 2026-10-02. **Two user decisions (Mariano) govern this handoff:**

1. **D2 is PARKED, not closed, and stays re-openable.** After re-close attempt 20 (`015537a2`),
   **2 of the 3 metrics pass** inside their unchanged `d81a8df6` bounds — slip 1500-2000
   **0.1943** (0.18855..0.19635) and slip 2000-2600 **0.2524** (0.24488..0.25487). **Launch and
   recovery both PASS** (`L = 0` at 0.19 %; recovery H1 398/400 = 99.5 %, median 1362.7 against
   the original's 398/400 and 1333.9). `driving-median` is **1852.66 / 1861.43 / 1854.65**
   against a lower bound of **1904.70**, about **1.3 % under**, and its carrier is **NOT
   identified**. **The bounds were not moved.** Residuals filed as `DEFERRED.md` **D-11071**:
   U-9180, U-9181 (`+0x4a4` 0.67804 orig vs 692.302 port, now the radius of
   `ContactProducer.cpp:67`'s admission test), `ProduceTerrainBatch` as a **port-only stand-in**
   for the BSP walk `FUN_00538c80`, and the unported outer chunk loop
   `0x00471143..0x00471151`.
2. **D3 is UNBLOCKED.** The modes 3/7 port (`FUN_00414c30` + the world-object query
   `FUN_00484c70`) is the closure path, fixed by the 2026-09-29 user decision.

## D2 RE-PICKUP CONDITION (read before touching physics)

Any finding that touches **`+0x4a4`**, the **contact collector / `FUN_00538c80`**, **grip-clamp
#6** (`0x004687f0..0x0046897b`), the **substep/chunk loop** (`0x00470c70`),
**`ReassertContacts`**, or **player-car speed on Training** re-opens D2. Report it as a
**"D2 REOPEN CANDIDATE"** row in your RESULT with evidence, and do **not** change D2 code for it
in the session that finds it.

> ## START HERE (D3 lane) — close D3
>
> Read `ROADMAP.md` §D3 in full (the gate table, the per-third pass criteria, and the closure-state
> blocks dated 2026-09-27..29) plus `re/analysis/D3_DRIVE_FORCE_2026-09-29.md`,
> `D3_DRIVE_2026-09-28.md` and `PLAYER_REGRESSION_2026-09-29.md`. **Do not re-derive them.**
>
> ### KNOWN STATE ENTERING THE LANE
>
> - **Modes: MET** (all four criteria, 2026-09-26).
> - **Powerups: MET** (criterion (c) met 2026-09-28d; one bounded residue, R_FLAME 2 queries of
>   546 on `g3`).
> - **AI (e) speed: MET 3/3**, all six gated values inside 0.05 % of the reference against a ±2 %
>   band.
> - **AI (b) steering: FAILS on all three cars**, measured under matched speed for the first time:
>   `c1_median` 42-48 against a band of `[0,0]`, `abs_steer_median` 46.5-58 against a ceiling of 23.
>   **The bands are NOT to be moved.**
> - **8 AI rows were demoted C3→C2 on 2026-09-29** by the dual-copy fix
>   (`re/analysis/DUAL_COPY_FIX_2026-09-29.md`) — the exe copies differ from the bodies those C3s
>   were earned on. **A live candidate explanation for (b) that has NOT been tested.**
>
> ### WHAT D2 ATTEMPT 20 MOVED, AND WHY D3 MUST RE-BASELINE FIRST
>
> Attempt 20 changed **every car's** contacts (`ContactProducer.cpp`'s admission test went
> plane-distance → spatial) and the substep loop (**2** substeps/frame, was 3). Both apply to AI
> slots. `ProduceTerrainBatch` is called per car at `VehiclePhysicsRun.cpp:1007`, and **every
> attempt-20 run was `participants=1`**, so the AI-side magnitude is **[UNCERTAIN]**.
> **Re-score (b) and (e) on all three cars on current HEAD, with the bands unchanged, before any
> port leg**, plus the powerups/modes quick checks §D3 names.
>
> AI slots 1+ still carry the **fitted start seed** at `VehiclePhysicsRun.cpp:702`
> (`+0xbf8 = 1`, `+0xbf4 = 1300`). Attempt 15 showed it is derivable from the ported rev-charge
> law (`FUN_0046d7f0` / `FUN_0046d780`, charge 300). Replace the fitted seed with the ported law
> **only** if the AI's own pre-race input drives it, **proven on the running original**, as its own
> pre-registered step with the promotion leg, and re-check (e) after.
>
> ### THE PORT LEG
>
> Transcribe `FUN_00414c30` and `FUN_00484c70` from the disassembly (read-only Ghidra pool slot, or
> `analyzeHeadless` + `DecompPC.java`; `GrepDecompAll.java` and `re/tools/dispsweep.py` exist; watch
> for folded bases, wrong arities, register-arg contracts). **Test the hypothesis on the running
> original first** — which mode the original's AI is in, frame by frame, at the (b) failure points,
> via a Frida **entry** hook. Then port with the full promotion leg: one faithful body per RVA, a TU
> listed in **both** `exe_sources.rsp` and `asi_sources.rsp`, dual-copy guard NEW=0 (check for
> duplicate AI implementations **by hand**), Frida path1 + path2 (or `re/CONFIDENCE.md`'s
> non-repeatable-function clause), and `re-classify` with only what was earned.
>
> ### STANDING RULES FOR THIS LANE
>
> Launch every game muted; set `MASHED_TITLE` on every run; `MASHED_WIN_POS=primary-bl`;
> `--poke-ctrl-slots` on race captures; **never** `MASHED_NAV_DEMO`. Track the PIDs you spawn and
> kill only those. Frida **entry hooks only**. Never write to the master Ghidra project. Never apply
> `unlock_*` to `original/MASHED.exe`. Build via `mashedmod\build.bat` from PowerShell. Mutate
> trackers only through `re-classify`, preserving each file's own line endings (`DEFERRED.md`,
> `UNCERTAINTIES.md`, `hooks.csv`, `STUBS.md` are CRLF; `ROADMAP.md` and this file are LF).
> Pre-register every rule and gate; if one fails, STOP, and state prominently any gate or decision
> rule you replace and why. Run the collateral review (`collateral.py` with a floor; paired pre vs
> post on the same side; cross-side at matched frame/time, never only speed-banded).
>
> ### STILL OPEN
>
> **D3:** AI criterion (b) on all three cars. **D2 (parked, D-11071):** U-9180, U-9181.
> **Elsewhere:** U-9177, U-9176, U-9156, U-9171, §20.14's `-0.1` duty cycle, D1-residue R1, the
> outer chunk loop.
