// Mashed RE — RenderWare D3D8 render-state cached dirty-queue setters (cluster B).
// c3-batch-sgr1 session 2.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file (all 5 cdecl int(int param_1), pure leaves, return 1):
//   0x004d7390  RenderStateSetOpcode39      — raw-store setter,    dirty-queue opcode 0x39
//   0x004d73e0  RenderStateSetOpcode3a      — raw-store setter,    dirty-queue opcode 0x3a
//   0x004d7430  RenderStateSetOpcode3b      — raw-store setter,    dirty-queue opcode 0x3b
//   0x004d7bc0  TextureStateSetFilterMode   — table-lookup setter, dirty-queue opcode 0x09
//   0x004d7c10  TextureStateSetAddressMode  — table-lookup setter, dirty-queue opcode 0x16
//
// All five share ONE shared dirty-state byte-queue and write index:
//   queue base  0x007d5168  (uint32 array; slot = queue[idx], stride 4 via `eax*4`)
//   write index 0x007d6c14  (DAT_007d6c14)
//
// Common shape (verified by capstone disasm of MASHED.exe.unpatched):
//   if (param_1 != *cache) {              // change-detect gate
//       *store = <value>;                 // raw param_1, or table[param_1] (lookup variants)
//       if (*dirty == 0) {                // one-shot enqueue guard
//           idx = *0x007d6c14;
//           *dirty = 1;
//           ((uint32*)0x007d5168)[idx] = <opcode>;
//           *0x007d6c14 = idx + 1;
//       }
//       *cache = param_1;
//   }
//   return 1;
// The value-store happens on every cache-MISS; the enqueue only when *dirty==0;
// the cache update on every cache-miss; a cache-HIT writes nothing.
//
// Analysis notes (per-RVA C1->C2 plates):
//   re/analysis/render_6_c1_to_c2_s2/004d7390.md   (U-5338)
//   re/analysis/render_6_c1_to_c2_s2/004d73e0.md   (U-5339)
//   re/analysis/render_6_c1_to_c2_s3/004d7430.md   (U-5350)
//   re/analysis/render_6_c1_to_c2_s3/004d7bc0.md   (U-5353)
//   re/analysis/render_6_c1_to_c2_s3/004d7c10.md   (U-5354)
//
// Sibling 0x004d7100 (same cluster/queue) is the C3 precedent; verified non-degen
// via early_window_leaf_diff.py + arg_type seed_globals_arg_multiobs (seeds the
// cache so both Orig and Reimpl execute the full body). The plain
// void_setter_observe lane is DEGENERATE for these (Orig primes *cache, Reimpl
// then cache-hits and no-ops) — see c3-batch-sgr1-s2 promotion notes.

#include "../Core/HookSystem.h"

#include <cstdint>

namespace {

// Shared dirty-state byte queue + write index (cited above).
inline std::uint32_t* QueueBase()  { return reinterpret_cast<std::uint32_t*>(0x007d5168u); }
inline std::uint32_t* QueueIndex() { return reinterpret_cast<std::uint32_t*>(0x007d6c14u); }

} // namespace

// ---------------------------------------------------------------------------
// RenderStateSetOpcode39  --  0x004d7390
//
// Original: FUN_004d7390 (0x004d7390..0x004d73d7). undefined4 FUN(int param_1).
// Disasm (MASHED.exe.unpatched):
//   0x004d7394  mov eax,[0x7d6b04]          ; cache
//   0x004d7399  cmp ecx,eax / je end        ; change-detect
//   0x004d73a2  mov [0x7d59c0],ecx          ; store raw param_1 (NO table lookup)
//   0x004d73a8  test [0x7d59c4] / jne skip  ; dirty guard
//   0x004d73ac  mov eax,[0x7d6c14]          ; idx
//   0x004d73b1  mov [0x7d59c4],1            ; dirty = 1
//   0x004d73bb  mov [eax*4+0x7d5168],0x39   ; enqueue opcode 0x39
//   0x004d73c7  mov [0x7d6c14],eax+1        ; idx++
//   0x004d73cc  mov [0x7d6b04],ecx          ; cache = param_1
//   0x004d73d2  mov eax,1 / ret
//
// Globals: cache 0x007d6b04, store 0x007d59c0, dirty 0x007d59c4. ref: 004d7390.md
// ---------------------------------------------------------------------------

// 0x004d7390
extern "C" __declspec(dllexport) std::uint32_t __cdecl RenderStateSetOpcode39(int param_1)
{
    if (param_1 != *reinterpret_cast<int*>(0x007d6b04u)) {            // 0x004d7399 cache check
        *reinterpret_cast<std::uint32_t*>(0x007d59c0u) =             // 0x004d73a2 raw store
            static_cast<std::uint32_t>(param_1);
        if (*reinterpret_cast<std::uint32_t*>(0x007d59c4u) == 0u) {  // 0x004d73a8 dirty guard
            std::uint32_t idx = *QueueIndex();                       // 0x004d73ac
            *reinterpret_cast<std::uint32_t*>(0x007d59c4u) = 1u;     // 0x004d73b1 dirty = 1
            QueueBase()[idx] = 0x39u;                                // 0x004d73bb opcode 0x39
            *QueueIndex() = idx + 1u;                                // 0x004d73c7 idx++
        }
        *reinterpret_cast<std::uint32_t*>(0x007d6b04u) =            // 0x004d73cc cache = param_1
            static_cast<std::uint32_t>(param_1);
    }
    return 1u;                                                       // 0x004d73d2
}

RH_ScopedInstall(RenderStateSetOpcode39, 0x004d7390);

// ---------------------------------------------------------------------------
// RenderStateSetOpcode3a  --  0x004d73e0
//
// Original: FUN_004d73e0 (0x004d73e0..0x004d7427). Sibling of 0x004d7390 with
// cache 0x007d6b08, store 0x007d59c8, dirty 0x007d59cc, opcode 0x3a.
// Disasm cites: 0x004d73f2 mov [0x7d59c8],ecx ; 0x004d740b mov [eax*4+0x7d5168],0x3a ;
//   0x004d741c mov [0x7d6b08],ecx. ref: 004d73e0.md
// ---------------------------------------------------------------------------

// 0x004d73e0
extern "C" __declspec(dllexport) std::uint32_t __cdecl RenderStateSetOpcode3a(int param_1)
{
    if (param_1 != *reinterpret_cast<int*>(0x007d6b08u)) {            // 0x004d73e9
        *reinterpret_cast<std::uint32_t*>(0x007d59c8u) =             // 0x004d73f2 raw store
            static_cast<std::uint32_t>(param_1);
        if (*reinterpret_cast<std::uint32_t*>(0x007d59ccu) == 0u) {  // 0x004d73f8 dirty guard
            std::uint32_t idx = *QueueIndex();                       // 0x004d73fc
            *reinterpret_cast<std::uint32_t*>(0x007d59ccu) = 1u;     // 0x004d7401 dirty = 1
            QueueBase()[idx] = 0x3au;                                // 0x004d740b opcode 0x3a
            *QueueIndex() = idx + 1u;                                // 0x004d7417 idx++
        }
        *reinterpret_cast<std::uint32_t*>(0x007d6b08u) =            // 0x004d741c cache = param_1
            static_cast<std::uint32_t>(param_1);
    }
    return 1u;                                                       // 0x004d7422
}

RH_ScopedInstall(RenderStateSetOpcode3a, 0x004d73e0);

// ---------------------------------------------------------------------------
// RenderStateSetOpcode3b  --  0x004d7430
//
// Original: FUN_004d7430 (0x004d7430..0x004d7477). Sibling with cache 0x007d6b0c,
// store 0x007d59d0, dirty 0x007d59d4, opcode 0x3b.
// Disasm cites: 0x004d7442 mov [0x7d59d0],ecx ; 0x004d745b mov [eax*4+0x7d5168],0x3b ;
//   0x004d746c mov [0x7d6b0c],ecx. ref: 004d7430.md
// ---------------------------------------------------------------------------

// 0x004d7430
extern "C" __declspec(dllexport) std::uint32_t __cdecl RenderStateSetOpcode3b(int param_1)
{
    if (param_1 != *reinterpret_cast<int*>(0x007d6b0cu)) {            // 0x004d7439
        *reinterpret_cast<std::uint32_t*>(0x007d59d0u) =             // 0x004d7442 raw store
            static_cast<std::uint32_t>(param_1);
        if (*reinterpret_cast<std::uint32_t*>(0x007d59d4u) == 0u) {  // 0x004d7448 dirty guard
            std::uint32_t idx = *QueueIndex();                       // 0x004d744c
            *reinterpret_cast<std::uint32_t*>(0x007d59d4u) = 1u;     // 0x004d7451 dirty = 1
            QueueBase()[idx] = 0x3bu;                                // 0x004d745b opcode 0x3b
            *QueueIndex() = idx + 1u;                                // 0x004d7467 idx++
        }
        *reinterpret_cast<std::uint32_t*>(0x007d6b0cu) =            // 0x004d746c cache = param_1
            static_cast<std::uint32_t>(param_1);
    }
    return 1u;                                                       // 0x004d7472
}

RH_ScopedInstall(RenderStateSetOpcode3b, 0x004d7430);

// ---------------------------------------------------------------------------
// TextureStateSetFilterMode  --  0x004d7bc0
//
// Original: FUN_004d7bc0 (0x004d7bc0..0x004d7c0d). TABLE-LOOKUP variant.
// Disasm:
//   0x004d7bc4  mov eax,[0x7d6b2c]              ; cache
//   0x004d7bc9  cmp ecx,eax / je end
//   0x004d7bcd  mov eax,[ecx*4+0x5d8c48]        ; table lookup (dword stride)
//   0x004d7bd4  mov [0x7d5840],eax              ; store mapped value
//   0x004d7bde  test [0x7d5844] / jne skip      ; dirty guard
//   0x004d7be7  mov [0x7d5844],1                ; dirty = 1
//   0x004d7bf1  mov [eax*4+0x7d5168],9          ; enqueue opcode 0x09
//   0x004d7bfd  mov [0x7d6c14],idx+1            ; idx++
//   0x004d7c02  mov [0x7d6b2c],ecx              ; cache = param_1
//   0x004d7c08  mov eax,1 / ret
//
// Table 0x005d8c48 is image-resident .rdata (RVA 0x1cc000..0x1e9044): entries
// [0..11] = {0,1,2,0,3,1,2,0,1,2,3,4}. Globals: cache 0x007d6b2c, store
// 0x007d5840, dirty 0x007d5844. ref: 004d7bc0.md
// ---------------------------------------------------------------------------

// 0x004d7bc0
extern "C" __declspec(dllexport) std::uint32_t __cdecl TextureStateSetFilterMode(int param_1)
{
    if (param_1 != *reinterpret_cast<int*>(0x007d6b2cu)) {            // 0x004d7bc9
        // 0x004d7bcd table lookup: ((uint32*)0x005d8c48)[param_1]
        std::uint32_t mapped = reinterpret_cast<std::uint32_t*>(0x005d8c48u)[param_1];
        *reinterpret_cast<std::uint32_t*>(0x007d5840u) = mapped;     // 0x004d7bd4 store mapped
        if (*reinterpret_cast<std::uint32_t*>(0x007d5844u) == 0u) {  // 0x004d7bde dirty guard
            std::uint32_t idx = *QueueIndex();                       // 0x004d7be2
            *reinterpret_cast<std::uint32_t*>(0x007d5844u) = 1u;     // 0x004d7be7 dirty = 1
            QueueBase()[idx] = 0x9u;                                 // 0x004d7bf1 opcode 0x09
            *QueueIndex() = idx + 1u;                                // 0x004d7bfd idx++
        }
        *reinterpret_cast<std::uint32_t*>(0x007d6b2cu) =            // 0x004d7c02 cache = param_1
            static_cast<std::uint32_t>(param_1);
    }
    return 1u;                                                       // 0x004d7c08
}

RH_ScopedInstall(TextureStateSetFilterMode, 0x004d7bc0);

// ---------------------------------------------------------------------------
// TextureStateSetAddressMode  --  0x004d7c10
//
// Original: FUN_004d7c10 (0x004d7c10..0x004d7c5d). TABLE-LOOKUP sibling of
// 0x004d7bc0 with table 0x005d8d18, cache 0x007d6b18, store 0x007d58a8,
// dirty 0x007d58ac, opcode 0x16.
// Disasm cites: 0x004d7c1d mov eax,[ecx*4+0x5d8d18] ; 0x004d7c24 mov [0x7d58a8],eax ;
//   0x004d7c41 mov [eax*4+0x7d5168],0x16 ; 0x004d7c52 mov [0x7d6b18],ecx.
// Table 0x005d8d18 .rdata: [0..11] = {0,1,2,3,0,1,2,3,4,5,6,7}. ref: 004d7c10.md
// ---------------------------------------------------------------------------

// 0x004d7c10
extern "C" __declspec(dllexport) std::uint32_t __cdecl TextureStateSetAddressMode(int param_1)
{
    if (param_1 != *reinterpret_cast<int*>(0x007d6b18u)) {            // 0x004d7c19
        // 0x004d7c1d table lookup: ((uint32*)0x005d8d18)[param_1]
        std::uint32_t mapped = reinterpret_cast<std::uint32_t*>(0x005d8d18u)[param_1];
        *reinterpret_cast<std::uint32_t*>(0x007d58a8u) = mapped;     // 0x004d7c24 store mapped
        if (*reinterpret_cast<std::uint32_t*>(0x007d58acu) == 0u) {  // 0x004d7c29 dirty guard
            std::uint32_t idx = *QueueIndex();                       // 0x004d7c32
            *reinterpret_cast<std::uint32_t*>(0x007d58acu) = 1u;     // 0x004d7c37 dirty = 1
            QueueBase()[idx] = 0x16u;                                // 0x004d7c41 opcode 0x16
            *QueueIndex() = idx + 1u;                                // 0x004d7c4d idx++
        }
        *reinterpret_cast<std::uint32_t*>(0x007d6b18u) =            // 0x004d7c52 cache = param_1
            static_cast<std::uint32_t>(param_1);
    }
    return 1u;                                                       // 0x004d7c58
}

RH_ScopedInstall(TextureStateSetAddressMode, 0x004d7c10);
