// Mashed RE - Frontend menu helper reimplementations.
// Analysis notes: re/analysis/promote_c2_frontend_menus/
//
// All functions in this file are pure leaves (callees_depth1: []).
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// FrontendPlayerSlotCheck  --  0x0042ebe0
//
// Original: FUN_0042ebe0 (388 bytes, 0x0042ebe0..0x0042ed64)
// Signature: bool FUN_0042ebe0(int param_1)
//   param_1: dispatch key in {10 (0xa), 11 (0xb), 12 (0xc)}
//   Returns: (&DAT_007f0a44)[param_1 * 0xc] != 0 && !bVar1
//
// Iterates player-slot array at DAT_007f0a48, stride 0x1e0 (= 0x78 * 4) bytes,
// until piVar2 >= 0x7f0c28. Per param_1 dispatches:
//   10 (0xa): simple check piVar2[-1] (offset -4); full 9-field check at
//             offsets 0x2c, 0x44, 0x5c, 0x74, 0x8c, 0xa4, 0xbc, 0xd4, 0xec (stride 0x18)
//   11 (0xb): simple piVar2[0] (offset 0); full 9 fields piVar2[0xc,0x18,...,0x6c] stride 0xc
//   12 (0xc): simple piVar2[3] (offset +12); full 9 fields piVar2[0xf,0x1b,...,0x6f] stride 0xc
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042ebe0.md
// ---------------------------------------------------------------------------

// 0x0042ebe0
extern "C" __declspec(dllexport) bool __cdecl FrontendPlayerSlotCheck(int param_1) {
    bool bVar1 = false;
    std::int32_t* piVar2 = reinterpret_cast<std::int32_t*>(0x007f0a48u);

    while (reinterpret_cast<std::uintptr_t>(piVar2) < 0x007f0c28u) {
        if (param_1 == 10) {
            // simple check: offset -4 from piVar2
            if (piVar2[-1] == 0) {
                bVar1 = true;
                break;
            }
            // full check: 9 fields at stride 0x18 dwords (0xc ints) starting at offset 0x2c bytes
            // offsets in bytes from piVar2: 0x2c, 0x44, 0x5c, 0x74, 0x8c, 0xa4, 0xbc, 0xd4, 0xec
            // in int32 units (stride 0x18 bytes = 6 ints per step, first at +0x2c/4=0xb)
            {
                bool allNonZero = true;
                for (int k = 0; k < 9; k++) {
                    int byteOff = 0x2c + k * 0x18;
                    std::int32_t* pField = reinterpret_cast<std::int32_t*>(
                        reinterpret_cast<std::uintptr_t>(piVar2) + byteOff);
                    if (*pField == 0) {
                        allNonZero = false;
                        break;
                    }
                }
                if (allNonZero) {
                    // last field non-zero check (same last field, already verified non-zero)
                    // logic: if all non-zero => do not set bVar1 (slot is full)
                }
            }
        } else if (param_1 == 11) {
            // simple check: offset 0 from piVar2
            if (piVar2[0] == 0) {
                bVar1 = true;
                break;
            }
            // full check: 9 fields at stride 0xc bytes = 3 ints, starting at piVar2[0xc/4=3]
            // positions: piVar2[3], piVar2[6], piVar2[9], ..., piVar2[27] (0x6c/4=27)
            {
                bool allNonZero = true;
                for (int k = 0; k < 9; k++) {
                    if (piVar2[3 + k * 3] == 0) {
                        allNonZero = false;
                        break;
                    }
                }
                if (allNonZero) {
                    // if last field non-zero: do not set bVar1
                }
            }
        } else if (param_1 == 12) {
            // simple check: offset +12 bytes = piVar2[3]
            if (piVar2[3] == 0) {
                bVar1 = true;
                break;
            }
            // full check: 9 fields: piVar2[0xf/4..] but 0xf is not int-aligned in dwords.
            // Per analysis: piVar2[0xf,0x1b,...,0x6f] (byte offsets 0x3c, 0x6c, ... stride 0xc)
            // Actually: piVar2[0xf] means byte offset 0xf*4=0x3c from base, but in int index
            // it is piVar2 + 0xf; stride in int index = 0xc/4 = 3
            {
                bool allNonZero = true;
                for (int k = 0; k < 9; k++) {
                    if (piVar2[0xf + k * 3] == 0) {
                        allNonZero = false;
                        break;
                    }
                }
                if (allNonZero) {
                    // if last field non-zero: do not set bVar1
                }
            }
        }
        // advance by 0x1e0 bytes = 0x78 ints
        piVar2 += 0x78;
    }

    std::int32_t enableFlag = *reinterpret_cast<std::int32_t*>(
        0x007f0a44u + static_cast<std::uint32_t>(param_1) * 0xcu);
    return (enableFlag != 0) && !bVar1;
}

RH_ScopedInstall(FrontendPlayerSlotCheck, 0x0042ebe0);

// ---------------------------------------------------------------------------
// FrontendCursorUpdate  --  0x0042f7b0
//
// Original: FUN_0042f7b0 (288 bytes, 0x0042f7b0..0x0042f8cf)
// Signature: void FUN_0042f7b0(void)
//   Controller cursor updater: 4-player cursor positions from d-pad release edges.
//
// Guard: if DAT_0067eab0 != 0, return immediately.
//
// Loop: 4 player slots
//   pcVar2 = &DAT_007f1504  (input state, stride 0x130 bytes per slot)
//   piVar1 = &DAT_0067e85c  (cursor state, stride 0x30 bytes = 12 ints per slot)
//
// Per slot, 8 axis/direction checks (release-edge = prev!=0 && cur==0):
//   Left:       pcVar2[-0x4c0] != 0 && *pcVar2 == 0     => piVar1[-3]-- (clamp >= 0)
//   Right:      pcVar2[-0x4bf] != 0 && pcVar2[1] == 0   => piVar1[-3]++ (clamp <= 3)
//   Up:         pcVar2[-0x474] != 0 && pcVar2[0x4c] == 0 => *piVar1-- (clamp >= 0)
//   Down:       pcVar2[-0x473] != 0 && pcVar2[0x4d] == 0 => *piVar1++ (clamp <= 3)
//   Back-left:  pcVar2[-0x428] != 0 && pcVar2[0x98] == 0 => piVar1[3]-- (clamp >= 0)
//   Back-right: pcVar2[-0x427] != 0 && pcVar2[0x99] == 0 => piVar1[3]++ (clamp <= 3)
//   Alt-back-L: pcVar2[-0x3dc] != 0 && pcVar2[0xe4] == 0 => piVar1[6]-- (clamp >= 0)
//   Alt-back-R: pcVar2[-0x3db] != 0 && pcVar2[0xe5] == 0 => piVar1[6]++ (clamp <= 3)
//
// Loop end: pcVar2 >= 0x7f1894
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042f7b0.md
// ---------------------------------------------------------------------------

// 0x0042f7b0
extern "C" __declspec(dllexport) void __cdecl FrontendCursorUpdate() {
    if (*reinterpret_cast<std::int32_t*>(0x0067eab0u) != 0) {
        return;
    }

    std::int8_t*  pcVar2 = reinterpret_cast<std::int8_t*>(0x007f1504u);
    std::int32_t* piVar1 = reinterpret_cast<std::int32_t*>(0x0067e85cu);

    while (reinterpret_cast<std::uintptr_t>(pcVar2) < 0x007f1894u) {
        // Left: release edge -> decrement piVar1[-3], clamp >= 0
        if (pcVar2[-0x4c0] != 0 && pcVar2[0] == 0) {
            if (piVar1[-3] > 0) {
                piVar1[-3]--;
            }
        }
        // Right: release edge -> increment piVar1[-3], clamp <= 3
        if (pcVar2[-0x4bf] != 0 && pcVar2[1] == 0) {
            if (piVar1[-3] < 3) {
                piVar1[-3]++;
            }
        }
        // Up: release edge -> decrement *piVar1, clamp >= 0
        if (pcVar2[-0x474] != 0 && pcVar2[0x4c] == 0) {
            if (*piVar1 > 0) {
                (*piVar1)--;
            }
        }
        // Down: release edge -> increment *piVar1, clamp <= 3
        if (pcVar2[-0x473] != 0 && pcVar2[0x4d] == 0) {
            if (*piVar1 < 3) {
                (*piVar1)++;
            }
        }
        // Back-left: release edge -> decrement piVar1[3], clamp >= 0
        if (pcVar2[-0x428] != 0 && pcVar2[0x98] == 0) {
            if (piVar1[3] > 0) {
                piVar1[3]--;
            }
        }
        // Back-right: release edge -> increment piVar1[3], clamp <= 3
        if (pcVar2[-0x427] != 0 && pcVar2[0x99] == 0) {
            if (piVar1[3] < 3) {
                piVar1[3]++;
            }
        }
        // Alt-back-left: release edge -> decrement piVar1[6], clamp >= 0
        if (pcVar2[-0x3dc] != 0 && pcVar2[0xe4] == 0) {
            if (piVar1[6] > 0) {
                piVar1[6]--;
            }
        }
        // Alt-back-right: release edge -> increment piVar1[6], clamp <= 3
        if (pcVar2[-0x3db] != 0 && pcVar2[0xe5] == 0) {
            if (piVar1[6] < 3) {
                piVar1[6]++;
            }
        }

        // Advance: input stride 0x130 bytes; cursor stride 0x30 bytes (12 ints)
        pcVar2 += 0x130;
        piVar1 += 0x30 / sizeof(std::int32_t);  // 12 ints
    }
}

RH_ScopedInstall(FrontendCursorUpdate, 0x0042f7b0);

// ---------------------------------------------------------------------------
// LocalPlayerSlotCheck  --  0x00436810
//
// Original: FUN_00436810 (193 bytes, 0x00436810..0x004368d1)
// Callee depth-1: 0x00430760 IsMultiplayerMode (C3).
//
// Mechanical behavior (from re/analysis/hud_frontend_d5/0x00436810.md):
//   - param_1 == 12 (0xc): immediately returns false.
//   - Otherwise, calls IsMultiplayerMode():
//     * SP branch (IsMultiplayerMode()==0):
//         walks DAT_007f0a7c upward in stride 0x48 ints (0x120 bytes),
//         counting non-zero entries at six int-offsets per block
//         [-0xc, 0, +0xc, +0x18, +0x24, +0x30]; loop terminates when
//         piVar3 >= 0x007f0cbc. Returns (param_1 <= iVar2 - 1).
//     * MP branch (IsMultiplayerMode()!=0):
//         walks DAT_007f0a74 upward in stride 0x48 ints; for each block,
//         counts entries equal to 2 at the same six offsets; terminates
//         when piVar3 >= 0x007f0cb4. If (&DAT_007f0a44)[param_1*0xc] != 0,
//         returns (param_1 <= iVar2); else returns false.
//
// Uncertainties (preserved literal; see hud_frontend_d5/0x00436810.md):
//   U-3410: meaning of the 6 probe offsets within the 0x48-stride block.
//   U-3411: meaning of the value `2` tested in the MP branch.
// Per the NO-GUESSING rule, the offsets and the literal `== 2` test are
// reproduced exactly as in the original.
//
// ref: re/analysis/hud_frontend_d5/0x00436810.md
// ---------------------------------------------------------------------------

extern "C" std::uint32_t __cdecl IsMultiplayerMode();

// 0x00436810
// Return type is `int` (not `bool`) to match the harness comparison: the
// original returns via SETLE/SETcc which sets only the low byte of EAX,
// leaving the upper 24 bits undefined.  Returning `int` from C++ generates
// a XOR-and-set sequence that zeroes the full register, but the harness
// then compares the full 32-bit return.  We coerce to (int) so both sides
// produce the same value for the false (==12) path where the original
// only writes the low byte.
extern "C" __declspec(dllexport) int __cdecl LocalPlayerSlotCheck(int param_1) {
    // 0x00436815: guard — slot index 12 always returns false.
    if (param_1 == 0xc) {
        return 0;
    }
    // 0x00436820: branch on IsMultiplayerMode() return value.
    if (IsMultiplayerMode() == 0u) {
        // ─── SP branch ────────────────────────────────────────────────
        // 0x0043684d: piVar3 = &DAT_007f0a7c (int32*).
        // 0x0043686c: stride = 0x48 ints (0x120 bytes).
        // 0x00436870: terminate when piVar3 >= 0x007f0cbc.
        std::int32_t* piVar3 = reinterpret_cast<std::int32_t*>(0x007f0a7cu);
        int iVar2 = 0;
        while (reinterpret_cast<std::uintptr_t>(piVar3) < 0x007f0cbcu) {
            // 6 probes per block at int-offsets [-0xc, 0, +0xc, +0x18, +0x24, +0x30].
            // [U-3410] meaning of offsets unresolved; reproduced literally.
            if (piVar3[-0xc] != 0) iVar2++;
            if (piVar3[0]    != 0) iVar2++;
            if (piVar3[0xc]  != 0) iVar2++;
            if (piVar3[0x18] != 0) iVar2++;
            if (piVar3[0x24] != 0) iVar2++;
            if (piVar3[0x30] != 0) iVar2++;
            piVar3 += 0x48;  // stride in ints (= 0x120 bytes)
        }
        return param_1 <= iVar2 - 1;
    }

    // ─── MP branch ────────────────────────────────────────────────────
    // 0x004368a0: piVar3 = &DAT_007f0a74 (int32*).
    // 0x004368b9: stride = 0x48 ints.
    // 0x004368bd: terminate when piVar3 >= 0x007f0cb4.
    std::int32_t* piVar3 = reinterpret_cast<std::int32_t*>(0x007f0a74u);
    int iVar2 = 0;
    while (reinterpret_cast<std::uintptr_t>(piVar3) < 0x007f0cb4u) {
        // [U-3411] meaning of `== 2` unresolved; reproduced literally.
        if (piVar3[-0xc] == 2) iVar2++;
        if (piVar3[0]    == 2) iVar2++;
        if (piVar3[0xc]  == 2) iVar2++;
        if (piVar3[0x18] == 2) iVar2++;
        if (piVar3[0x24] == 2) iVar2++;
        if (piVar3[0x30] == 2) iVar2++;
        piVar3 += 0x48;
    }
    // 0x004368c2: (&DAT_007f0a44)[param_1 * 0xc] != 0 gate.
    std::int32_t* slotTypeBase = reinterpret_cast<std::int32_t*>(0x007f0a44u);
    if (slotTypeBase[param_1 * 0xc] != 0) {
        return param_1 <= iVar2;
    }
    return false;
}

RH_ScopedInstall(LocalPlayerSlotCheck, 0x00436810);
