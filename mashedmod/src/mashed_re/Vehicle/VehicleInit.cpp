// Mashed RE — WS-A3: per-vehicle suspension/spring/mass init FUN_0046b540.
//
// STATUS: verbatim port, PENDING diff-original C4 (x87 float10 sqrt loops use double
// intermediates; track-type override table beyond key 0 not yet fully harvested — U-A3-TABLE).
//
// Initializes one car's 0xd04 record (base g_vehicleArrayBase = DAT_008815a0, stride 0xd04):
// fixed wheel geometry + spring/mass, the track-type-keyed handling overrides, three
// FastSqrt radius loops, and the mass redistribution. Returns 0 if slot>=16 else 1.
// Field byte offsets are (absolute DAT_00881xxx - 0x008815a0); every constant memory_read
// (Ghidra pool11, read_only, 2026-06-16). Decomp: FUN_0046b540. Struct map: vehicle.md §4.1.
//
// Anchored to MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
//
// [dcphys 2026-10-21] ONE body for 0x0046b540 in both targets (see the A3* thunks above VehicleInit). The contact
// hull builder 0x0046b1c0 that used to live in this TU moved to Vehicle/VehicleContactHull.cpp (exe only), because
// that RVA stays split by target (U-9155) and this TU is now also in the .asi.
#include "ForceIntegrator.h"
#include "../Math/RwLutTables.h"   // [d2settle U-9181] Math::LutSqrt = 0x004c3b30
#include <cstdint>
#include <cstddef>   // std::ptrdiff_t (exe build's stricter include set surfaces this)
#include <cstring>
#include <cmath>
#include <cstdlib>   // [U-9147] std::getenv / std::atoi for MASHED_HANDLING_TYPE
#include <cstdio>    // [d2settle] MASHED_A3_LOG

namespace mashed_re {
namespace Vehicle {

extern int* g_vehicleArrayBase;   // DAT_008815a0 (16 × 0xd04 records)

// FastSqrt FUN_004c3b30 (C4) — the init uses it for radii.
// [d2settle U-9181 2026-10-16] CORRECTED. This said "std::sqrt is bit-equivalent ...
// (FastSqrt is an x87 fsqrt wrapper)". It is not: 0x004c3b30 is RW's 4096-entry LUT sqrt
// (Math/RwLutTables.cpp, tables verified 4096/4096 against the live original). Measured on
// the original's own A3 inputs (o_p0a.msd frame 1, verify/d2settle_20261016/PREREG §0.2):
// the LUT reproduces 24/24 of this function's sqrt outputs bit-exactly, std::sqrt 0/24.
// The visible symptom was U-9181: +0x4a4 read 0x3f2d9196 (0.678003) on the exe against
// the original's 0x3f2d93d1 (0.678037). `MASHED_NO_A3_LUT=1` restores std::sqrt and the
// pre-fix operand order (A/B only).
static bool A3LutOn() {
    static const bool s_on = (std::getenv("MASHED_NO_A3_LUT") == nullptr);
    return s_on;
}
static inline float FSqrt(float x) { return A3LutOn() ? Math::LutSqrt(x) : std::sqrt(x); }

// raw-bitpattern / typed writers into the record
static inline void WB(char* r, int off, std::uint32_t bits) { std::memcpy(r + off, &bits, 4); }
static inline void WF(char* r, int off, float v)            { std::memcpy(r + off, &v, 4); }
static inline void WI(char* r, int off, std::int32_t v)     { std::memcpy(r + off, &v, 4); }
static inline float RF(char* r, int off)                    { float v; std::memcpy(&v, r + off, 4); return v; }

// Handling-override table @0x00613140: 5-int entries { tag, paramA, spring, k1, k2 }, ended by tag -1. The original
// walks it with NO break, so the last matching tag wins (0x0046b58a..0x0046b5dc):
//     tag = [0x00613140]; if (tag != -1) { e = 0x00613148;
//       do { if (tag == type) { _DAT_00613108 = (float)e[-1];          // fild/fstp 0x0046b5a4
//                               _DAT_00613114 = (float)e[0];           //           0x0046b5ad
//                               _DAT_00613130 = (float)e[1] * 0.001;   // fmul _DAT_005cc558 0x0046b5b8
//                               _DAT_0061313c = (float)e[2] * 0.001; } //           0x0046b5c7
//            tag = e[3]; e += 5; } while (tag != -1); }
//
// [dcphys 2026-10-21] READ FROM THE IMAGE, all of it. The table is initialized .data (raw offset 0x213140 in
// original/MASHED.exe.unpatched), 13 entries, terminator at 0x00613244, and its only references are A3's two
// reads (0x0046b58c, 0x0046b599), so it never changes at runtime. The live --peek harvest it replaces (U-9147)
// covered tags 0..18 and assumed spring 40000 for tag 18; the image says 42000. Only the type the selector returns
// is consumed, and the standalone's default type is 6, so the extra rows change nothing on the default path.
struct HandlingOverride { int key, paramA, spring, k1, k2; };
static const HandlingOverride kHandlingTable[] = {
    {  0, 100, 40000, 1000, 1500 },   // 0x00613140
    {  6, 105, 40000, 1000, 1500 },   // 0x00613154   <- the reference scenario's type
    { 12,  95, 40000, 1000, 1500 },   // 0x00613168
    { 18, 100, 42000, 1000, 1500 },   // 0x0061317c
    { 24, 100, 38000, 1000, 1500 },   // 0x00613190
    { 30, 100, 40000, 1100, 1500 },   // 0x006131a4 (entry i at 0x00613140 + i*0x14)
    { 36, 100, 40000,  900, 1500 },
    { 42, 100, 40000, 1000, 1600 },
    { 48, 100, 40000, 1000, 1400 },
    { 54, 100, 45000, 1000, 1300 },
    { 60, 100, 40000, 1000, 1400 },
    { 66, 100, 45000, 1000, 1400 },
    { 72, 100, 40000, 1000, 1500 },   // 0x00613230
    { -1,   0,     0,    0,    0 },   // terminator 0x00613244
};
static_assert(sizeof(HandlingOverride) == 0x14, "the walk below steps 5 ints, as 0x0046b5d6 `add ecx,0x14`");

// [U-9147 2026-09-29] The selector. The original computes it as (0x0046b56c..0x0046b585)
//     v = FUN_00430790() (= DAT_0067f17c); if (FUN_0042f6a0() (= DAT_0067e9fc) == 10) v = FUN_00431d70() (= DAT_0067ea94);
//     type = FUN_0040ce80(v) = (*(int**)0x005f2770)[v][1]
// and *(int**)0x005f2770 is a runtime object array the standalone does not have.
//
// MEASURED, not chosen: on the reference scenario (track 0, car 0, quick race) the
// original's live `_DAT_00613108` is **105.0** — nine `--peek` samples over 30 s of a
// running race — and 105 is the tag-6 entry. The other three globals come out
// 40000 / 1.0 / 1.5 on the original, which is what every entry gives, so tag 6 changes
// only `_DAT_00613108`.
//
// Consequence, and it is the whole of U-9147's first divergent quantity: at full lock
// `BodyOrient_OmegaFromSteer` gives `dyaw/frame = in[0]*frameMs*_DAT_00613108*1e-4/3000`,
// so 100 vs 105 is a flat 100/105 on the body yaw rate. Measured d(bodyH)/frame:
// -0.04249 (port, tag 0) vs -0.04462 (original) = 0.9523.
//
// `MASHED_HANDLING_TYPE=0` reverts to the pre-fix selector for A/B (ROADMAP v3: a flag
// may only turn the ported behaviour OFF). [UNCERTAIN U-9783] FUN_0040ce80 itself is ported, but the
// object array *(int**)0x005f2770 it indexes has no producer in the standalone, so the type cannot yet
// vary per car or per track there; the .asi reads the live chain (A3HandlingType below).
static int HandlingTypeIndex() {
    static const int s_t = [] {
        const char* e = std::getenv("MASHED_HANDLING_TYPE");
        return e ? std::atoi(e) : 6;
    }();
    return s_t;
}

// [dcphys 2026-10-21] ONE body for 0x0046b540 in both targets (D4 dual-copy burn-down, audit P4,
// verify/dcphys_20261021): this TU is in exe_sources.rsp AND asi_sources.rsp, and the .asi's A3_Entry
// (PhysicsChainHooks.cpp) calls VehicleInit. The only target-specific part is below: the record base, the
// selector's input (the standalone has no 0x005f2770 array, so it keeps the measured type), the table (the .asi
// walks the live one, the standalone the image copy above) and where the four handling globals go (the standalone
// has a reader only for _DAT_00613108, g_handlingTorque; FUN_0046dd80's 0x0061313c and A3's own reads of
// 0x00613114 / 0x00613130 have no standalone reader).
#ifdef MASHED_STANDALONE
static inline char* A3Record(int slot) {
    return reinterpret_cast<char*>(g_vehicleArrayBase) + (std::ptrdiff_t)slot * 0xd04;
}
static inline int        A3HandlingType() { return HandlingTypeIndex(); }
static inline const int* A3Table()        { return &kHandlingTable[0].key; }
static inline void A3PublishHandling(float param, float /*spring*/, float /*k1*/, float /*k2*/) {
    // [U-9147 2026-09-29] _DAT_00613108. It used to be `(void)h_param` with the comment
    // "consumed by later subsystems, not this record write" — true, and the later
    // subsystem is BodyOrient_OmegaFromSteer, which hardcoded 100.0f and so never saw
    // the override at all. Publish it instead of discarding it.
    g_handlingTorque = param;
}
#else
static inline char* A3Record(int slot) {                                                    // DAT_008815a0
    return reinterpret_cast<char*>(0x008815a0u) + (std::ptrdiff_t)slot * 0xd04;
}
static inline int A3HandlingType() {
    typedef int (__cdecl* fn_v_t)();
    typedef int (__cdecl* fn_i_t)(int);
    int v = reinterpret_cast<fn_v_t>(0x00430790u)();                                         // 0x0046b56c
    if (reinterpret_cast<fn_v_t>(0x0042f6a0u)() == 10) v = reinterpret_cast<fn_v_t>(0x00431d70u)();   // 0x0046b573..7d
    return reinterpret_cast<fn_i_t>(0x0040ce80u)(v);                                         // 0x0046b585
}
static inline const int* A3Table() { return reinterpret_cast<const int*>(0x00613140u); }
static inline void A3PublishHandling(float param, float spring, float k1, float k2) {
    *reinterpret_cast<float*>(0x00613108u) = param;  *reinterpret_cast<float*>(0x00613114u) = spring;
    *reinterpret_cast<float*>(0x00613130u) = k1;     *reinterpret_cast<float*>(0x0061313cu) = k2;
}
#endif

static inline float Bits(std::uint32_t b) { float f; std::memcpy(&f, &b, 4); return f; }
const     float kScale001  = Bits(0x3a83126fu);  // _DAT_005cc558 0.001 (override int->float scale)
// [dcphys 2026-10-21] the image's bits. This was the decimal 0.277778f = 0x3e8e38eb, 7 ULP above _DAT_005cea44.
const     float kRadiusK   = Bits(0x3e8e38e4u);  // _DAT_005cea44 1/3.6, loop1 squared-sum factor
constexpr float kRadiusMul = 1.05f;      // _DAT_005cea54 (0x3f866666) radius scale
constexpr float kDistMul   = 3.6f;       // _DAT_005cc754 (0x40666666) attach-distance scale
constexpr float kRecip1    = 1.0f;       // _DAT_005cc320 (0x3f800000)


// 0x0046b540 — VehicleInit(slot, trackType). trackType is unused (the selector is A3HandlingType above); the
// standalone's spawn loop passes course_id_.
int VehicleInit(int slot, int /*trackType*/)
{
    // In the original's order: the handling defaults and the table walk run BEFORE the slot check
    // (0x0046b544..0x0046b5dc, then 0x0046b5e2 `cmp esi,0x10`).
    float h_param  = 100.0f, h_spring = 40000.0f, h_k1 = 1.0f, h_k2 = 1.5f;   // 0x0046b544..0x0046b562
    const int typeIdx = A3HandlingType();
    for (const int* e = A3Table(); e[0] != -1; e += 5) {
        if (e[0] == typeIdx) {
            h_param  = (float)e[1];
            h_spring = (float)e[2];
            h_k1     = (float)e[3] * kScale001;
            h_k2     = (float)e[4] * kScale001;
        }
    }
    A3PublishHandling(h_param, h_spring, h_k1, h_k2);

    if ((unsigned)slot >= 16) return 0;                                       // 0x0046b5e2..0x0046b5ed

    char* rec = A3Record(slot);

    WF(rec, 0x154, h_k2);
    WB(rec, 0x174, 0x3f8a9bd0); WB(rec, 0x238, 0x3f8a9bd0);
    WB(rec, 0x2fc, 0xbf92b7fe); WB(rec, 0x3c0, 0xbf92b7fe);
    WI(rec, 0x168, 2);          WI(rec, 0x22c, 2);
    WF(rec, 0x18c, h_k1); WF(rec, 0x250, h_k1); WF(rec, 0x314, h_k1); WF(rec, 0x3d8, h_k1);
    WB(rec, 0x190, 0x42080000); WB(rec, 0x254, 0x42080000);   // 34.0 spring rate
    WB(rec, 0x318, 0x42080000); WB(rec, 0x3dc, 0x42080000);
    // override int fields (raw copies of DAT_00613110/118/11c/120/124/128/12c into the record)
    WB(rec, 0x49c, 0x457a0000);                 // DAT_00613110 = 4000.0
    WF(rec, 0x498, h_spring);                   // DAT_00613114
    WB(rec, 0x478, 0x40800000);                 // DAT_00613118 = 4.0
    WB(rec, 0x47c, 0x40400000);                 // DAT_0061311c = 3.0
    WB(rec, 0x480, 0x4019999a);                 // DAT_00613120 = 2.4
    WB(rec, 0x484, 0x40066666);                 // DAT_00613124 = 2.1
    WB(rec, 0x488, 0x3fe66666);                 // DAT_00613128 = 1.8
    WB(rec, 0x48c, 0x40c00000);                 // DAT_0061312c = 6.0
    WB(rec, 0x68,  0x3e9a0275); WB(rec, 0x74, 0x3e9a0275);
    WI(rec, 0x4,   1);    // active flag  (&DAT_008815a4)[slot*0x341]
    WI(rec, 0x10,  0);    // state        (&DAT_008815b0)
    WB(rec, 0x150, 0x3e19999a);                 // 0.15
    WB(rec, 0x158, 0x3fa51eb8);                 // 1.290
    WB(rec, 0x50,  0x447a0000);                 // mass 1000.0
    WI(rec, 0x15c, 0); WI(rec, 0x160, 0); WI(rec, 0x164, 0);   // ref point (0,0,0)
    WB(rec, 0x16c, 0x3f19e83e);                 // wheel arm +0.601
    WF(rec, 0x170, 0.397094f);
    WB(rec, 0x230, 0xbf19e83e); WB(rec, 0x234, 0x3ecb4fe8);    // -0.601 / 0.397
    WB(rec, 0x2f4, 0x3f698890); WB(rec, 0x2f8, 0x3f078ee3);
    WB(rec, 0x3b8, 0xbf698890); WB(rec, 0x3bc, 0x3f078ee3);
    WI(rec, 0x2f0, 1); WI(rec, 0x3b4, 1);
    WB(rec, 0x60,  0x3e2b020c); WF(rec, 0x64, 0.0f); WB(rec, 0x6c, 0xbe2b020c);
    WI(rec, 0x70,  0);
    WB(rec, 0x78,  0x3e81bda5); WI(rec, 0x7c, 0); WB(rec, 0x80, 0xbea30553);
    WB(rec, 0x84,  0xbe81bda5); WI(rec, 0x88, 0); WB(rec, 0x8c, 0xbea30553);
    WI(rec, 0x9a8, 0); WI(rec, 0x9ac, 1);       // contact double-buffer A/B
    WI(rec, 0xad0, 0);
    WI(rec, 0x14c, 0); WI(rec, 0x148, 0); WI(rec, 0x144, 0);
    WB(rec, 0x54,  0x3a83126f);                 // 0.001

    // loop1: per-wheel scaled radius -> max; wheel stride 0x31 floats (0xc4 bytes) from +0x170
    // [d2settle U-9181] with A3LutOn() every sum below is in the ORIGINAL's operand order
    // (each one is rounded to float before the sqrt call, `fstp dword ptr [esp]`), and
    // this TU is SSE2 so every float op rounds as the original's PC_24 x87 does.
    const bool lut = A3LutOn();
    float maxr = 0.0f;
    {
        char* w = rec + 0x170;
        for (int i = 0; i < 4; ++i, w += 0x31 * 4) {
            WF(w, 3 * 4, RF(w, 0));                       // pfVar9[3] = pfVar9[0]
            float r;
            if (lut) {
                // 0x0046b7f2..0x0046b821: A = m[+4]*K, B = m[0]*K, C = m[-4]*K,
                // s = (C*C + A*A) + B*B; 0x0046b82b s == 0.0 skips the sqrt (r = 0).
                const float A = RF(w, 1 * 4) * kRadiusK, B = RF(w, 0) * kRadiusK,
                            C = RF(w, -1 * 4) * kRadiusK;
                const float s = (C * C + A * A) + B * B;
                r = (s != 0.0f) ? FSqrt(s) : 0.0f;        // 0x0046b83f call 0x004c3b30
            } else {
                double s = (double)RF(w, 0) * kRadiusK * RF(w, 0) * kRadiusK
                         + (double)RF(w, 1 * 4) * kRadiusK * RF(w, 1 * 4) * kRadiusK
                         + (double)RF(w, -1 * 4) * kRadiusK * RF(w, -1 * 4) * kRadiusK;
                r = (s != 0.0) ? FSqrt((float)s) : 0.0f;
            }
            if (maxr < r) maxr = r;                       // 0x0046b847..0x0046b852
        }
    }
    WF(rec, 0x4a0, maxr * kRadiusMul);                    // 0x0046b86b..0x0046b87a
    // loop2: pfVar8[2] = |wheel offset|
    {
        char* w = rec + 0x170;
        for (int i = 0; i < 4; ++i, w += 0x31 * 4) {
            if (lut) {
                // 0x0046b880..0x0046b8a5: X = m[-4], Y = m[0], Z = m[+4]; (Z*Z + Y*Y) + X*X.
                const float X = RF(w, -1 * 4), Y = RF(w, 0), Z = RF(w, 1 * 4);
                WF(w, 2 * 4, FSqrt((Z * Z + Y * Y) + X * X));
            } else {
                WF(w, 2 * 4, FSqrt(RF(w,1*4)*RF(w,1*4) + RF(w,0)*RF(w,0) + RF(w,-1*4)*RF(w,-1*4)));
            }
        }
    }
    // loops 3-5 (0x0046b8c1.., 0x0046b920.., 0x0046b9a0..): dx, dy, dz against the ref
    // point, s = (dz*dz + dy*dy) + dx*dx (0x0046b93b..0x0046b949).
    auto dist = [&](float dx, float dy, float dz) {
        return lut ? FSqrt((dz * dz + dy * dy) + dx * dx) : FSqrt(dx*dx + dy*dy + dz*dz);
    };
    // loop3: 4 attach points from +0x64, out at +0x4bc, OUTPUT STRIDE 0x40
    //
    // [U-9147 2026-09-29] The output stride was 0x10 here and in loops 4/5 below. SETTLED
    // FROM THE DISASSEMBLY of the original, not from symmetry: all three loops advance the
    // output pointer by 0x40, and the counts and source stride match this port exactly.
    //     loop3  0x0046b8b6  lea ebx, [esi + 0x4bc]   0x0046b8bc  mov ebp, 4
    //            0x0046b903  add edi, 0xc             0x0046b906  add ebx, 0x40
    //            0x0046b90a  fstp dword ptr [ebx - 0x40]
    //     loop4  0x0046b90f  lea ebx, [esi + 0x5bc]   0x0046b91b  mov ebp, 8
    //            0x0046b97f  add edi, 0xc             0x0046b982  add ebx, 0x40
    //     loop5  0x0046b988  lea ebx, [esi + 0x7bc]   0x0046b994  mov ebp, 6
    //            0x0046b9e2  add edi, 0xc             0x0046b9e5  add ebx, 0x40
    // The base spacing agrees independently: 0x5bc - 0x4bc = 0x100 = 4 x 0x40 and
    // 0x7bc - 0x5bc = 0x200 = 8 x 0x40. The .asi C4 copy is also 0x40
    // (`PhysicsChainHooks.cpp:3003`/`:3011`, `float*` += 0x10 = 0x40 bytes).
    {
        char* p = rec + 0x64; char* o = rec + 0x4bc;
        for (int i = 0; i < 4; ++i, p += 3 * 4, o += 0x40) {
            float dx = RF(p,-1*4) - RF(rec,0x15c), dy = RF(p,0) - RF(rec,0x160), dz = RF(p,1*4) - RF(rec,0x164);
            WF(o, 0, dist(dx, dy, dz) * kDistMul);
        }
    }
    // loop4: 8 points from +0x94, out at +0x5bc; updates max
    {
        char* p = rec + 0x94; char* o = rec + 0x5bc;
        for (int i = 0; i < 8; ++i, p += 3 * 4, o += 0x40) {   // stride: see loop3
            float dx = RF(p,-1*4) - RF(rec,0x15c), dy = RF(p,0) - RF(rec,0x160), dz = RF(p,1*4) - RF(rec,0x164);
            float r = dist(dx, dy, dz);                        // 0x0046b954 / 0x0046b959
            WF(o, 0, r * kDistMul);
            if (maxr < r) maxr = r;                            // 0x0046b968..0x0046b97b
        }
    }
    // loop5: 6 points from +0xf4, out at +0x7bc
    {
        char* p = rec + 0xf4; char* o = rec + 0x7bc;
        for (int i = 0; i < 6; ++i, p += 3 * 4, o += 0x40) {   // stride: see loop3
            float dx = RF(p,-1*4) - RF(rec,0x15c), dy = RF(p,0) - RF(rec,0x160), dz = RF(p,1*4) - RF(rec,0x164);
            WF(o, 0, dist(dx, dy, dz) * kDistMul);
        }
    }
    WF(rec, 0x4a4, maxr * kRadiusMul);                         // 0x0046b9ee..0x0046ba00

    // mass redistribution across the 4 spring values (+0x300/+0x3c4/+0x23c/+0x178)
    float f1 = RF(rec,0x300) + RF(rec,0x3c4) + RF(rec,0x23c) + RF(rec,0x178);
    float f2 = RF(rec,0x50) /
        (f1/RF(rec,0x300) + f1/RF(rec,0x3c4) + f1/RF(rec,0x23c) + f1/RF(rec,0x178));
    if (lut) {
        // 0x0046ba49..0x0046ba79: ((t3c4 + t300) + t23c) + t178, each t = (q*f2)*a.
        f1 = (((f1/RF(rec,0x3c4))*f2*RF(rec,0x3c4) + (f1/RF(rec,0x300))*f2*RF(rec,0x300))
              + (f1/RF(rec,0x23c))*f2*RF(rec,0x23c)) + (f1/RF(rec,0x178))*f2*RF(rec,0x178);
    } else {
        f1 = (f1/RF(rec,0x178))*f2*RF(rec,0x178) + (f1/RF(rec,0x23c))*f2*RF(rec,0x23c)
           + (f1/RF(rec,0x300))*f2*RF(rec,0x300) + (f1/RF(rec,0x3c4))*f2*RF(rec,0x3c4);
    }
    WF(rec, 0x58, f1);
    WF(rec, 0x5c, kRecip1 / f1);

    // [d2settle G-A3] MASHED_A3_LOG=<path>: one line per init, the 26 A3 outputs as raw
    // bits, compared against the original's o_p0a.msd frame-1 record. DIAGNOSTIC ONLY,
    // default-OFF, writes nothing when unset.
    if (const char* a3log = std::getenv("MASHED_A3_LOG")) {
        if (std::FILE* lf = std::fopen(a3log, "a")) {
            auto B = [&](int off) { std::uint32_t b; std::memcpy(&b, rec + off, 4); return b; };
            std::fprintf(lf, "slot=%d 4a0=%08x 178=%08x 23c=%08x 300=%08x 3c4=%08x", slot,
                         B(0x4a0), B(0x178), B(0x23c), B(0x300), B(0x3c4));
            for (int i = 0; i < 4; ++i)  std::fprintf(lf, " %x=%08x", 0x4bc + i * 0x40, B(0x4bc + i * 0x40));
            for (int i = 0; i < 8; ++i)  std::fprintf(lf, " %x=%08x", 0x5bc + i * 0x40, B(0x5bc + i * 0x40));
            for (int i = 0; i < 6; ++i)  std::fprintf(lf, " %x=%08x", 0x7bc + i * 0x40, B(0x7bc + i * 0x40));
            std::fprintf(lf, " 4a4=%08x 58=%08x 5c=%08x\n", B(0x4a4), B(0x58), B(0x5c));
            std::fclose(lf);
        }
    }
    return 1;
}

} // namespace Vehicle
} // namespace mashed_re
