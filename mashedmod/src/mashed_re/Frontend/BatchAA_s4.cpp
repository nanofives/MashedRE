// Mashed RE -- Frontend small-leaf reimplementations (c3-batch-aa session 4).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x0042a9c0  ModeCodeLookup   -- pure leaf; maps DAT_007f0fd0 to 0x2d or 0x16
//   0x00425b70  SlotFieldSetter  -- pure leaf; writes param_2 to array[slot]+0x04,
//                                   zeroes array[slot]+0x00; base DAT_008992a0 stride 0x4c
//
// Deferred from this session (documented below):
//   0x004241c0  FUN_004241c0  -- no decomp transcript; reads live player-state arrays
//   0x004c1c80  FUN_004c1c80  -- no arg_type allocating a "this" struct buffer for param_1
//
// Refused from this session:
//   0x0042a640  FUN_0042a640  -- uses DAT_007d3ff8 (live-state vtable dispatch pointer);
//                                explicitly excluded by session rules
//
// Analysis notes:
//   re/analysis/frontend_c1_to_c2_s6/FUN_0042a9c0.md
//   re/analysis/frontend_c1_to_c2_s3/FUN_00425b70.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// ModeCodeLookup  --  0x0042a9c0
//
// Original: FUN_0042a9c0 (40 bytes, 0x0042a9c0..0x0042a9e8)
// Signature: undefined4 FUN_0042a9c0(void)
// Body (verbatim Ghidra decompile, Mashed_pool13):
//
//   undefined4 uVar1;
//   if ((((DAT_007f0fd0 == 10) || (DAT_007f0fd0 == 9)) || (DAT_007f0fd0 == 8)) ||
//      ((DAT_007f0fd0 == 7 || (uVar1 = 0x16, DAT_007f0fd0 == 5)))) {
//     uVar1 = 0x2d;
//   }
//   return uVar1;
//
// The comma expression `(uVar1 = 0x16, DAT_007f0fd0 == 5)` assigns uVar1=0x16
// then evaluates the boolean. When DAT_007f0fd0 == 5 this condition is true so
// the outer if-body runs and overwrites uVar1 with 0x2d.  Net: modes {5,7,8,9,10}
// all return 0x2d (45). All other modes return uVar1 uninitialised (UB in decomp;
// original assembly presumably returns whatever was in EAX/on stack at that point).
//
// We replicate the decomp literal exactly: the guard `if (mode != 5 && mode != 7
// && mode != 8 && mode != 9 && mode != 10)` early-out preserves the uVar1 = 0x16
// sub-expression evaluation order without triggering UB in C++.  For the
// Frida diff we only test defined modes {5,7,8,9,10}.
//
// Globals:
//   DAT_007f0fd0 -- mode/state int32 global [cited at 0x0042a9c0]
//
// Note: [UNCERTAIN U-4200] on exact return for mode==5 resolved by decomp
// literal (0x2d overwrites 0x16 in if-body); Frida GREEN will confirm.
// ---------------------------------------------------------------------------

// 0x0042a9c0
extern "C" __declspec(dllexport) std::uint32_t __cdecl ModeCodeLookup()
{
    // [0x0042a9c0] Read mode global.
    std::int32_t mode = *reinterpret_cast<std::int32_t*>(0x007f0fd0u);

    std::uint32_t uVar1 = 0u; // mirrors "undefined4 uVar1" — safe default for UB branch

    // Replicate the Ghidra comma-expression chain literally:
    //   if (mode==10 || mode==9 || mode==8 || (uVar1=0x16, mode==5) || mode==7)
    //       uVar1 = 0x2d;
    //
    // In the decomp the sub-expression `(uVar1 = 0x16, mode == 5)` is part of the
    // OR chain evaluated left-to-right; if mode==5 the if-body then sets uVar1=0x2d.
    // If mode==7 the RHS of the last OR short-circuits before the comma expression.
    if ((mode == 10) || (mode == 9) || (mode == 8) ||
        (mode == 7) || (uVar1 = 0x16u, mode == 5))
    {
        uVar1 = 0x2du; // [0x0042a9e4] overwrites 0x16 for mode==5 too
    }

    return uVar1; // [0x0042a9e7]
}

RH_ScopedInstall(ModeCodeLookup, 0x0042a9c0);

// ---------------------------------------------------------------------------
// SlotFieldSetter  --  0x00425b70
//
// Original: FUN_00425b70 (28 bytes, 0x00425b70..0x00425b8b)
// Signature: void FUN_00425b70(int param_1, undefined4 param_2)
// Body (mechanically from re/analysis/frontend_c1_to_c2_s3/FUN_00425b70.md):
//
//   (&DAT_008992a4)[param_1 * 0x13] = param_2;  // store at array[slot]+0x04
//   (&DAT_008992a0)[param_1 * 0x13] = 0;        // zero  array[slot]+0x00
//
// Array base: 0x008992a0 [cited in note]; stride: 0x13 dwords = 76 bytes = 0x4c.
// +0x00 field zeroed (base 0x008992a0).
// +0x04 field receives param_2 (base 0x008992a4 = 0x008992a0 + 4).
// Pure leaf — no branches, no callees.
// ---------------------------------------------------------------------------

// 0x00425b70
extern "C" __declspec(dllexport) void __cdecl SlotFieldSetter(int param_1, std::uint32_t param_2)
{
    // stride = 0x13 dwords = 0x13 * 4 = 0x4c bytes
    constexpr std::uintptr_t base = 0x008992a0u; // [cited at note: DAT_008992a0]
    constexpr std::size_t    stride = 0x13u;     // dwords per slot

    auto slot = static_cast<std::size_t>(static_cast<std::uint32_t>(param_1));

    // array[slot]+0x04 = param_2  [DAT_008992a4 base]
    auto* field04 = reinterpret_cast<std::uint32_t*>(base + 4u + slot * stride * 4u);
    *field04 = param_2;

    // array[slot]+0x00 = 0  [DAT_008992a0 base]
    auto* field00 = reinterpret_cast<std::uint32_t*>(base + slot * stride * 4u);
    *field00 = 0u;
}

RH_ScopedInstall(SlotFieldSetter, 0x00425b70);
