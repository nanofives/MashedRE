// Mashed RE — WS-R6 final drain: AI line-bank switcher + per-frame pre-tick,
// verbatim ports authored from raw asm (Mashed_pool13 RO, 2026-07-02):
//
//   0x00417180  AiBankSwitch(vehicle)   spline-index cycling / line-type bank
//                                       switching / switch timer / random
//                                       spline variation
//   0x004177b0  AiPreTick(void)         race-angle array update + candidate
//                                       slots + mode-4/9 speed rubber-banding
//                                       + powerup speed doubling + slow-line
//                                       flag state machine + probability rolls
//
// Raw-asm corrections over the C1 plates (cited to this session's listings):
//   0x00417180: the type-0 and type-3 branches call FUN_00414030(-1) even when
//     the count-check passes (JGE 0x0041723f / 0x0041727d land ON the call);
//     type-2/type-1 branches test the line type read once into ECX (0x004171a5)
//     while type-0/type-3 re-read memory (0x0041720c/0x00417249); after any
//     switch-request the timer is 1 so the timer branch returns that frame.
//   0x004177b0: the band ladder commits DAT_0089a370 (tick*_DAT_005cc948) and
//     DAT_0089a374 (band index 0..4) and zeroes 0x0089a4c4+0x0089a4c8 per
//     vehicle (0x00417c27..0x00417c3d — NOT 4c0/4c4 as the plate read);
//     probability stage is two-stage: table-1 (0x005f30a0) hit -> flag=1 +
//     zero 0x0089a4c0[v] (0x00417c8a..0x00417caa); table-1 miss + table-2
//     (0x005f3180) hit -> flag=2 (0x00417cde). Modes 4/9 -> flag=2
//     (0x00417cde), mode 8 -> flag=0 (0x00417adc).
//
// FPU protocol: FUN_0046dd80 / FUN_00408ad0 / FUN_00417730 / FUN_00472650
// return float10 in ST0 consumed by the caller (FSTP/FMUL/FCOM in the
// listings) -> declared float-returning (PC=24 value-identical). __ftol of
// DAT_0089a360 (0x00417b2a) forwarded to the original FUN_004a2c48 via the
// x87 FLD shim. FILD of DAT_007f0ff8 (0x00417b2f) is exact as (float) for
// race-scale tick values (< 2^24).
//
// Dead artifact NOT mirrored: MOV EDX,[ESP+0x10] @0x00417b6c reloads the
// stale phase-2 FUN_00407a20 scratch slot, but every path that consumes EDX
// (the 0x0089a374 store / table index) first passes through a band that sets
// it (0x00417b63/0x00417b90/0x00417bb5/0x00417bd8/0x00417c09) — the stale
// value is unreachable at any store.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (prologue bytes verified vs original\MASHED.exe.unpatched this session:
//    0x00417180: 53 55 56 8b 74 24 10 / 0x004177b0: 83 ec 08 53 55 56)

#include "../Core/HookSystem.h"
#include "AiState.h"

#include <cstdint>
#include <cstring>

using namespace Ai;

namespace {

inline float Cf(std::uint32_t bits) { float f; std::memcpy(&f, &bits, 4); return f; }
inline std::uintptr_t st74(std::uintptr_t base, int v) { return base + static_cast<std::uintptr_t>(v) * 0x74u; }

typedef int   (__cdecl* fn_iv_t)(void);
typedef int   (__cdecl* fn_ii_t)(int);
typedef void  (__cdecl* fn_vi_t)(int);
typedef float (__cdecl* fn_fv_t)(void);
typedef float (__cdecl* fn_fi_t)(int);
typedef void  (__cdecl* fn_vif_t)(int, float);
typedef float (__cdecl* fn_rand_t)(int, std::uint32_t);

inline void  SplineReset(int a)      { reinterpret_cast<fn_vi_t>(0x00414030)(a); }   // writes 0x008032d4+i*0x14 only (decomp this session)
inline float RandFloat(std::uint32_t hiBits) { return reinterpret_cast<fn_rand_t>(0x00472650)(0, hiBits); } // float10-ST0
inline int   TargetEnable()          { return reinterpret_cast<fn_iv_t>(0x00443080)(); }
inline float SpeedBaseGet()          { return reinterpret_cast<fn_fv_t>(0x0046dd80)(); }   // float10-ST0 (f32-sourced)
inline void  VehSpeedSet(int v, float f) { reinterpret_cast<fn_vif_t>(0x0046dd90)(v, f); }
inline int   PerVehA20(int v)        { return reinterpret_cast<fn_ii_t>(0x00407a20)(v); }
inline float RaceAngleRaw(int v)     { return reinterpret_cast<fn_fi_t>(0x00408ad0)(v); }  // float10-ST0
inline float RaceAngleGet(int v)     { return reinterpret_cast<fn_fi_t>(0x00417730)(v); }  // float10-ST0 (f32-sourced)
inline int   PowerupAlive(int v)     { return reinterpret_cast<fn_ii_t>(0x00454a30)(v); }
inline int   VehType(int v)          { return reinterpret_cast<fn_ii_t>(0x0040e470)(v); }
inline int   SubGameMode()           { return reinterpret_cast<fn_iv_t>(0x0040e350)(); }

void* g_ftol_4a2c48 = reinterpret_cast<void*>(0x004a2c48);
__declspec(naked) int Ftol(double /*x*/) {
    __asm {
        fld  qword ptr [esp + 4]
        call dword ptr [g_ftol_4a2c48]
        ret
    }
}

} // namespace

// ===========================================================================
// 0x00417180  AiBankSwitch(vehicle)
// ===========================================================================
extern "C" __declspec(dllexport)
void __cdecl AiBankSwitch(int veh)
{
    const std::uintptr_t s = static_cast<std::uintptr_t>(veh) * 0x74u;

    if (I32(0x0089a500u + s) != 0) {                       // 0x0041718a switch request
        int t = I32(0x0089a4ccu + s);                      // 0x004171a5 (read ONCE into ECX)
        I32(0x0089a504u + s) = 1;                          // 0x004171ad start timer
        if (t == 2) {                                      // 0x004171ab slow lines
            int idx = I32(0x0089a4d0u + s) + 1;            // 0x004171b5
            I32(0x0089a4d0u + s) = idx;                    // 0x004171c0
            if (idx > 2) I32(0x0089a4d0u + s) = 0;         // 0x004171c8
            SplineReset(-1);                               // 0x004171d0
        }
        if (t == 1) {                                      // 0x004171d8 inside lines
            int idx = I32(0x0089a4d0u + s) + 1;            // 0x004171e1
            I32(0x0089a4d0u + s) = idx;                    // 0x004171ea
            if (idx > 2 || I32(0x008022acu + static_cast<std::uintptr_t>(idx) * 0x204u) < 4) { // 0x004171f0/0x004171f8
                I32(0x0089a4ccu + s) = 2;                  // 0x00417200
                I32(0x0089a4d0u + s) = 0;                  // 0x00417206
            }
        }
        if (I32(0x0089a4ccu + s) == 0) {                   // 0x0041720c race lines (RE-READS memory)
            int idx = I32(0x0089a4d0u + s) + 1;            // 0x00417214
            I32(0x0089a4d0u + s) = idx;                    // 0x0041721d
            if (idx > 2 || I32(0x00801ca0u + static_cast<std::uintptr_t>(idx) * 0x204u) < 4) { // 0x00417223/0x0041722b
                I32(0x0089a4ccu + s) = 1;                  // 0x00417233
                I32(0x0089a4d0u + s) = 0;                  // 0x00417239
            }
            SplineReset(-1);                               // 0x00417241 (called on BOTH outcomes)
        }
        if (I32(0x0089a4ccu + s) == 3) {                   // 0x00417249 cheat lines (RE-READS memory)
            int idx = I32(0x0089a4d0u + s) + 1;            // 0x00417252
            I32(0x0089a4d0u + s) = idx;                    // 0x0041725b
            if (idx > 2 || I32(0x00802ec4u + static_cast<std::uintptr_t>(idx) * 0x204u) < 4) { // 0x00417261/0x00417269
                I32(0x0089a4ccu + s) = 0;                  // 0x00417271
                I32(0x0089a4d0u + s) = 0;                  // 0x00417277
            }
            SplineReset(-1);                               // 0x0041727f (called on BOTH outcomes)
        }
        I32(0x0089a500u + s) = 0;                          // 0x00417287 clear request
    }

    // switch timer (0x0041728d)
    int timer = I32(0x0089a504u + s);
    if (timer != 0 && timer < 9000) {                      // 0x00417293/0x00417297
        I32(0x0089a504u + s) = timer + I32(0x007f1008u);   // 0x0041729e..0x004172a7
        return;                                            // 0x004172b0
    }
    I32(0x0089a504u + s) = 0;                              // 0x004172b1

    // line-type reset by debug flag (0x004172b7)
    if (I32(0x0089a368u) == 0) I32(0x0089a4ccu + s) = 0;   // 0x004172b7/0x004172bf
    if (I32(0x0089a368u) == 2) I32(0x0089a4ccu + s) = 0;   // 0x004172c5/0x004172cd
    if (I32(0x0089a368u) == 1) {                           // 0x004172d3
        if (I32(0x0089a4ccu + s) != 2) SplineReset(-1);    // 0x004172db/0x004172e5
        I32(0x0089a4ccu + s) = 2;                          // 0x004172ed
    }

    // low-frequency random spline variation (0x004172f3)
    int period = I32(0x007f0ff8u) / 3000;                  // 0x004172f9 (signed /3000 idiom)
    if ((period & 0xf) == 0xf) {                           // 0x0041730e/0x00417311
        if (I32(0x0063bd90u) != period) {                  // 0x00417316 dedup
            float rnd = RandFloat(0x3f800000u);            // 0x00417324 FUN_00472650(0, 1.0f)
            if (rnd < F32(0x005cc32cu)) {                  // 0x00417329 (TEST 0x5/JP-skip)
                I32(0x0089a4d0u + s) = I32(0x0089a4d0u + s) + 1; // 0x00417339
                SplineReset(-1);                           // 0x00417348
            }
        }
        I32(0x0063bd90u) = period;                         // 0x00417350
    } else {
        I32(0x0063bd90u) = 0;                              // 0x0041735e
    }
}
RH_ScopedInstall(AiBankSwitch, 0x00417180);

// ===========================================================================
// 0x004177b0  AiPreTick(void)
// ===========================================================================
extern "C" __declspec(dllexport)
void __cdecl AiPreTick(void)
{
    if (TargetEnable() != 0) I32(0x0089a368u) = 2;         // 0x004177b6/0x004177c4

    // ── phase 2: race-angle array + candidate slots, vehicles 0..3 ──
    int fd0 = 0;                                           // EDX (reloaded per iteration @0x004177f9)
    for (int v = 0; v < 4; ++v) {
        float spd = SpeedBaseGet();                        // 0x004177d2
        VehSpeedSet(v, spd);                               // 0x004177dc (round-trips the f32)
        int a20 = PerVehA20(v);                            // 0x004177e2
        float a20f = static_cast<float>(a20);              // 0x004177eb FILD -> FSTP
        float ra = RaceAngleRaw(v);                        // 0x004177f4 (float10-ST0)
        fd0 = I32(0x007f0fd0u);                            // 0x004177f9
        float val = ra * F32(0x005cc328u) + a20f;          // 0x004177ff/0x0041780b
        F32(0x0089a880u + static_cast<std::uintptr_t>(v) * 4u) = val; // 0x0041780f FST (keeps ST0)
        if (fd0 == 4 || fd0 == 9 || fd0 == 7 || fd0 == 8) { // 0x00417808..0x00417825
            if (!(val < F32(0x005cc31cu))) {               // 0x0041782b (TEST AH,0x1/JNZ-skip)
                float fv = static_cast<float>(v);          // 0x0041783c FILD local
                int recorded = 0;                          // ECX
                if (F32(0x0089a870u) == fv) recorded = 1;  // 0x00417842..0x00417851 (FCOMP TEST 0x44)
                if (F32(0x0089a874u) == fv) recorded = 1;  // 0x00417856..
                if (F32(0x0089a878u) == fv) recorded = 1;  // 0x0041786a..
                if (F32(0x0089a87cu) == fv) {              // 0x0041787e JNP -> continue
                    // already in slot 4: nothing
                } else if (recorded == 0) {                // 0x0041788d
                    // store v into the first empty (== _DAT_005cc33c) slot
                    if      (F32(0x0089a870u) == F32(0x005cc33cu)) F32(0x0089a870u) = fv; // 0x00417891/0x004178a4
                    else if (F32(0x0089a874u) == F32(0x005cc33cu)) F32(0x0089a874u) = fv; // 0x004178ac/0x004178bf
                    else if (F32(0x0089a878u) == F32(0x005cc33cu)) F32(0x0089a878u) = fv; // 0x004178c7/0x004178da
                    else if (F32(0x0089a87cu) == F32(0x005cc33cu)) F32(0x0089a87cu) = fv; // 0x004178e2/0x004178f5
                }
            }
        }
    }

    // ── phase 3: mode-9 speed scaling, vehicle 1 (0x0041790d) ──
    if (fd0 == 9) {
        float mult = Cf(0x3f800000u);                      // 0x00417918 (1.0)
        float ang = RaceAngleGet(1);                       // 0x00417920 FUN_00417730(1)
        if (ang < F32(0x005cc348u)) mult = Cf(0x3f733333u); // 0x00417925/0x00417935 (0.95)
        if (ang < F32(0x005cc950u)) mult = Cf(0x3f666666u); // 0x0041793d/0x0041794a (0.9)
        if (ang < F32(0x005cc9a4u)) mult = Cf(0x3e800000u); // 0x00417952/0x0041795f (0.25)
        if (!(ang <= F32(0x005cc574u))) mult = Cf(0x3f933333u); // 0x00417967/0x00417974 (1.15)
        float spd = SpeedBaseGet();                        // 0x0041797c
        VehSpeedSet(1, spd * mult);                        // 0x00417981/0x0041798b
        fd0 = I32(0x007f0fd0u);                            // 0x00417990 EDX reload
    }

    // ── phase 3b: mode-4 speed scaling, vehicles 1..3 (0x00417999) ──
    if (fd0 == 4) {
        for (int v = 1; v < 4; ++v) {                      // 0x004179a2/0x00417a69
            float mult = Cf(0x3f800000u);                  // 0x004179b1 (1.0)
            float ang = RaceAngleGet(v);                   // 0x004179b9
            if (!(ang <= F32(0x005cc950u))) {              // 0x004179be (TEST 0x41/JNZ-skip)
                mult = (F32(0x005cc55cu) - F32(0x0089a360u)) * F32(0x005cc56cu)
                       * F32(0x005cc8f4u) + F32(0x005cc320u); // 0x004179ce..0x004179ec
            }
            if (!(ang <= F32(0x005cc574u))) {              // 0x004179f0
                mult = (F32(0x005cc55cu) - F32(0x0089a360u)) * F32(0x005cc56cu)
                       * F32(0x005ccac8u) + mult;          // 0x004179fd..0x00417a19
            }
            if (ang < F32(0x005cc32cu)) {                  // 0x00417a1d (TEST 0x5/JP-skip)
                mult = mult - F32(0x005cc8f0u);            // 0x00417a2a..0x00417a34
            }
            if (ang < F32(0x005cc9a0u)) {                  // 0x00417a38 (TEST 0x5/JP-skip)
                mult = mult - F32(0x005cc564u);            // 0x00417a45..0x00417a4f
            }
            float spd = SpeedBaseGet();                    // 0x00417a53
            VehSpeedSet(v, spd * mult);                    // 0x00417a58/0x00417a61
        }
    }

    // ── phase 4: powerup-alive speed doubling (0x00417a73) ──
    int any = 0;                                           // EDI
    for (int v = 0; v < 4; ++v) {                          // 0x00417a78..0x00417a8e
        if (PowerupAlive(v) != 0) any = 1;                 // 0x00417a79/0x00417a85
    }
    if (any != 0) {                                        // 0x00417a90
        for (int v = 0; v < 4; ++v) {                      // 0x00417a97..0x00417abc
            if (VehType(v) == 2) {                         // 0x00417a98/0x00417aa0
                float spd = SpeedBaseGet();                // 0x00417aa4
                VehSpeedSet(v, spd + spd);                 // 0x00417aa9 FADD ST0,ST0
            }
        }
    }

    // ── phase 5: slow-line flag state machine (0x00417abe) ──
    int m = I32(0x007f0fd0u);                              // 0x00417abe
    if (m == 4 || m == 9) { I32(0x0089a368u) = 2; return; } // 0x00417ac3/0x00417acc -> 0x00417cde
    if (m == 8)           { I32(0x0089a368u) = 0; return; } // 0x00417ad5/0x00417adc
    if (SubGameMode() != 6) return;                        // 0x00417ae7/0x00417aec

    if (I32(0x0089a368u) == 2) {                           // 0x00417af5
        int t = I32(0x0089a36cu) + I32(0x007f1008u);       // 0x00417afd/0x00417b02
        I32(0x0089a36cu) = t;                              // 0x00417b0d
        if (t > 180000) {                                  // 0x00417b08 (0x2bf20)
            I32(0x0089a368u) = 1;                          // 0x00417b14
            I32(0x0089a36cu) = 0;                          // 0x00417b1e
        }
    }

    int row = Ftol(static_cast<double>(F32(0x0089a360u))); // 0x00417b24/0x00417b2a FLD -> __ftol
    int ecx = 0, edx = 0;                                  // 0x00417b37 (edx band-defined before any use)
    float tickscale = static_cast<float>(I32(0x007f0ff8u)) * F32(0x005cc948u); // 0x00417b2f/0x00417b39
    float diff = tickscale - F32(0x0089a370u);             // 0x00417b3f/0x00417b45 FSUBR

    // band ladder (0x00417b47..0x00417c15); 'done' exits fall to the commit check
    if (!(diff <= F32(0x005cc320u))) {                     // 0x00417b47 (TEST 0x41/JNZ-skip)
        if (tickscale < F32(0x005cc574u)) { edx = 0; ecx = 1; } // 0x00417b56 (TEST 0x5/JP-skip)
    }
    if (!(diff <= F32(0x005cc55cu))) {                     // 0x00417b70 (TEST 0x41/JNZ-done)
        if (tickscale < F32(0x005cc354u)) { edx = 1; ecx = 1; } // 0x00417b83/0x00417b90
        if (!(tickscale <= F32(0x005ccd6cu))) {            // 0x00417b99 (TEST 0x41/JNZ-skip)
            if (tickscale < F32(0x005cd0f8u)) { edx = 2; ecx = 1; } // 0x00417ba8/0x00417bb5
        }
        if (!(diff <= F32(0x005ccd6cu))) {                 // 0x00417bbc (TEST 0x41/JNZ-done)
            if (tickscale < F32(0x005cd0f4u)) { edx = 3; ecx = 1; } // 0x00417bcb/0x00417bd8
            if (!(diff <= F32(0x005ccd6cu))) {             // 0x00417be2 (redundant recheck; mirrored)
                if (!(tickscale <= F32(0x005cc728u)) &&    // 0x00417bef (TEST 0x41/JNZ-done)
                    tickscale < F32(0x005cd0f0u)) {        // 0x00417bfc (TEST 0x5/JP-done)
                    edx = 4; ecx = 1;                      // 0x00417c09
                }
            }
        }
    }

    if (ecx != 0) {                                        // 0x00417c17 (band-5 jumps straight in)
        F32(0x0089a370u) = tickscale;                      // 0x00417c1b FSTP
        I32(0x0089a374u) = edx;                            // 0x00417c21
        for (std::uintptr_t a = 0x0089a4c4u; a < 0x0089a694u; a += 0x74u) { // 0x00417c27..0x00417c3d
            I32(a + 4u) = 0;                               // 0x00417c30 (0x0089a4c8+v*0x74)
            I32(a)      = 0;                               // 0x00417c33 (0x0089a4c4+v*0x74)
        }
    }
    if (I32(0x0089a368u) != 0) return;                     // 0x00417c43
    if (ecx == 0) return;                                  // 0x00417c4f

    // probability rolls (0x00417c57): index = (row*5 + band)*4
    std::uintptr_t off = static_cast<std::uintptr_t>((row * 5 + edx)) * 4u; // 0x00417c57..0x00417c5c
    float prob1 = static_cast<float>(I32(0x005f30a0u + off)); // 0x00417c5f/0x00417c69 FILD
    float rnd1 = RandFloat(0x42c80000u);                   // 0x00417c77 FUN_00472650(0, 100.0f)
    if (!(rnd1 > prob1)) {                                 // 0x00417c7c (TEST 0x41/JP-table2 iff rnd>prob)
        I32(0x0089a368u) = 1;                              // 0x00417c8a
        for (std::uintptr_t a = 0x0089a4c0u; a < 0x0089a690u; a += 0x74u) { // 0x00417c94..0x00417caa
            I32(a) = 0;
        }
        return;                                            // 0x00417cb2
    }
    float prob2 = static_cast<float>(I32(0x005f3180u + off)); // 0x00417cb3/0x00417cbd FILD
    float rnd2 = RandFloat(0x42c80000u);                   // 0x00417ccb
    if (!(rnd2 > prob2)) {                                 // 0x00417cd0 (TEST 0x41/JP-ret iff rnd>prob)
        I32(0x0089a368u) = 2;                              // 0x00417cde
    }
}
RH_ScopedInstall(AiPreTick, 0x004177b0);
