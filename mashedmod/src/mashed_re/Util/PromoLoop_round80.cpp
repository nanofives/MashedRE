// Mashed RE — promote-round round 80 (pipeline round_7: per-vehicle physics state reset).
//
// Function in this file:
//   0x0046baa0  VehiclePhysicsStateReset  — 571B leaf; zeros velocity/force/angular/contact
//               fields for one vehicle slot (0-15) at strides 0xd04 and 0x341.
//               Returns 0 if slot > 15; 1 on success.
//
// Analysis note: re/analysis/game_state_d5/0x0046baa0.md
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// VehiclePhysicsStateReset  --  0x0046baa0
//
// Original: FUN_0046baa0 (571 bytes, 0x0046baa0..0x0046bcdb)
// Signature: undefined4 FUN_0046baa0(uint param_1)
//   param_1: vehicle slot index (0..15 valid; > 15 -> return 0)
// Returns: 0 if out-of-bounds, 1 on success.
//
// Algorithm (cited from 0x0046baa0 body):
//   1. Guard: if (param_1 > 0xf) return 0;                             [0x0046baa4]
//   2. Compute stride: iVar1 = param_1 * 0xd04;                        [0x0046bab0]
//      (Note: accesses via (&DAT_xxx)[param_1 * 0x341] on undefined4*
//       are equivalent byte offsets = param_1 * 0x341 * 4 = param_1 * 0xd04 = iVar1)
//   3. Self-ref write: (&DAT_008815a0)[param_1 * 0x341] = param_1;     [0x0046bab8]
//   4. Zero velocity XYZ (3 fields at 0x881740/44/48 + iVar1);         [0x0046babb..]
//   5. Zero angular rate XYZ (0x881734 and 0x8817a0/a8 + iVar1);       [0x0046bac8..]
//   6. Zero force/torque accumulators (0x881804/08/0c/7f8 + iVar1);    [0x0046bad7..]
//   7. Zero contact normals (0x881864/6c/8c8/cc/d0/bc + iVar1);        [0x0046bae7..]
//   8. Zero suspension (0x881928/30 + iVar1);                           [0x0046baff..]
//   9. Set 4 fields to 0x3f800000 (1.0f):
//      0x8817a4, 0x881868, 0x88192c, 0x8819f0 + iVar1;                 [0x0046bb07..]
//  10. Set 4 fields to integer 1:
//      0x881738, 0x8817fc, 0x8818c0, 0x881984 + iVar1;                 [0x0046bb23..]
//  11. Zero 0x88198c/90/94, 0x881980, 0x8819ec/f4 + iVar1;             [0x0046bb37..]
//  12. Write 0x40800000 (4.0f) and -1.0f (0xbf800000) and 0s to
//      the 0x341-stride per-slot fields at 0x881f80..0x8820bc;         [0x0046bb57..]
//  13. Loop 1 (16 iterations): packed 3-dword array at
//      (&DAT_008820d0)[param_1 * 0x341]; writes [0, -1.0f, 0] per entry; [0x0046bb89]
//  14. Zero remaining physics fields 0x88218c, 0x881f64/60/5c + iVar1
//      and per-slot at 0x881f58/54/50 etc.;                            [0x0046bbba..]
//  15. Loop 2 (18 iterations): 0x40-stride array at
//      &DAT_00881a4c + iVar1; writes 0xffffffff per entry;             [0x0046bc96]
//  16. Zero per-slot entity table fields at 0x8815b4..0x8815ec
//      (stride 0x341);                                                  [0x0046bcb5..]
//  17. Return 1.                                                         [0x0046bcd9]
//
// Constants (all cited from 0x0046baa0 body):
//   0xf (15)       — guard upper-bound                                  [0x0046baa4]
//   0xd04 (3332)   — per-vehicle stride in the physics SOA               [0x0046bab0]
//   0x341 (833)    — undefined4* stride (byte offset = *4 = 0xd04)       [0x0046bab8]
//   0x3f800000     — 1.0f in IEEE 754                                    [0x0046bb07]
//   0xbf800000     — -1.0f in IEEE 754                                   [0x0046bb89]
//   0x40800000     — 4.0f in IEEE 754                                    [0x0046bb57]
//   0xffffffff     — sentinel for loop 2                                 [0x0046bc96]
//   0x10 (16)      — iteration count for loop 1                          [0x0046bb89]
//   0x12 (18)      — iteration count for loop 2                          [0x0046bc96]
//   0x40 (64)      — byte stride for loop 2                              [0x0046bc96]
//
// Uncertainties (non-blocking):
//   U-3012: per-vehicle physics SOA field semantics (exact names of all
//           zeroed/initialized fields). Known anchors: 0x881734..748 = velocity XYZ.
//
// ref: re/analysis/game_state_d5/0x0046baa0.md
// ---------------------------------------------------------------------------

// 0x0046baa0
extern "C" __declspec(dllexport) std::uint32_t __cdecl VehiclePhysicsStateReset(
    std::uint32_t param_1)
{
    // Guard: slot index > 15 -> return 0. [0x0046baa4]
    if (param_1 > 0xfu) {
        return 0u;
    }

    // Per-vehicle byte stride in the physics SOA. [0x0046bab0]
    int iVar1 = static_cast<int>(param_1) * 0xd04;

    // Self-reference: write slot index to entity table.                  [0x0046bab8]
    // (&DAT_008815a0)[param_1 * 0x341] is undefined4* array -> byte off = param_1*0xd04 = iVar1
    *reinterpret_cast<std::uint32_t*>(0x008815a0u + static_cast<unsigned>(iVar1)) = param_1;

    // Zero velocity XYZ.                                                  [0x0046babb]
    *reinterpret_cast<std::uint32_t*>(0x00881740u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881744u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881748u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero angular rate fields.                                           [0x0046bac8]
    *reinterpret_cast<std::uint32_t*>(0x00881734u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008817a0u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008817a8u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero force accumulators XYZ and torque.                            [0x0046bad7]
    *reinterpret_cast<std::uint32_t*>(0x00881804u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881808u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x0088180cu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008817f8u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero contact normals and contact state fields.                     [0x0046bae7]
    *reinterpret_cast<std::uint32_t*>(0x00881864u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x0088186cu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008818c8u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008818ccu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008818d0u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008818bcu + static_cast<unsigned>(iVar1)) = 0u;

    // Zero suspension state.                                             [0x0046baff]
    *reinterpret_cast<std::uint32_t*>(0x00881928u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881930u + static_cast<unsigned>(iVar1)) = 0u;

    // Set angular rate scale, contact normal Z, suspension scale, inertia/mass to 1.0f. [0x0046bb07]
    *reinterpret_cast<std::uint32_t*>(0x008817a4u + static_cast<unsigned>(iVar1)) = 0x3f800000u;
    *reinterpret_cast<std::uint32_t*>(0x00881868u + static_cast<unsigned>(iVar1)) = 0x3f800000u;
    *reinterpret_cast<std::uint32_t*>(0x0088192cu + static_cast<unsigned>(iVar1)) = 0x3f800000u;
    *reinterpret_cast<std::uint32_t*>(0x008819f0u + static_cast<unsigned>(iVar1)) = 0x3f800000u;

    // Set 4 fields to integer 1.                                         [0x0046bb23]
    *reinterpret_cast<std::uint32_t*>(0x00881738u + static_cast<unsigned>(iVar1)) = 1u;
    *reinterpret_cast<std::uint32_t*>(0x008817fcu + static_cast<unsigned>(iVar1)) = 1u;
    *reinterpret_cast<std::uint32_t*>(0x008818c0u + static_cast<unsigned>(iVar1)) = 1u;
    *reinterpret_cast<std::uint32_t*>(0x00881984u + static_cast<unsigned>(iVar1)) = 1u;

    // Zero additional physics fields.                                    [0x0046bb37]
    *reinterpret_cast<std::uint32_t*>(0x0088198cu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881990u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881994u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881980u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008819ecu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008819f4u + static_cast<unsigned>(iVar1)) = 0u;

    // Write 4.0f to per-slot field (0x341 stride = 0xd04 byte offset).  [0x0046bb57]
    *reinterpret_cast<std::uint32_t*>(0x00881f80u + static_cast<unsigned>(iVar1)) = 0x40800000u;

    // Zero more physics fields (iVar1 stride).                          [0x0046bb5f]
    *reinterpret_cast<std::uint32_t*>(0x008820bcu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008820b8u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008820b4u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero per-slot fields at 0x341 stride.                             [0x0046bb71]
    *reinterpret_cast<std::uint32_t*>(0x00882090u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00882094u + static_cast<unsigned>(iVar1)) = 0u;

    // Write -1.0f to per-slot field.                                     [0x0046bb82]
    *reinterpret_cast<std::uint32_t*>(0x00882098u + static_cast<unsigned>(iVar1)) = 0xbf800000u;

    // Zero per-slot field.                                               [0x0046bb8b]
    *reinterpret_cast<std::uint32_t*>(0x0088209cu + static_cast<unsigned>(iVar1)) = 0u;

    // Loop 1: 16 iterations, packed 3-dword entries starting at         [0x0046bb89]
    // (&DAT_008820d0)[param_1 * 0x341] = 0x008820d0 + iVar1.
    // Each entry: [element[-1]=0, element[0]=-1.0f, element[+1]=0], advance by 3.
    {
        std::uint32_t* puVar2 = reinterpret_cast<std::uint32_t*>(
            0x008820d0u + static_cast<unsigned>(iVar1));
        int iVar3 = 0x10;
        do {
            puVar2[-1] = 0u;
            *puVar2    = 0xbf800000u;
            puVar2[1]  = 0u;
            puVar2 += 3;
            iVar3 -= 1;
        } while (iVar3 != 0);
    }

    // Zero additional fields (iVar1 stride).                            [0x0046bbba]
    *reinterpret_cast<std::uint32_t*>(0x0088218cu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881f64u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881f60u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881f5cu + static_cast<unsigned>(iVar1)) = 0u;

    // Zero per-slot fields (0x341 stride).                              [0x0046bbcc]
    *reinterpret_cast<std::uint32_t*>(0x00881f58u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881f54u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881f50u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero physics fields (iVar1 stride).                               [0x0046bbde]
    *reinterpret_cast<std::uint32_t*>(0x00882088u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00882084u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00882080u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x0088207cu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00882078u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00882074u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero per-slot fields (0x341 stride).                              [0x0046bbfa]
    *reinterpret_cast<std::uint32_t*>(0x00881f88u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881f84u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero physics fields (iVar1 stride).                               [0x0046bc0c]
    *reinterpret_cast<std::uint32_t*>(0x008820a8u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008820a4u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008820a0u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero per-slot fields (0x341 stride).                              [0x0046bc1e]
    *reinterpret_cast<std::uint32_t*>(0x00881f8cu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881f90u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero physics field (iVar1 stride).                                [0x0046bc30]
    *reinterpret_cast<std::uint32_t*>(0x00881f94u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00882070u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881a30u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881a34u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008820c4u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008820c8u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero per-slot field (0x341 stride).                               [0x0046bc52]
    *reinterpret_cast<std::uint32_t*>(0x0088208cu + static_cast<unsigned>(iVar1)) = 0u;

    // Zero physics fields (iVar1 stride).                               [0x0046bc5a]
    *reinterpret_cast<std::uint32_t*>(0x008820acu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008820b0u + static_cast<unsigned>(iVar1)) = 0u;

    // Loop 2: 18 iterations, stride 0x40 (64 bytes = 0x10 uint32_t elements). [0x0046bc96]
    // Writes 0xffffffff to &DAT_00881a4c + iVar1, then +0x40, +0x80, ...
    {
        std::uint32_t* puVar2 = reinterpret_cast<std::uint32_t*>(
            0x00881a4cu + static_cast<unsigned>(iVar1));
        int iVar3 = 0x12;
        do {
            *puVar2  = 0xffffffffu;
            puVar2  += 0x10;
            iVar3   -= 1;
        } while (iVar3 != 0);
    }

    // Zero per-slot entity table fields (0x341 stride).                 [0x0046bcb5]
    *reinterpret_cast<std::uint32_t*>(0x008815b4u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815b8u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815bcu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815d4u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815d0u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815ccu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815c8u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815c4u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815c0u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815ecu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815e8u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815e4u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815e0u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815dcu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815d8u + static_cast<unsigned>(iVar1)) = 0u;

    return 1u; // success [0x0046bcd9]
}

RH_ScopedInstall(VehiclePhysicsStateReset, 0x0046baa0);
