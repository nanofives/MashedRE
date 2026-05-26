# Frontend C4 lift plan (2026-05-26)

**Goal:** raise Frontend subsystem C4 count from 21 → ~131 (S-DoD target ≥50%
of 262 rows). The lift uses main-menu canonical-observation under the
**post-loader-fix** runtime (`dinput8` shim autoloads `mashed_re_dev.asi`,
landed 2026-05-24, `[[project-loader-split-dinput8]]`).

## Audit-window contamination — must re-validate before lift

19 of the current 21 Frontend C4 rows have evidence dated 2026-05-15..05-17,
inside the broken-loader window (`[[project-loader-broken-9d-audit]]`).
Their canonical-observation evidence was collected while the .asi was NOT
loaded — so Frida watched the unhooked original, not our reimpl.

Affected rows (evidence column → date in audit window):

| RVA        | Name                       | Evidence column                                    |
|------------|----------------------------|----------------------------------------------------|
| 0042b930   | MenuAlphaGet               | main_menu_idle_10s_2026-05-17                      |
| 0040bb70   | SpriteLookupTableA         | main_menu_idle_10s_2026-05-15                      |
| 0040bb90   | SpriteLookupTableB         | main_menu_idle_10s_2026-05-15                      |
| 0040b7a0   | HotkeyStringBaseGet        | main_menu_idle_10s_2026-05-15                      |
| 0040b6c0   | FrontendArrayGet           | main_menu_idle_10s_2026-05-15                      |
| 0040ad20   | FrontendGlobalGet          | main_menu_idle_10s_2026-05-15                      |
| 0042ebe0   | FrontendPlayerSlotCheck    | main_menu_navigate_arrows_2026-05-15               |
| 00430a10   | HudSlotTypePlayer0         | main_menu_navigate_all_2026-05-15                  |
| 00430a60   | HudSlotTypePlayer1         | main_menu_navigate_all_2026-05-15                  |
| 00430ab0   | HudSlotTypePlayer2         | main_menu_navigate_all_2026-05-15                  |
| 0042ee00   | SpriteSlotGate             | main_menu_navigate_all_2026-05-15                  |
| 0042b310   | FUN_0042b310               | main_menu_navigate_arrows_2026-05-15               |
| 0042aff0   | FUN_0042aff0               | main_menu_navigate_arrows_2026-05-15               |
| 0042b180   | FUN_0042b180               | main_menu_navigate_arrows_2026-05-15               |
| 0042b770   | FUN_0042b770               | main_menu_navigate_arrows_2026-05-15               |
| 0042e590   | SpriteAnimFrameThunk       | main_menu_navigate_all_2026-05-15                  |
| 004c5c00   | FUN_004c5c00               | boot-to-menu-canonical-wave0                       |
| 0042fe30   | RaceEndFlagIfEndMode       | main_menu_idle_10s_2026-05-17                      |
| 0042fe50   | RaceEndAltFlagIfEndMode    | main_menu_idle_10s_2026-05-17                      |
| 0042fe80   | GetRaceEndFlag             | main_menu_idle_10s_2026-05-15                      |

The two rows NOT clearly in the window (`004cc160` boot-to-menu-canonical-r2,
which is undated in the evidence string) need date-of-evidence inspection
in the CHANGELOG before any judgement.

## Phase A — Re-validate the suspect 19+ C4 rows (do this FIRST)

For each row above:

1. Confirm the install line in `mashedmod/src/mashed_re/Frontend/*.cpp` is
   NOT prefixed with `// MASS-DISABLED`. If it is, that row has already
   been demoted by the mass-disable; skip to demotion step.
2. Build the canonical `.asi` from current `main`.
3. Spawn MASHED via Frida with the dinput8 shim loaded.
4. Confirm `Process.enumerateModules()` contains `mashed_re_dev.asi`
   (must be present — without it the run is invalid like 2026-05-15..05-24).
5. Attach `Interceptor.attach(rva, ...)` to the original RVA. The expected
   behavior on a CORRECTLY-INSTALLED inline-JMP hook is that the
   Interceptor sees ZERO calls (because the inline-JMP redirects past the
   first 5 bytes that Interceptor wants to instrument). Use the
   `re/frida/run_verify_hook.py` path2 harness which already checks for
   this signature.
6. If run_verify_hook.py confirms the JMP is live AND a 10-s/30-s
   canonical observation completes without crash, the row STAYS at C4
   with new evidence string `main_menu_post_loaderfix_<date>`.
7. If the JMP is NOT live (mass-disabled or broken install) OR the
   observation crashes, DEMOTE the row to C3 via `re-classify` (the
   underlying impl is still bit-identical per its prior path1 GREEN diff;
   the C4 evidence is gone but C3 stands).

Estimated work: ~21 rows × ~5 min/row = ~2 hours of attended Frida work
in a single session. Single-session: this is NOT a fanout task — Frida
can only attach to one MASHED process at a time.

## Phase B — New C4 lift (only after Phase A clears)

Once the suspect rows are re-validated, lift new C4 candidates from the
current 72 Frontend C3 rows. The lift target is whatever's MENU-REACHABLE
under boot-to-menu canonical observation.

Reachability heuristic:
- Read the C3 row's `evidence` column for prior canonical observations.
- If the function fires during boot-to-menu (visible in
  `log/observe_c4_sweep_*.txt` or in any `count=N` evidence string with
  N > 0), it is menu-reachable.
- If `count=0` for all observed runs, the function is past-menu and
  must wait for runtime past-menu fix (currently blocked,
  `[[project-runtime-blocked]]`).

Expected lift yield: ~30-40 of the 72 Frontend C3 rows are menu-reachable
based on the audit-window data (extrapolating from the 21 lifted-then-
contaminated rows). After Phase A re-validates ~19 of those and Phase B
adds 10-20 new ones, the Frontend C4 count should land in the 30-45 range
— still well below the 131 S-DoD target.

## The remaining 86-101 C4 promotions

That is past-menu material. It cannot be lifted until either:
- runtime past-menu boots successfully (currently blocked on FUN_004b6940,
  per `[[project-runtime-blocked]]`), OR
- a force-call test harness is built that installs the hook then uses
  Frida.Interceptor to verify the JMP redirect routes through reimpl
  before forcing a call. (Per ROADMAP.md:107 path-to-real-C4-demo #2.)

## How this plan fits into the 4-step roadmap

1. ✅ Step 1 (re-validate suspect frontend C3/C4 rows): triaged 2026-05-26.
   The 9 still-MASS-DISABLED phase-a2 entries are all blocked by harness
   gaps or runtime needs and are documented in DEFERRED/UNCERTAINTIES.
   Phase A above covers the 19 C4 rows whose canonical-observation
   evidence is suspect.
2. ✅ Step 2 (c3 replay batch leftovers): already complete (`menus_lap_time_*`
   landed C3 2026-05-21 via harness-extension).
3. ✅ Step 3 (next batch): `c3_batch_s.txt` generated 2026-05-26
   (6×5 = 30 leaf candidates, Sonnet 4.6, filter v4 yield 88/161 = 54.7%).
4. ⏳ Step 4 (C4 lift): THIS PLAN — Phase A re-validates the 19 suspect
   rows then Phase B lifts ~10-20 new C4 from menu-reachable C3 rows.
   Past-menu C4 is the blocker for the remaining ~90 promotions needed
   to hit S-DoD.

## Pre-flight before running Phase A

- [ ] Confirm `dinput8.dll` shim is deployed in `original/` and autoloads
      `mashed_re_dev.asi` (memory `[[project-loader-split-dinput8]]`).
- [ ] Build current `main` → `mashed_re_dev.asi`.
- [ ] Run `re/frida/poll_attach_catch_crash.py` against MASHED for 60s as
      a sanity check that the dinput8 + .asi + d3d9-shim chain is stable.
- [ ] Confirm at least one previously-known-good C4 hook (e.g. SpriteSlotGate
      0x0042ee00) still passes run_verify_hook.py — this is the canary that
      proves the harness works before sweeping all 19 suspects.
