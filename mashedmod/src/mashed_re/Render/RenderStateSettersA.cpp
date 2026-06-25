// Mashed RE — Render-state setter cluster C2->C3 promotions (c3-batch-sgr1 session s1).
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Six leaf accessors of the deferred render-state dirty-queue subsystem. One
// read accessor and five cached dirty-queue setters. All are __cdecl, plain
// `ret` (no stdcall cleanup — byte-verified epilogue `b8 01 00 00 00 c3`),
// no callees. Shared dirty queue: dword array at 0x007d5168 indexed by the
// running counter 0x007d6c14 (same queue used by EventEnqueueCascade4d7100 /
// Mark4d5480 / Set4d6c40 / Set4d6c90 — see Util\PromoLoop_sessionB.cpp).
//
// Functions promoted in this file:
//   0x004d71f0  RenderState::GetTexturingOverride — read DAT_007d6b10
//   0x004d7200  RenderState::SetOpcode34 — RAW store -> DAT_007d5998 (opcode 0x34)
//   0x004d7250  RenderState::SetOpcode35 — table 0x005d8d28[i] -> DAT_007d59a0 (0x35)
//   0x004d72a0  RenderState::SetOpcode36 — table 0x005d8d28[i] -> DAT_007d59a8 (0x36)
//   0x004d72f0  RenderState::SetOpcode37 — table 0x005d8d28[i] -> DAT_007d59b0 (0x37)
//   0x004d7340  RenderState::SetOpcode38 — table 0x005d8d4c[i] -> DAT_007d59b8 (0x38)
//
// Analysis refs:
//   re/analysis/render_6_c1_to_c2_s2/004d71f0.md
//   re/analysis/render_6_c1_to_c2_s2/004d7200.md
//   re/analysis/render_6_c1_to_c2_s2/004d7250.md
//   re/analysis/render_6_c1_to_c2_s2/004d72a0.md
//   re/analysis/render_6_c1_to_c2_s2/004d72f0.md
//   re/analysis/render_6_c1_to_c2_s2/004d7340.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RenderState::GetTexturingOverride — 0x004d71f0
//
// Original: FUN_004d71f0 (6 bytes of code, 0x004d71f0..0x004d71f5)
// Signature: undefined4 FUN_004d71f0(void)
//
// Byte-verified body:
//   a1 10 6b 7d 00     mov  eax, [0x007d6b10]   ; texturing-override flag
//   c3                 ret                      ; cdecl, no immediate cleanup
//
// Pure read accessor. DAT_007d6b10 is the texturing-override flag whose
// write-side is EventEnqueueCascade4d7100 (0x004d7100). Pure leaf (no callees).
// ---------------------------------------------------------------------------

// 0x004d71f0
extern "C" __declspec(dllexport) std::uint32_t __cdecl RenderState_GetTexturingOverride()
{
    return *reinterpret_cast<std::uint32_t*>(0x007d6b10);
}

RH_ScopedInstall(RenderState_GetTexturingOverride, 0x004d71f0);

// ---------------------------------------------------------------------------
// RenderState::SetOpcode34 — 0x004d7200
//
// Original: FUN_004d7200 (0x004d7200..0x004d7247)
// Signature: undefined4 FUN_004d7200(int param_1)
//
// Byte-verified body (cache 0x007d6af0, value-store DAT_007d5998 RAW, dirty
// flag DAT_007d599c, queued opcode 0x34):
//   8b 4c 24 04                 mov  ecx, [esp+4]        ; param_1
//   a1 f0 6a 7d 00              mov  eax, [0x007d6af0]   ; cache
//   3b c8 / 74 35               cmp ecx,eax ; je ret     ; cache hit -> return 1
//   a1 9c 59 7d 00              mov  eax, [0x007d599c]    ; load dirty flag
//   89 0d 98 59 7d 00           mov  [0x007d5998], ecx    ; RAW store (no table)
//   85 c0 / 75 20               test eax,eax ; jne skip   ; dirty already set
//   a1 14 6c 7d 00              mov  eax, [0x007d6c14]     ; queue index
//   c7 05 9c 59 7d 00 01..      mov  [0x007d599c], 1       ; dirty = 1
//   c7 04 85 68 51 7d 00 34..   mov  [0x007d5168+eax*4], 0x34
//   40                          inc  eax
//   a3 14 6c 7d 00              mov  [0x007d6c14], eax     ; queue index++
//   89 0d f0 6a 7d 00           mov  [0x007d6af0], ecx     ; cache = param_1
//   b8 01 00 00 00 / c3         mov eax,1 ; ret
//
// Unlike the four table-lookup twins below, this stores param_1 directly (no
// table indirection). Pure leaf (no callees). Caller: FUN_004d7480.
// ---------------------------------------------------------------------------

// 0x004d7200
extern "C" __declspec(dllexport) std::uint32_t __cdecl RenderState_SetOpcode34(std::uint32_t param_1)
{
    if (param_1 == *reinterpret_cast<std::uint32_t*>(0x007d6af0)) return 1;
    *reinterpret_cast<std::uint32_t*>(0x007d5998) = param_1;               // RAW store
    if (*reinterpret_cast<std::uint32_t*>(0x007d599c) == 0) {
        std::uint32_t c = *reinterpret_cast<std::uint32_t*>(0x007d6c14);
        *reinterpret_cast<std::uint32_t*>(0x007d599c) = 1;
        reinterpret_cast<std::uint32_t*>(0x007d5168)[c] = 0x34;
        *reinterpret_cast<std::uint32_t*>(0x007d6c14) = c + 1;
    }
    *reinterpret_cast<std::uint32_t*>(0x007d6af0) = param_1;
    return 1;
}

RH_ScopedInstall(RenderState_SetOpcode34, 0x004d7200);

// ---------------------------------------------------------------------------
// RenderState::SetOpcode35 — 0x004d7250
//
// Original: FUN_004d7250 (0x004d7250..0x004d729d)
// Signature: undefined4 FUN_004d7250(int param_1)
//
// Byte-verified body (cache 0x007d6af4, table 0x005d8d28 stride 4, value-store
// DAT_007d59a0, dirty flag DAT_007d59a4, queued opcode 0x35):
//   8b 4c 24 04                 mov  ecx, [esp+4]
//   a1 f4 6a 7d 00              mov  eax, [0x007d6af4]   ; cache
//   3b c8 / 74 3b               cmp ecx,eax ; je ret
//   8b 04 8d 28 8d 5d 00        mov  eax, [ecx*4 + 0x005d8d28]   ; table[param_1]
//   a3 a0 59 7d 00              mov  [0x007d59a0], eax
//   a1 a4 59 7d 00              mov  eax, [0x007d59a4]    ; dirty flag
//   85 c0 / 75 20               test eax,eax ; jne skip
//   a1 14 6c 7d 00              mov  eax, [0x007d6c14]
//   c7 05 a4 59 7d 00 01..      mov  [0x007d59a4], 1
//   c7 04 85 68 51 7d 00 35..   mov  [0x007d5168+eax*4], 0x35
//   40 / a3 14 6c 7d 00         inc eax ; mov [0x007d6c14], eax
//   89 0d f4 6a 7d 00           mov  [0x007d6af4], ecx    ; cache = param_1
//   b8 01 00 00 00 / c3         mov eax,1 ; ret
//
// Table 0x005d8d28 is shared with SetOpcode36/SetOpcode37. Pure leaf. Caller:
// FUN_004d7480.
// ---------------------------------------------------------------------------

// 0x004d7250
extern "C" __declspec(dllexport) std::uint32_t __cdecl RenderState_SetOpcode35(std::uint32_t param_1)
{
    if (param_1 == *reinterpret_cast<std::uint32_t*>(0x007d6af4)) return 1;
    *reinterpret_cast<std::uint32_t*>(0x007d59a0) = *reinterpret_cast<std::uint32_t*>(0x005d8d28 + param_1 * 4);
    if (*reinterpret_cast<std::uint32_t*>(0x007d59a4) == 0) {
        std::uint32_t c = *reinterpret_cast<std::uint32_t*>(0x007d6c14);
        *reinterpret_cast<std::uint32_t*>(0x007d59a4) = 1;
        reinterpret_cast<std::uint32_t*>(0x007d5168)[c] = 0x35;
        *reinterpret_cast<std::uint32_t*>(0x007d6c14) = c + 1;
    }
    *reinterpret_cast<std::uint32_t*>(0x007d6af4) = param_1;
    return 1;
}

RH_ScopedInstall(RenderState_SetOpcode35, 0x004d7250);

// ---------------------------------------------------------------------------
// RenderState::SetOpcode36 — 0x004d72a0
//
// Original: FUN_004d72a0 (0x004d72a0..0x004d72ed)
// Signature: undefined4 FUN_004d72a0(int param_1)
//
// Byte-verified twin of SetOpcode35: cache 0x007d6af8, table 0x005d8d28 stride
// 4, value-store DAT_007d59a8, dirty flag DAT_007d59ac, queued opcode 0x36.
//   ...
//   8b 04 8d 28 8d 5d 00        mov  eax, [ecx*4 + 0x005d8d28]
//   a3 a8 59 7d 00              mov  [0x007d59a8], eax
//   c7 04 85 68 51 7d 00 36..   mov  [0x007d5168+eax*4], 0x36
//   89 0d f8 6a 7d 00           mov  [0x007d6af8], ecx
//   b8 01 00 00 00 / c3
// Pure leaf. Caller: FUN_004d7480.
// ---------------------------------------------------------------------------

// 0x004d72a0
extern "C" __declspec(dllexport) std::uint32_t __cdecl RenderState_SetOpcode36(std::uint32_t param_1)
{
    if (param_1 == *reinterpret_cast<std::uint32_t*>(0x007d6af8)) return 1;
    *reinterpret_cast<std::uint32_t*>(0x007d59a8) = *reinterpret_cast<std::uint32_t*>(0x005d8d28 + param_1 * 4);
    if (*reinterpret_cast<std::uint32_t*>(0x007d59ac) == 0) {
        std::uint32_t c = *reinterpret_cast<std::uint32_t*>(0x007d6c14);
        *reinterpret_cast<std::uint32_t*>(0x007d59ac) = 1;
        reinterpret_cast<std::uint32_t*>(0x007d5168)[c] = 0x36;
        *reinterpret_cast<std::uint32_t*>(0x007d6c14) = c + 1;
    }
    *reinterpret_cast<std::uint32_t*>(0x007d6af8) = param_1;
    return 1;
}

RH_ScopedInstall(RenderState_SetOpcode36, 0x004d72a0);

// ---------------------------------------------------------------------------
// RenderState::SetOpcode37 — 0x004d72f0
//
// Original: FUN_004d72f0 (0x004d72f0..0x004d733d)
// Signature: undefined4 FUN_004d72f0(int param_1)
//
// Byte-verified twin of SetOpcode35: cache 0x007d6afc, table 0x005d8d28 stride
// 4, value-store DAT_007d59b0, dirty flag DAT_007d59b4, queued opcode 0x37.
//   8b 04 8d 28 8d 5d 00        mov  eax, [ecx*4 + 0x005d8d28]
//   a3 b0 59 7d 00              mov  [0x007d59b0], eax
//   c7 04 85 68 51 7d 00 37..   mov  [0x007d5168+eax*4], 0x37
//   89 0d fc 6a 7d 00           mov  [0x007d6afc], ecx
//   b8 01 00 00 00 / c3
// Pure leaf. Caller: FUN_004d7480.
// ---------------------------------------------------------------------------

// 0x004d72f0
extern "C" __declspec(dllexport) std::uint32_t __cdecl RenderState_SetOpcode37(std::uint32_t param_1)
{
    if (param_1 == *reinterpret_cast<std::uint32_t*>(0x007d6afc)) return 1;
    *reinterpret_cast<std::uint32_t*>(0x007d59b0) = *reinterpret_cast<std::uint32_t*>(0x005d8d28 + param_1 * 4);
    if (*reinterpret_cast<std::uint32_t*>(0x007d59b4) == 0) {
        std::uint32_t c = *reinterpret_cast<std::uint32_t*>(0x007d6c14);
        *reinterpret_cast<std::uint32_t*>(0x007d59b4) = 1;
        reinterpret_cast<std::uint32_t*>(0x007d5168)[c] = 0x37;
        *reinterpret_cast<std::uint32_t*>(0x007d6c14) = c + 1;
    }
    *reinterpret_cast<std::uint32_t*>(0x007d6afc) = param_1;
    return 1;
}

RH_ScopedInstall(RenderState_SetOpcode37, 0x004d72f0);

// ---------------------------------------------------------------------------
// RenderState::SetOpcode38 — 0x004d7340
//
// Original: FUN_004d7340 (0x004d7340..0x004d738d)
// Signature: undefined4 FUN_004d7340(int param_1)
//
// Byte-verified: same cache/dirty/queue pattern but a DIFFERENT source table at
// 0x005d8d4c (stride 4, = 0x005d8d28 + 0x24). cache 0x007d6b00, value-store
// DAT_007d59b8, dirty flag DAT_007d59bc, queued opcode 0x38.
//   8b 4c 24 04
//   a1 00 6b 7d 00              mov  eax, [0x007d6b00]   ; cache
//   3b c8 / 74 3b
//   8b 04 8d 4c 8d 5d 00        mov  eax, [ecx*4 + 0x005d8d4c]   ; table[param_1]
//   a3 b8 59 7d 00              mov  [0x007d59b8], eax
//   a1 bc 59 7d 00              mov  eax, [0x007d59bc]    ; dirty flag
//   85 c0 / 75 20
//   c7 05 bc 59 7d 00 01..      mov  [0x007d59bc], 1
//   c7 04 85 68 51 7d 00 38..   mov  [0x007d5168+eax*4], 0x38
//   89 0d 00 6b 7d 00           mov  [0x007d6b00], ecx
//   b8 01 00 00 00 / c3
// Pure leaf. Caller: FUN_004d7480.
// ---------------------------------------------------------------------------

// 0x004d7340
extern "C" __declspec(dllexport) std::uint32_t __cdecl RenderState_SetOpcode38(std::uint32_t param_1)
{
    if (param_1 == *reinterpret_cast<std::uint32_t*>(0x007d6b00)) return 1;
    *reinterpret_cast<std::uint32_t*>(0x007d59b8) = *reinterpret_cast<std::uint32_t*>(0x005d8d4c + param_1 * 4);
    if (*reinterpret_cast<std::uint32_t*>(0x007d59bc) == 0) {
        std::uint32_t c = *reinterpret_cast<std::uint32_t*>(0x007d6c14);
        *reinterpret_cast<std::uint32_t*>(0x007d59bc) = 1;
        reinterpret_cast<std::uint32_t*>(0x007d5168)[c] = 0x38;
        *reinterpret_cast<std::uint32_t*>(0x007d6c14) = c + 1;
    }
    *reinterpret_cast<std::uint32_t*>(0x007d6b00) = param_1;
    return 1;
}

RH_ScopedInstall(RenderState_SetOpcode38, 0x004d7340);
