// Mashed RE -- Render/RwStreamWriteBytes.cpp
// C2->C3 promotion, worktree wf_b0f68acd-63f-39.
//
// Binary anchor: MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file (1):
//   0x004cc770  RwStreamWriteBytes -- pass-through wrapper: calls FUN_004cbe80
//                                     and returns param_1 unchanged.
//
// Analysis note: re/analysis/render_5_c1_to_c2_s1/FUN_004cc770.md (C2)
//
// Callee:
//   0x004cbe80  FUN_004cbe80 (C2 Save/RwStream.cpp; RW stream write dispatcher)
//
// Diff strategy:
//   arg_type='int_pair'; signature ['uint32','uint32'] (2 args).
//   Test vectors: [[0,0],...] -- param_1=0 -> FUN_004cbe80 switch(*0) -> AV 0x0.
//   crash_equal_ok=True: both orig and reimpl AV at 0x0; crash records match.

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// 0x004cc770  RwStreamWriteBytes
//
// Original: FUN_004cc770 (28 bytes, 0x004cc770..0x004cc78b)
// Signature: undefined4 FUN_004cc770(undefined4 param_1, undefined4 param_2,
//                                    uint param_3)
//   param_1: RW stream context pointer (passed to FUN_004cbe80)
//   param_2: source data pointer (passed to FUN_004cbe80)
//   param_3: byte count (passed to FUN_004cbe80)
//   Returns: param_1 (always; ignores FUN_004cbe80 return value)
//
// Body (2 instructions):
//   CALL FUN_004cbe80(param_1, param_2, param_3)   [0x004cc770]
//   RETURN param_1                                  [0x004cc783]
//
// The return value of FUN_004cbe80 is discarded; this wrapper always returns
// param_1 regardless of write success or failure.
//
// Analysis note: re/analysis/render_5_c1_to_c2_s1/FUN_004cc770.md
// ---------------------------------------------------------------------------

// Callee 0x004cbe80 is our ported RwStreamWrite_s2 (Save/RwStream.cpp, C4/impl) --
// call the port directly rather than trampolining through the original image.
// [0x004cc770 body -> 0x004cbe80]
extern "C" void* __cdecl RwStreamWrite_s2(std::uint32_t* param_1,
                                          std::uint32_t* param_2,
                                          std::uint32_t  param_3);

// 0x004cc770
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwStreamWriteBytes(
    std::uint32_t  param_1,   // RW stream context pointer [0x004cc770 param_1]
    std::uint32_t  param_2,   // source data pointer       [0x004cc770 param_2]
    std::uint32_t  param_3)   // byte count                [0x004cc770 param_3]
{
    // Call stream write dispatcher; discard return value. [0x004cc770: CALL FUN_004cbe80]
    RwStreamWrite_s2(reinterpret_cast<std::uint32_t*>(param_1),
                     reinterpret_cast<std::uint32_t*>(param_2),
                     param_3);

    // Always return param_1. [0x004cc783: MOV EAX, param_1; RET]
    return param_1;
}

RH_ScopedInstall(RwStreamWriteBytes, 0x004cc770);
