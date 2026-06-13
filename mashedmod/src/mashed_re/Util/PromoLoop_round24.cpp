// Mashed RE — promote-round round 24 (resume: clean shapes, existing handlers).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x00496900  SlotActiveThunk         — vehicle; 4B thunk -> 0x00497450
//   0x00415860  InteractionCooldownSet  — ai; indexed constant setter
//
// Both bodies byte-verified in original\MASHED.exe.unpatched 2026-06-13.
//
// Analysis:
//   re/analysis/bucket_vehicle_004922e0_0057c500/0x00496900.md
//   re/analysis/bucket_ai_00407a40_00415880/0x00415860.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// SlotActiveThunk  --  0x00496900   (subsystem: vehicle)
//
// Original: thunk_FUN_00497450 (5 bytes, 0x00496900..0x00496904)
// Bytes: E9 4B 0B 00 00   (jmp rel32 -> 0x00496905 + 0xB4B = 0x00497450)
// Signature: forwards int player_idx to FUN_00497450 (slot-active predicate).
//
// FUN_00497450 (input C2): returns 0 for idx > 3, else the slot-active byte at
// (&DAT_007e96fc)[idx*0x80] != 0 (live race state). The reimpl call-throughs
// to the original-image predicate (a call vs the thunk's jmp is observationally
// identical for the return value).
//
// Constants (cited from thunk body at 0x00496900):
//   0x00497450 — jump target (rel32 0xB4B)
//
// Caller: FUN_00432080 (vehicle, C3). Callee: FUN_00497450 (input, C2).
//
// Uncertainties (non-blocking):
//   U-1509: meaning of the slot-active value (data-semantic, in the callee).
// ---------------------------------------------------------------------------

// 0x00496900
extern "C" __declspec(dllexport) std::uint32_t __cdecl SlotActiveThunk(int player_idx) {
    // 0x00497450 target cited at 0x00496900 thunk body (E9 rel32 = 0xB4B).
    using Fn = std::uint32_t(__cdecl*)(int);
    return reinterpret_cast<Fn>(0x00497450u)(player_idx);
}

RH_ScopedInstall(SlotActiveThunk, 0x00496900);

// ---------------------------------------------------------------------------
// InteractionCooldownSet  --  0x00415860   (subsystem: ai)
//
// Original: FUN_00415860 (17 bytes, 0x00415860..0x00415871)
// Bytes: 8B 44 24 04 / 6B C0 74 / C7 80 08 A5 89 00 30 75 00 00 / C3
//   (mov eax,[esp+4]; imul eax,eax,0x74;
//    mov dword ptr [eax+0x0089a508],0x7530; ret)
// Signature: void FUN_00415860(int param_1)
//
// Writes the constant 30000 (0x7530) to the per-slot interaction-cooldown
// entry at 0x0089a508 + param_1*0x74 (re-arms the countdown decremented in
// FUN_00415880 ram-targeting).
//
// Constants (cited from function body at 0x00415860):
//   0x0089a508 — per-player interaction-cooldown table base (stride 0x74)
//   0x7530     — cooldown reset value (30000)
//
// Caller: FUN_004252c0 (vehicle proximity/interaction detector, C2). Leaf,
// no uncertainties.
// ---------------------------------------------------------------------------

// 0x00415860
extern "C" __declspec(dllexport) void __cdecl InteractionCooldownSet(int param_1) {
    // 0x0089a508 base + 0x74 byte stride + value 0x7530 cited at 0x00415860 body.
    *reinterpret_cast<std::uint32_t*>(
        0x0089a508u + static_cast<std::uint32_t>(param_1) * 0x74u) = 0x7530u;
}

RH_ScopedInstall(InteractionCooldownSet, 0x00415860);
