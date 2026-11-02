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

// Player-slot record walk shared by 0x0042b310 / 0x0042b540 / 0x0042b770:
// id dword at 0x007f1a14 + i*0x10, i = 0..3 (0x0042b3bf / 0x0042b877 `add esi, 0x10`).
static constexpr std::uintptr_t kSlotRecStride = 0x10;

static inline const std::int32_t* SlotRec(int i) {
    return reinterpret_cast<const std::int32_t*>(kPlayerSlotArray + i * kSlotRecStride);
}

static inline char PadAt(std::uintptr_t base, std::int32_t pad) {
    return *reinterpret_cast<const char*>(base + static_cast<std::uintptr_t>(pad * kStride));
}

// ---------------------------------------------------------------------------
// 0x0042b770  MenuButtonDetectE
//
// Edge detect on held column 0x007f1049 / previous column 0x007f1509.
// Re-ported from the bytes 2026-10-12 (negctl re-earn). The earlier port
// walked the slot records at 4-byte stride over 8 iterations. The original
// steps `add esi, 0x10` (0x0042b877) over 4 records up to 0x007f1a54
// (0x0042b87b), with index EDI 0..3.
//
// 0x1000 path (0x0042b783..0x0042b836): pads 0..6 OR held && !previous into
//   EBX. Pad 7 (0x007f125d / 0x007f171d) returns 1 directly (0x0042b836),
//   otherwise return EBX.
// Otherwise (0x0042b83d..0x0042b888): record i is eligible when its id != -1
//   (0x0042b846) and FUN_0040e470(i) != 2 (0x0042b84c). Then held && !previous
//   on pad id (0x0042b85e / 0x0042b868) sets EBX. No early exit. Return EBX.
// No timer, no screen-type check.
// ---------------------------------------------------------------------------

// MATCH-S07: the body of 0x0042b770 (MenuButtonDetectE) moved to Match/MatchS07_0042b770.cpp,
// which compiles the byte-matched source re/match/s07/0042b770.c.

// 0x0042b310 MenuButtonDetectC and 0x0042b540 MenuButtonDetectD shared the static helper
// MenuButtonDetectScreenGate here. MATCH-S07 moved both bodies to Match/MatchS07_0042b310.cpp and
// Match/MatchS07_0042b540.cpp (byte-matched sources re/match/s07/0042b310.c / 0042b540.c), so the helper went too.

// MATCH-S07: the body of 0x0042b310 (MenuButtonDetectC) moved to Match/MatchS07_0042b310.cpp,
// which compiles the byte-matched source re/match/s07/0042b310.c.

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

// MATCH-S07: the body of 0x0042b960 (CarSlotInit1P) moved to Match/MatchS07_0042b960.cpp,
// which compiles the byte-matched source re/match/s07/0042b960.c.

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

// MATCH-S07: the body of 0x0042b9e0 (CarSlotAssign) moved to Match/MatchS07_0042b9e0.cpp,
// which compiles the byte-matched source re/match/s07/0042b9e0.c.

// MATCH-S07: the body of 0x0042b540 (MenuButtonDetectD) moved to Match/MatchS07_0042b540.cpp,
// which compiles the byte-matched source re/match/s07/0042b540.c.

// ---------------------------------------------------------------------------
// 0x0042b8b0  ScreenWidthGet
//
// Returns DAT_0067ea54 (uint16_t) — current screen pixel width.
// 6-byte leaf getter. No callees.
// Analysis: re/analysis/promote_c2_hud_ingame/0x0042b8b0.md
// ---------------------------------------------------------------------------

// MATCH-S07: the body of 0x0042b8b0 (ScreenWidthGet) moved to Match/MatchS07_0042b8b0.cpp,
// which compiles the byte-matched source re/match/s07/0042b8b0.c.

// ---------------------------------------------------------------------------
// 0x0042b8c0  ScreenHeightGet
//
// Returns DAT_0067ea56 (uint16_t) — current screen pixel height.
// 6-byte leaf getter. No callees.
// Analysis: re/analysis/promote_c2_hud_ingame/0x0042b8c0.md
// ---------------------------------------------------------------------------

// MATCH-S07: the body of 0x0042b8c0 (ScreenHeightGet) moved to Match/MatchS07_0042b8c0.cpp,
// which compiles the byte-matched source re/match/s07/0042b8c0.c.
