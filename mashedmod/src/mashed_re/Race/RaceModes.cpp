// RaceModes — see RaceModes.h. Verbatim transcription of the Mashed game-mode
// → race-rule pipeline (WS-G1). RVA citations inline; full map in
// re/analysis/game_mode_rules_REmap_20260616.md.
#include "RaceModes.h"

namespace mashed_re {
namespace Race {
namespace RaceModes {

namespace {
// Cup event-type table — high word of each 4-byte DAT_005f65c8 entry, read
// from MASHED.exe .rdata at 0x005f65c8 via Ghidra (Mashed_pool7, 2026-06-16).
// Indexed [modeOff + track*3]; modeOff 0/1/2 for game mode 3/4/5; 13 tracks.
// (Entries 39+ in the binary have high-word 0 — not part of the cup.)
const int kCupEvent[39] = {
    // track 0       track 1       track 2       track 3
    0x9, 0x3, 0x1,  0xa, 0x1, 0xb,  0xa, 0x4, 0x5,  0x3, 0x1, 0x7,
    // track 4       track 5       track 6       track 7
    0x1, 0x2, 0x5,  0x1, 0xc, 0x4,  0x1, 0x6, 0xc,  0xa, 0x3, 0x7,
    // track 8       track 9       track 10      track 11
    0x3, 0x1, 0xc,  0x1, 0xc, 0x5,  0x3, 0x5, 0x1,  0x1, 0xc, 0x6,
    // track 12
    0x1, 0x4, 0x7,
};
const int kCupTracks = 13;
}  // namespace

// 0x0042f6b0 — switch base 0x0042f6b8 (writes DAT_0067e9fc).
int SelectionToGameMode(int sel) {
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
        default: return -1;  // sel 4,6,7,>11 -> original makes no write
    }
}

// 0x0043f3xx: iVar8 = (modeOff + track*3); high word of DAT_005f65c8[iVar8].
int EventType(int gameMode, int track) {
    int modeOff;
    switch (gameMode) {
        case 3:  modeOff = 0; break;
        case 4:  modeOff = 1; break;
        case 5:  modeOff = 2; break;
        default: return -1;          // non-championship: no table read
    }
    if (track < 0 || track >= kCupTracks) return -1;
    return kCupEvent[modeOff + track * 3];
}

// 0x0043dfd0 race-launch (uVar10 = entry & 0xffff0000) -> DAT_007f0fd0.
int RaceRuleFromEvent(int eventType) {
    switch (eventType) {
        case 0x7: return 5;   // 0x70000
        case 0x4: return 10;  // 0x40000
        case 0x8: return 7;   // 0x80000
        case 0x5: return 9;   // 0x50000
        case 0x2:             // 0x20000
        case 0x3:             // 0x30000
        case 0xb: return 4;   // 0xb0000
        case 0x6: return 8;   // 0x60000
        case 0x0:             // 0x00000
        case 0x1:             // 0x10000
        case 0x9:             // 0x90000
        case 0xa:             // 0xa0000
        case 0xc: return 0;   // 0xc0000
        default:  return 0;   // pre-switch default (DAT_007f0fd0 = 0, 0x0043f7a7)
    }
}

// switch(DAT_0067e9fc) in the race-launch action; modes outside 3/4/5 leave
// the pre-switch default rule 0 (FUN_00430b60 gate / push-screen branches do
// not write DAT_007f0fd0).
int RaceRule(int gameMode, int track) {
    switch (gameMode) {
        case 3:
        case 4:
        case 5:
            return RaceRuleFromEvent(EventType(gameMode, track));
        case 2:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        default:
            return 0;
    }
}

// 0xff420000 action, mode != 10 branch: DAT_0067ea88 -> DAT_007f0fd0.
int RaceRuleFromGameLength(int gameLength) {
    switch (gameLength) {
        case 0:  return 0;
        case 1:  return 1;
        case 2:  return 2;
        default: return 0;
    }
}

// FUN_00410d10: cases 5 and 7 early-return before the proximity-elimination
// block; every other rule reaches it.
bool IsEliminationRule(int rule) {
    return rule != 5 && rule != 7;
}

int RaceModeForObjective(int rule) {
    return IsEliminationRule(rule) ? 0 : 1;
}

}  // namespace RaceModes
}  // namespace Race
}  // namespace mashed_re
