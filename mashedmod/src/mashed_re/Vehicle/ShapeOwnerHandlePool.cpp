// Mashed RE — shape-owner handle pool getter.
// Companion to 0x004840d0 JointPtr6ce81cGet (joint-handle pool DAT_006ce81c).
//
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// 0x004840b0  ShapeOwnerHandleGet
// Disasm 0x004840b0 (verbatim):
//   mov eax,[esp+4]               ; param_1 (idx)
//   cmp eax,4 ; jb 0x4840bc       ; idx < 4 -> read ; else
//   xor eax,eax ; ret             ; idx >= 4 -> return 0
//   mov eax,[eax*4 + 0x6ce82c]    ; return DAT_006ce82c[idx]  (dword stride)
// Bounds-checked [0,3] read of the 4-slot shape-owner handle pool.
// Ref: re/analysis/bucket_vehicle_004820e0_00485420/004840b0.md
// ─────────────────────────────────────────────────────────────────────────────
static constexpr std::uintptr_t kShapeOwnerPool_6ce82c = 0x006ce82cu;

extern "C" __declspec(dllexport) std::uint32_t __cdecl ShapeOwnerHandleGet(std::uint32_t idx) {
    if (idx > 3u) return 0u;
    return reinterpret_cast<const std::uint32_t*>(kShapeOwnerPool_6ce82c)[idx];
}
RH_ScopedInstall(ShapeOwnerHandleGet, 0x004840b0);
