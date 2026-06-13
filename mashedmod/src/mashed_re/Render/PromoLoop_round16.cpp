// Mashed RE — promote-round round 16 (Ghidra disassembly pass).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x004c9eb0  DeviceModeBestBelowSet — render; device-mode scan + 0x006181c4 set
//
// This is the round-14/15 carry-over. The full body (0x004c9eb0..0x004c9f42,
// 58 instructions) was disassembled in round 16 (Mashed_pool2 read-only):
//   - Both vtable calls are __stdcall (NO `add esp` after either CALL — the
//     callee pops 12 / 20 bytes), object pushed as an explicit first arg
//     (not __thiscall: ECX holds the vtable, the object goes on the stack).
//   - vtable[0x18] = count query: stdcall(obj, DAT_007d410c, *puVar4) -> int.
//   - vtable[0x1c] = descriptor fetch: stdcall(obj, DAT_007d410c, *puVar4,
//     idx, out16) -> void; writes into a 16-byte stack buffer (SUB ESP,0x10).
//   - uStack_8 = *(uint32_t*)(buffer + 8): LEA EDX,[ESP+0xc] at ESP=E-0x1c
//     gives buffer=E-0x10; MOV EAX,[ESP+0x14] reads E-0x8 = buffer+8.
//   - The accumulator EBX (best value strictly below DAT_006181c4) persists
//     across the 3 outer elements; on an exact match (uStack_8==target) it is
//     set to the target and the inner scan for that element breaks.
//
// Constants cited from the disassembly:
//   0x006181c4 — result global (param_1 written at entry; best-below written
//                at exit if it differs)
//   0x007d4108 — device object pointer (its [0] is the vtable)
//   0x007d410c — second arg passed to both vtable calls
//   0x005d8b80 — base of the 3-element category array (loop end 0x005d8b8c)
//   vtable +0x18 (count), +0x1c (fetch); buffer size 0x10; uStack_8 at +8
//
// Caller: FUN_00493710 RW_INIT_FN (render, C2). Callees are indirect-only
// vtable dispatch (no named callee row to gate — same situation as
// PerModeRenderMachine 0x00404320). Diff is menu-attach: the device object at
// 0x007d4108 is populated post-RW-init, and mode enumeration is deterministic.
//
// Analysis: re/analysis/skeleton_prep_boot_winmain_b/004c9eb0.md (note carries
// the verbatim decomp transcript + this disassembly's convention findings).

#include "../Core/HookSystem.h"

#include <cstdint>

namespace {

// __stdcall: callee pops the args (verified — no caller-side `add esp`).
// Object is an explicit first parameter (not `this`).
using VtCount = int(__stdcall*)(void* obj, void* arg2, std::uint32_t mode);
using VtFetch = void(__stdcall*)(void* obj, void* arg2, std::uint32_t mode,
                                 int idx, void* out16);

}  // namespace

// ---------------------------------------------------------------------------
// DeviceModeBestBelowSet  --  0x004c9eb0   (subsystem: render)
//
// void FUN_004c9eb0(uint param_1):
//   writes param_1 to DAT_006181c4, then (if param_1 != 0) scans the device's
//   enumerated descriptors across 3 categories, tracking the largest fetched
//   value strictly below DAT_006181c4 (or the target itself on exact match),
//   and overwrites DAT_006181c4 with that value if it differs.
// ---------------------------------------------------------------------------

// 0x004c9eb0
extern "C" __declspec(dllexport) void __cdecl DeviceModeBestBelowSet(std::uint32_t param_1) {
    // MOV [0x006181c4],EAX
    *reinterpret_cast<std::uint32_t*>(0x006181c4u) = param_1;
    // TEST EAX,EAX / JZ epilogue
    if (param_1 == 0u) {
        return;
    }

    std::uint32_t ebx = 0u;                                  // XOR EBX,EBX (best-below)
    const std::uint32_t* edi =                               // MOV EDI,0x5d8b80
        reinterpret_cast<const std::uint32_t*>(0x005d8b80u);
    std::uint32_t ecx = 0u;                                  // last-read DAT_006181c4

    do {  // outer loop, 0x004c9eca
        void* obj = *reinterpret_cast<void**>(0x007d4108u);
        void** vtbl = *reinterpret_cast<void***>(obj);       // MOV ECX,[EAX]
        VtCount fn18 = reinterpret_cast<VtCount>(vtbl[0x18 / 4]);
        int esi = fn18(obj,
                       *reinterpret_cast<void**>(0x007d410cu),
                       *edi);                                 // count -> ESI

        if (esi != 0) {                                      // TEST ESI,ESI / JZ
            do {  // inner loop, 0x004c9ee5
                void* obj2 = *reinterpret_cast<void**>(0x007d4108u);
                std::uint8_t buf[16];                         // SUB ESP,0x10 frame slot
                esi = esi - 1;                                // DEC ESI (before the call)
                void** vtbl2 = *reinterpret_cast<void***>(obj2);
                VtFetch fn1c = reinterpret_cast<VtFetch>(vtbl2[0x1c / 4]);
                fn1c(obj2,
                     *reinterpret_cast<void**>(0x007d410cu),
                     *edi, esi, buf);
                // MOV EAX,[ESP+0x14] == uStack_8 == buffer+8
                const std::uint32_t eax =
                    *reinterpret_cast<const std::uint32_t*>(buf + 8);
                // MOV ECX,[0x006181c4]
                ecx = *reinterpret_cast<const std::uint32_t*>(0x006181c4u);
                if (eax == ecx) {                             // CMP/JZ 0x004c9f1d
                    ebx = ecx;                                // MOV EBX,ECX
                    break;                                    // JMP 0x004c9f27
                }
                // JNC 0x004c9f17 -> skip update when eax >= ecx (unsigned)
                if (eax < ecx) {
                    if (ebx < eax) {                          // CMP EBX,EAX / JNC
                        ebx = eax;                            // MOV EBX,EAX
                    }
                }
                // 0x004c9f17: TEST ESI,ESI / JNZ inner
            } while (esi != 0);
        } else {
            // 0x004c9f21: MOV ECX,[0x006181c4]
            ecx = *reinterpret_cast<const std::uint32_t*>(0x006181c4u);
        }
        edi = edi + 1;                                       // ADD EDI,0x4
    } while (reinterpret_cast<std::uintptr_t>(edi) < 0x005d8b8cu);  // CMP/JL

    // CMP ECX,EBX / JZ skip / MOV [0x006181c4],EBX
    if (ecx != ebx) {
        *reinterpret_cast<std::uint32_t*>(0x006181c4u) = ebx;
    }
}

RH_ScopedInstall(DeviceModeBestBelowSet, 0x004c9eb0);
