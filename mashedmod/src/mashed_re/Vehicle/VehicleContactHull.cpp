// Mashed RE — FUN_0046b1c0 VehicleBuildContactHull: ONE body, both targets.
//
// [dcphys 2026-10-21] Moved out of Vehicle/VehicleInit.cpp, verbatim, so that A3 (0x0046b540) can be ONE TU
// shared by both targets (D4 dual-copy burn-down, verify/dcphys_20261021). VehicleSpawnHull is the standalone's
// 0x0040ed57/0x0040ed62 pair, called by the spawn loop (VehiclePhysicsRun.cpp) immediately before A3, the original's
// order at 0x0040ed62 / 0x0040ed68. It used to be called from inside the exe's A3 body, which the original's A3 never does.
// [dcleaf 2026-10-22] U-9155's split by target is folded: this TU is in BOTH rsp files. The record base is
// Vehicle::VehRecord (the standalone's g_vehicleArrayBase, the .asi's absolute 0x008815a0) and the body returns the
// original's 1 / 0 (0x0046b4e8 / 0x0046b1cc). The .asi installs it at 0x0046b1c0 through the export
// VehicleSlotAabbExpand (the name the naked x87 copy, Vehicle/VehicleSlotAabbExpand.cpp, now deleted, exported), with
// an in-process A/B against the original (MASHED_LEAF_SELFTEST). The measured box and VehicleSpawnHull stay exe only.
// Arithmetic: every op of the original's x87 chains rounds to a 24-bit mantissa at MASHED.exe's PC_24 and the results
// are stored as floats, so the float (SSE) chains below give the same bits for normal operands (A/B: RESULT_dcleaf.md).
//
// Anchored to MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include "VehicleRecordBase.h"
#ifndef MASHED_STANDALONE
#include <windows.h>
#include "../Core/HookSystem.h"
#endif

namespace mashed_re {
namespace Vehicle {

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
#ifdef MASHED_STANDALONE
static const float kContactHullBox[6] = {
    +0.21879999339580536f,   // measured @0x0063dc10
    +0.30860000848770140f,   // measured @0x0063dc14
    +0.45379999279975890f,   // measured @0x0063dc18
    -0.21879999339580536f,   // measured @0x0063dc1c
    +0.03739999979734421f,   // measured @0x0063dc20
    -0.52329999208450320f,   // measured @0x0063dc24
};
#endif
static const float kHullHalf  = 0.5f;                  // _DAT_005cc32c, FMUL @0x0046b2d6
static const float kHullThird = 0.33333298563957214f;  // _DAT_005ce034 = 0x3eaaaa9f, FMUL @0x0046b343

// 0x0046b1c0  VehicleBuildContactHull
int VehicleBuildContactHull(int slot, const float* b)
{
    if ((unsigned)slot >= 16) return 0;                     // 0x0046b1c7 cmp eax,0x10 / jb; 0x0046b1cc xor eax,eax
    char* rec = const_cast<char*>(VehRecord(static_cast<std::uint32_t>(slot)));

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
    return 1;
}

#ifdef MASHED_STANDALONE
// 0x0040ed57 + 0x0040ed62: the box FUN_0041f000 fills (measured, see kContactHullBox), then the hull.
void VehicleSpawnHull(int slot) { VehicleBuildContactHull(slot, kContactHullBox); }
#endif

} // namespace Vehicle
} // namespace mashed_re

#ifndef MASHED_STANDALONE
// =====================================================================================================================
// .asi install + in-process A/B. MASHED_LEAF_SELFTEST=1: every call snapshots the 0xa8 output bytes (record
// +0x90..+0x137), runs the original, puts the snapshot back, runs the original again (noo), puts it back, runs the shared
// body, and compares the 42 dwords and the return value (ndiff). The game keeps the shared body's output. Log:
// leaf_selftest.log in the CWD (shared with Vehicle/ForceIntegratorLeaves.cpp).
// =====================================================================================================================
namespace {
// Trampoline: the 5-byte E9 hook covers MOV EAX,[ESP+4] (4 bytes) and the first byte of SUB ESP,0xC; re-run both and
// resume at 0x0046b1c7 (CMP EAX,0x10).
void* g_hullResume = reinterpret_cast<void*>(0x0046b1c7);
__declspec(naked) int __cdecl HullOrig(unsigned, const float*) {
    __asm {
        mov  eax, dword ptr [esp + 4]
        sub  esp, 0Ch
        jmp  dword ptr [g_hullResume]
    }
}
int HullSelfTestOn() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_LEAF_SELFTEST"); v = (s && s[0] && s[0] != '0') ? 1 : 0; }
    return v;
}
void HullLog(const char* s) {
    HANDLE h = CreateFileA("leaf_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}
long g_hullCalls = 0;
__declspec(noinline) int __cdecl HullDispatch(unsigned slot, const float* b) {
    using mashed_re::Vehicle::VehicleBuildContactHull;
    if (!HullSelfTestOn() || slot >= 16) return VehicleBuildContactHull(static_cast<int>(slot), b);
    std::uint32_t* out = reinterpret_cast<std::uint32_t*>(0x008815a0u + slot * 0xd04u + 0x90u);
    std::uint32_t snap[42], o1[42], o2[42];
    std::memcpy(snap, out, sizeof snap);
    const int r1 = HullOrig(slot, b);  std::memcpy(o1, out, sizeof o1); std::memcpy(out, snap, sizeof snap);
    const int r2 = HullOrig(slot, b);  std::memcpy(o2, out, sizeof o2); std::memcpy(out, snap, sizeof snap);
    const int rm = VehicleBuildContactHull(static_cast<int>(slot), b);
    int noo = (r1 != r2), nd = (rm != r1), first = -1;
    for (int i = 0; i < 42; ++i) {
        noo += (o1[i] != o2[i]);
        if (out[i] != o1[i]) { ++nd; if (first < 0) first = i; }
    }
    ++g_hullCalls;
    std::uint32_t in[6]; std::memcpy(in, b, sizeof in);
    char line[260];
    wsprintfA(line, "[%ld] fn=46b1c0 slot=%u in=%08x,%08x,%08x,%08x,%08x,%08x r=%d noo=%d ndiff=%d firstOff=0x%x\r\n", g_hullCalls,
              slot, in[0], in[1], in[2], in[3], in[4], in[5], r1, noo, nd, first < 0 ? 0 : 0x90 + first * 4);
    HullLog(line);
    return rm;
}
}  // namespace

// The installed entry and the path1 export (re/frida/hooks_registry.py vehicle_slot_aabb_expand).
extern "C" __declspec(dllexport) int __cdecl VehicleSlotAabbExpand(unsigned slot, float* in6) {
    return HullDispatch(slot, in6);
}
RH_ScopedInstall(VehicleSlotAabbExpand, 0x0046b1c0);
#endif
