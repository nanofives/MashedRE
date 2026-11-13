// Mashed RE - trap on the original's code range (IMG-3-TEXT-TRAP, decision L-149 (a) step 3). Exe only.
// Spec: verify/portstate_20261113/RESULT_PORTSTATE-IMAGE.md section 4. Knob MASHED_CODE_TRAP (default OFF), read only
// on WinMain's MASHED_ORIG_TOP branch. The default (hand) path never calls this file.
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Compat {

// MASHED_CODE_TRAP: 0 / unset = off, 1 = report the first trap and exit, 2 = report every new execute trap whose call
// site decodes and simulate a return (eax = 0, the callee's arguments left to the caller), exit on anything else.
int CodeTrap_Mode();

// WinMain, MASHED_ORIG_TOP branch, after the image / seed and the code-pointer fix-ups: protects 0x00401000..0x005cbfff
// (the original's .text and _rwcseg, page rounded, inside the B17 pad) PAGE_NOACCESS and installs the vectored handler.
// No-op when the knob is off. Returns true when armed.
bool CodeTrap_Arm();

// True after a successful CodeTrap_Arm.
bool CodeTrap_Armed();

// The B16 thunk installer (StandaloneRvaThunks.cpp) calls this for each page it writes a thunk into: the page becomes
// PAGE_EXECUTE_READWRITE and, on its first carve, is filled with 0xCC so the bytes around the thunk still trap
// (EXCEPTION_BREAKPOINT). Pages outside the trap range are not touched. Returns true when the page was in the range.
bool CodeTrap_AllowPage(std::uintptr_t page);

// True when the trap is armed and addr lies in 0x00401000..0x005cbfff (StandaloneThunks_MapRange clips its granules).
bool CodeTrap_InRange(std::uintptr_t addr);

// After the per-run log truncate: the "[code-trap]" lines held since Arm.
void CodeTrap_Log(const char* logPath);

} // namespace Compat
} // namespace mashed_re
