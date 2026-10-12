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
// Signature: uint32 FUN_0042ebe0(int p)   (cdecl; eax = 0 or 1, full dword)
// Re-ported from the bytes 2026-10-12 (reearn/fstate).
//
// Disasm:
//   0x0042ebe4 xor ecx,ecx            ; ecx = "a zero cell was found" (bVar1)
//   0x0042ebe6 mov eax,0x7f0a48       ; loop base
//   loop body 0x0042ebf0..0x0042ed51, step 0x0042ed53 add eax,0x1e0,
//   test 0x0042ed58 cmp eax,0x7f0c28 ; jl (signed).  0x7f0a48 + 0x1e0 ==
//   0x7f0c28, so the body runs exactly once with eax = 0x7f0a48.
//   Body, by p (all unequal p fall through to the loop step):
//     p == 10: [eax-4]    0x0042ebf5, then [eax+0x2c],0x5c,0x8c,0xbc,0xec,0x11c,
//              0x14c,0x17c,0x1ac  0x0042ec18..0x0042ec7a  -> column 0x7f0a44 rows 0..9
//     p == 11: [eax]      0x0042ebff, then [eax+0x30],0x60,0x90,0xc0,0xf0,0x120,
//              0x150,0x180,0x1b0  0x0042ec8a..0x0042ecd4  -> column 0x7f0a48 rows 0..9
//     p == 12: [eax+0xc]  0x0042ec08, then [eax+0x3c],0x6c,0x9c,0xcc,0xfc,0x12c,
//              0x15c,0x18c,0x1bc  0x0042ece1..0x0042ed49  -> column 0x7f0a54 rows 0..9
//     (row stride 0x30). Any cell == 0 -> 0x0042ece8 mov ecx,1 and leave the loop.
//   Tail 0x0042eced: edx = [p*0x30 + 0x7f0a44] (lea+shl 4, 0x0042ecf3);
//     eax = 0 ; if edx != 0: al = (ecx == 0)  (0x0042ecf9..0x0042ed02).
// ---------------------------------------------------------------------------

// 0x0042ebe0
extern "C" __declspec(dllexport) std::uint32_t __cdecl FrontendPlayerSlotCheck(int param_1) {
    std::uint32_t found = 0;                                                       // 0x0042ebe4
    for (std::uint32_t base = 0x007f0a48u;                                         // 0x0042ebe6
         static_cast<std::int32_t>(base) < 0x007f0c28;                           // 0x0042ed58 jl
         base += 0x1e0u) {                                                         // 0x0042ed53
        std::uintptr_t col;
        if (param_1 == 10)      col = base - 4u;                                   // 0x0042ebf5
        else if (param_1 == 11) col = base;                                        // 0x0042ebff
        else if (param_1 == 12) col = base + 0xcu;                                 // 0x0042ec08
        else continue;                                                             // 0x0042ec06 -> 0x0042ed53
        for (std::uint32_t row = 0; row < 10u; ++row) {
            if (*reinterpret_cast<const std::uint32_t*>(col + row * 0x30u) == 0) {
                found = 1;                                                         // 0x0042ece8
                goto tail;
            }
        }
    }
tail:
    {
        const std::uint32_t flag = *reinterpret_cast<const std::uint32_t*>(
            static_cast<std::uint32_t>(0x007f0a44u + static_cast<std::uint32_t>(param_1) * 0x30u));   // 0x0042ecf3
        if (flag == 0) return 0u;                                                  // 0x0042ecfe
        return found == 0 ? 1u : 0u;                                               // 0x0042ed02 sete
    }
}

RH_ScopedInstall(FrontendPlayerSlotCheck, 0x0042ebe0);  // re-ported 2026-10-12 reearn/fstate

// ---------------------------------------------------------------------------
// FrontendCursorUpdate  --  0x0042f7b0
//
// Original: FUN_0042f7b0 (281 bytes, 0x0042f7b0..0x0042f8c8)
// Signature: void FUN_0042f7b0(void)
//
// IDENTIFIED 2026-09-04: this is ABILITY SELECT's (nav screen 15) per-frame
// input step -- the exact sibling of the Team Select step FUN_0042fa00.
// FUN_0043c000 drives both off the panel-state flags:
//   0x0043c17d  mov eax, dword ptr [0x67e7d8]   ; ability panel state
//   0x0043c182  cmp eax, esi                    ; esi = 1
//   0x0043c184  jne 0x43c1a9
//   0x0043c1a2  call 0x42f7b0                   ; <-- this function
// against the team arm at 0x0043c434..0x0043c44a `call 0x42fa00`, and
// FUN_00431f30 sets DAT_0067e7d8 = 1 for page id 0xf (screen 15) exactly as it
// sets DAT_0067e7e0 = 1 for 9 / 0x10 (screen 16).
//
// The table it edits is DAT_0067e850 (12 entries, stride 12 bytes) -- the same
// table FUN_0043a610 walks to place each row's car sprite at
// x = ability * 130 + 64, and the one the screen-15/16 entry sequence resets to
// 1 for all twelve entries. piVar1 starts at &DAT_0067e85c, so piVar1[-3] is
// entry 0.
//
// Guard: if DAT_0067eab0 != 0, return immediately.
//
// Loop: 12 profiles, FOUR UNROLLED PER ITERATION (3 iterations).
//   pcVar2 = &DAT_007f1504  (PROCESSED input, per-profile stride 0x4c;
//                            0x130 = 4 * 0x4c per iteration)
//   piVar1 = &DAT_0067e85c  (ability table, per-profile stride 12;
//                            0x30 bytes = 4 * 12 per iteration)
// The negative displacements resolve to the ACTIVE array: 0x7f1504 - 0x4c0 =
// 0x7f1044, i.e. active[col] with col 0 / col 1 -- the same two columns
// FUN_0042fa00 reads, NOT left/right. The edge test is `active != 0 &&
// processed == 0` (newly pressed), MASHED's standard protocol; an earlier
// reading of these as "release edges" had the two arrays backwards.
//
// Per iteration, 4 profiles x 2 directions (profile p at active/processed
// offset p * 0x4c, ability entry piVar1[3 * p - 3]):
//   p0 col0: pcVar2[-0x4c0] != 0 && pcVar2[0]    == 0 => piVar1[-3]-- (guard != 0)
//   p0 col1: pcVar2[-0x4bf] != 0 && pcVar2[1]    == 0 => piVar1[-3]++ (guard  < 3)
//   p1 col0: pcVar2[-0x474] != 0 && pcVar2[0x4c] == 0 => *piVar1--    (guard != 0)
//   p1 col1: pcVar2[-0x473] != 0 && pcVar2[0x4d] == 0 => *piVar1++    (guard  < 3)
//   p2 col0: pcVar2[-0x428] != 0 && pcVar2[0x98] == 0 => piVar1[3]--  (guard != 0)
//   p2 col1: pcVar2[-0x427] != 0 && pcVar2[0x99] == 0 => piVar1[3]++  (guard  < 3)
//   p3 col0: pcVar2[-0x3dc] != 0 && pcVar2[0xe4] == 0 => piVar1[6]--  (guard != 0)
//   p3 col1: pcVar2[-0x3db] != 0 && pcVar2[0xe5] == 0 => piVar1[6]++  (guard  < 3)
// Range [0, 3] = the four ability columns (Elite / Pro / Amateur / Rookie).
// Unlike the team step there is NO slot-assignment guard: every profile's entry
// is editable whether or not it holds a car slot.
//
// Loop end: pcVar2 >= 0x7f1894 (the original is a do-while; pcVar2 starts at
// 0x7f1504, so the pre-test form below is equivalent).
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042f7b0.md
// ref: re/analysis/race_hud_capture_20260902.md (Finding 29)
// ---------------------------------------------------------------------------

// 0x0042f7b0
extern "C" __declspec(dllexport) void __cdecl FrontendCursorUpdate() {
    if (*reinterpret_cast<std::int32_t*>(0x0067eab0u) != 0) {
        return;
    }

    std::int8_t*  pcVar2 = reinterpret_cast<std::int8_t*>(0x007f1504u);
    std::int32_t* piVar1 = reinterpret_cast<std::int32_t*>(0x0067e85cu);

    while (reinterpret_cast<std::uintptr_t>(pcVar2) < 0x007f1894u) {
        // profile 0, col 0: newly pressed -> decrement piVar1[-3], guard != 0
        if (pcVar2[-0x4c0] != 0 && pcVar2[0] == 0) {
            if (piVar1[-3] > 0) {
                piVar1[-3]--;
            }
        }
        // profile 0, col 1: newly pressed -> increment piVar1[-3], guard < 3
        if (pcVar2[-0x4bf] != 0 && pcVar2[1] == 0) {
            if (piVar1[-3] < 3) {
                piVar1[-3]++;
            }
        }
        // profile 1, col 0: newly pressed -> decrement *piVar1, guard != 0
        if (pcVar2[-0x474] != 0 && pcVar2[0x4c] == 0) {
            if (*piVar1 > 0) {
                (*piVar1)--;
            }
        }
        // profile 1, col 1: newly pressed -> increment *piVar1, guard < 3
        if (pcVar2[-0x473] != 0 && pcVar2[0x4d] == 0) {
            if (*piVar1 < 3) {
                (*piVar1)++;
            }
        }
        // profile 2, col 0: newly pressed -> decrement piVar1[3], guard != 0
        if (pcVar2[-0x428] != 0 && pcVar2[0x98] == 0) {
            if (piVar1[3] > 0) {
                piVar1[3]--;
            }
        }
        // profile 2, col 1: newly pressed -> increment piVar1[3], guard < 3
        if (pcVar2[-0x427] != 0 && pcVar2[0x99] == 0) {
            if (piVar1[3] < 3) {
                piVar1[3]++;
            }
        }
        // profile 3, col 0: newly pressed -> decrement piVar1[6], guard != 0
        if (pcVar2[-0x3dc] != 0 && pcVar2[0xe4] == 0) {
            if (piVar1[6] > 0) {
                piVar1[6]--;
            }
        }
        // profile 3, col 1: newly pressed -> increment piVar1[6], guard < 3
        if (pcVar2[-0x3db] != 0 && pcVar2[0xe5] == 0) {
            if (piVar1[6] < 3) {
                piVar1[6]++;
            }
        }

        // Advance one unrolled group of 4 profiles: input 0x130 = 4 * 0x4c,
        // ability table 0x30 bytes = 4 * 12
        pcVar2 += 0x130;
        piVar1 += 0x30 / sizeof(std::int32_t);  // 12 ints
    }
}

RH_ScopedInstall(FrontendCursorUpdate, 0x0042f7b0);  // re-enabled 2026-05-24 c3-frontend-b

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

RH_ScopedInstall(LocalPlayerSlotCheck, 0x00436810);  // re-enabled 2026-05-24 c3-frontend-b
