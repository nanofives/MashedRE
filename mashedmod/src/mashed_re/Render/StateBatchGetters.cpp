// Mashed RE — STATE-lane getter batch (methods-efficiency pilot, 2026-07-29).
//
// Four leaf getters selected by MEASUREMENT (commit 0a9a47e8): semantically
// SAFE per semantic_screen.py AND exercised in a Quick Battle race per
// prescreen_batch.py. Decomp + full disasm read from Ghidra slot Mashed_pool0
// this session; every instruction cited below.
//
// Binary anchor: MASHED.exe size=2,846,720
//   sha256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// 0x004f8660  PluginDataDwordA(obj*) -> *(u32*)(obj + *(u32*)0x007d73a8)
//
// Verbatim 0x004f8660..0x004f866d:
//   004f8660 mov eax,[esp+4]
//   004f8664 mov ecx,[0x007d73a8]
//   004f866a mov eax,[ecx+eax]
//   004f866d ret
// DAT_007d73a8 is a RenderWare plugin BYTE-OFFSET (registered by
// FUN_004e8f50(4, 0x50f, ...) at 0x004f8580 — plate
// re/analysis/bucket_004f022d/0x004f8580.md; sentinel run 20260730_103906
// read 0x60), NOT a base pointer; the argument is the live object pointer.
// The add is commutative so the machine code is unchanged either way.
// The state_gate in hooks_registry requires DAT_007d73a8 non-zero (offset
// registered) before any force-call.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl PluginDataDwordA(const void* obj) {
    const std::uint32_t pluginOff = *reinterpret_cast<const std::uint32_t*>(0x007d73a8u);
    return *reinterpret_cast<const std::uint32_t*>(
        reinterpret_cast<std::uintptr_t>(obj) + pluginOff);
}
RH_ScopedInstall(PluginDataDwordA, 0x004f8660);

// ─────────────────────────────────────────────────────────────────────────────
// 0x004f8690  PluginDataDwordB(obj*) -> *(u32*)(obj + *(u32*)0x007d73ac)
//
// Twin of 0x004f8660 with plugin byte-offset DAT_007d73ac (registered by
// FUN_004f0910 at 0x004f8580; sentinel run 20260730_103906 read 0x88).
// Decomp (2026-07-29, Mashed_pool0):
//   return *(undefined4 *)(DAT_007d73ac + param_1);
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl PluginDataDwordB(const void* obj) {
    const std::uint32_t pluginOff = *reinterpret_cast<const std::uint32_t*>(0x007d73acu);
    return *reinterpret_cast<const std::uint32_t*>(
        reinterpret_cast<std::uintptr_t>(obj) + pluginOff);
}
RH_ScopedInstall(PluginDataDwordB, 0x004f8690);

// ─────────────────────────────────────────────────────────────────────────────
// 0x004cfee0  RasterPluginByteGet(int* obj) -> byte at *obj + 8 + DAT_00911ae4
//
// Verbatim 0x004cfee0..0x004cfefb:
//   004cfee0 mov eax,[esp+4]
//   004cfee4 mov eax,[eax]              ; obj[0]
//   004cfee6 test eax,eax / jz 0x4cfef9 ; null -> 0
//   004cfeea mov edx,[0x00911ae4]       ; raster-plugin extension offset
//   004cfef0 xor ecx,ecx
//   004cfef2 mov cl,[eax+edx+0x8]
//   004cfef6 mov eax,ecx / ret          ; byte zero-extended into EAX
//   004cfef9 xor eax,eax / ret
// Return is the FULL zero-extended EAX (xor ecx,ecx / mov cl), so uint32 here
// matches the original's register contract exactly.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl RasterPluginByteGet(const int* obj) {
    const std::uint32_t inner = *reinterpret_cast<const std::uint32_t*>(obj);
    if (inner == 0u) return 0u;
    const std::uint32_t pluginOff =
        *reinterpret_cast<const std::uint32_t*>(0x00911ae4u);
    return *reinterpret_cast<const std::uint8_t*>(
        static_cast<std::uintptr_t>(inner) + pluginOff + 8u);
}
RH_ScopedInstall(RasterPluginByteGet, 0x004cfee0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x004f3cb0  PtrArrayFindLastIndex(int* c, int value) -> last idx of value, or
//             count-1 for a non-positive count / -1 when not found
//
// Verbatim 0x004f3cb0..0x004f3cd6:
//   004f3cb0 mov ecx,[esp+4]            ; c
//   004f3cb4 push esi
//   004f3cb5 mov eax,[ecx+4]            ; count
//   004f3cb8 test eax,eax / jle 0x4f3cd4
//   004f3cbc mov ecx,[ecx]              ; data (read ONLY when count > 0)
//   004f3cbe mov edx,[esp+0xc]          ; value
//   004f3cc2 lea ecx,[ecx+eax*4]
//   004f3cc5 mov esi,[ecx-4] / sub ecx,4 / dec eax
//   004f3ccc cmp esi,edx / jz 0x4f3cd5  ; found -> return eax (the index)
//   004f3cd0 test eax,eax / jg 0x4f3cc5
//   004f3cd4 dec eax                    ; exhaust/empty -> eax-1
//   004f3cd5 pop esi / ret
// NOTE the empty/negative-count path returns count-1 (NOT always -1): a count
// of -3 returns -4. Reproduced verbatim below via the shared `i - 1` exit.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl PtrArrayFindLastIndex(const int* c, int value) {
    int i = c[1];
    if (i > 0) {
        const std::uintptr_t data = static_cast<std::uintptr_t>(
            static_cast<std::uint32_t>(c[0]));
        do {
            --i;
            if (*reinterpret_cast<const int*>(data + static_cast<std::uintptr_t>(i) * 4u)
                == value) {
                return i;
            }
        } while (i > 0);
    }
    return i - 1;
}
RH_ScopedInstall(PtrArrayFindLastIndex, 0x004f3cb0);
