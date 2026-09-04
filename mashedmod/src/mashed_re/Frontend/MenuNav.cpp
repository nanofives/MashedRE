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

// Button-detection globals.
static constexpr std::uintptr_t kDispatchSel       = 0x007f1a0c; // 0x1000 = path A
static constexpr std::uintptr_t kActiveByteBase2   = 0x007f1046; // active byte col +2 (path A base)
static constexpr std::uintptr_t kActiveByteBase3   = 0x007f1047; // active byte col +3
static constexpr std::uintptr_t kProcessedBase4    = 0x007f1506; // processed byte col +4
static constexpr std::uintptr_t kProcessedBase5    = 0x007f1507; // processed byte col +5
static constexpr int            kStride            = 0x4c;       // per-player record stride
static constexpr std::uintptr_t kPathARow8Sentinel2= 0x7f125a;   // path A row-8 sentinel (col+2)
static constexpr std::uintptr_t kPathARow8Sentinel3= 0x7f125b;   // path A row-8 sentinel (col+3)
static constexpr std::uintptr_t kPathAUpperBound4  = 0x7f1765;   // path A processed upper (col+4)
static constexpr std::uintptr_t kPathAUpperBound5  = 0x7f1766;   // path A processed upper (col+5)
static constexpr std::uintptr_t kScreenTypeBase    = 0x0067ed3c; // screen-type field base
static constexpr std::uintptr_t kSlotStride        = 0x40;       // slot stride for screen-type
static constexpr std::uintptr_t kTimerA            = 0x0067f1b0; // hold-timer A (float)
static constexpr std::uintptr_t kTimerB            = 0x0067f1b4; // hold-timer B (float)
static constexpr std::uintptr_t kFrameDelta        = 0x007f1004; // per-frame delta float
static constexpr std::uintptr_t kHoldThreshold     = 0x005ccac0; // hold threshold float
static constexpr std::uintptr_t kPlayerSlotLoop    = 0x007f1a14; // player slot array start (path B)
static constexpr std::uintptr_t kPlayerSlotLoopEnd = 0x7f1a54;   // exclusive end of 8 * 4 bytes

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
// Traverses the active slot's entry array, skipping (cursor+1) inner groups,
// then does a final scan stopping at 0xFF05/14/060000 sentinels.
// Returns *(iVar1 + 4 + iVar3*4) unless the sentinel is 0xFF060000 (-1).
// Pure read-only, no side-effects.
// ---------------------------------------------------------------------------
// 0x0042ac90
extern "C" __declspec(dllexport) std::uint32_t __cdecl MenuEntryGet() {
    const std::int32_t slot  = *reinterpret_cast<std::int32_t*>(kSlotIndex);
    const auto iVar1 = *reinterpret_cast<std::int32_t*>(kPtrTableBase + static_cast<std::int32_t>(slot) * 0x40);
    std::int32_t iVar3 = 0;

    const std::int32_t cursor = *reinterpret_cast<std::int32_t*>(kCursorTableBase + static_cast<std::int32_t>(slot) * 0x40);

    // Phase: skip (cursor+1) inner groups if cursor != -1
    if (cursor != -1) {
        std::int32_t iVar4 = cursor + 1;
        do {
            // Inner for-loop: advance iVar3 until 0xFF040000 entry
            std::int32_t iVar2 = *reinterpret_cast<std::int32_t*>(iVar1 + iVar3 * 4);
            while (static_cast<std::uint32_t>(iVar2) != kSentInnerTerm) {
                // Body: update iVar2 = iVar3*4, then advance iVar3, then read next
                iVar2 = iVar3 * 4;
                iVar3 += 1;
                iVar2 = *reinterpret_cast<std::int32_t*>(iVar1 + 4 + iVar2);
            }
            iVar3 += 1; // advance past 0xFF040000 entry
            iVar4 -= 1;
        } while (iVar4 > 0);
    }

    // Final scan: advance until we hit a stopping sentinel
    std::int32_t iVar4 = *reinterpret_cast<std::int32_t*>(iVar1 + iVar3 * 4);
    while (static_cast<std::uint32_t>(iVar4) != kSentFinalA &&
           static_cast<std::uint32_t>(iVar4) != kSentFinalB &&
           static_cast<std::uint32_t>(iVar4) != kSentFinalC) {
        iVar4 = iVar3 * 4;
        iVar3 += 1;
        iVar4 = *reinterpret_cast<std::int32_t*>(iVar1 + 4 + iVar4);
    }

    if (static_cast<std::uint32_t>(*reinterpret_cast<std::int32_t*>(iVar1 + iVar3 * 4)) == kSentFinalC) {
        return 0xffffffffu;
    }
    return *reinterpret_cast<std::uint32_t*>(iVar1 + 4 + iVar3 * 4);
}

// MASS-DISABLED 2026-05-24 needs-canonical-menu-ptr-table: RH_ScopedInstall(MenuEntryGet, 0x0042ac90);
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

// DELIBERATELY NOT INSTALLED. 0x0042fa00 already has a live hook: the
// .asi-side PlayerSlotEdgeAdjust in Frontend/SkeletonAndScatter_t6.cpp (C3,
// GREEN 0/10). This is its EXE-SIDE TWIN and exists only because that TU is in
// asi_sources.rsp but NOT in build.bat's exe list, so the standalone cannot
// reach it -- which is the actual reason the standalone could not assign teams.
// MenuNav.cpp is in both lists, so putting the copy here makes it available to
// mashed_re.exe without dragging a PIZ/Im2D-heavy TU into the exe link.
//
// A second RH_ScopedInstall on one RVA is the U-9065 failure mode: both
// register, one silently wins, and the other becomes dead code that still reads
// as C3. This copy therefore registers nothing, and the .asi keeps the
// established install.
//
// KNOWN DIVERGENCE between the twins, both inert, recorded so it cannot drift
// unnoticed: PlayerSlotEdgeAdjust guards with `*piVar5 > 0` and then adds a
// trailing `if (<0) =0; if (>2) =2;` clamp pair. The original has NEITHER --
// it guards `*piVar5 != 0` and clamps only through the two ±1 guards
// (0x0042fa00 body). Both forms are equivalent for values already in [0,2],
// which is why that copy diffed GREEN. This copy follows the original.
// Verified path1 GREEN 7/7 non-degenerate as `menu_team_select_tick`
// (log/diff_menu_team_select_tick.csv).

// ---------------------------------------------------------------------------
// 0x0042aff0  MenuButtonDetectA
// Detects a "button pressed" event on byte-column +2 (0x7f1046) with hold-
// repeat for screens 6-7, using timer _DAT_0067f1b0.
// Calls FUN_0040e470(iVar5) to check AI status; skip if == 2.
// Returns 1 if change detected, 0 otherwise.
// ---------------------------------------------------------------------------
// 0x0042aff0
extern "C" __declspec(dllexport) std::uint32_t __cdecl MenuButtonDetectA() {
    bool bVar1 = false;

    if (*reinterpret_cast<std::int32_t*>(kDispatchSel) == 0x1000) {
        // Path A: check 7 fixed rows at stride 0x4c from 0x7f1046
        for (int n = 0; n < 7; n++) {
            if (*reinterpret_cast<char*>(kActiveByteBase2 + n * kStride) != '\0') {
                bVar1 = true;
                break;
            }
        }
        // Row-8 sentinel check: if non-zero, jump to hold-repeat check
        if (*reinterpret_cast<char*>(kPathARow8Sentinel2) == '\0') {
            goto hold_repeat;
        }
    } else {
        // Path B: iterate player-slot array
        auto* piVar4 = reinterpret_cast<std::int32_t*>(kPlayerSlotLoop);
        int iVar5 = 0;
        while (reinterpret_cast<std::uintptr_t>(piVar4) < kPlayerSlotLoopEnd) {
            const std::int32_t carSlot = *piVar4;
            if (carSlot != -1 && CallSlotState(iVar5) != 2) {
                if (*reinterpret_cast<char*>(kActiveByteBase2 + carSlot * kStride) != '\0') {
                    bVar1 = true;
                }
            }
            piVar4 = reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uintptr_t>(piVar4) + 4);
            iVar5++;
        }
        goto hold_repeat;
    }

hold_repeat:
    {
        const std::int32_t slot   = *reinterpret_cast<std::int32_t*>(kSlotIndex);
        const std::int32_t screen = *reinterpret_cast<std::int32_t*>(kScreenTypeBase + slot * kSlotStride);
        if (bVar1 && screen > 5 && screen < 8) {
            float& timer = *reinterpret_cast<float*>(kTimerA);
            timer += *reinterpret_cast<float*>(kFrameDelta);
            if (timer > *reinterpret_cast<float*>(kHoldThreshold)) {
                timer = 0.2f;
                return 1u;
            }
        } else if (!bVar1) {
            *reinterpret_cast<float*>(kTimerA) = 0.0f;
        }
    }

    // Phase 3: check processed byte at column +4
    if (*reinterpret_cast<std::int32_t*>(kDispatchSel) == 0x1000) {
        // Path A
        auto* pcVar2 = reinterpret_cast<char*>(kProcessedBase4);
        while (true) {
            // Skip if active byte (col+2 = pcVar2 - 0x4c0) is zero OR processed byte is non-zero
            if (*reinterpret_cast<char*>(reinterpret_cast<std::uintptr_t>(pcVar2) - 0x4c0) == '\0' ||
                *pcVar2 != '\0') {
                pcVar2 = reinterpret_cast<char*>(reinterpret_cast<std::uintptr_t>(pcVar2) + kStride);
                if (reinterpret_cast<std::uintptr_t>(pcVar2) > kPathAUpperBound4) {
                    return 0u;
                }
            } else {
                break;
            }
        }
    } else {
        // Path B
        auto* piVar4 = reinterpret_cast<std::int32_t*>(kPlayerSlotLoop);
        int iVar5 = 0;
        while (reinterpret_cast<std::uintptr_t>(piVar4) < kPlayerSlotLoopEnd) {
            const std::int32_t carSlot = *piVar4;
            if (carSlot == -1 || CallSlotState(iVar5) == 2) {
                piVar4 = reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uintptr_t>(piVar4) + 4);
                iVar5++;
                continue;
            }
            if (*reinterpret_cast<char*>(kActiveByteBase2 + carSlot * kStride) == '\0' ||
                *reinterpret_cast<char*>(kProcessedBase4 + carSlot * kStride) != '\0') {
                piVar4 = reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uintptr_t>(piVar4) + 4);
                iVar5++;
                continue;
            }
            // Found a valid unprocessed input
            break;
            piVar4 = reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uintptr_t>(piVar4) + 4);
            iVar5++;
        }
        if (reinterpret_cast<std::uintptr_t>(piVar4) >= kPlayerSlotLoopEnd) {
            return 0u;
        }
    }

    return 1u;
}

RH_ScopedInstall(MenuButtonDetectA, 0x0042aff0);  // re-enabled 2026-05-24 (pre-regression C4 leaf)

// ---------------------------------------------------------------------------
// 0x0042b180  MenuButtonDetectB
// Sister of MenuButtonDetectA. Checks byte-column +3 (0x7f1047) and processed
// col +5 (0x7f1507), with timer _DAT_0067f1b4.
// Structurally identical to MenuButtonDetectA — see 0x0042aff0.md.
// ---------------------------------------------------------------------------
// 0x0042b180
extern "C" __declspec(dllexport) std::uint32_t __cdecl MenuButtonDetectB() {
    bool bVar1 = false;

    if (*reinterpret_cast<std::int32_t*>(kDispatchSel) == 0x1000) {
        // Path A: check 7 fixed rows at stride 0x4c from 0x7f1047
        for (int n = 0; n < 7; n++) {
            if (*reinterpret_cast<char*>(kActiveByteBase3 + n * kStride) != '\0') {
                bVar1 = true;
                break;
            }
        }
        if (*reinterpret_cast<char*>(kPathARow8Sentinel3) == '\0') {
            goto hold_repeat_b;
        }
    } else {
        // Path B: iterate player-slot array
        auto* piVar4 = reinterpret_cast<std::int32_t*>(kPlayerSlotLoop);
        int iVar5 = 0;
        while (reinterpret_cast<std::uintptr_t>(piVar4) < kPlayerSlotLoopEnd) {
            const std::int32_t carSlot = *piVar4;
            if (carSlot != -1 && CallSlotState(iVar5) != 2) {
                if (*reinterpret_cast<char*>(kActiveByteBase3 + carSlot * kStride) != '\0') {
                    bVar1 = true;
                }
            }
            piVar4 = reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uintptr_t>(piVar4) + 4);
            iVar5++;
        }
        goto hold_repeat_b;
    }

hold_repeat_b:
    {
        const std::int32_t slot   = *reinterpret_cast<std::int32_t*>(kSlotIndex);
        const std::int32_t screen = *reinterpret_cast<std::int32_t*>(kScreenTypeBase + slot * kSlotStride);
        if (bVar1 && screen > 5 && screen < 8) {
            float& timer = *reinterpret_cast<float*>(kTimerB);
            timer += *reinterpret_cast<float*>(kFrameDelta);
            if (timer > *reinterpret_cast<float*>(kHoldThreshold)) {
                timer = 0.2f;
                return 1u;
            }
        } else if (!bVar1) {
            *reinterpret_cast<float*>(kTimerB) = 0.0f;
        }
    }

    // Phase 3: check processed byte at column +5
    if (*reinterpret_cast<std::int32_t*>(kDispatchSel) == 0x1000) {
        // Path A
        auto* pcVar2 = reinterpret_cast<char*>(kProcessedBase5);
        while (true) {
            if (*reinterpret_cast<char*>(reinterpret_cast<std::uintptr_t>(pcVar2) - 0x4c0) == '\0' ||
                *pcVar2 != '\0') {
                pcVar2 = reinterpret_cast<char*>(reinterpret_cast<std::uintptr_t>(pcVar2) + kStride);
                if (reinterpret_cast<std::uintptr_t>(pcVar2) > kPathAUpperBound5) {
                    return 0u;
                }
            } else {
                break;
            }
        }
    } else {
        // Path B
        auto* piVar4 = reinterpret_cast<std::int32_t*>(kPlayerSlotLoop);
        int iVar5 = 0;
        while (reinterpret_cast<std::uintptr_t>(piVar4) < kPlayerSlotLoopEnd) {
            const std::int32_t carSlot = *piVar4;
            if (carSlot == -1 || CallSlotState(iVar5) == 2) {
                piVar4 = reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uintptr_t>(piVar4) + 4);
                iVar5++;
                continue;
            }
            if (*reinterpret_cast<char*>(kActiveByteBase3 + carSlot * kStride) == '\0' ||
                *reinterpret_cast<char*>(kProcessedBase5 + carSlot * kStride) != '\0') {
                piVar4 = reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uintptr_t>(piVar4) + 4);
                iVar5++;
                continue;
            }
            break;
            piVar4 = reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uintptr_t>(piVar4) + 4);
            iVar5++;
        }
        if (reinterpret_cast<std::uintptr_t>(piVar4) >= kPlayerSlotLoopEnd) {
            return 0u;
        }
    }

    return 1u;
}

RH_ScopedInstall(MenuButtonDetectB, 0x0042b180);  // re-enabled 2026-05-24 (pre-regression C4 leaf)
