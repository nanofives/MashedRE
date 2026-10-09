// Mashed RE — D-11073 leg 1b: the race sub-state machine's phases 4 and 5.
// Brief: re/BRIEF_D11073.md §2 "Leg 1b"; pre-registration:
// verify/d3_gatefire_20261008/PREREG_LEG1B.md sections 3-5.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Listed in BOTH exe_sources.rsp and asi_sources.rsp. One body per RVA:
//   0x0040dbd0  SubStateEnter4           (123 B)  FUN_004111c0 case 4
//   0x004103a0  SubStateHeldCountdown5   (360 B)  FUN_004111c0 case 5, units in EAX
// Listings: verify/d3_gatefire_20261008/leg1b_decomp.txt (decomp_pc.py, read-only pool
// clone) and a capstone pass over original/MASHED.exe.unpatched (both 2026-10-09).
//
// TWO ARMS, one body. The .asi (diff-original reference) calls every callee the original
// calls, at its RVA. Under /DMASHED_STANDALONE (exe only) each callee resolves to the port:
//   FUN_0041d910 / 0041da90 / 0041b520 / 0046baa0 / 0046c750 / 0046c730 -> the moved
//       bodies in Race/SubStateCallees.cpp (record-routed, PREREG_LEG1B.md section 1)
//   FUN_0046d7f0 / 0046d780 -> Vehicle/LaunchRevCharge.cpp (both targets already)
//   FUN_00418860 (AI tick) and FUN_0046c7b0 (car alive) -> the SubStateHost hooks the
//       TrackRenderer installs. car_alive is the port's Ai s_host.car_alive vector, a NAMED
//       DEVIATION (BLOCKER 3): FUN_0046c7b0 reads record +0x4, which the port does not keep
//       in step with g_aib.alive (RESULT_LEG0.md section 3).
//   FUN_0042f6a0 / 0042f500 -> their one-load bodies at the absolutes DAT_0067e9fc /
//       DAT_0067ea64 (both 0 in the exe, L1B_INPUTS_SCORE.txt).
//   FUN_00426c90 -> NOT CALLED. Named PARTIAL: it calls through [DAT_0063d7e4]+0x40, and
//       DAT_0063d7e4 is 0 in the exe (would AV at 0x40). On the ORIGINAL the slot reads 0 on
//       106/106 phase-5 samples (o_leg1b.csv), so there it takes the no-call arm.
//   FUN_00429860 / 00430820 / 0040e560 / 00429840 / 00411d60 / 00411ae0 -> only reached when
//       FUN_0042f6a0() == 2; DAT_0067e9fc is 0 in the exe, 10 in the original, so neither
//       side enters them. The exe arm does not call them (a raw call would leave the mapped
//       text window, exe_main.cpp:8346); the e9fc test is kept so the branch structure is
//       the original's.
//   FUN_0041e080 / FUN_0045b350 (FUN_0040dbd0's other two arms) -> same reason: not reached
//       on either side's measured inputs; the exe arm logs and skips them.

#include "SubStateMachine.h"
#include "SubStateCallees.h"
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace mashed_re { namespace Vehicle {
#ifdef MASHED_STANDALONE
extern unsigned char g_launchAccelByte[16];
#endif
} }

extern "C" int __cdecl BoundedTableSignClamp46d7f0(unsigned idx, int val);   // 0x0046d7f0
extern "C" int __cdecl LaunchRevRelease46d780(int car);                       // 0x0046d780

namespace {

inline std::int32_t& G32(std::uintptr_t a) { return *reinterpret_cast<std::int32_t*>(a); }

#ifdef MASHED_STANDALONE
SubStateHost s_ssHost = {};

void ExeOnce(const char* what) {
    if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
        std::fprintf(lf, "[substate] UNEXPECTED arm reached in the exe: %s (not ported, skipped)\n", what);
        std::fclose(lf);
    }
}
#endif

// FUN_0042f6a0: `return DAT_0067e9fc;`   FUN_0042f500: `return DAT_0067ea64;`
// (RESULT_LEG0.md section 3). In the asi the original is called so a hooked copy is honoured.
inline int RoundSubMode() {
#ifdef MASHED_STANDALONE
    return G32(0x0067e9fcu);
#else
    return reinterpret_cast<int(__cdecl*)()>(0x0042f6a0u)();
#endif
}
inline int TeamPlay() {
#ifdef MASHED_STANDALONE
    return G32(0x0067ea64u);
#else
    return reinterpret_cast<int(__cdecl*)()>(0x0042f500u)();
#endif
}
inline int CarAlive(int v) {                                   // FUN_0046c7b0
#ifdef MASHED_STANDALONE
    return s_ssHost.car_alive ? s_ssHost.car_alive(v) : 0;
#else
    return reinterpret_cast<int(__cdecl*)(int)>(0x0046c7b0u)(v);
#endif
}

} // namespace

#ifdef MASHED_STANDALONE
void SubState_SetHost(const SubStateHost& h) { s_ssHost = h; }
#endif

// ===========================================================================
// 0x0040dbd0  SubStateEnter4  (FUN_0040dbd0, 123 B, C2; caller FUN_004111c0 case 4 at
// 0x004112a5, which pushes ESI (units) and pops it at 0x004112aa: the body reads no arg).
//
//   FUN_0041d910();                              zero DAT_0063d584 / DAT_0063d588
//   switch (FUN_0042f6a0()) {
//   case 2:  FUN_0041e080(); DAT_0063ba8c = 5; return;
//   default: if (FUN_0042f500() != 0) { FUN_0045b350(); DAT_0063ba8c = 5; return; } break;
//   case 6: case 10:
//            if (FUN_0042f500() != 0 && DAT_007f0fd0 != 2) { FUN_0045b350(); ...5; return; }
//   }
//   FUN_0041b520(); DAT_0063ba8c = 5;
// ===========================================================================

// 0x0040dbd0
extern "C" __declspec(dllexport) void __cdecl SubStateEnter4(void)
{
    Clear63d584Pair();                                          // FUN_0041d910
    const int sub = RoundSubMode();                             // FUN_0042f6a0
    if (sub == 2) {
#ifdef MASHED_STANDALONE
        ExeOnce("FUN_0040dbd0 case 2 -> FUN_0041e080");
#else
        reinterpret_cast<void(__cdecl*)()>(0x0041e080u)();      // FUN_0041e080
#endif
        G32(0x0063ba8cu) = 5;
        return;
    }
    if (sub == 6 || sub == 10) {
        if (TeamPlay() != 0 && G32(0x007f0fd0u) != 2) {         // FUN_0042f500, DAT_007f0fd0
#ifdef MASHED_STANDALONE
            ExeOnce("FUN_0040dbd0 case 6/10 -> FUN_0045b350");
#else
            reinterpret_cast<void(__cdecl*)()>(0x0045b350u)();  // FUN_0045b350
#endif
            G32(0x0063ba8cu) = 5;
            return;
        }
    } else {
        if (TeamPlay() != 0) {                                  // default arm
#ifdef MASHED_STANDALONE
            ExeOnce("FUN_0040dbd0 default -> FUN_0045b350");
#else
            reinterpret_cast<void(__cdecl*)()>(0x0045b350u)();
#endif
            G32(0x0063ba8cu) = 5;
            return;
        }
    }
    Update41b520();                                             // FUN_0041b520
    G32(0x0063ba8cu) = 5;
}
RH_ScopedInstall(SubStateEnter4, 0x0040dbd0);

// ===========================================================================
// 0x004103a0  SubStateHeldCountdown5  (FUN_004103a0, 360 B, C2; caller FUN_004111c0 case 5:
// 0x004112b5 mov esi,[esp+0xc]; 0x004112b9 mov eax,esi; 0x004112bb call 0x4103a0).
// `units` arrives in EAX (0x004103a4 mov ebx,eax). Listing, cited per statement below.
// ===========================================================================

extern "C" void __cdecl SubStateHeldCountdown5_C(int units)
{
    float t = 0.0f;                                             // 0x004103a6 [esp+0xc] = 0

    // 0x004103ae..0x004103ca: FUN_0040e560 only when FUN_00429860() != 0 &&
    // FUN_00430820() == 0 && FUN_0042f6a0() == 2.
#ifdef MASHED_STANDALONE
    if (RoundSubMode() == 2) ExeOnce("FUN_004103a0 0x004103ca FUN_0040e560 path");
#else
    if (reinterpret_cast<int(__cdecl*)()>(0x00429860u)() != 0 &&
        reinterpret_cast<int(__cdecl*)()>(0x00430820u)() == 0 &&
        RoundSubMode() == 2)
        reinterpret_cast<void(__cdecl*)()>(0x0040e560u)();
#endif

    G32(0x005f29b8u) = G32(0x005f29b8u) - units;                // 0x004103e3 / 0x004103f0
    G32(0x007f1014u) = (G32(0x007f1018u) < G32(0x007f0ff4u)) ? 1 : 0;   // 0x004103e5..0x004103f6

    // 0x004103e7..0x00410406: FUN_00426c90(**(PTR_PTR_005f2770 + DAT_0063ba78*4)).
#ifndef MASHED_STANDALONE
    {
        const std::uintptr_t tbl = static_cast<std::uintptr_t>(G32(0x005f2770u));
        const std::uintptr_t p   = static_cast<std::uintptr_t>(
            G32(tbl + static_cast<std::uintptr_t>(G32(0x0063ba78u)) * 4u));
        reinterpret_cast<void(__cdecl*)(std::uint32_t)>(0x00426c90u)(
            static_cast<std::uint32_t>(G32(p)));                // 0x00410406
    }
#endif   // exe: named PARTIAL, measured no-op on the original (file header)

#ifdef MASHED_STANDALONE
    if (s_ssHost.ai_tick) s_ssHost.ai_tick();                       // FUN_00418860 (0x0041040e)
#else
    reinterpret_cast<void(__cdecl*)()>(0x00418860u)();          // 0x0041040e
#endif

    // 0x00410413..0x00410450: per car with a non-zero slot-table entry and alive == 1.
    for (int v = 0, off = 0x34; off < 0x44; off += 4, ++v) {
        const std::uintptr_t tbl = static_cast<std::uintptr_t>(G32(0x005f2770u));   // 0x00410420
#ifdef MASHED_STANDALONE
        if (!tbl) continue;                                     // exe-only guard (pre-seed table)
#endif
        if (G32(tbl + static_cast<std::uintptr_t>(off)) == 0) continue;            // 0x00410425
        if (CarAlive(v) != 1) continue;                         // 0x0041042c / 0x00410434
        VehiclePhysicsStateReset(static_cast<std::uint32_t>(v));   // 0x0041043a
#ifdef MASHED_STANDALONE
        // FUN_0046d7f0 reads the car's cooked accel byte at 0x007f103c + slot*0x4c
        // (0x0046d801..0x0046d80a). The exe arm of that body reads g_launchAccelByte[v];
        // load it from the same bytes for the AI cars, and from the player's cooked accel
        // (the host's hook, the stand-in's binding) for v == 0.
        {
            unsigned char b = 0;
            if (v == 0) {
                b = s_ssHost.player_accel_byte ? s_ssHost.player_accel_byte() : 0;
            } else {
                const int slot = G32(0x007f1a14u + static_cast<std::uintptr_t>(v) * 0x10u);
                if (slot >= 0 && slot < 16)
                    b = *reinterpret_cast<const unsigned char*>(
                        0x007f103cu + static_cast<std::uintptr_t>(slot) * 0x4cu);
            }
            mashed_re::Vehicle::g_launchAccelByte[v] = b;
        }
#endif
        BoundedTableSignClamp46d7f0(static_cast<unsigned>(v), units);   // 0x00410441 (v, EAX)
    }

    DeltaTimeOutGet(reinterpret_cast<std::uint32_t*>(&t));      // 0x00410457 FUN_0041da90(&t)
    std::uint32_t kbits = 0x3fee147bu;                          // _DAT_005ccdf4 = 1.86f
    float k;
    std::memcpy(&k, &kbits, sizeof k);
    // fcomp + `test ah,1` + jne skip: skipped when C0 is set, i.e. t < k OR unordered (NaN).
    if (t >= k) {                                               // 0x0041045c..0x0041046e
        // 0x00410470..0x004104be: per alive car, release + copy into 0x007f0a04 + v*12.
        std::uintptr_t out = 0x007f0a08u;                       // 0x00410472
        for (int v = 0, off = 0x34; off < 0x44; off += 4, ++v, out += 12u) {
            const std::uintptr_t tbl = static_cast<std::uintptr_t>(G32(0x005f2770u));
#ifdef MASHED_STANDALONE
            if (!tbl) continue;
#endif
            if (G32(tbl + static_cast<std::uintptr_t>(off)) == 0) continue;        // 0x00410486
            if (CarAlive(v) != 1) continue;                     // 0x0041048d / 0x00410495
            LaunchRevRelease46d780(v);                          // 0x0041049b
            G32(out - 4u) = static_cast<std::int32_t>(EntityVelocityCounterGet(v)); // 0x004104a7
            G32(out)      = static_cast<std::int32_t>(EntityDamageStateGet(v));     // 0x004104b2
        }
        G32(0x005f29b8u) = 0;                                   // 0x004104c0
        if (RoundSubMode() == 2) {                              // 0x004104ca
#ifdef MASHED_STANDALONE
            ExeOnce("FUN_004103a0 0x004104d4 FUN_00429840/FUN_00411d60 path");
#else
            reinterpret_cast<void(__cdecl*)(int)>(0x00429840u)(1);   // 0x004104d6
            reinterpret_cast<void(__cdecl*)()>(0x00411d60u)();       // 0x004104de
#endif
        }
        G32(0x0063ba8cu) = 6;                                   // 0x004104e3
#ifdef MASHED_STANDALONE
        if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
            std::uint32_t tb; std::memcpy(&tb, &t, sizeof tb);
            std::fprintf(lf, "[substate] exit d584=%d d588_read=%.9g (0x%08x) -> DAT_0063ba8c=6\n",
                         G32(0x0063d584u), t, tb);
            std::fclose(lf);
        }
#endif
    }
    if (RoundSubMode() == 2) {                                  // 0x004104ed
#ifdef MASHED_STANDALONE
        ExeOnce("FUN_004103a0 0x004104fe FUN_00411ae0 path");
#else
        reinterpret_cast<void(__cdecl*)(int, int)>(0x00411ae0u)(0, 0);   // 0x004104fe
#endif
    }
}

// 0x004103a0
// Register-arg entry: units in EAX. EBX/ESI/EDI are preserved by the cdecl C body, EAX/ECX/EDX
// are clobbered as the original clobbers them; the caller (0x004112c0) next reads only ESI.
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl SubStateHeldCountdown5(void)
{
    __asm {
        push eax
        call SubStateHeldCountdown5_C
        add  esp, 4
        ret
    }
}
RH_ScopedInstall(SubStateHeldCountdown5, 0x004103a0);
