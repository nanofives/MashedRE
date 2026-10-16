// Mashed RE — AI course-0x21 track-object seek (FUN_00414f00): ONE body for both build targets (seek414f00, 2026-10-16).
//
// hooks.csv called this "powerup-seek targeting". The bytes show what it reads (verify/seek414f00_20261016, PREREG section 0):
//   0x00684de0 = obj+0xc8 and 0x00684dac = obj+0x94 of the Warzone track object at 0x00684d18, whose state machine
//   FUN_00451d80 (one caller, 0x00452ba2 in the course-0x21 per-track tick 0x00452b20, record slot 0x005f3578) moves the
//   object's matrix row (measured: z 58.45 -> 64.62 in states 2/3, captures O2/O3) and writes DAT_0088ff50[v] = FUN_00451990(obj, car v matrix), an XZ box test (FUN_00451ad0, 0x00451b47 = 1,
//   0x00451b5b = 0). This function READS those words; it is not their producer (the dualcopy3 note had it inverted).
//
// What the original does (listing 0x00414f00..0x00415011, disasm_414f00.txt):
//   0x00414f03  fld [0x00684de0]; fcomp [0x005cc32c] (0.5); test ah,5; jnp -> return 0     (h < 0.5 -> 0; NaN passes)
//   0x00414f15  fld [0x00684de0]; fcomp [0x005cc72c] (30.0); test ah,0x41; jne -> go on   (h > 30.0 -> 0; NaN passes)
//   0x00414f34  FUN_00452ea0(v) = [0x0088ff50 + v*4]; == 0 -> return 0
//   0x00414f44  esi = FUN_00452160() = [0x00684dac] + 0x30    (the object matrix's translation row)
//   0x00414f51  FUN_0046d6d0(&l14, v), 0x00414f5c FUN_0046d570(&l10, v): results never read (U-7574). Both write only their
//               out-param, a stack local here (0x0046d6ec `mov [ecx],eax`; 0x0046d580 / 0x0046d647 `[edi]`, and FUN_0046d570's
//               normalize writes its own [esp+0xc], 0x0046d5d6). Dead artifacts, elided in both targets.
//   0x00414f67  FUN_0046d4a0(&rec, v): rec = 0x00881ec8 + v*0xd04 + [0x00881f48 + v*0xd04]*0x40 (v < 0x10 only, 0x0046d4a4)
//   0x00414f70  d = (esi[0] - rec[0x30], esi[1] - rec[0x34], esi[2] - rec[0x38]), each FSTP dword
//   0x00414f92  m = FUN_004c3ac0(&d)    (Vec3Magnitude: (x*x + y*y) + z*z FSTP dword, then the RW sqrt LUT)
//   0x00414f97  fcom [0x005cc348] (1.5); test ah,0x41; jne -> 0x00414fc2.  m > 1.5: cand = (esi[0], esi[2]) raw, return 1
//   0x00414fc2  fcomp [0x005cc564] (0.25); test ah,0x41; jne -> return 0.  m > 0.25 and FUN_0046c7b0(0) != 0:
//               cand = (car 0 rec[0x30], rec[0x38]) raw, return 1;  else return 0
// param_1 (spline) and param_2 (look) are never read (the body touches [esp+0x2c]/[esp+0x30]/[esp+0x34] = cand and v only).
// For v >= 0x10 FUN_0046d4a0 returns without writing rec (0x0046d4a7), so the original would read an unwritten stack slot;
// both callers (0x00416478, 0x00417fc8) pass the AI step's vehicle index.
//
// Thunks per target (the AiPostStepBrake.cpp shape):
//   Rec(v)    .asi: the original FUN_0046d4a0.  exe: the record mirror's +0x928 ring row [+0x9a8] (VehiclePhysics_Record*,
//             published for both ring halves since 2026-10-06), the same address arithmetic.
//   Mag(d)    .asi: the original FUN_004c3ac0.  exe: Math::LutSqrt((x*x + y*y) + z*z) in float, the PC_24 form (the race
//             loop runs at PC_24, memory track-load-runs-at-pc24) over the rebuilt LUT (Math/RwLutTables.cpp, 4096/4096).
//   Alive0()  .asi: the original FUN_0046c7b0(0).  exe: Ai_HostCarAlive(0), the bound Host::car_alive.
//   the four thresholds: the original's .rdata in the .asi, literals read from the unpatched image in the exe (whose .rdata pad
//   reads zero): 0x005cc32c = 0x3f000000 = 0.5, 0x005cc72c = 0x41f00000 = 30.0, 0x005cc348 = 0x3fc00000 = 1.5,
//   0x005cc564 = 0x3e800000 = 0.25.
// The three one-line leaf reads (0x00684de0, 0x0088ff50[v], 0x00684dac) are direct reads in both targets. The exe has no writer
// of any of them yet (the track object at 0x00684d18 is not ported), so there gate 1 fails on 0.0 and the body returns 0.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched; prologue 0x00414f00: 83 ec 18 e8 a8 df 03 00)

#include "../Core/HookSystem.h"
#include "AiState.h"
#include "AiStandalone.h"
#include "AiTrackObjSeek.h"
#ifdef MASHED_STANDALONE
#include "../Math/RwLutTables.h"
#include "../Vehicle/VehiclePhysicsRun.h"
#include <cstdio>
#include <cstdlib>
#endif

#include <cstdint>
#include <cstring>

using namespace Ai;

namespace {

inline float Fbits(std::uint32_t u) { float f; std::memcpy(&f, &u, 4); return f; }

#ifdef MASHED_STANDALONE
// FUN_0046d4a0 + the +0x30/+0x34/+0x38 reads: the record mirror's ring row, raw dwords.
inline void RecPos(int v, std::uint32_t out[3]) {
    namespace VP = mashed_re::Vehicle;
    const int t = VP::VehiclePhysics_RecordI32(v, 0x9a8);              // [0x00881f48 + v*0xd04] = record +0x9a8
    const int row = 0x928 + t * 0x40;                                  // 0x00881ec8 = record +0x928, 0x0046d4b8 shl 6
    out[0] = static_cast<std::uint32_t>(VP::VehiclePhysics_RecordI32(v, row + 0x30));
    out[1] = static_cast<std::uint32_t>(VP::VehiclePhysics_RecordI32(v, row + 0x34));
    out[2] = static_cast<std::uint32_t>(VP::VehiclePhysics_RecordI32(v, row + 0x38));
}
inline float Mag(const float d[3]) { return mashed_re::Math::LutSqrt((d[0] * d[0] + d[1] * d[1]) + d[2] * d[2]); }   // FUN_004c3ac0
inline int   Alive0()              { return Ai_HostCarAlive(0); }                                         // FUN_0046c7b0(0)
inline float GateLo()              { return 0.5f; }                                                       // _DAT_005cc32c
inline float GateHi()              { return 30.0f; }                                                      // _DAT_005cc72c
inline float FarGate()             { return 1.5f; }                                                       // _DAT_005cc348
inline float NearGate()            { return 0.25f; }                                                      // _DAT_005cc564
#else
typedef void  (__cdecl* fn_rec_t)(std::uintptr_t*, int);
typedef float (__cdecl* fn_mag_t)(const float*);
typedef int   (__cdecl* fn_alive_t)(int);
inline void RecPos(int v, std::uint32_t out[3]) {
    std::uintptr_t rec = 0;
    reinterpret_cast<fn_rec_t>(0x0046d4a0)(&rec, v);                   // FUN_0046d4a0
    std::memcpy(out, reinterpret_cast<const void*>(rec + 0x30u), 12);
}
inline float Mag(const float d[3]) { return reinterpret_cast<fn_mag_t>(0x004c3ac0)(d); }                  // FUN_004c3ac0
inline int   Alive0()              { return reinterpret_cast<fn_alive_t>(0x0046c7b0)(0); }               // FUN_0046c7b0(0)
inline float GateLo()              { return F32(0x005cc32cu); }
inline float GateHi()              { return F32(0x005cc72cu); }
inline float FarGate()             { return F32(0x005cc348u); }
inline float NearGate()            { return F32(0x005cc564u); }
#endif

} // namespace

// ===========================================================================
// 0x00414f00  AiTrackObjSeek(spline, look, cand, vehicle) -> 0 / 1
// ===========================================================================
extern "C" __declspec(dllexport)
int __cdecl AiTrackObjSeek(void* /*param_1*/, void* /*param_2*/, std::uint32_t* param_3, int param_4)
{
    const float held = F32(0x00684de0u);                                   // FUN_00452eb0 @0x00414f03
    if (held < GateLo()) return 0;                                          // fcomp _DAT_005cc32c, jnp @0x00414f13
    if (F32(0x00684de0u) > GateHi()) return 0;                              // FUN_00452eb0 @0x00414f15, fcomp _DAT_005cc72c
    if (I32(0x0088ff50u + static_cast<std::uintptr_t>(param_4) * 4u) == 0) return 0;   // FUN_00452ea0 @0x00414f34

    const std::uint32_t* tgt = reinterpret_cast<const std::uint32_t*>(
        static_cast<std::uintptr_t>(static_cast<std::uint32_t>(I32(0x00684dacu))) + 0x30u);   // FUN_00452160 @0x00414f44
    std::uint32_t own[3];
    RecPos(param_4, own);                                                   // FUN_0046d4a0 @0x00414f67
    float d[3];
    d[0] = Fbits(tgt[0]) - Fbits(own[0]);                                   // 0x00414f70 fld [eax+0x30]; fsubr [esi]
    d[1] = Fbits(tgt[1]) - Fbits(own[1]);                                   // 0x00414f79 fld [esi+4]; fsub [eax+0x34]
    d[2] = Fbits(tgt[2]) - Fbits(own[2]);                                   // 0x00414f83 fld [eax+0x38]; fsubr [esi+8]
    const float m = Mag(d);                                                 // FUN_004c3ac0 @0x00414f92
    if (m > FarGate()) {                                                    // fcom _DAT_005cc348 @0x00414f97
        param_3[0] = tgt[0];                                                // 0x00414fa7..0x00414faf
        param_3[1] = tgt[2];                                                // 0x00414fb1..0x00414fb5
        return 1;
    }
    if (m > NearGate() && Alive0() != 0) {                                  // fcomp _DAT_005cc564 @0x00414fc2, FUN_0046c7b0(0)
        std::uint32_t car0[3];
        RecPos(0, car0);                                                    // FUN_0046d4a0(&rec, 0) @0x00414fe4
        param_3[0] = car0[0];                                               // 0x00414fed / 0x00414ff7
        param_3[1] = car0[2];                                               // 0x00414ff9 / 0x00414ffd
        return 1;
    }
    return 0;
}
RH_ScopedInstall(AiTrackObjSeek, 0x00414f00);

#ifdef MASHED_STANDALONE
// ===========================================================================
// exe call-site wrapper for the LAB_00416462 / 0x00417fc8 mode-10 blocks (Ai/AiStandalone.cpp). Default ON; MASHED_NO_SEEK414F00=1
// skips the call (the pre-seek414f00 behaviour). MASHED_SEEK_TRACE=<abs path> appends one line per 600 calls and one per ret=1, the
// knob-took witness: calls > 0 on course 0x21 with the knob on, none with it off.
// ===========================================================================
namespace {
long s_seekCalls = 0, s_seekGate1 = 0, s_seekRet1 = 0;
std::FILE* SeekTrace() {
    static bool s_tried = false;
    static std::FILE* s_f = nullptr;
    if (!s_tried) {
        s_tried = true;
        if (const char* p = std::getenv("MASHED_SEEK_TRACE")) s_f = std::fopen(p, "a");
    }
    return s_f;
}
} // namespace

namespace Ai {

bool Seek414f00_Enabled() {
    static const bool s_on = (std::getenv("MASHED_NO_SEEK414F00") == nullptr);
    return s_on;
}

int AiTrackObjSeek_Exe(int v, float cand[2]) {
    std::uint32_t c[2];
    std::memcpy(c, cand, 8);
    const int r = AiTrackObjSeek(nullptr, nullptr, c, v);
    std::memcpy(cand, c, 8);
    ++s_seekCalls;
    const float h = F32(0x00684de0u);
    if (!(h < 0.5f) && !(h > 30.0f)) ++s_seekGate1;
    if (r) ++s_seekRet1;
    if (std::FILE* f = SeekTrace()) {
        if (r || s_seekCalls % 600 == 1) {
            std::fprintf(f, "seek414f00 v=%d ret=%d calls=%ld gate1=%ld ret1=%ld held=%08x ff50v=%08x\n", v, r, s_seekCalls,
                         s_seekGate1, s_seekRet1, static_cast<unsigned>(I32(0x00684de0u)),
                         static_cast<unsigned>(I32(0x0088ff50u + static_cast<std::uintptr_t>(v) * 4u)));
            std::fflush(f);
        }
    }
    return r;
}

} // namespace Ai
#endif
