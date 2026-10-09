// Mashed RE — D-11073 leg 1a: exe-side substrate for the race sub-state machine's
// pre-race phases (FUN_004111c0 states 4 -> 5). Brief: re/BRIEF_D11073.md §2 "Leg 1a";
// pre-registration: verify/d3_gatefire_20261008/PREREG_LEG1A.md.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Listed in BOTH exe_sources.rsp and asi_sources.rsp. Before 2026-10-09 every body below
// lived in an asi-only TU, so mashed_re.exe could not call any of them. They were MOVED
// here, not copied: one body per RVA, one RH_ScopedInstall per RVA, same export names
// (so hooks_registry.py keys and the installed copies keep their identity).
//   0x0041d910  Clear63d584Pair           from Util/PromoLoop_round39.cpp
//   0x0041da90  DeltaTimeOutGet           from Util/PromoLoop_round20.cpp
//   0x0041ae20  Copy41ae20                from Util/PromoLoop_sessionB.cpp
//   0x0041b520  Update41b520              from Util/PromoLoop_sessionB.cpp (was naked asm
//                                         calling the RAW address 0x0041ae20, which the exe
//                                         cannot execute: text is mapped only
//                                         0x00420000..0x0047ffff, exe_main.cpp:8346)
//   0x0046baa0  VehiclePhysicsStateReset  from Util/PromoLoop_round80.cpp
//   0x0046c750  EntityVelocityCounterGet  from Util/PromoLoop_round10.cpp (the INSTALLED copy;
//   0x0046c730  EntityDamageStateGet        round38's Table882194Get/Table882198Get were
//                                           semantically identical and refused at install,
//                                           hooks.csv dual-install audit 2026-09-09; deleted)
//
// Plus two PARTIALS (not function entries, so no RH_ScopedInstall), behind MASHED_SUBSTATE:
//   SubState_StoreStep         = FUN_0040fc00's step store, 0x0040fc09..0x0040fc57
//   HudSlideBillboardTimerTail = FUN_0041d930's tail, 0x0041da5f..0x0041da76. The RW billboard
//                                half (raw calls to 0x004c1480 / 0x004c13e0) is NOT here; the
//                                whole body HudSlideBillboardTick (HUD/ScenarioLeaves_sa2.cpp)
//                                calls this tail as its last statement.
//
// In leg 1a nothing in the exe calls the seven moved bodies, and nothing in the exe reads
// 0x007f100c / 0x0063d584 / 0x0063d588 (census in PREREG_LEG1A.md §2), so the default build
// and the MASHED_SUBSTATE arm are both predicted byte-identical on the dumps.

#include "SubStateCallees.h"
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// ===========================================================================
// MASHED_SUBSTATE knob + the two timer partials
// ===========================================================================

bool SubState_Enabled()
{
    static const bool s_on = [] {
        const bool on = (std::getenv("MASHED_SUBSTATE") != nullptr);
        if (on) {
            if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
                std::fprintf(lf, "[substate] MASHED_SUBSTATE=1: step store 0x0040fc57 + "
                                 "FUN_0041d930 tail 0x0041da5f..0x0041da76 live\n");
                std::fclose(lf);
            }
        }
        return on;
    }();
    return s_on;
}

// PARTIAL of FUN_0040fc00 (0x0040fc00), step store only:
//   0x0040fc05  mov [esp+4],esi            (units)
//   0x0040fc09  fild dword [esp+4]
//   0x0040fc0d  jge  0x40fc15              (negative: fadd [0x5cc94c] = 2^32, 0x0040fc0f)
//   0x0040fc1b  fmul dword [0x5cc948]      (_DAT_005cc948 = 0x39aec33e, 1/3000)
//   0x0040fc21  fstp dword [esp+8]  ...  0x0040fc57  mov [0x7f100c],eax
// The units of the port are (int)(dt*3000+0.5) (TrackRenderer.cpp, the Ai_AdvanceClock call),
// never negative, so the 2^32 fixup arm is unreachable here and is not transcribed.
// The product of two floats is exact in x87 extended precision, so the single rounding at the
// fstp equals the SSE single-precision multiply.
void SubState_StoreStep(int units)
{
    if (!SubState_Enabled()) return;
    std::uint32_t kbits = 0x39aec33eu;                      // _DAT_005cc948 (.rdata)
    float k;
    std::memcpy(&k, &kbits, sizeof k);
    *reinterpret_cast<float*>(0x007f100cu) = static_cast<float>(units) * k;   // 0x0040fc57
}

// PARTIAL of FUN_0041d930 (0x0041d930), tail only:
//   0x0041da5f  fld  dword [0x63d588]
//   0x0041da65  mov  eax,[0x63d584]
//   0x0041da6a  fadd dword [0x7f100c]
//   0x0041da70  inc  eax
//   0x0041da71  mov  [0x63d584],eax
//   0x0041da76  fstp dword [0x63d588]
// Nothing in the tail depends on the billboard half above it (the instruction before is
// `call 0x4c13e0` at 0x0041da5a, whose return is unused).
extern "C" void __cdecl HudSlideBillboardTimerTail(void)
{
    *reinterpret_cast<std::int32_t*>(0x0063d584u) =
        *reinterpret_cast<std::int32_t*>(0x0063d584u) + 1;                       // 0x0041da71
    *reinterpret_cast<float*>(0x0063d588u) =
        *reinterpret_cast<float*>(0x0063d588u) + *reinterpret_cast<float*>(0x007f100cu); // 0x0041da76
}

// ===========================================================================
// 0x0041d910  Clear63d584Pair  (FUN_0041d910, 21 B; caller FUN_0040dbd0, first statement)
// C7 05 84 D5 63 00 00 00 00 00 C7 05 88 D5 63 00 00 00 00 00 C3
// ===========================================================================

// 0x0041d910
extern "C" __declspec(dllexport) void __cdecl Clear63d584Pair(void) {
    *reinterpret_cast<std::uint32_t*>(0x0063d584u) = 0u;  // cited at 0x0041d910
    *reinterpret_cast<std::uint32_t*>(0x0063d588u) = 0u;  // cited at 0x0041d91a
}
RH_ScopedInstall(Clear63d584Pair, 0x0041d910);

// ===========================================================================
// 0x0041da90  DeltaTimeOutGet  (FUN_0041da90, 17 B; caller FUN_004103a0)
// 8B 44 24 04 / 85 C0 / 74 08 / 8B 0D 88 D5 63 00 / 89 08 / C3
//   (mov eax,[esp+4]; test eax,eax; jz +8; mov ecx,[0x0063d588]; mov [eax],ecx; ret)
// Null-guarded: if param_1 != NULL, copies DAT_0063d588 to *param_1.
// DAT_0063d588 is the timer FUN_0041d930's tail advances (above) and FUN_004103a0 compares
// against _DAT_005ccdf4 (1.86f). U-3585 (data semantics) answered by RESULT_LEG0.md.
// ===========================================================================

// 0x0041da90
extern "C" __declspec(dllexport) void __cdecl DeltaTimeOutGet(std::uint32_t* param_1) {
    // null guard + 0x0063d588 cited at 0x0041da90 body.
    if (param_1 != nullptr) {
        *param_1 = *reinterpret_cast<const std::uint32_t*>(0x0063d588u);   // 0x0041da98
    }
}
RH_ScopedInstall(DeltaTimeOutGet, 0x0041da90);

// ===========================================================================
// 0x0041ae20  Copy41ae20  (FUN_0041ae20, 52 B, __thiscall; caller FUN_0041b520)
//   0x0041ae20  mov edx,[ecx+0x64]           i = this[0x64]
//   0x0041ae24  lea edx,[edx+edx*2]
//   0x0041ae2b  lea edx,[edx*4+0x5f3304]     row = 0x005f3304 + i*12
//   0x0041ae32..0x0041ae41                   this[0x50..0x58] = row[0..2]
//   0x0041ae44  fld [eax] / 0x0041ae46 fadd st0,st0 / 0x0041ae51 fstp [eax]   this[0x50] *= 2
//   0x0041ae49  mov dword [ecx+0x70],0
// Declared __fastcall (this in ECX) so the ABI matches the original; export @Copy41ae20@4.
// Doubling a float is exact in both x87 and SSE.
// ===========================================================================

// 0x0041ae20
extern "C" __declspec(dllexport) void __fastcall Copy41ae20(char* self) {
    std::uint32_t i = *reinterpret_cast<std::uint32_t*>(self + 0x64);
    const std::uint32_t* rec = reinterpret_cast<const std::uint32_t*>(0x005f3304u + i * 12u);
    std::uint32_t* dst = reinterpret_cast<std::uint32_t*>(self + 0x50);
    dst[0] = rec[0]; dst[1] = rec[1]; dst[2] = rec[2];
    *reinterpret_cast<std::uint32_t*>(self + 0x70) = 0;
    float* f = reinterpret_cast<float*>(self + 0x50);
    *f = *f + *f;
}
RH_ScopedInstall(Copy41ae20, 0x0041ae20);

// ===========================================================================
// 0x0041b520  Update41b520  (FUN_0041b520, 22 B; caller FUN_0040dbd0)
//   0x0041b520  mov ecx,0x63c8d0
//   0x0041b525  call 0x41ae20
//   0x0041b52a  add ecx,0x74
//   0x0041b52d  cmp ecx,0x63caa0
//   0x0041b533  jl 0x41b525                  (4 records, stride 0x74)
//   0x0041b535  ret
// Calls the C body Copy41ae20 directly (the exe cannot execute the raw 0x0041ae20). The
// original clobbers EAX/ECX/EDX; so does this (all caller-saved).
// ===========================================================================

// 0x0041b520
extern "C" __declspec(dllexport) void __cdecl Update41b520(void)
{
    for (std::uintptr_t rec = 0x0063c8d0u; rec < 0x0063caa0u; rec += 0x74u)   // 0x0041b52d
        Copy41ae20(reinterpret_cast<char*>(rec));                              // 0x0041b525
}
RH_ScopedInstall(Update41b520, 0x0041b520);

// ===========================================================================
// 0x0046c750  EntityVelocityCounterGet  (FUN_0046c750, 25 B; caller FUN_004103a0)
// 8B 44 24 04 / 83 F8 10 / 7C 03 / 33 C0 / C3 / 69 C0 04 0D 00 00 / 8B 80 94 21 88 00 / C3
//   jl = SIGNED compare (int param). U-3586: stride-0xd04 block layout (data-semantic).
// ===========================================================================

// 0x0046c750
extern "C" __declspec(dllexport) std::uint32_t __cdecl EntityVelocityCounterGet(int param_1) {
    // signed bound (jl vs 0x10) cited at 0x0046c757.
    if (param_1 >= 0x10) {
        return 0u;
    }
    // 0x00882194 base + 0xd04 stride cited at 0x0046c75c / 0x0046c762.
    return *reinterpret_cast<const std::uint32_t*>(
        0x00882194u + static_cast<std::uint32_t>(param_1) * 0xd04u);
}
RH_ScopedInstall(EntityVelocityCounterGet, 0x0046c750);

// ===========================================================================
// 0x0046c730  EntityDamageStateGet  (FUN_0046c730, 25 B; callers FUN_004103a0, FUN_004642f0)
// Same shape as 0x0046c750 with base 0x00882198 (8B 80 98 21 88 00 at 0x0046c742).
// ===========================================================================

// 0x0046c730
extern "C" __declspec(dllexport) std::uint32_t __cdecl EntityDamageStateGet(int param_1) {
    // signed bound (jl vs 0x10) cited at 0x0046c737.
    if (param_1 >= 0x10) {
        return 0u;
    }
    // 0x00882198 base + 0xd04 stride cited at 0x0046c73c / 0x0046c742.
    return *reinterpret_cast<const std::uint32_t*>(
        0x00882198u + static_cast<std::uint32_t>(param_1) * 0xd04u);
}
RH_ScopedInstall(EntityDamageStateGet, 0x0046c730);


// ---------------------------------------------------------------------------
// VehiclePhysicsStateReset  --  0x0046baa0
//
// Original: FUN_0046baa0 (571 bytes, 0x0046baa0..0x0046bcdb)
// Signature: undefined4 FUN_0046baa0(uint param_1)
//   param_1: vehicle slot index (0..15 valid; > 15 -> return 0)
// Returns: 0 if out-of-bounds, 1 on success.
//
// Algorithm (cited from 0x0046baa0 body):
//   1. Guard: if (param_1 > 0xf) return 0;                             [0x0046baa4]
//   2. Compute stride: iVar1 = param_1 * 0xd04;                        [0x0046bab0]
//      (Note: accesses via (&DAT_xxx)[param_1 * 0x341] on undefined4*
//       are equivalent byte offsets = param_1 * 0x341 * 4 = param_1 * 0xd04 = iVar1)
//   3. Self-ref write: (&DAT_008815a0)[param_1 * 0x341] = param_1;     [0x0046bab8]
//   4. Zero velocity XYZ (3 fields at 0x881740/44/48 + iVar1);         [0x0046babb..]
//   5. Zero angular rate XYZ (0x881734 and 0x8817a0/a8 + iVar1);       [0x0046bac8..]
//   6. Zero force/torque accumulators (0x881804/08/0c/7f8 + iVar1);    [0x0046bad7..]
//   7. Zero contact normals (0x881864/6c/8c8/cc/d0/bc + iVar1);        [0x0046bae7..]
//   8. Zero suspension (0x881928/30 + iVar1);                           [0x0046baff..]
//   9. Set 4 fields to 0x3f800000 (1.0f):
//      0x8817a4, 0x881868, 0x88192c, 0x8819f0 + iVar1;                 [0x0046bb07..]
//  10. Set 4 fields to integer 1:
//      0x881738, 0x8817fc, 0x8818c0, 0x881984 + iVar1;                 [0x0046bb23..]
//  11. Zero 0x88198c/90/94, 0x881980, 0x8819ec/f4 + iVar1;             [0x0046bb37..]
//  12. Write 0x40800000 (4.0f) and -1.0f (0xbf800000) and 0s to
//      the 0x341-stride per-slot fields at 0x881f80..0x8820bc;         [0x0046bb57..]
//  13. Loop 1 (16 iterations): packed 3-dword array at
//      (&DAT_008820d0)[param_1 * 0x341]; writes [0, -1.0f, 0] per entry; [0x0046bb89]
//  14. Zero remaining physics fields 0x88218c, 0x881f64/60/5c + iVar1
//      and per-slot at 0x881f58/54/50 etc.;                            [0x0046bbba..]
//  15. Loop 2 (18 iterations): 0x40-stride array at
//      &DAT_00881a4c + iVar1; writes 0xffffffff per entry;             [0x0046bc96]
//  16. Zero per-slot entity table fields at 0x8815b4..0x8815ec
//      (stride 0x341);                                                  [0x0046bcb5..]
//  17. Return 1.                                                         [0x0046bcd9]
//
// Constants (all cited from 0x0046baa0 body):
//   0xf (15)       — guard upper-bound                                  [0x0046baa4]
//   0xd04 (3332)   — per-vehicle stride in the physics SOA               [0x0046bab0]
//   0x341 (833)    — undefined4* stride (byte offset = *4 = 0xd04)       [0x0046bab8]
//   0x3f800000     — 1.0f in IEEE 754                                    [0x0046bb07]
//   0xbf800000     — -1.0f in IEEE 754                                   [0x0046bb89]
//   0x40800000     — 4.0f in IEEE 754                                    [0x0046bb57]
//   0xffffffff     — sentinel for loop 2                                 [0x0046bc96]
//   0x10 (16)      — iteration count for loop 1                          [0x0046bb89]
//   0x12 (18)      — iteration count for loop 2                          [0x0046bc96]
//   0x40 (64)      — byte stride for loop 2                              [0x0046bc96]
//
// Uncertainties (non-blocking):
//   U-3012: per-vehicle physics SOA field semantics (exact names of all
//           zeroed/initialized fields). Known anchors: 0x881734..748 = velocity XYZ.
//
// ref: re/analysis/game_state_d5/0x0046baa0.md
// ---------------------------------------------------------------------------

// 0x0046baa0
extern "C" __declspec(dllexport) std::uint32_t __cdecl VehiclePhysicsStateReset(
    std::uint32_t param_1)
{
    // Guard: slot index > 15 -> return 0. [0x0046baa4]
    if (param_1 > 0xfu) {
        return 0u;
    }

    // Per-vehicle byte stride in the physics SOA. [0x0046bab0]
    int iVar1 = static_cast<int>(param_1) * 0xd04;

    // Self-reference: write slot index to entity table.                  [0x0046bab8]
    // (&DAT_008815a0)[param_1 * 0x341] is undefined4* array -> byte off = param_1*0xd04 = iVar1
    *reinterpret_cast<std::uint32_t*>(0x008815a0u + static_cast<unsigned>(iVar1)) = param_1;

    // Zero velocity XYZ.                                                  [0x0046babb]
    *reinterpret_cast<std::uint32_t*>(0x00881740u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881744u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881748u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero angular rate fields.                                           [0x0046bac8]
    *reinterpret_cast<std::uint32_t*>(0x00881734u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008817a0u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008817a8u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero force accumulators XYZ and torque.                            [0x0046bad7]
    *reinterpret_cast<std::uint32_t*>(0x00881804u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881808u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x0088180cu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008817f8u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero contact normals and contact state fields.                     [0x0046bae7]
    *reinterpret_cast<std::uint32_t*>(0x00881864u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x0088186cu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008818c8u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008818ccu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008818d0u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008818bcu + static_cast<unsigned>(iVar1)) = 0u;

    // Zero suspension state.                                             [0x0046baff]
    *reinterpret_cast<std::uint32_t*>(0x00881928u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881930u + static_cast<unsigned>(iVar1)) = 0u;

    // Set angular rate scale, contact normal Z, suspension scale, inertia/mass to 1.0f. [0x0046bb07]
    *reinterpret_cast<std::uint32_t*>(0x008817a4u + static_cast<unsigned>(iVar1)) = 0x3f800000u;
    *reinterpret_cast<std::uint32_t*>(0x00881868u + static_cast<unsigned>(iVar1)) = 0x3f800000u;
    *reinterpret_cast<std::uint32_t*>(0x0088192cu + static_cast<unsigned>(iVar1)) = 0x3f800000u;
    *reinterpret_cast<std::uint32_t*>(0x008819f0u + static_cast<unsigned>(iVar1)) = 0x3f800000u;

    // Set 4 fields to integer 1.                                         [0x0046bb23]
    *reinterpret_cast<std::uint32_t*>(0x00881738u + static_cast<unsigned>(iVar1)) = 1u;
    *reinterpret_cast<std::uint32_t*>(0x008817fcu + static_cast<unsigned>(iVar1)) = 1u;
    *reinterpret_cast<std::uint32_t*>(0x008818c0u + static_cast<unsigned>(iVar1)) = 1u;
    *reinterpret_cast<std::uint32_t*>(0x00881984u + static_cast<unsigned>(iVar1)) = 1u;

    // Zero additional physics fields.                                    [0x0046bb37]
    *reinterpret_cast<std::uint32_t*>(0x0088198cu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881990u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881994u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881980u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008819ecu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008819f4u + static_cast<unsigned>(iVar1)) = 0u;

    // Write 4.0f to per-slot field (0x341 stride = 0xd04 byte offset).  [0x0046bb57]
    *reinterpret_cast<std::uint32_t*>(0x00881f80u + static_cast<unsigned>(iVar1)) = 0x40800000u;

    // Zero more physics fields (iVar1 stride).                          [0x0046bb5f]
    *reinterpret_cast<std::uint32_t*>(0x008820bcu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008820b8u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008820b4u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero per-slot fields at 0x341 stride.                             [0x0046bb71]
    *reinterpret_cast<std::uint32_t*>(0x00882090u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00882094u + static_cast<unsigned>(iVar1)) = 0u;

    // Write -1.0f to per-slot field.                                     [0x0046bb82]
    *reinterpret_cast<std::uint32_t*>(0x00882098u + static_cast<unsigned>(iVar1)) = 0xbf800000u;

    // Zero per-slot field.                                               [0x0046bb8b]
    *reinterpret_cast<std::uint32_t*>(0x0088209cu + static_cast<unsigned>(iVar1)) = 0u;

    // Loop 1: 16 iterations, packed 3-dword entries starting at         [0x0046bb89]
    // (&DAT_008820d0)[param_1 * 0x341] = 0x008820d0 + iVar1.
    // Each entry: [element[-1]=0, element[0]=-1.0f, element[+1]=0], advance by 3.
    {
        std::uint32_t* puVar2 = reinterpret_cast<std::uint32_t*>(
            0x008820d0u + static_cast<unsigned>(iVar1));
        int iVar3 = 0x10;
        do {
            puVar2[-1] = 0u;
            *puVar2    = 0xbf800000u;
            puVar2[1]  = 0u;
            puVar2 += 3;
            iVar3 -= 1;
        } while (iVar3 != 0);
    }

    // Zero additional fields (iVar1 stride).                            [0x0046bbba]
    *reinterpret_cast<std::uint32_t*>(0x0088218cu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881f64u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881f60u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881f5cu + static_cast<unsigned>(iVar1)) = 0u;

    // Zero per-slot fields (0x341 stride).                              [0x0046bbcc]
    *reinterpret_cast<std::uint32_t*>(0x00881f58u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881f54u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881f50u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero physics fields (iVar1 stride).                               [0x0046bbde]
    *reinterpret_cast<std::uint32_t*>(0x00882088u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00882084u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00882080u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x0088207cu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00882078u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00882074u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero per-slot fields (0x341 stride).                              [0x0046bbfa]
    *reinterpret_cast<std::uint32_t*>(0x00881f88u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881f84u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero physics fields (iVar1 stride).                               [0x0046bc0c]
    *reinterpret_cast<std::uint32_t*>(0x008820a8u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008820a4u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008820a0u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero per-slot fields (0x341 stride).                              [0x0046bc1e]
    *reinterpret_cast<std::uint32_t*>(0x00881f8cu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881f90u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero physics field (iVar1 stride).                                [0x0046bc30]
    *reinterpret_cast<std::uint32_t*>(0x00881f94u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00882070u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881a30u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00881a34u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008820c4u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008820c8u + static_cast<unsigned>(iVar1)) = 0u;

    // Zero per-slot field (0x341 stride).                               [0x0046bc52]
    *reinterpret_cast<std::uint32_t*>(0x0088208cu + static_cast<unsigned>(iVar1)) = 0u;

    // Zero physics fields (iVar1 stride).                               [0x0046bc5a]
    *reinterpret_cast<std::uint32_t*>(0x008820acu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008820b0u + static_cast<unsigned>(iVar1)) = 0u;

    // Loop 2: 18 iterations, stride 0x40 (64 bytes = 0x10 uint32_t elements). [0x0046bc96]
    // Writes 0xffffffff to &DAT_00881a4c + iVar1, then +0x40, +0x80, ...
    {
        std::uint32_t* puVar2 = reinterpret_cast<std::uint32_t*>(
            0x00881a4cu + static_cast<unsigned>(iVar1));
        int iVar3 = 0x12;
        do {
            *puVar2  = 0xffffffffu;
            puVar2  += 0x10;
            iVar3   -= 1;
        } while (iVar3 != 0);
    }

    // Zero per-slot entity table fields (0x341 stride).                 [0x0046bcb5]
    *reinterpret_cast<std::uint32_t*>(0x008815b4u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815b8u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815bcu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815d4u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815d0u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815ccu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815c8u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815c4u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815c0u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815ecu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815e8u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815e4u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815e0u + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815dcu + static_cast<unsigned>(iVar1)) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008815d8u + static_cast<unsigned>(iVar1)) = 0u;

    return 1u; // success [0x0046bcd9]
}

RH_ScopedInstall(VehiclePhysicsStateReset, 0x0046baa0);
