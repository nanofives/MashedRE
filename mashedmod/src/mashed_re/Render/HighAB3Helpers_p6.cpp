// Mashed RE — Render high-address cluster helper functions (c3-batch-p session 6).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x004d7ff0  RwIdentityPassthrough  —  4B pure leaf; identity (return param_1)
//   0x00552d70  ViewportStackPop       — 39B pure leaf; viewport/camera stack decrement
//   0x004d8480  RwErrSlotWrite         — 85B pure leaf; conditional 64-bit error-slot store
//
// Deferred this session:
//   0x00497310  ReadInputForAction     — 293B; >200B multi-branch input dispatcher with
//                                        callee FUN_004a2c48 (U-3024 unresolved); STOP-AND-ASK deferred.
//
// Analysis notes:
//   re/analysis/promote_c1_high_ab3/0x004d7ff0.md
//   re/analysis/promote_c1_high_ab3/0x00552d70.md
//   re/analysis/promote_c1_high_ab3/0x004d8480.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// 0x004d7ff0  FUN_004d7ff0  RwIdentityPassthrough  (4 bytes)
//
// Signature: undefined4 FUN_004d7ff0(undefined4 param_1)
// Body: single instruction — return param_1.  No reads, no writes, no branches.
//
// All 125 callers are in the RW object management / D3D9 pipeline cluster
// (0x004c1c10..0x005c4d19 range).
//
// Uncertainty U-0131 (semantic, non-blocking): whether this is a purposeful
// pass-through or a Ghidra-missed tail-call is not resolvable from static analysis.
// Leaf-function exemption applies (no callees; re/CONFIDENCE.md §C2→C3).
//
// Cited from 0x004d7ff0 body:
//   0x004d7ff0: MOV EAX, [ESP+4]  ; load param_1 into EAX
//   0x004d7ff4: RET                ; return
// ─────────────────────────────────────────────────────────────────────────────

// 0x004d7ff0
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwIdentityPassthrough(std::uint32_t param_1)
{
    return param_1;
}

RH_ScopedInstall(RwIdentityPassthrough, 0x004d7ff0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00552d70  FUN_00552d70  ViewportStackPop  (39 bytes)
//
// Signature: void FUN_00552d70(void)
// Purpose: decrements the viewport/camera stack depth counter and clears
//          two render-cache invalidation slots.
//
// Algorithm (cited from 0x00552d70 body):
//   1. if (DAT_00912b04 <= 0): return  (guard — do nothing if stack is already empty)
//   2. DAT_00912b04 = DAT_00912b04 - 1  (decrement stack depth)
//   3. DAT_00912bd8 = 0                 (zero stage-state cache slot)
//   4. DAT_00912bec = 0                 (zero distance-threshold cache slot)
//
// Constants (cited from 0x00552d70 body):
//   0x00912b04  — viewport stack depth counter (int)
//   0x00912bd8  — stage-state invalidation slot (uint32)
//   0x00912bec  — distance-threshold invalidation slot (uint32)
//
// Callers (9): 0x004275d0, 0x004278d0, 0x00427990, 0x00427ad0, 0x00427be0,
//              0x00427e00, 0x00427f00, 0x00427ff0, 0x00428140
//   All 9 are in the 0x004275d0..0x00428293 frontend drawing cluster (C2+).
//
// No callees. Leaf-function exemption applies (re/CONFIDENCE.md §C2→C3).
// ─────────────────────────────────────────────────────────────────────────────

// Statics for the three stack globals (addresses cited from 0x00552d70 body).
static volatile std::int32_t*  const s_StackDepth   = reinterpret_cast<std::int32_t*>(0x00912b04u);
static volatile std::uint32_t* const s_StageCache   = reinterpret_cast<std::uint32_t*>(0x00912bd8u);
static volatile std::uint32_t* const s_DistCache    = reinterpret_cast<std::uint32_t*>(0x00912becu);

// 0x00552d70
extern "C" __declspec(dllexport) void __cdecl ViewportStackPop(void)
{
    if (*s_StackDepth <= 0) return;         // guard: stack must be non-empty
    *s_StackDepth   = *s_StackDepth - 1;    // 0x00912b04: decrement depth
    *s_StageCache   = 0u;                   // 0x00912bd8: invalidate stage-state cache
    *s_DistCache    = 0u;                   // 0x00912bec: invalidate distance-threshold cache
}

RH_ScopedInstall(ViewportStackPop, 0x00552d70);

// ─────────────────────────────────────────────────────────────────────────────
// 0x004d8480  FUN_004d8480  RwErrSlotWrite  (85 bytes)
//
// Signature: undefined4* FUN_004d8480(undefined4* param_1)
// Purpose: conditionally stores a 64-bit error record into a single-entry
//          error slot, guarded by an "empty" sentinel.
//
// Algorithm (cited from 0x004d8480 body):
//   error_slot = *(undefined4**)(DAT_007d6c5c + DAT_007d3ff8)
//   slot[0] must equal 0 AND slot[1] must equal 0x80000000 ("empty" sentinel)
//   if slot is free (both conditions hold):
//     if param_1[1] & 0x80000000 == 0:
//       slot[0] = *param_1        (low dword)
//       slot[1] = param_1[1]      (high dword)
//     else:
//       slot[0] = 0               (suppress high-bit value in low dword)
//       slot[1] = param_1[1]      (high dword preserved)
//   if slot already occupied: no write (early return)
//   returns param_1 in all paths.
//
// Constants (cited from 0x004d8480 body):
//   0x80000000  (-2147483648)  — empty sentinel value at slot[1]  (0x004d84ac)
//   0x80000000  (-2147483648)  — high-bit test mask on param_1[1] (0x004d84c5)
//   DAT_007d6c5c (0x007d6c5c)  — error slot array base pointer
//   DAT_007d3ff8 (0x007d3ff8)  — index/offset into error slot array
//
// Uncertainties (non-blocking):
//   U-0132 (structural): whether DAT_007d3ff8 is a DWORD index or raw byte offset.
//   U-0133 (semantic):   param_1[0..1] may form HRESULT pair, LARGE_INTEGER, or
//                        RW error-code pair — semantic not confirmed.
// Both are non-blocking for C3 (leaf-function exemption; no semantic [Blocks C3] flags).
//
// No callees. Leaf-function exemption applies (re/CONFIDENCE.md §C2→C3).
// ─────────────────────────────────────────────────────────────────────────────

// 0x004d8480
extern "C" __declspec(dllexport) std::uint32_t* __cdecl RwErrSlotWrite(std::uint32_t* param_1)
{
    // Dereference the error slot pointer:
    //   base  = *(uint32_t*)(0x007d6c5c)      — array base pointer
    //   index = *(uint32_t*)(0x007d3ff8)      — byte offset/index into array
    //   slot  = (uint32_t*)(base + index)
    const std::uint32_t base  = *reinterpret_cast<std::uint32_t*>(0x007d6c5cu);  // DAT_007d6c5c
    const std::uint32_t index = *reinterpret_cast<std::uint32_t*>(0x007d3ff8u);  // DAT_007d3ff8
    std::uint32_t* const slot = reinterpret_cast<std::uint32_t*>(base + index);

    // Guard: slot must be "empty" (slot[0] == 0 AND slot[1] == 0x80000000).
    if (slot[0] != 0u || slot[1] != 0x80000000u) {  // 0x004d84ac: sentinel test
        return param_1;  // occupied — no write
    }

    // Conditional write based on high bit of param_1[1]:
    if ((param_1[1] & 0x80000000u) == 0u) {  // 0x004d84c5: high-bit test
        slot[0] = param_1[0];  // low dword of error record
        slot[1] = param_1[1];  // high dword of error record
    } else {
        slot[0] = 0u;          // suppress: write 0 for low dword
        slot[1] = param_1[1];  // high dword preserved as-is
    }

    return param_1;  // always return param_1 (identity for callers)
}

RH_ScopedInstall(RwErrSlotWrite, 0x004d8480);
