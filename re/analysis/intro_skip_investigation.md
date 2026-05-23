# Intro-skip investigation (2026-05-23) — DEFERRED

## Goal

Skip the multi-second MASHED intro/splash video sequence (4 stages: Criterion
splash, Empire intro, Supersonic logo, game intro video). Currently MASHED
plays intro for ~8-15s before reaching main menu. Skipping would speed up
canonical-observation runs and manual testing.

## What was tried

**Approach 1 — NOP the call site at 0x00402838 (inside AppInit)**
- 5-byte CALL `e8 13 2b 09 00` (target = FUN_00495350) replaced with NOPs.
- Result: MASHED crashes at t≈3s.
- Cause: skipped HardwareShowIntroVideo entirely; next function in AppInit
  (FUN_00494c80 — DirectShow menu-video setup) depends on state initialized
  by FUN_00495350.

**Approach 2 — RET at FUN_00495350 entry (0x00495350)**
- Replace first byte `0x55` (PUSH EBP) with `0xC3` (RET).
- Result: MASHED crashes at t≈3s.
- Same root cause as #1.

**Approach 3 — .asi hook: setup+cleanup loop without playback**
- Author HardwareShowIntroVideo_Skip in Boot/SkipIntro.cpp.
- Hook calls original FUN_00494a80 + FUN_00493f80 + FUN_00494460 for each of
  4 stages, no playback loop in between.
- Result: MASHED crashes at t≈6s.
- Cause: calling setup+cleanup in rapid succession (no Sleep, no message
  pump) breaks something — possibly DirectShow filter graph or RW
  texture-render state.

## Key findings

- FUN_00495350 calls FUN_00494a80 (per stage video loader) + FUN_00493f80
  (duration) + has a playback loop + FUN_00494460 (cleanup).
- FUN_00494a80 sets DAT_00771a18 (video object) + DAT_00771a04 (1=playing)
  + DAT_00771a08 (param_3 cache).
- FUN_00494460 destroys DAT_00771a18 (FUN_004c7650) + zeros both flags.
- The 3-second-delayed crash on Approach 1/2 suggests state initialized
  during the intro loop (not just setup/cleanup) is required by later boot
  code. CoInitialize is in WindowCreate, so COM is up. The missing state
  is likely RW-render or DirectShow-filter related.

## Files left behind

- `scripts/patch_mashed_skip_intro.py` — Approach 2 patch script. **DOES NOT
  WORK**. Left committed for next-session reference.
- `mashedmod/src/mashed_re/Boot/SkipIntro.cpp.disabled` — Approach 3 hook.
  **DOES NOT WORK**. Renamed to .disabled so it doesn't compile into the
  .asi until investigation continues.

## Next session

1. Identify what state inside FUN_00495350's playback loop is initialized
   that downstream code requires. Likely candidates:
   - RW renderer state set during FUN_004c1bb0/FUN_004c1a00 calls per frame
   - DirectShow filter graph state set during message-pump interactions
   - Frame-counter or timer state
2. Try: hook FUN_00495350 to do FULL setup+cleanup for stage 1 + minimal
   playback (e.g., 10 iterations of the loop instead of waiting for video
   end), then skip stages 2,3,4. See if 1 stage is enough state.
3. If stage 1 short-circuit works: extend to all stages with 10-iter caps.
4. If still crashes: investigate FUN_00494c80 (DirectShow menu-video setup)
   directly to understand what state it expects.

## Workaround for now

- Canonical-observation runs work fine with BOOT_WAIT=8s + idle 30s (per
  R1/R2/R3, Wave 0/2/3 — all reached menu through intro).
- Manual testing: user can press ESC/SPACE/ENTER to skip intro screens.
- The runtime `_intro_skip.py` helper (sends keypresses post-resume) works
  but the user prefers binary patch for cleanliness — that's the open work.
