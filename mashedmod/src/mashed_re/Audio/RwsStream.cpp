// Mashed RE — RWS audio stream/header reader + tree-walk cluster (c3-batch-i-s1).
//
// Cluster of 4 reimplementations promoted C2 -> C3 in c3-batch-i-s1:
//   0x005ab380  AudioRwsChunkHeaderRead        — 12-byte chunk header reader + version decode
//   0x005ab410  AudioRwsChunkTypeSeek          — chunk-type seek loop with version-range gate
//   0x005abf80  AudioWaveVtableSlot1cDispatch  — vtable+0x1c indirect call (unrecovered jump)
//   0x005aa0c0  AudioTreeWalkPredicateSearch   — recursive predicate tree-walk
//
// (0x005aea00 is **NOT** included in this cluster — U-0125 explicitly marks
//  "Blocks: C3" in UNCERTAINTIES.md; refused per the no-overclaiming rule.)
//
// Source analysis:
//   re/analysis/promote_c2_audio_rws/005ab380.md
//   re/analysis/promote_c2_audio_rws/005ab410.md
//   re/analysis/promote_c2_audio_rws/005abf80.md
//   re/analysis/audio_rws_loader_d3/005aa0c0.md
//
#include "../Core/HookSystem.h"
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ab380  FUN_005ab380  AudioRwsChunkHeaderRead   (~80 bytes)
//
// undefined4 FUN_005ab380(undefined4 param_1, undefined4 *param_2)
//   param_1 = stream handle (passed through unchanged on success)
//   param_2 = output chunk-header struct (5 × uint32 = 20 bytes)
//
// Reads 12 bytes from the stream via FUN_004cbd30(stream, &localbuf, 0xc).
// On short read (< 0xc bytes), returns 0.  Otherwise:
//   out[+0x00] = chunk type   (raw u32 at bytes 0..3)
//   out[+0x04] = chunk size   (raw u32 at bytes 4..7)
//   Version decode (raw u32 at bytes 8..11 = local_4):
//     - if (local_4 & 0xffff0000) == 0  (old-format version):
//         out[+0x08] = local_4 << 8
//         out[+0x0c] = 0
//     - else  (new-format version: library+build packed):
//         out[+0x08] = (local_4 >> 0xe & 0x3ff00) + 0x30000 | local_4 >> 0x10 & 0x3f
//         out[+0x0c] = local_4 & 0xffff      (build number, low 16 bits)
//   out[+0x10] = 0  (always cleared in both paths)
//
// Returns param_1 (stream) on success, 0 on short-read.
//
// Constants cited from analysis note:
//   0xc          (byte count, passed to FUN_004cbd30)
//   0xffff0000   (mask to detect old vs new format version word)
//   0x3ff00, 0x30000, 0x3f, 0xe, 0x10  (version field decode)
//
// Callee: FUN_004cbd30 at 0x004cbd30 (RwStreamRead — C2, cross-subsystem).
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl
AudioRwsChunkHeaderRead(std::uint32_t param_1, std::uint32_t* param_2)
{
    typedef int (__cdecl *StreamReadFn)(std::uint32_t, void*, int);
    static constexpr std::uintptr_t kStreamRead = 0x004cbd30u;
    const auto streamRead = reinterpret_cast<StreamReadFn>(kStreamRead);

    std::uint32_t local_c;   // chunk type   (bytes 0..3)
    std::uint32_t local_8;   // chunk size   (bytes 4..7)
    std::uint32_t local_4;   // raw version word (bytes 8..11)

    const int bytes = streamRead(param_1, &local_c, 0xc);
    if (bytes != 0xc) {
        return 0u;
    }

    param_2[1] = local_8;          // out[+0x04] = chunk size
    param_2[0] = local_c;          // out[+0x00] = chunk type

    if ((local_4 & 0xffff0000u) == 0u) {
        // Old-format: no library/build fields.
        param_2[2] = local_4 << 8;  // out[+0x08]
        param_2[3] = 0u;            // out[+0x0c]
        param_2[4] = 0u;            // out[+0x10]
        return param_1;
    }
    // New-format: library + build packed.
    param_2[4] = 0u;                // out[+0x10] = 0 (first, matches decomp order)
    param_2[2] = ((local_4 >> 0xeu) & 0x3ff00u) + 0x30000u
                 | ((local_4 >> 0x10u) & 0x3fu);     // out[+0x08]
    param_2[3] = local_4 & 0xffffu;                  // out[+0x0c] = build number
    return param_1;
}

RH_ScopedInstall(AudioRwsChunkHeaderRead, 0x005ab380);  // re-enabled 2026-05-24 c3-audio-a

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ab410  FUN_005ab410  AudioRwsChunkTypeSeek    (~140 bytes)
//
// undefined4 FUN_005ab410(undefined4 param_1, int param_2,
//                         undefined4 *param_3, uint *param_4)
//   param_1 = stream
//   param_2 = target chunk type
//   param_3 = output: chunk size (if non-NULL)
//   param_4 = output: decoded RW version (if non-NULL)
//
// Reads first header via FUN_005ab380(stream, &local_14, 0).  On success, loops
// while local_14 != param_2:
//   - FUN_004cc050(stream, local_10)  — skip chunk payload (seek by size)
//   - FUN_005ab380(stream, &local_14, 0)  — read next header
//   - any failure (0 return) → return 0
// After exiting the loop (target type found), if local_c (decoded version)
// is in (0x34fff, 0x37003) — i.e. 0x35000..0x37002 inclusive — write outputs
// and return 1.  Otherwise return 0.
//
// Constants:
//   0x34fff  (lower bound, exclusive — at 0x005ab410 body, version range gate)
//   0x37003  (upper bound, exclusive)
//
// Callees:
//   FUN_005ab380 at 0x005ab380  (header reader — c3-batch-i-s1 above)
//   FUN_004cc050 at 0x004cc050  (RwStreamSkip — C2, cross-subsystem)
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl
AudioRwsChunkTypeSeek(std::uint32_t param_1, int param_2,
                      std::uint32_t* param_3, std::uint32_t* param_4)
{
    typedef std::uint32_t (__cdecl *HdrReadFn)(std::uint32_t, std::uint32_t*);
    typedef int           (__cdecl *StreamSkipFn)(std::uint32_t, std::uint32_t);

    static constexpr std::uintptr_t kHdrRead    = 0x005ab380u;
    static constexpr std::uintptr_t kStreamSkip = 0x004cc050u;

    const auto hdrRead    = reinterpret_cast<HdrReadFn>(kHdrRead);
    const auto streamSkip = reinterpret_cast<StreamSkipFn>(kStreamSkip);

    // Local header buffer matches decomp: local_14 (type), local_10 (size),
    // local_c (decoded version), plus two trailing slots written by hdrRead.
    int           local_14;
    std::uint32_t local_10;
    std::uint32_t local_c;
    std::uint32_t local_8;
    std::uint32_t local_4;
    std::uint32_t* const hdrBuf = reinterpret_cast<std::uint32_t*>(&local_14);

    std::uint32_t r = hdrRead(param_1, hdrBuf);
    if (r != 0u) {
        while (local_14 != param_2) {
            // Skip payload (seek by size).
            const int skipOk = streamSkip(param_1, local_10);
            if (skipOk == 0) return 0u;
            // Read next header.
            r = hdrRead(param_1, hdrBuf);
            if (r == 0u) return 0u;
        }
        // Version range gate: 0x34fff < local_c < 0x37003.
        if ((0x34fffu < local_c) && (local_c < 0x37003u)) {
            if (param_3 != nullptr) *param_3 = local_10;
            if (param_4 != nullptr) *param_4 = local_c;
            return 1u;
        }
    }
    return 0u;
    // Silence unused-warning for trailing-slot locals (matches decomp layout).
    (void)local_8; (void)local_4;
}

RH_ScopedInstall(AudioRwsChunkTypeSeek, 0x005ab410);  // re-enabled 2026-05-24 c3-audio-a

// ─────────────────────────────────────────────────────────────────────────────
// 0x005abf80  FUN_005abf80  AudioWaveVtableSlot1cDispatch  (~16 bytes)
//
// void FUN_005abf80(int param_1)
//   param_1 = wave_node pointer
//
// Pure vtable-dispatch trampoline:
//   uVar1 = *(code **)(*(int *)(param_1 + 0xc) + 0x1c);
//   if (uVar1 != NULL) (*uVar1)();
//
// Ghidra warning at 0x005abf8f: "Could not recover jumptable at 0x005abf8f.
// Too many branches" — the indirect call's target is dispatch-dependent.
//
// U-0998: decompiler marks return type void due to indirect-jump recovery
// failure; callers (FUN_005abd30, FUN_005abfa0) consume the result in a
// boolean while-loop, but per cataloged U-0998 (Blocks: none) the void
// signature is accepted for C3.
//
// Memory accesses:
//   *(param_1 + 0x0c)        — context pointer (vtable base)
//   *(*(param_1+0x0c) + 0x1c) — function pointer at vtable slot +0x1c
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl
AudioWaveVtableSlot1cDispatch(int param_1)
{
    typedef void (__cdecl *VtableFn)(void);

    const int  contextPtr = *reinterpret_cast<int*>(param_1 + 0x0c);
    const auto fn         = *reinterpret_cast<VtableFn*>(contextPtr + 0x1cu);

    if (fn != nullptr) {
        fn();
    }
}

RH_ScopedInstall(AudioWaveVtableSlot1cDispatch, 0x005abf80);  // re-enabled 2026-05-24 c3-audio-a

// ─────────────────────────────────────────────────────────────────────────────
// 0x005aa0c0  FUN_005aa0c0  AudioTreeWalkPredicateSearch  (~100 bytes)
//
// undefined4* FUN_005aa0c0(undefined4 *param_1, int *param_2, code *param_3,
//                          undefined4 param_4, int param_5)
//   param_1 = tree node (NULL → use global root at DAT_009146fc)
//   param_2 = output: parent pointer (written on match, written 0 on miss)
//   param_3 = predicate callback (returns 1 on match, 0 otherwise)
//   param_4 = user data passed to predicate
//   param_5 = if 1, test root self before walking children
//
// Recursive depth-first walk with predicate callback:
//   1. NULL fallback: if param_1 == 0, substitute DAT_009146fc.
//   2. Root self-test: if param_5 == 1 and (*param_3)(root, root, p4) == 1,
//        return root (writing root to *param_2 first).
//   3. Iterate sibling list at param_1[4] until end (== param_1 + 4):
//        node = childptr - 6 (raw u32 word offset == -0x18 bytes)
//        if (*param_3)(root, node, p4) == 1: return node, write root to *param_2
//        else recurse FUN_005aa0c0(node, p2, p3, p4, 0) and bubble non-NULL.
//   4. If exhausted: write 0 to *param_2 (if non-NULL) and return NULL.
//
// Global: DAT_009146fc at 0x009146fc (audio tree root, runtime-initialized).
// Tree link offset: children linked at node + 4*4 = node + 0x10.
// Node-from-childlink: childptr - 6 words = childptr - 0x18 bytes.
//
// No catalog blockers for this RVA itself (S-0985, D-2865 are tracker rows
// describing dependents, not unresolved structural uncertainties on this fn).
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t* __cdecl
AudioTreeWalkPredicateSearch(std::uint32_t* param_1, int* param_2,
                             void* param_3, std::uint32_t param_4,
                             int param_5)
{
    typedef int (__cdecl *PredFn)(std::uint32_t*, std::uint32_t*, std::uint32_t);

    // Cited in analysis note: global tree root at 0x009146fc.
    static constexpr std::uintptr_t kTreeRootGlobal = 0x009146fcu;

    if (param_1 == nullptr) {
        param_1 = *reinterpret_cast<std::uint32_t**>(kTreeRootGlobal);
    }

    const auto pred = reinterpret_cast<PredFn>(param_3);

    // Root self-test (only when param_5 == 1).
    if (param_5 == 1) {
        if (pred(param_1, param_1, param_4) == 1) {
            if (param_2 != nullptr) {
                *param_2 = reinterpret_cast<int>(param_1);
            }
            return param_1;
        }
    }

    // Iterate sibling list at param_1[4] until sentinel == param_1 + 4.
    std::uint32_t* puVar1   = reinterpret_cast<std::uint32_t*>(param_1[4]);
    std::uint32_t* sentinel = param_1 + 4;
    while (puVar1 != sentinel) {
        // node = childptr - 6 dwords (=-0x18 bytes).
        std::uint32_t* puVar3 = puVar1 - 6;

        if (pred(param_1, puVar3, param_4) == 1) {
            if (param_2 != nullptr) {
                *param_2 = reinterpret_cast<int>(param_1);
            }
            return puVar3;
        }

        // Recurse.
        std::uint32_t* result =
            AudioTreeWalkPredicateSearch(puVar3, param_2, param_3, param_4, 0);
        if (result != nullptr) {
            return result;
        }

        // Advance sibling pointer.
        puVar1 = reinterpret_cast<std::uint32_t*>(*puVar1);
    }

    // Exhausted: clear out-param and return NULL.
    if (param_2 != nullptr) {
        *param_2 = 0;
    }
    return nullptr;
}

RH_ScopedInstall(AudioTreeWalkPredicateSearch, 0x005aa0c0);  // re-enabled 2026-05-24 c3-audio-a
