// Mashed RE — frontend port-state (PORT-STATE-01, 2026-11-08). Exe only.
// PREREG: verify/portstate_20261108/PREREG_PORT-STATE-01.md.
//
// The exe's frontend keeps the original's game state in port storage (MenuNavSM.cpp g_game_state), so a byte-matched
// frontend body, which reads the original address, reads different memory than the code that writes it. This header
// carries the first slice of the move onto the original addresses (same shape as MASHED_PORTSTATE_RECORDS, L-88):
//
//   MASHED_PORTSTATE_FRONTEND (default OFF, exe only, read once). ON: the frontend's game-mode word is DAT_0067e9fc
//   itself (Nav_GameModeWord(), MenuNavSM.cpp). OFF: it stays in g_game_state.game_mode_off, as before.
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
