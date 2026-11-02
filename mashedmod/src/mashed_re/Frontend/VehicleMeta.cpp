// Mashed RE - Vehicle metadata getters used by the Frontend subsystem.
// These functions query global vehicle unlock/slot data referenced in HUD
// and frontend menu draw paths.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// VehicleUnlockFlagGet  --  0x0042ef40
//
// Original: FUN_0042ef40 (147 bytes, 0x0042ef40..0x0042efd3)
// Signature: int __cdecl FUN_0042ef40(int param_1, int param_2)
//   param_1 = vehicle index (0-based; no bounds check in original — [UNCERTAIN U-3176])
//   param_2 = slot type discriminant; accepted raw values: 2,3,4,5,10
//             and 1000-offset variants: 0x3e9(1001),0x3ea(1002),0x3ed(1005),0x3f3(1011)
//
// Array base: DAT_007f0e50 (0x007f0e50); stride 0xc (12 bytes) per vehicle.
// Reads one byte from the vehicle's unlock record at a byte offset determined
// by param_2, then returns 1 if that byte == 0x01, else 0.
//
// Byte offsets into each vehicle's 0xc-byte record:
//   param_2 == 2 (default):          DAT_007f0e50 + param_1*0xc + 0   (0x0042ef6e)
//   param_2 == 3  or 0x3e9 (1001):   DAT_007f0e51 + param_1*0xc + 0   (0x0042ef78/85)
//   param_2 == 4  or 0x3ea (1002):   DAT_007f0e52 + param_1*0xc + 0   (0x0042ef8e/98)
//   param_2 == 5  or 0x3ed (1005):   DAT_007f0e55 + param_1*0xc + 0   (0x0042efa5/b3)
//   param_2 == 10 or 0x3f3 (1011):   DAT_007f0e5b + param_1*0xc + 0   (0x0042efbd/c5)
//
// NOTE: The original function performs NO bounds check on param_1.
// [UNCERTAIN U-3176] Maximum vehicle count unknown from this function alone;
// reproduced as-is without a bounds guard.
//
// Analysis: re/analysis/hud_frontend_d4/0x0042ef40.md
// ---------------------------------------------------------------------------

// MATCH-S07: the body of 0x0042ef40 (VehicleUnlockFlagGet) moved to Match/MatchS07_0042ef40.cpp,
// which compiles the byte-matched source re/match/s07/0042ef40.c.
