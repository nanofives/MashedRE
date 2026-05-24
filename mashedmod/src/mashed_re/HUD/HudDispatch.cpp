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
// FUN_0041c2d0  --  0x0041c2d0
//
// Original: FUN_0041c2d0 (34 bytes, 0x0041c2d0–0x0041c2f2)
// EAX-thiscall vtable dispatcher. Object pointer arrives in EAX (register
// convention, not stack). Dispatches vtable[0x48] (18th slot = Draw/Render)
// unconditionally on four member objects at EAX+0xc, EAX+4, EAX+0, EAX+8.
// Dispatch order: EAX[3]→EAX[1]→EAX[0]→EAX[2] (offsets 12,4,0,8).
// No guards. Shared by two game modes: mode-10 (sub_0041a3e0) and mode-5
// (sub_0041c300), both called only when their respective enable guards are
// non-zero (DAT_0063c628, DAT_0063cdbc).
// Verification: crash_equal_ok=True — calling without valid EAX (EAX=0)
// causes identical null-deref crash on both original and reimpl.
// ref: re/analysis/promote_c2_hud_ingame/0x0041c2d0.md
// ---------------------------------------------------------------------------

// 0x0041c2d0
// EAX-thiscall: object ptr passed in EAX, not on stack.
// We implement via __declspec(naked) + inline asm to preserve the calling
// convention exactly: EAX is the object base; vtable[0x48] on EAX[3/1/0/2].
extern "C" __declspec(dllexport) __declspec(naked)
void __cdecl FUN_0041c2d0()
{
    __asm {
        // EAX = object ptr (passed by caller in EAX convention)
        // Dispatch order: EAX[3]=*(EAX+0xc), EAX[1]=*(EAX+4), EAX[0]=*(EAX), EAX[2]=*(EAX+8)
        // Each: load member ptr, call vtable[0x48](member_ptr)
        // 0x0041c2d0: *(*(EAX+0xc) + 0x48)(*(EAX+0xc))
        mov ecx, dword ptr [eax + 0x0c]  // EAX[3]
        mov edx, dword ptr [ecx]         // vtable ptr of member[3]
        push ecx                         // arg: member ptr
        call dword ptr [edx + 0x48]      // vtable[0x48]
        // 0x0041c2da: *(*(EAX+4) + 0x48)(*(EAX+4))
        mov ecx, dword ptr [eax + 0x04]  // EAX[1]
        mov edx, dword ptr [ecx]
        push ecx
        call dword ptr [edx + 0x48]
        // 0x0041c2e2: *(*EAX + 0x48)(*EAX)
        mov ecx, dword ptr [eax]         // EAX[0]
        mov edx, dword ptr [ecx]
        push ecx
        call dword ptr [edx + 0x48]
        // 0x0041c2ea: *(*(EAX+8) + 0x48)(*(EAX+8))
        mov ecx, dword ptr [eax + 0x08]  // EAX[2]
        mov edx, dword ptr [ecx]
        push ecx
        call dword ptr [edx + 0x48]
        ret
    }
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(FUN_0041c2d0, 0x0041c2d0);
