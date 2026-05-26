// Mashed RE — Frontend global-getter leaf reimplementations (c3-batch-s session 4).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file:
//   0x00425ee0  SlotWordPtrGet       — indexed ptr: &DAT_00899294 + param_1*0x13
//   0x00425ef0  ActiveSlotCount      — counts active slots in 8-slot table
//   0x00426020  GlobalDat00646e58Get — returns &DAT_00646e58
//   0x00426080  GlobalDat00656ed8Get — returns DAT_00656ed8 (value)
//   0x00426090  GlobalDat0066ce58Get — returns &DAT_0066ce58
//
// Analysis notes:
//   re/analysis/frontend_c1_to_c2_s4/FUN_00425ee0.md
//   re/analysis/frontend_c1_to_c2_s4/FUN_00425ef0.md
//   re/analysis/frontend_c1_to_c2_s4/FUN_00426020.md
//   re/analysis/frontend_c1_to_c2_s4/FUN_00426080.md
//   re/analysis/frontend_c1_to_c2_s4/FUN_00426090.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// SlotWordPtrGet  --  0x00425ee0
//
// Original: FUN_00425ee0 (12 bytes, 0x00425ee0..0x00425eeb)
// Signature: undefined4 * FUN_00425ee0(int param_1)
//   param_1: slot index
// Returns: &DAT_00899294 + param_1 * 0x13
//
// The base DAT_00899294 (0x00899294) is offset +0x34 into the first slot of a
// 0x4c-byte-stride (0x13 int32-word-stride) table rooted at 0x00899260.
// Returns pointer to word[0xd] of slot param_1 in that table.
//
// Constants cited from 0x00425ee0 body:
//   0x00899294 — base address (DAT_00899294)                  [0x00425ee3]
//   0x13       — stride in int32 words (= 0x4c bytes)         [0x00425ee8]
//
// Callees: none (pure leaf)
// Callers: FUN_00464a50 @ 0x00464a50
//
// ref: re/analysis/frontend_c1_to_c2_s4/FUN_00425ee0.md
// ---------------------------------------------------------------------------

// 0x00425ee0
extern "C" __declspec(dllexport) std::uint32_t* __cdecl SlotWordPtrGet(int param_1) {
    // Returns &DAT_00899294 + param_1 * 0x13.
    // 0x00899294 cited at 0x00425ee3; stride 0x13 (int32 words) cited at 0x00425ee8.
    return reinterpret_cast<std::uint32_t*>(0x00899294u) + static_cast<unsigned>(param_1) * 0x13u;
}

RH_ScopedInstall(SlotWordPtrGet, 0x00425ee0);  // c3-batch-s-s4

// ---------------------------------------------------------------------------
// ActiveSlotCount  --  0x00425ef0
//
// Original: FUN_00425ef0 (174 bytes, 0x00425ef0..0x00425f9e)
// Signature: int FUN_00425ef0(void)
// Returns: count of active slots (0..8) in the 8-slot table at 0x00899260
//
// Each slot occupies 0x4c bytes (stride). A slot is "active" when:
//   slot[N]+0x44 != 0  AND  slot[N]+0x00 != 0
//
// The 8 slot pairs are hardcoded (not a loop).
// Slot base: 0x00899260; stride: 0x4c = 76 bytes.
//
// Memory accesses (cited from 0x00425ef0 body):
//   Slot 0: 0x008992a4 (+0x44), 0x00899260 (+0x00)
//   Slot 1: 0x008992f0 (+0x44), 0x008992ac (+0x00)
//   Slot 2: 0x0089933c (+0x44), 0x008992f8 (+0x00)
//   Slot 3: 0x00899388 (+0x44), 0x00899344 (+0x00)
//   Slot 4: 0x008993d4 (+0x44), 0x00899390 (+0x00)
//   Slot 5: 0x00899420 (+0x44), 0x008993dc (+0x00)
//   Slot 6: 0x0089946c (+0x44), 0x00899428 (+0x00)
//   Slot 7: 0x008994b8 (+0x44), 0x00899474 (+0x00)
//
// Callees: none (pure leaf)
// Callers: FUN_00464a50 @ 0x00464a50
//
// ref: re/analysis/frontend_c1_to_c2_s4/FUN_00425ef0.md
// ---------------------------------------------------------------------------

// 0x00425ef0
extern "C" __declspec(dllexport) int __cdecl ActiveSlotCount() {
    int iVar1 = 0;

    // Slot 0: word_A at 0x008992a4 (+0x44), word_B at 0x00899260 (+0x00)
    if ((*reinterpret_cast<std::int32_t*>(0x008992a4u) != 0) &&
        (*reinterpret_cast<std::int32_t*>(0x00899260u) != 0)) { iVar1 = iVar1 + 1; }

    // Slot 1: word_A at 0x008992f0, word_B at 0x008992ac
    if ((*reinterpret_cast<std::int32_t*>(0x008992f0u) != 0) &&
        (*reinterpret_cast<std::int32_t*>(0x008992acu) != 0)) { iVar1 = iVar1 + 1; }

    // Slot 2: word_A at 0x0089933c, word_B at 0x008992f8
    if ((*reinterpret_cast<std::int32_t*>(0x0089933cu) != 0) &&
        (*reinterpret_cast<std::int32_t*>(0x008992f8u) != 0)) { iVar1 = iVar1 + 1; }

    // Slot 3: word_A at 0x00899388, word_B at 0x00899344
    if ((*reinterpret_cast<std::int32_t*>(0x00899388u) != 0) &&
        (*reinterpret_cast<std::int32_t*>(0x00899344u) != 0)) { iVar1 = iVar1 + 1; }

    // Slot 4: word_A at 0x008993d4, word_B at 0x00899390
    if ((*reinterpret_cast<std::int32_t*>(0x008993d4u) != 0) &&
        (*reinterpret_cast<std::int32_t*>(0x00899390u) != 0)) { iVar1 = iVar1 + 1; }

    // Slot 5: word_A at 0x00899420, word_B at 0x008993dc
    if ((*reinterpret_cast<std::int32_t*>(0x00899420u) != 0) &&
        (*reinterpret_cast<std::int32_t*>(0x008993dcu) != 0)) { iVar1 = iVar1 + 1; }

    // Slot 6: word_A at 0x0089946c, word_B at 0x00899428
    if ((*reinterpret_cast<std::int32_t*>(0x0089946cu) != 0) &&
        (*reinterpret_cast<std::int32_t*>(0x00899428u) != 0)) { iVar1 = iVar1 + 1; }

    // Slot 7: word_A at 0x008994b8, word_B at 0x00899474
    if ((*reinterpret_cast<std::int32_t*>(0x008994b8u) != 0) &&
        (*reinterpret_cast<std::int32_t*>(0x00899474u) != 0)) { iVar1 = iVar1 + 1; }

    return iVar1;
}

RH_ScopedInstall(ActiveSlotCount, 0x00425ef0);  // c3-batch-s-s4

// ---------------------------------------------------------------------------
// GlobalDat00646e58Get  --  0x00426020
//
// Original: FUN_00426020 (5 bytes, 0x00426020..0x00426024)
// Signature: undefined * FUN_00426020(void)
// Returns: &DAT_00646e58 — address of global at 0x00646e58
//
// Pure getter thunk: no computation, no side-effects.
// Callers: FUN_00406ce0, FUN_0040d270, FUN_00471ac0
//
// ref: re/analysis/frontend_c1_to_c2_s4/FUN_00426020.md
// ---------------------------------------------------------------------------

// 0x00426020
extern "C" __declspec(dllexport) void* __cdecl GlobalDat00646e58Get() {
    // Returns &DAT_00646e58 cited at 0x00426020 body.
    return reinterpret_cast<void*>(0x00646e58u);
}

RH_ScopedInstall(GlobalDat00646e58Get, 0x00426020);  // c3-batch-s-s4

// ---------------------------------------------------------------------------
// GlobalDat00656ed8Get  --  0x00426080
//
// Original: FUN_00426080 (5 bytes, 0x00426080..0x00426084)
// Signature: undefined4 FUN_00426080(void)
// Returns: DAT_00656ed8 — 4-byte value at global 0x00656ed8
//
// Pure value-return getter thunk. Single caller: FUN_0040da50.
//
// ref: re/analysis/frontend_c1_to_c2_s4/FUN_00426080.md
// ---------------------------------------------------------------------------

// 0x00426080
extern "C" __declspec(dllexport) std::uint32_t __cdecl GlobalDat00656ed8Get() {
    // Return value of DAT_00656ed8 cited at 0x00426080 body.
    return *reinterpret_cast<std::uint32_t*>(0x00656ed8u);
}

RH_ScopedInstall(GlobalDat00656ed8Get, 0x00426080);  // c3-batch-s-s4

// ---------------------------------------------------------------------------
// GlobalDat0066ce58Get  --  0x00426090
//
// Original: FUN_00426090 (5 bytes, 0x00426090..0x00426094)
// Signature: undefined * FUN_00426090(void)
// Returns: &DAT_0066ce58 — address of global at 0x0066ce58
//
// Pure pointer-return getter thunk. High fan-in: 8 callers share this global.
//
// ref: re/analysis/frontend_c1_to_c2_s4/FUN_00426090.md
// ---------------------------------------------------------------------------

// 0x00426090
extern "C" __declspec(dllexport) void* __cdecl GlobalDat0066ce58Get() {
    // Returns &DAT_0066ce58 cited at 0x00426090 body.
    return reinterpret_cast<void*>(0x0066ce58u);
}

RH_ScopedInstall(GlobalDat0066ce58Get, 0x00426090);  // c3-batch-s-s4
