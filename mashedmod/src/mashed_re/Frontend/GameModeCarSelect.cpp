// Mashed RE - Game-mode car-select state initializer.
// Analysis note: re/analysis/game_mode_cont2/0x00431d00.md
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Callee dependency (C2 drift-promoted from C1):
//   0x00431b80  FUN_00431b80 (C2)
//     Car-select cursor mover; in_EAX=player index into 0067ea98; unaff_ESI=direction.
//     Non-standard calling convention: player index passed in EAX, direction in ESI.
//     We call the original at runtime for C3 verification.

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

// 7 globals written in sequence by FUN_00431d00 (body 0x00431d00)
static constexpr std::uintptr_t kGlobal_ea74 = 0x0067ea74; // written 1
static constexpr std::uintptr_t kGlobal_ea90 = 0x0067ea90; // written 1
static constexpr std::uintptr_t kGlobal_ea94 = 0x0067ea94; // written 0
static constexpr std::uintptr_t kGlobal_ea98 = 0x0067ea98; // written 1 (player-1 slot)
static constexpr std::uintptr_t kGlobal_ea9c = 0x0067ea9c; // written 2 (player-2 slot)
static constexpr std::uintptr_t kGlobal_eaa0 = 0x0067eaa0; // written 3 (player-3 slot)
static constexpr std::uintptr_t kGlobal_eaac = 0x0067eaac; // written 0

// FUN_00431b80 call triple: (0) -> capture result -> (result) -> (0)
// Non-standard calling convention: EAX=player_index, ESI=direction.
// We invoke the original function directly via a raw function pointer with
// inline asm to set EAX/ESI correctly.
static constexpr std::uintptr_t kFun00431b80 = 0x00431b80;

// ---------------------------------------------------------------------------
// Callee 0x00431b80 invocation helper
//
// FUN_00431b80 uses a non-standard calling convention per U-1655: the
// in_EAX operand (player index) and unaff_ESI (direction) are not standard
// stack args. For C3 synthetic diff, the function is called at the quiescent
// main menu where the game state already provides valid EAX/ESI context.
//
// We call through a standard __cdecl function pointer. The game's own
// calling context (EAX=0, ESI=0) matches what the analysis shows for the
// first two calls (arg=0). For a synthetic diff at main menu this is safe:
// the cursor table at 0x0067ea98 is initialized by boot code before we arrive.
//
// Using a volatile asm approach to set EAX/ESI before the call:
// ---------------------------------------------------------------------------
static std::int32_t CallCursorMover(std::int32_t player_idx, std::int32_t direction) {
    std::int32_t result = 0;
    // kFun00431b80 = 0x00431b80; place in local for asm address operand
    const std::uintptr_t fn_addr = kFun00431b80;
    __asm {
        mov eax, player_idx
        mov esi, direction
        mov ecx, fn_addr
        call ecx
        mov result, eax
    }
    return result;
}

// ---------------------------------------------------------------------------
// 0x00431d00  CarSelectReset
//
// Initializes car-select state machine globals and invokes the cursor-mover
// three times in a chained pattern:
//   uVar1 = FUN_00431b80(0)   [call 1: arg = 0]
//   FUN_00431b80(uVar1)        [call 2: arg = result of call 1]
//   FUN_00431b80(0)            [call 3: arg = 0]
// Returns void.
// ref: re/analysis/game_mode_cont2/0x00431d00.md
// ---------------------------------------------------------------------------

// 0x00431d00
extern "C" __declspec(dllexport) void __cdecl CarSelectReset() {
    // Global writes in order (body 0x00431d00)
    *reinterpret_cast<std::int32_t*>(kGlobal_ea74) = 1;
    *reinterpret_cast<std::int32_t*>(kGlobal_ea90) = 1;
    *reinterpret_cast<std::int32_t*>(kGlobal_ea94) = 0;
    *reinterpret_cast<std::int32_t*>(kGlobal_ea98) = 1;
    *reinterpret_cast<std::int32_t*>(kGlobal_ea9c) = 2;
    *reinterpret_cast<std::int32_t*>(kGlobal_eaa0) = 3;
    *reinterpret_cast<std::int32_t*>(kGlobal_eaac) = 0;

    // FUN_00431b80 call triple (chained pattern)
    // Call 1: player_idx=0, direction from ESI (which caller had set; use 0)
    const std::int32_t uVar1 = CallCursorMover(0, 0);
    // Call 2: player_idx=uVar1, direction 0
    CallCursorMover(uVar1, 0);
    // Call 3: player_idx=0, direction 0
    CallCursorMover(0, 0);
}

// DISABLED 2026-05-24: hook hangs MASHED at boot. Reimpl calls FUN_00431b80
// three times with EAX=0/ESI=0 via inline asm — non-standard calling
// convention with caller-supplied context. At boot context EAX/ESI are not
// what the original expected; suspect infinite loop in cursor mover. Re-enable
// only after diff-original validation in a real car-select scenario.
// RH_ScopedInstall(CarSelectReset, 0x00431d00);
