// Mashed RE — F4 lap-line crossing sequence (WS-F).
//
// Algorithm-faithful port of the original's lap-line crossing rule
// (FUN_00408610, lap-line list FUN_00426cf0 -> DAT_0066d6e4[6]; full subsystem
// map in re/analysis/lap_progress_subsystem_2026-06-16.md). The original flags
// a LAPDATA Lap_Line gate only on a forward one-gate advance onto it
// (cur==prev+1 && cur==lapLine) and treats the declared lap-line set as a
// sequence that must all be crossed for a lap — the "why two lap lines"
// anti-shortcut. It is NOT bit-identical: the original derives its gate index
// from a spline racing-line projection the standalone doesn't port; the
// standalone advances gates forward by proximity, so every "reach" IS that
// forward advance and this header only needs the gate just reached.
//
// Kept in a header (not a TrackRenderer method) so it is exercised verbatim by
// both the renderer and Track/tests/lap_test.cpp — no copy to diverge.
#pragma once

#include <cstdint>
#include <vector>

namespace mashed_re {
namespace Track {

// Call when a car reaches gate `gateReached` (its forward target, by proximity).
// `lapLines` = LAPDATA Lap_Line gate indices, front() = the primary start/finish.
// `lapMask` is the per-car set of lap lines crossed since the last lap (updated
// in place). Returns true iff this reach completes a lap (the primary line is
// reached with every declared lap line already crossed). With no LAPDATA, falls
// back to the plain start/finish gate 0.
inline bool LapLineStep(int gateReached, const std::vector<int>& lapLines,
                        std::uint32_t& lapMask) {
    if (lapLines.empty()) return gateReached == 0;
    const int primary = lapLines.front();
    const std::size_t nbits = lapLines.size() < 32 ? lapLines.size() : 32;
    for (std::size_t k = 0; k < nbits; ++k)
        if (gateReached == lapLines[k]) lapMask |= (1u << k);
    if (gateReached != primary) return false;
    const std::uint32_t need =
        (lapLines.size() >= 32) ? 0xffffffffu : ((1u << lapLines.size()) - 1u);
    const bool lap = (lapMask & need) == need;
    lapMask = 1u;   // primary just crossed; reset the set for the next lap
    return lap;
}

}  // namespace Track
}  // namespace mashed_re
