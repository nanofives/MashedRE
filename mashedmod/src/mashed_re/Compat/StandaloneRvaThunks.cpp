// Mashed RE — standalone RVA-thunk installer implementation (Milestone B16).
// See StandaloneRvaThunks.h for the rationale.

#include "StandaloneRvaThunks.h"

#include <windows.h>

namespace mashed_re {
namespace Compat {

namespace {
std::uint32_t g_count = 0;
constexpr std::uintptr_t kGran = 0x10000u;  // 64KB allocation granularity
}  // namespace

extern "C" std::uint16_t __cdecl Standalone_ScreenWidth()    { return 800; }
extern "C" std::uint16_t __cdecl Standalone_ScreenHeight()   { return 600; }
extern "C" void          __cdecl Standalone_CreditsNoOp(int) {}

bool StandaloneThunks_MapRange(std::uintptr_t base, std::size_t bytes) {
    const std::uintptr_t lo = base & ~(kGran - 1);
    const std::uintptr_t hi = (base + bytes + (kGran - 1)) & ~(kGran - 1);
    bool any = false;
    for (std::uintptr_t a = lo; a < hi; a += kGran) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (VirtualQuery(reinterpret_cast<LPVOID>(a), &mbi, sizeof(mbi)) == 0) continue;
        if (mbi.State == MEM_FREE) {
            if (VirtualAlloc(reinterpret_cast<LPVOID>(a), kGran,
                             MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)) any = true;
        } else if (mbi.State == MEM_RESERVE) {
            if (VirtualAlloc(reinterpret_cast<LPVOID>(a), kGran,
                             MEM_COMMIT, PAGE_EXECUTE_READWRITE)) any = true;
        } else {  // MEM_COMMIT — already mapped; ensure RWX
            DWORD old = 0;
            if (VirtualProtect(reinterpret_cast<LPVOID>(a), kGran,
                               PAGE_EXECUTE_READWRITE, &old)) any = true;
        }
    }
    return any;
}

bool StandaloneThunks_Install(std::uintptr_t rva, const void* target) {
    MEMORY_BASIC_INFORMATION mbi{};
    if (VirtualQuery(reinterpret_cast<LPVOID>(rva), &mbi, sizeof(mbi)) == 0 ||
        mbi.State != MEM_COMMIT) {
        return false;
    }
    DWORD old = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(rva), 5, PAGE_EXECUTE_READWRITE, &old)) {
        return false;
    }
    std::uint8_t* p = reinterpret_cast<std::uint8_t*>(rva);
    const std::int32_t rel =
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(target) - (rva + 5));
    p[0] = 0xE9;
    *reinterpret_cast<std::int32_t*>(p + 1) = rel;
    DWORD tmp = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(rva), 5, old, &tmp);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPVOID>(rva), 5);
    ++g_count;
    return true;
}

std::uint32_t StandaloneThunks_Count() { return g_count; }

}  // namespace Compat
}  // namespace mashed_re
