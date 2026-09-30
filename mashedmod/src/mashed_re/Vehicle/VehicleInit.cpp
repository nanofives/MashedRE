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
#include <cstddef>   // std::ptrdiff_t (exe build's stricter include set surfaces this)
#include <cstring>
#include <cmath>
#include <cstdlib>   // [U-9147] std::getenv / std::atoi for MASHED_HANDLING_TYPE

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

// Handling-override table @0x00613140 (5-int stride, -1 terminated). The original walks
// it as (`Vehicle/PhysicsChainHooks.cpp:2938-2951`, A3 body 0x0046b540):
//     e = 0x00613148;  tag = [0x00613140];
//     do { if (tag == typeIdx) { _DAT_00613108 = (float)e[-1];
//                                _DAT_00613114 = (float)e[0];
//                                _DAT_00613130 = (float)e[1] * 0.001;
//                                _DAT_0061313c = (float)e[2] * 0.001; }
//          tag = e[3]; e += 5; } while (tag != -1);
//
// [U-9147 2026-09-29] HARVESTED LIVE, closing the old `[UNCERTAIN U-A3-TABLE]` for the
// first four entries. `re/frida/scenario_launch.py --peek` on a running
// original/MASHED.exe read 0x00613148..0x00613180 as ints:
//     0x00613148 40000  0x0061314c 1000  0x00613150 1500  0x00613154 6    0x00613158 105
//     0x0061315c 40000  0x00613160 1000  0x00613164 1500  0x00613168 12   0x0061316c 95
//     0x00613170 40000  0x00613174 1000  0x00613178 1500  0x0061317c 18   0x00613180 100
// so, resolved through the walk above, tag 0 -> 100, tag 6 -> 105, tag 12 -> 95,
// tag 18 -> 100, with spring/k1/k2 the same 40000/1000/1500 in every entry.
// The previous row here recorded "{ 0, 100, ... }" only, which is why the port ran the
// tag-0 value. [UNCERTAIN] tags past 18 are still unharvested.
struct HandlingOverride { int key, paramA, spring, k1, k2; };
static const HandlingOverride kHandlingTable[] = {
    {  0, 100, 40000, 1000, 1500 },   // e[-1] = 0x00613144
    {  6, 105, 40000, 1000, 1500 },   // e[-1] = 0x00613158   <- what the original uses
    { 12,  95, 40000, 1000, 1500 },   // e[-1] = 0x0061316c
    { 18, 100, 40000, 1000, 1500 },   // e[-1] = 0x00613180
    { -1,   0,     0,    0,    0 },   // terminator
};

// [U-9147 2026-09-29] The selector. The original computes it as
// `FUN_0040ce80(FUN_00430790())` — a vehicle/DFF type index, NOT a course id — and
// neither of those is ported (no DFF in the standalone). `TrackRenderer.cpp` passes
// `course_id_` here, which with MASHED_TRACK_SEL=0 is 0 and selects the tag-0 entry.
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
// may only turn the ported behaviour OFF). [UNCERTAIN] the producer FUN_0040ce80 is
// still unported, so this cannot yet vary per car or per track.
static int HandlingTypeIndex(int fallback) {
    static const int s_t = [] {
        const char* e = std::getenv("MASHED_HANDLING_TYPE");
        return e ? std::atoi(e) : 6;
    }();
    (void)fallback;
    return s_t;
}

constexpr float kScale001  = 0.001f;     // _DAT_005cc558 (override int->float scale, 0x3a83126f)
constexpr float kRadiusK   = 0.277778f;  // _DAT_005cea44 (0x3e8e38e4) loop1 squared-sum factor
constexpr float kRadiusMul = 1.05f;      // _DAT_005cea54 (0x3f866666) radius scale
constexpr float kDistMul   = 3.6f;       // _DAT_005cc754 (0x40666666) attach-distance scale
constexpr float kRecip1    = 1.0f;       // _DAT_005cc320 (0x3f800000)

// ---------------------------------------------------------------------------
// FUN_0046b1c0 — VehicleBuildContactHull(slot, box)
//
// [U-9155 2026-09-29] The 18 contact points at record +0x60..+0x137 are what the
// car<->world contact scan (0x00469aa0 -> 0x00468d80) tests against the terrain.
// A3 (0x0046b540) only READS points 4..17 — its loops at 0x0046b915
// (`lea edi,[esi+0x94]`, 8 iterations) and 0x0046b98e (`lea edi,[esi+0xf4]`, 6) build
// the per-slot radii at +0x5bc/+0x7bc from them. THIS function is their producer, and
// it runs immediately BEFORE A3 at the call site:
//     [0x0040ed57] call 0x0041f000   ; fill the 6-float box for this car
//     [0x0040ed62] call 0x0046b1c0   ; -> the 14 points at +0x90..+0x137
//     [0x0040ed68] call 0x0046b540   ; A3, which reads them
// (2 call sites, `re/tools/callsites.py 0x0046b1c0`: 0x0040ed62 and 0x0040ff86.)
//
// WHERE THE BOX COMES FROM. `FUN_0041f000(car, out)` copies 6 dwords from
// `DAT_0063dc10 + car*0x2ac`. That address is in `.data` PAST the section's raw size,
// i.e. it is filled at load time and is NOT readable from the file. So it was
// MEASURED on the running anchored original, the same way `_DAT_00613108` was:
//     py -3.12 re/frida/scenario_launch.py --peek "0063dc10:f,...,0063dc24:f" \
//         --cars 4 --car 0 --poke-ctrl-slots --hold 25
// 7 samples over 22.6 s, all identical, and cars 0/1/2/3 (0x0063dc10, 0x0063debc,
// 0x0063e168, 0x0063e414) carry the SAME six values; the unspawned slot 4
// (0x0063e6c0) reads all zero.
//
// [UNCERTAIN U-9155] the PRODUCER of DAT_0063dc10 is not ported and not identified, so
// the port cannot derive the box per car/model — it seeds the measured one for every
// slot. All four cars agreed on the reference scenario, so this is exact there and
// unverified elsewhere. NEXT COMMAND: `py -3.12 re/tools/findoffset.py --writes 0x2ac`
// scoped to the DAT_0063dc10 base, or peek the table across several car selections.
// DUAL COPY, DELIBERATE — read this before touching either side.
// `Vehicle/VehicleSlotAabbExpand.cpp` already carries a C3, Frida-GREEN, naked-x87
// transcription of this same RVA. The standalone CANNOT call it: that body writes the
// ORIGINAL's absolute record array (`0x008815a0 + slot*0xd04`) and reads the image
// constants at `ds:[05CC32Ch]`/`ds:[05CE034h]`, none of which is mapped in
// `mashed_re.exe`, whose records live in `g_records`. So the split is by TARGET:
// the naked copy stays `.asi`-only (its `RH_ScopedInstall` at :215 is its only
// consumer) and this record-base-relative copy is the exe's. Registered in
// `re/tools/dual_copy_allowlist.txt` as CROSS-TARGET, the same shape as the
// 0x0046b540 pair already there. Cross-check that makes the split safe: the naked
// port's own header states it writes "rec[+0x90 .. +0x134] (42 dwords, 0xa8 bytes)
// derived ENTIRELY from in6[0..5] plus 0x005cc32c (=0.5f) and 0x005ce034" -- exactly
// the field range, count and constants below.
static const float kContactHullBox[6] = {
    +0.21879999339580536f,   // measured @0x0063dc10
    +0.30860000848770140f,   // measured @0x0063dc14
    +0.45379999279975890f,   // measured @0x0063dc18
    -0.21879999339580536f,   // measured @0x0063dc1c
    +0.03739999979734421f,   // measured @0x0063dc20
    -0.52329999208450320f,   // measured @0x0063dc24
};
static const float kHullHalf  = 0.5f;                  // _DAT_005cc32c, FMUL @0x0046b2d6
static const float kHullThird = 0.33333298563957214f;  // _DAT_005ce034 = 0x3eaaaa9f, FMUL @0x0046b343

// 0x0046b1c0  VehicleBuildContactHull
void VehicleBuildContactHull(int slot, const float* b)
{
    if ((unsigned)slot >= 16) return;                       // 0x0046b1c7
    char* rec = reinterpret_cast<char*>(g_vehicleArrayBase) + (std::ptrdiff_t)slot * 0xd04;

    // Points 4..11 — the 8 box corners, each component copied straight through
    // (0x0046b1e3 .. 0x0046b2df; every store is a `mov dword ptr [eax+off], edx`,
    // i.e. a bit copy, not an arithmetic conversion).
    WF(rec, 0x90, b[0]); WF(rec, 0x94, b[4]); WF(rec, 0x98, b[2]);   // p4
    WF(rec, 0x9c, b[3]); WF(rec, 0xa0, b[4]); WF(rec, 0xa4, b[2]);   // p5
    WF(rec, 0xa8, b[0]); WF(rec, 0xac, b[4]); WF(rec, 0xb0, b[5]);   // p6
    WF(rec, 0xb4, b[3]); WF(rec, 0xb8, b[4]); WF(rec, 0xbc, b[5]);   // p7
    WF(rec, 0xc0, b[0]); WF(rec, 0xc4, b[1]); WF(rec, 0xc8, b[2]);   // p8
    WF(rec, 0xcc, b[3]); WF(rec, 0xd0, b[1]); WF(rec, 0xd4, b[2]);   // p9
    WF(rec, 0xd8, b[0]); WF(rec, 0xdc, b[1]); WF(rec, 0xe0, b[5]);   // p10
    WF(rec, 0xe4, b[3]); WF(rec, 0xe8, b[1]); WF(rec, 0xec, b[5]);   // p11

    // Points 12..17 — six edge points derived from p4/p5/p6/p7 (the four corners of
    // the +0x94-plane face). The x87 chains are transcribed from the loads, not from
    // the algebra: each is either (A+B)*0.5 or (2A+B)*(1/3).
    auto lerp2 = [&](int dst, int a, int bb, float k) {
        WF(rec, dst + 0, (RF(rec, a + 0) + RF(rec, bb + 0)) * k);
        WF(rec, dst + 4, (RF(rec, a + 4) + RF(rec, bb + 4)) * k);
        WF(rec, dst + 8, (RF(rec, a + 8) + RF(rec, bb + 8)) * k);
    };
    auto lerp3 = [&](int dst, int a, int bb) {
        WF(rec, dst + 0, (RF(rec, a + 0) + RF(rec, a + 0) + RF(rec, bb + 0)) * kHullThird);
        WF(rec, dst + 4, (RF(rec, a + 4) + RF(rec, a + 4) + RF(rec, bb + 4)) * kHullThird);
        WF(rec, dst + 8, (RF(rec, a + 8) + RF(rec, a + 8) + RF(rec, bb + 8)) * kHullThird);
    };
    lerp2(0xf0, 0x90, 0x9c, kHullHalf);   // p12 = (p4+p5)/2        0x0046b1f2..0x0046b301
    lerp3(0xfc, 0x9c, 0xb4);              // p13 = (2*p5+p7)/3      0x0046b307..0x0046b363
    lerp3(0x108, 0xb4, 0x9c);             // p14 = (2*p7+p5)/3      0x0046b369..0x0046b3c5
    lerp2(0x114, 0xa8, 0xb4, kHullHalf);  // p15 = (p6+p7)/2        0x0046b3cb..0x0046b417
    lerp3(0x120, 0xa8, 0x90);             // p16 = (2*p6+p4)/3      0x0046b41d..0x0046b479
    lerp3(0x12c, 0x90, 0xa8);             // p17 = (2*p4+p6)/3      0x0046b47f..0x0046b4db
}

// 0x0046b540 — VehicleInit(slot, trackType). trackType replaces the original's
// FUN_0040ce80(FUN_00430790()/...) lookup (no DFF in the standalone; pass it in).
int VehicleInit(int slot, int trackType)
{
    // 0x0040ed62 runs the hull builder immediately before this function, and A3's
    // loops 4/5 read what it wrote. Calling it here keeps the pair together in the
    // standalone, which has no equivalent of the 0x0040ecf8 spawn loop.
    VehicleBuildContactHull(slot, kContactHullBox);

    if ((unsigned)slot >= 16) return 0;

    // handling defaults (original globals _DAT_00613108 / DAT_00613114 / DAT_00613130 / 0061313c)
    float h_param  = 100.0f, h_spring = 40000.0f, h_k1 = 1.0f, h_k2 = 1.5f;
    const int typeIdx = HandlingTypeIndex(trackType);   // [U-9147] see the note above
    for (const HandlingOverride* e = kHandlingTable; e->key != -1; ++e) {
        if (e->key == typeIdx) {
            h_param  = (float)e->paramA;
            h_spring = (float)e->spring;
            h_k1     = (float)e->k1 * kScale001;
            h_k2     = (float)e->k2 * kScale001;
        }
    }
    // [U-9147 2026-09-29] _DAT_00613108. It used to be `(void)h_param` with the comment
    // "consumed by later subsystems, not this record write" — true, and the later
    // subsystem is BodyOrient_OmegaFromSteer, which hardcoded 100.0f and so never saw
    // the override at all. Publish it instead of discarding it.
    g_handlingTorque = h_param;

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
            WF(o, 0, FSqrt(dx*dx + dy*dy + dz*dz) * kDistMul);
        }
    }
    // loop4: 8 points from +0x94, out at +0x5bc; updates max
    {
        char* p = rec + 0x94; char* o = rec + 0x5bc;
        for (int i = 0; i < 8; ++i, p += 3 * 4, o += 0x40) {   // stride: see loop3
            float dx = RF(p,-1*4) - RF(rec,0x15c), dy = RF(p,0) - RF(rec,0x160), dz = RF(p,1*4) - RF(rec,0x164);
            float r = FSqrt(dx*dx + dy*dy + dz*dz);
            WF(o, 0, r * kDistMul);
            if (maxr < r) maxr = r;
        }
    }
    // loop5: 6 points from +0xf4, out at +0x7bc
    {
        char* p = rec + 0xf4; char* o = rec + 0x7bc;
        for (int i = 0; i < 6; ++i, p += 3 * 4, o += 0x40) {   // stride: see loop3
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
