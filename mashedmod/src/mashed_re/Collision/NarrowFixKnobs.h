// [D4-WSB-NARROW-FIX 2026-10-27] Default-OFF knobs for the NARROW band's output diffs
// (verify/wsbnarrow_20261027/PREREG_D4-WSB-NARROW-FIX.md). Not seeded by Core/FaithDefaults.cpp.
//   MASHED_NARROW_DOT_ORDER=1  3-term sums in the original's FADD association (the original adds the first two
//                              products it loads), and float-precision arithmetic in the SSE TUs where the port
//                              used float10 (= double); the race loop runs at x87 PC_24.
//   MASHED_NARROW_PAIR_INT=1   0x0057644d: pair record +0x0c = INTEGER [+0x44] + param_6 (MOV ECX,[EAX+0x44]).
#pragma once
#include <cstdlib>

namespace mashed_re {
namespace Collision {

inline bool NarrowDotOrder() {
    static const bool s = [] { const char* e = std::getenv("MASHED_NARROW_DOT_ORDER"); return e && e[0] == '1'; }();
    return s;
}
inline bool NarrowPairInt() {
    static const bool s = [] { const char* e = std::getenv("MASHED_NARROW_PAIR_INT"); return e && e[0] == '1'; }();
    return s;
}

}  // namespace Collision
}  // namespace mashed_re
