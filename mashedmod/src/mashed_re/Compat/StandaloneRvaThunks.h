// Mashed RE — standalone RVA-thunk installer (Milestone B16, Phase C/F/G).
//
// MASHED's reimpls call each other by ABSOLUTE MASHED RVA (e.g. MenuChromeShellA
// reaches ChromeBaseDraw via a pointer to 0x00472c60, and the screen-dim getters
// at 0x0042b8b0/0x0042b8c0). In the dev `.asi` those RVAs are MASHED's own code
// (or our inline-JMP hooks). In the standalone exe (image based at 0x10000000)
// the entire 0x00400000..0x004fffff MASHED .text range is unmapped, so any such
// call faults.
//
// This installer maps the needed MASHED .text granules and writes a 5-byte
// `E9 rel32` JMP thunk at each chosen RVA, redirecting it to our standalone
// reimpl (or a small stub). It is the standalone analogue of InjectHooks: it
// lets reimpls that reference other functions by RVA run outside MASHED. Reusable
// for every future frontend draw/update function the standalone drives.

#pragma once

#include <cstdint>
#include <cstddef>

namespace mashed_re {
namespace Compat {

// VirtualAlloc/commit the 64KB granules spanning [base, base+bytes) as
// PAGE_EXECUTE_READWRITE so thunks can be written AND executed there. MEM_FREE
// granules are reserved+committed; MEM_RESERVE are committed; already-committed
// granules are re-protected to RWX. Returns true if at least one granule is now
// usable. Idempotent.
bool StandaloneThunks_MapRange(std::uintptr_t base, std::size_t bytes);

// Write `E9 rel32` at `rva` so a call/jmp to `rva` lands at `target`. The rva's
// granule must already be mapped (StandaloneThunks_MapRange). Returns false if
// the granule is not committed. Flushes the instruction cache.
bool StandaloneThunks_Install(std::uintptr_t rva, const void* target);

// Stubs the standalone supplies for MASHED RVAs that have no standalone reimpl.
// The screen getters return the fixed 800x600 backbuffer dims (MASHED reads them
// as undefined2 / 16-bit); the credits line is a no-op (no credits screen in the
// standalone frontend yet).
extern "C" std::uint16_t __cdecl Standalone_ScreenWidth();    // -> 800
extern "C" std::uint16_t __cdecl Standalone_ScreenHeight();   // -> 600
extern "C" void          __cdecl Standalone_CreditsNoOp(int); // no-op

// Count of thunks successfully installed (for logging).
std::uint32_t StandaloneThunks_Count();

}  // namespace Compat
}  // namespace mashed_re
