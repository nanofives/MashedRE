// Mashed RE — Frontend menu-score and lap-data getters.
// Six trivial pure-leaf getters from the Frontend subsystem.
// All are indexed reads or direct global dereferences with no callees.
// Per the NO-GUESSING rule, names are semantically grounded in the
// Ghidra decomp literal — no inferred intent beyond what the disasm shows.
//
// 0x0040b6b0  FUN_0040b6b0  12B  c3-batch-b-s1  C2 -> C3
//   Disasm: MOV EAX, [DAT_008a9530 + param_1*4]   ; indexed read, mode-score array
//           RET
//
// 0x0040b7a0  FUN_0040b7a0  6B   c3-batch-b-s1  C2 -> C3
//   Disasm: A1 EC B8 63 00   MOV EAX, [0x0063b8ec]
//           C3               RET
//
// 0x0040b7b0  FUN_0040b7b0  66B  c3-batch-b-s1  C2 -> C3
//   Disasm: switch(param_2) { 0: DAT_008a9530[param_1]; 1: DAT_008a9560[param_1];
//                              2: DAT_008a9540[param_1]; 3: DAT_008a9550[param_1]; default: 0 }
//
// 0x00429870  FUN_00429870  79B  c3-batch-b-s1  C2 -> C3
//   Disasm: time_A = DAT_0067d98c*0x3c + DAT_0067d994 + _DAT_0067d99c
//           time_B = DAT_0067d990*0x3c + DAT_0067d998 + _DAT_0067d9a0
//           return (time_A < time_B) ? 1 : 0
//
// 0x00429a70  FUN_00429a70  12B  c3-batch-b-s1  C2 -> C3
//   Disasm: FLD  [DAT_0067d99c + param_1*4]   ; indexed float read, frac array
//           FSTP [esp-4] ; return float
//           RET
//
// 0x00429a80  FUN_00429a80  12B  c3-batch-b-s1  C2 -> C3
//   Disasm: MOV EAX, [DAT_0067d98c + param_1*4]   ; indexed read, laps array
//           RET// Mashed RE - Frontend menu score / race-data getter reimplementations.
// Analysis notes:
//   re/analysis/frontend_promote_menus_b/00429a90.md   (LapSecsGetBySlot)
//   re/analysis/frontend_promote_menus_b/00430760.md   (IsMultiplayerMode)
//   re/analysis/frontend_promote_menus_b/0040b620.md   (SlotSortByModeScore)
//
// All functions in this file are pure leaves (callees_depth1: []).
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
#include "../Core/HookSystem.h"

#include <cstdint>

// MATCH-S02: the body of 0x0040b6b0 (ModeScoreGetBySlot) moved to Match/MatchS02_0040b6b0.cpp,
// which compiles the byte-matched source re/match/s02/0040b6b0.c.

// MATCH-S02: the body of 0x0040b7a0 (HotkeyStringBaseGet) moved to Match/MatchS02_0040b7a0.cpp,
// which compiles the byte-matched source re/match/s02/0040b7a0.c.

// MATCH-S02: the body of 0x0040b7b0 (PlayerHotkeyTableGet) moved to Match/MatchS02_0040b7b0.cpp,
// which compiles the byte-matched source re/match/s02/0040b7b0.c.

// MATCH-S06: the body of 0x00429870 (LapTimeALessThanB) moved to Match/MatchS06_00429870.cpp,
// which compiles the byte-matched source re/match/s06/00429870.c.

// MATCH-S06: the body of 0x00429a70 (LapFracGetBySlot) moved to Match/MatchS06_00429a70.cpp,
// which compiles the byte-matched source re/match/s06/00429a70.c.

// MATCH-S06: the body of 0x00429a80 (LapLapsGetBySlot) moved to Match/MatchS06_00429a80.cpp,
// which compiles the byte-matched source re/match/s06/00429a80.c.

// ---------------------------------------------------------------------------
// LapSecsGetBySlot  --  0x00429a90
//
// Original: FUN_00429a90 (12 bytes, 0x00429a90..0x00429a9b)
// Returns: (&DAT_0067d994)[param_1] — indexed read of lap-time seconds component
//   by slot index.  Same pattern as FUN_00429870 (time-A seconds).
// No callees, no branches, no side-effects.
// ref: re/analysis/frontend_promote_menus_b/00429a90.md
// ---------------------------------------------------------------------------

// MATCH-S06: the body of 0x00429a90 (LapSecsGetBySlot) moved to Match/MatchS06_00429a90.cpp,
// which compiles the byte-matched source re/match/s06/00429a90.c.

// ---------------------------------------------------------------------------
// IsMultiplayerMode  --  0x00430760
//
// Original: FUN_00430760 (38 bytes, 0x00430760..0x00430786)
// Returns: 1 if DAT_0067e9fc (game mode global, cited at 0x00430764) is any of
//   {2, 3, 4, 5, 10}; else 0.
// Decomp: if (DAT_0067e9fc == 2 || DAT_0067e9fc == 10 || DAT_0067e9fc == 3
//              || DAT_0067e9fc == 4 || DAT_0067e9fc == 5) return 1; return 0;
// No callees, no side-effects.
// ref: re/analysis/frontend_promote_menus_b/00430760.md
// ---------------------------------------------------------------------------

// BUGFIX 2026-07-28 — REGISTER-ABI DEFECT (Sweep A Class A; the ECX/EDX class, 11th hit).
// The plain-C body compiled to `mov ecx,[0x67e9fc]` + a chain of `cmp ecx,imm` (measured in
// mashed_re_dev.asi at export IsMultiplayerMode, 13 instructions). The ORIGINAL uses EAX and
// ONLY EAX -- 0x00430760 `MOV EAX,[0x0067e9fc]`, five `CMP EAX,imm`, `XOR EAX,EAX`/`MOV EAX,1`
// -- so ECX survives the call, and an unhooked caller relies on exactly that:
//
//     004333fd  MOV  ECX,dword ptr [0x0067f0c0]   ; ECX loaded BEFORE the call
//     00433403  CALL 0x00430760                   ; original preserves ECX
//     00433408  TEST EAX,EAX
//     0043340a  JZ   0x004334fa
//     00433410  TEST ECX,ECX                      ; ECX read AFTER the call -> live across it
//     00433412  JZ   0x004334df
//
// With the hook installed the caller branched on a destroyed ECX. 12 call sites reference
// 0x00430760 (reference_to); 0x00433403 is the one with a proven live ECX.
//
// Transcribed verbatim from 0x00430760..0x00430786 so the register footprint is EAX-only by
// construction, not by hoping the compiler picks EAX. A `__cdecl` C body may use ECX/EDX as
// scratch at any time -- only naked asm can pin the ABI.
//
// 0x00430760
extern "C" __declspec(dllexport) __declspec(naked) std::uint32_t __cdecl IsMultiplayerMode() {
    __asm {
        // `ds:` IS LOAD-BEARING. Without it MSVC assembles `mov eax, dword ptr [0x0067e9fc]`
        // as `B8 FC E9 67 00  mov eax,67E9FCh` -- the ADDRESS as an immediate, not a load --
        // and the function would compare 0x0067e9fc against 2/3/4/5/10 and return 0 forever.
        // Caught by disassembling the .obj; it compiles clean either way. With `ds:` it emits
        // `3E A1 FC E9 67 00`, i.e. the original's `A1` load plus a redundant segment prefix.
        mov  eax, dword ptr ds:[0x0067e9fc]  // 0x00430760  MOV EAX,[0x0067e9fc]
        cmp  eax, 2                        // 0x00430765
        je   L_one                         // 0x00430768
        cmp  eax, 0xa                      // 0x0043076a
        je   L_one                         // 0x0043076d
        cmp  eax, 3                        // 0x0043076f
        je   L_one                         // 0x00430772
        cmp  eax, 4                        // 0x00430774
        je   L_one                         // 0x00430777
        cmp  eax, 5                        // 0x00430779
        je   L_one                         // 0x0043077c
        xor  eax, eax                      // 0x0043077e
        ret                                // 0x00430780
    L_one:
        mov  eax, 1                        // 0x00430781
        ret                                // 0x00430786
    }
}

RH_ScopedInstall(IsMultiplayerMode, 0x00430760);  // re-enabled 2026-05-24 c3-frontend-b

// ---------------------------------------------------------------------------
// SlotSortByModeScore  --  0x0040b620
//
// Original: FUN_0040b620 (135 bytes, 0x0040b620..0x0040b6a7)
// Signature: void FUN_0040b620(int* param_1)
//   param_1: pointer to int[4] output array; filled with slot indices sorted
//            descending by per-slot mode score.
// Globals:
//   DAT_008a9530  (+4 stride, 4 elements) — per-slot mode scores (0x0040b630)
//   DAT_007f1a14  (+0x10 stride, 4 elements) — slot activity flags (0x0040b638)
// Logic:
//   For each slot i (0..3):
//     score[i] = (DAT_007f1a14[i*4] == -1) ? -100 : DAT_008a9530[i*4]
//     output[i] = i
//   Bubble sort output[] descending by score[].
// No callees.
// ref: re/analysis/frontend_promote_menus_b/0040b620.md
// NOTE: arg_type 'void_out_ptr' (pass ptr, call, readback 4-element array) is
//   not supported by the Frida diff harness. This function is OMITTED from the
//   automated diff run.  Manual / behavioral verification only at C3 gate.
// ---------------------------------------------------------------------------

// MATCH-S02: the body of 0x0040b620 (SlotSortByModeScore) moved to Match/MatchS02_0040b620.cpp,
// which compiles the byte-matched source re/match/s02/0040b620.c.