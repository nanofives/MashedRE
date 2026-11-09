// Mashed RE — port-state seed (PORT-STATE-02, decision L-99 (a)). Exe only.
#pragma once

#include <cstdint>

struct PortStateSeedEntry {
    std::uint32_t addr;     // original VA (inside the exe's B17 image pad)
    std::uint32_t len;      // bytes
    std::uint32_t off;      // offset into kPortStateSeedData
};

namespace mashed_re {
namespace Compat {

// Default ON since L-101; MASHED_NO_PORTSTATE_SEED=1 (read once) opts out: no write, no log.
bool PortStateSeed_On();

// WinMain entry, before any game code: copies the generated table (Compat/PortStateSeed.inc) into the pad at
// the original addresses when the knob is ON. Returns the number of entries written (0 when OFF or refused).
unsigned PortStateSeed_Apply();

// After the per-run log truncate: one "[portstate-seed]" line with the counts and FNV-1a witnesses (ON only).
void PortStateSeed_Log(const char* logPath);

} // namespace Compat
} // namespace mashed_re
