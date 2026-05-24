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

// 0x0042ef40
extern "C" __declspec(dllexport) int __cdecl VehicleUnlockFlagGet(int param_1, int param_2) {
    static const std::uintptr_t kArrayBase = 0x007f0e50u;
    static const int             kStride    = 0xc;

    // Compute byte address for the given slot type.
    // Offsets match exactly what the decompiler shows for each case/branch.
    const std::uint8_t* record =
        reinterpret_cast<const std::uint8_t*>(kArrayBase + param_1 * kStride);

    std::uint8_t flag_byte;

    switch (param_2) {
    case 2:
    default:
        // param_2==2 or any unrecognised value uses byte offset 0.
        // 0x007f0e50 + param_1*0xc  (cited at 0x0042ef6e)
        flag_byte = record[0];
        break;

    case 3:
    case 0x3e9:  // 1001 — cited at 0x0042ef85
        // 0x007f0e51 + param_1*0xc  (byte +1)  (cited at 0x0042ef78)
        flag_byte = record[1];
        break;

    case 4:
    case 0x3ea:  // 1002 — cited at 0x0042ef98
        // 0x007f0e52 + param_1*0xc  (byte +2)  (cited at 0x0042ef8e)
        flag_byte = record[2];
        break;

    case 5:
    case 0x3ed:  // 1005 — cited at 0x0042efb3
        // 0x007f0e55 + param_1*0xc  (byte +5)  (cited at 0x0042efa5)
        flag_byte = record[5];
        break;

    case 10:
    case 0x3f3:  // 1011 — cited at 0x0042efc5
        // 0x007f0e5b + param_1*0xc  (byte +11)  (cited at 0x0042efbd)
        flag_byte = record[11];
        break;
    }

    return (flag_byte == 0x01u) ? 1 : 0;
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(VehicleUnlockFlagGet, 0x0042ef40);
