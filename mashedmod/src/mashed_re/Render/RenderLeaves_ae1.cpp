// Mashed RE — Render cluster C2->C3 promotions (c3-batch-ae session s1).
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions promoted in this file:
//   0x00492440  RenderStatsAccumulate — per-frame stats rollup over a 60-frame
//                                        window (struct-ptr in/out, pure leaf).
//   0x004b46b0  Vec3Equal             — exact-equality predicate for two float3
//                                        vectors (two float* in, int return).
//
// Skipped this session (recorded in PROMOTION_QUEUE c3_batch_ae s1):
//   0x004a2d18  __unlock_file2(1, ESI) CRT thunk           — library
//   0x00462950  course audio loader (COM vtable + RWS)     — live-state
//   0x004ad1e0  __amsg_exit(2) CRT abort (does-not-return) — library
//   0x004e6680  RpClumpRender (RW live-state)              — live-state
//   0x0045de80  track audio zone->channel mapper           — live-state
//                 (reads PTR_DAT_00612f30[] track table, writes many globals)
//   0x004e1df0  RW extension-chunk drain                   — live-state
//                 (needs a live RwStream + C2 stream-I/O callees)
//   0x004a4fc1  write_char CRT                             — library
//   0x004a4da0  __aulldvrm CRT                             — library
//   0x004039e0  float10 (x87 80-bit ST0) getter            — no Frida arg_type
//                 (already documented deferred in BatchAB_s1.cpp)
//
// Analysis refs:
//   re/analysis/skeleton_prep_boot_winmain_a/00492440.md
//   re/analysis/render_3_c1_to_c2_s3/FUN_004b46b0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RenderStatsAccumulate — 0x00492440
//
// Original: FUN_00492440 (124 bytes, 0x00492440..0x004924bc)
// Signature: void FUN_00492440(int param_1)  // param_1 = stats record pointer
//
// param_1 points at a stats record; all accesses are byte offsets from it:
//   +0x20  A sample (in, s32)          +0x34  A average (out)
//   +0x24  B sample (in, s32)          +0x38  B average (out)
//   +0x28  frame counter (s32)         +0x3c  A running max
//   +0x2c  A accumulator (s32)         +0x40  B running max
//   +0x30  B accumulator (s32)
//
// Mechanics (mechanical description, no inference):
//   - iVar1 = *(p+0x20); iVar2 = *(p+0x24)
//   - *(p+0x2c) += iVar1 ; *(p+0x30) += iVar2
//   - if *(p+0x28) > 0x3b (59):           // 60 frames completed
//       *(p+0x34) = *(p+0x2c) / 0x3c (60) ; *(p+0x2c) = 0
//       *(p+0x38) = *(p+0x30) / 0x3c (60) ; *(p+0x30) = 0
//       *(p+0x28) = 0
//   - *(p+0x28) += 1                      // unconditional, after the branch
//   - if *(p+0x3c) < iVar1: *(p+0x3c) = iVar1
//   - if *(p+0x40) < iVar2: *(p+0x40) = iVar2
//
// Constants cited:
//   0x3b (59)  — threshold compare (~0x00492466)
//   0x3c (60)  — rolling-average divisor (~0x00492480)
//
// Signed s32 throughout: accumulate/divide/compare match x86 idiv/jl semantics.
// Pure leaf (no callees). ref: re/analysis/skeleton_prep_boot_winmain_a/00492440.md
// ---------------------------------------------------------------------------

// 0x00492440
extern "C" __declspec(dllexport) void __cdecl RenderStatsAccumulate(void* param_1)
{
    std::uint8_t* p = static_cast<std::uint8_t*>(param_1);

    auto field = [p](std::uint32_t off) -> std::int32_t& {
        return *reinterpret_cast<std::int32_t*>(p + off);
    };

    std::int32_t iVar1 = field(0x20);   // A sample
    std::int32_t iVar2 = field(0x24);   // B sample

    field(0x2c) += iVar1;               // A accumulator
    field(0x30) += iVar2;               // B accumulator

    if (field(0x28) > 0x3b) {           // 60-frame window complete
        field(0x34) = field(0x2c) / 0x3c;   // A average over 60 frames
        field(0x2c) = 0;                     // reset A accumulator
        field(0x38) = field(0x30) / 0x3c;   // B average over 60 frames
        field(0x30) = 0;                     // reset B accumulator
        field(0x28) = 0;                     // reset frame counter
    }
    field(0x28) += 1;                   // advance frame counter (unconditional)

    if (field(0x3c) < iVar1) {          // track A maximum
        field(0x3c) = iVar1;
    }
    if (field(0x40) < iVar2) {          // track B maximum
        field(0x40) = iVar2;
    }
}

RH_ScopedInstall(RenderStatsAccumulate, 0x00492440);

// ---------------------------------------------------------------------------
// Vec3Equal — 0x004b46b0
//
// Original: FUN_004b46b0 (53 bytes, 0x004b46b0..0x004b46e5)
// Signature: undefined4 FUN_004b46b0(float *param_1, float *param_2)
//
// Body (verbatim decomp):
//   if (((*param_1 == *param_2) && (param_1[1] == param_2[1]))
//        && (param_1[2] == param_2[2])) return 1;
//   return 0;
//
// Exact IEEE-754 float equality, component-by-component (no epsilon). Note
// this is FP `==`, not a bit compare: +0.0 == -0.0 is true; NaN == NaN is
// false. The reimpl uses C `==` so it inherits the same x86 comiss/fcom
// ordering semantics as the original.
//
// Pure leaf (no callees). Caller: FUN_00478660.
// ref: re/analysis/render_3_c1_to_c2_s3/FUN_004b46b0.md
// ---------------------------------------------------------------------------

// 0x004b46b0
extern "C" __declspec(dllexport) std::uint32_t __cdecl Vec3Equal(const float* param_1, const float* param_2)
{
    if (param_1[0] == param_2[0] && param_1[1] == param_2[1] && param_1[2] == param_2[2]) {
        return 1;
    }
    return 0;
}

RH_ScopedInstall(Vec3Equal, 0x004b46b0);
