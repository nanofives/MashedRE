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

// Car-slot records: id dword at 0x007f1a14 + i*0x10, i = 0..3 (0x0042b3bf `add esi, 0x10`)
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
#ifdef MASHED_STANDALONE
    // 0x0040e480 is MASHED .text. The standalone maps no such code (only the
    // 0x00420000 and 0x00470000 .text granules are claimed, by
    // Compat/StandaloneRvaThunks), so calling through this RVA AVs the process
    // -- observed as rc 0xC0000005 the first time mashed_re.exe called
    // CarSlotAssign, 2026-09-04.
    //
    // No-op rather than an inline write: the callee sets the per-slot CAR-ALIVE
    // entry, whose only consumers are race-side functions that are themselves
    // unported (FUN_0040eee0 and the elimination counters read it). The
    // standalone tracks car liveness in TrackRenderer instead, so nothing in
    // the exe reads what this would have written. What the exe DOES need from
    // CarSlotAssign -- the slot table at 0x007f1a14 (profile index + Player
    // Colour) and the 0x1000 verdict -- is unaffected.
    //
    // Deliberately NOT an inline write to `*(PTR_PTR_005f2770) + idx*4 + 0x34`:
    // 0x005f2770 is not one of the committed granules, so dereferencing it here
    // would trade one access violation for another.
    (void)idx; (void)val;
#else
    reinterpret_cast<FunSlotWriteFn>(kFun0040e480)(idx, val);
#endif
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
// 0x0042b540  MenuButtonDetectD
// The two bodies are the same instruction stream with three operands changed:
// held column (0x007f1044 / 0x007f1045), previous-frame column (0x007f1504 /
// 0x007f1505) and the cleared dword (0x0067f1b8 / 0x0067f1bc). Re-ported from
// the bytes 2026-10-12 (negctl re-earn). The earlier port had two defects:
//   1. screen type 0x13 returned 1 only for sub 2; the original returns 1 for
//      sub 0, 1 and 2 (0x0042b3f9 je / 0x0042b3fe je -> 0x0042b3e7, and
//      0x0042b400 cmp 2 -> 0x0042b405);
//   2. both player-slot walks stepped 4 bytes over 8 records instead of
//      0x10 bytes over 4 records (0x0042b3bf / 0x0042b521 `add esi, 0x10`).
//
// Phase 1 (0x0042b310..0x0042b3c9): held = any eligible pad's held byte != 0.
//   0x1000 path: pads 0..6 OR into EBX, pad 7 jumps straight to phase 2.
//   Otherwise: slot record i (0..3) is eligible when its id != -1 and
//   FUN_0040e470(i) != 2; the pad is the id (0x0042b3ad imul 0x4c).
// Phase 2 (0x0042b3cb..0x0042b40f): not held -> dword = 0 (0x0042b40f), go to
//   phase 3. Held -> t = *(0x0067ed3c + DAT_0067e9f8*0x40):
//   t == 0x13: sub = *(0x0067ed40 + DAT_0067e9f8*0x40); sub 0/1/2 -> return 1,
//              any other sub -> phase 3;
//   t == 0x1e: return 1;  any other t -> phase 3.
// Phase 3 (0x0042b419..0x0042b533): return 1 if any eligible pad has
//   held != 0 && previous == 0, else 0. The slot walk does not stop early.
// ---------------------------------------------------------------------------

static constexpr std::uintptr_t kSlotRecStride = 0x10;  // 0x0042b3bf `add esi, 0x10`

static inline const std::int32_t* SlotRec(int i) {
    return reinterpret_cast<const std::int32_t*>(kPlayerSlotArray + i * kSlotRecStride);
}

static inline char PadAt(std::uintptr_t base, std::int32_t pad) {
    return *reinterpret_cast<const char*>(base + static_cast<std::uintptr_t>(pad * kStride));
}

static std::uint32_t MenuButtonDetectScreenGate(std::uintptr_t heldBase, std::uintptr_t prevBase,
                                                std::uintptr_t clearAddr) {
    bool held = false;  // EBX

    if (*reinterpret_cast<const std::int32_t*>(kDispatchSel) == 0x1000) {
        for (int k = 0; k < 7; ++k) {                       // 0x0042b324..0x0042b381
            if (PadAt(heldBase, k) != '\0') held = true;
        }
        if (PadAt(heldBase, 7) != '\0') held = true;        // 0x0042b386 jne 0x0042b3cf
    } else {
        for (int i = 0; reinterpret_cast<std::uintptr_t>(SlotRec(i)) < kPlayerSlotEnd; ++i) {
            const std::int32_t id = *SlotRec(i);            // 0x0042b398
            if (id == -1) continue;
            if (CallSlotState(i) == 2) continue;            // 0x0042b39e
            if (PadAt(heldBase, id) != '\0') held = true;   // 0x0042b3b0
        }
    }

    if (!held) {
        *reinterpret_cast<std::uint32_t*>(clearAddr) = 0u; // 0x0042b40f
    } else {
        const std::uintptr_t off = static_cast<std::uintptr_t>(
            *reinterpret_cast<const std::int32_t*>(kSlotIndex)) << 6;   // 0x0042b3d4 `shl eax, 6`
        const std::int32_t t = *reinterpret_cast<const std::int32_t*>(kScreenTypeBase + off);
        if (t == kScreenId13) {
            const std::int32_t sub = *reinterpret_cast<const std::int32_t*>(kCursorBase + off);  // 0x0042b3f1
            if (sub == 0 || sub == 1 || sub == 2) return 1u;            // 0x0042b3f9 / 0x0042b3fe / 0x0042b400
        } else if (t == kScreenId1e) {
            return 1u;                                                  // 0x0042b3e2
        }
    }

    std::uint32_t edge = 0;  // EBP
    if (*reinterpret_cast<const std::int32_t*>(kDispatchSel) == 0x1000) {
        for (int k = 0; k < 8; ++k) {                       // 0x0042b429..0x0042b4da
            if (PadAt(heldBase, k) != '\0' && PadAt(prevBase, k) == '\0') edge = 1;
        }
        return edge;
    }
    for (int i = 0; reinterpret_cast<std::uintptr_t>(SlotRec(i)) < kPlayerSlotEnd; ++i) {
        const std::int32_t id = *SlotRec(i);                // 0x0042b4f0
        if (id == -1) continue;
        if (CallSlotState(i) == 2) continue;                // 0x0042b4f6
        if (PadAt(heldBase, id) != '\0' && PadAt(prevBase, id) == '\0') edge = 1;  // 0x0042b508 / 0x0042b512
    }
    return edge;                                            // 0x0042b52f
}

// 0x0042b310
extern "C" __declspec(dllexport) std::uint32_t __cdecl MenuButtonDetectC() {
    return MenuButtonDetectScreenGate(kActiveBase0, kProcessedBase0, kNoInputFlag);
}

RH_ScopedInstall(MenuButtonDetectC, 0x0042b310);  // re-ported 2026-10-12 (negctl re-earn)

// ---------------------------------------------------------------------------
// 0x0042b960  CarSlotInit1P
//
// Re-ported from the bytes 2026-10-12 (negctl re-earn). ECX counts pads
// 0..7 over the pair 0x007f1042 / 0x007f1502 (stride 0x4c, bound 0x007f1762
// at 0x0042b983) and stops at the first pad with held != 0 && previous == 0.
// When none matches the loop runs out with ECX = 8. Either way ECX is stored
// once, after the loop (0x0042b991). The earlier port stored 0 on a miss.
// Then 0x007f1a24/34/44 = -1 (0x0042b98a `or eax, -1`), 0x007f1a0c = 1
// (0x0042b9a6), and FUN_0040e480 (0,1), (1,0), (2,0), (3,0) (0x0042b9b0..cb).
// ---------------------------------------------------------------------------

static constexpr std::uintptr_t kTableA_base = 0x007f1042; // 0x0042b970 `[eax - 0x4c0]`
static constexpr std::uintptr_t kTableB_base = 0x007f1502; // 0x0042b962
static constexpr std::uintptr_t kScanBound   = 0x7f1762;   // 0x0042b983 (exclusive)

static constexpr std::uintptr_t kSlot1       = 0x007f1a14;
static constexpr std::uintptr_t kSlot2       = 0x007f1a24;
static constexpr std::uintptr_t kSlot3       = 0x007f1a34;
static constexpr std::uintptr_t kSlot4       = 0x007f1a44;
static constexpr std::uintptr_t kModeSel     = 0x007f1a0c;

// 0x0042b960
extern "C" __declspec(dllexport) void __cdecl CarSlotInit1P() {
    std::int32_t idx = 0;                                   // 0x0042b960 `xor ecx, ecx`
    for (std::uintptr_t prev = kTableB_base; prev < kScanBound; prev += kStride, ++idx) {
        if (*reinterpret_cast<const char*>(prev - (kTableB_base - kTableA_base)) != '\0' &&
            *reinterpret_cast<const char*>(prev) == '\0') {
            break;                                          // 0x0042b97d je 0x0042b98a
        }
    }

    *reinterpret_cast<std::int32_t*>(kSlot1)  = idx;        // 0x0042b991
    *reinterpret_cast<std::uint32_t*>(kSlot2) = 0xffffffffu;// 0x0042b997
    *reinterpret_cast<std::uint32_t*>(kSlot3) = 0xffffffffu;// 0x0042b99c
    *reinterpret_cast<std::uint32_t*>(kSlot4) = 0xffffffffu;// 0x0042b9a1
    *reinterpret_cast<std::int32_t*>(kModeSel) = 1;         // 0x0042b9a6

    CallSlotWrite(0, 1);                                    // 0x0042b9b0
    CallSlotWrite(1, 0);                                    // 0x0042b9b9
    CallSlotWrite(2, 0);                                    // 0x0042b9c2
    CallSlotWrite(3, 0);                                    // 0x0042b9cb
}

RH_ScopedInstall(CarSlotInit1P, 0x0042b960);  // re-ported 2026-10-12 (negctl re-earn)

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
// Same instruction stream as 0x0042b310 (see MenuButtonDetectScreenGate) with
// held 0x007f1045 (0x0042b554), previous 0x007f1505 (0x0042b662) and cleared
// dword 0x0067f1bc (0x0042b63f). Re-ported 2026-10-12 (negctl re-earn): the
// earlier port fell through to the edge detect for type 0x13 sub 0/1
// (0x0042b629 / 0x0042b62e je 0x0042b617 return 1) and walked the slot
// records at 4-byte stride (0x0042b5ef / 0x0042b751 `add esi, 0x10`).
// ---------------------------------------------------------------------------

static constexpr std::uintptr_t kActiveBase1    = 0x007f1045; // 0x0042b554
static constexpr std::uintptr_t kProcessedBase1 = 0x007f1505; // 0x0042b662
static constexpr std::uintptr_t kNoInputFlag1   = 0x0067f1bc; // 0x0042b63f

// 0x0042b540
extern "C" __declspec(dllexport) std::uint32_t __cdecl MenuButtonDetectD() {
    return MenuButtonDetectScreenGate(kActiveBase1, kProcessedBase1, kNoInputFlag1);
}

RH_ScopedInstall(MenuButtonDetectD, 0x0042b540);  // re-ported 2026-10-12 (negctl re-earn)

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
