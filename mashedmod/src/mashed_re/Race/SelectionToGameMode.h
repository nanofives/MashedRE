// Mashed RE - FUN_00405890's neighbour FUN_0042f6b0 (0x0042f6b0): the menu-selection -> game-mode table, ONE body for both targets.
//
// dualcopy4 (2026-10-16, ROADMAP D4). The RVA had two bodies with the same case table: Frontend/FrontendState.cpp:MenuModeSync
// (both targets; reads DAT_0067f184, writes DAT_0067e9fc, installed in the .asi) and Race/RaceModes.cpp:SelectionToGameMode
// (exe only; pure, returns the value, -1 where the original writes nothing; used by Frontend/MenuNavSM.cpp:RowModeSync).
// The table is this function. MenuModeSync is its original-address reader/writer, SelectionToGameMode its host-side name.
//
// Original (0x0042f6b0, switch base 0x0042f6b8; DAT_0067f184 = selection, DAT_0067e9fc = game mode):
#pragma once

namespace mashed_re {
namespace Race {
namespace RaceModes {

inline int SelectionToGameModeBody(int sel) {
    switch (sel) {
        case 0:  return 2;   // 0x0042f6bc
        case 1:  return 3;   // 0x0042f6c6
        case 2:  return 4;   // 0x0042f6d0
        case 3:  return 6;   // 0x0042f6da
        case 5:  return 5;   // 0x0042f6e4
        case 8:  return 7;   // 0x0042f6ee
        case 9:  return 8;   // 0x0042f6f8
        case 10: return 9;   // 0x0042f702
        case 11: return 10;  // 0x0042f70c
        default: return -1;  // sel 4, 6, 7 and > 11: the original makes no write
    }
}

}  // namespace RaceModes
}  // namespace Race
}  // namespace mashed_re
