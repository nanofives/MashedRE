// Mashed RE — AI driver cluster: per-frame orchestration spine (WS-C2).
//
// Verbatim ports of the integer/dispatch backbone of the FUN_00418860
// opponent-AI cluster:
//   0x00418860  AiTickLoop              per-frame 4-vehicle iterator
//   0x00418560  AiVehicleStep           per-vehicle bank-select + dispatch + ctrl
//   0x00417640  AiPostStepPowerupBrake  post-step powerup-brake override
//
// These reproduce the original control flow byte-for-byte. The float-heavy
// decision functions they invoke — the primary control step (FUN_00416250),
// its 4/9 (FUN_00416a30) and 8 (FUN_00417da0) variants, the pre-tick
// rubber-band (FUN_004177b0), the bank switcher (FUN_00417180), and the
// targeting/LOS leaves — are still called by RVA (original code, live in the
// .asi) pending their own ports (ai_controller.md §9 steps 4-7). This file is
// the dev .asi (hook) target only; the standalone exe keeps the scaffold until
// the full callee closure + tuning-const harvest land.
//
// Verification: C3 via snapshot/restore A/B drivers (AiControllerAB.cpp,
// AbAiPost/AbAiVehStep/AbAiTick) installed at the target RVAs in live scenario
// races, 2026-07-02 — all three GREEN (0 confirmed mismatches across ~44k
// paired calls; per-branch coverage counters in log/ai_ab_*.log). The rule-8
// run caught + fixed a mode-8 dispatch bug (FUN_00417da0 takes the same 4 args
// as FUN_00416250; the C1 plate's no-arg reading was wrong).
//
// Byte→axis map: re/analysis/ai_ctrl_byte_map_RESOLVED_2026-06-16.md
//   [0]/[1]=steer  [3]=fire  [4]=accel  [5]=brake  [6]/[7]=scratch
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Source decompilations (pool5, 2026-06-16):
//   re/analysis/ai_path_following/0x00418860.md  + this session's exact decomp
//   re/analysis/ai_update/0x00418560.md          + this session's exact decomp
//   re/analysis/ai_update/0x00417640.md          + this session's exact decomp

#include "../Core/HookSystem.h"
#include "AiState.h"

#include <cstdint>

using namespace Ai;

namespace {

// --- typed call-throughs to original callees (resolve to live RVAs in .asi) --
// __cdecl matches the cluster's calling convention (Ghidra default; consistent
// with the existing Ai/PromoLoop_round1.cpp ports).
typedef int   (__cdecl* fn_iv_t)(void);          // int  f(void)
typedef int   (__cdecl* fn_ii_t)(int);           // int  f(int)
typedef void  (__cdecl* fn_vv_t)(void);          // void f(void)
typedef void  (__cdecl* fn_vi_t)(int);           // void f(int)
typedef void  (__cdecl* fn_vii_t)(int, int);     // void f(int,int)
typedef float (__cdecl* fn_fv_t)(void);          // float f(void)
typedef float (__cdecl* fn_ffp_t)(float*);       // float f(float*)
typedef void  (__cdecl* fn_pii_t)(void*, int);   // void f(ptr,int)
typedef std::uint8_t* (__cdecl* fn_pv_t)(void);  // ptr f(void)
// control step: f(spline, vehicle, ctrl, arg4=0x42c80000)
typedef void  (__cdecl* fn_step_t)(void*, int, std::uint8_t*, std::uint32_t);

inline int  call_0042f6a0() { return reinterpret_cast<fn_iv_t>(0x0042f6a0)(); } // round type
inline int  call_0046c7b0(int v) { return reinterpret_cast<fn_ii_t>(0x0046c7b0)(v); } // alive
inline void call_0040e480(int a, int b) { reinterpret_cast<fn_vii_t>(0x0040e480)(a, b); } // CarSlotStateSet
inline void call_004177b0() { reinterpret_cast<fn_vv_t>(0x004177b0)(); } // pre-tick
inline int  call_0040e470(int v) { return reinterpret_cast<fn_ii_t>(0x0040e470)(v); } // veh type
inline int  call_00443080() { return reinterpret_cast<fn_iv_t>(0x00443080)(); } // AI-targeting enable
inline int  call_0040e350() { return reinterpret_cast<fn_iv_t>(0x0040e350)(); } // sub game mode
inline void call_00413fe0() { reinterpret_cast<fn_vv_t>(0x00413fe0)(); } // AI state reset
inline int  call_0040e4a0() { return reinterpret_cast<fn_iv_t>(0x0040e4a0)(); } // elapsed time
inline void call_00417180(int v) { reinterpret_cast<fn_vi_t>(0x00417180)(v); } // bank switcher
inline int  call_00426c00() { return reinterpret_cast<fn_iv_t>(0x00426c00)(); } // track index
inline int  call_00407a40(int v) { return reinterpret_cast<fn_ii_t>(0x00407a40)(v); } // per-veh data
inline float call_00452eb0() { return reinterpret_cast<fn_fv_t>(0x00452eb0)(); }
inline int  call_00452ea0(int v) { return reinterpret_cast<fn_ii_t>(0x00452ea0)(v); }
inline std::uint8_t* call_00452160() { return reinterpret_cast<fn_pv_t>(0x00452160)(); }
inline void call_0046d6d0(float* o, int v) { reinterpret_cast<fn_pii_t>(0x0046d6d0)(o, v); }
inline void call_0046d570(float* o, int v) { reinterpret_cast<fn_pii_t>(0x0046d570)(o, v); }
inline void call_0046d4a0(int* o, int v) { reinterpret_cast<fn_pii_t>(0x0046d4a0)(o, v); }
inline float call_004c3ac0(float* p) { return reinterpret_cast<fn_ffp_t>(0x004c3ac0)(p); }
// control steps + per-vehicle step + post-step are called via their RVAs so a
// per-function diff toggle (or a later port-hook) is honoured transparently.
inline void call_00416a30(void* s, int v, std::uint8_t* c) {
    reinterpret_cast<fn_step_t>(0x00416a30)(s, v, c, kCtrlStepArg4);
}
inline void call_00416250(void* s, int v, std::uint8_t* c) {
    reinterpret_cast<fn_step_t>(0x00416250)(s, v, c, kCtrlStepArg4);
}
// Mode-8 variant takes the SAME 4 args: the original pushes
// (puVar9, vehicle, ctrl, 0x42c80000) BEFORE the mode-8 branch at 0x00418776..
// 0x0041877d and both call sites (0x00418780 FUN_00417da0 / 0x004187b7
// FUN_00416250) consume them; FUN_00417da0 reads [ebp+8]/[ebp+0xc] at
// 0x00417de0/0x00417db1. The C1 plate's no-arg reading was WRONG (caught
// 2026-07-02 by the AbAiVehStep rule-8 A/B run: the no-arg call AV'd).
inline void call_00417da0(void* s, int v, std::uint8_t* c) {
    reinterpret_cast<fn_step_t>(0x00417da0)(s, v, c, kCtrlStepArg4);
}
inline void call_00417640(int v, std::uint8_t* c) {
    reinterpret_cast<void(__cdecl*)(int, std::uint8_t*)>(0x00417640)(v, c);
}
inline void call_00418560(int v, int t) {
    reinterpret_cast<fn_vii_t>(0x00418560)(v, t);
}

// Per-vehicle behaviour record helpers (stride 0x74; decomp indexes *0x1d dwords).
inline std::uintptr_t st(std::uintptr_t field, int v) {
    return field + static_cast<std::uintptr_t>(v) * 0x74u;
}

} // namespace

// ===========================================================================
// 0x00417640  AiPostStepPowerupBrake(vehicle, ctrl)
//
// On track 0x21 (FUN_00426c00), when the per-vehicle powerup predicate fires
// (FUN_00452ea0) and the held-type float (FUN_00452eb0) is <= _DAT_005cc72c and
// the committed mode (DAT_0089a52c[v]) is not 10, overrides accel/brake: full
// brake when the rate/range floats clear the gate, else clears both.
// Cited: 0x00417640 body (this session's decomp).
// ===========================================================================
extern "C" __declspec(dllexport)
void __cdecl AiPostStepPowerupBrake(int param_1, std::uint8_t* param_2)
{
    if (call_00426c00() != 0x21) return;                                   // track gate
    if (call_00452eb0() > F32(0x005cc72cu)) return;                        // held-type float gate
    if (I32(st(0x0089a52cu, param_1)) == 10) return;                      // mode-10 skip
    if (call_00452ea0(param_1) == 0) return;                              // powerup predicate

    float* pfVar2 = reinterpret_cast<float*>(call_00452160());
    float local_14, local_10;
    int   local_18;
    call_0046d6d0(&local_14, param_1);   // rate-like float
    call_0046d570(&local_10, param_1);   // range-like float
    call_0046d4a0(&local_18, param_1);   // own struct ptr

    float local_c = pfVar2[0] - F32(static_cast<std::uintptr_t>(local_18) + 0x30u);
    float local_8 = pfVar2[1] - F32(static_cast<std::uintptr_t>(local_18) + 0x34u);
    float local_4 = pfVar2[2] - F32(static_cast<std::uintptr_t>(local_18) + 0x38u);
    (void)local_8; (void)local_4;
    call_004c3ac0(&local_c);             // length (return discarded, as in original)

    if (F32(0x005cc568u) < local_14 && local_10 < F32(0x005ccad0u)) {
        param_2[4] = 0;                  // accel off
        param_2[5] = 0xff;               // full brake
        return;
    }
    param_2[4] = 0;                      // accel off
    param_2[5] = 0;                      // brake off
}

RH_ScopedInstall(AiPostStepPowerupBrake, 0x00417640);

// ===========================================================================
// 0x00418560  AiVehicleStep(vehicle [, type-unused])
//
// Resolves the output slot, zeroes the steer/accel/brake/scratch bytes, then
// either (mode 5) applies the startup-countdown accel hold, or selects the AI
// line-bank, dispatches to the control step variant by game mode, runs the
// post-step, and replays/records the override-input bytes.
// Cited: 0x00418560 body (this session's decomp).
// Declared single-param; FUN_00418860 pushes a 2nd arg which __cdecl discards.
// ===========================================================================
extern "C" __declspec(dllexport)
void __cdecl AiVehicleStep(int param_1)
{
    int iVar7 = I32(kSlotTableBase + static_cast<std::uintptr_t>(param_1) * kSlotTableStride);
    int iVar11 = iVar7 * 0x4c;
    std::uint8_t* pbVar12 = reinterpret_cast<std::uint8_t*>(kCtrlBlockBase + static_cast<std::uintptr_t>(iVar7) * 0x4cu);

    // Zero ctrl bytes [4],[5],[0],[1],[7],[6] (block = 0x7f1038 + iVar7*0x4c).
    U8(0x007f103cu + static_cast<std::uintptr_t>(iVar7) * 0x4cu) = 0;       // [4] accel
    U8(0x007f103cu + static_cast<std::uintptr_t>(iVar11) + 1u) = 0;         // [5] brake
    pbVar12[0] = 0;                                                         // [0] steer
    U8(0x007f1038u + static_cast<std::uintptr_t>(iVar11) + 1u) = 0;         // [1] steer
    U8(0x007f103cu + static_cast<std::uintptr_t>(iVar11) + 3u) = 0;         // [7] scratch
    U8(0x007f103cu + static_cast<std::uintptr_t>(iVar11) + 2u) = 0;         // [6] scratch

    int iVar8 = call_0040e350();
    if (iVar8 == 5) {
        U32(kFrame0ff8) = 0;
        call_00413fe0();
        int elapsed = call_0040e4a0();
        int cd = I32(st(kAiMode5Countdown, param_1));
        if (cd < 0) {
            if (elapsed < (0x4a - cd) * 100) {
                U8(0x007f103cu + static_cast<std::uintptr_t>(iVar7) * 0x4cu) = 0xff; // [4]=0xff
                return;
            }
        } else if (elapsed < (cd + 0x40) * 100) {
            U8(0x007f103cu + static_cast<std::uintptr_t>(iVar7) * 0x4cu) = 0xff;     // [4]=0xff
            return;
        }
        return;
    }

    // mode != 5
    call_00417180(param_1);
    int iVar8b = param_1 * 0x74;
    void* puVar9 = reinterpret_cast<void*>(kSplineRace);

    if (2 < I32(st(kAiSplineIndex, param_1))) {
        I32(st(kAiSplineIndex, param_1)) = 0;
    }
    int iVar10 = I32(st(kAiLineType, param_1));

    if (iVar10 == 0) {
        int idx = I32(st(kAiSplineIndex, param_1));
        if (I32(kSplineRaceCnt + static_cast<std::uintptr_t>(idx) * kSplineStride) < 4) {
            I32(st(kAiSplineIndex, param_1)) = 0;
        }
        idx = I32(st(kAiSplineIndex, param_1));
        puVar9 = reinterpret_cast<void*>(kSplineRace + static_cast<std::uintptr_t>(idx) * kSplineStride);
    }
    if (iVar10 == 2) {
        int idx = I32(st(kAiSplineIndex, param_1));
        if (I32(kSplineSlowCnt + static_cast<std::uintptr_t>(idx) * kSplineStride) < 4) {
            I32(st(kAiSplineIndex, param_1)) = 0;
        }
        idx = I32(st(kAiSplineIndex, param_1));
        if (I32(kSplineSlowCnt + static_cast<std::uintptr_t>(idx) * kSplineStride) < 4) {
            puVar9 = reinterpret_cast<void*>(kSplineRace);
        } else {
            puVar9 = reinterpret_cast<void*>(kSplineSlow + static_cast<std::uintptr_t>(idx) * kSplineStride);
        }
    }
    if (iVar10 == 1) {
        int idx = I32(st(kAiSplineIndex, param_1));
        if (I32(kSplineInsideCnt + static_cast<std::uintptr_t>(idx) * kSplineStride) < 4) {
            I32(st(kAiSplineIndex, param_1)) = 0;
        }
        idx = I32(st(kAiSplineIndex, param_1));
        if (I32(kSplineInsideCnt + static_cast<std::uintptr_t>(idx) * kSplineStride) < 4) {
            puVar9 = reinterpret_cast<void*>(kSplineRace);
        } else {
            puVar9 = reinterpret_cast<void*>(kSplineInside + static_cast<std::uintptr_t>(idx) * kSplineStride);
        }
    }
    if (iVar10 == 3) {
        int idx = I32(st(kAiSplineIndex, param_1));
        if (I32(kSplineCheatCnt + static_cast<std::uintptr_t>(idx) * kSplineStride) < 4) {
            I32(st(kAiSplineIndex, param_1)) = 0;
        }
        idx = I32(st(kAiSplineIndex, param_1));
        if (I32(kSplineCheatCnt + static_cast<std::uintptr_t>(idx) * kSplineStride) < 4) {
            puVar9 = reinterpret_cast<void*>(kSplineRace);
        } else {
            puVar9 = reinterpret_cast<void*>(kSplineCheat + static_cast<std::uintptr_t>(idx) * kSplineStride);
        }
    }

    if (I32(kDbgSplineEn) == 1) {
        int dbgIdx = I32(kDbgSplineIdx);
        switch (I32(kDbgSplineType)) {
        default: puVar9 = reinterpret_cast<void*>(kSplineRace   + static_cast<std::uintptr_t>(dbgIdx) * kSplineStride); break;
        case 1:  puVar9 = reinterpret_cast<void*>(kSplineInside + static_cast<std::uintptr_t>(dbgIdx) * kSplineStride); break;
        case 2:  puVar9 = reinterpret_cast<void*>(kSplineSlow   + static_cast<std::uintptr_t>(dbgIdx) * kSplineStride); break;
        case 3:  puVar9 = reinterpret_cast<void*>(kSplineCheat  + static_cast<std::uintptr_t>(dbgIdx) * kSplineStride); break;
        }
    }

    int fd0 = I32(kGameModeFd0);
    if (fd0 == 4 || fd0 == 9) {
        call_00416a30(puVar9, param_1, pbVar12);
    } else if (fd0 == 8) {
        call_00417da0(puVar9, param_1, pbVar12);
        if (call_00426c00() == 0x21) {
            int dv = call_00407a40(param_1);
            if (dv < 0x44 && 0x37 < dv) {
                U8(0x007f103cu + static_cast<std::uintptr_t>(iVar11) + 1u) = 0;   // [5]=0
            }
            if (dv < 0x5f && 0x58 < dv) {
                U8(0x007f103cu + static_cast<std::uintptr_t>(iVar11) + 1u) = 0;   // [5]=0
            }
        }
    } else {
        call_00416250(puVar9, param_1, pbVar12);
    }

    call_00417640(param_1, pbVar12);

    if (I32(0x0089a4fcu + static_cast<std::uintptr_t>(iVar8b)) != 0) {
        int t = I32(0x0089a4fcu + static_cast<std::uintptr_t>(iVar8b)) - I32(kOverrideStep);
        I32(0x0089a4fcu + static_cast<std::uintptr_t>(iVar8b)) = t;
        if (t < 0) {
            U32(0x0089a4fcu + static_cast<std::uintptr_t>(iVar8b)) = 0;
        }
        std::uint8_t b1 = U8(0x0089a510u + static_cast<std::uintptr_t>(iVar8b));
        std::uint8_t b4 = U8(0x0089a514u + static_cast<std::uintptr_t>(iVar8b));
        pbVar12[0]      = U8(0x0089a50cu + static_cast<std::uintptr_t>(iVar8b));   // [0]
        std::uint8_t b5 = U8(0x0089a518u + static_cast<std::uintptr_t>(iVar8b));
        U8(0x007f1038u + static_cast<std::uintptr_t>(iVar11) + 1u) = b1;           // [1]
        U8(0x007f103cu + static_cast<std::uintptr_t>(iVar7) * 0x4cu) = b4;         // [4]
        U8(0x007f103cu + static_cast<std::uintptr_t>(iVar11) + 1u) = b5;           // [5]
    }

    // Record current [1],[4],[0],[5] into the replay slots (+0x40/44/48/4c).
    std::uint8_t cur1 = U8(0x007f1038u + static_cast<std::uintptr_t>(iVar11) + 1u);
    std::uint8_t cur4 = U8(0x007f103cu + static_cast<std::uintptr_t>(iVar7) * 0x4cu);
    U32(0x0089a50cu + static_cast<std::uintptr_t>(iVar8b)) = static_cast<std::uint32_t>(pbVar12[0]);
    std::uint8_t cur5 = U8(0x007f103cu + static_cast<std::uintptr_t>(iVar11) + 1u);
    U32(0x0089a510u + static_cast<std::uintptr_t>(iVar8b)) = static_cast<std::uint32_t>(cur1);
    U32(0x0089a514u + static_cast<std::uintptr_t>(iVar8b)) = static_cast<std::uint32_t>(cur4);
    U32(0x0089a518u + static_cast<std::uintptr_t>(iVar8b)) = static_cast<std::uint32_t>(cur5);
}

RH_ScopedInstall(AiVehicleStep, 0x00418560);

// ===========================================================================
// 0x00418860  AiTickLoop(void)
//
// Per-frame entry. Guarded on race-line count > 3. In race/battle rounds marks
// AI cars 1..3 alive (CarSlotStateSet), runs the pre-tick rubber-band, then for
// each of vehicles 0..3 steps the alive AI vehicles; in DAT_007f0fd0==7 it also
// force-steps vehicle 0.
// Cited: 0x00418860 body (this session's decomp).
// ===========================================================================
extern "C" __declspec(dllexport)
void __cdecl AiTickLoop(void)
{
    if (3 < I32(kSplineRaceCnt)) {                                          // DAT_00801ca0 > 3
        int r = call_0042f6a0();
        if (r == 3 || (r = call_0042f6a0(), r == 4) || (r = call_0042f6a0(), r == 5) ||
            (r = call_0042f6a0(), r == 10) || I32(kGameModeFd0) == 4 ||
            I32(kGameModeFd0) == 9 || I32(kGameModeFd0) == 8 || I32(kGameModeFd0) == 10) {
            if (call_0046c7b0(1) == 1) call_0040e480(1, 2);
            if (call_0046c7b0(2) == 1) call_0040e480(2, 2);
            if (call_0046c7b0(3) == 1) call_0040e480(3, 2);
        }
        call_004177b0();
        int v = 0;
        do {
            int t = call_0040e470(v);
            if ((t != 0 && t != 1) || call_00443080() == 1) {
                if (call_0046c7b0(v) == 1) {
                    call_00418560(v, t);
                }
                int r3 = call_0042f6a0();
                if ((r3 == 3 || (r3 = call_0042f6a0(), r3 == 4) || (r3 = call_0042f6a0(), r3 == 5)) &&
                    I32(kGameModeFd0) == 7) {
                    call_00418560(0, t);
                }
            }
            v = v + 1;
        } while (v < 4);
    }
}

RH_ScopedInstall(AiTickLoop, 0x00418860);
