// Mashed RE — WS-H2 installed-hook C4 lane: the race scoring trio.
//
// .asi-ONLY verbatim ports of the three race-scoring state machines, installed
// at their original RVAs via RH_ScopedInstall so the inline-JMP runs live during
// a canonical race (the WS-H2 C4 path; re/analysis/WS_H2_C4_LANE_FINDINGS_2026-06-16.md).
// These read/write the ORIGINAL global score arrays (DAT_008a94e0…) directly —
// they are NOT the standalone-adapted TrackRenderer scoring (which uses its own
// arrays); this file is excluded from the exe target (build.bat .asi list only).
//
// Anchored to MASHED.exe SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// Decompiled read-only from Mashed_pool13 (2026-06-16). Every constant/stride is
// byte-verified against the listing (strides resolved from the disassembly:
// score arrays stride 4, team array stride 16 @+0, event-ring delta stride 32).
//
// Callees are forwarded to their original RVA. Where a callee is itself hooked
// (FUN_0040b290 here; FUN_0046c700/FUN_00431d80 in SplashGameMode_t5.cpp), the
// forward routes through the live inline-JMP — correct composition.
//
// Debug stdout calls (FUN_004a2cbd, the wprintf at 0x0040eee0 format strings
// 0x5ccdcc/0x5ccdb4/…) are SIDE-EFFECT-ONLY and omitted — they do not touch game
// state / telemetry, and forwarding a variadic narrow/wide printf adds ABI risk
// for zero bit-identity value. Their call sites are cited inline.

#include "../Core/HookSystem.h"

#include <cstdint>

namespace {

// ── absolute-address accessors (byte-offset arithmetic; stride byte-verified) ──
inline std::int32_t& I32(std::uintptr_t a) { return *reinterpret_cast<std::int32_t*>(a); }
inline float&        F32(std::uintptr_t a) { return *reinterpret_cast<float*>(a); }

// scalar game-state globals
inline std::int32_t& GameMode()  { return I32(0x007f0fd0); }  // DAT_007f0fd0
inline std::int32_t& PrimaryPl() { return I32(0x007f0fd4); }  // DAT_007f0fd4
inline std::int32_t& ModePl8()   { return I32(0x007f0fd8); }  // DAT_007f0fd8
inline std::int32_t& NetFlag()   { return I32(0x007f1014); }  // DAT_007f1014
inline std::int32_t& CtxWord()   { return I32(0x007f1030); }  // DAT_007f1030
inline std::int32_t& Participants() { return I32(0x008a94d0); }  // DAT_008a94d0
inline std::int32_t& Slots()     { return I32(0x005f2770); }  // PTR_PTR_005f2770

// per-car arrays (stride 4)
inline std::int32_t& Score(int i)     { return I32(0x008a94e0 + i * 4); }  // DAT_008a94e0
inline std::int32_t& PrevScore(int i) { return I32(0x008a9570 + i * 4); }  // DAT_008a9570
inline std::int32_t& DeltaDisp(int i) { return I32(0x008a9520 + i * 4); }  // DAT_008a9520
inline std::int32_t& PostClamp(int i) { return I32(0x008a9500 + i * 4); }  // DAT_008a9500
inline std::int32_t& Timer(int i)     { return I32(0x008a9510 + i * 4); }  // DAT_008a9510
inline std::int32_t& ElimOrder(int i) { return I32(0x008a94c0 + i * 4); }  // DAT_008a94c0
inline std::int32_t& TeamId(int i)    { return I32(0x007f1a18 + i * 16); } // DAT_007f1a18 (SHL 4)

// event circular buffer (per-slot strides byte-verified at 0x0040b39f..0x0040b3cd)
inline std::int32_t& RingType(int i)  { return I32(0x007e9de4 + i * 4); }
inline std::int32_t& RingCtx(int i)   { return I32(0x007ea1e4 + i * 4); }
inline std::int32_t& RingCar(int i)   { return I32(0x007ea5e4 + i * 4); }
inline std::int32_t& RingDelta(int i) { return I32(0x007ea9e4 + i * 32); }  // SHL 5
inline std::int32_t& RingPtr()        { return I32(0x007e9de0); }

// slot-active probe: *(int*)(*(int*)0x5f2770 + byte_off)
inline std::int32_t SlotActive(int byte_off) {
    return *reinterpret_cast<std::int32_t*>(Slots() + byte_off);
}

// ── callee forwarders (all __cdecl; routes through any live hook at the RVA) ──
template <class R> using F0 = R(__cdecl*)();
template <class R> using F1 = R(__cdecl*)(int);
inline int   FUN_0040e340()       { return reinterpret_cast<F0<int>>(0x0040e340)(); }
inline int   FUN_0040e350()       { return reinterpret_cast<F0<int>>(0x0040e350)(); }
inline int   FUN_0040e370(int i)  { return reinterpret_cast<F1<int>>(0x0040e370)(i); }
inline int   FUN_0042f500()       { return reinterpret_cast<F0<int>>(0x0042f500)(); }
inline int   FUN_0042f6a0()       { return reinterpret_cast<F0<int>>(0x0042f6a0)(); }
inline int   FUN_0046c7b0(int i)  { return reinterpret_cast<F1<int>>(0x0046c7b0)(i); }
inline int   FUN_0040e470(int i)  { return reinterpret_cast<F1<int>>(0x0040e470)(i); }
inline int   FUN_0040b6d0(int i)  { return reinterpret_cast<F1<int>>(0x0040b6d0)(i); }
inline float FUN_00408ad0(int i)  { return reinterpret_cast<F1<float>>(0x00408ad0)(i); }
inline float FUN_00408a50(int i)  { return reinterpret_cast<F1<float>>(0x00408a50)(i); }
inline void  FUN_00408a70(int i, float f) {
    reinterpret_cast<void(__cdecl*)(int, float)>(0x00408a70)(i, f);
}
inline void  FUN_00422fd0(int i)  { reinterpret_cast<void(__cdecl*)(int)>(0x00422fd0)(i); }
inline int   FUN_0046c700(int a, int b) {
    return reinterpret_cast<int(__cdecl*)(int, int)>(0x0046c700)(a, b);   // hooked: EntityScoreFieldAdd
}
inline int   FUN_00431d80()       { return reinterpret_cast<F0<int>>(0x00431d80)(); }  // hooked: TiebreakFlagGet
inline void  FUN_0040d590(int a, int b, int c, int d) {
    reinterpret_cast<void(__cdecl*)(int, int, int, int)>(0x0040d590)(a, b, c, d);
}
// 0x00410510-only callees
inline int   FUN_00417740(int i)  { return reinterpret_cast<F1<int>>(0x00417740)(i); }
inline float FUN_00417730(int i)  { return reinterpret_cast<F1<float>>(0x00417730)(i); }
inline int   FUN_00405890()       { return reinterpret_cast<F0<int>>(0x00405890)(); }
inline void  FUN_0046cbb0(int a, int* b, void* c) {
    reinterpret_cast<void(__cdecl*)(int, int*, void*)>(0x0046cbb0)(a, b, c);
}
inline void  FUN_0045bed0()       { reinterpret_cast<F0<void>>(0x0045bed0)(); }
inline void  FUN_0045bf30()       { reinterpret_cast<F0<void>>(0x0045bf30)(); }
inline void  FUN_00458f80(int i)  { reinterpret_cast<void(__cdecl*)(int)>(0x00458f80)(i); }
inline void  FUN_00448700(int a, int b) {
    reinterpret_cast<void(__cdecl*)(int, int)>(0x00448700)(a, b);
}
inline int   FUN_00426c00()       { return reinterpret_cast<F0<int>>(0x00426c00)(); }
inline void  FUN_0044df80(int i)  { reinterpret_cast<void(__cdecl*)(int)>(0x0044df80)(i); }

// forwarder to the score adder (routes through this file's own installed hook)
inline void  FUN_0040b290(int a, unsigned b) {
    reinterpret_cast<void(__cdecl*)(int, unsigned)>(0x0040b290)(a, b);
}

}  // namespace

// ===========================================================================
// ScoreAdd_0040b290  --  FUN_0040b290(int car, uint delta)  (409 B)
// Score adder: mode 1/2 delta filters, prev-snapshot, signed display, network
// clamp, event-ring write (when FUN_0040e350() ∉ {1..5}), accumulate + 6000 ms
// timer + floor-at-0. NOTE the x87 float caveat does NOT apply — pure integer.
// ===========================================================================
extern "C" __declspec(dllexport) void __cdecl ScoreAdd_0040b290(int param_1, unsigned int param_2) {
    int iVar3, iVar4, iVar5;
    bool bVar6;

    if (GameMode() == 1) {                                  // 0x0040b290
        if ((int)param_2 < 0) param_2 = 0;                  // 0x0040b2a4
        iVar4 = 0; iVar5 = 0;
        do {
            iVar3 = FUN_0040e370(iVar5);                    // 0x0040b2b1
            if (iVar3 != 0) iVar4 = iVar4 + 1;
            iVar5 = iVar5 + 1;
        } while (iVar5 < 4);
        if (iVar4 == 4) {                                   // 0x0040b2c4
            iVar4 = FUN_0042f500();                          // 0x0040b2ca
            if (iVar4 == 0) param_2 = (unsigned)(param_2 == 2);
        }
        iVar4 = FUN_0042f500();                              // 0x0040b2db
        if (iVar4 == 0) bVar6 = (param_1 == PrimaryPl());    // 0x0040b300
        else            bVar6 = (TeamId(PrimaryPl()) == TeamId(param_1));  // 0x0040b2e4
        if (!bVar6) param_2 = 0;                             // 0x0040b308
    }
    if (GameMode() == 2) {                                   // 0x0040b30a
        if ((int)param_2 < 0) param_2 = 0;
        if (TeamId(param_1) == 1) param_2 = 0;               // 0x0040b31e
        iVar4 = FUN_0040e340();                              // 0x0040b329
        if (iVar4 == 4) param_2 = (unsigned)(param_2 == 2);
        else if (param_2 == 2) param_2 = 1;                  // 0x0040b344
    }
    PrevScore(param_1) = Score(param_1);                     // 0x0040b349/0x0040b350
    bVar6 = (NetFlag() != 0);                                // 0x0040b357
    DeltaDisp(param_1) = (int)param_2;                       // 0x0040b360
    if (bVar6 && ((int)param_2 < 0)) param_2 = 0;            // 0x0040b367
    PostClamp(param_1) = (int)param_2;                       // 0x0040b36f
    iVar4 = FUN_0040e350();                                  // 0x0040b376
    int uVar2 = CtxWord();                                   // 0x0040b39a (read before branch)
    if (iVar4 != 1 && iVar4 != 2 && iVar4 != 3 && iVar4 != 4 && iVar4 != 5) {
        int wp = RingPtr();                                  // 0x0040b394
        RingType(wp)  = 0x2ff01;                             // 0x0040b39f
        RingCtx(wp)   = uVar2;                               // 0x0040b3b0
        RingCar(wp)   = param_1;                             // 0x0040b3bd
        RingDelta(wp) = (int)param_2;                        // 0x0040b3cd (SHL 5)
        RingPtr() = RingPtr() + 1;                           // 0x0040b3d8
        if (0xff < RingPtr()) RingPtr() = 0;                 // 0x0040b3d9
    }
    Score(param_1) = Score(param_1) + (int)param_2;          // 0x0040b3eb
    Timer(param_1) = 6000;                                   // 0x0040b3f2 (0x1770)
    if (Score(param_1) < 0) Score(param_1) = 0;              // 0x0040b3fd
}

RH_ScopedInstall(ScoreAdd_0040b290, 0x0040b290);

// ===========================================================================
// Helper: the wrap-adjusted progress comparison "is A behind B?" used
// throughout FUN_0040eee0 / the original 0x0040f… straggler picks.
//   a = (float)FUN_00408ad0(A); b = FUN_00408ad0(B); (only A is (float)-cast)
//   window: 80.0 (_DAT_005cc730), 20.0 (_DAT_005ccd6c), 100.0 (_DAT_005cc568)
//   returns true when a <= b  (A is the straggler / further behind)
// The original compares on the x87 80-bit stack. The .asi target builds WITHOUT
// /arch:SSE2, so MSVC emits x87 for these float +/-/<= as well (the same property
// that let WS-A2 bit-match the RW-math leaves; [[project-wsa2-rwmath-bitident]]).
// The (float)-cast on `a` forces the 32-bit store/reload the original does. So
// this is bit-exact for the progress compares; the integer score writes are
// trivially exact. Cited: the repeated 0x0040f… straggler-pick blocks.
// ===========================================================================
namespace {
bool ProgBehind(int A, int B) {
    float a = (float)FUN_00408ad0(A);
    float b = FUN_00408ad0(B);
    const float c80 = F32(0x005cc730), c20 = F32(0x005ccd6c), c100 = F32(0x005cc568);
    if ((a <= c80) || (c20 <= b)) {
        if ((c80 < b) && (a < c20)) b = b - c100;
    } else {
        a = a - c100;
    }
    return a <= b;
}
}  // namespace

// ===========================================================================
// ScoreElim_0040eee0  --  FUN_0040eee0(int car, int delta)  (3356 B)
// Round-end elimination scoring. Verbatim transcription of the pool13 decomp.
// Dispatch on Participants() (DAT_008a94d0): ==2 / ==3 / ==4 team+FFA paths +
// the iVar5==1 progress-equalize tail. delta is the caller's score weight (=1
// from 0x00410d10). Debug wprintf(0x004a2cbd) calls omitted (stdout-only).
// ===========================================================================
extern "C" __declspec(dllexport) std::uint32_t __cdecl ScoreElim_0040eee0(int param_1, int param_2) {
    int iVar2, iVar3, iVar4, iVar5, iVar6, iVar7;
    int* piVar8;
    bool bVar9;
    std::uint32_t uVar12;
    int local_10, local_8;
    float fVar1;   // hoisted (target of goto LAB_0040fbbb crosses its scope)

    iVar7 = 0;
    local_8 = 0;
    iVar2 = FUN_0042f6a0();                                  // 0x0040eeed (race type)
    // wprintf("car reset=%d, score=%d\n", param_1, param_2)  -- 0x0040ef00 (omitted)
    iVar5 = 0; iVar6 = 0x34;
    do {                                                     // count alive  0x0040ef20
        if ((SlotActive(iVar6) != 0) && (iVar3 = FUN_0046c7b0(iVar7), iVar3 == 1)) iVar5 = iVar5 + 1;
        iVar6 = iVar6 + 4; iVar7 = iVar7 + 1;
    } while (iVar6 < 0x44);
    // wprintf("numcarsremaining=%d\n", iVar5)  -- 0x0040ef45 (omitted)
    if (iVar5 == 1) {                                        // 0x0040ef56
        iVar7 = 0; iVar6 = 0x34;
        do {
            if ((SlotActive(iVar6) != 0) && (iVar3 = FUN_0046c7b0(iVar7), iVar3 == 1)) local_8 = iVar7;
            iVar6 = iVar6 + 4; iVar7 = iVar7 + 1;
        } while (iVar6 < 0x44);
    }
    iVar7 = 0;
    iVar6 = ElimOrder(0);                                    // append param_1 at first -1 slot
    while (iVar6 != -1) {
        iVar6 = ElimOrder(iVar7 + 1);
        iVar7 = iVar7 + 1;
    }
    bVar9 = Participants() == 2;
    ElimOrder(iVar7) = param_1;
    if (bVar9) {                                             // DAT_008a94d0 == 2
        if (iVar5 == 1) {
            FUN_0040b290(param_1, 0xffffffff);
            FUN_0040b290(local_8, 1);
        } else {
            FUN_0040b290(0, 0);
            FUN_0040b290(1, 0);
        }
    }
    if (Participants() == 3) {                               // DAT_008a94d0 == 3
        iVar6 = FUN_0042f500();
        if (iVar6 == 0) {
            if (iVar5 == 2) {
                FUN_0040b290(param_1, (unsigned)(-param_2));
                local_10 = iVar7;
                if (GameMode() == 2) {
                    bVar9 = false; iVar6 = 0;
                    do {
                        if ((ModePl8() != iVar6) && (iVar3 = FUN_0046c7b0(iVar6), iVar3 == 1)) {
                            if (bVar9) {
                                FUN_00422fd0(iVar6);
                                FUN_0040b290(iVar6, 0);
                                FUN_0040b290(iVar7, 0);
                            } else { bVar9 = true; iVar7 = iVar6; }
                        }
                        iVar6 = iVar6 + 1; local_10 = iVar7;
                    } while (iVar6 < 4);
                }
                iVar7 = local_10;
                if ((((iVar2 == 3) || (iVar2 == 4)) || (iVar2 == 5) || (iVar2 == 10)) &&
                    ((iVar6 = FUN_0042f500(), iVar6 == 0 && (GameMode() == 0)))) {
                    bVar9 = false; iVar6 = 0; iVar3 = 0x34;
                    do {
                        if (((SlotActive(iVar3) != 0) && (iVar7 = FUN_0046c7b0(iVar6), iVar7 == 1)) &&
                            (iVar7 = FUN_0040e470(iVar6), iVar7 == 2)) {
                            if (bVar9) {
                                if (ProgBehind(iVar6, local_10)) {
                                    FUN_00422fd0(iVar6); FUN_0040b290(local_10, (unsigned)param_2); iVar7 = iVar6;
                                } else {
                                    FUN_00422fd0(local_10); FUN_0040b290(iVar6, (unsigned)param_2); iVar7 = local_10;
                                }
                                FUN_0040b290(iVar7, 0);
                            } else { bVar9 = true; local_10 = iVar6; }
                        }
                        iVar3 = iVar3 + 4; iVar6 = iVar6 + 1; iVar7 = local_10;
                    } while (iVar3 < 0x44);
                }
            } else if (iVar5 == 1) {
                FUN_0040b290(param_1, 0);
                FUN_0040b290(local_8, 1);
            }
        } else if (iVar5 == 2) {                             // teams, 2 remaining
            iVar6 = -1; local_10 = -1; iVar3 = 0;
            do {
                if ((SlotActive(iVar3 * 4 + 0x34) != 0) && (iVar4 = FUN_0046c7b0(iVar3), iVar4 == 1)) {
                    if (iVar6 == -1) { local_10 = TeamId(iVar3); iVar6 = local_10; iVar7 = iVar3; }
                    else {
                        iVar4 = iVar3 * 4;
                        if (iVar6 == TeamId(iVar3)) {
                            if (ProgBehind(iVar3, iVar7)) FUN_00422fd0(iVar3);
                            else { FUN_00422fd0(iVar7); iVar3 = iVar7; }
                            FUN_0046c700(iVar3, 1);
                            iVar4 = *reinterpret_cast<int*>(0x007f1a18 + iVar4 * 4);  // (&DAT_007f1a18)[iVar4]
                            iVar3 = 0; piVar8 = reinterpret_cast<int*>(0x007f1a18);
                            do {
                                if (piVar8[-1] != -1) {
                                    uVar12 = (*piVar8 == iVar4) ? 1u : 0xffffffffu;
                                    FUN_0040b290(iVar3, uVar12);
                                }
                                piVar8 = piVar8 + 4; iVar3 = iVar3 + 1; iVar6 = local_10;
                            } while ((std::uintptr_t)piVar8 < 0x7f1a58);
                        }
                    }
                }
                iVar3 = iVar3 + 1;
            } while (iVar3 < 4);
        } else if (iVar5 == 1) {                             // teams, 1 remaining
            iVar6 = TeamId(local_8);
            iVar3 = 0; piVar8 = reinterpret_cast<int*>(0x007f1a18);
            do {
                if (piVar8[-1] != -1) {
                    uVar12 = (*piVar8 == iVar6) ? 1u : 0xffffffffu;
                    FUN_0040b290(iVar3, uVar12);
                }
                piVar8 = piVar8 + 4; iVar3 = iVar3 + 1;
            } while ((std::uintptr_t)piVar8 < 0x7f1a58);
        }
    }
    local_10 = iVar7;
    if (Participants() == 4) {                               // DAT_008a94d0 == 4 (the FFA path)
        iVar6 = FUN_0042f6a0();
        if (((iVar6 == 6) && (iVar6 = FUN_00431d80(), iVar6 != 0)) && (iVar5 == 2)) {
            // wprintf("here, numremaining=%d\n", iVar5)  -- (omitted)
            bVar9 = false; iVar6 = 0; iVar7 = 0x34;
            do {
                if (((SlotActive(iVar7) != 0) && (iVar3 = FUN_0046c7b0(iVar6), iVar3 == 1)) &&
                    (iVar3 = FUN_0040e470(iVar6), iVar3 == 2)) {
                    if (bVar9) {
                        if (ProgBehind(iVar6, local_10)) {
                            FUN_00422fd0(iVar6); FUN_0040b290(local_10, 2); iVar3 = iVar6;
                        } else {
                            FUN_00422fd0(local_10); FUN_0040b290(iVar6, 2); iVar3 = local_10;
                        }
                        FUN_0040b290(iVar3, 1);
                    } else { bVar9 = true; local_10 = iVar6; }
                }
                iVar7 = iVar7 + 4; iVar6 = iVar6 + 1;
            } while (iVar7 < 0x44);
        }
        iVar6 = local_10;
        iVar7 = FUN_0042f500();
        if (iVar7 == 0) {                                    // no teams (FFA)
            if (iVar5 == 3) {
                FUN_0040b290(param_1, (unsigned)(param_2 * -2));
                if (GameMode() == 2) {
                    iVar5 = 0; iVar6 = 0;
                    do {
                        if ((ModePl8() != iVar6) && (iVar7 = FUN_0046c7b0(iVar6), iVar7 == 1)) {
                            if (iVar5 == 0) { iVar5 = 1; local_10 = iVar6; }
                            else if (iVar5 == 1) { iVar5 = 2; param_1 = iVar6; }
                            else if (iVar5 == 2) {
                                FUN_00422fd0(iVar6); FUN_0040b290(iVar6, 0);
                                FUN_00422fd0(local_10); FUN_0040b290(local_10, 0);
                                FUN_0040b290(param_1, 0);
                            }
                        }
                        iVar6 = iVar6 + 1;
                    } while (iVar6 < 4);
                }
                if ((((iVar2 == 3) || (iVar2 == 4)) || (iVar2 == 5) || (iVar2 == 10)) &&
                    ((iVar2 = FUN_0042f500(), iVar2 == 0 && (GameMode() == 0)))) {
                    iVar6 = 0; iVar5 = 0; iVar2 = 0x34;
                    do {
                        if (((SlotActive(iVar2) != 0) && (iVar7 = FUN_0046c7b0(iVar5), iVar7 == 1)) &&
                            (iVar7 = FUN_0040e470(iVar5), iVar7 == 2)) {
                            if (iVar6 == 0) { iVar6 = 1; local_10 = iVar5; }
                            else if (iVar6 == 1) { iVar6 = 2; param_1 = iVar5; }
                            else if (iVar6 == 2) { FUN_0040d590(iVar5, local_10, param_1, param_2); }
                        }
                        iVar2 = iVar2 + 4; iVar5 = iVar5 + 1;
                    } while (iVar2 < 0x44);
                    return 0;
                }
            } else if (iVar5 == 2) {
                FUN_0040b290(param_1, (unsigned)(-param_2));
                if (GameMode() == 2) {
                    bVar9 = false; iVar5 = 0;
                    do {
                        if ((ModePl8() != iVar5) && (iVar6 = FUN_0046c7b0(iVar5), iVar6 == 1)) {
                            if (bVar9) {
                                FUN_00422fd0(iVar5); FUN_0040b290(iVar5, 0); FUN_0040b290(local_10, 0);
                            } else { bVar9 = true; local_10 = iVar5; }
                        }
                        iVar5 = iVar5 + 1;
                    } while (iVar5 < 4);
                }
                if ((((iVar2 == 3) || (iVar2 == 4)) || (iVar2 == 5) || (iVar2 == 10)) &&
                    ((iVar2 = FUN_0042f500(), iVar2 == 0 && (GameMode() == 0)))) {
                    bVar9 = false; iVar2 = 0; iVar5 = 0x34;
                    do {
                        if (((SlotActive(iVar5) != 0) && (iVar6 = FUN_0046c7b0(iVar2), iVar6 == 1)) &&
                            (iVar6 = FUN_0040e470(iVar2), iVar6 == 2)) {
                            if (bVar9) {
                                if (ProgBehind(iVar2, local_10)) {
                                    FUN_00422fd0(iVar2); FUN_0040b290(local_10, 2); iVar6 = iVar2;
                                } else {
                                    FUN_00422fd0(local_10); FUN_0040b290(iVar2, 2); iVar6 = local_10;
                                }
                                FUN_0040b290(iVar6, 1);
                            } else { bVar9 = true; local_10 = iVar2; }
                        }
                        iVar5 = iVar5 + 4; iVar2 = iVar2 + 1;
                    } while (iVar5 < 0x44);
                    return 0;
                }
            } else if (iVar5 == 1) {
                iVar2 = FUN_0040b6d0(param_1);
                if (10 < iVar2) param_2 = 0;
                FUN_0040b290(param_1, (unsigned)param_2);
                FUN_0040b290(local_8, 2);
                goto LAB_0040fbbb;
            }
        } else if (iVar5 == 3) {                             // teams, 3 remaining
            iVar2 = 0; local_10 = -1; iVar5 = 0;
            do {
                if ((SlotActive(iVar5 * 4 + 0x34) != 0) && (iVar7 = FUN_0046c7b0(iVar5), iVar7 == 1)) {
                    if (local_10 == -1) { local_10 = TeamId(iVar5); iVar2 = iVar2 + 1; iVar6 = iVar5; }
                    else if (local_10 == TeamId(iVar5)) {
                        if (iVar2 == 1) param_1 = iVar5;
                        iVar2 = iVar2 + 1;
                        if (iVar2 == 3) {
                            if (ProgBehind(iVar5, iVar6)) {
                                FUN_00422fd0(iVar5);
                                iVar7 = param_1; iVar3 = iVar6;
                                if (!ProgBehind(iVar6, param_1)) goto LAB_0040f69e;
                            } else {
                                FUN_00422fd0(iVar6);
                                iVar7 = iVar5; iVar3 = param_1;
                                if (ProgBehind(iVar5, param_1)) {
                                    LAB_0040f69e: iVar3 = iVar7;
                                }
                            }
                            FUN_00422fd0(iVar3);
                            iVar7 = TeamId(iVar5);
                            iVar5 = 0; piVar8 = reinterpret_cast<int*>(0x007f1a18);
                            do {
                                if (piVar8[-1] != -1) {
                                    iVar3 = param_2;
                                    if (*piVar8 != iVar7) iVar3 = -param_2;
                                    FUN_0040b290(iVar5, (unsigned)iVar3);
                                }
                                piVar8 = piVar8 + 4; iVar5 = iVar5 + 1;
                            } while ((std::uintptr_t)piVar8 < 0x7f1a58);
                        }
                    }
                }
                iVar5 = iVar5 + 1;
            } while (iVar5 < 4);
        } else {
            if (iVar5 == 2) {                                // teams, 2 remaining
                iVar2 = -1; iVar6 = 0; iVar5 = local_10;
                do {
                    if ((SlotActive(iVar6 * 4 + 0x34) != 0) && (iVar7 = FUN_0046c7b0(iVar6), iVar7 == 1)) {
                        if (iVar2 == -1) { iVar2 = TeamId(iVar6); iVar5 = iVar6; local_10 = iVar6; }
                        else {
                            iVar7 = iVar6 * 4;
                            if (iVar2 == TeamId(iVar6)) {
                                if (!ProgBehind(iVar6, iVar5)) iVar6 = iVar5;
                                FUN_00422fd0(iVar6);
                                iVar7 = *reinterpret_cast<int*>(0x007f1a18 + iVar7 * 4);
                                iVar6 = 0; piVar8 = reinterpret_cast<int*>(0x007f1a18);
                                do {
                                    if (piVar8[-1] != -1) {
                                        uVar12 = (*piVar8 == iVar7) ? 1u : 0xffffffffu;
                                        FUN_0040b290(iVar6, uVar12);
                                    }
                                    piVar8 = piVar8 + 4; iVar6 = iVar6 + 1; iVar5 = local_10;
                                } while ((std::uintptr_t)piVar8 < 0x7f1a58);
                            }
                        }
                    }
                    iVar6 = iVar6 + 1;
                } while (iVar6 < 4);
                return 0;
            }
            if (iVar5 == 1) {                                // teams, 1 remaining
                iVar2 = TeamId(local_8);
                iVar5 = 0; piVar8 = reinterpret_cast<int*>(0x007f1a18);
                do {
                    if (piVar8[-1] != -1) {
                        uVar12 = (*piVar8 == iVar2) ? 1u : 0xffffffffu;
                        FUN_0040b290(iVar5, uVar12);
                    }
                    piVar8 = piVar8 + 4; iVar5 = iVar5 + 1;
                } while ((std::uintptr_t)piVar8 < 0x7f1a58);
                goto LAB_0040fbbb;
            }
        }
    } else if (iVar5 == 1) {
        LAB_0040fbbb:
        fVar1 = (float)FUN_00408a50(local_8);
        FUN_00408a70(0, fVar1);
        FUN_00408a70(1, fVar1);
        FUN_00408a70(2, fVar1);
        FUN_00408a70(3, fVar1);
        return 1;
    }
    return 0;
}

RH_ScopedInstall(ScoreElim_0040eee0, 0x0040eee0);

// ===========================================================================
// EvalResult_00410510  --  FUN_00410510(void) -> int  (820 B)
// Match-end evaluator. Scans players for a winning score (>11 when
// Participants()==4), then a switch on GameMode() for the per-mode end
// conditions; on conclusion sets DAT_0063b90c/DAT_0063ba8c=0xb and the result
// state. Returns winner (1-based) / -1 draw / 0 pending. Verbatim from pool13.
// ===========================================================================
extern "C" __declspec(dllexport) int __cdecl EvalResult_00410510(void) {
    bool bVar1;
    int iVar2, iVar3, iVar4, iVar5, iVar6;
    float fVar7;
    int local_c;
    float fStack_8;
    unsigned char local_4[4];

    iVar6 = 0;
    I32(0x0063b90c) = 0;                                     // DAT_0063b90c = 0
    iVar2 = FUN_0042f6a0();
    if (iVar2 == 2) return 0;
    iVar2 = 1;
    do {
        iVar3 = FUN_0040b6d0(iVar2 + -1);
        if ((((Participants() == 2) || (Participants() == 3)) ||
             (iVar4 = FUN_0042f500(), iVar4 != 0)) && (iVar3 == 8)) iVar6 = iVar2;
        if ((GameMode() == 2) && (iVar3 == 8)) iVar6 = iVar2;
        if ((Participants() == 4) && (0xb < iVar3)) iVar6 = iVar2;   // score > 11
        bVar1 = iVar2 < 4;
        iVar2 = iVar2 + 1;
    } while (bVar1);
    if (iVar6 != 0) {
        iVar3 = 0; iVar2 = 0x34;
        do {
            if ((SlotActive(iVar2) != 0) && (iVar4 = FUN_0046c7b0(iVar3), iVar4 == 1)) {
                iVar4 = FUN_0040b6d0(iVar3);
                if (((Participants() == 2) || ((Participants() == 3 || (iVar5 = FUN_0042f500(), iVar5 != 0)))) &&
                    (iVar4 == 8)) iVar6 = iVar3 + 1;
                if ((Participants() == 4) && (0xb < iVar4)) iVar6 = iVar3 + 1;
            }
            iVar2 = iVar2 + 4; iVar3 = iVar3 + 1;
        } while (iVar2 < 0x44);
    }
    switch (GameMode()) {
    case 4:
        iVar6 = FUN_00417740(0); iVar6 = iVar6 + 1; break;
    case 5:
        iVar6 = FUN_0046c7b0(0);
        if (iVar6 == 0) { iVar6 = -1; goto LAB_0041062a; }
        iVar6 = FUN_00405890(); break;
    case 7:
        iVar3 = 0; iVar2 = FUN_0040e340();
        if (0 < iVar2) {
            do {
                fVar7 = FUN_00417730(iVar3);
                if (F32(0x005cc31c) < fVar7) { iVar6 = iVar3 + 1; break; }
                iVar3 = iVar3 + 1; iVar2 = FUN_0040e340();
            } while (iVar3 < iVar2);
        }
        break;
    case 8:
        iVar6 = FUN_00417740(0); iVar6 = iVar6 + 1;
        if (iVar6 != 1) { if (iVar6 == 0) iVar6 = -1; goto LAB_0041062a; }
        break;
    case 9:
        iVar6 = FUN_00417740(0);
        if (iVar6 != -1) {
            LAB_00410801: iVar6 = -1; I32(0x007f0fcc) = 0; goto LAB_0041062a;
        }
        FUN_0046cbb0(0, &local_c, local_4);
        if (local_c != 0) { iVar6 = -1; I32(0x007f0fcc) = 0; goto LAB_0041062a; }
        fStack_8 = FUN_00417730(1);
        fVar7 = FUN_00417730(0);
        if (F32(0x005cc9c8) <= fStack_8 - fVar7) { iVar6 = -1; I32(0x007f0fcc) = 0; goto LAB_0041062a; }
        goto LAB_004107e3;
    case 10:
        FUN_0046cbb0(0, &local_c, local_4);
        if (local_c != 0) { iVar6 = -1; I32(0x007f0fcc) = 0; goto LAB_0041062a; }
        fVar7 = FUN_00417730(0);
        if (fVar7 < F32(0x005cc574)) {
            // (a<b)==(a==b) NaN-aware compare of DAT_007f0fe4 vs DAT_005d757c (case-10 only)
            if ((F32(0x007f0fe4) < F32(0x005d757c)) == (F32(0x007f0fe4) == F32(0x005d757c))) return 0;
            goto LAB_00410801;
        }
        LAB_004107e3: iVar6 = 1; I32(0x007f0fcc) = 1; goto LAB_0041062a;
    }
    if (iVar6 == 0) return 0;
LAB_0041062a:
    FUN_0045bed0();
    FUN_0045bf30();
    FUN_00458f80(0);
    I32(0x0063ba8c) = 0xb;                                   // DAT_0063ba8c = 0xb
    I32(0x0063b90c) = 1;                                     // DAT_0063b90c = 1
    I32(0x0063b910) = 0x442f0000;                            // _DAT_0063b910 (=700.0f)
    I32(0x0063b918) = 0;
    I32(0x0063b91c) = 0;
    if (iVar6 == -1) {
        FUN_00448700(1, 0);
        I32(0x0063b914) = 1;
        I32(0x007f0fcc) = 0;
    } else {
        iVar2 = iVar6 + -1;
        FUN_00448700(1, iVar2);
        I32(0x007f0fcc) = (iVar2 == 0) ? 1 : 0;
        I32(0x0063b914) = iVar2;
    }
    iVar2 = FUN_00426c00();
    if (iVar2 == 0x26) FUN_0044df80(0);
    return iVar6;
}

RH_ScopedInstall(EvalResult_00410510, 0x00410510);
