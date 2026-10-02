// D2SinkProbe.h — DIAGNOSTIC ONLY, default-OFF.
//
// [D2 attempt 19 / U-9178] The per-frame interval `[the +0x9e4 store 0x004686cc, the render
// tick]` is `T_post` in a18_budget.py's decomposition, and on the port ~90 % of it has no
// identified producer. This probe samples |velocity| at EVERY producer's own phase inside
// that interval, per substep, so the interval's deltas telescope to T_post exactly.
//
// Armed by `MASHED_D2SINK=<path>`. Unset -> every call site is one `if (g_on)` test and
// nothing is written. Slot 0 only. It writes a file and nothing else: no record field is
// touched, so it cannot move any ported behaviour. Pre-registration:
// verify/d2_sink_20261002/PREREG_STEP1.md.
#pragma once

namespace mashed_re {
namespace D2Sink {

// true only when MASHED_D2SINK is set (resolved once, on first use).
bool Armed();

// Frame ordinal, bumped once per slot-0 frame. Called at the top of EVERY slot's frame:
// slot 0 arms the channel and bumps the ordinal, any other slot disarms it, so the
// downstream Mark() sites (which do not know the slot — ContactFixup's do not receive it)
// stay slot-0-only without threading `slot` through the call chain.
void Begin(int slot);

// `sub` is the substep index, or -1 outside the substep loop. `chunk`/`rem` are the substep
// ms chunk and the remaining budget, 0 outside the loop.
void Mark(const char* tag, const void* rec, int sub, int pass, float chunk, float rem);

// [STEP 1B] same line plus a wheel index and four site-local floats, so the three
// velocity write sites inside 0x0046f6c0 are separable by site AND by wheel.
void MarkWheel(const char* tag, const void* rec, int wheel,
               float a, float b, float c, float d);

// Called from the substep loop so Mark() can tag lines without threading the index through
// ContactFixup's call chain.
void SetSubstep(int sub, int pass, float chunk, float rem);
void ClearSubstep();
void NoteFixup();

}  // namespace D2Sink
}  // namespace mashed_re
