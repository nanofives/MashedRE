// Mashed RE — WS-R6 final drain: the three AI control-step variants, verbatim
// ports authored from raw asm (Mashed_pool13 RO, 2026-07-02):
//
//   0x00416250  AiControlStep        primary control step (dispatch default)
//   0x00416a30  AiControlStepM49     game-mode-4/9 variant
//   0x00417da0  AiControlStepM8      game-mode-8 variant
//
// Caller FUN_00418560 pushes FOUR args (spline, vehicle, ctrl, 0x42c80000)
// before the dispatch branch (0x00418776..0x0041877d); NONE of the three
// bodies reads the 4th ([EBP+0x14] never referenced — checked in all three
// listings this session), so the ports declare the three consumed params;
// __cdecl caller-cleanup makes the extra push transparent.
//
// Variant deltas (verified instruction-level against 0x00416250 this session):
//   0x00416a30: NO mode-10 block (no FUN_00426c00/FUN_00414f00), NO
//     FUN_004148b0/FUN_00415020 mode-6 sub-paths, NO behavior-mode commit to
//     0x0089a52c. Everything else instruction-identical.
//   0x00417da0: CALL 0x00417cf0 replaces CALL 0x004148b0 (0x00418015), and the
//     DAT_0089a368==1 debug-override block is ABSENT (0x00418530 goes straight
//     to the final game-mode gate). Mode-10 block + mode commit PRESENT.
//
// x87 discipline (all rounding points mirrored from the listings):
//   - FUN_00443440 returns float10 in ST0 which the original pops (FSTP ST0 @
//     0x004162b5) -> declared float-returning, result discarded.
//   - FUN_00415e20 returns ONE float10 in ST0; the original FSTs an f32 copy
//     (0x0041659e -> [base+0x14]) and keeps the f80 live for the band FCOMs.
//     Under the game's D3D9 FPU control word (PC=24) every x87 op result is
//     f32-precision, so the float-typed call-through is value-identical.
//   - The hidden steer-byte expressions the decompiler mangled (extraout_ST0/
//     ST1) are transcribed from the listings: FILD/FMUL/FIMUL -> __ftol
//     (window-replay path) and ang*local_3c*_DAT_005cd0e8 [* local_40*
//     _DAT_005cc9a0] clamped at _DAT_005cd04c -> __ftol twice (commit path).
//   - __ftol is FORWARDED to the original FUN_004a2c48 via an x87 FLD shim
//     (AiLeaderTimer.cpp pattern) so the rounding mode is bit-guaranteed.
//   - The original ftols the SAME value twice on the commit paths (FLD ST0 @
//     0x00416690/0x004167aa then the base value); mirrored with two calls.
//   - FST [ESP+0x18] of the mode-2 dot product (0x0041696f) lands in the dead
//     local_40 stack slot (never read after) — not mirrored (no state effect).
//
// Comparison polarity: every FCOM/FCOMP + TEST AH,mask + Jcc pair is
// transcribed as the exact C predicate ('x < m' for TEST 0x5/JP-skip,
// '!(x <= m)' for TEST 0x41/JNZ-skip), preserving ordered/unordered behavior
// for all non-NaN inputs (game data carries no NaNs on these paths).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (prologue bytes of all three RVAs verified against
//    original\MASHED.exe.unpatched this session: 55 8b ec 83 e4 f8)

#include "../Core/HookSystem.h"
#include "AiState.h"

#include <cstdint>
#include <cstring>

using namespace Ai;

namespace {

inline float Cf(std::uint32_t bits) { float f; std::memcpy(&f, &bits, 4); return f; }

// per-vehicle strides
inline std::uintptr_t st74(std::uintptr_t base, int v) { return base + static_cast<std::uintptr_t>(v) * 0x74u; }
inline std::uintptr_t st14(std::uintptr_t base, int v) { return base + static_cast<std::uintptr_t>(v) * 0x14u; }
inline std::uintptr_t stB4(std::uintptr_t base, int v) { return base + static_cast<std::uintptr_t>(v) * 0xb4u; }

// ── typed call-throughs to original callees (live RVAs in the .asi) ─────────
typedef int   (__cdecl* fn_iv_t)(void);
typedef int   (__cdecl* fn_ii_t)(int);
typedef void  (__cdecl* fn_out_t)(void*, int);
typedef int   (__cdecl* fn_pred_t)(void*, void*, void*, int);       // (spline,&tgt,&cand,veh)
typedef int   (__cdecl* fn_pred5_t)(void*, void*, void*, int, int); // FUN_00415220 (+ftol arg)
typedef int   (__cdecl* fn_los_t)(void*, void*);                    // FUN_00416060 (&xz,&cand)
typedef float (__cdecl* fn_lookahead_t)(void*, float*, std::uint32_t, float*, int); // FUN_00443440
typedef void  (__cdecl* fn_seed_t)(void*, void*, int);              // FUN_004161e0
typedef float (__cdecl* fn_ang_t)(int, std::uint32_t, std::uint32_t); // FUN_00415e20 (float10 ST0)
typedef float*(__cdecl* fn_vec_t)(int);                             // FUN_00408af0
typedef int   (__cdecl* fn_wall2_t)(void*, int);                    // FUN_004150e0 (&xz,veh)

inline int    GameModeGet()            { return reinterpret_cast<fn_iv_t>(0x0040e350)(); }
inline void   VehF6a0(float* o, int v) { reinterpret_cast<fn_out_t>(0x0046d6a0)(o, v); }
inline void   VehF6d0(float* o, int v) { reinterpret_cast<fn_out_t>(0x0046d6d0)(o, v); }
inline void   VehRec(int* o, int v)    { reinterpret_cast<fn_out_t>(0x0046d4a0)(o, v); }
inline void   VehVel(float* o, int v)  { reinterpret_cast<fn_out_t>(0x0046d510)(o, v); }
inline float  Lookahead(void* s, float* xz, float* out) {
    return reinterpret_cast<fn_lookahead_t>(0x00443440)(s, xz, 0x41200000u, out, 0); // 10.0f
}
inline void   TargetSeed(void* s, void* tgt, int v) { reinterpret_cast<fn_seed_t>(0x004161e0)(s, tgt, v); }
inline int    Pred14570(void* s, void* t, void* c, int v) { return reinterpret_cast<fn_pred_t>(0x00414570)(s, t, c, v); }
inline int    Pred15880(void* s, void* t, void* c, int v) { return reinterpret_cast<fn_pred_t>(0x00415880)(s, t, c, v); }
inline int    Pred14a70(void* s, void* t, void* c, int v) { return reinterpret_cast<fn_pred_t>(0x00414a70)(s, t, c, v); }
inline int    Pred14c30(void* s, void* t, void* c, int v) { return reinterpret_cast<fn_pred_t>(0x00414c30)(s, t, c, v); }
inline int    Pred14f00(void* s, void* t, void* c, int v) { return reinterpret_cast<fn_pred_t>(0x00414f00)(s, t, c, v); }
inline int    Pred148b0(void* s, void* t, void* c, int v) { return reinterpret_cast<fn_pred_t>(0x004148b0)(s, t, c, v); }
inline int    Pred17cf0(void* s, void* t, void* c, int v) { return reinterpret_cast<fn_pred_t>(0x00417cf0)(s, t, c, v); }
inline int    Pred15220(void* s, void* t, void* c, int v, int f) { return reinterpret_cast<fn_pred5_t>(0x00415220)(s, t, c, v, f); }
inline int    Los(void* xz, void* c)   { return reinterpret_cast<fn_los_t>(0x00416060)(xz, c); }
inline int    WallAhead(int v)         { return reinterpret_cast<fn_ii_t>(0x00415d00)(v); }
inline int    WallLat(void* xz, int v) { return reinterpret_cast<fn_wall2_t>(0x004150e0)(xz, v); }
inline int    TrackIdx()               { return reinterpret_cast<fn_iv_t>(0x00426c00)(); }
inline int    TargetEnable()           { return reinterpret_cast<fn_iv_t>(0x00443080)(); }
inline int    Spawned15020(int v)      { return reinterpret_cast<fn_ii_t>(0x00415020)(v); }
inline float  SteerAngle(int v, std::uint32_t a, std::uint32_t b) {
    return reinterpret_cast<fn_ang_t>(0x00415e20)(v, a, b);   // float10-ST0 -> float
}
inline float* Vec408af0(int v)         { return reinterpret_cast<fn_vec_t>(0x00408af0)(v); }

// __ftol forwarded to the original FUN_004a2c48 via x87 FLD shim
// (feedback_x87_st0_float10_return_fnptr / AiLeaderTimer.cpp pattern).
void* g_ftol_4a2c48 = reinterpret_cast<void*>(0x004a2c48);
__declspec(naked) int Ftol(double /*x*/) {
    __asm {
        fld  qword ptr [esp + 4]
        call dword ptr [g_ftol_4a2c48]
        ret
    }
}

// ── shared body ──────────────────────────────────────────────────────────────
// The three variants are ONE code shape with compile-time deltas; kMode10Block/
// kMode6Deep/kDebugOverride select them. Every line cites 0x00416250's listing;
// the deltas cite their variant's listing. A template (not runtime flags) so
// each exported function compiles to straight-line code like the original.
enum StepVariant { kStepMain, kStepM49, kStepM8 };

template <StepVariant V>
void ControlStepBody(void* spline, int veh, std::uint8_t* ctrl)
{
    int gameMode = GameModeGet();                          // 0x0041625c
    float spd38;  VehF6a0(&spd38, veh);                    // 0x0041626e local_38
    float rate3c; VehF6d0(&rate3c, veh);                   // 0x00416279 local_3c
    int rec;      VehRec(&rec, veh);                       // 0x00416284 local_30
    float xz[2];                                           // local_1c/local_18
    xz[0] = F32(static_cast<std::uintptr_t>(rec) + 0x30u); // 0x0041628d
    xz[1] = F32(static_cast<std::uintptr_t>(rec) + 0x38u); // 0x00416297
    float dist40;                                          // local_40
    (void)Lookahead(spline, xz, &dist40);                  // 0x004162b0 (+FSTP ST0)
    if (F32(0x005cd09cu) < dist40) {                       // 0x004162be (TEST 0x41/JNZ)
        dist40 = F32(0x005ccac4u) - dist40;                // 0x004162cb
    }
    std::uint32_t tgt[2];                                  // local_24/local_20
    TargetSeed(spline, tgt, veh);                          // 0x004162e0
    int mode = 0;                                          // local_48 (EBX)
    std::uint32_t cand[2];                                 // local_2c/local_28

    // ── targeting chain (0x004162f7..0x0041645e) ──
    if (Pred14570(spline, tgt, cand, veh) != 0 && Los(xz, cand) != 0) {
        tgt[0] = cand[0]; tgt[1] = cand[1]; mode = 1;      // 0x00416319
    } else if (Pred15880(spline, tgt, cand, veh) != 0 && Los(xz, cand) != 0 &&
               WallAhead(veh) == 0) {
        tgt[0] = cand[0]; tgt[1] = cand[1]; mode = 2;      // 0x0041636e
    } else {
        int r = Pred14a70(spline, tgt, cand, veh);         // 0x00416394
        if (r == 2) {                                      // 0x004163fd early return
            ctrl[4] = 0; ctrl[5] = 0xff;                   // 0x00416405
            return;
        }
        bool decided = false;
        if (r == 1 && Los(xz, cand) != 0) {                // 0x004163ab
            tgt[0] = cand[0]; tgt[1] = cand[1]; mode = 3;  // 0x004163e6
            decided = true;
        }
        bool skipWallLat = false;
        if (!decided) {
            r = Pred14c30(spline, tgt, cand, veh);         // 0x004163c3
            if (r == 1) {
                if (Los(xz, cand) != 0) {                  // 0x004163da
                    tgt[0] = cand[0]; tgt[1] = cand[1]; mode = 3;
                    decided = true;
                }
            } else if (r == 2 && Los(xz, cand) != 0) {     // 0x00416414/0x00416423
                tgt[0] = cand[0]; tgt[1] = cand[1]; mode = 7; // 0x0041642f
                decided = true;
                skipWallLat = true;                        // JMP 0x0041645e->0x00416462 path
            }
        }
        // mode-7 jumps past the WallLat check (0x00416444 -> LAB_0041645e);
        // mode-3 also lands at 0x0041645e. Only the fall-through path (no
        // decision) runs FUN_004150e0.
        if (!decided && !skipWallLat) {
            if (WallLat(xz, veh) == 1) mode = 9;           // 0x0041644c
        }
    }

    // ── mode-10 block (0x00416462; ABSENT in 0x00416a30) ──
    if (V != kStepM49) {
        if (TrackIdx() == 0x21 && Pred14f00(spline, tgt, cand, veh) != 0) { // 0x00416467/0x00416478
            tgt[0] = cand[0]; tgt[1] = cand[1]; mode = 10; // 0x00416484
        }
    }

    // ── mode-6 block (0x0041649d) ──
    if (gameMode == 6 && TargetEnable() == 0) {            // 0x0041649d/0x004164a8
        if (V != kStepM49) {
            if (mode == 0) {                               // 0x004164b5
                // 0x00416250: FUN_004148b0; 0x00417da0: FUN_00417cf0 (0x00418015)
                int hit = (V == kStepM8) ? Pred17cf0(spline, tgt, cand, veh)
                                         : Pred148b0(spline, tgt, cand, veh);
                if (hit != 0 && Los(xz, cand) != 0) {      // 0x004164db
                    ctrl[5] = 0xff;                        // 0x004164ea
                    ctrl[0] = 0;                           // 0x004164ee (BL, mode==0 here)
                    ctrl[1] = 0;                           // 0x004164f0
                    return;
                }
                if (Spawned15020(veh) != 0) mode = 5;      // 0x004164fb/0x00416507
            }
        }
        if (I32(stB4(0x0088fc88u, veh)) == 0) {            // 0x00416517 (stride 0xb4)
            I32(st74(0x0089a51cu, veh)) = 0;               // 0x0041656a
            I32(st74(0x0089a520u, veh)) = 0;
            I32(st74(0x0089a524u, veh)) = 0;
        } else {
            int ft = Ftol(static_cast<double>(dist40));    // 0x00416523 FLD local_40 -> __ftol
            if (Pred15220(spline, tgt, cand, veh, ft) != 0 && Los(xz, cand) != 0) { // 0x00416539/0x0041654f
                mode = 8;                                  // 0x0041655b
            }
        }
    }

    // ── mode commit (0x00416590; ABSENT in 0x00416a30) ──
    if (V != kStepM49) {
        I32(st74(0x0089a52cu, veh)) = mode;
    }

    // ── steering angle + history bands (0x00416596..0x004167d5) ──
    float ang  = SteerAngle(veh, tgt[0], tgt[1]);          // 0x00416596; FST [base+0x14]
    float fv10 = F32(0x005d757cu);                         // 0x004165a5 FLD

    // band 1: ang < _DAT_005cd09c (0x004165af, TEST 0x5/JP-skip)
    if (ang < F32(0x005cd09cu)) {
        float histDc = F32(st14(0x008032dcu, veh));        // 0x004165c6
        F32(st14(0x008032d8u, veh)) = Cf(0x43b40000u);     // 0x004165cc (360.0)
        if (histDc < ang) fv10 = histDc;                   // 0x004165d6 (TEST 0x5/JP-skip)
        F32(st14(0x008032dcu, veh)) = ang;                 // 0x004165f7
        if (F32(0x005cc320u) < ang) {                      // 0x004165f1 (TEST 0x41/JNZ-skip)
            int flag  = I32(st74(0x0089a4ecu, veh));       // 0x00416608
            int delta = I32(0x007f0ff4u) - I32(st74(0x0089a4f0u, veh)); // 0x00416611
            if (flag == 1 || delta >= 200) {               // 0x0041660f/0x0041661c
                // commit path (0x00416648): steer[0] + record
                float val = ang * rate3c * F32(0x005cd0e8u);              // 0x00416648..0x00416656
                if (mode == 0 && F32(0x005ccd6cu) < dist40) {             // 0x00416650/0x00416662
                    val = val * (dist40 * F32(0x005cc9a0u));              // 0x0041666f FMULP
                }
                if (!(val <= F32(0x005cd04cu))) val = F32(0x005cd04cu);   // 0x0041667b clamp
                ctrl[0] = static_cast<std::uint8_t>(Ftol(static_cast<double>(val))); // 0x00416692 (dup)
                int rec2 = Ftol(static_cast<double>(val));                // 0x00416699 (base value)
                int now  = I32(0x007f0ff4u);
                I32(st74(0x0089a4f4u, veh)) = rec2;        // 0x004166a4
                I32(st74(0x0089a4ecu, veh)) = 1;           // 0x004166aa
                I32(st74(0x0089a4f0u, veh)) = now;         // 0x004166b4
            } else {
                // window-replay path (0x00416623): steer[1]
                int w = 200 - delta;                       // 0x00416628
                float v2 = static_cast<float>(w) * F32(0x005cd0ecu)       // 0x0041662e FILD/FMUL
                         * static_cast<float>(I32(st74(0x0089a4f4u, veh))); // 0x00416638 FIMUL
                ctrl[1] = static_cast<std::uint8_t>(Ftol(static_cast<double>(v2))); // 0x0041663e
            }
        }
    }

    // band 2: _DAT_005cd09c < ang (0x004166be, TEST 0x41/JNZ-skip)
    if (!(ang <= F32(0x005cd09cu))) {
        float histD8 = F32(st14(0x008032d8u, veh));        // 0x004166d5
        F32(st14(0x008032dcu, veh)) = 0.0f;                // 0x004166db
        if (!(histD8 <= ang)) {                            // 0x004166e5 (TEST 0x41/JNZ-keep)
            fv10 = F32(0x005ccac4u) - histD8;              // 0x004166f2
        }
        bool inBand = ang < F32(0x005cd0e4u);              // 0x00416706 (TEST 0x5/JP-skip)
        F32(st14(0x008032d8u, veh)) = ang;                 // 0x0041670c
        if (inBand) {
            int flag  = I32(st74(0x0089a4ecu, veh));       // 0x0041671d
            int delta = I32(0x007f0ff4u) - I32(st74(0x0089a4f0u, veh)); // 0x00416726
            if (flag == 2 || delta >= 200) {               // 0x00416724/0x00416731
                // commit path (0x0041675c): steer[1] + record
                float val = (F32(0x005ccac4u) - ang) * rate3c * F32(0x005cd0e8u); // 0x0041675c..0x00416770
                if (mode == 0 && F32(0x005ccd6cu) < dist40) {             // 0x00416766/0x0041677c
                    val = val * (dist40 * F32(0x005cc9a0u));              // 0x00416789 FMULP
                }
                if (!(val <= F32(0x005cd04cu))) val = F32(0x005cd04cu);   // 0x00416795 clamp
                ctrl[1] = static_cast<std::uint8_t>(Ftol(static_cast<double>(val))); // 0x004167ac (dup)
                int rec2 = Ftol(static_cast<double>(val));                // 0x004167b4 (base value)
                int now  = I32(0x007f0ff4u);
                I32(st74(0x0089a4f4u, veh)) = rec2;        // 0x004167bf
                I32(st74(0x0089a4ecu, veh)) = 2;           // 0x004167c5
                I32(st74(0x0089a4f0u, veh)) = now;         // 0x004167cf
            } else {
                // window-replay path (0x00416738): steer[0]
                int w = 200 - delta;                       // 0x0041673d
                float v2 = static_cast<float>(w) * F32(0x005cd0ecu)       // 0x00416743 FILD/FMUL
                         * static_cast<float>(I32(st74(0x0089a4f4u, veh))); // 0x0041674d FIMUL
                ctrl[0] = static_cast<std::uint8_t>(Ftol(static_cast<double>(v2))); // 0x00416753
            }
        }
    }

    // ── default accel + brake bands (0x004167d5..0x0041688d) ──
    ctrl[4] = 0xff;                                        // 0x004167dc
    float prevSpd = F32(st14(0x008032e0u, veh));           // 0x004167e0 (FSUB operand)
    float dspd = spd38 - prevSpd;                          // 0x004167d5/0x004167e0
    F32(st14(0x008032e0u, veh)) = spd38;                   // 0x004167eb
    if (!(dspd <= F32(0x005cc9b0u)) && !(spd38 <= F32(0x005cc55cu))) { // 0x004167f2/0x00416803
        ctrl[4] = 0; ctrl[5] = 0xff;                       // 0x00416810
    }
    if (!(fv10 <= F32(0x005ccd6cu)) && !(rate3c <= F32(0x005cd0b8u))) { // 0x00416818/0x00416829
        ctrl[4] = 0; ctrl[5] = 0xff;                       // 0x00416836
    }
    if (ang < F32(0x005cd09cu)) {                          // 0x0041683e (TEST 0x5/JP-skip)
        if (!(ang <= F32(0x005cc72cu))) {                  // 0x0041684b (TEST 0x41/JNZ-skip)
            ctrl[4] = 0xff; ctrl[5] = 0xff; ctrl[0] = 0xff; // 0x00416858
        }
    }
    if (!(ang <= F32(0x005cd09cu))) {                      // 0x00416863 (TEST 0x41/JNZ-else)
        if (ang < F32(0x005cd0e0u)) {                      // 0x00416870 (TEST 0x5/JP-skip)
            ctrl[4] = 0xff; ctrl[5] = 0xff; ctrl[1] = 0xff; // 0x0041687d
        }
    }

    // ── per-mode output dispatch (0x0041688d..0x004169e0) ──
    if (mode == 7) {
        ctrl[4] = 0x40;                                    // 0x00416896
    } else if (mode == 5) {
        float m5 = F32(st74(0x0089a4e8u, veh));            // 0x004168a4
        if (!(m5 <= F32(0x005cc35cu))) ctrl[4] = 0;        // 0x004168aa/0x004168b7
        if (!(m5 <= F32(0x005cd0a0u))) ctrl[5] = 0x40;     // 0x004168c1/0x004168ce
        int q = I32(0x007f0ff8u) / 0x3c;                   // 0x004168d2 (signed /60 idiom)
        if (q & 0x20) ctrl[0] = 0xff;                      // 0x004168eb/0x004168f4
        else          ctrl[1] = 0xff;                      // JZ 0x004169dc
    } else if (mode == 9) {
        if (!(rate3c <= F32(0x005cd0dcu))) { ctrl[4] = 0; ctrl[5] = 0xff; } // 0x00416905/0x00416912
        if (!(rate3c <= F32(0x005cd0d8u))) { ctrl[4] = 0; }                 // 0x0041691e/0x0041692f
    } else if (mode == 2) {
        float* pf = Vec408af0(veh);                        // 0x00416942
        float vel[3];                                      // local_14/local_10/local_c
        VehVel(vel, veh);                                  // 0x0041694f
        // dot = (pf[1]*vel[1] + vel[0]*pf[0]) + vel[2]*pf[2]   (0x00416954..0x0041696d)
        float dot = pf[1] * vel[1] + vel[0] * pf[0] + vel[2] * pf[2];
        // (FST [ESP+0x18] @0x0041696f writes the dead local_40 slot — no state)
        // cross = vel[0]*pf[2] - vel[2]*pf[0]                  (0x00416973..0x00416980)
        float cross = vel[0] * pf[2] - vel[2] * pf[0];
        float absdot = dot;
        if (dot < F32(0x005d757cu)) absdot = -dot;         // 0x00416984 (TEST 0x5/JP-skip) FCHS
        if (cross < F32(0x005d757cu)) {                    // 0x00416997 (TEST 0x5/JP-else)
            if (absdot < F32(0x005cc9c8u)) ctrl[1] = 0;    // 0x0041699f/0x004169b1
            if (absdot < F32(0x005cc9bcu)) ctrl[0] = 0xff; // 0x004169b5/0x004169c2
        } else {
            if (absdot < F32(0x005cc9c8u)) ctrl[0] = 0;    // 0x004169c7/0x004169cc
            if (absdot < F32(0x005cc9bcu)) ctrl[1] = 0xff; // 0x004169cf/0x004169dc
        }
    }

    // ── debug accel override (0x004169e0; ABSENT in 0x00417da0) ──
    if (V != kStepM8) {
        if (I32(0x0089a368u) == 1) {
            int b4 = static_cast<int>(ctrl[4]);            // 0x004169e9 MOVZX
            ctrl[4] = static_cast<std::uint8_t>(
                Ftol(static_cast<double>(static_cast<float>(b4) * F32(0x005ccac0u)))); // 0x004169f1..0x004169fb
        }
    }

    // ── final game-mode gate (0x00416a03) ──
    if (gameMode != 6 && gameMode != 5 && gameMode != 9 && gameMode != 10 && gameMode != 0xb) {
        ctrl[4] = 0; ctrl[5] = 0;                          // 0x00416a20
    }
}

} // namespace

// ===========================================================================
// 0x00416250  AiControlStep(spline, vehicle, ctrl [, 100.0f unused])
// ===========================================================================
extern "C" __declspec(dllexport)
void __cdecl AiControlStep(void* spline, int veh, std::uint8_t* ctrl)
{
    ControlStepBody<kStepMain>(spline, veh, ctrl);
}
RH_ScopedInstall(AiControlStep, 0x00416250);

// ===========================================================================
// 0x00416a30  AiControlStepM49 — game-mode-4/9 variant
// ===========================================================================
extern "C" __declspec(dllexport)
void __cdecl AiControlStepM49(void* spline, int veh, std::uint8_t* ctrl)
{
    ControlStepBody<kStepM49>(spline, veh, ctrl);
}
RH_ScopedInstall(AiControlStepM49, 0x00416a30);

// ===========================================================================
// 0x00417da0  AiControlStepM8 — game-mode-8 variant
// ===========================================================================
extern "C" __declspec(dllexport)
void __cdecl AiControlStepM8(void* spline, int veh, std::uint8_t* ctrl)
{
    ControlStepBody<kStepM8>(spline, veh, ctrl);
}
RH_ScopedInstall(AiControlStepM8, 0x00417da0);
