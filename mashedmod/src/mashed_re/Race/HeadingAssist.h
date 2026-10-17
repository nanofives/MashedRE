// Mashed RE - the steer-0 heading assist FUN_0046c7d0 and its track-heading helper FUN_004093b0
// (exe only). See HeadingAssist.cpp.
#pragma once

namespace mashed_re {
namespace Race {

// Default ON (mask 0x1, user decision 2026-10-17, verify/dflip_20261017). MASHED_NO_HEADING_ASSIST=1 opts out;
// MASHED_HEADING_ASSIST=<mask> overrides the mask (0 = off). 0x1: AI slots 1..3 (HeadingAssist_Frame). 0x2: the player
// (HeadingAssist_Player), default ON since u9749 2026-10-17; MASHED_NO_HEADING_ASSIST_PLAYER=1 drops it.
unsigned HeadingAssist_Mask();

// FUN_00424eb0's per-car loop (0x00424eb0, call FUN_0046c7d0(car, dt)): run after the AI tick and
// before the cars' physics. dtUnits = the frame budget in 1/3000 s units (50 at 60 fps), the same
// value the port hands Ai_AdvanceClock.
void HeadingAssist_Frame(int dtUnits);

// [u9749 2026-10-17] Mask 0x2 (default ON since 2026-10-17, user decision; MASHED_NO_HEADING_ASSIST_PLAYER=1 opts out;
// U-9743 / U-9749): the same law for car 0,
// with the player's steer bytes of this frame. Call before the player's physics step.
void HeadingAssist_Player(int dtUnits, unsigned char steer0, unsigned char steer1);

}  // namespace Race
}  // namespace mashed_re
