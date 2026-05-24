// Mashed RE — Save/ReplayThunk.cpp
// 0x0040de00  thunk_FUN_004117b0  (compiler-generated 4-byte JMP thunk)
// Session: save-sdone-final (2026-05-22)
//
// Mechanical description (re/analysis/timer_d3_cont1_a/0x0040de00.md):
//   4-byte JMP thunk; tail-calls 0x004117b0 (ReplaySave, vehicle/C3).
//   No prologue, no stack frame, no arguments — pure unconditional JMP.
//
// Anti-island gate: target 0x004117b0 (ReplaySave) is C3. Gate satisfied.
//
// Reimpl strategy:
//   ThunkReplaySave() calls ReplaySave() directly (C++ linkage to the
//   reimpl in Vehicle/Replay.cpp).  This is NOT recursive:
//     - RH_ScopedInstall(ThunkReplaySave, 0x0040de00) patches the thunk RVA.
//     - RH_ScopedInstall(ReplaySave,      0x004117b0) patches the target RVA.
//   The two are independent hooks.  ThunkReplaySave() calls ReplaySave()
//   via normal C++ linker resolution (not through 0x004117b0), so no loop.
//
// Frida evidence (arg_type: none):
//   At quiescent main menu both sides hit the save-once latch
//   (DAT_005f29c8 != 0 after first call) and return void.
//   Bit-identical under void_match: GREEN 10/10 path1.
//
// Binary anchor: MASHED.exe size=2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//
// Stubs present in TARGET body (non-blocking for this thunk's promotion):
//   S-3654  0x004117b0 body (described; already C3)
//   S-3655  0x00430820 FUN_00430820  pre-write gate
//   S-3656  0x00483ca0 FUN_00483ca0  replay_write
//   S-3657  0x004099a0 FUN_004099a0  post-save action (already C3 as AutosaveTrigger)
// These stubs live inside ReplaySave (C3), not inside this thunk. Non-blocking.
//
// Uncertainties (non-blocking):
//   U-3641  second sprintf result possibly dead-store inside ReplaySave body.

#include "../Core/HookSystem.h"

// Forward declaration — defined in Vehicle/Replay.cpp, linked into the same DLL.
extern "C" __declspec(dllexport) void __cdecl ReplaySave();

// 0x0040de00  thunk_FUN_004117b0
// Compiler-generated 4-byte JMP thunk that tail-calls ReplaySave (0x004117b0).
// Our reimpl is a one-liner that calls the already-C3 ReplaySave reimpl directly.
extern "C" __declspec(dllexport) void __cdecl ThunkReplaySave() {
    ReplaySave();
}
// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(ThunkReplaySave, 0x0040de00);
