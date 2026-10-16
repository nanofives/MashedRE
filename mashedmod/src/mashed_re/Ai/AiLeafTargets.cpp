// Mashed RE — D3 bulk G-AILEAF (verify/d3_bulk_20261011/PLAN.md §0, §3 rank 2, §4).
//
// FUN_00414570 (mode-1 candidate) and FUN_00415880 (mode-2 candidate), the first two leaves of
// FUN_00416250's targeting chain (calls 0x004162f7 / 0x0041633f), plus FUN_0046d570, verbatim naked x87.
// Generated from the anchored bytes by re/tools/gen_naked_asm.py (spec re/tools/naked_specs/aileaf_g1.json
// -> AiLeafTargets_naked.inc). Also verbatim in that file, private to this TU (AL_ prefix) so the replay
// can serve every host input: FUN_00443440 (spline curvature), FUN_00443300, FUN_004233e0, FUN_00408af0,
// FUN_00408a50, FUN_00426bb0, FUN_0040e470, FUN_004c39b0, FUN_004c3ac0 (RW LUT base patched).
//
// Host bindings (cdecl, callee-saved registers preserved by the compiler):
//   AL_VehPtr     FUN_0046d4a0 -> PtrCompute881ec8 (exe-rebound record array)
//   AL_Speed      FUN_0046d6d0 -> record +0x9e4
//   AL_Fwd        FUN_0046d510 -> the record's +0x928 matrix `at` row (does NOT store record +0x9d4,
//                 same as P3's TG_Fwd: the exe's physics owns that field)
//   AL_Alive      FUN_0046c7b0 -> VehicleSlotGetter (record-rebound)
//   AL_State      FUN_0046cbb0 -> VehicleCarStateRead (record-rebound)
//   AL_FloatAsInt FUN_0046cc10 -> truncation of record +0x9e0. The exe's VehicleFloatFieldAsInt reads the
//                 absolute 0x00881f80, the blank pad in the exe, so it is NOT used here.
//   AL_Acos       FUN_0x004a3384 CRT acos, transcribed from 0x004a3384 / 0x004a70a5 / 0x004a70d5 /
//                 0x004a713b / 0x004a7047 (see the function).
//   AL_Ftol       FUN_004a2c48 __ftol2 (truncate ST0 into EDX:EAX)
//   AL_DbgRaster / AL_DbgDraw  FUN_004671a0 / FUN_004b55a0: FUN_00443440's debug-draw arm, taken only
//                 when its 5th argument is 1; both callers pass 0. Counted.
//   AL_ErrPass / AL_Nop        FUN_004c39b0's RW error funnel.
// Patches: the indexed read of the .data table 0x005f2fc0 at 0x004158c4 -> exe-safe copy; FUN_0046d570's
// record base (imul at 0x0046d588) + g_al_recdelta, so [esi + 0x881f50..7c] land in the exe's record array.
// The absolute .bss globals the bodies read stay at their original VA (the exe maps 0x00500000..0x009fffff
// blank): 0x007f0fd0, 0x007f1008, 0x0089a360/368/374, the AI slot table 0x0089a4c0 + v*0x74, the progress
// block 0x008a96dc + v*0x30c, 0x0066d6d8, 0x005f2770.
#include "AiLeafTargets.h"
#include "../Math/RwLutTables.h"
#include "../Vehicle/VehicleRecordBase.h"
#include "../Race/PathNodes.h"            // PathProg_Enabled: the progress producer FUN_00414570 / FUN_00415880 read

#include <float.h>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

extern "C" std::uint32_t __cdecl PtrCompute881ec8(std::uint32_t* out, std::uint32_t idx);            // 0x0046d4a0
extern "C" std::uint32_t __cdecl VehicleSlotGetter(std::uint32_t vehicleIdx);                         // 0x0046c7b0
extern "C" int __cdecl VehicleCarStateRead(std::uint32_t carIdx, std::uint32_t* a, std::uint32_t* b); // 0x0046cbb0

// ---- exe-safe .rdata / .data copies (values read from the anchored image) ------------------------
extern "C" {
const std::uint32_t g_al_c_5cc318 = 0x3f19999au;   // 0.6
const std::uint32_t g_al_c_5cc31c = 0x40400000u;   // 3.0
const std::uint32_t g_al_c_5cc320 = 0x3f800000u;   // 1.0
const std::uint32_t g_al_c_5cc328 = 0x3c23d70au;   // 0.01
const std::uint32_t g_al_c_5cc32c = 0x3f000000u;   // 0.5
const std::uint32_t g_al_c_5cc33c = 0xbf800000u;   // -1.0
const std::uint32_t g_al_c_5cc358 = 0x40a00000u;   // 5.0
const std::uint32_t g_al_c_5cc35c = 0x40800000u;   // 4.0
const std::uint32_t g_al_c_5cc56c = 0x3dcccccdu;   // 0.1
const std::uint32_t g_al_c_5cc574 = 0x40000000u;   // 2.0
const std::uint32_t g_al_c_5cc72c = 0x41f00000u;   // 30.0
const std::uint32_t g_al_c_5cc950 = 0x3f400000u;   // 0.75
const std::uint32_t g_al_c_5cc9b8 = 0x40e00000u;   // 7.0
const std::uint32_t g_al_c_5cc9c0 = 0x3e4ccccdu;   // 0.2
const std::uint32_t g_al_c_5cc9fc = 0x447a0000u;   // 1000.0
const std::uint32_t g_al_c_5ccac4 = 0x43b40000u;   // 360.0
const std::uint32_t g_al_c_5ccad0 = 0x42b40000u;   // 90.0
const std::uint64_t g_al_c_5ccae0 = 0x404ca5dcc0000000ull;   // qword, 57.29577...
const std::uint32_t g_al_c_5ccd04 = 0x43fa0000u;   // 500.0
const std::uint32_t g_al_c_5ccd6c = 0x41a00000u;   // 20.0
const std::uint32_t g_al_c_5cd050 = 0x3e000000u;   // 0.125
const std::uint32_t g_al_c_5cd094 = 0x42652ee6u;   // 57.2958
const std::uint32_t g_al_c_5cd098 = 0x451c4000u;   // 2500.0
const std::uint32_t g_al_c_5cd09c = 0x43340000u;   // 180.0
const std::uint32_t g_al_c_5cd0ac = 0x44bb8000u;   // 1500.0
const std::uint32_t g_al_c_5cd324 = 0x43870000u;   // 270.0
const std::uint32_t g_al_c_5ce034 = 0x3eaaaa9fu;   // 0.33333
const std::uint32_t g_al_c_5d757c = 0x00000000u;   // 0.0
// .data 0x005f2fc0 (file-backed), first 64 dwords; FUN_00415880 indexes it with
// DAT_0089a374 + ftol(DAT_0089a360) * 5 (0x004158b9..0x004158c4). Live: 0 + 2*5 = 10.
const std::uint32_t g_al_tab_5f2fc0[64] = {
    0x0u,0x0u,0x0u,0x1u,0x1u,0x0u,0x0u,0x0u,0x1u,0x1u,0x0u,0x0u,0x0u,0x1u,0x1u,0x0u,
    0x0u,0x0u,0x1u,0x1u,0x0u,0x0u,0x1u,0x1u,0x1u,0x0u,0x0u,0x1u,0x1u,0x1u,0x1u,0x1u,
    0x1u,0x1u,0x1u,0x1u,0x1u,0x2u,0x2u,0x2u,0x1u,0x2u,0x2u,0x2u,0x2u,0x1u,0x2u,0x2u,
    0x2u,0x2u,0x1u,0x2u,0x2u,0x2u,0x2u,0x0u,0x3cu,0x50u,0x5au,0x5au,0x64u,0x28u,0x3cu,0x4bu };
// [*(0x007d3ff8) + *(0x007d3ffc)] = {sqrt table, inv sqrt table}
struct AlLut { const std::uint32_t* sqrt_; const std::uint32_t* inv_; };
AlLut g_al_lutptrs = { nullptr, nullptr };
// FUN_0046d570: (record array base) - 0x008815a0, added to the v*0xd04 index at 0x0046d588.
std::uint32_t g_al_recdelta = 0;
}

namespace {

// ---- replay context ---------------------------------------------------------------------------
struct CarIn {
    float x, z, speed, at[3];
    std::uint32_t alive; std::int32_t stA, stB, slot;
    float prog, vec[3];
    std::int32_t fai, pad;
};
static_assert(sizeof(CarIn) == 64, "CarIn row layout");
bool  s_replay = false;
CarIn s_car[4];
std::uint32_t s_mat[4][16];

long s_calls[2] = {0, 0}, s_ret1[2] = {0, 0}, s_acc[2] = {0, 0}, s_accNoCommit[2] = {0, 0};
long s_dbg = 0;

inline const char* Rec(std::uint32_t idx) { return mashed_re::Vehicle::VehRecord(idx); }

std::string Hex(const void* p, std::size_t n) {
    static const char* d = "0123456789abcdef";
    std::string s(n * 2, '0');
    const unsigned char* b = static_cast<const unsigned char*>(p);
    for (std::size_t i = 0; i < n; ++i) { s[2 * i] = d[b[i] >> 4]; s[2 * i + 1] = d[b[i] & 15]; }
    return s;
}

}  // namespace

extern "C" {

std::uint32_t __cdecl AL_VehPtr(std::uint32_t* out, std::uint32_t idx) {       // 0x0046d4a0
    if (idx > 0xfu) return 0;
    if (s_replay) { *out = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&s_mat[idx & 3][0])); return 1; }
    return PtrCompute881ec8(out, idx);
}

std::uint32_t __cdecl AL_Speed(float* out, std::uint32_t idx) {                 // 0x0046d6d0
    if (idx > 0xfu) return 0;
    if (s_replay) { *out = s_car[idx & 3].speed; return 1; }
    const char* r = Rec(idx);
    *out = r ? *reinterpret_cast<const float*>(r + 0x9e4) : 0.0f;
    return 1;
}

std::uint32_t __cdecl AL_Fwd(float* out, std::uint32_t idx) {                   // 0x0046d510
    if (idx > 0xfu) return 0;
    if (s_replay) { for (int k = 0; k < 3; ++k) out[k] = s_car[idx & 3].at[k]; return 1; }
    const char* r = Rec(idx);
    if (!r) { out[0] = out[1] = out[2] = 0.0f; return 1; }
    const float* m = reinterpret_cast<const float*>(r + mashed_re::Vehicle::kOff_WheelMatrices);
    for (int k = 0; k < 3; ++k) out[k] = m[8 + k];                              // `at` row (+0x20)
    return 1;
}

std::uint32_t __cdecl AL_Alive(std::uint32_t idx) {                             // 0x0046c7b0
    if (idx > 0xfu) return 0xffffffffu;
    if (s_replay) return s_car[idx & 3].alive;
    return VehicleSlotGetter(idx);
}

int __cdecl AL_State(std::uint32_t idx, std::int32_t* a, std::int32_t* b) {     // 0x0046cbb0
    if (idx > 0xfu) return 0;
    if (s_replay) { *a = s_car[idx & 3].stA; *b = s_car[idx & 3].stB; return 1; }
    return VehicleCarStateRead(idx, reinterpret_cast<std::uint32_t*>(a), reinterpret_cast<std::uint32_t*>(b));
}

int __cdecl AL_FloatAsInt(std::int32_t* out, std::uint32_t idx) {               // 0x0046cc10
    if (idx >= 0x10u) return 0;                                                 // *out untouched
    if (s_replay) { *out = s_car[idx & 3].fai; return 1; }
    const char* r = Rec(idx);
    *out = r ? static_cast<std::int32_t>(*reinterpret_cast<const float*>(r + 0x9e0)) : 0;   // __ftol truncates
    return 1;
}

int __cdecl AL_DbgRaster() { ++s_dbg; return 0; }                              // 0x004671a0 (debug arm only)
int __cdecl AL_DbgDraw()   { ++s_dbg; return 0; }                              // 0x004b55a0 (debug arm only)
__declspec(naked) void AL_Nop()     { __asm { ret } }
__declspec(naked) void AL_ErrPass() { __asm { xor eax, eax
                                              ret } }

// FUN_004a2c48 __ftol2: truncate ST0 (popped) into EDX:EAX; touches only EAX/EDX.
__declspec(naked) void AL_Ftol() {
    __asm {
        sub    esp, 12
        fnstcw word ptr [esp]
        mov    ax, word ptr [esp]
        or     ax, 0x0c00
        mov    word ptr [esp + 2], ax
        fldcw  word ptr [esp + 2]
        fistp  qword ptr [esp + 4]
        fldcw  word ptr [esp]
        mov    eax, dword ptr [esp + 4]
        mov    edx, dword ptr [esp + 8]
        add    esp, 12
        ret
    }
}

// CRT acos(double) at 0x004a3384, the paths the race can reach:
//   0x004a70d5   x = [esp+4] (qword); exponent all-ones -> NaN/Inf path (not reproduced: [UNCERTAIN], the
//                callers clamp to [-1, 1] first, 0x00414807..0x0041482f and FUN_0046d570's clamp)
//   0x004a338f   save the caller's CW; if != 0x27f, 0x004a70a5 loads (CW & 0x300) | 0x7f
//                (caller's precision kept, round-to-nearest, all exceptions masked)
//   |x| < 1      fld1; fadd st,st1; fld1; fsub st,st2; fmulp; fsqrt; fxch; fpatan
//   |x| == 1     +1 -> fldz, -1 -> fldpi (0x004a33e8..0x004a33f6)
//   exit         [0x00773990] == 0 (live witness: 0 in the race, verify/d3_bulk_20261011/peek2_orig.json)
//                -> 0x004a713b: CW == 0x27f -> pop, ret; else if (CW & 0x20) and FSW.PE -> 0x004a7047:
//                fstp qword / fld qword (result rounded to double), fldcw CW; else fldcw CW.
//                0x004a7047's call to 0x004ae17b (CRT error report) is not reproduced: [UNCERTAIN] side
//                effects (errno), the value path is the qword round trip.
//   |x| > 1      domain error path 0x004a340f (not reachable after the callers' clamp; [UNCERTAIN]).
__declspec(naked) void AL_Acos() {
    __asm {
        mov    eax, dword ptr [esp + 8]
        and    eax, 0x7ff00000
        fld    qword ptr [esp + 4]
        sub    esp, 12
        fnstcw word ptr [esp]                       // caller CW
        mov    dx, word ptr [esp]
        cmp    dx, 0x27f
        je     L_cw_done
        and    dx, 0x300
        or     dx, 0x7f
        mov    word ptr [esp + 2], dx
        fldcw  word ptr [esp + 2]
    L_cw_done:
        cmp    eax, 0x3ff00000
        jae    L_one
        fld1
        fadd   st, st(1)
        fld1
        fsub   st, st(2)
        fmulp  st(1), st
        fsqrt
        fxch   st(1)
        fpatan
        jmp    L_exit
    L_one:
        // exponent >= 0x3ff: exactly +-1 here (callers clamp)
        mov    ecx, dword ptr [esp + 20]            // high dword of x (12 + 8)
        fstp   st(0)
        test   ecx, 0x80000000
        je     L_pos
        fldpi
        jmp    L_exit
    L_pos:
        fldz
    L_exit:
        mov    ax, word ptr [esp]
        cmp    ax, 0x27f
        je     L_ret
        and    ax, 0x20
        je     L_restore
        fnstsw ax
        and    ax, 0x20
        je     L_restore
        fstp   qword ptr [esp + 4]                  // 0x004a7050: round to double
        fld    qword ptr [esp + 4]                  // 0x004a7073
    L_restore:
        fldcw  word ptr [esp]
    L_ret:
        add    esp, 12
        ret
    }
}

}  // extern "C"

#include "AiLeafTargets_naked.inc"

namespace Ai {

namespace {

typedef int (__cdecl* leaf_fn)(std::uint32_t spline, const float* look, float* cand, int v);
typedef int (__cdecl* d570_fn)(float* out, std::uint32_t v);

void EnsureLut() {
    if (!g_al_lutptrs.sqrt_) {
        g_al_lutptrs.sqrt_ = mashed_re::Math::RwLutSqrtTable();
        g_al_lutptrs.inv_  = mashed_re::Math::RwLutInvSqrtTable();
    }
    const char* base = Rec(0);
    g_al_recdelta = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(base)) - 0x008815a0u;
}

// x87 precision: MASHED_AILEAF_PC = 24 | 53 | 64; default 24 (the race loop's).
unsigned int PcBits() {
    const char* s = std::getenv("MASHED_AILEAF_PC");
    if (s && std::strcmp(s, "53") == 0) return _PC_53;
    if (s && std::strcmp(s, "64") == 0) return _PC_64;
    return _PC_24;
}
struct PcScope {
    unsigned int old = 0;
    PcScope() { _controlfp_s(&old, 0, 0); unsigned int t; _controlfp_s(&t, PcBits(), _MCW_PC); }   // read first (U-9216)
    ~PcScope() { unsigned int t; _controlfp_s(&t, old & _MCW_PC, _MCW_PC); }
};

std::FILE* Trace() {
    static std::FILE* f = nullptr; static bool init = false;
    if (!init) {
        init = true;
        if (const char* p = std::getenv("MASHED_AILEAF_TRACE")) {
            f = std::fopen(p, "w");
            if (f) std::fprintf(f, "n\tframe\tleaf\tv\tret\tcand_x\tcand_z\tslot508\tslot4dc\tslot4e0\n");
        }
    }
    return f;
}

inline std::int32_t& I32v(std::uintptr_t a) { return *reinterpret_cast<std::int32_t*>(a); }

void TraceRow(int leaf, int v, int ret, const float* cand) {
    std::FILE* f = Trace();
    if (!f) return;
    static long n = 0;
    const std::uintptr_t blk = 0x0089a4c0u + static_cast<std::uintptr_t>(v) * 0x74u;
    std::fprintf(f, "%ld\t%d\t%s\t%d\t%d\t%.9g\t%.9g\t%d\t%d\t%d\n", ++n, I32v(0x007f0ff4u), leaf ? "15880" : "14570", v, ret,
                 cand[0], cand[1], I32v(blk + 0x48), I32v(blk + 0x1c), I32v(blk + 0x20));
    if (leaf == 0 && ret == 1) {
        // Diagnostic: every input of FUN_00414570's acceptance test, as the body read it.
        const std::uint32_t tab = static_cast<std::uint32_t>(I32v(0x005f2770u));
        std::fprintf(f, "#in14570 v=%d nodes=%d fd0=%d f4c0=%d", v, I32v(0x0066d6d8u), I32v(0x007f0fd0u), I32v(blk));
        for (int c = 0; c < 4; ++c) {
            const char* r = Rec(static_cast<std::uint32_t>(c));
            std::uint32_t mp = 0; PtrCompute881ec8(&mp, static_cast<std::uint32_t>(c));
            const float* m = reinterpret_cast<const float*>(static_cast<std::uintptr_t>(mp));
            std::uint32_t sa = 0, sb = 0; VehicleCarStateRead(static_cast<std::uint32_t>(c), &sa, &sb);
            std::fprintf(f, " | c%d alive=%u slot=%d st=%u prog=%.6g speed=%.6g x=%.6g z=%.6g", c, VehicleSlotGetter(static_cast<std::uint32_t>(c)),
                         tab ? *reinterpret_cast<const std::int32_t*>(static_cast<std::uintptr_t>(tab) + 0x34u + c * 4u) : -99, sa,
                         *reinterpret_cast<const float*>(0x008a96e8u + c * 0x30cu), r ? *reinterpret_cast<const float*>(r + 0x9e4) : -1.0f,
                         m ? m[12] : 0.0f, m ? m[14] : 0.0f);
        }
        std::fprintf(f, "\n");
    }
    if ((n & 63) == 0) std::fflush(f);
}

}  // namespace

bool AiLeaf_Enabled() {
    static const bool s_on = [] {
        const char* s = std::getenv("MASHED_AILEAF");
        return s && s[0] && s[0] != '0';
    }();
    // Inputs with no exe writer unless MASHED_PATHPROG: the per-car progress 0x008a96e8 + v*0x30c (FUN_00408a50)
    // and the node count 0x0066d6d8 (FUN_00426bb0). Without them FUN_00414570's "ahead in race" test admits every
    // live car (progress 0 - 0 < 5 - 0), so the knob is inert without PATHPROG, as MASHED_TGT3 is.
    return s_on && mashed_re::Race::PathProg_Enabled();
}

int AiLeaf_414570(unsigned int spline, int v, const float look[2], float cand[2]) {
    if (v < 0 || v > 3 || !Rec(0)) return 0;
    EnsureLut();
    // DIAGNOSTIC BRIDGE (Phase 2 experiment, default OFF): MASHED_AILEAF_SEED4C0=<n> writes n into the AI factor
    // 0x0089a4c0 + v*0x74 before the call. The exe has no writer for it (original writers 0x00414208 FUN_00414180,
    // 0x00417ca0 FUN_004177b0); the original holds 2 on all four cars (verify/d3_bulk_20261011/peek_orig.json,
    // 63/63 phase-3 samples). Measurement only: it is not a port.
    static const char* s_seed = std::getenv("MASHED_AILEAF_SEED4C0");
    if (s_seed) I32v(0x0089a4c0u + static_cast<std::uintptr_t>(v) * 0x74u) = std::atoi(s_seed);
    // G-AILEAF-LIVE (diagnostic, default OFF): MASHED_AILEAF_LIVEROWS=<tsv> writes this call's full input set and the
    // body's outputs in the G-AILEAF-REPLAY row format (every return-1 call + every 50th call), so
    // re/tools/aileaf_unicorn.py --check can run the SAME inputs through the original bytes.
    static std::FILE* s_live = [] { const char* p = std::getenv("MASHED_AILEAF_LIVEROWS"); return p ? std::fopen(p, "w") : nullptr; }();
    CarIn liveCars[4]; std::uint32_t liveGlob[6]; unsigned char liveAi[0x74]; float liveCandIn[2] = { cand[0], cand[1] };
    unsigned short liveCw = 0;
    const std::uintptr_t liveBlk = 0x0089a4c0u + static_cast<std::uintptr_t>(v) * 0x74u;
    if (s_live) {
        std::memset(liveCars, 0, sizeof liveCars);
        const std::uint32_t tab = static_cast<std::uint32_t>(I32v(0x005f2770u));
        for (int c = 0; c < 4; ++c) {
            CarIn& L = liveCars[c];
            std::uint32_t mp = 0; PtrCompute881ec8(&mp, static_cast<std::uint32_t>(c));
            const float* m = reinterpret_cast<const float*>(static_cast<std::uintptr_t>(mp));
            L.x = m ? m[12] : 0.0f; L.z = m ? m[14] : 0.0f;
            AL_Speed(&L.speed, static_cast<std::uint32_t>(c)); AL_Fwd(L.at, static_cast<std::uint32_t>(c));
            L.alive = AL_Alive(static_cast<std::uint32_t>(c)); AL_State(static_cast<std::uint32_t>(c), &L.stA, &L.stB);
            L.slot = tab ? *reinterpret_cast<const std::int32_t*>(static_cast<std::uintptr_t>(tab) + 0x34u + c * 4u) : 0;
            std::memcpy(&L.prog, reinterpret_cast<const void*>(0x008a96e8u + c * 0x30cu), 4);
            std::memcpy(L.vec, reinterpret_cast<const void*>(0x008a96dcu + c * 0x30cu), 12);
            AL_FloatAsInt(&L.fai, static_cast<std::uint32_t>(c));
        }
        liveGlob[0] = I32v(0x007f0fd0u); liveGlob[1] = I32v(0x0066d6d8u); liveGlob[2] = I32v(0x007f1008u);
        liveGlob[3] = I32v(0x0089a360u); liveGlob[4] = I32v(0x0089a368u); liveGlob[5] = I32v(0x0089a374u);
        std::memcpy(liveAi, reinterpret_cast<const void*>(liveBlk), 0x74);
    }
    int ret;
    { PcScope pc; if (s_live) { __asm { fnstcw liveCw } } ret = reinterpret_cast<leaf_fn>(&AL_414570)(spline, look, cand, v); }
    if (s_live) {
        static long nLive = 0; ++nLive;
        if (ret == 1 || (nLive % 50) == 0) {
            std::fprintf(s_live, "570\t%ld\t%d\t%x\t%s\t%s\t%s\t%s\t%s\t%s\t\t%d\t%s\t%s\t00000000\n", nLive, v, liveCw,
                         Hex(reinterpret_cast<const void*>(static_cast<std::uintptr_t>(spline)), 0x204).c_str(), Hex(look, 8).c_str(),
                         Hex(liveCandIn, 8).c_str(), Hex(liveCars, sizeof liveCars).c_str(), Hex(liveGlob, 24).c_str(),
                         Hex(liveAi, 0x74).c_str(), ret, Hex(cand, 8).c_str(), Hex(reinterpret_cast<const void*>(liveBlk), 0x74).c_str());
            std::fflush(s_live);
        }
    }
    ++s_calls[0]; if (ret) ++s_ret1[0];
    TraceRow(0, v, ret, cand);
    return ret;
}

int AiLeaf_415880(unsigned int spline, int v, const float look[2], float cand[2]) {
    if (v < 0 || v > 3 || !Rec(0)) return 0;
    EnsureLut();
    int ret;
    { PcScope pc; ret = reinterpret_cast<leaf_fn>(&AL_415880)(spline, look, cand, v); }
    ++s_calls[1]; if (ret) ++s_ret1[1];
    TraceRow(1, v, ret, cand);
    return ret;
}

// FUN_0046d570(&out, v): the verbatim naked AL_46d570, run under the same x87 precision scope and record rebase as the two
// leaves above. Called by the shared FUN_00417640 body (Ai/AiPostStepBrake.cpp) in the exe. 0.0f when the record array is not up
// (no race physics yet), the value an unwritten out-slot would hold in the original's caller.
float AiLeaf_46d570(int v) {
    if (v < 0 || v > 3 || !Rec(0)) return 0.0f;
    EnsureLut();
    float out = 0.0f;
    { PcScope pc; reinterpret_cast<d570_fn>(&AL_46d570)(&out, static_cast<std::uint32_t>(v)); }
    return out;
}

void AiLeaf_CountAccept(int leaf, int committed) {
    if (leaf < 0 || leaf > 1) return;
    ++s_acc[leaf];
    if (!committed) ++s_accNoCommit[leaf];
    if (std::FILE* f = Trace()) {
        std::fprintf(f, "#accept leaf=%s committed=%d calls=%ld ret1=%ld acc=%ld acc_nocommit=%ld\n",
                     leaf ? "15880" : "14570", committed, s_calls[leaf], s_ret1[leaf], s_acc[leaf], s_accNoCommit[leaf]);
        std::fflush(f);
    }
}

// ======================================================================================
// G-AILEAF-REPLAY. Row format (re/tools/aileaf_unicorn.py writes it, tab-separated):
//   kind n v cw spline(0x204 B hex, 570 only) look(8 B) cand_in(8 B) cars(4 x 64 B CarIn) globals(24 B:
//   7f0fd0 66d6d8 7f1008 89a360 89a368 89a374) ai_in(0x74 B at 0x0089a4c0 + v*0x74) d570_in(24 B: record
//   +0x9b0 and +0x9d4 vec3, d570 only) | expected: ret cand_out(8 B) ai_out(0x74 B) d570_out(4 B)
// The row's globals are written at their original VAs, the call runs under the row's control word, and
// every touched VA is restored afterwards. The CW is the hardware word (0x007f = PC_24, 0x027f = PC_53).
// ======================================================================================
namespace {
bool FromHex(const std::string& h, void* out, std::size_t n) {
    if (h.size() != n * 2) return false;
    unsigned char* o = static_cast<unsigned char*>(out);
    for (std::size_t i = 0; i < n; ++i) {
        unsigned b = 0;
        if (std::sscanf(h.c_str() + i * 2, "%2x", &b) != 1) return false;
        o[i] = static_cast<unsigned char>(b);
    }
    return true;
}
std::vector<std::string> Split(const std::string& s, char d) {
    std::vector<std::string> o; std::size_t p = 0;
    while (true) { std::size_t q = s.find(d, p); o.push_back(s.substr(p, q == std::string::npos ? std::string::npos : q - p));
                   if (q == std::string::npos) break; p = q + 1; }
    return o;
}
struct Saved { std::uintptr_t a; std::vector<unsigned char> b; };
void SaveRange(std::vector<Saved>& v, std::uintptr_t a, std::size_t n) {
    Saved s; s.a = a; s.b.assign(reinterpret_cast<unsigned char*>(a), reinterpret_cast<unsigned char*>(a) + n); v.push_back(s);
}
}  // namespace

void AiLeaf_MaybeReplay() {
    const char* in = std::getenv("MASHED_AILEAF_REPLAY");
    if (!in) return;
    const char* outp = std::getenv("MASHED_AILEAF_REPLAY_OUT");
    std::FILE* o = outp ? std::fopen(outp, "w") : nullptr;
    if (!o) return;
    EnsureLut();
    std::vector<std::string> files = Split(in, ';');
    std::FILE* rowsOut = nullptr;
    if (const char* rp = std::getenv("MASHED_AILEAF_REPLAY_ROWS")) {
        rowsOut = std::fopen(rp, "w");
        if (rowsOut) std::fprintf(rowsOut, "kind\tn\tcw\tret\tcand\tai\td570\n");
    }
    long rows = 0, exact = 0, retBad = 0, candBad = 0, aiBad = 0, dBad = 0, parseBad = 0;
    long byKind[3] = {0, 0, 0}, byRet1[3] = {0, 0, 0};
    // fake records for FUN_0046d570 (record array base moved to this buffer for the call)
    static std::vector<unsigned char> fakeRec(0xd04u * 4u, 0);
    // fake slot-state table for FUN_0040e470: *(0x005f2770) -> table, entry c at +0x34 + c*4
    static std::uint32_t fakeSlot[0x20];
    static unsigned char splineBuf[0x204 + 64];
    for (const std::string& fn : files) {
        std::FILE* f = std::fopen(fn.c_str(), "r");
        if (!f) { std::fprintf(o, "cannot open %s\n", fn.c_str()); continue; }
        std::string line; int c;
        while (true) {
            line.clear();
            while ((c = std::fgetc(f)) != EOF && c != '\n') if (c != '\r') line.push_back(static_cast<char>(c));
            if (line.empty() && c == EOF) break;
            if (line.empty() || line[0] == '#') { if (c == EOF) break; continue; }
            const std::vector<std::string> k = Split(line, '\t');
            if (k.size() < 15) { ++parseBad; continue; }
            const std::string kind = k[0];
            const int v = std::atoi(k[2].c_str());
            const unsigned int cw = static_cast<unsigned int>(std::strtoul(k[3].c_str(), nullptr, 16));
            float look[2], candIn[2], candExp[2];
            std::uint32_t glob[6];
            unsigned char aiIn[0x74], aiExp[0x74], d570In[24];
            float d570Exp = 0.0f;
            if (v < 0 || v > 3 || !FromHex(k[5], look, 8) || !FromHex(k[6], candIn, 8) || !FromHex(k[7], s_car, sizeof s_car) ||
                !FromHex(k[8], glob, 24) || !FromHex(k[9], aiIn, 0x74) || !FromHex(k[12], candExp, 8) ||
                !FromHex(k[13], aiExp, 0x74)) { ++parseBad; continue; }
            if (kind == "570" && !FromHex(k[4], splineBuf, 0x204)) { ++parseBad; continue; }
            if (kind == "d570" && (!FromHex(k[10], d570In, 24) || !FromHex(k[14], &d570Exp, 4))) { ++parseBad; continue; }
            const int retExp = std::atoi(k[11].c_str());
            const int ki = kind == "570" ? 0 : kind == "880" ? 1 : 2;
            // stage the row
            std::vector<Saved> sv;
            const std::uintptr_t blk = 0x0089a4c0u + static_cast<std::uintptr_t>(v) * 0x74u;
            SaveRange(sv, 0x007f0fd0u, 4); SaveRange(sv, 0x0066d6d8u, 4); SaveRange(sv, 0x007f1008u, 4);
            SaveRange(sv, 0x0089a360u, 4); SaveRange(sv, 0x0089a368u, 4); SaveRange(sv, 0x0089a374u, 4);
            SaveRange(sv, blk, 0x74); SaveRange(sv, 0x005f2770u, 4);
            for (int cc = 0; cc < 4; ++cc) SaveRange(sv, 0x008a96dcu + cc * 0x30cu, 0x10);
            I32v(0x007f0fd0u) = static_cast<std::int32_t>(glob[0]); I32v(0x0066d6d8u) = static_cast<std::int32_t>(glob[1]);
            I32v(0x007f1008u) = static_cast<std::int32_t>(glob[2]); I32v(0x0089a360u) = static_cast<std::int32_t>(glob[3]);
            I32v(0x0089a368u) = static_cast<std::int32_t>(glob[4]); I32v(0x0089a374u) = static_cast<std::int32_t>(glob[5]);
            std::memcpy(reinterpret_cast<void*>(blk), aiIn, 0x74);
            std::memset(fakeSlot, 0, sizeof fakeSlot);
            for (int cc = 0; cc < 4; ++cc) {
                fakeSlot[0x34 / 4 + cc] = static_cast<std::uint32_t>(s_car[cc].slot);
                std::memcpy(reinterpret_cast<void*>(0x008a96dcu + cc * 0x30cu), s_car[cc].vec, 12);
                std::memcpy(reinterpret_cast<void*>(0x008a96e8u + cc * 0x30cu), &s_car[cc].prog, 4);
                std::memset(s_mat[cc], 0, sizeof s_mat[cc]);
                std::memcpy(&s_mat[cc][12], &s_car[cc].x, 4);
                std::memcpy(&s_mat[cc][14], &s_car[cc].z, 4);
            }
            I32v(0x005f2770u) = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(fakeSlot));
            float cand[2] = { candIn[0], candIn[1] };
            int ret = 0; float d570Out = 0.0f;
            s_replay = true;
            {
                // The row's HARDWARE control word, loaded as-is (fldcw). _controlfp_s takes MSVC's abstract
                // encoding (_PC_24 = 0x20000, _RC_UP = 0x200), so passing it 0x027f selected round-up at PC_64:
                // the first replay of this harness ran that way (RESULT_AILEAF.md, finding 3).
                unsigned short hwOld = 0, hwRow = static_cast<unsigned short>(cw);
                __asm { fnstcw hwOld }
                __asm { fclex }                    // the Unicorn rows start from fninit: same sticky flags (none)
                __asm { fldcw hwRow }
                if (ki == 0) {
                    ret = reinterpret_cast<leaf_fn>(&AL_414570)(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(splineBuf)),
                                                             look, cand, v);
                } else if (ki == 1) {
                    ret = reinterpret_cast<leaf_fn>(&AL_415880)(0u, look, cand, v);
                } else {
                    std::memset(fakeRec.data(), 0, fakeRec.size());
                    std::memcpy(fakeRec.data() + v * 0xd04u + 0x9b0u, d570In, 12);
                    std::memcpy(fakeRec.data() + v * 0xd04u + 0x9d4u, d570In + 12, 12);
                    const std::uint32_t keep = g_al_recdelta;
                    g_al_recdelta = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(fakeRec.data())) - 0x008815a0u;
                    ret = reinterpret_cast<d570_fn>(&AL_46d570)(&d570Out, static_cast<std::uint32_t>(v));
                    g_al_recdelta = keep;
                }
                __asm { fclex }
                __asm { fldcw hwOld }
            }
            s_replay = false;
            unsigned char aiOut[0x74];
            std::memcpy(aiOut, reinterpret_cast<void*>(blk), 0x74);
            for (auto it = sv.rbegin(); it != sv.rend(); ++it)
                std::memcpy(reinterpret_cast<void*>(it->a), it->b.data(), it->b.size());
            if (rowsOut) std::fprintf(rowsOut, "%s\t%s\t%x\t%d\t%s\t%s\t%s\n", kind.c_str(), k[1].c_str(), cw, ret,
                                      Hex(cand, 8).c_str(), Hex(aiOut, 0x74).c_str(), Hex(&d570Out, 4).c_str());
            ++rows; ++byKind[ki]; if (retExp != 0) ++byRet1[ki];
            const bool rOk = ret == retExp;
            const bool cOk = std::memcmp(cand, candExp, 8) == 0;
            const bool aOk = std::memcmp(aiOut, aiExp, 0x74) == 0;
            const bool dOk = ki != 2 || std::memcmp(&d570Out, &d570Exp, 4) == 0;
            if (!rOk) ++retBad;
            if (!cOk) ++candBad;
            if (!aOk) ++aiBad;
            if (!dOk) ++dBad;
            if (rOk && cOk && aOk && dOk) ++exact;
            else if (retBad + candBad + aiBad + dBad <= 20)
                std::fprintf(o, "MISMATCH %s n=%s v=%d cw=%x ret=%d/%d cand=%s/%s ai=%d d570=%s/%s\n", kind.c_str(), k[1].c_str(), v, cw,
                             ret, retExp, Hex(cand, 8).c_str(), k[12].c_str(), aOk ? 1 : 0, Hex(&d570Out, 4).c_str(), k[14].c_str());
            if (c == EOF) break;
        }
        std::fclose(f);
    }
    std::fprintf(o, "G-AILEAF-REPLAY rows=%ld exact=%ld ret_bad=%ld cand_bad=%ld ai_bad=%ld d570_bad=%ld parse_bad=%ld "
                    "rows_570=%ld rows_880=%ld rows_d570=%ld ret1_570=%ld ret1_880=%ld ret1_d570=%ld dbg_calls=%ld\n",
                 rows, exact, retBad, candBad, aiBad, dBad, parseBad, byKind[0], byKind[1], byKind[2],
                 byRet1[0], byRet1[1], byRet1[2], s_dbg);
    std::fclose(o);
    if (rowsOut) std::fclose(rowsOut);
}

}  // namespace Ai
