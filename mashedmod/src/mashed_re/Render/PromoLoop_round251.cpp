// Mashed RE — promote-round round 251 (RW error-module plugin close callback).
//
// Function in this file:
//   0x004d8470  RwErrorModuleDtor — 16-byte plugin close callback for the
//               RenderWare error module (plugin id 0x40f).
//
// Analysis note: re/analysis/render_6_c1_to_c2_s3/ (rwID_ERRORMODULE_dtor)
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RwErrorModuleDtor  --  0x004d8470
//
// Original: 16 bytes, 0x004d8470..0x004d847f. Complete disassembly, read from
// original\MASHED.exe.unpatched 2026-09-12 — the whole function, not an excerpt:
//
//   0x004d8470  mov eax, [0x7d6c60]     ; load the module's open-count
//   0x004d8475  dec eax                 ; -- 1
//   0x004d8476  mov [0x7d6c60], eax     ; store it back
//   0x004d847b  mov eax, [esp+4]        ; return value = param_1, verbatim
//   0x004d847f  ret                     ; __cdecl
//
// So, exhaustively:  int fn(int object) { *(int*)0x007d6c60 -= 1; return object; }
//
// There is no branch, no other memory access, and no callee. The decrement is
// unconditional — it does NOT clamp at zero, so a port that added a guard would
// diverge on an already-zero counter. Transcribed as a plain decrement for that
// reason.
//
// REGISTRATION (how this is reached). It is a plugin callback, never called by
// name. The registration site pushes it at 0x004c335a:
//   0x004c3364  push 0x40f      ; plugin id
//   0x004c335f  push 0x4d8430   ; the sibling constructor callback
//   0x004c335a  push 0x4d8470   ; THIS function, as the close/dtor callback
//   0x004c3358  push 0
// and a byte search of the anchored image for the little-endian dword
// `70 84 4d 00` returns exactly ONE hit, at 0x004c335b — so that push is the
// only reference to this address anywhere in the image. At run time it is
// invoked by the plugin-registry teardown walker FUN_004d8060 (C2), which walks
// the list at registry+0x14 and calls the function pointer at node+0x24.
//
// Constants (both cited above):
//   0x007d6c60 — the error-module open-count      [0x004d8470 / 0x004d8476]
//   0x40f      — the plugin id, at the registration site only [0x004c3364]
//
// Uncertainties: U-5381 is open against this row but was RESOLVED 2026-09-11
// and its Blocks cell reads "nothing".
// ---------------------------------------------------------------------------

// 0x004d8470
extern "C" __declspec(dllexport) int __cdecl RwErrorModuleDtor(int object)
{
    // Unconditional decrement, no zero-clamp. [0x004d8470..0x004d8476]
    *reinterpret_cast<std::int32_t*>(0x007d6c60u) -= 1;

    // Return param_1 verbatim. [0x004d847b]
    return object;
}

RH_ScopedInstall(RwErrorModuleDtor, 0x004d8470);
