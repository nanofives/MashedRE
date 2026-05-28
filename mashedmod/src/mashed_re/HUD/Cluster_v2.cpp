// Mashed RE — HUD batch-v session 2 (C2->C3 promotions).
// Analysis notes:
//   re/analysis/boot_hud_promote_ae1/0x00402f80.md
//
// Binary anchor:
//   original\MASHED.exe.unpatched  SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//
// Hooks in this file:
//   0x00402f80  CupFloatInit  — pure void(void) initialiser; writes 4 float globals
//
// Refusals this session (gate failures documented):
//   0x004c1c80 — no arg_type for (int, in_ptr_2uint32) signature;
//               STOP-AND-ASK deferred (no matching arg_type in diff_template.js).
//               Note: callee FUN_004c0e50 IS now C2 (stale batch-k note), but
//               harness gap still blocks C3 promotion.
//   0x00555f20 — takes a live font context pointer (int param_1 = ctx ptr);
//               makes vtable+0x108 call requiring valid live object;
//               no arg_type creates a valid font context from test scalars;
//               STOP-AND-ASK deferred (no matching arg_type in diff_template.js).

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// CupFloatInit globals (Ghidra addresses from analysis note 0x00402f80.md)
// ---------------------------------------------------------------------------

// DAT_00636ae8 — initialised to 0 by this function (0x00402f80)
static std::uint32_t* const g_CupFloat_636ae8 =
    reinterpret_cast<std::uint32_t*>(0x00636ae8u);

// DAT_00636aec — initialised to 0x42700000 (+60.0f IEEE-754) by this function
static float* const g_CupFloat_636aec =
    reinterpret_cast<float*>(0x00636aecu);

// DAT_00636af0 — initialised to 0 by this function (0x00402f8a)
static std::uint32_t* const g_CupFloat_636af0 =
    reinterpret_cast<std::uint32_t*>(0x00636af0u);

// DAT_00636af4 — initialised to 0x42200000 (+40.0f IEEE-754) by this function
static float* const g_CupFloat_636af4 =
    reinterpret_cast<float*>(0x00636af4u);

// ===========================================================================
// 0x00402f80  CupFloatInit
//
// Decompilation (Ghidra, pool2, batch-ae-s1):
//   void FUN_00402f80(void) {
//     DAT_00636ae8 = 0;                   // written at 0x00402f80
//     DAT_00636af0 = 0;                   // written at 0x00402f8a
//     DAT_00636af4 = 0x42200000;          // written at 0x00402f94  (+40.0f)
//     DAT_00636aec = 0x42700000;          // written at 0x0040299e  (+60.0f)
//   }
//
// Pure initialiser: no calls, no reads, no branches.
// Writes 4 consecutive globals with constant values.
// Caller: FUN_00433240 (C2).
// Leaf-exemption applies (no callees).
// ===========================================================================
// 0x00402f80
extern "C" __declspec(dllexport) void __cdecl CupFloatInit()
{
    *g_CupFloat_636ae8 = 0u;
    *g_CupFloat_636af0 = 0u;
    *g_CupFloat_636af4 = 40.0f;     // 0x42200000
    *g_CupFloat_636aec = 60.0f;     // 0x42700000
}

RH_ScopedInstall(CupFloatInit, 0x00402f80);  // C3 evidence: void_write_observe GREEN 10/10
