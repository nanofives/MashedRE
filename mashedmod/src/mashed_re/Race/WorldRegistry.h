// Mashed RE — mode 7 leg M1: the original's world-object registry / collision broker, verbatim.
// Exe-only TU. PREREG: verify/d3_mode7_20261010/PREREG_MODE7.md.
#pragma once
#include <cstdint>

namespace mashed_re {
namespace Race {

// MASHED_REGISTRY (default OFF).
bool Registry_Enabled();

// FUN_00484c90 (race reset, FUN_004111c0). No-op unless enabled.
void Registry_RaceInit();

// FUN_0040fc00's FUN_00485070 call (0x0040fd07): pair pass + publish. No-op unless enabled.
void Registry_FrameTick();

// FUN_00484cf0(desc): desc = {pos x, y, z, radius, type, data, +0x18, callback} (8 dwords).
// Returns 1 if registered. For leg M2's registrants; no caller in M1.
int Registry_Register(const std::uint32_t desc[8]);

// G-M1-REG / G-M1-PASS: MASHED_REGISTRY_REPLAY=<o_*.msd.reg.tsv>, report to MASHED_REGISTRY_REPLAY_OUT.
void Registry_MaybeReplay();

}  // namespace Race
}  // namespace mashed_re
