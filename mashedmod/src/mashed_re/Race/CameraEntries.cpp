// Mashed RE — D-11073 leg 2: the race camera's entry objects (phase 3, the camera hold).
// Pre-registration: verify/d3_gatefire_20261008/PREREG_LEG2.md.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Listed in BOTH exe_sources.rsp and asi_sources.rsp, and in x87_tus.txt (/arch:IA32): these are
// instruction-order transcriptions of x87 streams. One body per RVA:
//   0x00441b30  CamEntryBuild441b30       (329 B)  one entry from one table row
//   0x00441c80  CamCarMidpoint441c80      (179 B)  midpoint of the most-separated car pair
//   0x00442600  CamEntriesInit442600      (349 B)  per-track entry table -> entry array, once
//   0x00445aa0  CamEntryType0Step445aa0  (2579 B)  type-0 entry step; site (a) releases phase 3
// Listings: verify/d3_gatefire_20261008/leg2_445aa0.asm and leg2_bytes.py (capstone over the
// unpatched image), leg2_decomp.txt (decomp_pc.py). Where the decomp and the bytes disagree the bytes
// win: the decomp read the degrees factor `fmul qword [0x005ccae0]` (a DOUBLE, 57.295799255371094)
// as the float -2.0, and hid every `_ftol2` operand.
//
// TWO ARMS, one body. The .asi calls every callee the original calls, at its RVA. Under
// /DMASHED_STANDALONE (exe only):
//   - constants are literals (the exe maps MASHED's data range zero-filled, exe_main.cpp:7617, so
//     .rdata / file-backed .data read 0 there); each literal cites its address and bits;
//   - FUN_004c3bf0 / 004c3c60 / 004c3ac0 / 004c3b30 / 004c51a0 / 004c4d20 / 0046d4a0 -> the ported
//     bodies (Math\*, Collision\RwpSolverMath2.cpp, Vehicle\VehicleRecordPtr.cpp; all both targets);
//   - FUN_004430a0 (flag set, asi-only body) -> its one store, DAT_00897fe0 = v;
//   - FUN_00426c00 -> CamEntryHost.track_id (aib_track_index(); the exe's DAT_00644158 reads 0);
//   - FUN_0040e180 -> CamEntryHost.most_separated (RaceCamera::MostSeparatedPair over the cars);
//   - FUN_00472650 -> Ai::Ai_RandRange (the FUN_00472650 law on the verbatim FUN_00534870 ring);
//   - FUN_00405540 -> NOT CALLED. Named: it returns 0 when DAT_00639d78 == 0 (live 656/656,
//     RESULT_CADENCE.md), and the exe has no camera-path clip module, so DAT_00639d78 is 0 there;
//   - the 13 tables -> CameraEntryTables.h (generated from the image).
// FPU (MEASURED, run_diff 2026-10-09): the process does NOT run at 24-bit precision; a float-local
// transcription was 1 ulp off on +0x8c/+0x94/+0xc8. Rule used here: every value the bytes keep in ST
// is a `double`; only an `fst`/`fstp dword` site rounds to float. ST0-returning callees are declared
// `double` so their result is not rounded at the call. `fpatan`/`fsin` are the original instructions.

#include "CameraEntries.h"
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstdio>
#include <cstring>

#ifdef MASHED_STANDALONE
#include "CameraEntryTables.h"
#include "../Ai/AiStandalone.h"
extern "C" float __cdecl Vec2Length(const float* v);                         // 0x004c3bf0
extern "C" float __cdecl Vec2Normalize(float* out, const float* in);         // 0x004c3c60
extern "C" float __cdecl Vec3Magnitude(const float* v);                      // 0x004c3ac0
extern "C" float __cdecl FastSqrt(float x);                                  // 0x004c3b30
extern "C" void  __cdecl FUN_004c51a0(float* m, float* v, int mode);         // 0x004c51a0
extern "C" void* __cdecl RwMatrixRotate(void* m, const float* axis, float deg, int mode); // 0x004c4d20
extern "C" std::uint32_t __cdecl PtrCompute881ec8(std::uint32_t* out, std::uint32_t idx); // 0x0046d4a0
#endif

namespace {

inline std::int32_t& I32(std::uintptr_t a) { return *reinterpret_cast<std::int32_t*>(a); }
inline float&        Fe(std::uint8_t* e, std::uint32_t o) { return *reinterpret_cast<float*>(e + o); }
inline std::int32_t& Ie(std::uint8_t* e, std::uint32_t o) { return *reinterpret_cast<std::int32_t*>(e + o); }
inline float AsF(std::uint32_t b) { float f; std::memcpy(&f, &b, 4); return f; }

// Constants (address : bits). Read from the image, leg2_bytes.py f32, 2026-10-09.
const float k1_3000   = AsF(0x39aec33eu);   // 0x005cc948
const float k4        = AsF(0x40800000u);   // 0x005cc35c
const float kThird    = AsF(0x3eaaaaabu);   // 0x005ccac8
const float k0_05     = AsF(0x3d4ccccdu);   // 0x005cc9a0
const float k3        = AsF(0x40400000u);   // 0x005cc31c
const float k1        = AsF(0x3f800000u);   // 0x005cc320
const float kM1       = AsF(0xbf800000u);   // 0x005cc33c
const float k14       = AsF(0x41600000u);   // 0x005cd96c
const float k0_02     = AsF(0x3ca3d70au);   // 0x005ce18c  release distance
const float k6        = AsF(0x40c00000u);   // 0x005cd0a0
const float k0_75     = AsF(0x3f400000u);   // 0x005cc950
const float k0        = 0.0f;               // 0x005d757c (file-backed 0)
const float k5        = AsF(0x40a00000u);   // 0x005cc358
const float k0_2      = AsF(0x3e4ccccdu);   // 0x005cc9c0
const float k10       = AsF(0x41200000u);   // 0x005cc55c
const float k7        = AsF(0x40e00000u);   // 0x005cc9b8 (and the 0x40e00000 immediate at 0x00445c60)
const float k60       = AsF(0x42700000u);   // 0x005cc728
const float k1_7      = AsF(0x3e124925u);   // 0x005ce188
const float k8        = AsF(0x41000000u);   // 0x005cc9f4
const float k0_125    = AsF(0x3e000000u);   // 0x005cd050
const float kSin1     = AsF(0x3b490e56u);   // 0x005ce184
const float k0_25     = AsF(0x3e800000u);   // 0x005cc564
const float k0_5      = AsF(0x3f000000u);   // 0x005cc32c
const float k0_1      = AsF(0x3dcccccdu);   // 0x005cc56c
const float k15       = AsF(0x41700000u);   // 0x005cc9b0
const float k90       = AsF(0x42b40000u);   // 0x005ccad0
const float k270      = AsF(0x43870000u);   // 0x005cd324
const float k180      = AsF(0x43340000u);   // 0x005cd09c
const float k2        = AsF(0x40000000u);   // 0x005cc574
const float k360      = AsF(0x43b40000u);   // 0x005ccac4
const float k50_3     = AsF(0x41855555u);   // 0x005ce180
const float kSin2     = AsF(0x3bc90e56u);   // 0x005ce17c
const float k30       = AsF(0x41f00000u);   // 0x005cc72c
const double kDeg     = 57.295799255371094; // 0x005ccae0 (qword)
const std::uint32_t kAxisY[3] = { 0x00000000u, 0x3f800000u, 0x00000000u };   // 0x006146fc (0, 1, 0)

// atan(q) in the original's degrees form: fld1; fpatan; fmul qword [0x005ccae0]; fchs;
// fsubr dword [0x005cd09c]  ->  180 - (-(atan(q) * 57.2958)).
double AtanDeg180(double q) {
    double r;
    __asm {
        fld   q
        fld1
        fpatan
        fmul  kDeg
        fchs
        fsubr k180
        fstp  r
    }
    return r;
}
double Fsin(double x) {
    double r;
    __asm {
        fld  x
        fsin
        fstp r
    }
    return r;
}

// ---- callee bindings ---------------------------------------------------------------------
#ifdef MASHED_STANDALONE
CamEntryHost s_camHost = {};
inline double V2Len(const float* v)             { return Vec2Length(v); }
inline double V2Norm(float* o, const float* i)  { return Vec2Normalize(o, i); }
inline double V3Mag(const float* v)             { return Vec3Magnitude(v); }
inline double FSqrt(float x)                    { return FastSqrt(x); }
inline void  MatTranslate(float* m, float* v, int mode) { FUN_004c51a0(m, v, mode); }
inline void  MatRotate(void* m, const float* axis, float deg, int mode) { RwMatrixRotate(m, axis, deg, mode); }
inline void  FlagSet(int v)                     { I32(0x00897fe0u) = v; }          // FUN_004430a0 body
inline int   TrackId()                          { return s_camHost.track_id ? s_camHost.track_id() : 0; }
inline void  MostSep(int* a, int* b)            { if (s_camHost.most_separated) s_camHost.most_separated(a, b); }
inline std::uint32_t VehPtr(std::uint32_t* o, std::uint32_t i) { return PtrCompute881ec8(o, i); }
inline double RandRange(float lo, float hi)     { return ::Ai::Ai_RandRange(lo, hi); }
inline const float* AxisY()                     { return reinterpret_cast<const float*>(kAxisY); }
#else
inline double V2Len(const float* v)            { return reinterpret_cast<double(__cdecl*)(const float*)>(0x004c3bf0u)(v); }
inline double V2Norm(float* o, const float* i) { return reinterpret_cast<double(__cdecl*)(float*, const float*)>(0x004c3c60u)(o, i); }
inline double V3Mag(const float* v)            { return reinterpret_cast<double(__cdecl*)(const float*)>(0x004c3ac0u)(v); }
inline double FSqrt(float x)                   { return reinterpret_cast<double(__cdecl*)(float)>(0x004c3b30u)(x); }
inline void  MatTranslate(float* m, float* v, int mode) { reinterpret_cast<void(__cdecl*)(float*, float*, int)>(0x004c51a0u)(m, v, mode); }
inline void  MatRotate(void* m, const float* axis, float deg, int mode) { reinterpret_cast<void*(__cdecl*)(void*, const float*, float, int)>(0x004c4d20u)(m, axis, deg, mode); }
inline void  FlagSet(int v)                   { reinterpret_cast<void(__cdecl*)(int)>(0x004430a0u)(v); }
inline int   TrackId()                        { return reinterpret_cast<int(__cdecl*)()>(0x00426c00u)(); }
inline void  MostSep(int* a, int* b)          { reinterpret_cast<void(__cdecl*)(int*, int*)>(0x0040e180u)(a, b); }
inline std::uint32_t VehPtr(std::uint32_t* o, std::uint32_t i) { return reinterpret_cast<std::uint32_t(__cdecl*)(std::uint32_t*, std::uint32_t)>(0x0046d4a0u)(o, i); }
inline double RandRange(float lo, float hi)    { return reinterpret_cast<double(__cdecl*)(float, float)>(0x00472650u)(lo, hi); }
inline const float* AxisY()                   { return reinterpret_cast<const float*>(0x006146fcu); }
#endif

} // namespace

#ifdef MASHED_STANDALONE
void CamEntry_SetHost(const CamEntryHost& h) { s_camHost = h; }
#endif

// ===========================================================================
// 0x00441b30  CamEntryBuild441b30(entry, row). row = 12 dwords of a FUN_00442600 table.
// ===========================================================================
// 0x00441b30
extern "C" __declspec(dllexport) void __cdecl CamEntryBuild441b30(std::uint8_t* e, const float* row)
{
    float xyz[3] = { row[0], row[1], row[2] };                              // 0x00441b3a..0x00441b53
    const std::uint32_t r3 = reinterpret_cast<const std::uint32_t*>(row)[3];
    const std::uint32_t r4 = reinterpret_cast<const std::uint32_t*>(row)[4];
    const std::uint32_t r5 = reinterpret_cast<const std::uint32_t*>(row)[5];
    const std::uint32_t r6 = reinterpret_cast<const std::uint32_t*>(row)[6];
    const std::int32_t t7 = static_cast<std::int32_t>(row[7]);               // 0x00441b6e _ftol2
    const std::int32_t t8 = static_cast<std::int32_t>(row[8]);               // 0x00441b78 _ftol2
    const std::int32_t t9 = static_cast<std::int32_t>(row[9]);               // 0x00441b82 _ftol2
    const std::uint32_t r10 = reinterpret_cast<const std::uint32_t*>(row)[10];
    const std::uint32_t r11 = reinterpret_cast<const std::uint32_t*>(row)[11];
    Ie(e, 0x04) = t7;                                                        // 0x00441b91
    Ie(e, 0x08) = t8;                                                        // 0x00441bab
    Ie(e, 0x00) = 1;                                                         // 0x00441bbb
    Ie(e, 0x34) = 0x3f800000; Ie(e, 0x20) = 0x3f800000; Ie(e, 0x0c) = 0x3f800000;   // 0x00441bc1..c7
    Ie(e, 0x1c) = 0; Ie(e, 0x14) = 0; Ie(e, 0x10) = 0; Ie(e, 0x30) = 0;      // 0x00441bc9..d2
    Ie(e, 0x2c) = 0; Ie(e, 0x24) = 0; Ie(e, 0x44) = 0; Ie(e, 0x40) = 0; Ie(e, 0x3c) = 0;   // ..0x00441be1
    Ie(e, 0x18) = Ie(e, 0x18) | 0x20003;                                     // 0x00441be4
    MatTranslate(reinterpret_cast<float*>(e + 0xc), xyz, 0);                 // 0x00441be7
    Ie(e, 0xac) = static_cast<std::int32_t>(r3);                             // 0x00441bf8
    Ie(e, 0xb0) = static_cast<std::int32_t>(r4);                             // 0x00441c02
    Ie(e, 0x94) = 0; Ie(e, 0x90) = 0; Ie(e, 0x8c) = 0;                       // 0x00441c0f..1b
    Ie(e, 0xa0) = 0; Ie(e, 0x9c) = 0; Ie(e, 0x98) = 0;                       // 0x00441c21..2d
    Ie(e, 0xa4) = 0x41400000;                                                // 0x00441c33 12.0
    Ie(e, 0xb4) = static_cast<std::int32_t>(r5);                             // 0x00441c3d
    Ie(e, 0xbc) = 0; Ie(e, 0xb8) = 0;                                        // 0x00441c43 / 49
    Ie(e, 0xc0) = 0x3f800000;                                                // 0x00441c4f
    Ie(e, 0xc4) = static_cast<std::int32_t>(r6);                             // 0x00441c55
    Ie(e, 0xcc) = t9;                                                        // 0x00441c5b
    Ie(e, 0xd0) = static_cast<std::int32_t>(r10);                            // 0x00441c61
    Ie(e, 0xd4) = static_cast<std::int32_t>(r11);                            // 0x00441c6c
}
RH_ScopedInstall(CamEntryBuild441b30, 0x00441b30);

// ===========================================================================
// 0x00441c80  CamCarMidpoint441c80(out, cam). FUN_0040e180(&a, &b); if cam[7] != 0 both = cam[8];
// out = p_a - (p_a - p_b) * 0.5, p = FUN_0046d4a0(car) +0x30..+0x38.
// exe: a failed PtrCompute881ec8 (records not allocated) leaves out at 0 (named; never fails on
// the original for idx < 16).
// ===========================================================================
// 0x00441c80
extern "C" __declspec(dllexport) void __cdecl CamCarMidpoint441c80(float* out, const std::int32_t* cam)
{
    int a = 0, b = 0;
    MostSep(&a, &b);                                                         // 0x00441c8d
    if (cam[7] != 0) { a = cam[8]; b = cam[8]; }                             // 0x00441c96..0x00441ca7
    std::uint32_t p = 0;
#ifdef MASHED_STANDALONE
    if (!VehPtr(&p, static_cast<std::uint32_t>(a)) || !p) { out[0] = out[1] = out[2] = 0.f; return; }
#else
    VehPtr(&p, static_cast<std::uint32_t>(a));                               // 0x00441cb5
#endif
    const float* pa = reinterpret_cast<const float*>(p + 0x30u);
    const float ax = pa[0], ay = pa[1], az = pa[2];                          // 0x00441cbe..c4
#ifdef MASHED_STANDALONE
    if (!VehPtr(&p, static_cast<std::uint32_t>(b)) || !p) { out[0] = out[1] = out[2] = 0.f; return; }
#else
    VehPtr(&p, static_cast<std::uint32_t>(b));                               // 0x00441cdd
#endif
    const float* pb = reinterpret_cast<const float*>(p + 0x30u);
    const float dx = ax - pb[0], dy = ay - pb[1], dz = az - pb[2];           // 0x00441ce2..0x00441cf8
    out[0] = ax - dx * k0_5;                                                 // 0x00441d05..0f
    out[1] = ay - dy * k0_5;                                                 // 0x00441d11..1b
    out[2] = az - dz * k0_5;                                                 // 0x00441d22..2c
}
RH_ScopedInstall(CamCarMidpoint441c80, 0x00441c80);

// ===========================================================================
// 0x00442600  CamEntriesInit442600(). Once (guard DAT_0089898c). Table by track id; count =
// _ftol2(table[0]); per row FUN_00441b30; three first-index trackers by the row's type float;
// DAT_00897fc0 = _ftol2(FUN_00472650(0, 7.0)) << 8.
// ===========================================================================
namespace {
const float* TableFor(int track) {
#ifdef MASHED_STANDALONE
    using mashed_re::CamEntry::kTables;
    const float* def = nullptr;
    for (const auto& t : kTables) {
        if (t.track == track) return reinterpret_cast<const float*>(t.words);
        if (t.track == -1) def = reinterpret_cast<const float*>(t.words);
    }
    return def;                                                              // default arm 0x005f82a8
#else
    switch (track) {                                                         // 0x00442623..0x0044268a
    case 0x02: return reinterpret_cast<const float*>(0x005f91e0u);
    case 0x03: return reinterpret_cast<const float*>(0x005f7e48u);
    case 0x0b: return reinterpret_cast<const float*>(0x005f9368u);
    case 0x19: return reinterpret_cast<const float*>(0x005f8980u);
    case 0x1a: return reinterpret_cast<const float*>(0x005f8490u);
    case 0x1e: return reinterpret_cast<const float*>(0x005f8d50u);
    case 0x21: return reinterpret_cast<const float*>(0x005f7bd0u);
    case 0x22: return reinterpret_cast<const float*>(0x005f8798u);
    case 0x24: return reinterpret_cast<const float*>(0x005f8b68u);
    case 0x25: return reinterpret_cast<const float*>(0x005f8ea8u);
    case 0x26: return reinterpret_cast<const float*>(0x005f8030u);
    case 0x27: return reinterpret_cast<const float*>(0x005f7a48u);
    default:   return reinterpret_cast<const float*>(0x005f82a8u);
    }
#endif
}
}

// 0x00442600
extern "C" __declspec(dllexport) void __cdecl CamEntriesInit442600(void)
{
    if (I32(0x0089898cu) == 1) return;                                       // 0x00442600..0x0044260d
    I32(0x0089898cu) = 1;
    const int track = TrackId();                                             // 0x0044261b
    const float* tbl = TableFor(track);
#ifndef MASHED_STANDALONE
    reinterpret_cast<int(__cdecl*)(const float*)>(0x00405540u)(tbl);         // 0x00442690
#endif
    I32(0x008964a0u) = -1; I32(0x0089899cu) = -1; I32(0x00898998u) = -1;    // 0x0044269d..a7
    const std::int32_t n = static_cast<std::int32_t>(tbl[0]);                // 0x00442695 fld; 0x004426ac _ftol2
    I32(0x00898994u) = n;                                                    // 0x004426b7
    std::uint8_t* ent = reinterpret_cast<std::uint8_t*>(0x008964c0u);        // 0x004426c0
    const float* row = tbl + 1;                                              // esi + 0x20 - 0x1c
    for (std::int32_t k = 0; k < n; ++k, ent += 0xd8, row += 12) {           // 0x0044272e..0x0044273a
        CamEntryBuild441b30(ent, row);                                       // 0x004426cd
        const std::uint32_t ty = reinterpret_cast<const std::uint32_t*>(row)[7];
        if (ty == 0x3f800000u) {                                             // 0x004426d7
            if (I32(0x008964a0u) == -1) I32(0x008964a0u) = k;                // 0x004426e7..eb
            I32(0x0089649cu) = k;                                            // 0x004426f1
        }
        if (ty == 0x40000000u) {                                             // 0x004426fc
            if (I32(0x0089899cu) == -1) I32(0x0089899cu) = k;                // 0x00442704..0c
            I32(0x008964a4u) = k;                                            // 0x00442712
        }
        if (ty == 0x40400000u && I32(0x00898998u) == -1) I32(0x00898998u) = k;   // 0x00442718..28
    }
    const double r = RandRange(0.0f, k7);                                    // 0x00442744 FUN_00472650(0, 7.0)
    I32(0x00897fc0u) = static_cast<std::int32_t>(r) << 8;                    // 0x0044274c..0x00442756
#ifdef MASHED_STANDALONE
    if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
        std::fprintf(lf, "[leg2] FUN_00442600 track=%d count=%d types=", track, n);
        for (std::int32_t k = 0; k < n; ++k)
            std::fprintf(lf, "%d%s", Ie(reinterpret_cast<std::uint8_t*>(0x008964c0u) + k * 0xd8, 4), k + 1 < n ? "," : "");
        std::uint8_t* e0 = reinterpret_cast<std::uint8_t*>(0x008964c0u);
        std::fprintf(lf, " e0=(%.6g,%.6g,%.6g) a4=%.6g ac/b0/b4=%.6g/%.6g/%.6g draws_before=%u fc0=%d\n",
                     Fe(e0, 0x3c), Fe(e0, 0x40), Fe(e0, 0x44), Fe(e0, 0xa4), Fe(e0, 0xac), Fe(e0, 0xb0),
                     Fe(e0, 0xb4), ::Ai::Ai_RandDrawCount() - 1u, I32(0x00897fc0u));
        std::fclose(lf);
    }
#endif
}
RH_ScopedInstall(CamEntriesInit442600, 0x00442600);

// ===========================================================================
// 0x00445aa0  CamEntryType0Step445aa0(entry, cam). Transcribed from leg2_445aa0.asm in
// instruction order. Stack slots named by their [esp+N] at function entry depth (after the four
// pushes): dt [0x10], dist [0x18]/[0x64], k1 [0x20], t [0x48..0x50], dir [0x40..0x44].
// ===========================================================================
// 0x00445aa0
extern "C" __declspec(dllexport) void __cdecl CamEntryType0Step445aa0(std::uint8_t* e, std::int32_t* cam)
{
    // `f` suffix = a float stack slot ([esp+N] dword); `d` = a value the bytes keep in ST.
    const float dt = static_cast<float>(static_cast<double>(I32(0x007f1008u)) * k1_3000);   // 0x00445aa3..0x00445abd fstp [0x10]
    float t[3];                                                              // [0x48], [0x4c], [0x50]
    CamCarMidpoint441c80(t, cam);                                            // 0x00445ac1
    if (cam[0] != 0) {                                                       // 0x00445ad3
        std::memcpy(&t[0], &cam[1], 4);                                      // 0x00445ad5 raw dword
        std::memcpy(&t[2], &cam[3], 4);                                      // 0x00445adb raw dword
        float d2[2] = { static_cast<float>(static_cast<double>(Fe(e, 0x3c)) - t[0]),
                        static_cast<float>(static_cast<double>(Fe(e, 0x44)) - t[2]) };   // 0x00445ad8..0x00445afa
        double Ld = V2Len(d2);                                               // 0x00445afe (ST0)
        if (Ld > static_cast<double>(k4)) Ld = k4;                           // 0x00445b03..0x00445b15
        Ld = Ld * kThird;                                                    // 0x00445b1b
        Ie(e, 0xa4) = cam[2];                                                // 0x00445b28
        t[0] = static_cast<float>(static_cast<double>(t[0]) + Ld);           // 0x00445b24..0x00445b30
        t[2] = static_cast<float>(Ld + t[2]);                                // 0x00445b34..0x00445b38
    } else {
        Ie(e, 0xa4) = 0x41400000;                                            // 0x00445b3e 12.0
        if (TrackId() == 0x1a) Ie(e, 0xa4) = 0x41a00000;                     // 0x00445b48..0x00445b52 20.0
    }
    const double hd = static_cast<double>(Fe(e, 0x40)) - Fe(e, 0xa4);        // 0x00445b5c (ST)
    const float  hf = static_cast<float>(hd);                                // 0x00445b6b fst [0x64]
    if (hd < k0_05) Fe(e, 0x90) = static_cast<float>(static_cast<double>(dt) * k3 + Fe(e, 0x90));   // 0x00445b76..86
    if (hd > k0_05) Fe(e, 0x90) = static_cast<float>(static_cast<double>(Fe(e, 0x90)) - static_cast<double>(dt) * k3);  // 0x00445b99..a9
    const double k1d = static_cast<double>(k1) - dt;                         // 0x00445baf (ST)
    const float  k1f = static_cast<float>(k1d);                              // 0x00445bb9 fst [0x20]
    double kd = k1d;
    if (hf < k1 && hf > kM1) kd = static_cast<double>(k1) - (static_cast<double>(dt) + dt);   // 0x00445bbd..0x00445be7
    Fe(e, 0x90) = static_cast<float>(kd * Fe(e, 0x90));                      // 0x00445bed / 0x00445bfb
    float dv[2] = { static_cast<float>(static_cast<double>(Fe(e, 0x3c)) - t[0]),
                    static_cast<float>(static_cast<double>(Fe(e, 0x44)) - t[2]) };   // 0x00445c05..0x00445c1c
    float dir[2];
    V2Norm(dir, dv);                                                         // 0x00445c20 (ST0 popped)
    const float dist = static_cast<float>(V2Len(dv));                        // 0x00445c2c..0x00445c31 fstp
    float pd = dist;                                                         // [0x64]
    bool lab = (Ie(e, 0xa8) != 0);                                           // 0x00445c42
    if (!lab) {
        float p2f;                                                           // [0x68]
        double gd;                                                           // ST1
        if (cam[0] == 0) {                                                   // 0x00445c50
            p2f = k7;                                                        // 0x00445c60 0x40e00000
            if (dist < p2f) Ie(e, 0xa8) = 1;                                 // 0x00445c68..0x00445c73
            gd = static_cast<double>(dt) * k14;                              // 0x00445c7d..0x00445c81
        } else {
            p2f = 0.0f;                                                      // 0x00445c52
            if (dist < k0_02) FlagSet(0);                                    // 0x00445cda..0x00445cec site (a)
            for (std::uintptr_t c = 0x007f1042u; c < 0x007f12a2u; c += 0x4cu)   // 0x00445cf4..0x00445d16
                if (*reinterpret_cast<const std::uint8_t*>(c) != 0) FlagSet(0);  // site (b)
            gd = static_cast<double>(dt) * k6;                               // 0x00445d18..0x00445d1c
        }
        double f6d = static_cast<double>(dist) - p2f;                        // 0x00445c89 / 0x00445c8f
        if (cam[0] != 0 && f6d < k0_75) f6d = k0_75;                         // 0x00445c87 re-read, 0x00445c95..a4
        if (dist < k0) pd = 0.0f;                                            // 0x00445caa..0x00445cbb
        double f9d;
        if (f6d < k0) f9d = k0;                                              // 0x00445cc3..0x00445cd2
        else          f9d = (f6d > k5) ? static_cast<double>(k5) : f6d;      // 0x00445d27..0x00445d36
        double sd = gd * f9d;                                                // 0x00445d3c fmulp
        sd = sd * k0_2;                                                      // 0x00445d46
        Fe(e, 0x8c) = static_cast<float>(Fe(e, 0x8c) - dir[0] * sd);         // 0x00445d4c..0x00445d58
        Fe(e, 0x94) = static_cast<float>(Fe(e, 0x94) - dir[1] * sd);         // 0x00445d5e..0x00445d6a
        lab = (Ie(e, 0xa8) != 0);                                            // 0x00445d3e / 0x00445d72
    }
    if (lab) {                                                               // 0x00445d74
        if (pd > k10) Ie(e, 0xa8) = 0;                                       // 0x00445d74..0x00445d85
        if (cam[0] != 0) Ie(e, 0xa8) = 0;                                    // 0x00445d8b..0x00445d8f
        const double ad = static_cast<double>(dt) * k60;                     // 0x00445d95..0x00445d99
        double bd = static_cast<double>(k7) - pd;                            // 0x00445d9f / 0x00445da5
        if (bd < k0) bd = k0;                                                // 0x00445da9..0x00445db8
        double s2d = ad * bd;                                                // 0x00445dbe fmulp
        s2d = s2d * k1_7;                                                    // 0x00445dc0
        Fe(e, 0x8c) = static_cast<float>(dir[0] * s2d + Fe(e, 0x8c));        // 0x00445dc6..0x00445dd2
        Fe(e, 0x94) = static_cast<float>(dir[1] * s2d + Fe(e, 0x94));        // 0x00445dd8..0x00445de4
    }
    double kkd = k1f;                                                        // 0x00445dec fld [0x20] (the FLOAT copy)
    if (pd < k10 && Ie(e, 0xa8) == 0) kkd = static_cast<double>(k1) - (static_cast<double>(dt) + dt);   // 0x00445df0..0x00445e11
    Fe(e, 0x8c) = static_cast<float>(kkd * Fe(e, 0x8c));                     // 0x00445e17..0x00445e21
    Fe(e, 0x94) = static_cast<float>(kkd * Fe(e, 0x94));                     // 0x00445e27..0x00445e2d
    float mv[3] = { static_cast<float>(static_cast<double>(Fe(e, 0x8c)) * dt),
                    static_cast<float>(static_cast<double>(Fe(e, 0x90)) * dt),
                    static_cast<float>(static_cast<double>(Fe(e, 0x94)) * dt) };   // 0x00445e33..0x00445e7a
    MatTranslate(reinterpret_cast<float*>(e + 0xc), mv, 2);                  // 0x00445e7e
    if (cam[0] != 0) {                                                       // 0x00445e83 re-read
        CamCarMidpoint441c80(t, cam);                                        // 0x00445e96
        if (dist < k8) {                                                     // 0x00445e9b..0x00445ead
            const float* cf = reinterpret_cast<const float*>(cam);
            const double d0d = static_cast<double>(t[0]) - cf[4];            // 0x00445eaf..b2 (ST)
            const double d1d = static_cast<double>(t[1]) - cf[5];            // 0x00445eb6..b9 (ST)
            const float  d2f = static_cast<float>(static_cast<double>(t[2]) - cf[6]);   // 0x00445ebd..c4 fstp [0x5c]
            const float  wf  = static_cast<float>((static_cast<double>(k8) - dist) * k0_125);   // 0x00445ec8..d8 fstp [0x64]
            const float  m0f = static_cast<float>(d0d * wf);                 // 0x00445ede..e2 fstp [0x54]
            const double m1d = d1d * wf;                                     // 0x00445ee6 (ST)
            const double m2d = static_cast<double>(d2f) * wf;                // 0x00445eea..ee (ST)
            t[0] = static_cast<float>(static_cast<double>(t[0]) - m0f);      // 0x00445ef2..fa
            t[1] = static_cast<float>(static_cast<double>(t[1]) - m1d);      // 0x00445efe..0x00445f04
            t[2] = static_cast<float>(static_cast<double>(t[2]) - m2d);      // 0x00445f08..0x00445f0e
        }
    }
    // ---- orientation (0x00445f16..0x004464a3); no release-path reader ----
    const double exd = static_cast<double>(t[0]) - Fe(e, 0x3c);              // 0x00445f16..1f (ST)
    const float  exf = static_cast<float>(exd);                              // 0x00445f30 fst [0x24]
    const int idx1 = (I32(0x007f101cu) + static_cast<int>(0xfffffe00u)) & 0x3ff;   // 0x00445f1a..0x00445f27
    const float eyf = static_cast<float>(static_cast<double>(t[1]) - Fe(e, 0x40));   // 0x00445f34..3b fstp [0x28]
    const float ezf = static_cast<float>(static_cast<double>(t[2]) - Fe(e, 0x44));   // 0x00445f3f..46 fstp [0x68]
    double snd = Fsin(static_cast<double>(idx1) * kSin1);                    // 0x00445f4a..0x00445f54
    if (snd < k0) snd = -snd;                                                // 0x00445f56..0x00445f63
    if (snd < k0_2) snd = static_cast<double>(k0_2) - (static_cast<double>(k0_2) - snd) * k0_25;   // 0x00445f65..0x00445f7e
    Fe(e, 0xc0) = static_cast<float>(snd * k0_5 + k0_1);                     // 0x00445f84..0x00445f90
    if (cam[0] != 0) Ie(e, 0xc0) = 0x3f19999a;                               // 0x00445f96..0x00445f9c
    const double limd = static_cast<double>(Fe(e, 0xc0)) * k15;              // 0x00445fa6..0x00445fac (ST)
    float lim = static_cast<float>(limd);                                    // 0x00445fb2 fst [0x14]
    if (limd > k5) lim = k5;                                                 // 0x00445fb6..0x00445fc3
    const std::int32_t c0 = cam[0];                                          // 0x00445fcb ecx
    if (c0 != 0 && dist < k4) lim = dist;                                    // 0x00445fcd..0x00445fe6
    double yawd;
    if (ezf != k0) {                                                         // 0x00445fea..0x00445ff9
        yawd = AtanDeg180(exd / ezf);                                        // 0x00445ffb..0x0044600b (exd still in ST)
        if (ezf > k0) yawd = yawd + k180;                                    // 0x00446011..0x00446022
    } else {
        yawd = (exd < k0) ? k270 : k90;                                      // 0x0044602a..0x0044603f
    }
    const float g2f = static_cast<float>(static_cast<double>(dt) * k5);      // 0x00446047..0x00446055 fst [0x30], fstp [0x1c]
    float gyf = g2f;                                                         // [0x1c]
    if (c0 != 0 && dist > k2) gyf = static_cast<float>(static_cast<double>(dt) * k10);   // 0x00446059..0x00446076
    double ywd = static_cast<double>(Fe(e, 0xb0)) - yawd;                    // 0x0044607a
    while (ywd >= k360) ywd = ywd - k360;                                    // 0x00446080..0x004460a1
    while (ywd < k0) ywd = ywd + k360;                                       // 0x004460a3..0x004460c1
    if (ywd > k180) ywd = -(static_cast<double>(k360) - ywd);                // 0x004460c3..0x004460d6
    if (ywd > lim) {                                                         // 0x004460d8..0x004460e1
        Fe(e, 0xbc) = static_cast<float>(static_cast<double>(Fe(e, 0xbc)) - gyf);   // 0x004460e3..0x004460e9
    } else {
        if (ywd < k0) ywd = -ywd;                                            // 0x004460ef..0x004460fc
        if (ywd > lim) Fe(e, 0xbc) = static_cast<float>(static_cast<double>(gyf) + Fe(e, 0xbc));   // 0x004460fe..0x0044610d
        else           Fe(e, 0xbc) = static_cast<float>(static_cast<double>(Fe(e, 0xbc)) * k0_5);   // 0x00446115..0x0044611b
    }
    const double qd = ywd / lim;                                             // 0x00446127 (ST)
    const float  qf = static_cast<float>(qd);                                // 0x0044612b fst [0x64]
    float rrf;
    if (qd < k4) rrf = AsF(0x42c80000u);                                     // 0x0044612f..0x0044613c 100.0
    else if (qf < k10) rrf = static_cast<float>((static_cast<double>(k10) - qf) * k50_3);   // 0x00446146..0x00446167
    else rrf = 0.0f;                                                         // 0x0044616d
    float v3[3] = { static_cast<float>(static_cast<double>(Fe(e, 0x3c)) - t[0]),
                    static_cast<float>(static_cast<double>(Fe(e, 0x40)) - t[1]),
                    static_cast<float>(static_cast<double>(Fe(e, 0x44)) - t[2]) };   // 0x00446175..0x00446197
    double md = V3Mag(v3) - Fe(e, 0xa4);                                     // 0x0044619b / 0x004461a0 (ST)
    if (md < k0) md = -md;                                                   // 0x004461a9..0x004461b6
    double w2d = static_cast<double>(rrf) - (md + md);                       // 0x004461b8..0x004461bc
    if (w2d < k0) w2d = k0;                                                  // 0x004461c0..0x004461cf
    Fe(e, 0xc8) = static_cast<float>(w2d);                                   // 0x004461d9
    Fe(e, 0xb0) = static_cast<float>(static_cast<double>(Fe(e, 0xbc)) + Fe(e, 0xb0));   // 0x004461df..0x004461ef
    const float lim2 = static_cast<float>(static_cast<double>(lim) + lim);   // 0x004461f5..0x004461fb fstp [0x2c]
    const double kbd = (md < lim2) ? (static_cast<double>(k1) - static_cast<double>(dt) * k4) : static_cast<double>(k1f);   // 0x004461ff..0x0044621c
    Fe(e, 0xbc) = static_cast<float>(kbd * Fe(e, 0xbc));                     // 0x00446220
    const double s2d2 = static_cast<double>(ezf) * ezf + static_cast<double>(exf) * exf;   // 0x0044622c..0x0044623c (ST)
    const float  s2f  = static_cast<float>(s2d2);                            // 0x0044623e fst [0x68]
    double pitchd = 0.0;
    bool special = (s2d2 == k0);                                             // 0x00446242..0x0044624d
    if (!special) {
        const double r2d = FSqrt(s2f);                                       // 0x0044624f..0x00446254 (dword arg)
        const float  r2f = static_cast<float>(r2d);                          // 0x00446259 fst [0x68]
        if (r2d == k0) special = true;                                       // 0x0044625d..0x0044626b
        else {
            pitchd = AtanDeg180(static_cast<double>(eyf) / r2f);             // 0x0044626d..0x00446281
            if (r2f > k0) pitchd = pitchd + k180;                            // 0x00446287..0x00446298
        }
    }
    if (special) pitchd = (eyf < k0) ? k270 : k90;                           // 0x004462a0..0x004462b9
    double pwd = static_cast<double>(Fe(e, 0xac)) - (static_cast<double>(k360) - pitchd);   // 0x004462bf / 0x004462cd
    while (pwd >= k360) pwd = pwd - k360;                                    // 0x004462d3..0x004462f1
    while (pwd < k0) pwd = pwd + k360;                                       // 0x004462f3..0x00446311
    if (pwd > k180) pwd = -(static_cast<double>(k360) - pwd);                // 0x00446313..0x00446326
    if (pwd > lim) {                                                         // 0x00446328..0x00446331
        Fe(e, 0xb8) = static_cast<float>(static_cast<double>(Fe(e, 0xb8)) - g2f);   // 0x00446333..0x00446339 ([0x1c] = g2)
    } else if (pwd < -static_cast<double>(lim)) {                            // 0x0044633f..0x0044634e
        Fe(e, 0xb8) = static_cast<float>(static_cast<double>(g2f) + Fe(e, 0xb8));   // 0x00446350..0x0044635a
        pwd = -pwd;                                                          // 0x00446360
    } else {
        Fe(e, 0xb8) = static_cast<float>(static_cast<double>(Fe(e, 0xb8)) * k0_5);  // 0x00446364..0x0044636a
    }
    Fe(e, 0xac) = static_cast<float>(static_cast<double>(Fe(e, 0xb8)) + Fe(e, 0xac));   // 0x00446376..0x0044638a
    const double kcd = (pwd < lim2) ? (static_cast<double>(k1) - static_cast<double>(dt) * k4) : static_cast<double>(k1f);   // 0x00446390..0x004463ad
    Fe(e, 0xb8) = static_cast<float>(kcd * Fe(e, 0xb8));                     // 0x004463b1
    const int idx2 = I32(0x007f101cu) & 0x3ff;                               // 0x004463bd..0x004463c3
    double rsd = Fsin(static_cast<double>(idx2) * kSin2) * k30;              // 0x004463cd..0x004463dd
    if (cam[0] != 0 && dist < k4) {                                          // 0x004463d1..0x004463f4
        double xd = static_cast<double>(dist) - k1;                          // 0x004463f6..0x004463fa
        if (xd < k0) xd = k0;                                                // 0x00446400..0x0044640f
        rsd = rsd * xd;                                                      // 0x00446417
        rsd = rsd * kThird;                                                  // 0x00446419
    }
    Fe(e, 0xb4) = static_cast<float>(rsd);                                   // 0x00446424
    Ie(e, 0x74) = 0x3f800000; Ie(e, 0x60) = 0x3f800000; Ie(e, 0x4c) = 0x3f800000;   // 0x00446432..38
    Ie(e, 0x5c) = 0; Ie(e, 0x54) = 0; Ie(e, 0x50) = 0; Ie(e, 0x70) = 0;      // 0x00446452..5b
    Ie(e, 0x6c) = 0; Ie(e, 0x64) = 0; Ie(e, 0x84) = 0; Ie(e, 0x80) = 0; Ie(e, 0x7c) = 0;   // ..0x00446470
    Ie(e, 0x58) = Ie(e, 0x58) | 0x20003;                                     // 0x00446443 / 0x00446473
    MatRotate(e + 0x4c, AxisY(), Fe(e, 0xb0), 2);                            // 0x00446476
    MatRotate(e + 0x4c, reinterpret_cast<const float*>(e + 0x4c), Fe(e, 0xac), 2);   // 0x00446486
    MatRotate(e + 0x4c, reinterpret_cast<const float*>(e + 0x6c), Fe(e, 0xb4), 2);   // 0x00446496
    MatTranslate(reinterpret_cast<float*>(e + 0x4c), reinterpret_cast<float*>(e + 0x3c), 2);   // 0x004464a3
}
RH_ScopedInstall(CamEntryType0Step445aa0, 0x00445aa0);
