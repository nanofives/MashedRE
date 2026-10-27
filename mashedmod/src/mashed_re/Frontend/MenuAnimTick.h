// Mashed RE - FUN_004325c0 (0x004325c0): the menu slide-animation tick, ONE body for both targets
// (D4-FU-MENUSPEED, 2026-10-27, decision L-35 = 3). Body in Frontend/MenuAnimTickTwin.cpp, which is in both rsp lists.
// The .asi export MenuAnimTickTwin passes the original's live globals; the exe Anim_Tick (Frontend/MenuNavSM.cpp)
// passes its g_records / frame clock / nav slot. Only the inputs below differ per target, never the logic.
#pragma once

#include <cstdint>

#include "MenuNavSM.h"

namespace mashed_re {
namespace Frontend {

struct MenuAnimTickIn {
    float        dt;          // DAT_007f1004 (FUN_00493480 frame dt)
    std::int32_t phase;       // DAT_0067eca4: >= 4 picks base 2.0 (0x004325c8)
    std::int32_t gate;        // DAT_0067e914 (0x00432603)
    std::int32_t item_total;  // DAT_0067ece0, read once into EBP (0x004325dd)
    int (*list_base_y)(int count, int spacing);  // FUN_0042ac50 (count in EAX, spacing in ECX)
    // Screen match on the type-0 settle (0x004326e4 / 0x0043271f). The original compares record +0x30
    // ([esi+0x20]) against 0x005f7370 and 0x005f72a0; the exe has no pointer there and passes its nav screen id.
    bool         screen_per_record;  // true: read record +0x30; false: use `screen`
    std::int32_t screen;
    std::int32_t screen_24;   // key for the 0x1c recompute - 58.0 (0x005f7370 / screen 24)
    std::int32_t screen_18;   // key for the extra - 58.0 (0x005f72a0 / screen 18)
};

// Walks records 0..29 (0x00898ad0..0x008990e8, stride 0x34). Returns 1 when nothing is left animating.
int MenuAnimTickBody(MenuRecord* recs, const MenuAnimTickIn& in);

// MASHED_MENU_ANIM_SPEED (float > 0), default 1.0 = the original's rate. Multiplies the base speed of the
// K-scaled steps only; the float path (0x00432751) has no base in the original and none here.
float MenuAnimSpeed();

}  // namespace Frontend
}  // namespace mashed_re
