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
// Global addresses
// ---------------------------------------------------------------------------

// Race-state gate: int32_t; allowed pass-through values: 5, 6, 7
// 0x0063ba8c
static constexpr std::uintptr_t kRaceStateAddr  = 0x0063ba8c;
// Game-mode: int32_t inner switch discriminant
// 0x007f0fd0
static constexpr std::uintptr_t kGameModeAddr   = 0x007f0fd0;

// ---------------------------------------------------------------------------
// Callee prototypes (all called via raw address, no thunk required)
// ---------------------------------------------------------------------------

// 0x00426ba0 — HUD draw-enable flag getter; returns undefined4 DAT_0066d704
typedef std::int32_t  (__cdecl *HudDrawEnabledFn)();
// 0x0042f6a0 — game sub-mode getter; returns undefined4 DAT_0067e9fc
typedef std::uint32_t (__cdecl *GetRaceSubModeFn)();
// 0x0042f500 — discriminator getter; returns undefined4 DAT_0067ea64
typedef std::int32_t  (__cdecl *GetDat0067ea64Fn)();
// 0x0041e850 — sub-mode 2; void
typedef void (__cdecl *Sub0041e850Fn)();
// 0x0041ded0 — game-mode 4/7/8/9; void; param int32_t (1 or 0)
typedef void (__cdecl *Sub0041ded0Fn)(std::int32_t param_1);
// 0x0041c300 — game-mode 5; void
typedef void (__cdecl *Sub0041c300Fn)();
// 0x0041a3e0 — game-mode 10; void
typedef void (__cdecl *Sub0041a3e0Fn)();
// 0x0041b630 — {5/6}-path, FUN_0042f500==0 branch; void
typedef void (__cdecl *Sub0041b630Fn)();
// 0x0041c0c0 — {5/6}-path, FUN_0042f500!=0 branch; void
typedef void (__cdecl *Sub0041c0c0Fn)();
// 0x0041ccc0 — {7}-path, default/6/10 sub-modes, FUN_0042f500==0; void
typedef void (__cdecl *Sub0041ccc0Fn)();
// 0x0041d870 — {7}-path, default/6/10 sub-modes, FUN_0042f500!=0; void
typedef void (__cdecl *Sub0041d870Fn)();
// 0x0041db80 — post-switch tail; only on {5/6}-path; void
typedef void (__cdecl *Sub0041db80Fn)();
// 0x00403160 — sub-mode 0xb viewport handler; void
typedef void (__cdecl *Sub00403160Fn)();

static inline std::int32_t  CallHudDrawEnabled() { return reinterpret_cast<HudDrawEnabledFn>(0x00426ba0)();  }
static inline std::uint32_t CallGetRaceSubMode()  { return reinterpret_cast<GetRaceSubModeFn>(0x0042f6a0)(); }
static inline std::int32_t  CallGetDat0067ea64()  { return reinterpret_cast<GetDat0067ea64Fn>(0x0042f500)(); }
static inline void          Call0041e850()         { reinterpret_cast<Sub0041e850Fn>(0x0041e850)();           }
static inline void          Call0041ded0(std::int32_t p) { reinterpret_cast<Sub0041ded0Fn>(0x0041ded0)(p);   }
static inline void          Call0041c300()         { reinterpret_cast<Sub0041c300Fn>(0x0041c300)();           }
static inline void          Call0041a3e0()         { reinterpret_cast<Sub0041a3e0Fn>(0x0041a3e0)();           }
static inline void          Call0041b630()         { reinterpret_cast<Sub0041b630Fn>(0x0041b630)();           }
static inline void          Call0041c0c0()         { reinterpret_cast<Sub0041c0c0Fn>(0x0041c0c0)();           }
static inline void          Call0041ccc0()         { reinterpret_cast<Sub0041ccc0Fn>(0x0041ccc0)();           }
static inline void          Call0041d870()         { reinterpret_cast<Sub0041d870Fn>(0x0041d870)();           }
static inline void          Call0041db80()         { reinterpret_cast<Sub0041db80Fn>(0x0041db80)();           }
static inline void          Call00403160()         { reinterpret_cast<Sub00403160Fn>(0x00403160)();           }

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

// 0x0040dfc0
extern "C" __declspec(dllexport) void __cdecl HudIngameDispatch() {
    std::int32_t  iVar1;
    std::uint32_t uVar2;
    bool          bVar3;

    // Guard 1: HUD draw enabled
    iVar1 = CallHudDrawEnabled();
    if (iVar1 == 0) return;

    // Guard 2: race-state gate
    const std::int32_t raceState = *reinterpret_cast<const std::int32_t*>(kRaceStateAddr);
    if (raceState < 5) return;
    if (raceState > 6) {
        if (raceState != 7) return;

        // ── DAT_0063ba8c == 7 branch ──────────────────────────────────────────
        uVar2 = CallGetRaceSubMode();
        switch (uVar2) {
        case 2:
            Call0041e850(); return;
        case 3: case 4: case 5: {
            const std::int32_t gameMode = *reinterpret_cast<const std::int32_t*>(kGameModeAddr);
            switch (gameMode) {
            case 4: case 7: case 9: Call0041ded0(1); return;
            case 5:                 Call0041c300();   return;
            case 8:                 Call0041ded0(0);  return;
            case 10:                Call0041a3e0();   return;
            // no default: falls through to outer default
            }
            // fall-through to outer default
        }
        // FALLTHROUGH
        default: {
            iVar1 = CallGetDat0067ea64();
            if (iVar1 == 0) { Call0041ccc0(); return; }
            break;
        }
        case 6: case 10: {
            iVar1 = CallGetDat0067ea64();
            const std::int32_t gameMode2 = *reinterpret_cast<const std::int32_t*>(kGameModeAddr);
            if ((iVar1 == 0) || (gameMode2 == 2)) { Call0041ccc0(); return; }
            break;
        }
        } // switch uVar2 (high branch)

        // Reached when break above (case default/6/10 with non-zero result)
        Call0041d870(); return;
    }

    // ── DAT_0063ba8c in {5, 6} branch ──────────────────────────────────────
    uVar2 = CallGetRaceSubMode();
    switch (uVar2) {
    case 2:
        Call0041e850(); break;
    case 3: case 4: case 5: {
        const std::int32_t gameMode = *reinterpret_cast<const std::int32_t*>(kGameModeAddr);
        switch (gameMode) {
        case 4: case 7: case 9: Call0041ded0(1); break;
        case 5:                 Call0041c300();   break;
        case 8:                 Call0041ded0(0);  break;
        case 10:                Call0041a3e0();   break;
        default:                goto switchD_0040e06a_caseD_7;
        }
        break;
    }
    case 6: case 10: {
        iVar1 = CallGetDat0067ea64();
        if (iVar1 != 0) {
            bVar3 = (*reinterpret_cast<const std::int32_t*>(kGameModeAddr) == 2);
            goto LAB_0040e0d2;
        }
        goto LAB_0040e0db;
    }
    default:
    switchD_0040e06a_caseD_7:
        iVar1 = CallGetDat0067ea64();
        bVar3 = (iVar1 == 0);
LAB_0040e0d2:
        if (!bVar3) { Call0041c0c0(); break; }
LAB_0040e0db:
        Call0041b630(); break;
    case 0xb:
        Call00403160(); break;
    } // switch uVar2 (low branch)

    // Post-switch: unconditional tail (only reached on {5,6}-path)
    iVar1 = static_cast<std::int32_t>(CallGetRaceSubMode());
    if (iVar1 == 0xb) return;
    Call0041db80();
}

RH_ScopedInstall(HudIngameDispatch, 0x0040dfc0);  // re-enabled 2026-05-24 c3-safe

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

// ---------------------------------------------------------------------------
// 0x0041ccc0  HudSlotLoopCcc0
//
// Called from HudIngameDispatch on the DAT_0063ba8c == 7 path for
// default/fallthrough/case 6/case 10 sub-modes when FUN_0042f500() returns 0
// or (sub-mode 6/10 and DAT_007f0fd0 == 2).
//
// Iterates 4 entries of the array at 0x0063ce20 with stride 0x114 (276 bytes).
// Per entry: reads int32_t at offset +0x110; if non-zero → calls FUN_0041c9a0.
// Loop condition: (int32_t)puVar1 < 0x63d270  (pointer-as-int32 comparison).
//
// FUN_0041c9a0 is __thiscall-via-EAX (same convention as FUN_0041b340 in
// HudSlotLoopB630); the original loop emits
//   0x0041ccd0  MOV EAX, ESI         ; this = current entry pointer
//   0x0041ccd2  CALL 0x0041c9a0
// We replicate the EAX setup with inline asm; see HudSlotLoopB630 for the
// rationale on why a plain C call would crash in production despite passing
// the synthetic main-menu diff.
//
// Struct accesses:
//   Base 0x0063ce20 + n*0x114, offset +0x110: int32_t enable flag.
// ---------------------------------------------------------------------------

// 0x0041ccc0
extern "C" __declspec(dllexport) void __cdecl HudSlotLoopCcc0() {
    std::uint8_t* puVar1 = reinterpret_cast<std::uint8_t*>(0x0063ce20u);
    constexpr std::uintptr_t kCallee0041c9a0 = 0x0041c9a0u;
    do {
        if (*reinterpret_cast<const std::int32_t*>(puVar1 + 0x110) != 0) {
            // Replicate 0x0041ccd0..0x0041ccd6: MOV EAX, ESI ; CALL 0x0041c9a0
            const std::uintptr_t fn_addr = kCallee0041c9a0;
            std::uint8_t* entry = puVar1;
            __asm {
                mov eax, entry
                mov ecx, fn_addr
                call ecx
            }
        }
        puVar1 += 0x114;
    } while (reinterpret_cast<std::int32_t>(puVar1) < static_cast<std::int32_t>(0x0063d270u));
}

RH_ScopedInstall(HudSlotLoopCcc0, 0x0041ccc0);  // re-enabled 2026-05-24 c3-safe

// ---------------------------------------------------------------------------
// FUN_0041c2d0  --  0x0041c2d0   (35 bytes, 0x0041c2d0..0x0041c2f2)
//
// Re-ported from the bytes of original\MASHED.exe.unpatched on 2026-10-12
// (U-9671). The block pointer arrives in EAX:
//   0x0041c2d0 push esi
//   0x0041c2d1 mov  esi,eax                ; esi = block
//   0x0041c2d3 mov  eax,[esi+0xc]          ; m = block[3]
//   0x0041c2d6 push eax
//   0x0041c2d7 call [eax+0x48]             ; m->[+0x48](m)
//   0x0041c2da mov  eax,[esi+4]            ; m = block[1]
//   0x0041c2dd push eax
//   0x0041c2de call [eax+0x48]
//   0x0041c2e1 mov  eax,[esi]              ; m = block[0]
//   0x0041c2e3 push eax
//   0x0041c2e4 call [eax+0x48]
//   0x0041c2e7 mov  esi,[esi+8]            ; m = block[2]
//   0x0041c2ea push esi
//   0x0041c2eb call [esi+0x48]
//   0x0041c2ee add  esp,0x10               ; caller cleans all four args
//   0x0041c2f1 pop  esi
//   0x0041c2f2 ret                         ; EAX = the last call's EAX
// [m+0x48] is a function-pointer FIELD of the member, called with one
// dereference. It is not a vtable slot: there is no `mov reg,[m]` before the
// call. Ghidra's decomp agrees: (**(code **)(in_EAX[3] + 0x48))(in_EAX[3]).
// No guards. Callers: 0x0041a3e0 (tail jmp at 0x0041a3ee, EAX = 0x0063c610)
// and 0x0041c300 (tail jmp at 0x0041c30e, EAX = 0x0063cda4).
//
// The port is a naked EAX shim around a cdecl body. The shim preserves every
// register the original preserves (ESI, and EBX/EDI/EBP, which the body saves
// as MSVC callee-saved) and returns the last member call's EAX, as the
// original does. ECX/EDX at return are not part of the contract: no caller
// reads them (0x0041a3e0 / 0x0041c300 tail-jump, so the reader would be
// 0x0040dfc0 after 0x0040e028 / 0x0040e043 / 0x0040e09e / 0x0040e0bd, and it
// does not, per re/analysis/negctl/reearn_20261011/fix_hud_dispatch_RESULT.md).
// ---------------------------------------------------------------------------

typedef std::uint32_t (__cdecl *HudMemberFn)(void* member);

static std::uint32_t __cdecl HudDispatchDraw4Body(void* const* block) {
    void* m;
    m = block[3];                                                              // 0x0041c2d3
    (*reinterpret_cast<HudMemberFn*>(static_cast<std::uint8_t*>(m) + 0x48))(m);   // 0x0041c2d7
    m = block[1];                                                              // 0x0041c2da
    (*reinterpret_cast<HudMemberFn*>(static_cast<std::uint8_t*>(m) + 0x48))(m);   // 0x0041c2de
    m = block[0];                                                              // 0x0041c2e1
    (*reinterpret_cast<HudMemberFn*>(static_cast<std::uint8_t*>(m) + 0x48))(m);   // 0x0041c2e4
    m = block[2];                                                              // 0x0041c2e7
    return (*reinterpret_cast<HudMemberFn*>(static_cast<std::uint8_t*>(m) + 0x48))(m);  // 0x0041c2eb
}

// 0x0041c2d0
extern "C" __declspec(dllexport) __declspec(naked)
std::uint32_t __cdecl FUN_0041c2d0()
{
    __asm {
        push eax                        // block (arrives in EAX, 0x0041c2d1)
        call HudDispatchDraw4Body
        add  esp, 4
        ret                             // EAX = last member call's EAX
    }
}

RH_ScopedInstall(FUN_0041c2d0, 0x0041c2d0);  // 2026-10-12 re-ported from bytes (U-9671)

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
// 0x0041c2d0 is ported above (U-9671, 2026-10-12): the two mode routers call
// the port FUN_0041c2d0 directly with EAX loaded by inline asm. 0x0041bc50 is
// not ported (C2), so 0x0041c0c0 calls it at its original RVA, as
// HudSlotLoopB630 does for 0x0041b340.
//
// Return value: each original leaves EAX defined at RET, so the ports return
// the same EAX (the flag read on the skip path, else the callee's EAX). No
// caller reads it except for 0x00426ba0 (0x00402a8d / 0x0040d279 / 0x0040dfc5
// / 0x00410cf6 / 0x004111cc), and no caller reads ECX/EDX after any of the
// four calls (call-site disasm, 2026-10-12).
// ---------------------------------------------------------------------------

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

// 0x0041c0c0  HudDispatchSlot2   (29 bytes)
//   0x0041c0c0 push esi
//   0x0041c0c1 mov esi,0x0063cab8
//   0x0041c0c6 mov eax,esi ; 0x0041c0c8 call 0x0041bc50   ; EAX = record
//   0x0041c0cd add esi,0x16c
//   0x0041c0d3 cmp esi,0x0063cd90 ; 0x0041c0d9 jl 0x0041c0c6 (signed)
//   0x0041c0db pop esi ; ret        ; EAX = last 0x0041bc50 EAX
// 0x0063cab8 + 2*0x16c == 0x0063cd90: two records, no per-record guard.
// Reached from 0x0040dfc0 at 0x0040e0d4 (states 5/6, FUN_0042f500() != 0 and,
// on the sub-mode 6/10 arm only, DAT_007f0fd0 != 2).
// 0x0041c0c0
extern "C" __declspec(dllexport) std::uint32_t __cdecl HudDispatchSlot2() {
    std::uint8_t* rec = reinterpret_cast<std::uint8_t*>(0x0063cab8u);         // 0x0041c0c1
    std::uint32_t r;
    do {
        __asm {
            mov  eax, rec               // 0x0041c0c6
            mov  ecx, 0x0041bc50
            call ecx                    // 0x0041c0c8
            mov  r, eax
        }
        rec += 0x16c;                                                          // 0x0041c0cd
    } while (reinterpret_cast<std::int32_t>(rec) < static_cast<std::int32_t>(0x0063cd90u));  // 0x0041c0d3 / jl
    return r;
}

RH_ScopedInstall(HudDispatchSlot2, 0x0041c0c0);  // 2026-10-12 ported from bytes (work/hud-dispatch)

// 0x0041c300  HudDispatchMode5   (20 bytes) -- same shape as 0x0041a3e0
//   0x0041c300 mov eax,[0x0063cdbc]
//   0x0041c305 test eax,eax / 0x0041c307 je 0x0041c313 (ret, EAX = 0)
//   0x0041c309 mov eax,0x0063cda4      ; EAX = 4-member block
//   0x0041c30e jmp 0x0041c2d0
// Reached from 0x0040dfc0 at 0x0040e028 (jmp, state 7) and 0x0040e09e (call,
// states 5/6), both when DAT_007f0fd0 == 5.
// 0x0041c300
extern "C" __declspec(dllexport) std::uint32_t __cdecl HudDispatchMode5() {
    std::uint32_t r = *reinterpret_cast<const std::uint32_t*>(0x0063cdbcu);   // 0x0041c300
    if (r != 0) {                                                              // 0x0041c307
        __asm {
            mov  eax, 0x0063cda4        // 0x0041c309
            call FUN_0041c2d0           // 0x0041c30e (tail jmp to 0x0041c2d0 in the original; the port)
            mov  r, eax
        }
    }
    return r;
}

RH_ScopedInstall(HudDispatchMode5, 0x0041c300);  // 2026-10-12 ported from bytes (work/hud-dispatch)

// 0x00426ba0  HudDrawEnabled   (6 bytes)
//   0x00426ba0 mov eax,[0x0066d704] ; 0x00426ba5 ret
// 0x00426ba0
extern "C" __declspec(dllexport) std::uint32_t __cdecl HudDrawEnabled() {
    return *reinterpret_cast<const std::uint32_t*>(0x0066d704u);              // 0x00426ba0
}

RH_ScopedInstall(HudDrawEnabled, 0x00426ba0);  // 2026-10-12 ported from bytes (work/hud-dispatch)
