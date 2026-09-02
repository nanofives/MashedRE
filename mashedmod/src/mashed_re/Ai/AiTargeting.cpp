// Mashed RE — AI driver cluster: line-of-sight + spline-support leaves (WS-C2).
//
// Verbatim ports of clean, isolatable leaves of the FUN_00418860 AI cluster:
//   0x00416060  AiLineOfSight       XZ ray-march tile LOS  (0=blocked, 1=clear)
//   0x004161e0  AiSplineTargetInit  seed target point from own XZ -> FUN_00443dc0
//   0x0046d510  AiVehicleVelocity3  per-vehicle velocity float3 getter
//
// AiLineOfSight is the C3a per-function-diff target named in ai_controller.md
// §10: two float* in, an int flag out -> directly run_diff-able in isolation.
//
// NOT ported (stays called-by-RVA): FUN_004150e0 (wall-lateral query) reads its
// two args off the x87 FPU stack (void sig + __ftol(ST0) of caller values) — not
// expressible as standard C++; needs a naked-asm shim. Carried for a later pass.
//
// Build is x87 (no /arch:SSE2) so plain C++ float math matches the original FPU
// codegen ([[project-wsa2-rwmath-bitident]]); __ftol(ST0) of a locally-computed
// value == (int)expr under x87.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Source decompilations (pool5, 2026-06-16):
//   re/analysis/ai_update_d2/0x00416060.md     + this session's exact decomp
//   re/analysis/bucket_ai_00415d00_00452ea0/0x004161e0.md + exact decomp
//   re/analysis/ai_update_d2/0x0046d510.md     + exact decomp

#include "../Core/HookSystem.h"
#include "AiState.h"

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <windows.h>

using namespace Ai;

namespace {

// Vec2 length (0x004c3bf0 = Vec2Length, C4) — returns float10 in ST0.
typedef float (__cdecl* fn_vec2len_t)(float*);
inline float call_004c3bf0(float* v) { return reinterpret_cast<fn_vec2len_t>(0x004c3bf0)(v); }
// Own-vehicle struct ptr getter (0x0046d4a0, C3).
typedef void (__cdecl* fn_ptrget_t)(int*, int);
inline void call_0046d4a0(int* out, int v) { reinterpret_cast<fn_ptrget_t>(0x0046d4a0)(out, v); }
// Spline lookahead target finder (0x00443dc0): f(spline, xz, outIdx, v, 1, 0).
typedef void (__cdecl* fn_443dc0_t)(void*, float*, void*, int, int, int);
inline void call_00443dc0(void* spline, float* xz, void* outIdx, int v) {
    reinterpret_cast<fn_443dc0_t>(0x00443dc0)(spline, xz, outIdx, v, 1, 0);
}
// RW transform-points (0x004c3df0): f(dst, src, 1, matrix).
typedef void (__cdecl* fn_4c3df0_t)(void*, void*, int, void*);
inline void call_004c3df0(void* dst, void* src, void* mtx) {
    reinterpret_cast<fn_4c3df0_t>(0x004c3df0)(dst, src, 1, mtx);
}

// Track tile grid (cited at 0x00416060 / 0x004150e0): 128x128 shorts at 0x007f1a9c,
// 8x8 sub-cell chars per tile at 0x007f9a9c.
static constexpr std::uintptr_t kTileGrid    = 0x007f1a9cu;
static constexpr std::uintptr_t kSubCellGrid = 0x007f9a9cu;

} // namespace

// ===========================================================================
// 0x00416060  AiLineOfSight(posA_xz, posB_xz) -> 0 blocked / 1 clear
//
// Ray-marches A->B in 1/length steps (step _DAT_005cc564); at each sample with
// a moved position, looks up the track tile (>0 && <0x200) and its sub-cell;
// sub-cell type 0 or 3 = wall -> blocked. Cited: 0x00416060 body.
// ===========================================================================
extern "C" __declspec(dllexport)
std::uint32_t __cdecl AiLineOfSight(float* param_1, float* param_2)
{
    float local_8  = param_2[0] - param_1[0];   // dx
    float local_18 = 0.0f;                       // t
    float local_4  = param_2[1] - param_1[1];   // dz
    float fVar1 = call_004c3bf0(&local_8);       // length(dx,dz)  [&local_8 = {dx,dz}]
    if (F32(0x005d757cu) < fVar1) {              // length > 0
        float fVar4 = F32(0x005cc320u) / fVar1;  // 1.0 / length
        do {
            float sampleX = fVar4 * local_8 * local_18 + param_1[0];
            float sampleZ = fVar4 * local_4 * local_18 + param_1[1];
            if (sampleX != param_1[0] && sampleZ != param_1[1]) {
                int ix = static_cast<int>(sampleX);   // __ftol
                int iz = static_cast<int>(sampleZ);   // __ftol
                short tile = *reinterpret_cast<short*>(
                    kTileGrid + static_cast<std::uintptr_t>(
                        ((( ix + 0x1f0) >> 3) * 0x80 + ((iz + 0x1f0) >> 3)) * 2));
                if (0 < tile && tile < 0x200) {
                    char sub = *reinterpret_cast<char*>(
                        kSubCellGrid + static_cast<std::uintptr_t>(
                            ((iz & 7) + tile * 8) * 8 + (ix & 7)));
                    if (sub == 0 || sub == 3) {
                        return 0;
                    }
                }
            }
            local_18 = local_18 + F32(0x005cc564u);   // t += step
        } while (local_18 < fVar1);
    }
    return 1;
}

RH_ScopedInstall(AiLineOfSight, 0x00416060);

// ===========================================================================
// 0x004161e0  AiSplineTargetInit(spline, outTargetIdx, vehicle)
//
// Reads the vehicle's own XZ (struct +0x30 / +0x38) and calls the spline
// lookahead finder to seed the target point. Cited: 0x004161e0 body.
// ===========================================================================
extern "C" __declspec(dllexport)
void __cdecl AiSplineTargetInit(void* param_1, void* param_2, int param_3)
{
    int local_c;
    call_0046d4a0(&local_c, param_3);            // own struct ptr
    float xz[2];
    xz[0] = F32(static_cast<std::uintptr_t>(local_c) + 0x30u);   // local_8 = X
    xz[1] = F32(static_cast<std::uintptr_t>(local_c) + 0x38u);   // local_4 = Z
    call_00443dc0(param_1, xz, param_2, param_3);
}

// ---------------------------------------------------------------------------
// 0x004161e0 in-race A/B self-test (2026-09-02, parent booted lane).
//
// This row's acceptance design was written into its hooks.csv row when it was assessed:
// "seeding the vehicle struct's +0x30/+0x38 and comparing the resulting outTargetIdx". The
// function writes nothing itself - its whole contribution is (fetch own-struct ptr, read X at
// +0x30 and Z at +0x38, pass them in that order) - so the only observable is what the CALLEE
// produces from those inputs.
//
// THE CALLEE IS NOT SIDE-EFFECT FREE, and this is the load-bearing detail. FUN_00443dc0 contains
//     if (param_5 != 0) { iVar6 = (&DAT_008032d4)[param_4*5];
//                         ...continuity constraint on iVar10 using iVar6...
//                         (&DAT_008032d4)[param_4*5] = iVar10; }
// and this call site passes param_5 = 1. So it WRITES a per-vehicle spline-index cache and
// re-reads it to constrain the next result. A naive run-both-and-compare A/B would have the second
// pass read what the first pass wrote, which self-poisons the comparison and could produce either
// a false mismatch or a false green. The cache is therefore snapshotted and restored around every
// pass. Address: DAT_008032d4 is an int array indexed [param_4*5], i.e. byte offset param_4*20.
//
// RESTORE VALIDATION, because "I restored the state I know about" is an assumption and this makes
// it a measurement: the ORIGINAL is run TWICE with a restore in between. If o1 != o2 or c1 != c2
// the restore did not capture everything that feeds the result, and the comparison for that call
// is declared VOID rather than counted as a pass. Only when the two original passes agree is the
// modded pass compared against them.
//   LIMITATION, stated rather than left implicit: this catches state that AFFECTS THE RESULT. If
//   the callee tree writes state that never feeds back into its own output, three executions would
//   touch it three times and the validation would not notice. The cap below bounds that exposure.
//
// Net state after the sequence is exactly one original execution (cache = c1, out = o1), so the
// game behaves as unhooked. Capped at kSplineMaxCompare because the callee is a 3464-byte spline
// search and this row fires ~143 times/second in a 4-car race; past the cap this is a pure
// passthrough with zero extra work.
long g_spline_calls = 0, g_spline_mismatch = 0, g_spline_void = 0;
long g_spline_outMoved = 0, g_spline_cacheMoved = 0;
std::uint32_t g_spline_outFold = 0;
const long kSplineMaxCompare = 400;

typedef void (__cdecl* fn_spline_t)(void*, void*, int);
void* g_orig_4161e8 = reinterpret_cast<void*>(0x004161e8);

// Trampoline. The 5-byte install JMP covers SUB ESP,0xc (3) + PUSH ESI (1) + the FIRST BYTE of
// MOV ESI,[ESP+0x1c] - so it splits that MOV and all three instructions must be re-executed
// before rejoining at the clean boundary 0x004161e8 (LEA EAX,[ESP+4]). Declared __cdecl with the
// same three args, so [ESP+0x1c] resolves to param_3 exactly as in the original frame.
__declspec(naked) void OrigSplineTargetInit(void* /*p1*/, void* /*p2*/, int /*p3*/) {
    __asm {
        sub    esp, 0xc
        push   esi
        mov    esi, dword ptr [esp + 0x1c]
        jmp    dword ptr [g_orig_4161e8]
    }
}

inline int SplineSelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_AI_SPLINE_SELFTEST"); v = (s && s[0]) ? 1 : 0; }
    return v;
}
void SplineSelfTestLog(const char* s) {
    HANDLE h = CreateFileA("ai_spline_target_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}

void SplineTargetDispatch(void* p1, void* p2, int p3) {
    if (SplineSelfTestEnabled() && g_spline_calls < kSplineMaxCompare && p2 != nullptr) {
        volatile std::uint32_t* pOut =
            reinterpret_cast<volatile std::uint32_t*>(p2);
        volatile std::uint32_t* pCache =
            reinterpret_cast<volatile std::uint32_t*>(0x008032d4u + (unsigned)p3 * 20u);
        const std::uint32_t out0 = *pOut;
        const std::uint32_t c0   = *pCache;

        OrigSplineTargetInit(p1, p2, p3);                 // original pass 1
        const std::uint32_t o1 = *pOut, c1 = *pCache;

        *pOut = out0; *pCache = c0;
        OrigSplineTargetInit(p1, p2, p3);                 // original pass 2 (restore validation)
        const std::uint32_t o2 = *pOut, c2 = *pCache;

        *pOut = out0; *pCache = c0;
        AiSplineTargetInit(p1, p2, p3);                   // modded pass
        const std::uint32_t m = *pOut, mc = *pCache;

        // Leave exactly one original execution's worth of state behind.
        *pOut = o1; *pCache = c1;

        ++g_spline_calls;
        const bool restoreOk = (o1 == o2) && (c1 == c2);
        if (!restoreOk) {
            ++g_spline_void;
        } else {
            if (o1 != out0) ++g_spline_outMoved;
            if (c1 != c0)   ++g_spline_cacheMoved;
            g_spline_outFold ^= o1;
            if (m != o1 || mc != c1) {
                ++g_spline_mismatch;
                char line[192];
                wsprintfA(line, "[%ld] MISMATCH out m=%08X o=%08X | cache m=%08X o=%08X | v=%d\r\n",
                          g_spline_calls, m, o1, mc, c1, p3);
                SplineSelfTestLog(line);
            }
        }
        if ((g_spline_calls & 0x3f) == 1) {
            char line[256];
            wsprintfA(line, "[%ld] calls=%ld mism=%ld void=%ld outMoved=%ld cacheMoved=%ld "
                            "outFold=%08X %s\r\n",
                      g_spline_calls, g_spline_calls, g_spline_mismatch, g_spline_void,
                      g_spline_outMoved, g_spline_cacheMoved, g_spline_outFold,
                      g_spline_mismatch ? "" : "ALL-GREEN");
            SplineSelfTestLog(line);
        }
        return;
    }
    // Not testing: the reimpl IS the shipped behaviour for this row.
    AiSplineTargetInit(p1, p2, p3);
}

// Naked entry installed at 0x004161e0 - __cdecl void(p1, p2, p3): forward all three, RET.
__declspec(naked) void AiSplineTarget_Entry() {
    __asm {
        mov    eax, dword ptr [esp + 0xc]
        push   eax
        mov    eax, dword ptr [esp + 0xc]
        push   eax
        mov    eax, dword ptr [esp + 0xc]
        push   eax
        call   SplineTargetDispatch
        add    esp, 0xc
        ret
    }
}

RH_ScopedInstall(AiSplineTarget_Entry, 0x004161e0);

// ===========================================================================
// 0x0046d510  AiVehicleVelocity3(outVec3, vehicleIdx) -> 1 ok / 0 OOB
//
// Transforms the per-vehicle velocity (0x00881ec8 + v*0xd04) by 0x00614708 into
// 0x00881f74 + v*0xd04, then copies that float3 out. Cited: 0x0046d510 body.
//   0x00881f74 stride 0x341 dwords (= 0xd04 bytes); 0x0f index bound.
// ===========================================================================
extern "C" __declspec(dllexport)
std::uint32_t __cdecl AiVehicleVelocity3(std::uint32_t* param_1, std::uint32_t param_2)
{
    if (0xf < param_2) return 0;
    std::uintptr_t off = static_cast<std::uintptr_t>(param_2) * 0xd04u;
    call_004c3df0(reinterpret_cast<void*>(0x00881f74u + off),
                  reinterpret_cast<void*>(0x00614708u),
                  reinterpret_cast<void*>(0x00881ec8u + off));
    param_1[0] = U32(0x00881f74u + off);
    param_1[1] = U32(0x00881f78u + off);
    param_1[2] = U32(0x00881f7cu + off);
    return 1;
}

// INSTALLER REMOVED 2026-09-01 (U-9065 class, found by area/vehicle r5's pre-queue grep).
// 0x0046d510 had TWO RH_ScopedInstall calls: this one and VehicleVelocityWorldGet
// (Ai/VehicleVelocityWorldGet.cpp:54). Whichever ran last silently won the site, which is
// exactly the defect that left 0x004277a0 bit-identical and never installed.
// VehicleVelocityWorldGet is the keeper on three independent grounds: hooks.csv points its
// C3 row at that file, hooks_registry's vehicle_velocity_world_get maps the RVA to that
// export, and its TU is in BOTH build.bat and asi_sources.rsp (this one is .asi-only), so it
// is the body that reaches the standalone exe.
// The two bodies are FUNCTIONALLY IDENTICAL - verified, not assumed. This one's helper
// call_004c3df0(dst, src, mtx) forwards (dst, src, 1, mtx) and is called here as
// (vel, 0x00614708, 0x00881ec8+off), yielding (vel, matrix, 1, struct); VehicleVelocityWorldGet
// passes (vel, matrix, 1, struct) directly. Same call, so removing this installer changes no
// behaviour. NOTE the helper's parameter NAMES at line 53 are misleading - its second
// parameter receives the matrix and its third the source - which is what made the two look
// divergent at a glance.
// AiVehicleVelocity3 has no internal callers and is left defined but uninstalled.
// RH_ScopedInstall(AiVehicleVelocity3, 0x0046d510);  // <- do not re-enable

// ---------------------------------------------------------------------------
// More call-throughs for the steering + wall-ahead leaves.
namespace {
// FUN_004c39b0 (RwV3dNormalize) RETURNS float10 IN ST0 (Ghidra: `float10 FUN_004c39b0`,
// it returns the squared-length). The forwarding fn-ptr MUST be declared returning a
// value (float) so the x87 .asi emits an FSTP that pops ST0 — a `void` decl leaks the
// x87 register stack, and AiSteeringAngleError calls it TWICE, so an in-thread double-
// invoke overflows the 8-deep x87 stack within a frame and every later FP op goes NaN
// (whole-sim freeze). [[feedback_x87_st0_float10_return_fnptr]]; same rule as
// AiLineOfSight.cpp's Vec2Length. FOUND via the parent's booted-race freeze trace.
typedef float  (__cdecl* fn_norm_t)(float*, float*);            // RwV3dNormalize -> float10 ST0
typedef double (__cdecl* fn_acos_t)(double);                    // acos (float10 in ST0)
typedef void   (__cdecl* fn_velget_t)(std::uint32_t*, int);     // velocity vec3 getter
typedef char   (__cdecl* fn_tile2_t)(float, float);            // tile query (float args)
inline float  call_004c39b0(float* d, float* s) { return reinterpret_cast<fn_norm_t>(0x004c39b0)(d, s); }
inline double call_004a3384(double x)           { return reinterpret_cast<fn_acos_t>(0x004a3384)(x); }
inline void   call_0046d510(std::uint32_t* o, int v) { reinterpret_cast<fn_velget_t>(0x0046d510)(o, v); }
inline char   call_00443d10(float x, float z)   { return reinterpret_cast<fn_tile2_t>(0x00443d10)(x, z); }
} // namespace

// ===========================================================================
// 0x00415e20  AiSteeringAngleError(vehicle, targetX, targetZ) -> signed angle
//
// Returns (via x87 ST0) the signed angular error between the bearing from the
// vehicle to (targetX,targetZ) and the vehicle's current heading: acos of each
// normalized direction dotted with +X, z-sign corrected, scaled by _DAT_005cc970,
// each wrapped to [0,2pi); the difference is re-wrapped to [0,2pi). Cited:
// 0x00415e20 body. Returned as float — MSVC leaves it in ST0 under x87 (the
// caller truncates to 32-bit immediately, so the value matches). acos via the
// original FUN_004a3384 (RVA), not MSVC acos.
// NOTE: (local_c,local_8,local_4) are stack-adjacent in the original and used as
// one 3-vector via &local_c — ported as an explicit float[3] to guarantee that.
//
// BUGFIX 2026-07-28 — WRONG OPERAND WIDTH on three constants. They are QWORD x87
// operands in the original (opcode prefix `dc`), not DWORD (`d8`), and reading them
// through F32 silently returned the low half of a double:
//   0x00415eba  dc0d 70c95c00  FMUL QWORD [0x005cc970]  = 57.2958 (180/pi)
//                              read as f32 -> 1.0842e-19, so the acos result was
//                              scaled to ~0 and the function returned a dead angle
//   0x00415e85  dc15 d0d05c00  FCOM QWORD [0x005cd0d0]  = -1.0  (read as f32 -> 0.0)
//   0x00415e9a  dc15 c8d05c00  FCOM QWORD [0x005cd0c8]  = +1.0  (read as f32 -> 0.0)
// The two clamps therefore compared against 0.0 instead of the acos domain limits,
// snapping every positive input to 1.0. Note 0x005ccac4 (360.0f, `d805 FADD DWORD`)
// and 0x005d757c (0.0f, `d80d FMUL DWORD`) in the same function ARE dwords and were
// always right — this is a per-constant transcription error, not a systematic one.
// Isolated by a 10-step MASHED_HOOK_LO/HI index bisect (registry index 616) after the
// menu-navigated race wedged at race entry with the full hook set while stock completed.
// ===========================================================================
extern "C" __declspec(dllexport)
float __cdecl AiSteeringAngleError(int param_1, float param_2, float param_3)
{
    const float kZero = F32(0x005d757cu);
    const float kTwoPi = F32(0x005ccac4u);

    int local_10;
    call_0046d4a0(&local_10, param_1);
    float n[3];                                   // {local_c, local_8, local_4}
    n[0] = param_2 - F32(static_cast<std::uintptr_t>(local_10) + 0x30u);   // local_c
    n[1] = 0.0f;                                                            // local_8
    n[2] = -(param_3 - F32(static_cast<std::uintptr_t>(local_10) + 0x38u)); // local_4
    call_004c39b0(n, n);
    float fVar1 = n[2] * kZero + n[1] * kZero + n[0];
    if (fVar1 < F64(0x005cd0d0u)) fVar1 = F32(0x005cc33cu);   // 0x00415e85 FCOM QWORD -> -1.0
    if (F64(0x005cd0c8u) < fVar1) fVar1 = F32(0x005cc320u);   // 0x00415e9a FCOM QWORD -> +1.0
    float fVar2 = static_cast<float>(call_004a3384(static_cast<double>(fVar1)) * F64(0x005cc970u));  // 0x00415eba FMUL QWORD -> 57.2958
    if (n[2] - n[0] * kZero < kZero) fVar2 = -fVar2;
    while (fVar2 < kZero) fVar2 += kTwoPi;
    float bearing = fVar2;

    call_0046d510(reinterpret_cast<std::uint32_t*>(n), param_1);   // velocity into n[]
    n[2] = -n[2];                                   // local_4 = -local_4
    n[1] = 0.0f;                                     // local_8 = 0
    call_004c39b0(n, n);
    fVar1 = n[2] * kZero + n[1] * kZero + n[0];
    if (fVar1 < F64(0x005cd0d0u)) fVar1 = F32(0x005cc33cu);   // 0x00415e85 FCOM QWORD -> -1.0
    if (F64(0x005cd0c8u) < fVar1) fVar1 = F32(0x005cc320u);   // 0x00415e9a FCOM QWORD -> +1.0
    fVar2 = static_cast<float>(call_004a3384(static_cast<double>(fVar1)) * F64(0x005cc970u));  // 0x00415eba FMUL QWORD -> 57.2958
    if (n[2] - n[0] * kZero < kZero) fVar2 = -fVar2;
    while (fVar2 < kZero) fVar2 += kTwoPi;

    float result = bearing - fVar2;
    while (result < kZero) result += kTwoPi;
    while (result < kZero) result += kTwoPi;   // (decomp has the wrap twice)
    while (kTwoPi < result) result -= kTwoPi;
    return result;
}

// ── in-race A/B self-test for AiSteeringAngleError (env MASHED_AI_STEER_SELFTEST) ──
// Mirrors Ai/AiLineOfSight.cpp's LosDispatch: the function is PURE (reads the vehicle
// struct + velocity via forwarded RVAs, returns a float via ST0, no writes), so the A/B
// just compares the returned float BIT PATTERN and RETURNS ORIG so game behaviour is
// unchanged during measurement (SAFE PASSTHROUGH when not self-testing). Must be run in
// a booted race (the vehicle table + velocity blocks the callees read are only live
// there); at the menu both sides read the same unset state -> a dead run, not evidence.
// U-9025 (the mid-race wedge once feared here) is RESOLVED (audio-thread bug in
// FUN_005aef00, commit f1855ad9), so the full hook set completes races 10/10.
namespace {

// Orig trampoline: the RH_ScopedInstall JMP (5 bytes) overwrites SUB ESP,0x10 (3) +
// PUSH ESI (1) + first byte of MOV ESI,[ESP+0x18]; re-exec all three (8 bytes) then
// jmp to the next instruction boundary 0x00415e28 (LEA EAX,[ESP+4]). After the two
// stack adjusts, [ESP+0x18] == the original [ESP+4] == param_1 (vehicle), matching the
// original's own ESI load. The .asi is x87 (no SSE2) so the original's ST0 result is the
// float return; declaring float pops ST0 correctly.
void* g_orig_415e28 = reinterpret_cast<void*>(0x00415e28);
__declspec(naked) float OrigSteeringAngleError(int /*veh*/, float /*tx*/, float /*tz*/) {
    __asm {
        sub  esp, 0x10
        push esi
        mov  esi, dword ptr [esp + 0x18]
        jmp  dword ptr [g_orig_415e28]
    }
}

inline int SteerSelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_AI_STEER_SELFTEST"); v = (s && s[0]) ? 1 : 0; }
    return v;
}
void SteerSelfTestLog(const char* s) {
    HANDLE h = CreateFileA("ai_steer_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}
long g_steer_calls = 0, g_steer_mismatch = 0;
const long kSteerMaxCompare = 40000;

// A/B: compare the returned float's exact bits (NaN-safe), log mismatches, return ORIG.
float SteerDispatch(int veh, float tx, float tz) {
    if (SteerSelfTestEnabled() && g_steer_calls < kSteerMaxCompare) {
        float o = OrigSteeringAngleError(veh, tx, tz);
        float m = AiSteeringAngleError(veh, tx, tz);
        ++g_steer_calls;
        std::uint32_t ob, mb;
        std::memcpy(&ob, &o, 4); std::memcpy(&mb, &m, 4);
        if (ob != mb) {
            ++g_steer_mismatch;
            char line[192];
            std::uint32_t tbx, tbz;
            std::memcpy(&tbx, &tx, 4); std::memcpy(&tbz, &tz, 4);
            wsprintfA(line, "[%ld] MISMATCH o=%08x m=%08x  veh=%d tx=%08x tz=%08x\r\n",
                      g_steer_calls, ob, mb, veh, tbx, tbz);
            SteerSelfTestLog(line);
        }
        if ((g_steer_calls & 0x7f) == 1) {
            char line[128];
            wsprintfA(line, "[%ld] calls=%ld mism=%ld %s\r\n", g_steer_calls, g_steer_calls,
                      g_steer_mismatch, g_steer_mismatch ? "" : "ALL-GREEN");
            SteerSelfTestLog(line);
        }
        return o;
    }
    return OrigSteeringAngleError(veh, tx, tz);
}

// Naked entry installed at 0x00415e20 — forwards the 3 __cdecl stack args (veh,tx,tz)
// to SteerDispatch and leaves the float result in ST0 for the original caller.
__declspec(naked) void AiSteer_Entry() {
    __asm {
        // [esp]=ret [esp+4]=veh [esp+8]=tx [esp+0xc]=tz
        push dword ptr [esp + 0x0c]   // tz
        push dword ptr [esp + 0x0c]   // tx (esp shifted 4 after 1st push)
        push dword ptr [esp + 0x0c]   // veh (esp shifted 8)
        call SteerDispatch
        add  esp, 0x0c                // cdecl: clean our 3 forwarded args
        ret                           // ST0 = float result; original is caller-cleans
    }
}

}  // namespace

RH_ScopedInstall(AiSteer_Entry, 0x00415e20);

// ===========================================================================
// 0x00415d00  AiWallAhead(vehicle) -> 1 wall ahead / 0 clear
//
// Extrapolates the vehicle's velocity 2x forward and ray-marches that segment
// (1/length steps, step _DAT_005cc564) sampling the track tile via FUN_00443d10;
// tile type 0 or 3 => wall => 1. Suppresses the mode-2 ram. Cited: 0x00415d00.
// (local_c,local_8,local_4) and (local_1c,local_18) are stack-adjacent vectors
// in the original — ported as explicit arrays.
// ===========================================================================
extern "C" __declspec(dllexport)
std::uint32_t __cdecl AiWallAhead(int param_1)
{
    int local_28;
    call_0046d4a0(&local_28, param_1);
    float local_24 = F32(static_cast<std::uintptr_t>(local_28) + 0x30u);  // own X
    float local_20 = F32(static_cast<std::uintptr_t>(local_28) + 0x38u);  // own Z
    float vel[3];                                   // {local_c, local_8, local_4}
    call_0046d510(reinterpret_cast<std::uint32_t*>(vel), param_1);
    vel[0] = vel[0] + vel[0];                        // 2x velX
    float local_34 = 0.0f;
    vel[1] = vel[1] + vel[1];                        // 2x velY
    vel[2] = vel[2] + vel[2];                        // 2x velZ
    float seg[2];                                    // {local_1c, local_18}
    seg[0] = (vel[0] + local_24) - local_24;
    seg[1] = (vel[2] + local_20) - local_20;
    float fVar1 = call_004c3bf0(seg);               // length(seg)
    if (F32(0x005d757cu) < fVar1) {
        float fVar2 = F32(0x005cc320u) / fVar1;
        do {
            float local_14 = fVar2 * seg[0] * local_34 + local_24;
            float local_10 = fVar2 * seg[1] * local_34 + local_20;
            char cVar3 = call_00443d10(local_14, local_10);
            if (cVar3 == 0 || cVar3 == 3) {
                return 1;
            }
            local_34 = local_34 + F32(0x005cc564u);
        } while (local_34 < fVar1);
    }
    return 0;
}

RH_ScopedInstall(AiWallAhead, 0x00415d00);

// ===========================================================================
// 0x00415200  AiVehicle0ZeroProgressGuard() -> 1 if veh0 progress == 0.0f else 0
//
// Exact disasm (pool2, 2026-09-01):
//   00415200  6a00              PUSH 0x0
//   00415202  e8b9da0200        CALL 0x00442cc0                 ; progress getter
//   00415207  d81d7c755d00      FCOMP float ptr [0x005d757c]    ; == sentinel (0.0f)
//   0041520d  83c404            ADD ESP,0x4
//   00415210  dfe0              FNSTSW AX
//   00415212  f6c444            TEST AH,0x44                    ; C3|C2 mask -> equality
//   00415215  7a06              JP 0x0041521d
//   00415217  b801000000        MOV EAX,0x1
//   0041521c  c3                RET
//   0041521d  33c0              XOR EAX,EAX
//   0041521f  c3                RET
//
// FUN_00442cc0(i) (0x00442cc0, pure leaf): `CMP EAX,4; JGE .; FLD [EAX*4+0x008989b0]`
// -> returns the per-vehicle progress float (global array base 0x008989b0), or the
// sentinel at 0x005d757c when i>=4. It RETURNS A FLOAT IN ST0, so the forwarding fn-ptr
// MUST be declared returning `float` (an FSTP pops ST0) — a `void` decl leaks the x87
// stack ([[x87-st0-float10-fnptr-void-leak]], the same class root-caused for
// AiSteeringAngleError). Sentinel 0x005d757c == 0.0f (dword; cited above at line ~199).
// The FCOMP compares two exact float32 values, so plain C float-equality reproduces it
// bit-for-bit under x87 (no intermediate rounding) — no naked-asm body needed.
// ref: re/analysis/bucket_ai_00415d00_00452ea0/0x00442cc0.md, .../0x00415200.md
// ===========================================================================
namespace {
typedef float (__cdecl* fn_progress_t)(int);            // FUN_00442cc0 -> float10 ST0
inline float call_00442cc0(int i) { return reinterpret_cast<fn_progress_t>(0x00442cc0)(i); }
} // namespace

extern "C" __declspec(dllexport)
std::uint32_t __cdecl AiVehicle0ZeroProgressGuard(void)
{
    float p = call_00442cc0(0);                 // g_progress[0]
    return (p == F32(0x005d757cu)) ? 1u : 0u;   // == sentinel (0.0f)
}

// ── in-race A/B self-test for AiVehicle0ZeroProgressGuard (env MASHED_AI_V0GUARD_SELFTEST) ──
// Mirrors SteerDispatch above: the function is PURE (calls the read-only progress getter,
// returns an int, no writes), so the A/B compares the returned EAX and RETURNS ORIG (safe
// passthrough; behaviour unchanged when the env var is unset). MUST be run in a booted race
// — g_progress[0] (0x008989b0) is only live there. Both branches are exercised: veh0's
// progress is exactly 0.0f on the grid (==sentinel -> returns 1) and becomes nonzero once
// moving (returns 0). At the menu both sides read the same unset state = a dead run.
namespace {

// Orig trampoline: the RH_ScopedInstall JMP (5 bytes) overwrites PUSH 0 (2) + the first 3
// bytes of CALL 0x00442cc0 (5). Re-exec the two stolen instructions (PUSH 0; CALL getter)
// then jmp to the next clean boundary 0x00415207 (FCOMP), which runs the equality tail and
// RETs with EAX set. The CALL is re-issued indirectly through a global so MSVC does not need
// to encode a relative displacement. The getter's ST0 result is consumed by the FCOMP at
// 0x00415207 (pops ST0), so the x87 stack stays balanced on this side too.
void* g_442cc0       = reinterpret_cast<void*>(0x00442cc0);
void* g_orig_415207  = reinterpret_cast<void*>(0x00415207);
__declspec(naked) std::uint32_t OrigVehicle0ZeroProgressGuard(void) {
    __asm {
        push 0
        call dword ptr [g_442cc0]
        jmp  dword ptr [g_orig_415207]
    }
}

inline int V0GuardSelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_AI_V0GUARD_SELFTEST"); v = (s && s[0]) ? 1 : 0; }
    return v;
}
void V0GuardSelfTestLog(const char* s) {
    HANDLE h = CreateFileA("ai_v0guard_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}
long g_v0guard_calls = 0, g_v0guard_mismatch = 0;
// Coverage counters (2026-09-01, parent booted-race lane). This guard returns 0 or 1, so a bare
// "N calls, 0 mismatches" cannot separate "both branches were compared" from "the FCOMP went the
// same way every call and the run only ever witnessed one constant" ([[scratch-field-false-green]]).
// ret1 = calls where the original returned 1 (progress[0] == 0.0f); ret1 in (0, calls) is the
// non-degeneracy proof.
long g_v0guard_ret1 = 0;
const long kV0GuardMaxCompare = 40000;

// A/B: compare the returned int exactly, log mismatches, return ORIG (safe passthrough).
std::uint32_t V0GuardDispatch(void) {
    if (V0GuardSelfTestEnabled() && g_v0guard_calls < kV0GuardMaxCompare) {
        std::uint32_t o = OrigVehicle0ZeroProgressGuard();
        std::uint32_t m = AiVehicle0ZeroProgressGuard();
        ++g_v0guard_calls;
        if (o != 0) ++g_v0guard_ret1;
        if (o != m) {
            ++g_v0guard_mismatch;
            char line[128];
            wsprintfA(line, "[%ld] MISMATCH o=%u m=%u\r\n", g_v0guard_calls, o, m);
            V0GuardSelfTestLog(line);
        }
        if ((g_v0guard_calls & 0x7f) == 1) {
            char line[128];
            wsprintfA(line, "[%ld] calls=%ld mism=%ld ret1=%ld ret0=%ld %s\r\n", g_v0guard_calls,
                      g_v0guard_calls, g_v0guard_mismatch, g_v0guard_ret1,
                      g_v0guard_calls - g_v0guard_ret1,
                      g_v0guard_mismatch ? "" : "ALL-GREEN");
            V0GuardSelfTestLog(line);
        }
        return o;
    }
    return OrigVehicle0ZeroProgressGuard();
}

// Naked entry installed at 0x00415200 — void(void) cdecl: just call the dispatch and RET,
// leaving the 0/1 result in EAX for the original caller (no args to clean).
__declspec(naked) void AiV0Guard_Entry() {
    __asm {
        call V0GuardDispatch
        ret
    }
}

}  // namespace

RH_ScopedInstall(AiV0Guard_Entry, 0x00415200);

// ===========================================================================
// 0x00415190  AiLeaderProgressRangeGate(veh, lo, hi) -> 1 / 0
//
// Exact disasm (pool2, 2026-09-01), body 0x00415190..0x004151f8:
//   00415190  8b442404          MOV EAX,[ESP+4]                 ; veh
//   00415194  57                PUSH EDI
//   00415195  50                PUSH EAX
//   00415196  e825db0200        CALL 0x00442cc0                 ; progress[veh] -> ST0
//   0041519b  d81d7c755d00      FCOMP float ptr [0x005d757c]    ; == 0.0f (at-grid)?
//   004151a1  83c404            ADD ESP,4
//   004151a4  dfe0/f6c444/7a4a  FNSTSW/TEST AH,0x44/JP 0x4151f5 ; not-equal -> return 0
//   004151ab  56 / 83cfff/33f6  PUSH ESI / EDI=-1 / ESI=0       ; iVar3=-1, i=0
//   004151b1  56 e8..0040e470   loop: FUN_0040e470(i)           ; -> EAX (int flag)
//   004151ba  83f801 7502 8bfe  CMP EAX,1; JNZ; MOV EDI,ESI     ; if ==1: iVar3=i
//   004151c1  46 83fe04 7cea    INC ESI; CMP 4; JL 0x4151b1
//   004151c7  83ffff 5e 7428    CMP EDI,-1; POP ESI; JZ 0x4151f5; iVar3==-1 -> 0
//   004151cd  57 e8..0x00442cc0 CALL FUN_00442cc0(iVar3) -> ST0 (progress[last])
//   004151d3  d8542410          FCOM  float ptr [ESP+0x10]      ; vs lo (param_2)
//   004151d7  83c404 dfe0 f6c401/7512  ADD 4; if ST0<lo (C0) -> FSTP+return 0
//   004151e1  d85c2410          FCOMP float ptr [ESP+0x10]      ; vs hi (param_3)
//   004151e5  dfe0 f6c441 7a09  if !(ST0<hi strict) -> return 0
//   004151ec  b801000000        MOV EAX,1                       ; else return 1
// Semantics: if progress[veh]==0.0f AND some vehicle `last` (the highest i in 0..3 with
// FUN_0040e470(i)==1) has lo <= progress[last] < hi, return 1, else 0.
//
// FUN_00442cc0 returns float10 in ST0 -> fn-ptr declared `float` (ST0 pop; the standing
// [[x87-st0-float10-fnptr-void-leak]] rule). FUN_0040e470 is consumed as EAX (int) by the
// original (CMP EAX,1), so its fn-ptr is `int`, no ST0 traffic. All three FP comparisons
// are float32 (dword operands) -> plain C is bit-identical under x87, no naked body.
// progress[last] is fetched ONCE and compared twice (FCOM then FCOMP), so the port must
// call the getter once and reuse. Caller: FUN_00415220 (C2, 4 sites — CallersPC 2026-09-01).
// ref: re/analysis/bucket_ai_00407a40_00415880/0x00415190.md
// ===========================================================================
namespace {
typedef int (__cdecl* fn_0040e470_t)(int);
inline int call_0040e470(int i) { return reinterpret_cast<fn_0040e470_t>(0x0040e470)(i); }
} // namespace

extern "C" __declspec(dllexport)
std::uint32_t __cdecl AiLeaderProgressRangeGate(int param_1, float param_2, float param_3)
{
    if (call_00442cc0(param_1) == F32(0x005d757cu)) {   // progress[veh] == 0.0f
        int last = -1;
        for (int i = 0; i < 4; ++i)
            if (call_0040e470(i) == 1) last = i;        // highest i with flag==1
        if (last != -1) {
            float q = call_00442cc0(last);              // progress[last] (once)
            if (param_2 <= q && q < param_3) return 1u;  // lo <= q < hi (strict)
        }
    }
    return 0u;
}

// ── in-race A/B self-test for AiLeaderProgressRangeGate (env MASHED_AI_LEADERGATE_SELFTEST) ──
// Same shape as SteerDispatch: PURE (two read-only getters, int return, no writes) -> A/B
// compares EAX and RETURNS ORIG (safe passthrough when the env var is unset). MUST run in a
// booted race (the progress array 0x008989b0 and FUN_0040e470's state are only live there).
namespace {

// Orig trampoline: the 5-byte RH_ScopedInstall JMP overwrites MOV EAX,[ESP+4] (4) + PUSH
// EDI (1) exactly (0x00415190..0x00415194); re-exec both then jmp to the next boundary
// 0x00415195 (PUSH EAX). [ESP+4] there == the trampoline caller's param_1 (veh), matching
// the original's own load, and PUSH EDI reproduces the frame the FCOM [ESP+0x10] offsets
// assume. cdecl float return is via ST0/EAX per the tail; here EAX (int).
void* g_orig_415195 = reinterpret_cast<void*>(0x00415195);
__declspec(naked) std::uint32_t OrigLeaderProgressRangeGate(int /*veh*/, float /*lo*/, float /*hi*/) {
    __asm {
        mov  eax, dword ptr [esp + 4]
        push edi
        jmp  dword ptr [g_orig_415195]
    }
}

inline int LeaderGateSelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_AI_LEADERGATE_SELFTEST"); v = (s && s[0]) ? 1 : 0; }
    return v;
}
void LeaderGateSelfTestLog(const char* s) {
    HANDLE h = CreateFileA("ai_leadergate_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}
long g_leadergate_calls = 0, g_leadergate_mismatch = 0;
const long kLeaderGateMaxCompare = 40000;

std::uint32_t LeaderGateDispatch(int veh, float lo, float hi) {
    if (LeaderGateSelfTestEnabled() && g_leadergate_calls < kLeaderGateMaxCompare) {
        std::uint32_t o = OrigLeaderProgressRangeGate(veh, lo, hi);
        std::uint32_t m = AiLeaderProgressRangeGate(veh, lo, hi);
        ++g_leadergate_calls;
        if (o != m) {
            ++g_leadergate_mismatch;
            char line[160];
            std::uint32_t lb, hb;
            std::memcpy(&lb, &lo, 4); std::memcpy(&hb, &hi, 4);
            wsprintfA(line, "[%ld] MISMATCH o=%u m=%u  veh=%d lo=%08x hi=%08x\r\n",
                      g_leadergate_calls, o, m, veh, lb, hb);
            LeaderGateSelfTestLog(line);
        }
        if ((g_leadergate_calls & 0x7f) == 1) {
            char line[128];
            wsprintfA(line, "[%ld] calls=%ld mism=%ld %s\r\n", g_leadergate_calls, g_leadergate_calls,
                      g_leadergate_mismatch, g_leadergate_mismatch ? "" : "ALL-GREEN");
            LeaderGateSelfTestLog(line);
        }
        return o;
    }
    return OrigLeaderProgressRangeGate(veh, lo, hi);
}

// Naked entry at 0x00415190 — forward the 3 cdecl stack args (veh,lo,hi) to the dispatch,
// clean our copies, RET with EAX = result (original is caller-cleans).
__declspec(naked) void AiLeaderGate_Entry() {
    __asm {
        // [esp]=ret [esp+4]=veh [esp+8]=lo [esp+0xc]=hi
        push dword ptr [esp + 0x0c]   // hi
        push dword ptr [esp + 0x0c]   // lo
        push dword ptr [esp + 0x0c]   // veh
        call LeaderGateDispatch
        add  esp, 0x0c
        ret
    }
}

}  // namespace

RH_ScopedInstall(AiLeaderGate_Entry, 0x00415190);
