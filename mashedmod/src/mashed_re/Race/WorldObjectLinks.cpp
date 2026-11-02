// Mashed RE — world-object cross-link insert and list reset: ONE body each for both build targets
// (D4-DC-INC-batch3, 2026-11-01).
//
// Before this file each RVA had two bodies, the dual-copy class (ROADMAP D4, re/tools/dual_copy_allowlist.txt):
//   Util/PromoLoop_sessionB.cpp                        .asi-only, installed, the body hooks.csv's evidence covers
//   RG_484a50 / RG_484c90 (Race/WorldRegistry_naked.inc) exe-only, private verbatim naked copies
// This TU is in BOTH exe_sources.rsp and asi_sources.rsp. The bodies moved here unchanged. RH_ScopedInstall installs
// them in the .asi and is a no-op in the exe. RG_484ac0's two `call`s now reach Insert484a50
// (re/tools/naked_specs/registry_m1.json) and Registry_RaceInit calls Zero484c90 (Race/WorldRegistry.cpp).
//   Insert484a50: the same 26 instructions as RG_484a50 (register convention EAX = container, ECX = item).
//   Zero484c90:   writes the same 300 records and the two dwords RG_484c90 wrote, at the same absolute addresses
//                 (the exe maps 0x00500000..0x009fffff). Its one caller is C, so the cdecl clobber set applies.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"
#include "WorldObjectLinks.h"

#include <cstdint>

// 0x00484a50  world-objects — byte-verified register-conv (EAX=container, ECX=item)
//   cross-link insert. Verbatim naked __asm port (bit-identical by construction).
//   Guards on [eax+0x1b4] & [eax+0x190]<0x64; stores ecx into eax[count]; tracks
//   [ecx+0x10] into [eax+0x1ac]/[eax+0x1b0]; registers eax into ecx[0x24+cnt2]; ret 1/0.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Insert484a50(void) {
    __asm {
        mov edx, dword ptr [eax + 0x1b4]
        test edx, edx
        jne L_ret0
        mov edx, dword ptr [eax + 0x190]
        cmp edx, 0x64
        jl L_ins
    L_ret0:
        xor eax, eax
        ret
    L_ins:
        mov dword ptr [eax + edx*4], ecx
        inc dword ptr [eax + 0x190]
        mov edx, dword ptr [eax + 0x1b0]
        test edx, edx
        jne L_b
        mov edx, dword ptr [eax + 0x1ac]
        test edx, edx
        jne L_c
        mov edx, dword ptr [ecx + 0x10]
        mov dword ptr [eax + 0x1ac], edx
    L_c:
        mov edx, dword ptr [eax + 0x1ac]
        cmp edx, dword ptr [ecx + 0x10]
        je L_b
        mov dword ptr [eax + 0x1b0], 1
        mov edx, dword ptr [ecx + 0x10]
        mov dword ptr [eax + 0x1ac], edx
    L_b:
        mov edx, dword ptr [ecx + 0x64]
        mov dword ptr [ecx + edx*4 + 0x24], eax
        inc dword ptr [ecx + 0x64]
        mov eax, 1
        ret
    }
}
RH_ScopedInstall(Insert484a50, 0x00484a50);

// 0x00484c90 — void fn(): for p=0x6dccbc; p<0x6e70cc; p+=0x8c (idx 0,1,2,..):
//   *(u16*)(p+0x1c)=idx; zero [p-4],[p],[p+4],[p+8],[p+0xc],[p+0x10],[p+0x18]; zero 9 dwords
//   at [p+0x64]. Then *0x6e70d8=0; *0x6e70cc=0.
extern "C" __declspec(dllexport) void __cdecl Zero484c90(void) {
    int idx = 0;
    for (std::uint8_t* p = reinterpret_cast<std::uint8_t*>(0x006dccbc);
         reinterpret_cast<std::uintptr_t>(p) < 0x006e70cc; p += 0x8c) {
        *reinterpret_cast<std::uint16_t*>(p + 0x1c) = static_cast<std::uint16_t>(idx);
        *reinterpret_cast<std::uint32_t*>(p - 4) = 0;
        *reinterpret_cast<std::uint32_t*>(p) = 0;
        *reinterpret_cast<std::uint32_t*>(p + 4) = 0;
        *reinterpret_cast<std::uint32_t*>(p + 8) = 0;
        *reinterpret_cast<std::uint32_t*>(p + 0xc) = 0;
        *reinterpret_cast<std::uint32_t*>(p + 0x10) = 0;
        *reinterpret_cast<std::uint32_t*>(p + 0x18) = 0;
        for (int k = 0; k < 9; k++) reinterpret_cast<std::uint32_t*>(p + 0x64)[k] = 0;
        idx++;
    }
    *reinterpret_cast<std::uint32_t*>(0x006e70d8) = 0;
    *reinterpret_cast<std::uint32_t*>(0x006e70cc) = 0;
}
RH_ScopedInstall(Zero484c90, 0x00484c90);
