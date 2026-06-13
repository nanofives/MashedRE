// Mashed RE — promote-round round 48 (SmplFzx array-stack push/pop pair).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Array-stack {top@0, cap@4, buf-ptr@8}. Diffed via early_window_leaf_diff
// stack_pop_snapshot / stack_push_snapshot. Callers smplfzx C2.
//   0x00485bd0  StackPop485bd0  — top--; <0?clamp,0:buf[top]  (caller 004850e0 smplfzx C2)
//   0x00485bf0  StackPush485bf0 — full?0:buf[top]=val,top++,1 (caller 004852e0 smplfzx C2)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00485bd0  top=*p-1; *p=top; if(top<0){*p=0;return 0;} return *(p[2]+top*4)
extern "C" __declspec(dllexport) std::uint32_t __cdecl StackPop485bd0(void* stk) {
    std::uint8_t* m = reinterpret_cast<std::uint8_t*>(stk);
    std::int32_t top = *reinterpret_cast<std::int32_t*>(m + 0) - 1;
    *reinterpret_cast<std::int32_t*>(m + 0) = top;
    if (top < 0) { *reinterpret_cast<std::int32_t*>(m + 0) = 0; return 0; }
    const std::uint32_t buf = *reinterpret_cast<std::uint32_t*>(m + 8);
    return reinterpret_cast<const std::uint32_t*>(static_cast<std::uintptr_t>(buf))[top];  // cited at 0x00485bd0
}
RH_ScopedInstall(StackPop485bd0, 0x00485bd0);

// 0x00485bf0  top=*p; if(p[1]<=top)return 0; if(top<0) FUN_004a332b(-42); *(p[2]+top*4)=val; (*p)++; return 1
extern "C" __declspec(dllexport) std::uint32_t __cdecl StackPush485bf0(void* stk, std::uint32_t val) {
    std::uint8_t* m = reinterpret_cast<std::uint8_t*>(stk);
    const std::int32_t top = *reinterpret_cast<std::int32_t*>(m + 0);
    if (*reinterpret_cast<std::int32_t*>(m + 4) <= top) return 0;
    if (top < 0)
        reinterpret_cast<void(__cdecl*)(std::uint32_t)>(static_cast<std::uintptr_t>(0x004a332bu))(0xffffffd6u);  // range-check trap (untested path)
    const std::uint32_t buf = *reinterpret_cast<std::uint32_t*>(m + 8);
    reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(buf))[top] = val;  // cited at 0x00485bf0
    *reinterpret_cast<std::int32_t*>(m + 0) = top + 1;
    return 1;
}
RH_ScopedInstall(StackPush485bf0, 0x00485bf0);
