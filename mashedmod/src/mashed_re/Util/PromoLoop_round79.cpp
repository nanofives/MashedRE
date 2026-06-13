// Mashed RE — promote-round round 79 (frontier-tool validation batch: 3 display-independent leaves).
//
// FIRST batch surfaced by the new tooling pipeline:
//   scripts/promote_frontier.py  — graph-level leaf finder (capstone call-graph over
//     MASHED.exe.unpatched): C2 ∩ first-party ∩ zero-callee ∩ <260B ∩ C2+ caller.
//   scripts/promote_classify.py  — disasm-shape auto-classifier; tags each frontier
//     row AUTO (display-independent, emittable) / STATE (needs a booted game) / MANUAL.
// All three below are AUTO pure leaves diffed via early_window_leaf_diff.py (no booted
// game needed — the display is environmentally wedged this session).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00495520  input — byte-verified: A1 78 1E 77 00 C3
//   MOV EAX,[0x00771e78] ; RET   (read_global u32; direct caller FUN_00496040 C2)
extern "C" __declspec(dllexport) std::uint32_t __cdecl Get771e78(void) {
    return *reinterpret_cast<std::uint32_t*>(0x00771e78u);
}
RH_ScopedInstall(Get771e78, 0x00495520);

// 0x004842b0  debug-overlay — byte-verified: 33 C0 / A3.. x4 / C3
//   XOR EAX,EAX ; MOV [0x6cfc88],EAX ; MOV [0x6cf068],EAX ; MOV [0x6cf06c],EAX ;
//   MOV [0x6cfc8c],EAX ; RET   (4-global zero-clear; direct caller FUN_00484580 C2)
extern "C" __declspec(dllexport) void __cdecl Clear4842b0(void) {
    *reinterpret_cast<std::uint32_t*>(0x006cfc88u) = 0;   // MOV [0x6cfc88],EAX
    *reinterpret_cast<std::uint32_t*>(0x006cf068u) = 0;   // MOV [0x6cf068],EAX
    *reinterpret_cast<std::uint32_t*>(0x006cf06cu) = 0;   // MOV [0x6cf06c],EAX
    *reinterpret_cast<std::uint32_t*>(0x006cfc8cu) = 0;   // MOV [0x6cfc8c],EAX
}
RH_ScopedInstall(Clear4842b0, 0x004842b0);

// NOTE: 0x005c9d00 (XOR EAX,EAX; RET = GetRaceEndTrigger) was surfaced by the
// classifier as const_return 0 and diffs GREEN bit-identically — but it is DELIBERATELY
// NOT hooked here. Its body is 3 bytes; the 5-byte inline-JMP patch overwrites past the
// function boundary into the next function (confirmed crasher, demoted C3->C2 2026-05-22).
// early_window proves bit-identity (hook bypassed) but cannot catch the install-time
// corruption. Needs a 2-byte/trampoline patch mechanism before it can be a safe C3.
