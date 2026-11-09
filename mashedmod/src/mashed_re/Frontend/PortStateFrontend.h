// Mashed RE — frontend port-state (PORT-STATE-01, 2026-11-08). Exe only.
// PREREG: verify/portstate_20261108/PREREG_PORT-STATE-01.md.
//
// The exe's frontend keeps the original's game state in port storage (MenuNavSM.cpp g_game_state), so a byte-matched
// frontend body, which reads the original address, reads different memory than the code that writes it. This header
// carries the move onto the original addresses (same shape as MASHED_PORTSTATE_RECORDS, L-88), one slice per step:
//
//   MASHED_PORTSTATE_FRONTEND (default OFF, exe only, read once). ON: the frontend's game-mode word is DAT_0067e9fc
//   itself (Nav_GameModeWord(), MenuNavSM.cpp), the menu records are DAT_00898ac0 and the player-active test reads
//   DAT_007e96fc (PORT-STATE-03). OFF: all three stay in port storage, as before.
//
// ModeWordScope is the bridge for the matched bodies that read DAT_0067e9fc (0x00430830, 0x004309b0): for the length
// of one call it puts the value the exe caller would have passed into the word and restores the previous value after.
// Both bodies are leaves (no callees, re/match/s07/00430830.c / 004309b0.c), so nothing else runs while the word holds
// the bridged value. With the knob ON the 0x00430830 sites pass Nav_GameModeWord(), which is the word, so the bridge
// writes nothing there. The witness ([portstate-frontend] lines in mashed_re.log) runs with the knob ON only.
#pragma once
#include <cstdint>

namespace mashed_re {
namespace Frontend {

constexpr std::uintptr_t kGameModeWord = 0x0067e9fcu;   // DAT_0067e9fc, read by FUN_0042f6a0 / 00430830 / 004309b0

// PORT-STATE-03 (2026-11-08, verify/portstate_20261108/PREREG_PORT-STATE-03.md), same knob:
// ON: the frontend menu records are DAT_00898ac0 itself (MenuNavSM.cpp g_records). The original array is 30 records of 0x34:
// 0x0042d3e0 loops while eax < 0x008990dc with eax = record + 4 (0x0042d413), so 0x008990d8.. are other globals (DAT_008990e4 is
// written by 0x0042aad0 and tested / cleared by 0x00428140, both compiled in the exe). With the knob ON the exe writes records
// 0..29 only (kOrigMenuRecords); the port store keeps its 31.
constexpr std::uintptr_t kMenuRecords     = 0x00898ac0u;
constexpr int            kOrigMenuRecords = 30;
// ON: the screen-0x1c player-active test reads DAT_007e96fc[i*0x80] (stride 0x200) through the matched 0x00497450.
constexpr std::uintptr_t kPlayerActiveWord = 0x007e96fcu;

// The matched 0x004325c0 (L-100 (a), both knob states) reads these input words; MenuNavSM.cpp Anim_Tick bridges them for one call.
constexpr std::uintptr_t kAnimDtWord        = 0x007f1004u;   // DAT_007f1004 frame dt (FUN_00493480)
constexpr std::uintptr_t kAnimPhaseWord     = 0x0067eca4u;   // DAT_0067eca4 (>= 4 picks 2.0f, 0x004325c8)
constexpr std::uintptr_t kAnimGateWord      = 0x0067e914u;   // DAT_0067e914 (0x00432603)
constexpr std::uintptr_t kAnimItemTotalWord = 0x0067ece0u;   // DAT_0067ece0 item count (read into EBP, 0x004325dd)
// Record +0x30 holds the screen's table pointer (0x0043d2a0: it->def = DAT_0067ed78[lv].def, re/match/s08/0043d2a0.c:302).
// 0x004325c0 compares it with these two (0x004326e4 / 0x0043271f) and never dereferences it.
constexpr std::uint32_t kScreen24Table = 0x005f7370u;        // &DAT_005f7370, screen 24
constexpr std::uint32_t kScreen18Table = 0x005f72a0u;        // &DAT_005f72a0, screen 18

bool PortStateFrontend_On();

enum ModeWordSite : int { kSite00430830RowAvail = 0, kSite00430830Arrows = 1, kSite004309b0Gate = 2, kSiteCount = 3 };

class ModeWordScope {
public:
    ModeWordScope(ModeWordSite site, std::int32_t value);
    ~ModeWordScope();
    ModeWordScope(const ModeWordScope&) = delete;
    ModeWordScope& operator=(const ModeWordScope&) = delete;
    // Witness: the matched body's return value at this site (knob ON only; no-op otherwise).
    void Result(std::int32_t ret) const;
private:
    ModeWordSite m_site;
    std::int32_t m_saved;
    bool         m_changed;
};

} // namespace Frontend
} // namespace mashed_re
