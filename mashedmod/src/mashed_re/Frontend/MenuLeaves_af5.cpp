// Mashed RE — Frontend menu-leaf reimplementations (c3-batch-af session 5, HARVEST).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions AUTHORED + VERIFIED in this file (3 of 9 candidates):
//   0x00431b70  MenuFlagDat007f0f10Get  — int(void) getter: return DAT_007f0f10
//   0x004298c0  MenuQuadGlobalZero      — void(void): zeroes 4 dword globals
//   0x00423b00  FrontendInputDispatch   — void(void): guard + 4-call menu-input thunk
//
// Candidates DEFERRED this session (NOT authored — see PROMOTION_QUEUE fragment):
//   0x00494460  VideoClose            — tail of chain calls D3D/RW device-vtable
//                                       teardown (FUN_004c7650 → DAT_007d3ff8 vtable
//                                       [+0x5c],[+0x11c]); synthetic 10x call at menu
//                                       is unsafe; needs live video-context state.
//   0x00432450  MenuStateMirrorSync   — ends in tail-call JMP to 6-arg FUN_0042bfb0
//                                       inheriting the caller frame; faithful port
//                                       requires naked-asm tail-jmp.
//   0x00429290  FrameStateDispatch    — per-frame begin/inputpoll/render/audio/present
//                                       dispatcher (5 callees incl flip); non-
//                                       deterministic side effects, not synth-diffable.
//   0x00401da0  RwSelectedEntryXform  — open structural uncertainties U-4197/U-4198
//                                       (both flagged "C3 of 0x00401da0"); C3 gated.
//   0x0043aa30  TeamPairingRender     — uncatalogued render callees (0x0042f8d0,
//                                       0x0042fab0, 0x0042bcb0, 0x004282a0, 0x00427f00)
//                                       + live RW device vtable; not a leaf.
//   0x00448220  PostRaceResultCamera  — requires live race state; open U-1870/U-1873.
//
// Analysis notes:
//   re/analysis/bucket_0041dc30/0x00431b70.md
//   re/analysis/frontend_c1_to_c2_s6/FUN_004298c0.md
//   re/analysis/frontend_gate_unblock_u/0x00423b00.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Forward declarations for callees used in this file (raw-address thunks).
// All four are void(void) per Ghidra (decomp + body epilogue confirmed):
//   0x00423040  FrontendDirInput  — directional-input handler (C2)
//   0x00423270  TabCycler         — menu tab cycler (C3)
//   0x00423320  CursorMover       — grid cursor movement (C3)
//   0x00423670  SplineEditorTick  — runtime AI-spline editor; void(void) (C1)
// ---------------------------------------------------------------------------
static auto* const s_FrontendDirInput =
    reinterpret_cast<void(__cdecl*)(void)>(0x00423040);
static auto* const s_TabCycler =
    reinterpret_cast<void(__cdecl*)(void)>(0x00423270);
static auto* const s_CursorMover =
    reinterpret_cast<void(__cdecl*)(void)>(0x00423320);
static auto* const s_SplineEditorTick =
    reinterpret_cast<void(__cdecl*)(void)>(0x00423670);

// ---------------------------------------------------------------------------
// MenuFlagDat007f0f10Get  --  0x00431b70
//
// Original: FUN_00431b70 (5 bytes, 0x00431b70..0x00431b74)
// Signature: undefined4 FUN_00431b70(void)
// Body (verbatim decomp):
//   return DAT_007f0f10;
//
// Pure getter leaf: reads and returns the 4-byte global at 0x007f0f10.
//
// Constants cited from 0x00431b70 body:
//   0x007f0f10 — DAT_007f0f10, global int returned.
//
// Verification: read_global — sentinel written to 0x007f0f10, fn() must return it.
// ref: re/analysis/bucket_0041dc30/0x00431b70.md
// ---------------------------------------------------------------------------

// 0x00431b70
extern "C" __declspec(dllexport) std::uint32_t __cdecl MenuFlagDat007f0f10Get(void) {
    // return DAT_007f0f10;  (0x007f0f10)
    return *reinterpret_cast<std::uint32_t*>(0x007f0f10u);
}

RH_ScopedInstall(MenuFlagDat007f0f10Get, 0x00431b70);  // c3-batch-af-s5

// ---------------------------------------------------------------------------
// MenuQuadGlobalZero  --  0x004298c0
//
// Original: FUN_004298c0 (27 bytes, 0x004298c0..0x004298da)
// Signature: void FUN_004298c0(void)
// Body (verbatim decomp + disassembly):
//   XOR EAX,EAX                              @ 0x004298c0
//   MOV dword ptr [0x0067d99c],0x0           @ 0x004298c2  (_DAT_0067d99c = 0)
//   MOV [0x0067d994],EAX                     @ 0x004298cc  (DAT_0067d994  = 0)
//   MOV [0x0067d98c],EAX                     @ 0x004298d1  (DAT_0067d98c  = 0)
//   MOV [0x008991bc],EAX                     @ 0x004298d6  (DAT_008991bc  = 0)
//   RET                                      @ 0x004298db
//
// Pure leaf — no callees, no branches; four 32-bit zero stores in fixed order.
//
// Constants cited from 0x004298c0 body (all immediate 0):
//   0x0067d99c, 0x0067d994, 0x0067d98c, 0x008991bc — zeroed globals.
//
// Verification: none (void leaf). Reimpl emits the identical four dword stores;
// asm-equivalence confirmed against the disassembly above.
// ref: re/analysis/frontend_c1_to_c2_s6/FUN_004298c0.md
// ---------------------------------------------------------------------------

// 0x004298c0
extern "C" __declspec(dllexport) void __cdecl MenuQuadGlobalZero(void) {
    *reinterpret_cast<std::uint32_t*>(0x0067d99cu) = 0;  // _DAT_0067d99c [0x004298c2]
    *reinterpret_cast<std::uint32_t*>(0x0067d994u) = 0;  // DAT_0067d994  [0x004298cc]
    *reinterpret_cast<std::uint32_t*>(0x0067d98cu) = 0;  // DAT_0067d98c  [0x004298d1]
    *reinterpret_cast<std::uint32_t*>(0x008991bcu) = 0;  // DAT_008991bc  [0x004298d6]
}

RH_ScopedInstall(MenuQuadGlobalZero, 0x004298c0);  // c3-batch-af-s5

// ---------------------------------------------------------------------------
// FrontendInputDispatch  --  0x00423b00
//
// Original: FUN_00423b00 (29 bytes, 0x00423b00..0x00423b1c)
// Signature: void FUN_00423b00(void)
// Body (verbatim decomp + disassembly):
//   CMP dword ptr [0x007f1a50],0x1   @ 0x00423b00
//   JNZ 0x00423b1d                   @ 0x00423b07   (guard: only run if == 1)
//   CALL 0x00423040                  @ 0x00423b09   FrontendDirInput
//   CALL 0x00423270                  @ 0x00423b0e   TabCycler
//   CALL 0x00423320                  @ 0x00423b13   CursorMover
//   JMP  0x00423670                  @ 0x00423b18   SplineEditorTick (tail-call)
//   RET                              @ 0x00423b1d
//
// Per-frame frontend menu-input dispatcher: gated by DAT_007f1a50 == 1, fans out
// to the directional-input handler + tab cycler + cursor mover + the AI-spline
// editor tick. The trailing JMP is a tail-call to a void(void) function, so it is
// observationally identical to a plain call followed by return.
//
// Constants cited from 0x00423b00 body:
//   0x007f1a50 — DAT_007f1a50, int32 guard; callees run iff == 1.
//   0x1        — guard comparison value.
//
// Verification: none (void dispatcher). Reimpl is a verbatim guard + 4-call
// transcription; the per-session diff confirms non-crash with the live callee
// chain at the menu. (See PROMOTION_QUEUE note: weak `none` evidence — central
// classify should confirm asm-equivalence of the emitted call sequence.)
// ref: re/analysis/frontend_gate_unblock_u/0x00423b00.md
// ---------------------------------------------------------------------------

// 0x00423b00
extern "C" __declspec(dllexport) void __cdecl FrontendInputDispatch(void) {
    // if (DAT_007f1a50 == 1) { ... }   [0x00423b00]
    if (*reinterpret_cast<std::int32_t*>(0x007f1a50u) == 1) {
        s_FrontendDirInput();   // CALL 0x00423040 [0x00423b09]
        s_TabCycler();          // CALL 0x00423270 [0x00423b0e]
        s_CursorMover();        // CALL 0x00423320 [0x00423b13]
        s_SplineEditorTick();   // JMP  0x00423670 [0x00423b18] (tail-call)
    }
}

RH_ScopedInstall(FrontendInputDispatch, 0x00423b00);  // c3-batch-af-s5
