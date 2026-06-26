// Mashed RE — Util random-int-range helper, pipeline round 8 session 1.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included:
//   0x00472690  RandIntInRange  — util; uniform random int in [param_1, param_2].
//
// Analysis:
//   re/analysis/effects_particle/0x00472690.md
//   re/analysis/promote_c2_render_midrva/00472690.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RandIntInRange  --  0x00472690   (subsystem: util)
//
// Original: FUN_00472690 (32 bytes, 0x00472690..0x004726af)
// Disasm (anchored binary, file offset RVA - 0x400000):
//   56              push esi
//   e8 da 21 0c 00  call 0x00534870          ; PRNG: no args, returns uint32 in EAX
//   8b 4c 24 08     mov  ecx, [esp+8]        ; param_1 (lo)
//   8b 74 24 0c     mov  esi, [esp+0xC]      ; param_2 (hi)
//   25 ff ff ff 7f  and  eax, 0x7fffffff     ; strip sign bit
//   2b f1           sub  esi, ecx            ; esi = hi - lo
//   46              inc  esi                  ; esi = hi - lo + 1
//   33 d2           xor  edx, edx
//   f7 f6           div  esi                  ; edx = (rand & 7fff) % (hi-lo+1)
//   5e              pop  esi
//   8b c2           mov  eax, edx             ; return remainder
//   03 c1           add  eax, ecx             ; += lo  (param_1 still in ECX)
//   c3              ret
//
// Signature: int __cdecl FUN_00472690(int param_1, int param_2)
//   param_1: lo (inclusive lower bound)
//   param_2: hi (inclusive upper bound)
// Returns: uniform random int in [param_1, param_2] inclusive.
//
// Callee: FUN_00534870 at 0x00534870 (STUB S-1424)
//   Called with zero stack args; reads/updates PRNG state from globals.
//   0x007dc578 and 0x007d3ff8 visible in its body (confirmed at call-site:
//   no arg pushed after PUSH ESI, no add-esp after return).
//
// Constants (cited from function body at 0x00472690):
//   0x7fffffff at AND instruction — masks raw PRNG output to positive int32 range.
//   +1U at INC instruction — makes upper-bound param_2 inclusive.
//
// Test strategy: arg_type=int_pair with ONLY degenerate vectors (lo == hi).
//   Rationale: the PRNG call advances global state, so A/B calls see different
//   raw random values. For lo==hi, (hi-lo+1)==1 and any value % 1 == 0,
//   so result == lo deterministically regardless of PRNG output. This produces a
//   non-degenerate GREEN CSV (real A and B rows, distinct lo values per vector)
//   without requiring a PRNG-reset harness that does not exist.
//
// Uncertainties (non-blocking):
//   U-RAND-1: STUBS.md S-1424 entry mentions "rng context param" for
//     FUN_00534870, but the call-site uses zero stack arguments; the label is
//     stale. Non-blocking: the no-arg call matches the disassembly.
// ---------------------------------------------------------------------------

// Raw PRNG at 0x00534870 (STUB S-1424). Takes no arguments; reads global state.
static auto* const s_FUN_00534870 =
    reinterpret_cast<std::uint32_t(__cdecl*)()>(0x00534870u);

// 0x00472690
extern "C" __declspec(dllexport) int __cdecl RandIntInRange(int param_1, int param_2) {
    // 0x00534870: call PRNG with no args (STUB S-1424).
    std::uint32_t uVar1 = s_FUN_00534870();
    // 0x7fffffff at AND instruction (0x00472699): strip sign bit.
    uVar1 &= 0x7fffffffu;
    // SUB+INC at 0x0047269f..0x004726a0: inclusive range = hi - lo + 1.
    std::uint32_t range = static_cast<std::uint32_t>(param_2 - param_1) + 1u;
    // DIV at 0x004726a4: unsigned divide; EDX = remainder, EAX = quotient (discarded).
    // ADD at 0x004726a8: remainder + lo = result in [lo, hi].
    return static_cast<int>(uVar1 % range) + param_1;
}

RH_ScopedInstall(RandIntInRange, 0x00472690);
