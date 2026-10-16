// Mashed RE — AI post-step powerup-brake override (FUN_00417640): ONE body for both build targets (dualcopy3, 2026-10-16).
//
// Before this file the RVA had two bodies that had drifted apart (re/analysis/DUAL_COPY_AUDIT_2026-09-29.md, ROADMAP D4 item 2):
//   Ai/AiController.cpp  .asi-only, installed, the body the C3 evidence cites (A/B driver AbAiPost, live race, 2026-07-02): it read
//                        its operands through the ORIGINAL callees and could set ctrl[5] = 0xff
//   Ai/AiStandalone.cpp  exe+asi, `PostStepPowerupBrake`, the exe's copy: it PINNED `rate = 0.0f` ([U-C-RATE1]), so the gate
//                        `100.0 < rate` could never hold and the brake branch was unreachable, and it omitted FUN_0046d570
// The pinned rate was a port defect (parent decision 2026-10-16), not a deliberate substitute: FUN_0046d6d0 is record +0x9e4, which
// Host::veh_f32 already carries for FUN_00416250 (AiStandalone.cpp, `veh_f32(v, 0x9e4)`), and FUN_0046d570 has an exe port in
// Ai/AiLeafTargets.cpp. This TU is in BOTH exe_sources.rsp and asi_sources.rsp; RH_ScopedInstall installs it in the .asi and is a
// no-op in the exe. The AiController.cpp text is kept (it is what the evidence covers); the two copies are deleted.
//
// What the original does (plate re/analysis/ai_update/0x00417640.md, ctrl bytes re/analysis/ai_ctrl_byte_map_RESOLVED_2026-06-16.md):
//   0x00417640  if FUN_00426c00() != 0x21 return                      (the course id, 0x21 = 33 = Warzone, GameFlow.cpp kAreas)
//               if FUN_00452eb0() > _DAT_005cc72c (30.0f) return      (FUN_00452eb0 = *(float*)0x00684de0)
//               if DAT_0089a52c[v] == 10 return                       (committed mode, per-vehicle stride 0x74)
//               if FUN_00452ea0(v) == 0 return                        (FUN_00452ea0 = *(u32*)(0x0088ff50 + v*4))
//               rate  <- FUN_0046d6d0(&rate, v)   (record +0x9e4)     range <- FUN_0046d570(&range, v)
//               if _DAT_005cc568 (100.0f) < rate && range < _DAT_005ccad0 (90.0f): ctrl[4] = 0, ctrl[5] = 0xff   (full brake)
//               else                                                  ctrl[4] = 0, ctrl[5] = 0                  (coast)
// The original also computes a displacement to the target and calls FUN_004c3ac0 (Vec3Magnitude) on it (FUN_00452160 /
// FUN_0046d4a0 / FUN_004c3ac0). Its result is discarded and read by nothing afterwards, so it is elided here, the same
// dead-artifact precedent AiControlStep.cpp documents; the AiController.cpp port computed it and never used it either.
//
// Three thunks differ by target (the audit's P4: arithmetic core shared, thunks per target):
//   TrackIndex()  .asi: the original FUN_00426c00.   exe: Ai_HostTrackIndex() = Host::track_index (`g_aib.course`, the engine
//                 Course_Id; the exe's DAT_00644158 reads 0, Race/CameraEntries.cpp:26)
//   Rate(v)       .asi: the original FUN_0046d6d0.   exe: Ai_HostVehF32(v, 0x9e4), the same record field
//   Range(v)      .asi: the original FUN_0046d570.   exe: AiLeaf_46d570(v), the verbatim naked AL_46d570
// and the three thresholds: the original's .rdata in the .asi (the A/B driver seeds those addresses to force both write paths), the
// values read from the unpatched image in the exe, whose pad is zero-filled there (0x005cc72c = 0x41f00000 = 30.0f,
// 0x005cc568 = 0x42c80000 = 100.0f, 0x005ccad0 = 0x42b40000 = 90.0f).
//
// The three operand reads that are the original's one-line leaf bodies (the held float, the mode word, the predicate word) are direct
// reads in both targets; PowerupRangeGet (`fld [0x00684de0]`) and Table88ff50Get (`[0x0088ff50 + i*4]`) are C3, byte-verified.
//
// NOT in the exe yet: the PRODUCERS of the predicate word 0x0088ff50[v] and the held float 0x00684de0. No exe TU writes either
// (they belong to the unported powerup-seek FUN_00414f00), so in the exe gate 4 cannot hold and the brake branch stays unreachable
// even on course 0x21. The body is faithful; its inputs are the open item.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched; prologue 0x00417640: 83 ec 18 e8 b8 f5 00 00)

#include "../Core/HookSystem.h"
#include "AiState.h"
#include "AiStandalone.h"
#ifdef MASHED_STANDALONE
#include "AiLeafTargets.h"
#endif

#include <cstdint>

using namespace Ai;

namespace {

#ifdef MASHED_STANDALONE
inline int   TrackIndex()   { return Ai_HostTrackIndex(); }
inline float Rate(int v)    { return Ai_HostVehF32(v, 0x9e4); }          // FUN_0046d6d0: record +0x9e4
inline float Range(int v)   { return AiLeaf_46d570(v); }                 // FUN_0046d570
inline float HeldGate()     { return 30.0f; }                            // _DAT_005cc72c
inline float RateGate()     { return 100.0f; }                           // _DAT_005cc568
inline float RangeGate()    { return 90.0f; }                            // _DAT_005ccad0
#else
typedef int  (__cdecl* fn_iv_t)(void);
typedef void (__cdecl* fn_pii_t)(void*, int);
inline int   TrackIndex()   { return reinterpret_cast<fn_iv_t>(0x00426c00)(); }
inline float Rate(int v)    { float r = 0.0f; reinterpret_cast<fn_pii_t>(0x0046d6d0)(&r, v); return r; }
inline float Range(int v)   { float r = 0.0f; reinterpret_cast<fn_pii_t>(0x0046d570)(&r, v); return r; }
inline float HeldGate()     { return F32(0x005cc72cu); }
inline float RateGate()     { return F32(0x005cc568u); }
inline float RangeGate()    { return F32(0x005ccad0u); }
#endif

} // namespace

// ===========================================================================
// 0x00417640  AiPostStepPowerupBrake(vehicle, ctrl)
// ===========================================================================
extern "C" __declspec(dllexport)
void __cdecl AiPostStepPowerupBrake(int param_1, std::uint8_t* param_2)
{
    if (TrackIndex() != 0x21) return;                                       // 0x00417643 FUN_00426c00, cmp 0x21
    if (F32(0x00684de0u) > HeldGate()) return;                              // FUN_00452eb0 > _DAT_005cc72c
    if (I32(0x0089a52cu + static_cast<std::uintptr_t>(param_1) * 0x74u) == 10) return;  // mode-10 skip
    if (I32(0x0088ff50u + static_cast<std::uintptr_t>(param_1) * 4u) == 0) return;      // FUN_00452ea0 == 0

    const float rate  = Rate(param_1);                                      // FUN_0046d6d0
    const float range = Range(param_1);                                     // FUN_0046d570

    if (RateGate() < rate && range < RangeGate()) {                         // _DAT_005cc568 < rate && range < _DAT_005ccad0
        param_2[4] = 0;                                                     // accel off
        param_2[5] = 0xff;                                                  // full brake
        return;
    }
    param_2[4] = 0;                                                         // accel off
    param_2[5] = 0;                                                         // brake off
}
RH_ScopedInstall(AiPostStepPowerupBrake, 0x00417640);
