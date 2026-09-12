// Mashed RE — promote-round round 256 (five forwarders).
//
// NAMING: these are MECHANICAL names, deliberately. Fwd<callee>_<literal> says what
// the function demonstrably does -- forwards to that callee binding that literal --
// and claims nothing about why. Earlier drafts called them ParticlePool703068Reset,
// SpriteBatch63bd50Reset and InputAxisScaleBiasApply; none of those roles is
// supported by anything read here, and a wrong name in hooks.csv is worse than a
// dull one. Where a name IS grounded it is used: 0x00428610 is the C4 row
// ViewportScaledRectDraw, and 0x004e66d0 is RpClumpForAllAtomics.
//
// Functions in this file:
//   0x00487140  Fwd4768c0_703068  — FUN_004768c0(&DAT_00703068)
//   0x00413bb0  Fwd4768c0_63bd50   — FUN_004768c0(&DAT_0063bd50)   [sibling]
//   0x00428760  ViewportScaledRectDraw_Arg7Zero            — FUN_00428610(a1..a6, 0)
//   0x004b5580  ClumpForAllAtomics_Cb4b5560   — RpClumpForAllAtomics(a1, FUN_004b5560, a2)
//   0x00495080  Fwd494fd0_DoublePlusHalf  — FUN_00494fd0(a1 + a1 + 0.5f, a2)
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Fwd4768c0_703068  --  0x00487140   and
// Fwd4768c0_63bd50   --  0x00413bb0
//
//   0x00487140  push 0x703068 / call 0x4768c0 / pop ecx / ret
//   0x00413bb0  push 0x63bd50 / call 0x4768c0 / pop ecx / ret
//
// A SIBLING PAIR: identical four-instruction bodies calling the SAME callee,
// differing ONLY in the pushed literal. That literal is the entire content of
// each function, so each one's A/B asserts its own address and would fail if the
// two were swapped.
//
// `pop ecx` is the one-argument cleanup (equivalent to `add esp,4`) and does NOT
// touch EAX, so the callee's return falls through in both. Ghidra types both
// `void`; that is the same mistyping corrected on 0x004c2d90 in r249, so both
// are transcribed as returning the callee's value.
// ---------------------------------------------------------------------------

typedef int(__cdecl* PoolReset_t)(std::uint32_t);

// 0x00487140
extern "C" __declspec(dllexport) int __cdecl Fwd4768c0_703068()
{
    return reinterpret_cast<PoolReset_t>(0x004768c0u)(0x00703068u);
}

RH_ScopedInstall(Fwd4768c0_703068, 0x00487140);

// 0x00413bb0
extern "C" __declspec(dllexport) int __cdecl Fwd4768c0_63bd50()
{
    return reinterpret_cast<PoolReset_t>(0x004768c0u)(0x0063bd50u);
}

RH_ScopedInstall(Fwd4768c0_63bd50, 0x00413bb0);

// ---------------------------------------------------------------------------
// ViewportScaledRectDraw_Arg7Zero  --  0x00428760
//
// Six forwarded arguments plus a HARD-CODED trailing 0, cleaned with
// `add esp,0x1c` at 0x00428785 — 28 bytes, i.e. SEVEN arguments, which is what
// fixes the arity. The prologue reloads each argument at a different ESP depth
// as the pushes accumulate (0x00428760..0x0042877f), so the mapping has to be
// worked rather than pattern-matched:
//   [esp+0x18]->a6, [esp+0x14]->a5, [esp+0x10]->a4, then after 2/3/4 pushes
//   [esp+0x14] addresses a3, a2, a1 in turn.
// Push order is 0, a6, a5, a4, a3, a2, a1 — so the callee sees
// (a1, a2, a3, a4, a5, a6, 0). 5 callers.
// ---------------------------------------------------------------------------

typedef int(__cdecl* ViewportScaledRectDraw7_t)(std::uint32_t, std::uint32_t, std::uint32_t,
                                      std::uint32_t, std::uint32_t, std::uint32_t,
                                      std::uint32_t);

// 0x00428760
extern "C" __declspec(dllexport) int __cdecl ViewportScaledRectDraw_Arg7Zero(
    std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
    std::uint32_t a4, std::uint32_t a5, std::uint32_t a6)
{
    return reinterpret_cast<ViewportScaledRectDraw7_t>(0x00428610u)(a1, a2, a3, a4, a5, a6, 0u);
}

RH_ScopedInstall(ViewportScaledRectDraw_Arg7Zero, 0x00428760);

// ---------------------------------------------------------------------------
// ClumpForAllAtomics_Cb4b5560  --  0x004b5580
//
//   push eax(a2) / push 0x4b5560 / push ecx(a1) / call 0x4e66d0 / add esp,0xc
//
// RpClumpForAllAtomics(a1, FUN_004b5560, a2) — the middle argument is a LITERAL
// CALLBACK ADDRESS, and it is the whole point of the function. The A/B records
// it, so a port that bound the wrong callback fails even though the call shape
// and argument count would be identical.
//
// The callee at 0x004e66d0 is RpClumpForAllAtomics; U-4416 (resolved 2026-09-11)
// records its mechanics: a circular-list walk invoking
// `(*param_2)(puVar2 + -0x10, param_3)` per node and early-outing when the
// callback returns 0. It is stubbed here, so none of that runs.
// ---------------------------------------------------------------------------

typedef int(__cdecl* RpClumpForAllAtomics_t)(std::uint32_t, std::uint32_t, std::uint32_t);

// 0x004b5580
extern "C" __declspec(dllexport) int __cdecl ClumpForAllAtomics_Cb4b5560(
    std::uint32_t clump, std::uint32_t user)
{
    return reinterpret_cast<RpClumpForAllAtomics_t>(0x004e66d0u)(
        clump, 0x004b5560u, user);
}

RH_ScopedInstall(ClumpForAllAtomics_Cb4b5560, 0x004b5580);

// ---------------------------------------------------------------------------
// Fwd494fd0_DoublePlusHalf  --  0x00495080
//
// Complete disassembly (0x00495080..0x004950a5):
//   fld   dword ptr [esp+4]      ; a1
//   mov   eax,[esp+8]            ; a2
//   fadd  st(0),st(0)            ; a1 + a1   (exact: a doubling)
//   push  esi
//   mov   esi,[0x771a0c]         ; DEAD LOAD - ESI is never read afterwards
//   push  eax                    ; a2
//   fadd  dword ptr [0x5cc32c]   ; + 0.5f
//   push  ecx                    ; placeholder to RESERVE the float's stack slot
//   fstp  dword ptr [esp]        ; store the float OVER that placeholder
//   call  0x494fd0               ; FUN_00494fd0(float, a2)
//   add   esp,8 / pop esi / ret
//
// So: FUN_00494fd0(a1 + a1 + 0.5f, a2). The constant at 0x005cc32c is read from
// the anchored binary as bytes `00 00 00 3f`, dword 0x3f000000, float 0.5 — the
// same constant already confirmed for U-5654.
//
// NAKED ASM, for the r254 reason. The x87 stream keeps an 80-bit intermediate
// across `fadd st,st` and `fadd [0x5cc32c]` and rounds to 32-bit ONCE at the
// `fstp`. A C expression compiled to SSE2 would round after EACH step, which can
// differ in the last bit. The doubling itself is exact, but the `+ 0.5f` is not
// guaranteed to be, and the build already mixes SSE2 and /arch:IA32 translation
// units — so the safe transcription is the instruction stream, not the algebra.
//
// TWO DETAILS TRANSCRIBED RATHER THAN TIDIED:
//   * `mov esi,[0x771a0c]` is a dead load — ESI is pushed, loaded, and popped
//     without ever being read. Kept, because removing it would change the memory
//     accesses the function performs even though it cannot change any result.
//   * `push ecx` is NOT an argument; it reserves the 4 bytes that `fstp [esp]`
//     then overwrites with the float. Reading it as a third argument would be
//     wrong.
//
// Sole caller FUN_0043dfd0.
// ---------------------------------------------------------------------------

// 0x00495080
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl
Fwd494fd0_DoublePlusHalf(float /*a1*/, std::uint32_t /*a2*/)
{
    // The original reaches its callee with a `call rel32`, which inline asm cannot
    // express against an absolute address. House pattern is load-then-call-register.
    // EDX is used deliberately: this function never assigns EDX, so at the call site
    // the original holds nothing but caller garbage there — clobbering it destroys no
    // defined value, whereas EAX still holds a2 at that point in the original.
    // Interceptor.replace at 0x004fd0's entry diverts an indirect call just as it
    // does a direct one, so the A/B stub still catches it.
    __asm {
        fld     dword ptr [esp + 4]
        mov     eax, dword ptr [esp + 8]
        fadd    st(0), st(0)
        push    esi
        mov     esi, dword ptr [771A0Ch]
        push    eax
        // MSVC's inline assembler rejects an x87 memory operand given as a bare
        // absolute (`fadd dword ptr [5CC32Ch]` -> C2415), though it accepts the
        // same form for `mov`. Addressing it through a register is the way in.
        // ECX is the right choice and costs nothing observable: the original's
        // `push ecx` is only reserving 4 bytes, and `fstp [esp]` overwrites that
        // slot with the float before anything can read it. So the pushed
        // placeholder holds 0x5CC32C here instead of the caller's ECX, and is
        // dead in both cases. EAX is left alone deliberately - it still carries
        // a2 into the call, exactly as the original does.
        mov     ecx, 5CC32Ch
        fadd    dword ptr [ecx]
        push    ecx
        fstp    dword ptr [esp]
        mov     edx, 494FD0h
        call    edx
        add     esp, 8
        pop     esi
        ret
    }
}

RH_ScopedInstall(Fwd494fd0_DoublePlusHalf, 0x00495080);
