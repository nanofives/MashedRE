// Mashed RE - HUD in-game dispatcher.
// Analysis notes: re/analysis/hud_ingame_promote_c2/0x0040dfc0.md
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All callees are C2+:
//   0x00426ba0  sub_00426ba0  C2  HUD draw-enable flag getter
//   0x0042f6a0  GetRaceSubMode C3  game sub-mode getter (DAT_0067e9fc)
//   0x0042f500  GetDat0067ea64 C3  discriminator getter
//   0x0041e850  sub_0041e850  C2  sub-mode 2 (both branches)
//   0x0041ded0  sub_0041ded0  C2  modes 4,7,9 (param=1) / mode 8 (param=0)
//   0x0041c300  sub_0041c300  C2  mode 5
//   0x0041a3e0  sub_0041a3e0  C2  mode 10
//   0x0041b630  sub_0041b630  C2  {5/6}-path when FUN_0042f500==0
//   0x0041c0c0  sub_0041c0c0  C2  {5/6}-path when FUN_0042f500!=0
//   0x0041ccc0  sub_0041ccc0  C2  {7}-path default/6/10 when FUN_0042f500==0
//   0x0041d870  sub_0041d870  C2  {7}-path default/6/10 when FUN_0042f500!=0
//   0x0041db80  sub_0041db80  C2  post-switch tail (only on {5/6}-path)
//   0x00403160  sub_00403160  C2  sub-mode 0xb viewport handler

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// 0x0040dfc0  HudIngameDispatch
//
// Entry point for all in-game HUD rendering. Called from FUN_00492e90 in game
// states 3 and 7 (DAT_00771968 at 0x00771968), wrapped in render-state pairs.
//
// Guards:
//   Guard 1: FUN_00426ba0() == 0 -> early return.
//   Guard 2: DAT_0063ba8c < 5    -> early return.
//            DAT_0063ba8c > 6    -> if != 7, early return; else high branch.
//
// DAT_0063ba8c == 7 branch (high):
//   - Dispatches FUN_0042f6a0() as switch discriminant.
//   - Sub-mode 2:         FUN_0041e850(); return.
//   - Sub-modes 3,4,5:    inner switch on DAT_007f0fd0:
//       4,7,9 -> FUN_0041ded0(1); return.
//       5     -> FUN_0041c300(); return.
//       8     -> FUN_0041ded0(0); return.
//       10    -> FUN_0041a3e0(); return.
//       (no default: falls through to outer default)
//   - Sub-modes 6,10:     FUN_0042f500(); if 0 OR DAT_007f0fd0==2 -> FUN_0041ccc0(); return.
//                         else break -> FUN_0041d870(); return.
//   - default:            FUN_0042f500(); if 0 -> FUN_0041ccc0(); return; else break.
//   - After break:        FUN_0041d870(); return.
//
// DAT_0063ba8c in {5, 6} branch (low):
//   Structurally mirrors high branch with different terminal callees.
//   - Sub-mode 2:         FUN_0041e850(); break.
//   - Sub-modes 3,4,5:    inner switch on DAT_007f0fd0:
//       4,7,9 -> FUN_0041ded0(1); break.
//       5     -> FUN_0041c300(); break.
//       8     -> FUN_0041ded0(0); break.
//       10    -> FUN_0041a3e0(); break.
//       default -> goto switchD_caseD_7 (outer default).
//   - Sub-modes 6,10:     FUN_0042f500(); if != 0 -> bVar3=(DAT_007f0fd0==2) -> LAB_0040e0d2.
//                         else -> LAB_0040e0db -> FUN_0041b630(); break.
//   - default / goto target:
//       FUN_0042f500(); bVar3=(iVar1==0).
//       LAB_0040e0d2: if !bVar3 -> FUN_0041c0c0(); break.
//       LAB_0040e0db: FUN_0041b630(); break.
//   - Sub-mode 0xb:       FUN_00403160(); break.
//   Post-switch (unconditional tail):
//     FUN_0042f6a0(); if result == 0xb -> return; else FUN_0041db80().
// ---------------------------------------------------------------------------

// MATCH-S02: the body of 0x0040dfc0 (HudIngameDispatch) moved to Match/MatchS02_0040dfc0.cpp,
// which compiles the byte-matched source re/match/s02/0040dfc0.c.

// ---------------------------------------------------------------------------
// 0x0041b630  HudSlotLoopB630
//
// Called from HudIngameDispatch on the DAT_0063ba8c ∈ {5,6} path when
// FUN_0042f500() returns 0, or on the default/goto path with FUN_0042f500()==0.
//
// Iterates 4 entries of the array at 0x0063c8d0 with stride 0x74 (116 bytes).
// Per entry: reads int32_t at offset +0x6c; if non-zero → calls FUN_0041b340.
// Loop condition: (int32_t)puVar1 < 0x63caa0  (pointer-as-int32 comparison).
//
// FUN_0041b340 is __thiscall-via-EAX: the original loop emits
//   0x0041b63d  MOV EAX, ESI         ; this = current entry pointer
//   0x0041b63f  CALL 0x0041b340
// so the callee reads the per-entry pointer from EAX. We replicate the
// MOV EAX,<entry> with inline asm before each call. Without it the callee
// reads garbage EAX → crash in production (the synthetic main-menu diff
// would still pass because the +0x6c flags are 0 at the menu and the
// callee branch is never taken — see precedent in GameModeCarSelect.cpp
// for the asm thunk pattern used here).
//
// Struct accesses:
//   Base 0x0063c8d0 + n*0x74, offset +0x6c: int32_t enable flag.
// ---------------------------------------------------------------------------

// 0x0041b630
extern "C" __declspec(dllexport) void __cdecl HudSlotLoopB630() {
    std::uint8_t* puVar1 = reinterpret_cast<std::uint8_t*>(0x0063c8d0u);
    constexpr std::uintptr_t kCallee0041b340 = 0x0041b340u;
    do {
        if (*reinterpret_cast<const std::int32_t*>(puVar1 + 0x6c) != 0) {
            // Replicate 0x0041b63d..0x0041b643: MOV EAX, ESI ; CALL 0x0041b340
            const std::uintptr_t fn_addr = kCallee0041b340;
            std::uint8_t* entry = puVar1;
            __asm {
                mov eax, entry
                mov ecx, fn_addr
                call ecx
            }
        }
        puVar1 += 0x74;
    } while (reinterpret_cast<std::int32_t>(puVar1) < static_cast<std::int32_t>(0x0063caa0u));
}

RH_ScopedInstall(HudSlotLoopB630, 0x0041b630);  // re-enabled 2026-05-24 c3-safe

// MATCH-S04: the body of 0x0041ccc0 (HudSlotLoopCcc0) moved to Match/MatchS04_0041c9a0.cpp (which also holds
// the port's first body of its callee 0x0041c9a0, before this called through the original address),
// which compiles the byte-matched source re/match/s04/0041c9a0.c.

// MATCH-S04: the bodies of 0x0041c2d0 (FUN_0041c2d0) and 0x0041c300 (HudDispatchMode5) moved to Match/MatchS04_0041c2d0.cpp,
// which compiles the byte-matched source re/match/s04/0041c2d0.c.

// MATCH-S04: the bodies of 0x0041bc50 (FUN_0041bc50) and 0x0041c0c0 (HudDispatchSlot2) moved to Match/MatchS04_0041bc50.cpp,
// which compiles the byte-matched source re/match/s04/0041bc50.c.

// ---------------------------------------------------------------------------
// 0x0041cc50  HudSlotUpdateCc50   (100 bytes)
//
// The STANDINGS PER-FRAME UPDATE, and the twin of HudSlotLoopCcc0 above: same
// array (0x0063ce20, stride 0x114), same enable flag (+0x110), but it drives the
// per-row UPDATE (FUN_0041c410) where Ccc0 drives the per-row DRAW (FUN_0041c9a0).
// Called from Race::Tick 0x0040fc00 (non-ghost branch 2), C2.
//
// Body, in order:
//   FUN_0040b930(&DAT_0063cde8);      // crown flags: score >= win threshold
//   FUN_0040b9a0(&DAT_0063ce08, 0);   // tied-for-highest-score flags
//   FUN_0040ba00(&DAT_0063cdd4);      // lowest-score flags
//   FUN_0040b970(&DAT_0063cdc0);      // score == 0 flags
//   FUN_0040b540(&DAT_0063cdf8);      // row order, descending by score
//   for each of 4 groups: if (*(group+0x110) != 0) FUN_0041c410();  // ESI = group
//   ++DAT_0063d270;                   // crown-pulse tick
//
// FUN_0041c410 then folds those four flag arrays into bits 0x40/0x80/0x100/0x200
// of each group's enable word at +0x10c, places the row from DAT_005f337c using
// the DAT_0063cdf8 order, and pulses the crown off DAT_0063d270.
// Evidence: re/analysis/race_hud_capture_20260902.md Finding 20.
//
// NOTE on 0x0063d270: it is BOTH the loop's exclusive end bound (0x0063ce20 +
// 4*0x114 == 0x0063d270) AND the tick counter incremented afterwards. The counter
// lives immediately past the end of the group array. Not a typo -- verbatim.
//
// FUN_0041c410 takes `this` in ESI (register convention), unlike FUN_0041c9a0 /
// FUN_0041b340 above which take it in EAX. The original emits
//   MOV ESI,<entry> ; CALL 0x0041c410
// so the asm thunk sets ESI, not EAX. ESI is callee-saved in this ABI, so it is
// pushed/popped around the call -- the EAX/ECX thunks above need no such guard
// because both of those registers are volatile.
//
// All six callees are C2+: 0x0040b930 C3, 0x0040b9a0 C4, 0x0040ba00 C3,
// 0x0040b970 C3, 0x0040b540 C3, 0x0041c410 C2. The five flag-array fills are
// called at their original VAs, which route through our own installed hooks for
// the four that we have ported -- exactly what the game does.
// ---------------------------------------------------------------------------

typedef void (__cdecl *FillFn)(void* out);
typedef void (__cdecl *FillFn2)(void* out, std::int32_t sel);

// 0x0041cc50
extern "C" __declspec(dllexport) void __cdecl HudSlotUpdateCc50() {
    reinterpret_cast<FillFn >(0x0040b930)(reinterpret_cast<void*>(0x0063cde8u));
    reinterpret_cast<FillFn2>(0x0040b9a0)(reinterpret_cast<void*>(0x0063ce08u), 0);
    reinterpret_cast<FillFn >(0x0040ba00)(reinterpret_cast<void*>(0x0063cdd4u));
    reinterpret_cast<FillFn >(0x0040b970)(reinterpret_cast<void*>(0x0063cdc0u));
    reinterpret_cast<FillFn >(0x0040b540)(reinterpret_cast<void*>(0x0063cdf8u));

    std::uint8_t* puVar1 = reinterpret_cast<std::uint8_t*>(0x0063ce20u);
    constexpr std::uintptr_t kCallee0041c410 = 0x0041c410u;
    do {
        if (*reinterpret_cast<const std::int32_t*>(puVar1 + 0x110) != 0) {
            // Replicate MOV ESI,<entry> ; CALL 0x0041c410
            const std::uintptr_t fn_addr = kCallee0041c410;
            std::uint8_t* entry = puVar1;
            __asm {
                push esi
                mov  esi, entry
                mov  ecx, fn_addr
                call ecx
                pop  esi
            }
        }
        puVar1 += 0x114;
    } while (reinterpret_cast<std::int32_t>(puVar1) < static_cast<std::int32_t>(0x0063d270u));

    ++*reinterpret_cast<std::int32_t*>(0x0063d270u);
}

RH_ScopedInstall(HudSlotUpdateCc50, 0x0041cc50);  // C3 2026-09-03 (Finding 20; caller 0x0040fc00 C2, callees C2+)

// ---------------------------------------------------------------------------
// Four leaves of HudIngameDispatch (0x0040dfc0), ported from the bytes of
// original\MASHED.exe.unpatched on 2026-10-12. Before this they had no body:
// HudIngameDispatch called the original RVAs and nothing was installed there.
//
// Two of the callees take their object in EAX (register convention):
//   0x0041c2d0 (C2): 0x0041c2d1 mov esi,eax; then for members [3],[1],[0],[2]
//                    of the EAX block: push member; call [member+0x48];
//                    0x0041c2ee add esp,0x10 (caller-cleaned cdecl calls).
//                    [member+0x48] is a function-pointer FIELD of the member,
//                    not a vtable slot: 0x0041c2d3 mov eax,[esi+0xc] /
//                    0x0041c2d7 call [eax+0x48] dereferences once.
//   0x0041bc50 (C2): 0x0041bc51 mov esi,eax; 29 pairs: flag dword at
//                    rec+X (X in 0x10..0x94), object at rec+X+0xa0, and
//                    push obj; call [obj+0x48] when the flag is non-zero.
// MATCH-S04: 0x0041c2d0 and 0x0041bc50 moved to Match/MatchS04_0041c2d0.cpp and
// Match/MatchS04_0041bc50.cpp (MATCH-S04); FUN_0041c2d0 there is still the
// EAX-convention entry, so HudDispatchMode10 below loads EAX and calls it.
//
// Return value: each original leaves EAX defined at RET, so the ports return
// the same EAX (the flag read on the skip path, else the callee's EAX). No
// caller reads it except for 0x00426ba0 (0x00402a8d / 0x0040d279 / 0x0040dfc5
// / 0x00410cf6 / 0x004111cc), and no caller reads ECX/EDX after any of the
// four calls (call-site disasm, 2026-10-12).
// ---------------------------------------------------------------------------

extern "C" std::uint32_t __cdecl FUN_0041c2d0();   // EAX-convention entry, Match/MatchS04_0041c2d0.cpp

// 0x0041a3e0  HudDispatchMode10   (20 bytes)
//   0x0041a3e0 mov eax,[0x0063c628]
//   0x0041a3e5 test eax,eax / 0x0041a3e7 je 0x0041a3f3 (ret, EAX = 0)
//   0x0041a3e9 mov eax,0x0063c610      ; EAX = 4-member block
//   0x0041a3ee jmp 0x0041c2d0          ; tail call, returns its EAX
// Reached from 0x0040dfc0 at 0x0040e043 (jmp, state 7) and 0x0040e0bd (call,
// states 5/6), both when DAT_007f0fd0 == 10.
// 0x0041a3e0
extern "C" __declspec(dllexport) std::uint32_t __cdecl HudDispatchMode10() {
    std::uint32_t r = *reinterpret_cast<const std::uint32_t*>(0x0063c628u);   // 0x0041a3e0
    if (r != 0) {                                                              // 0x0041a3e7
        __asm {
            mov  eax, 0x0063c610        // 0x0041a3e9
            call FUN_0041c2d0           // 0x0041a3ee (tail jmp to 0x0041c2d0 in the original; the port)
            mov  r, eax
        }
    }
    return r;
}

RH_ScopedInstall(HudDispatchMode10, 0x0041a3e0);  // 2026-10-12 ported from bytes (work/hud-dispatch)


// MATCH-S06: the body of 0x00426ba0 (HudDrawEnabled) moved to Match/MatchS06_00426ba0.cpp,
// which compiles the byte-matched source re/match/s06/00426ba0.c.
