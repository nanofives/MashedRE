// Mashed RE — Frontend small-leaf reimplementations (c3-batch-t session 1).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x00408ad0  RaceScoreFloatGetBySlot    — 16B leaf; float getter from stride-0x30c array
//   0x00401570  EntryTableScanByKey        — 36B leaf; stride-0x68 entry-table scan, writes DAT_00636ac0
//   0x0040d250  DoubleDerefIndexedGetter   — 16B leaf; (**(PTR_PTR_005f2770 + DAT_0063ba7c*4))
//   0x0041e080  ScoreboardStateZeroInit    — 67B leaf; zero 11 globals + write 0x47 to one field
//   0x00414120  CopyTable005f2a70To0089a384 — 80B leaf; zero+copy 0x9c bytes
//
// Deferred from batch t s1 (documented in session output):
//   0x0040b290  FUN_0040b290  — circ-buffer side effects + global state writes
//   0x0040d590  FUN_0040d590  — calls FUN_00422fd0 / FUN_0040b290 (state mutators)
//   0x00401da0  FUN_00401da0  — RW transform pipeline side effects
//   0x004189f0  thunk_FUN_00419760 — 4-byte thunk; E9+rel32 install on 4B is unsafe
//   0x004224d0  FUN_004224d0  — slot init with ptr param + memset callee; non-trivial harness
//
// Analysis notes:
//   re/analysis/frontend_c1_to_c2_s1/00408ad0.md
//   re/analysis/frontend_c1_to_c2_s1/00401570.md
//   re/analysis/frontend_c1_to_c2_s1/0040d250.md
//   re/analysis/frontend_c1_to_c2_s1/0041e080.md
//   re/analysis/frontend_c1_to_c2_s1/00414120.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RaceScoreFloatGetBySlot  --  0x00408ad0
//
// Original: FUN_00408ad0 (16 bytes). float10 FUN_00408ad0(int param_1).
// Body: return (float10)*(float *)(&DAT_008a96ec + param_1 * 0x30c)
//   0x008a96ec base [0x00408ad3]; stride 0x30c [0x00408ad8]
// U-4200 (semantic meaning) non-blocking.
// ---------------------------------------------------------------------------

// 0x00408ad0
extern "C" __declspec(dllexport) float __cdecl RaceScoreFloatGetBySlot(int param_1)
{
    // Read 32-bit float from DAT_008a96ec + param_1 * 0x30c. [0x00408ad3]
    return *reinterpret_cast<float*>(
        0x008a96ecu + static_cast<unsigned>(param_1) * 0x30cu);
}

RH_ScopedInstall(RaceScoreFloatGetBySlot, 0x00408ad0);

// ---------------------------------------------------------------------------
// EntryTableScanByKey  --  0x00401570
//
// Original: FUN_00401570 (36 bytes). void FUN_00401570(int param_1).
//   DAT_00636ac0 = &DAT_00636578;                            // init to base
//   for (p = base; (int)p < 0x636ac0; p += 0x68)
//       if (*(int*)(p+0x38) == param_1) DAT_00636ac0 = p;    // last-match wins
//   0x00636578 base [0x00401573]; 0x00636ac0 out/bound [0x00401576/0x00401589];
//   index 0xe == +0x38 [0x0040157e]; stride 0x1a*4 = 0x68 [0x00401582]
// ---------------------------------------------------------------------------

// 0x00401570
extern "C" __declspec(dllexport) void __cdecl EntryTableScanByKey(int param_1)
{
    // Output pointer global; initialized to base of table. [0x00401576]
    auto& out_ptr = *reinterpret_cast<std::uint32_t**>(0x00636ac0u);

    constexpr std::uintptr_t table_base = 0x00636578u;  // [0x00401573]
    constexpr std::uintptr_t table_end  = 0x00636ac0u;  // exclusive [0x00401589]
    constexpr std::size_t    stride     = 0x68u;        // = 0x1a*4 [0x00401582]

    out_ptr = reinterpret_cast<std::uint32_t*>(table_base);

    for (std::uintptr_t p = table_base; p < table_end; p += stride) {
        // entry[+0x38] cited at 0x0040157e (index 0xe in undefined4 units).
        std::int32_t key = *reinterpret_cast<std::int32_t*>(p + 0x38u);
        if (key == param_1) {
            out_ptr = reinterpret_cast<std::uint32_t*>(p);
        }
    }
}

RH_ScopedInstall(EntryTableScanByKey, 0x00401570);

// ---------------------------------------------------------------------------
// DoubleDerefIndexedGetter  --  0x0040d250
//
// Original: FUN_0040d250 (16 bytes). undefined4 FUN_0040d250(void).
// Body: return **(undefined4 **)(PTR_PTR_005f2770 + DAT_0063ba7c * 4)
//   0x005f2770 table base [0x0040d254]; 0x0063ba7c index [0x0040d254]
// Pure read — no side effects. U-4203 non-blocking.
// ---------------------------------------------------------------------------

// 0x0040d250
extern "C" __declspec(dllexport) std::uint32_t __cdecl DoubleDerefIndexedGetter()
{
    // MOV EAX,[0x005f2770] — load the pointer-to-pointer stored at the global.
    // PTR_PTR_005f2770: the global holds a pointer; we dereference it. [0x0040d250]
    std::uint32_t** table = *reinterpret_cast<std::uint32_t***>(0x005f2770u);
    // MOV ECX,[0x0063ba7c] — index. [0x0040d255]
    std::uint32_t idx = *reinterpret_cast<std::uint32_t*>(0x0063ba7cu);
    // MOV EDX,[EAX + ECX*4]; MOV EAX,[EDX] — table[idx] is a pointer; read *ptr.
    // [0x0040d25b, 0x0040d25e]
    return *table[idx];
}

RH_ScopedInstall(DoubleDerefIndexedGetter, 0x0040d250);

// ---------------------------------------------------------------------------
// ScoreboardStateZeroInit  --  0x0041e080
//
// Original: FUN_0041e080 (67 bytes). void FUN_0041e080(void).
// Zeroes 11 globals in 0x0063d798..0x0063d7dc; writes 0x47 to DAT_0063d7cc.
//   each write cited at 0x0041e083..0x0041e0c0. U-4209 non-blocking.
// ---------------------------------------------------------------------------

// 0x0041e080
extern "C" __declspec(dllexport) void __cdecl ScoreboardStateZeroInit()
{
    *reinterpret_cast<std::uint32_t*>(0x0063d798u) = 0u;    // [0x0041e083]
    *reinterpret_cast<std::uint32_t*>(0x0063d79cu) = 0u;    // [0x0041e089]
    *reinterpret_cast<std::uint32_t*>(0x0063d7a0u) = 0u;    // [0x0041e08f]
    *reinterpret_cast<std::uint32_t*>(0x0063d7c4u) = 0u;    // [0x0041e095]
    *reinterpret_cast<std::uint32_t*>(0x0063d7d0u) = 0u;    // [0x0041e09b]
    *reinterpret_cast<std::uint32_t*>(0x0063d7d4u) = 0u;    // [0x0041e0a1]
    *reinterpret_cast<std::uint32_t*>(0x0063d7d8u) = 0u;    // [0x0041e0a7]
    *reinterpret_cast<std::uint32_t*>(0x0063d7dcu) = 0u;    // [0x0041e0ad]
    *reinterpret_cast<std::uint32_t*>(0x0063d7b0u) = 0u;    // [0x0041e0b3]
    *reinterpret_cast<std::uint32_t*>(0x0063d7b4u) = 0u;    // [0x0041e0b8]
    *reinterpret_cast<std::uint32_t*>(0x0063d7ccu) = 0x47u; // [0x0041e0be]
    *reinterpret_cast<std::uint32_t*>(0x0063d7b8u) = 0u;    // [0x0041e0c0]
}

RH_ScopedInstall(ScoreboardStateZeroInit, 0x0041e080);

// ---------------------------------------------------------------------------
// CopyTable005f2a70To0089a384  --  0x00414120
//
// Original: FUN_00414120 (80 bytes). void FUN_00414120(void).
//   _DAT_0089a37c=0 [0x00414125]; DAT_0089a378=0 [0x0041412b]; _DAT_0089a380=0 [0x00414131]
//   loop iVar1 0..0x9c step 4 (3-iter inner unroll):
//     *(0x0089a420 + iVar1) = 0;                  [0x0041413d]
//     *(0x0089a384 + iVar1) = *(0x005f2a70 + iVar1); [0x00414143/49]
//   0x9c (156) total bytes [0x00414153]. U-4206 non-blocking.
// ---------------------------------------------------------------------------

// 0x00414120
extern "C" __declspec(dllexport) void __cdecl CopyTable005f2a70To0089a384()
{
    // Zero the three header dwords. [0x00414125, 0x0041412b, 0x00414131]
    *reinterpret_cast<std::uint32_t*>(0x0089a37cu) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x0089a378u) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x0089a380u) = 0u;

    // Iterate 0..0x9c step 4 (39 iterations; 156 bytes / 4).
    // [0x0041413d, 0x00414143, 0x00414149, 0x00414153]
    auto* dst_zero = reinterpret_cast<std::uint32_t*>(0x0089a420u);
    auto* src      = reinterpret_cast<std::uint32_t*>(0x005f2a70u);
    auto* dst_copy = reinterpret_cast<std::uint32_t*>(0x0089a384u);

    for (std::size_t i = 0; i < 0x9cu / 4u; ++i) {
        dst_zero[i] = 0u;
        dst_copy[i] = src[i];
    }
}

RH_ScopedInstall(CopyTable005f2a70To0089a384, 0x00414120);
