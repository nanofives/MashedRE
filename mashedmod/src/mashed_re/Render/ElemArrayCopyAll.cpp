// Mashed RE — Render hook: 0x004b4140 RenderElemArrayCopyAll
//
// SHA-256 anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//                 (preserved in original\MASHED.exe.unpatched)
#include "../Core/HookSystem.h"

// Callee hook — C3 reimpl of FUN_004b40c0 (RenderElemArrayCopy), installed in
// Render/RenderLeaves_ae2.cpp.
extern "C" void __cdecl RenderElemArrayCopy(int param_1, int param_2);

// ─────────────────────────────────────────────────────────────────────────────
// 0x004b4140  FUN_004b4140  RenderElemArrayCopyAll
// 1-arg __cdecl. Convenience wrapper: calls FUN_004b4130(param_1, 0).
// FUN_004b4130 [C3 adjustor thunk] replaces param_1 with param_1[0x18] (the
// sub-struct pointer s) and tail-calls FUN_004b40c0(s, 0) [C3 RenderElemArrayCopy].
// With param_2=0, FUN_004b40c0 reads count = *(s+0x24) and returns immediately
// (loop guard `param_2 != 0` is false) — the combined call is a no-op on memory.
//
// Reimpl reproduces the adjustor step (load s = param_1[0x18]) and calls
// RenderElemArrayCopy(s, 0), matching the original call chain bit-for-bit.
//
// ASM (0x004b4140..0x004b414f):
//   MOV  EAX,[ESP+0x4]    ; param_1 (p)
//   PUSH 0x0               ; arg2 = 0  (copy-destination = null)
//   PUSH EAX               ; arg1 = p
//   CALL 0x004b4130        ; FUN_004b4130(p, 0) -> adjustor -> FUN_004b40c0(p[0x18], 0)
//   ADD  ESP,0x8           ; cdecl caller cleanup
//   RET                    ; no immediate -> __cdecl
//
// ref: re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x004b4140.md
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport)
void __cdecl RenderElemArrayCopyAll(int param_1)
{
    // Adjustor step: FUN_004b4130 reads s = param_1[0x18] before calling FUN_004b40c0
    const int s = *reinterpret_cast<const int*>(param_1 + 0x18);
    // FUN_004b40c0(s, 0): reads count from s+0x24; loop skipped because param_2==0
    RenderElemArrayCopy(s, 0);
}

RH_ScopedInstall(RenderElemArrayCopyAll, 0x004b4140);
