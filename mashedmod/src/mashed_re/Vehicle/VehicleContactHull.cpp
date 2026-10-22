// Mashed RE — FUN_0046b1c0 VehicleBuildContactHull, the standalone's copy (exe only).
//
// [dcphys 2026-10-21] Moved out of Vehicle/VehicleInit.cpp, verbatim, so that A3 (0x0046b540) can be ONE TU
// shared by both targets (D4 dual-copy burn-down, verify/dcphys_20261021) while this RVA keeps its DELIBERATE
// split by target (U-9155, re/tools/dual_copy_allowlist.txt CROSS-TARGET:0046b1c0): the .asi's body is the naked
// copy in Vehicle/VehicleSlotAabbExpand.cpp. VehicleSpawnHull is the standalone's 0x0040ed57/0x0040ed62 pair,
// called by the spawn loop (VehiclePhysicsRun.cpp) immediately before A3, the original's order at 0x0040ed62 /
// 0x0040ed68. It used to be called from inside the exe's A3 body, which the original's A3 never does.
//
// Anchored to MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
#include <cstdint>
#include <cstddef>
#include <cstring>

namespace mashed_re {
namespace Vehicle {

extern int* g_vehicleArrayBase;   // DAT_008815a0 (16 x 0xd04 records)

static inline void  WF(char* r, int off, float v) { std::memcpy(r + off, &v, 4); }
static inline float RF(char* r, int off)          { float v; std::memcpy(&v, r + off, 4); return v; }
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

// 0x0040ed57 + 0x0040ed62: the box FUN_0041f000 fills (measured, see kContactHullBox), then the hull.
void VehicleSpawnHull(int slot) { VehicleBuildContactHull(slot, kContactHullBox); }

} // namespace Vehicle
} // namespace mashed_re
