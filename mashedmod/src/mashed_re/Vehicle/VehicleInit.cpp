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
#include "ForceIntegrator.h"
#include <cstdint>
#include <cstring>
#include <cmath>

namespace mashed_re {
namespace Vehicle {

extern int* g_vehicleArrayBase;   // DAT_008815a0 (16 × 0xd04 records)

// FastSqrt FUN_004c3b30 (C4) — the init uses it for radii; std::sqrt is bit-equivalent
// for the non-negative sums here (FastSqrt is an x87 fsqrt wrapper).
static inline float FSqrt(float x) { return std::sqrt(x); }

// raw-bitpattern / typed writers into the record
static inline void WB(char* r, int off, std::uint32_t bits) { std::memcpy(r + off, &bits, 4); }
static inline void WF(char* r, int off, float v)            { std::memcpy(r + off, &v, 4); }
static inline void WI(char* r, int off, std::int32_t v)     { std::memcpy(r + off, &v, 4); }
static inline float RF(char* r, int off)                    { float v; std::memcpy(&v, r + off, 4); return v; }

// Handling-override table @0x00613140 (5-int stride, -1 terminated). Harvested key-0 entry;
// the original walks `&DAT_00613148` comparing piVar6[3] (nextKey) to the track-type.
// [UNCERTAIN U-A3-TABLE]: only key 0 (Arctic-class) harvested clean; keys 6,12,… pending.
struct HandlingOverride { int key, paramA, spring, k1, k2; };
static const HandlingOverride kHandlingTable[] = {
    { 0, 100, 40000, 1000, 1500 },   // 0x00613144.. (next key 6)
    // [UNCERTAIN] keys 6,12,… not yet harvested — non-0 track types fall through to defaults
    { -1, 0, 0, 0, 0 },              // terminator
};

constexpr float kScale001  = 0.001f;     // _DAT_005cc558 (override int->float scale, 0x3a83126f)
constexpr float kRadiusK   = 0.277778f;  // _DAT_005cea44 (0x3e8e38e4) loop1 squared-sum factor
constexpr float kRadiusMul = 1.05f;      // _DAT_005cea54 (0x3f866666) radius scale
constexpr float kDistMul   = 3.6f;       // _DAT_005cc754 (0x40666666) attach-distance scale
constexpr float kRecip1    = 1.0f;       // _DAT_005cc320 (0x3f800000)

// 0x0046b540 — VehicleInit(slot, trackType). trackType replaces the original's
// FUN_0040ce80(FUN_00430790()/...) lookup (no DFF in the standalone; pass it in).
int VehicleInit(int slot, int trackType)
{
    if ((unsigned)slot >= 16) return 0;

    // handling defaults (original globals _DAT_00613108 / DAT_00613114 / DAT_00613130 / 0061313c)
    float h_param  = 100.0f, h_spring = 40000.0f, h_k1 = 1.0f, h_k2 = 1.5f;
    for (const HandlingOverride* e = kHandlingTable; e->key != -1; ++e) {
        if (e->key == trackType) {
            h_param  = (float)e->paramA;
            h_spring = (float)e->spring;
            h_k1     = (float)e->k1 * kScale001;
            h_k2     = (float)e->k2 * kScale001;
        }
    }
    (void)h_param;  // _DAT_00613108: consumed by later subsystems, not this record write

    char* rec = reinterpret_cast<char*>(g_vehicleArrayBase) + (std::ptrdiff_t)slot * 0xd04;

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
    float maxr = 0.0f;
    {
        char* w = rec + 0x170;
        for (int i = 0; i < 4; ++i, w += 0x31 * 4) {
            WF(w, 3 * 4, RF(w, 0));                       // pfVar9[3] = pfVar9[0]
            double s = (double)RF(w, 0) * kRadiusK * RF(w, 0) * kRadiusK
                     + (double)RF(w, 1 * 4) * kRadiusK * RF(w, 1 * 4) * kRadiusK
                     + (double)RF(w, -1 * 4) * kRadiusK * RF(w, -1 * 4) * kRadiusK;
            float r = (s != 0.0) ? FSqrt((float)s) : 0.0f;
            if (maxr < r) maxr = r;
        }
    }
    WF(rec, 0x4a0, maxr * kRadiusMul);
    // loop2: pfVar8[2] = |wheel offset|
    {
        char* w = rec + 0x170;
        for (int i = 0; i < 4; ++i, w += 0x31 * 4)
            WF(w, 2 * 4, FSqrt(RF(w,1*4)*RF(w,1*4) + RF(w,0)*RF(w,0) + RF(w,-1*4)*RF(w,-1*4)));
    }
    // loop3: 4 attach points from +0x64, out at +0x4bc stride 0x10
    {
        char* p = rec + 0x64; char* o = rec + 0x4bc;
        for (int i = 0; i < 4; ++i, p += 3 * 4, o += 0x10) {
            float dx = RF(p,-1*4) - RF(rec,0x15c), dy = RF(p,0) - RF(rec,0x160), dz = RF(p,1*4) - RF(rec,0x164);
            WF(o, 0, FSqrt(dx*dx + dy*dy + dz*dz) * kDistMul);
        }
    }
    // loop4: 8 points from +0x94, out at +0x5bc; updates max
    {
        char* p = rec + 0x94; char* o = rec + 0x5bc;
        for (int i = 0; i < 8; ++i, p += 3 * 4, o += 0x10) {
            float dx = RF(p,-1*4) - RF(rec,0x15c), dy = RF(p,0) - RF(rec,0x160), dz = RF(p,1*4) - RF(rec,0x164);
            float r = FSqrt(dx*dx + dy*dy + dz*dz);
            WF(o, 0, r * kDistMul);
            if (maxr < r) maxr = r;
        }
    }
    // loop5: 6 points from +0xf4, out at +0x7bc
    {
        char* p = rec + 0xf4; char* o = rec + 0x7bc;
        for (int i = 0; i < 6; ++i, p += 3 * 4, o += 0x10) {
            float dx = RF(p,-1*4) - RF(rec,0x15c), dy = RF(p,0) - RF(rec,0x160), dz = RF(p,1*4) - RF(rec,0x164);
            WF(o, 0, FSqrt(dx*dx + dy*dy + dz*dz) * kDistMul);
        }
    }
    WF(rec, 0x4a4, maxr * kRadiusMul);

    // mass redistribution across the 4 spring values (+0x300/+0x3c4/+0x23c/+0x178)
    float f1 = RF(rec,0x300) + RF(rec,0x3c4) + RF(rec,0x23c) + RF(rec,0x178);
    float f2 = RF(rec,0x50) /
        (f1/RF(rec,0x300) + f1/RF(rec,0x3c4) + f1/RF(rec,0x23c) + f1/RF(rec,0x178));
    f1 = (f1/RF(rec,0x178))*f2*RF(rec,0x178) + (f1/RF(rec,0x23c))*f2*RF(rec,0x23c)
       + (f1/RF(rec,0x300))*f2*RF(rec,0x300) + (f1/RF(rec,0x3c4))*f2*RF(rec,0x3c4);
    WF(rec, 0x58, f1);
    WF(rec, 0x5c, kRecip1 / f1);
    return 1;
}

} // namespace Vehicle
} // namespace mashed_re
