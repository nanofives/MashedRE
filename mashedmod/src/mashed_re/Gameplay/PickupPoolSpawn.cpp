// Mashed RE — PickupPoolSpawn, the pickup-pool spawn predicate + writer.
// Original: 0x00458e00  FUN_00458e00  gameplay  C2 -> C3
// Plate: re/analysis/bucket_gameplay_00458a40_0045ac40/0x00458e00.md
//
// PURPOSE. Append one pickup to the 25-entry pickup pool and return its index,
// or return -1 when the spawn is rejected. Three rejections, in the original's
// own order: the pool is full; the position coincides with an entry already in
// the pool; the type is 0x15 (BLANK). On the rank-2 arm a BLANK is not rejected
// but REPLACED by a randomly chosen type and flagged at +0x28.
//
// Disassembly read from original\MASHED.exe.unpatched with capstone,
// 0x00458e00..0x00458f11 (274 bytes). The decompilation came from a read-only
// Ghidra pool clone (Mashed_pool0, analyzeHeadless -readOnly).
//
//   0x00458e00  push ebp
//   0x00458e01  call 0x42fe30                 ; rank = RaceEndFlagIfEndMode()
//   0x00458e06  mov  edx,[0x68b9a8]           ; pool count
//   0x00458e0c  cmp  edx,0x19
//   0x00458e0f  mov  ebp,eax                  ; ebp = rank
//   0x00458e11  jl   0x458e18                 ; SIGNED: count >= 25 -> reject
//   0x00458e13  or   eax,0xffffffff           ; return -1
//   0x00458e18  test edx,edx
//   0x00458e1c  mov  edi,[esp+0x10]           ; param_1 = const float* pos
//   0x00458e22  je   0x458e6b                 ; empty pool -> skip the dedupe
//   0x00458e24  lea  ecx,[edx+edx*4] / shl ecx,4 / add ecx,0x68b1c8
//   0x00458e30  fld  [ecx-0x54] / fsub [edi]          ; dx, walking DOWN
//   0x00458e38  fld  [ecx]      / fsub [edi+4]        ; dy
//   0x00458e3e  fld  [ecx+4]    / fsub [edi+8]        ; dz
//   0x00458e44  fld st(1)/fmul st(2)                  ; dy*dy
//   0x00458e48  fld st(3)/fmul st(4)/faddp st(1)      ; + dx*dx
//   0x00458e4e  fld st(1)/fmul st(2)/faddp st(1)      ; + dz*dz
//   0x00458e54  fcomp [0x5cc558]
//   0x00458e65  jnp  0x458e8f                 ; sum < eps -> return -1
//   0x00458e6b  mov  eax,[esp+0x14]           ; param_2 = type
//   0x00458e6f  lea  esi,[edx+edx*4]/shl esi,4/add esi,0x68b198   ; entry = &pool[count]
//   0x00458e7b  cmp  ebp,2
//   0x00458e7e  mov  [esi+0x28],0
//   0x00458e85  mov  [esi+0x24],eax
//   0x00458e88  jne  0x458e96                 ; rank != 2 -> the normal arm
//   0x00458e8a  cmp  eax,0x15 / je 0x458ea2   ; rank 2: ONLY a BLANK proceeds
//   0x00458e8f  ... return -1
//   0x00458e96  cmp  eax,0x15 / jne 0x458eb1  ; normal arm: a BLANK is rejected
//   0x00458ea2  call 0x458d00                 ; rank 2: random replacement type
//   0x00458ea7  mov  [esi+0x24],eax
//   0x00458eaa  mov  [esi+0x28],1
//   0x00458eb1  push esi / call 0x458dd0      ; re-skin the entry from its +0x24
//   0x00458eb7  mov  eax,[esi] / mov ecx,[eax+4] / push ecx / call 0x4c15c0
//   0x00458ec2  ... copy pos to +0x2c and to +0x38, VERBATIM (no lift, no clamp)
//   0x00458eea  mov  eax,[0x68b9a8] / inc eax
//   0x00458ef1  mov  [esi+0x20],1
//   0x00458ef8  mov  [esi+0x18],0x42f00000
//   0x00458eff  mov  [esi+0x1c],0
//   0x00458f07  mov  [0x68b9a8],eax
//   0x00458f0f  dec  eax                      ; return the index it was stored at
//
// CONSTANTS, each read out of the binary rather than taken from a gloss
// (memory `plate-hex-gloss-authoritative`, `audit-annotated-consts-against-the-binary`):
//   0x005cc558  6f 12 83 3a  = 0x3a83126f = 0.0010000000474974513f — a SQUARED
//                distance, compared against dy*dy + dx*dx + dz*dz.
//   0x42f00000  = 120.0f, written to +0x18.
//
// THE SUM ORDER IS THE x87 STACK'S, NOT THE DECOMPILER'S. Ghidra prints
// `dz*dz + dx*dx + dy*dy`; tracing the eight x87 ops above gives
// `(dy*dy + dx*dx) + dz*dz`, which is what is written below. The comparison is
// against 1e-3 so no ordering can change a verdict at any plausible separation,
// but the port says what the instructions say. This TU is NOT on
// mashedmod/x87_tus.txt: that list is measured, not reasoned, and nothing here
// has been measured (see the file's own header).
//
// ──────────────────────────────────────────────────────────────────────────────
// TUNNEL NEUTRALIZATION (the GameSaveBuffer.cpp pattern, re/CONFIDENCE.md L43+)
//
// This body is compiled into BOTH targets from ONE source file (it is listed in
// mashedmod/exe_sources.rsp and mashedmod/asi_sources.rsp), so `exe_file ==
// file` and the evidence covers the copy that ships. Only the two storage bases
// differ per build:
//   * .asi (macro NOT defined): MASHED's own globals, so the behaviour the
//     diff-original A/B measures is the original's.
//   * exe (/DMASHED_STANDALONE): private storage in this TU. 0x0068b198 is NOT
//     one of exe_main.cpp's kB17ReserveBases granules, so in the standalone it
//     lands inside mashed_re.exe's own 11.9 MB .data (MEM_COMMIT/MEM_IMAGE —
//     measured, exe_main.cpp:7630-7643). Writing 25 x 0x50 bytes blind there
//     would clobber our own data. The pool is OWN, not BIND: nothing else in the
//     standalone reads MASHED's pool, so a private array desyncs nothing.
//
// STUBS in the standalone arm (recorded in STUBS.md):
//   S-A  FUN_00458dd0 (0x00458dd0) + FUN_004c15c0 (0x004c15c0) are skipped
//        TOGETHER. 0x00458dd0 re-skins the entry's RenderWare object from its
//        just-written +0x24 type (FUN_00458630 -> FUN_004b4080 -> FUN_004e8090);
//        0x004c15c0 then resets the frame matrix at *(entry+0)+4 and links it
//        into the RwGlobals dirty list at DAT_007d3ff8+0xbc. The standalone
//        binds no RenderWare object to +0x00, so +0x00 is 0 and 0x004c15c0 would
//        dereference 0+4. They are a pair: skipping one and not the other is the
//        crash.
//   S-B  FUN_00458d00 (0x00458d00), the rank-2 random replacement type, is not
//        reachable in the standalone (see below) and is not called there.
//
// RANK IN THE STANDALONE. FUN_0042fe30 is C4 and ported
// (Frontend/MenuRaceEnd.cpp RaceEndFlagIfEndMode), so the standalone calls the
// port by name. It returns 1 when the sub-mode is 0xb and otherwise DAT_0067ea74
// — a zeroed tunnel in the standalone, so the standalone reads rank 0. Rank 0
// and rank 1 take the IDENTICAL path here: `cmp ebp,2` is the only read of the
// rank, so every value except 2 behaves the same. Live measurement on the
// original reads rank 1 on the race path (verify/pickups_fix_20261001/
// RESULT_STAGE1.md P1), so the standalone's arm is the arm the original runs.
//
// [UNCERTAIN U-9168] The rank-2 arm is NOT covered by any bit-identity evidence and is
// not claimed. FUN_00458d00 picks its type with FUN_00472690(0,8) at 0x00458d47
// — a random draw, so it can never appear in an A/B that demands bit-identity.
// What drives DAT_0067ea74 to 2 has not been derived either. Recorded as a
// pre-registered blocker (verify/retrofit_20261001/PREREG.md A-B1).
#include "PickupPoolSpawn.h"

#include "../Core/HookSystem.h"

#include <cstring>

#ifdef MASHED_STANDALONE
// The ported rank source, resolved BY NAME (no MASHED address survives).
extern "C" std::uint32_t __cdecl RaceEndFlagIfEndMode();
namespace {
// OWN — private pool storage. Sizes match the MASHED globals exactly.
alignas(4) unsigned char s_pool[mashed_re::Gameplay::kPickupPoolMax *
                                mashed_re::Gameplay::kPickupEntryStride];  // was 0x0068b198
std::int32_t             s_poolCount = 0;                                   // was 0x0068b9a8
}  // namespace
#else
namespace {
// 0x00458e01 — the rank call, kept as the original's absolute call.
typedef std::uint32_t(__cdecl* RankFn)();
typedef void(__cdecl* EntryFn)(void*);
typedef void(__cdecl* FrameFn)(std::uint32_t);
typedef std::int32_t(__cdecl* PickFn)();
}  // namespace
#endif

namespace mashed_re {
namespace Gameplay {

unsigned char* PickupPool_Base() {
#ifdef MASHED_STANDALONE
    return s_pool;
#else
    return reinterpret_cast<unsigned char*>(0x0068b198u);   // 0x00458e75
#endif
}

std::int32_t* PickupPool_CountPtr() {
#ifdef MASHED_STANDALONE
    return &s_poolCount;
#else
    return reinterpret_cast<std::int32_t*>(0x0068b9a8u);    // 0x00458e06
#endif
}

const float* PickupPool_EntryPos(std::int32_t index) {
    return reinterpret_cast<const float*>(PickupPool_Base() +
                                          index * kPickupEntryStride + 0x2c);
}

std::int32_t PickupPool_EntryType(std::int32_t index) {
    return *reinterpret_cast<const std::int32_t*>(
        PickupPool_Base() + index * kPickupEntryStride + 0x24);
}

}  // namespace Gameplay
}  // namespace mashed_re

// 0x00458e00
extern "C" __declspec(dllexport) std::int32_t __cdecl
PickupPoolSpawn(const float* pos, std::int32_t type) {
    using namespace mashed_re::Gameplay;

    unsigned char* const pool  = PickupPool_Base();
    std::int32_t* const  count = PickupPool_CountPtr();

    // 0x00458e01 — the rank is read FIRST, before the cap test, and kept in EBP
    // across every later branch.
#ifdef MASHED_STANDALONE
    const std::int32_t rank = static_cast<std::int32_t>(RaceEndFlagIfEndMode());
#else
    const std::int32_t rank =
        static_cast<std::int32_t>(reinterpret_cast<RankFn>(0x0042fe30u)());
#endif

    const std::int32_t n = *count;

    // 0x00458e0c/0x00458e11 — signed `cmp 0x19` + `jl`, so n >= 25 rejects.
    if (n >= kPickupPoolMax)
        return -1;                                           // 0x00458e13

    // 0x00458e18..0x00458e69 — dedupe against every entry already in the pool,
    // walking DOWN from n-1 to 0 and comparing the LIVE position at +0x2c.
    if (n != 0) {
        for (std::int32_t i = n - 1; i >= 0; --i) {
            const float* const e =
                reinterpret_cast<const float*>(pool + i * kPickupEntryStride + 0x2c);
            const float dx = e[0] - pos[0];                   // 0x00458e30/0x00458e36
            const float dy = e[1] - pos[1];                   // 0x00458e38/0x00458e3b
            const float dz = e[2] - pos[2];                   // 0x00458e3e/0x00458e41
            // x87 order, traced at 0x00458e44..0x00458e52: (dy*dy + dx*dx) + dz*dz.
            const float d2 = (dy * dy + dx * dx) + dz * dz;
            // 0x00458e54 fcomp [0x005cc558] = 0x3a83126f.
            if (d2 < 0.0010000000474974513f)
                return -1;                                    // 0x00458e65 -> 0x00458e8f
        }
    }

    // 0x00458e6f — entry = &pool[n].
    unsigned char* const entry = pool + n * kPickupEntryStride;

    *reinterpret_cast<std::int32_t*>(entry + 0x28) = 0;        // 0x00458e7e
    *reinterpret_cast<std::int32_t*>(entry + 0x24) = type;     // 0x00458e85

    if (rank == 2) {                                           // 0x00458e7b/0x00458e88
        // 0x00458e8a — on this arm ONLY a BLANK proceeds; anything else rejects.
        if (type != 0x15)
            return -1;                                         // 0x00458e8d -> 0x00458e8f
#ifdef MASHED_STANDALONE
        // STUB S-5716 -- FUN_00458d00 (0x00458d00), the rank-2 random replacement
        // type, is unreachable here: the standalone reads rank 0 (see the header
        // comment). Left as an explicit no-op rather than an invented draw.
#else
        const std::int32_t picked = reinterpret_cast<PickFn>(0x00458d00u)();
        *reinterpret_cast<std::int32_t*>(entry + 0x24) = picked;   // 0x00458ea7
        *reinterpret_cast<std::int32_t*>(entry + 0x28) = 1;        // 0x00458eaa
#endif
    } else if (type == 0x15) {                                 // 0x00458e96
        return -1;                                             // 0x00458e99 -> 0x00458e9d
    }

#ifndef MASHED_STANDALONE
    // STUB S-5714 + S-5715 in mashed_re.exe ONLY: this whole block is skipped
    // there. 0x00458eb1 re-skins the entry from the +0x24 type that was just
    // written (FUN_00458dd0), then 0x00458ebd resets the object's frame matrix
    // (FUN_004c15c0). They are a PAIR -- the standalone binds no RenderWare
    // object to +0x00, so FUN_004c15c0 would dereference 0+4.
    reinterpret_cast<EntryFn>(0x00458dd0u)(entry);                        // 0x00458eb2
    const std::uint32_t obj = *reinterpret_cast<std::uint32_t*>(entry);   // 0x00458eb7
    reinterpret_cast<FrameFn>(0x004c15c0u)(
        *reinterpret_cast<std::uint32_t*>(obj + 4));                      // 0x00458eb9
#endif

    // 0x00458ec2..0x00458f0c — the position is stored VERBATIM into BOTH the live
    // slot (+0x2c) and the anchor (+0x38). No lift, no snap, no clamp.
    float* const live   = reinterpret_cast<float*>(entry + 0x2c);
    float* const anchor = reinterpret_cast<float*>(entry + 0x38);
    live[0]   = pos[0];                                        // 0x00458ec9
    live[1]   = pos[1];                                        // 0x00458ece
    live[2]   = pos[2];                                        // 0x00458ed4
    anchor[0] = pos[0];                                        // 0x00458edc
    anchor[1] = pos[1];                                        // 0x00458ee4
    anchor[2] = pos[2];                                        // 0x00458f0c

    *reinterpret_cast<std::int32_t*>(entry + 0x20)  = 1;            // 0x00458ef1
    *reinterpret_cast<std::uint32_t*>(entry + 0x18) = 0x42f00000u;  // 0x00458ef8 (120.0f)
    *reinterpret_cast<std::int32_t*>(entry + 0x1c)  = 0;            // 0x00458eff

    *count = n + 1;                                            // 0x00458eea/0x00458f07
    return n;                                                  // 0x00458f0f
}

RH_ScopedInstall(PickupPoolSpawn, 0x00458e00);
