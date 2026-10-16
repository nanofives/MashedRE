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

// ---------------------------------------------------------------------------
// 0x0042bb60  MenuTeamBalance
// Reads 4 player-slot indices; for each valid slot reads team from
// 0x0067e938+slot*12 (field [slot*3], subtract 1); writes team to
// 0x007f1a18/28/38/48; counts teamA and teamB; returns balance code.
// Side-effect: writes DAT_007f1a18/28/38/48.
// ---------------------------------------------------------------------------
// 0x0042bb60
extern "C" __declspec(dllexport) int __cdecl MenuTeamBalance() {
    // Step 1: clear team fields (4 entries at 0x7f1a18/28/38/48, stride 16 bytes).
    // Note: puVar1 += 4 in Ghidra pseudocode with uint32_t* type = +16 bytes per step.
    // Upper bound 0x7f1a58 is exclusive. 4 iterations: 0x7f1a18, 0x7f1a28, 0x7f1a38, 0x7f1a48.
    auto* puVar1 = reinterpret_cast<std::uint32_t*>(kTeamFieldBase);
    while (reinterpret_cast<std::uintptr_t>(puVar1) < kTeamClearEnd) {
        *puVar1 = 0xffffffffu;
        // Advance by 4 uint32_t elements = 16 bytes (pointer arithmetic stride = 16)
        puVar1 += 4;
    }

    // Step 2: resolve team for each of the 4 player slots
    int iVar2 = 0; // teamA count
    int iVar3 = 0; // total players
    int iVar4 = 0; // teamB count

    for (int n = 0; n < 4; n++) {
        const std::int32_t slotIdx = *reinterpret_cast<std::int32_t*>(kPlayerSlotBase + n * 16);
        if (slotIdx >= 0) {
            // team = table[slotIdx * 3] - 1  (read int at 0x0067e938 + slotIdx*12)
            const std::int32_t team = *reinterpret_cast<std::int32_t*>(kTeamTableBase + slotIdx * 12) - 1;
            // Write team to corresponding team field
            *reinterpret_cast<std::int32_t*>(kTeamFieldBase + n * 16) = team;
            if (team == 0) iVar2 += 1;
            if (team == 1) iVar4 += 1;
            iVar3 += 1;
        }
    }

    // Step 3: balance result
    if (iVar3 == 2) {
        return (iVar2 == 1 && iVar4 == 1) ? 0x1000 : 0;
    }
    if (iVar3 == 3) {
        if ((iVar2 == 1 && iVar4 == 2) || (iVar2 == 2 && iVar4 == 1)) return 0x1000;
        if (iVar2 == 0) return 1;
        return (iVar4 != 0) ? 3 : 2;
    }
    if (iVar3 == 4) {
        if ((iVar2 == 2 && iVar4 == 2) || (iVar2 == 1 && iVar4 == 3) || (iVar2 == 3 && iVar4 == 1)) return 0x1000;
        if (iVar2 == 0) return 1;
        return (iVar4 != 0) ? 3 : 2;
    }
    return -1;
}

RH_ScopedInstall(MenuTeamBalance, 0x0042bb60);  // re-enabled 2026-05-24 c3-frontend-a

// ---------------------------------------------------------------------------
// 0x0042fa00  MenuTeamSelectTick   (163 bytes)
//
// THE PLAYER-SETUP HANDLER: the per-frame input step of the Team Select screen
// (nav screen 16, renderer FUN_0043aa30). It is what makes teams assignable at
// all -- MenuTeamBalance above only DERIVES `DAT_007f1a18[n]` from the table
// this function edits, so without it the table stays at its cold-boot zeros,
// every team resolves to -1, and the balance check can never return a legal
// split. Ported 2026-09-04; sole caller FUN_0043c000 @0x0043c44a (C4).
//
// Per profile i in 0..11 (loop bound: the processed pointer runs to 0x007f1894
// exclusive at stride 0x4c, giving exactly 12 iterations):
//   if i is one of the four assigned car slots (DAT_007f1a14/24/34/44):
//       if UP   newly pressed and table[i] != 0   -> table[i] -= 1
//       if DOWN newly pressed and table[i] <  2   -> table[i] += 1
//
// "newly pressed" is the established active/processed edge test (active != 0 &&
// processed == 0), and the columns are 0 = UP and 1 = DOWN -- NOT left/right.
// The decompiler renders them as the byte displacements `pcVar3[-0x4c0]` /
// `pcVar3[-0x4bf]` off the PROCESSED pointer, which resolve as:
//   active   UP   0x007f1044 + i*0x4c + 0      processed UP   0x007f1504 + ...
//   active   DOWN 0x007f1044 + i*0x4c + 1      processed DOWN 0x007f1505 + ...
// matching the column model already documented at exe_main.cpp:2205.
//
// So each player scrolls their own marker through THREE states, and the value
// is a 3-state selector, not a boolean:
//   0 = no team  (MenuTeamBalance yields team -1)
//   1 = team A   (team 0)
//   2 = team B   (team 1)
// which is why MenuTeamBalance subtracts 1, and why the Team Select screen has
// two team panels (measured at x=248/428, exe_main.cpp:4818) with a player able
// to sit in neither.
//
// The entry fixup `if (DAT_0067ea88 == 2) DAT_0067ea88 = 0;` is verbatim and is
// kept even though the port never sets that global to 2: it is the same
// DAT_0067ea88 the car-select arm reads to pick the race rule
// (FUN_0043dfd0: ea88 == 2 -> rule 2 AND DAT_0067ea64 = 1), so silently
// dropping it would change a determinant the standings screen reads.
// ---------------------------------------------------------------------------
static constexpr std::uintptr_t kTeamProfiles      = 12;         // loop count
static constexpr std::uintptr_t kInputRecStride    = 0x4c;       // per-profile
static constexpr std::uintptr_t kActiveBaseCol0    = 0x007f1044; // active   UP
static constexpr std::uintptr_t kProcessedBaseCol0 = 0x007f1504; // processed UP
static constexpr std::uintptr_t kModeSelGlobal     = 0x0067ea88; // DAT_0067ea88

// 0x0042fa00
extern "C" __declspec(dllexport) void __cdecl MenuTeamSelectTick() {
    // Latched BEFORE the loop in the original (slots 3 and 0 into registers).
    const std::int32_t iVar2 = *reinterpret_cast<std::int32_t*>(kPlayerSlotBase + 3 * 16);
    const std::int32_t iVar1 = *reinterpret_cast<std::int32_t*>(kPlayerSlotBase + 0 * 16);

    if (*reinterpret_cast<std::int32_t*>(kModeSelGlobal) == 2)
        *reinterpret_cast<std::int32_t*>(kModeSelGlobal) = 0;

    for (std::int32_t i = 0; i < static_cast<std::int32_t>(kTeamProfiles); ++i) {
        const std::int32_t slot2 = *reinterpret_cast<std::int32_t*>(kPlayerSlotBase + 2 * 16);
        const std::int32_t slot1 = *reinterpret_cast<std::int32_t*>(kPlayerSlotBase + 1 * 16);
        // Original's short-circuit order: slot3, slot2, slot1, slot0.
        if (iVar2 == i || slot2 == i || slot1 == i || iVar1 == i) {
            std::int32_t* const team = reinterpret_cast<std::int32_t*>(
                kTeamTableBase + static_cast<std::uintptr_t>(i) * 12);
            const std::uint8_t* const act =
                reinterpret_cast<const std::uint8_t*>(
                    kActiveBaseCol0 + static_cast<std::uintptr_t>(i) * kInputRecStride);
            const std::uint8_t* const prc =
                reinterpret_cast<const std::uint8_t*>(
                    kProcessedBaseCol0 + static_cast<std::uintptr_t>(i) * kInputRecStride);
            if (act[0] != 0 && prc[0] == 0 && *team != 0) *team -= 1;   // UP
            if (act[1] != 0 && prc[1] == 0 && *team <  2) *team += 1;   // DOWN
        }
    }
}

// INSTALLED HERE (dualcopy4, 2026-10-16). Until now 0x0042fa00 was hooked by the .asi-side PlayerSlotEdgeAdjust in
// Frontend/SkeletonAndScatter_t6.cpp (asi-only TU) while this copy, which both targets compile, registered nothing so as not to
// double-install (U-9065). That left two bodies that differ for any team value outside [0, 2]: the asi copy guarded `*team > 0` and
// clamped to [0, 2], the original does neither (verify/dualcopy4_20261016/decomp_42fa00.txt: guard `*piVar5 != 0` on the decrement,
// `< 2` on the increment, no clamp). This body follows the original, so it is the one body and now owns the install; the asi copy is
// deleted and its registry key player_slot_edge_adjust retired. The exe already called this one (exe_main.cpp, under `if (team)`).
// Verified path1 as `menu_team_select_tick` (cache_setter_observe, log/diff_menu_team_select_tick.csv 7/7 on 2026-09-04; re-run on
// the installed build in verify/dualcopy4_20261016/wb/).
RH_ScopedInstall(MenuTeamSelectTick, 0x0042fa00);

// ---------------------------------------------------------------------------
// 0x0042aff0  MenuButtonDetectA
// 0x0042b180  MenuButtonDetectB
// The two bodies are the same instruction stream with three operands changed:
// held column (0x007f1046 / 0x007f1047), previous-frame column (0x007f1506 /
// 0x007f1507) and hold timer (0x0067f1b0 / 0x0067f1b4). Re-ported from the
// bytes 2026-10-12 (negctl re-earn). The earlier port had three defects:
//   1. pad 7's held byte (0x007f125a / 0x007f125b) was not counted as held,
//      so "only pad 7 held" cleared the timer that the original keeps
//      (0x0042b062 jne 0x0042b0ad skips the clear at 0x0042b0f7);
//   2. both player-slot walks stepped 4 bytes over 8 records instead of
//      0x10 bytes over 4 records (0x0042b09c / 0x0042b163 `add esi, 0x10`);
//   3. the hold compare ran on the float-rounded timer, not on the x87 sum.
//
// Phase 1 (0x0042aff0..0x0042b0a8): held = any eligible pad's held byte != 0.
//   0x1000 path: pads 0..6 OR into EDI, pad 7 jumps straight to phase 2.
//   Otherwise: slot record i (0..3) is eligible when its id != -1 and
//   FUN_0040e470(i) != 2; the pad is the id (0x0042b08a imul 0x4c).
// Phase 2 (0x0042b0a9..0x0042b0f7): not held -> timer = 0 (0x0042b0f7), go to
//   phase 3. Held -> screen type *(0x0067ed3c + DAT_0067e9f8*0x40) in [6,7]
//   (signed, 0x0042b0bc jl / 0x0042b0c1 jg): timer += frame delta; when the
//   sum > *(0x005ccac0), timer = 0.2f and return 1. Otherwise phase 3 with the
//   timer as written.
// Phase 3 (0x0042b101..0x0042b173): return 1 on the first eligible pad with
//   held != 0 && previous == 0, else 0.
// ---------------------------------------------------------------------------

// 0x0042b0c6..0x0042b0e3 (A) / 0x0042b256..0x0042b273 (B):
//   fld [0x7f1004]; fadd [timer]; fst [timer]; fcomp [0x5ccac0];
//   fnstsw ax; test ah, 0x41; jne <not greater>
// The compare reads the unrounded ST0 sum at the live control word, so this is
// transcribed in x87 rather than as a float expression in an SSE2 TU.
static bool MenuHoldTimerStepExceeds(float* timer) {
    const float* delta = reinterpret_cast<const float*>(kFrameDelta);
    const float* thr   = reinterpret_cast<const float*>(kHoldThreshold);
    std::uint16_t sw;
    __asm {
        mov    eax, delta
        fld    dword ptr [eax]
        mov    eax, timer
        fadd   dword ptr [eax]
        fst    dword ptr [eax]
        mov    eax, thr
        fcomp  dword ptr [eax]
        fnstsw ax
        mov    sw, ax
    }
    return (sw & 0x4100u) == 0;  // C0 (below) and C3 (equal) both clear
}

static inline const std::int32_t* PlayerSlotRec(int i) {
    return reinterpret_cast<const std::int32_t*>(kPlayerSlotLoop + i * kPlayerSlotRecStride);
}

static inline char PadByte(std::uintptr_t base, std::int32_t pad) {
    return *reinterpret_cast<const char*>(base + static_cast<std::uintptr_t>(pad * kStride));
}

static std::uint32_t MenuButtonDetectHoldRepeat(std::uintptr_t heldBase, std::uintptr_t prevBase,
                                                std::uintptr_t timerAddr) {
    bool held = false;  // EDI
    float* timer = reinterpret_cast<float*>(timerAddr);

    if (*reinterpret_cast<const std::int32_t*>(kDispatchSel) == 0x1000) {
        for (int k = 0; k < 7; ++k) {                       // 0x0042b000..0x0042b05d
            if (PadByte(heldBase, k) != '\0') held = true;
        }
        if (PadByte(heldBase, 7) != '\0') held = true;      // 0x0042b062 jne 0x0042b0ad
    } else {
        for (int i = 0; reinterpret_cast<std::uintptr_t>(PlayerSlotRec(i)) < kPlayerSlotLoopEnd; ++i) {
            const std::int32_t id = *PlayerSlotRec(i);      // 0x0042b075
            if (id == -1) continue;
            if (CallSlotState(i) == 2) continue;            // 0x0042b07b
            if (PadByte(heldBase, id) != '\0') held = true; // 0x0042b08d
        }
    }

    if (!held) {
        *reinterpret_cast<std::uint32_t*>(timerAddr) = 0u; // 0x0042b0f7
    } else {
        const std::int32_t slot   = *reinterpret_cast<const std::int32_t*>(kSlotIndex);
        const std::int32_t screen = *reinterpret_cast<const std::int32_t*>(
            kScreenTypeBase + static_cast<std::uintptr_t>(slot) * kSlotStride);
        if (screen >= 6 && screen <= 7 && MenuHoldTimerStepExceeds(timer)) {
            *reinterpret_cast<std::uint32_t*>(timerAddr) = kTimerRearmBits;  // 0x0042b0e5
            return 1u;
        }
    }

    if (*reinterpret_cast<const std::int32_t*>(kDispatchSel) == 0x1000) {
        for (int k = 0; k < kPadCount; ++k) {               // 0x0042b10d..0x0042b129
            if (PadByte(heldBase, k) != '\0' && PadByte(prevBase, k) == '\0') return 1u;
        }
        return 0u;                                          // 0x0042b12c
    }
    for (int i = 0; reinterpret_cast<std::uintptr_t>(PlayerSlotRec(i)) < kPlayerSlotLoopEnd; ++i) {
        const std::int32_t id = *PlayerSlotRec(i);          // 0x0042b137
        if (id == -1) continue;
        if (CallSlotState(i) == 2) continue;                // 0x0042b13d
        if (PadByte(heldBase, id) != '\0' && PadByte(prevBase, id) == '\0') return 1u;  // 0x0042b14f / 0x0042b159
    }
    return 0u;                                              // 0x0042b170
}

// 0x0042aff0
extern "C" __declspec(dllexport) std::uint32_t __cdecl MenuButtonDetectA() {
    return MenuButtonDetectHoldRepeat(kHeldBaseA, kPrevBaseA, kTimerA);
}

RH_ScopedInstall(MenuButtonDetectA, 0x0042aff0);  // re-ported 2026-10-12 (negctl re-earn)

// 0x0042b180
extern "C" __declspec(dllexport) std::uint32_t __cdecl MenuButtonDetectB() {
    return MenuButtonDetectHoldRepeat(kHeldBaseB, kPrevBaseB, kTimerB);
}

RH_ScopedInstall(MenuButtonDetectB, 0x0042b180);  // re-ported 2026-10-12 (negctl re-earn)
