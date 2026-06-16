// Mashed RE — WS-B2/B3 contact-solver tuning constants + batch layouts.
//
// EVERY value here was read this session from the anchored binary via Ghidra
// MCP (pool3, read_only). MASHED.exe SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (preserved in original\MASHED.exe.unpatched).
//
// Map + provenance: re/analysis/WSB2_B3_CONTACT_PORT_MAP.md
//
// These are the `_DAT_005c*` / `_DAT_0061*` scalars the contact solvers read.
// Each constant cites the EXACT address it lives at and the raw 4-byte value.
// Doubles (005cea90/98) are noted — the decompiler reads them with a `(float)`
// truncating cast.
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Collision {

// --- universal epsilon / zero compare (read in every solver) ---------------
constexpr float kZero          = 0.0f;        // DAT_005d757c   = 0x00000000
constexpr float kOne           = 1.0f;        // _DAT_005cc320  = 0x3f800000

// --- car<->car (FUN_00469df0) ----------------------------------------------
constexpr float kCC_ImpulseHalf   = 0.5f;       // _DAT_005cc32c  0x3f000000
constexpr float kCC_SpinStopScale = 0.01f;      // _DAT_005cc328  0x3c23d70a
constexpr float kCC_ImpulseLoClamp= -1.0f;      // _DAT_005cc33c  0xbf800000
constexpr float kCC_UIntFixup     = 4.2949673e9f; // _DAT_005cc94c 0x4f800000 (2^32)
constexpr float kCC_AngToLin      = 600.0f;     // _DAT_005cea50  0x44160000
constexpr float kCC_ImpactInScale = 1.99940e-7f;// _DAT_005cea4c  0x3456bf95
constexpr float kCC_Mag_LT3players= 100.0f;     // _DAT_005cc568  0x42c80000
constexpr float kCC_Mag_34players = 200.0f;     // _DAT_005ce194  0x43480000 (also floor)
constexpr float kCC_ImpulseScale  = -0.5f;      // _DAT_005cd50c  0xbf000000
constexpr float kCC_SpinScale     = 0.0005f;    // _DAT_005ce268  0x3a03126f
// bounce/impact timer literal:  param_1[699] = 0x1e0 (480)
constexpr std::int32_t kCC_BounceTimer = 0x1e0;

// --- terrain solver (FUN_00468d80) -----------------------------------------
constexpr float kTer_DepthThresh  = 0.42f;      // _DAT_005cea48  0x3ed70a3d
constexpr float kTer_DepthMin     = -0.1f;      // _DAT_005cd0fc  0xbdcccccd
constexpr float kTer_VelScale     = 360.0f;     // _DAT_005ccac4  0x43b40000
constexpr float kTer_VelScale2    = 0.277779f;  // _DAT_005cea44  0x3e8e38e4
constexpr float kTer_MagThresh    = 9.99999e-5f;// _DAT_005cd03c  0x38d1b717
constexpr float kArm_SandSpWheel  = 3.6f;       // s_Afff_SandSpWheel_005cc753._1_4_ @0x005cc754 = 0x40666666
constexpr float kAxisAngle90      = 90.0f;      // literal 0x42b40000 (FUN_004c4d20 angle arg)

// --- object solver (FUN_004694e0) ------------------------------------------
constexpr float kObj_DepthMin     = -0.4f;      // _DAT_005cd30c  0xbecccccd
constexpr float kObj_VelCorrScale = -10.0f;     // _DAT_005cca40  0xc1200000
constexpr float kObj_ImpulseScale = 100000.0f;  // _DAT_005ce23c  0x47c35000

// --- wheel classifier (FUN_0046cc40) ---------------------------------------
constexpr float kCls_DepthLo      = -0.25f;     // _DAT_005cea5c  0xbe800000
constexpr float kCls_DepthHi      = 0.25f;      // _DAT_005cc564  0x3e800000
constexpr float kCls_ApproachThr  = 0.3f;       // _DAT_005cc99c  0x3e99999a

// --- wheel solver (FUN_0046f6c0; ported here as the WS-A driver reference) --
constexpr float kWhl_TorqueScale  = 0.277779f;  // _DAT_005cea60  0x3e8e3bcd
constexpr float kWhl_SpringToState2 = -2.0f;    // _DAT_005cc34c  0xc0000000
constexpr float kWhl_StateToZero  = 0.02f;      // _DAT_005ce18c  0x3ca3d70a
constexpr float kWhl_FwdRangeHi   = 0.1f;       // _DAT_005cea90  (DOUBLE 0x3fb999...) cast (float)
constexpr float kWhl_FwdRangeLo   = -0.1f;      // _DAT_005cea98  (DOUBLE 0xbfb999...) cast (float)
constexpr float kWhl_StateCmp     = -0.005f;    // _DAT_005ceaa0  0xbba3d70a
constexpr float kWhl_HiSpeedProj  = 0.998901f;  // _DAT_005cea88  0x3f7fbe77
constexpr float kWhl_RadToDeg     = 57.29578f;  // _DAT_005cc98c  0x42652ee1 (180/pi)
constexpr float kWhl_RotClamp     = 2.5f;       // _DAT_005cd088  0x40200000
constexpr float kWhl_FricVelScale = 1000.0f;    // _DAT_005cc9fc  0x447a0000
constexpr float kWhl_LowSpeedThr  = 0.7f;       // _DAT_005cc340  0x3f333333
constexpr float kWhl_FricImpScale = 10.0f;      // _DAT_005cc55c  0x41200000
constexpr float kWhl_FricAngScale = 9.98199e-6f;// _DAT_005cc990  0x3727c5ac
constexpr float kWhl_GroundedThr  = 2.0f;       // _DAT_005cc574  0x40000000
constexpr float kWhl_DriftScale   = -20.0f;     // _DAT_005cd61c  0xc1a00000
constexpr float kWhl_DriftAngScale= -0.0625f;   // _DAT_005cea84  0xbd800000
constexpr float kWhl_ResetSpring  = 10.0f;      // literal 0x41200000 (puVar6[0]=10.0f reset)
// world-up vector used in the airborne-drift cross product (FUN_0046f6c0):
constexpr float kUpX = 0.0f;  // _DAT_006146fc 0x00000000
constexpr float kUpY = 1.0f;  // _DAT_00614700 0x3f800000
constexpr float kUpZ = 0.0f;  // _DAT_00614704 0x00000000

// --- dispatcher (FUN_00469aa0) ---------------------------------------------
// gravity/normal probe vector passed to the RW contact query:
//   local_4c = {0.0f, 0.1f, 0.0f}   (0x3dcccccd = 0.1f)
constexpr float kGravProbeY = 0.1f;             // literal 0x3dcccccd

// --- entry-loop distance gate (FUN_004709a0) -------------------------------
constexpr float kEntry_RadiusScale = 0.75f;     // _DAT_005cc950  0x3f400000

// ---------------------------------------------------------------------------
// Contact-batch entry layout (the 0x90-byte / 36-float record the terrain and
// classifier solvers iterate; count = DAT_0088e60c, base filled by the RW
// broadphase walk FUN_00538c80 + callback LAB_00468b80 — see the port map).
//
// Float-index semantics, derived from FUN_00468d80 / FUN_0046cc40 reads:
//   [0..2]   triangle vertex A (x,y,z)
//   [3..5]   triangle vertex B
//   [6..8]   triangle vertex C
//   [9..0xb] unit face normal
//   [0xc..0xd] per-contact surface metadata (material id? — [UNCERTAIN U-3630])
//   [0xd]    sentinel field: -1.7147562e+38 (first-frame) / -1.7014636e+38
//            (persistent) gate the debug-event class (no gameplay effect)
//   [0x19..0x21] three half-plane edge normals (SAT, 3 vec3) — FUN_00468d80
//   [0x1b..0x23] three half-plane edge normals (SAT layout in FUN_0046cc40)
//   [0x34 byte] contact key matched by the history scan FUN_00468b40
// The two solvers index this record slightly differently (0x19.. vs 0x1b..);
// both are preserved verbatim in the ports.
struct ContactBatchEntry { float f[36]; };   // stride 0x24 floats = 0x90 bytes
static_assert(sizeof(ContactBatchEntry) == 0x90, "batch entry is 0x90 bytes");

}  // namespace Collision
}  // namespace mashed_re
