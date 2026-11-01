// Mashed RE — Frontend menu navigation helpers.
// Original functions from frontend_promote_menus_a cluster.
//
// All RVAs are for MASHED.exe (size 2,846,720 / SHA-256 BDCAE093...).
//
// Callee dependency:
//   0x0040e470  FUN_0040e470 (C2 drift-promoted)
//     Signature: undefined4 FUN_0040e470(int param_1)
//     Returns *(PTR_PTR_005f2770 + param_1*4 + 0x34) — raw DWORD (1=player, 2=AI)
#include "../Core/HookSystem.h"
#include "MenuNav.h"

#include <windows.h>
#include <cstdint>

// ---------------------------------------------------------------------------
// Globals referenced by this cluster (read-only; game owns all writes)
// ---------------------------------------------------------------------------

// Slot index global: which of the 8 menu slots is currently active.
static constexpr std::uintptr_t kSlotIndex        = 0x0067e9f8;

// Per-slot pointer table: *(0x0067ed38 + slot*0x40) -> pointer to entry array.
static constexpr std::uintptr_t kPtrTableBase      = 0x0067ed38;

// Per-slot cursor: *(0x0067ed40 + slot*0x40) -> int cursor (skip count - 1).
static constexpr std::uintptr_t kCursorTableBase   = 0x0067ed40;

// Sentinel constants for the linked-entry array format.
static constexpr std::uint32_t  kSentInnerTerm     = 0xFF040000u; // inner group terminator
static constexpr std::uint32_t  kSentFinalA        = 0xFF050000u; // final-scan stop A
static constexpr std::uint32_t  kSentFinalB        = 0xFF140000u; // final-scan stop B
static constexpr std::uint32_t  kSentFinalC        = 0xFF060000u; // final-scan stop C => return -1

// Player slot index array for the team/input functions.
// Four entries at 0x7f1a14/24/34/44 (stride 16 / 0x10 per slot record).
static constexpr std::uintptr_t kPlayerSlotBase    = 0x007f1a14; // car-slot index, slot 0
static constexpr std::uintptr_t kTeamFieldBase     = 0x007f1a18; // team result field, slot 0
static constexpr std::uintptr_t kTeamTableBase     = 0x0067e938; // car-team lookup
static constexpr std::uintptr_t kTeamClearEnd      = 0x7f1a58;   // exclusive upper bound

// Button-detection globals (0x0042aff0 / 0x0042b180 bodies).
// Pad k (k = 0..7) owns a 0x4c-byte record; its held byte is at
// 0x007f1040 + col + k*0x4c and its previous-frame byte 0x4c0 bytes later.
static constexpr std::uintptr_t kDispatchSel       = 0x007f1a0c; // 0x1000 = all-8-pads path
static constexpr std::uintptr_t kHeldBaseA         = 0x007f1046; // 0x0042b000 / 0x0042b14f
static constexpr std::uintptr_t kHeldBaseB         = 0x007f1047; // 0x0042b190 / 0x0042b2df
static constexpr std::uintptr_t kPrevBaseA         = 0x007f1506; // 0x0042b10d / 0x0042b159
static constexpr std::uintptr_t kPrevBaseB         = 0x007f1507; // 0x0042b29d / 0x0042b2e9
static constexpr std::uintptr_t kHeldToPrev        = 0x4c0;      // 0x0042b112 `[eax - 0x4c0]`
static constexpr int            kStride            = 0x4c;       // 0x0042b08a imul / 0x0042b121 add
static constexpr int            kPadCount          = 8;          // 0x0042b124 bound 0x7f1766 = 0x7f1506 + 8*0x4c
static constexpr std::uintptr_t kScreenTypeBase    = 0x0067ed3c; // 0x0042b0b6
static constexpr std::uintptr_t kSlotStride        = 0x40;       // 0x0042b0b3 `shl ecx, 6`
static constexpr std::uintptr_t kTimerA            = 0x0067f1b0; // 0x0042b0cc (float)
static constexpr std::uintptr_t kTimerB            = 0x0067f1b4; // 0x0042b25c (float)
static constexpr std::uint32_t  kTimerRearmBits    = 0x3e4ccccdu;// 0x0042b0e5 (0.2f)
static constexpr std::uintptr_t kFrameDelta        = 0x007f1004; // 0x0042b0c6 (float)
static constexpr std::uintptr_t kHoldThreshold     = 0x005ccac0; // 0x0042b0d8 (float)
// Player-slot records: id dword at 0x007f1a14 + i*0x10, i = 0..3.
static constexpr std::uintptr_t kPlayerSlotLoop    = 0x007f1a14; // 0x0042b070
static constexpr std::uintptr_t kPlayerSlotRecStride = 0x10;     // 0x0042b09c `add esi, 0x10`
static constexpr std::uintptr_t kPlayerSlotLoopEnd = 0x007f1a54; // 0x0042b0a0 (exclusive, 4 records)

// Callee 0x0040e470: returns DWORD at PTR_PTR_005f2770 + param*4 + 0x34.
// We call the original during C3 verification; for the standalone build the
// function will be resolved via its own hook entry once promoted.
typedef std::uint32_t (__cdecl *FunSlotStateFn)(int param_1);
static constexpr std::uintptr_t kFun0040e470       = 0x0040e470;

static inline std::uint32_t CallSlotState(int idx) {
    return reinterpret_cast<FunSlotStateFn>(kFun0040e470)(idx);
}

// ---------------------------------------------------------------------------
// 0x0042ac90  MenuEntryGet
// ONE body for both builds (U-9667, 2026-10-12). MenuEntryWalk below is the
// table walk; the .asi export reads the three globals and calls it, and the
// standalone's select path (MenuNavSM.cpp Nav_Select / Nav_SelectAction) calls
// it with the nav slot's table and cursor. The standalone used to run its own
// ItemActionCode instead, which returned -1 at cursor -1 where the bytes return
// item 0's action code (re/analysis/u9667_menu_tables/RESULT.md); it is gone.
//
// Bytes (MASHED.exe.unpatched):
//   0x0042aca5 xor eax,eax / 0x0042aca7 cmp ecx,-1 / je 0x0042acd2   cursor -1 skips the item walk
//   0x0042acac inc ecx                                              cursor+1 passes
//   0x0042acb0 cmp [edx+eax*4],0xff040000 / je 0x0042accd           word at eax already T4?
//   0x0042acc0 mov esi,[edx+eax*4+4] / inc eax / cmp esi,0xff040000 / jne 0x0042acc0
//   0x0042accd inc eax / 0x0042acce dec ecx / jne 0x0042acb0        loop while ecx != 0
//   0x0042acd2 mov ecx,[edx+eax*4] / cmp ecx,0xff050000 / je 0x0042acfd
//   0x0042ace0 cmp ecx,0xff140000 / je / 0x0042ace8 cmp ecx,0xff060000 / je 0x0042acfd
//   0x0042acf0 mov ecx,[edx+eax*4+4] / inc eax / cmp ecx,0xff050000 / jne 0x0042ace0
//   0x0042acfd cmp [edx+eax*4],0xff060000 / jne 0x0042ad0a / or eax,-1 / ret
//   0x0042ad0a mov eax,[edx+eax*4+4] / ret
// The pass counter is the bytes' `dec ecx / jne` (stop at 0). The earlier .asi
// body stopped at `> 0`; the two agree for every cursor >= -1, and a cursor
// <= -2 walks off the table in the original either way.
// Pure read-only, no side-effects.
// ---------------------------------------------------------------------------
namespace mashed_re {
namespace Frontend {

// Table walk of 0x0042ac90 (anchored on the export below; one body, two callers).
std::uint32_t MenuEntryWalk(const std::uint32_t* table, std::int32_t cursor) {
    std::int32_t i = 0;                                    // EAX, 0x0042aca5
    if (cursor != -1) {                                    // 0x0042aca7
        std::uint32_t passes = static_cast<std::uint32_t>(cursor) + 1u;  // 0x0042acac
        do {
            if (table[i] != kSentInnerTerm) {              // 0x0042acb0
                std::uint32_t w;
                do {
                    w = table[i + 1];                      // 0x0042acc0
                    ++i;
                } while (w != kSentInnerTerm);
            }
            ++i;                                           // 0x0042accd past the 0xff040000
        } while (--passes != 0u);                          // 0x0042acce
    }

    // Final scan: stop on 0xff050000 / 0xff140000 / 0xff060000 (0x0042acd2..0x0042acf0).
    std::uint32_t w = table[i];
    while (w != kSentFinalA && w != kSentFinalB && w != kSentFinalC) {
        w = table[i + 1];
        ++i;
    }

    if (table[i] == kSentFinalC) return 0xffffffffu;       // 0x0042acfd
    return table[i + 1];                                   // 0x0042ad0a
}

} // namespace Frontend
} // namespace mashed_re

// 0x0042ac90
extern "C" __declspec(dllexport) std::uint32_t __cdecl MenuEntryGet() {
    const std::int32_t slot = *reinterpret_cast<std::int32_t*>(kSlotIndex);              // 0x0042ac90
    const auto* table = *reinterpret_cast<const std::uint32_t* const*>(
        kPtrTableBase + static_cast<std::uintptr_t>(slot) * 0x40);                       // 0x0042ac99
    const std::int32_t cursor = *reinterpret_cast<std::int32_t*>(
        kCursorTableBase + static_cast<std::uintptr_t>(slot) * 0x40);                    // 0x0042ac9f
    return mashed_re::Frontend::MenuEntryWalk(table, cursor);
}

RH_ScopedInstall(MenuEntryGet, 0x0042ac90);
// Re-enabled 2026-10-12 (U-9660): the MASS-DISABLE below was a harness limit, not a game fault. The fixture it asked
// for now exists (re/frida/negctl_fixtures/z_fix_u9660.py: slot index, per-slot table pointer, cursor and decoy
// slots seeded), and the port reads GREEN 16/16 against it with self-baseline GREEN. Original note kept below.
// [was] MASS-DISABLED 2026-05-24 needs-canonical-menu-ptr-table.
// Phase A1 audit 2026-05-24: function derefs *(0x0067ed38 + slot*0x40) (the
// per-slot menu pointer table). At quiescent main menu without an active
// menu screen, slot 0's pointer is NULL and both sides AV at 0x0
// identically (banned as crash_equal GREEN). Real test would require
// staging a synthetic entry array with sentinel terminators (0xFF040000 /
// 0xFF050000 / 0xFF060000 / 0xFF140000) and pointing the slot table at it.
// Scaffold cost > yield for one hook; canonical-scenario at menu navigation.

// MATCH-S07: the body of 0x0042bb60 (MenuTeamBalance) moved to Match/MatchS07_0042bb60.cpp,
// which compiles the byte-matched source re/match/s07/0042bb60.c.

// MATCH-S07: the body of 0x0042fa00 (MenuTeamSelectTick) moved to Match/MatchS07_0042fa00.cpp,
// which compiles the byte-matched source re/match/s07/0042fa00.c (its RH_ScopedInstall moved with it).

// MATCH-S07: the bodies of 0x0042aff0 (MenuButtonDetectA) and 0x0042b180 (MenuButtonDetectB), with their shared
// core MenuButtonDetectHoldRepeat and its x87 hold-timer step, moved to Match/MatchS07_0042aff0.cpp and
// Match/MatchS07_0042b180.cpp, which compile the byte-matched sources re/match/s07/0042aff0.c and 0042b180.c.
