// Mashed RE - Frontend button-detect and car-slot reimplementations.
// Analysis notes: re/analysis/frontend_promote_menus_a/
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Callee dependencies (C2 drift-promoted from C1):
//   0x0040e470  FUN_0040e470 (C2)
//     undefined4 FUN_0040e470(int param_1)
//     Returns *(PTR_PTR_005f2770 + param_1*4 + 0x34)
//   0x0040e480  FUN_0040e480 (C2)
//     void FUN_0040e480(int param_1, undefined4 param_2)
//     Writes *(PTR_PTR_005f2770 + param_1*4 + 0x34) = param_2

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

// Dispatch selector: 0x1000 = path A (single-player fixed-index mode).
static constexpr std::uintptr_t kDispatchSel    = 0x007f1a0c;
// Active byte array col +0 base (stride 0x4c, 8 slots = [0..6] + sentinel at 0x7f1258)
static constexpr std::uintptr_t kActiveBase0    = 0x007f1044;
// Active byte array col +5 base (stride 0x4c)
static constexpr std::uintptr_t kActiveBase5    = 0x007f1049;
// Processed byte array col +0 base (stride 0x4c)
static constexpr std::uintptr_t kProcessedBase0 = 0x007f1504;
// Processed byte array col +5 base (stride 0x4c)
static constexpr std::uintptr_t kProcessedBase5 = 0x007f1509;
// Per-player record stride in bytes
static constexpr int            kStride         = 0x4c;
// Path A row 7 active addresses (col +5 sentinel)
static constexpr std::uintptr_t kPathARow7Active5    = 0x007f125d; // 0x7f1049 + 7*0x4c
static constexpr std::uintptr_t kPathARow7Processed5 = 0x007f171d; // 0x7f1509 + 7*0x4c
// Path A upper bound for B-side scan (col +5)
static constexpr std::uintptr_t kPathBUpperBound     = 0x7f1a54;   // end of 8-slot player array

// Car-slot index array (4 slots of 4 bytes each, stride 4)
static constexpr std::uintptr_t kPlayerSlotArray = 0x007f1a14;
// Player slot array end (exclusive)
static constexpr std::uintptr_t kPlayerSlotEnd   = 0x7f1a54;

// Screen-type and cursor globals for 0x0042b310
static constexpr std::uintptr_t kSlotIndex       = 0x0067e9f8;
static constexpr std::uintptr_t kScreenTypeBase  = 0x0067ed3c; // screen-type at slot*0x40
static constexpr std::uintptr_t kCursorBase      = 0x0067ed40; // cursor at slot*0x40
static constexpr std::uintptr_t kNoInputFlag     = 0x0067f1b8; // zeroed when no active input (col+0)

// Screen IDs used in 0x0042b310 path
static constexpr std::int32_t   kScreenId13      = 0x13;
static constexpr std::int32_t   kScreenId1e      = 0x1e;

// Callee 0x0040e470: returns DWORD at PTR_PTR_005f2770 + param*4 + 0x34.
typedef std::uint32_t (__cdecl *FunSlotStateFn)(int param_1);
static constexpr std::uintptr_t kFun0040e470     = 0x0040e470;
static inline std::uint32_t CallSlotState(int idx) {
    return reinterpret_cast<FunSlotStateFn>(kFun0040e470)(idx);
}

// Callee 0x0040e480: writes DWORD at PTR_PTR_005f2770 + param_1*4 + 0x34 = param_2.
typedef void (__cdecl *FunSlotWriteFn)(int param_1, std::uint32_t param_2);
static constexpr std::uintptr_t kFun0040e480     = 0x0040e480;
static inline void CallSlotWrite(int idx, std::uint32_t val) {
    reinterpret_cast<FunSlotWriteFn>(kFun0040e480)(idx, val);
}

// ---------------------------------------------------------------------------
// 0x0042b770  MenuButtonDetectE
//
// Simplified button-detector: checks active byte col +5 AND !processed byte col +5.
// No timer, no screen-type check.
// Path A (DAT_007f1a0c == 0x1000): fixed stride scan N=0..6 + special-case N=7.
// Path B: car-slot index array at 0x007f1a14, filter via FUN_0040e470 != 2.
// Returns 1 (change detected) or 0.
// ref: re/analysis/frontend_promote_menus_a/0x0042b770.md
// ---------------------------------------------------------------------------

// 0x0042b770
extern "C" __declspec(dllexport) std::uint32_t __cdecl MenuButtonDetectE() {
    std::uint32_t uVar2 = 0;

    const std::int32_t sel = *reinterpret_cast<std::int32_t*>(kDispatchSel);
    if (sel == 0x1000) {
        // Path A: iterate slots 0..6 by stride
        for (int n = 0; n < 7; n++) {
            const auto active   = reinterpret_cast<const char*>(kActiveBase5 + n * kStride);
            const auto procsd   = reinterpret_cast<const char*>(kProcessedBase5 + n * kStride);
            if (*active != '\0' && *procsd == '\0') {
                uVar2 = 1;
            }
        }
        // Slot 7 sentinel
        if (*reinterpret_cast<const char*>(kPathARow7Active5) != '\0' &&
            *reinterpret_cast<const char*>(kPathARow7Processed5) == '\0') {
            return 1;
        }
    } else {
        // Path B: iterate player-slot index array
        const std::int32_t* piVar3 = reinterpret_cast<std::int32_t*>(kPlayerSlotArray);
        for (int iVar4 = 0; iVar4 < 8; iVar4++, piVar3++) {
            if (reinterpret_cast<std::uintptr_t>(piVar3) >= kPlayerSlotEnd) break;
            const std::int32_t slotIdx = *piVar3;
            if (slotIdx == -1) continue;
            if (CallSlotState(iVar4) == 2) continue; // U-3445: skip state-2 slots
            const auto active = reinterpret_cast<const char*>(kActiveBase5 + slotIdx * kStride);
            const auto procsd = reinterpret_cast<const char*>(kProcessedBase5 + slotIdx * kStride);
            if (*active != '\0' && *procsd == '\0') {
                uVar2 = 1;
            }
        }
    }

    return uVar2;
}

RH_ScopedInstall(MenuButtonDetectE, 0x0042b770);  // re-enabled 2026-05-24 batch-frontend

// ---------------------------------------------------------------------------
// 0x0042b310  MenuButtonDetectC
//
// Full button-detector for byte col +0: active (0x7f1044) AND !processed (0x7f1504).
// Phase 1: detect if any active input (bVar1). If none, zero _DAT_0067f1b8 -> phase 3.
// Screen-type branch (if bVar1):
//   screen == 0x13: cursor==2 -> return 1; else -> phase 3.
//   screen == 0x1e: return 1 immediately.
//   else: -> phase 3.
// Phase 3: detect active AND !processed -> uVar3.
// Returns 0 or 1.
// ref: re/analysis/frontend_promote_menus_a/0x0042b310.md
// ---------------------------------------------------------------------------

// 0x0042b310
extern "C" __declspec(dllexport) std::uint32_t __cdecl MenuButtonDetectC() {
    const std::int32_t sel = *reinterpret_cast<std::int32_t*>(kDispatchSel);

    // Phase 1 — detect active input at col +0
    bool bVar1 = false;
    if (sel == 0x1000) {
        // Path A: fixed stride
        for (int n = 0; n < 7; n++) {
            if (*reinterpret_cast<const char*>(kActiveBase0 + n * kStride) != '\0') {
                bVar1 = true;
            }
        }
        // Slot 7 sentinel (0x7f1044 + 7*0x4c = 0x7f1258)
        if (*reinterpret_cast<const char*>(0x7f1258) != '\0') {
            bVar1 = true;
        }
    } else {
        // Path B: car-slot index array
        const std::int32_t* piVar4 = reinterpret_cast<std::int32_t*>(kPlayerSlotArray);
        for (int iVar5 = 0; iVar5 < 8; iVar5++, piVar4++) {
            if (reinterpret_cast<std::uintptr_t>(piVar4) >= kPlayerSlotEnd) break;
            const std::int32_t slotIdx = *piVar4;
            if (slotIdx == -1) continue;
            if (CallSlotState(iVar5) == 2) continue; // U-3445
            if (*reinterpret_cast<const char*>(kActiveBase0 + slotIdx * kStride) != '\0') {
                bVar1 = true;
            }
        }
    }

    if (!bVar1) {
        // Phase 2: no active input — zero the flag and go to phase 3
        *reinterpret_cast<std::int32_t*>(kNoInputFlag) = 0;
        // fall through to phase 3 (uVar3 will remain 0)
    } else {
        // Screen-type branch
        const std::int32_t slot   = *reinterpret_cast<std::int32_t*>(kSlotIndex);
        const std::int32_t screen = *reinterpret_cast<std::int32_t*>(kScreenTypeBase + slot * 0x40);
        const std::int32_t cursor = *reinterpret_cast<std::int32_t*>(kCursorBase + slot * 0x40);

        if (screen == kScreenId13) {
            if (cursor == 2) return 1;
            // cursor == 0 or 1: fall through to phase 3
        } else if (screen == kScreenId1e) {
            return 1;
        }
        // else: fall through to phase 3
    }

    // Phase 3 — detect active AND !processed at col +0
    std::uint32_t uVar3 = 0;
    if (sel == 0x1000) {
        // Path A
        for (int n = 0; n < 8; n++) {
            if (*reinterpret_cast<const char*>(kActiveBase0 + n * kStride) != '\0' &&
                *reinterpret_cast<const char*>(kProcessedBase0 + n * kStride) == '\0') {
                uVar3 = 1;
            }
        }
    } else {
        // Path B
        const std::int32_t* piVar4 = reinterpret_cast<std::int32_t*>(kPlayerSlotArray);
        for (int iVar5 = 0; iVar5 < 8; iVar5++, piVar4++) {
            if (reinterpret_cast<std::uintptr_t>(piVar4) >= kPlayerSlotEnd) break;
            const std::int32_t slotIdx = *piVar4;
            if (slotIdx == -1) continue;
            if (CallSlotState(iVar5) == 2) continue;
            if (*reinterpret_cast<const char*>(kActiveBase0 + slotIdx * kStride) != '\0' &&
                *reinterpret_cast<const char*>(kProcessedBase0 + slotIdx * kStride) == '\0') {
                uVar3 = 1;
            }
        }
    }

    return uVar3;
}

RH_ScopedInstall(MenuButtonDetectC, 0x0042b310);  // re-enabled 2026-05-24 batch-frontend

// ---------------------------------------------------------------------------
// 0x0042b960  CarSlotInit1P
//
// Initializes the single-player car slot:
//   1. DAT_007f1a14 = 0.
//   2. Scans stride-0x4c table pair (0x7f1042/0x7f1502) to find first valid entry
//      where table A[i] != 0 AND table B[i] == 0; writes index to DAT_007f1a14.
//   3. DAT_007f1a24 = DAT_007f1a34 = DAT_007f1a44 = 0xffffffff.
//   4. DAT_007f1a0c = 1.
//   5. FUN_0040e480(0,1), FUN_0040e480(1,0), FUN_0040e480(2,0), FUN_0040e480(3,0).
// Returns void.
// ref: re/analysis/frontend_promote_menus_a/0x0042b960.md
// ---------------------------------------------------------------------------

static constexpr std::uintptr_t kTableA_base = 0x007f1042; // active: char table A
static constexpr std::uintptr_t kTableB_base = 0x007f1502; // processed: char table B
static constexpr std::uintptr_t kScanBound   = 0x7f1762;   // exclusive upper bound for scan

static constexpr std::uintptr_t kSlot1       = 0x007f1a14;
static constexpr std::uintptr_t kSlot2       = 0x007f1a24;
static constexpr std::uintptr_t kSlot3       = 0x007f1a34;
static constexpr std::uintptr_t kSlot4       = 0x007f1a44;
static constexpr std::uintptr_t kModeSel     = 0x007f1a0c;

// 0x0042b960
extern "C" __declspec(dllexport) void __cdecl CarSlotInit1P() {
    // Step 1
    *reinterpret_cast<std::int32_t*>(kSlot1) = 0;

    // Step 2: scan table B (stride 0x4c) looking for entry where A[i] != 0 && B[i] == 0
    const char* pcVar1 = reinterpret_cast<const char*>(kTableB_base);
    // Offset from table B to table A = 0x7f1042 - 0x7f1502 = -0x4c0
    // (i.e., tableA_base = pcVar1 - 0x4c0 at each iteration)
    std::int32_t foundIdx = 0;
    std::int32_t idx = 0;
    do {
        const char* tableA_entry = pcVar1 - 0x4c0;
        if (*tableA_entry != '\0' && *pcVar1 == '\0') {
            // Found: write current index
            *reinterpret_cast<std::int32_t*>(kSlot1) = idx;
            break;
        }
        pcVar1 = pcVar1 + kStride;
        idx += 1;
        (void)foundIdx;
    } while (reinterpret_cast<std::uintptr_t>(pcVar1) < kScanBound);

    // Step 3
    *reinterpret_cast<std::uint32_t*>(kSlot2) = 0xffffffffu;
    *reinterpret_cast<std::uint32_t*>(kSlot3) = 0xffffffffu;
    *reinterpret_cast<std::uint32_t*>(kSlot4) = 0xffffffffu;

    // Step 4
    *reinterpret_cast<std::int32_t*>(kModeSel) = 1;

    // Step 5
    CallSlotWrite(0, 1);
    CallSlotWrite(1, 0);
    CallSlotWrite(2, 0);
    CallSlotWrite(3, 0);
}

RH_ScopedInstall(CarSlotInit1P, 0x0042b960);  // re-enabled 2026-05-24 c3-frontend-a

// ---------------------------------------------------------------------------
// 0x0042b9e0  CarSlotAssign
//
// Car-selection confirm / player-slot assigner.
// Step 1: reset 4 player slots via FUN_0040e480(0..3, 0).
// Step 2: count valid car options at DAT_0067eafc (stride 0x12 ints, 6 offsets).
//         If count < 2: return 1.
// Step 3: collision detection in DAT_0067eaf0 stride-3 array (12 entries).
//         If collision (two equal positive entries): return 0.
// Step 4: clear slot table 0x007f1a14..0x7f1a54 to 0xffffffff.
// Step 5: assign car IDs to player slots; FUN_0040e480(player, 1) per assignment.
//         Returns 0x1000 when all 4 assigned or iVar4 == 7.
// Returns: 0x1000 (success), 0 (collision), 1 (auto-confirm).
// ref: re/analysis/frontend_promote_menus_a/0x0042b9e0.md
// ---------------------------------------------------------------------------

static constexpr std::uintptr_t kCarOptionBase   = 0x0067eafc; // car-option validity array
static constexpr std::uintptr_t kCarOptionBound  = 0x0067eb8c; // exclusive upper bound (step 2)
static constexpr std::int32_t   kCarOptionStride = 0x12;       // stride in int* units (72 bytes)
static constexpr std::uintptr_t kCarChoiceBase   = 0x0067eaf0; // player car-choice array
static constexpr std::uintptr_t kCollOuterBound  = 0x0067eb74; // collision scan outer bound
static constexpr std::uintptr_t kCollInnerBound  = 0x0067eb80; // collision scan inner bound
static constexpr std::uintptr_t kSlotTableBase   = 0x007f1a14; // slot index table
static constexpr std::uintptr_t kSlotTableBound  = 0x7f1a54;   // slot table clear bound
static constexpr std::uintptr_t kCarIdTableBase  = 0x007f1a1c; // car-ID table (+8 from slot table)
static constexpr std::uintptr_t kCarIdTableBound = 0x7f1a5b;   // assignment loop upper bound
// kModeSel already defined above (0x007f1a0c)

// 0x0042b9e0
extern "C" __declspec(dllexport) std::uint32_t __cdecl CarSlotAssign() {
    // Step 1: reset all 4 player slots to 0
    for (int iVar5 = 0; iVar5 < 4; iVar5++) {
        CallSlotWrite(iVar5, 0);
    }

    // Step 2: count valid car options
    std::int32_t iVar5 = 0xc; // starts at 12 (total option fields)
    const std::int32_t* piVar2 = reinterpret_cast<const std::int32_t*>(kCarOptionBase);
    do {
        // Check 6 offsets: [-3], [0], [3], [6], [9], [0xc] in int* units
        static const int offsets[] = { -3, 0, 3, 6, 9, 0xc };
        for (int o : offsets) {
            if (piVar2[o] < 1) iVar5 -= 1;
        }
        piVar2 = reinterpret_cast<const std::int32_t*>(
            reinterpret_cast<std::uintptr_t>(piVar2) + kCarOptionStride * 4);
    } while (reinterpret_cast<std::uintptr_t>(piVar2) < kCarOptionBound);

    if (iVar5 < 2) return 1; // fewer than 2 valid options -> early OK

    // Step 3: collision detection
    bool bVar1 = false;
    const std::int32_t* piVar2b = reinterpret_cast<const std::int32_t*>(kCarChoiceBase);
    while (reinterpret_cast<std::uintptr_t>(piVar2b) < kCollOuterBound) {
        const std::int32_t* piVar6 = piVar2b + 3;
        while (reinterpret_cast<std::uintptr_t>(piVar6) < kCollInnerBound) {
            if (*piVar2b == *piVar6 && *piVar2b > 0) bVar1 = true;
            piVar6 += 3;
        }
        piVar2b += 3;
    }
    if (bVar1) return 0; // collision detected

    // Step 4: clear slot table to 0xffffffff
    std::uint32_t* puVar3 = reinterpret_cast<std::uint32_t*>(kSlotTableBase);
    do {
        *puVar3 = 0xffffffffu;
        puVar3 = reinterpret_cast<std::uint32_t*>(
            reinterpret_cast<std::uintptr_t>(puVar3) + 16);
    } while (reinterpret_cast<std::uintptr_t>(puVar3) < kSlotTableBound);

    // Step 5: assign car IDs to player slots
    // Outer do-while: advances player-slot pointer piVar2c and player index local_4.
    // Inner do-while: steps car-ID iVar4 from 1..6 until a match is found.
    // When iVar4 == 7 or piVar2c exceeds bound: DAT_007f1a0c=2, return 0x1000.
    std::int32_t local_4 = 0; // player index
    std::int32_t* piVar2c = reinterpret_cast<std::int32_t*>(kCarIdTableBase);
    std::int32_t iVar4 = 1; // car ID starts at 1
    std::int32_t local_8 = 0; // 0=no-match, 1=matched, 2=exhausted

    do {
        if (local_8 == 2) {
            *reinterpret_cast<std::int32_t*>(kModeSel) = 2;
            return 0x1000u;
        }
        local_8 = 0;

        // Inner scan: find player whose car choice == iVar4
        const std::int32_t* piVar6 = reinterpret_cast<const std::int32_t*>(kCarChoiceBase);
        std::int32_t iVar5b = 0;
        while (reinterpret_cast<std::uintptr_t>(piVar6) < kCollInnerBound) {
            if (*piVar6 == iVar4) {
                // write slot index and car-ID-1 into current player record
                *(piVar2c - 2) = iVar5b;  // piVar2c[-2] = choice array index
                *piVar2c = iVar4 - 1;     // piVar2c[0]  = car-ID-1
                CallSlotWrite(local_4, 1);
                local_8 = 1;
            }
            piVar6 += 3;
            iVar5b += 1;
        }

        iVar4 += 1;
        if (iVar4 == 7) { local_8 = 2; continue; }

    } while (local_8 == 0);  // continue scanning if no match this round

    // Advance to next player slot (only reached when local_8 == 1 or 2)
    if (local_8 == 2) {
        *reinterpret_cast<std::int32_t*>(kModeSel) = 2;
        return 0x1000u;
    }

    local_4 += 1;
    piVar2c += 4;
    if (reinterpret_cast<std::uintptr_t>(piVar2c) > kCarIdTableBound) {
        *reinterpret_cast<std::int32_t*>(kModeSel) = 2;
        return 0x1000u;
    }

    // Continue outer assignment loop for remaining players
    do {
        if (local_8 == 2) {
            *reinterpret_cast<std::int32_t*>(kModeSel) = 2;
            return 0x1000u;
        }
        local_8 = 0;

        const std::int32_t* piVar6 = reinterpret_cast<const std::int32_t*>(kCarChoiceBase);
        std::int32_t iVar5b = 0;
        while (reinterpret_cast<std::uintptr_t>(piVar6) < kCollInnerBound) {
            if (*piVar6 == iVar4) {
                *(piVar2c - 2) = iVar5b;
                *piVar2c = iVar4 - 1;
                CallSlotWrite(local_4, 1);
                local_8 = 1;
            }
            piVar6 += 3;
            iVar5b += 1;
        }

        iVar4 += 1;
        if (iVar4 == 7) { local_8 = 2; continue; }

        if (local_8 != 0) {
            local_4 += 1;
            piVar2c += 4;
            if (reinterpret_cast<std::uintptr_t>(piVar2c) > kCarIdTableBound) {
                *reinterpret_cast<std::int32_t*>(kModeSel) = 2;
                return 0x1000u;
            }
        }
    } while (true);
}

RH_ScopedInstall(CarSlotAssign, 0x0042b9e0);  // re-enabled 2026-05-24 c3-frontend-a

// ---------------------------------------------------------------------------
// 0x0042b540  MenuButtonDetectD
//
// Structural sibling of MenuButtonDetectC (0x0042b310). Differences only:
//   - Active byte col: +1 (0x7f1045 vs 0x7f1044)
//   - Processed byte col: +1 (0x7f1505 vs 0x7f1504)
//   - No-input flag: _DAT_0067f1bc (vs _DAT_0067f1b8)
// All phases and path A/B logic identical to MenuButtonDetectC.
// ref: re/analysis/frontend_promote_menus_a/0x0042b540.md
// ---------------------------------------------------------------------------

// Additional constants for col+1 (all others reuse the constants declared above)
static constexpr std::uintptr_t kActiveBase1    = 0x007f1045; // active byte col +1 base
static constexpr std::uintptr_t kProcessedBase1 = 0x007f1505; // processed byte col +1 base
// Slot 7 sentinel for col+1: 0x7f1045 + 7*0x4c = 0x7f1259
static constexpr std::uintptr_t kPathARow7Active1    = 0x007f1259;
// Slot 7 sentinel processed col+1: 0x7f1505 + 7*0x4c = 0x7f17c5
static constexpr std::uintptr_t kPathARow7Proc1      = 0x007f17c5;
// No-input flag for col+1 (vs kNoInputFlag = 0x0067f1b8 for col+0)
static constexpr std::uintptr_t kNoInputFlag1   = 0x0067f1bc;

// 0x0042b540
extern "C" __declspec(dllexport) std::uint32_t __cdecl MenuButtonDetectD() {
    const std::int32_t sel = *reinterpret_cast<std::int32_t*>(kDispatchSel);

    // Phase 1 -- detect active input at col +1
    bool bVar1 = false;
    if (sel == 0x1000) {
        // Path A: fixed stride (7 entries)
        for (int n = 0; n < 7; n++) {
            if (*reinterpret_cast<const char*>(kActiveBase1 + n * kStride) != '\0') {
                bVar1 = true;
            }
        }
        // Slot 7 sentinel: 0x7f1045 + 7*0x4c = 0x7f1259
        if (*reinterpret_cast<const char*>(kPathARow7Active1) != '\0') {
            bVar1 = true;
        }
    } else {
        // Path B: car-slot index array
        const std::int32_t* piVar4 = reinterpret_cast<std::int32_t*>(kPlayerSlotArray);
        for (int iVar5 = 0; iVar5 < 8; iVar5++, piVar4++) {
            if (reinterpret_cast<std::uintptr_t>(piVar4) >= kPlayerSlotEnd) break;
            const std::int32_t slotIdx = *piVar4;
            if (slotIdx == -1) continue;
            if (CallSlotState(iVar5) == 2) continue; // U-3445
            if (*reinterpret_cast<const char*>(kActiveBase1 + slotIdx * kStride) != '\0') {
                bVar1 = true;
            }
        }
    }

    if (!bVar1) {
        // Phase 2: no active input -- zero the col+1 flag
        *reinterpret_cast<std::int32_t*>(kNoInputFlag1) = 0;
        // fall through to phase 3 (uVar3 stays 0)
    } else {
        // Screen-type branch (identical to MenuButtonDetectC)
        const std::int32_t slot   = *reinterpret_cast<std::int32_t*>(kSlotIndex);
        const std::int32_t screen = *reinterpret_cast<std::int32_t*>(kScreenTypeBase + slot * 0x40);
        const std::int32_t cursor = *reinterpret_cast<std::int32_t*>(kCursorBase + slot * 0x40);

        if (screen == kScreenId13) {
            if (cursor == 2) return 1;
            // cursor 0/1: fall through to phase 3
        } else if (screen == kScreenId1e) {
            return 1;
        }
        // else: fall through to phase 3
    }

    // Phase 3 -- detect active AND !processed at col +1
    std::uint32_t uVar3 = 0;
    if (sel == 0x1000) {
        // Path A: 8 entries (note: uses 8 like MenuButtonDetectC phase 3)
        for (int n = 0; n < 8; n++) {
            if (*reinterpret_cast<const char*>(kActiveBase1 + n * kStride) != '\0' &&
                *reinterpret_cast<const char*>(kProcessedBase1 + n * kStride) == '\0') {
                uVar3 = 1;
            }
        }
    } else {
        // Path B
        const std::int32_t* piVar4 = reinterpret_cast<std::int32_t*>(kPlayerSlotArray);
        for (int iVar5 = 0; iVar5 < 8; iVar5++, piVar4++) {
            if (reinterpret_cast<std::uintptr_t>(piVar4) >= kPlayerSlotEnd) break;
            const std::int32_t slotIdx = *piVar4;
            if (slotIdx == -1) continue;
            if (CallSlotState(iVar5) == 2) continue;
            if (*reinterpret_cast<const char*>(kActiveBase1 + slotIdx * kStride) != '\0' &&
                *reinterpret_cast<const char*>(kProcessedBase1 + slotIdx * kStride) == '\0') {
                uVar3 = 1;
            }
        }
    }

    return uVar3;
}

RH_ScopedInstall(MenuButtonDetectD, 0x0042b540);  // re-enabled 2026-05-24 c3-frontend-a

// ---------------------------------------------------------------------------
// 0x0042b8b0  ScreenWidthGet
//
// Returns DAT_0067ea54 (uint16_t) — current screen pixel width.
// 6-byte leaf getter. No callees.
// Analysis: re/analysis/promote_c2_hud_ingame/0x0042b8b0.md
// ---------------------------------------------------------------------------

// Screen dimension globals (uint16_t each)
static constexpr std::uintptr_t kScreenWidth  = 0x0067ea54; // uint16_t pixel width
static constexpr std::uintptr_t kScreenHeight = 0x0067ea56; // uint16_t pixel height

// 0x0042b8b0
extern "C" __declspec(dllexport) std::uint32_t __cdecl ScreenWidthGet() {
    return static_cast<std::uint32_t>(
        *reinterpret_cast<std::uint16_t*>(kScreenWidth));
}

RH_ScopedInstall(ScreenWidthGet, 0x0042b8b0);  // re-enabled 2026-05-24 c3-safe

// ---------------------------------------------------------------------------
// 0x0042b8c0  ScreenHeightGet
//
// Returns DAT_0067ea56 (uint16_t) — current screen pixel height.
// 6-byte leaf getter. No callees.
// Analysis: re/analysis/promote_c2_hud_ingame/0x0042b8c0.md
// ---------------------------------------------------------------------------

// 0x0042b8c0
extern "C" __declspec(dllexport) std::uint32_t __cdecl ScreenHeightGet() {
    return static_cast<std::uint32_t>(
        *reinterpret_cast<std::uint16_t*>(kScreenHeight));
}

RH_ScopedInstall(ScreenHeightGet, 0x0042b8c0);  // re-enabled 2026-05-24 c3-safe
