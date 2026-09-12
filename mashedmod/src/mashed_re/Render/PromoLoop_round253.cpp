// Mashed RE — promote-round round 253 (RW plugin-registry + D3D9 leaves).
//
// Functions in this file:
//   0x004b4000  Fwd4b3fc0_Arg2Zero      — 2-arg forwarder, binds a literal 0.
//   0x004d8550  RwPipeModuleDtor        — plugin close callback, id 0x409.
//   0x004d8090  RwPluginListCopyDispatch    — walks a plugin list, calls each node's
//                                         callback with (a, b, node[0], node[1]).
//   0x004f10e0  StrideSelectByTagAndBit — branch-selected stride formula.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Fwd4b3fc0_Arg2Zero  --  0x004b4000
//
// Complete disassembly (0x004b4000..0x004b400f):
//   mov eax,[esp+4] / push 0 / push eax / call 0x4b3fc0 / add esp,8 / ret
//
// So: FUN_004b3fc0(param_1, 0), __cdecl, with 0 a HARD-CODED literal.
// Nothing touches EAX between the call and the ret, so the callee's return
// falls through — the same passthrough Ghidra mistyped on 0x004c2d90 in r249.
// Sole caller FUN_00479330.
// ---------------------------------------------------------------------------

typedef int(__cdecl* Fwd4b3fc0_t)(std::uint32_t, std::uint32_t);

// 0x004b4000
extern "C" __declspec(dllexport) int __cdecl Fwd4b3fc0_Arg2Zero(std::uint32_t arg)
{
    // FUN_004b3fc0(param_1, 0); EAX falls through. [0x004b4004..0x004b400f]
    return reinterpret_cast<Fwd4b3fc0_t>(0x004b3fc0u)(arg, 0u);
}

RH_ScopedInstall(Fwd4b3fc0_Arg2Zero, 0x004b4000);

// ---------------------------------------------------------------------------
// RwPipeModuleDtor  --  0x004d8550
//
// Complete disassembly (0x004d8550..0x004d8559):
//   call 0x4d3d50 / mov eax,[esp+4] / ret
//
// NOTE THE ARGUMENT COUNT: the call pushes NOTHING, and ESP is not adjusted
// after it, so `[esp+4]` still addresses this function's own param_1. The
// callee takes no arguments from this site. Returns param_1 verbatim.
//
// Sibling of 0x004d8470 (RwErrorModuleDtor, promoted r251): same plugin-callback
// shape, id 0x409 rather than 0x40f. Registered at 0x004c34a7 — a byte search of
// the anchored image for the dword `50 85 4d 00` returns exactly that one hit.
// U-5384, U-5385 are open against it; both have Blocks = nothing.
// ---------------------------------------------------------------------------

// 0x004d8550
extern "C" __declspec(dllexport) int __cdecl RwPipeModuleDtor(int object)
{
    reinterpret_cast<void(__cdecl*)()>(0x004d3d50u)();  // [0x004d8550]
    return object;                                       // [0x004d8555]
}

RH_ScopedInstall(RwPipeModuleDtor, 0x004d8550);

// ---------------------------------------------------------------------------
// RwPluginListCopyDispatch  --  0x004d8090
//
// Decompilation (complete):
//   int FUN_004d8090(int param_1,undefined4 param_2,undefined4 param_3)
//   {
//     undefined4 *puVar1;
//     for (puVar1 = *(undefined4 **)(param_1 + 0x10); puVar1 != (undefined4 *)0x0;
//         puVar1 = (undefined4 *)puVar1[0xc]) {
//       (*(code *)puVar1[10])(param_2,param_3,*puVar1,puVar1[1]);
//     }
//     return param_1;
//   }
//
// Walks the intrusive list whose head is at param_1+0x10, advancing through
// node[0xc] (byte +0x30), and invokes the function pointer at node[10]
// (byte +0x28) with FOUR arguments in this exact order:
//   (param_2, param_3, node[0], node[1])
// Returns param_1 unchanged. 4 callers.
//
// ARGUMENT ORDER AND NODE-FIELD ORDER ARE THE WHOLE CONTENT of this function,
// which is why its A/B stubs the callback and compares the recorded call
// SEQUENCE across a multi-node list rather than a single node — a port that
// walked the list backwards, or swapped node[0] with node[1], produces the same
// call COUNT and would pass a count-only check.
// ---------------------------------------------------------------------------

// 0x004d8090
extern "C" __declspec(dllexport) int __cdecl RwPluginListCopyDispatch(
    int registry, std::uint32_t a, std::uint32_t b)
{
    for (std::uint32_t* node = *reinterpret_cast<std::uint32_t**>(registry + 0x10);
         node != nullptr;
         node = reinterpret_cast<std::uint32_t*>(node[0xc])) {
        reinterpret_cast<void(__cdecl*)(std::uint32_t, std::uint32_t,
                                        std::uint32_t, std::uint32_t)>(node[10])(
            a, b, node[0], node[1]);
    }
    return registry;
}

RH_ScopedInstall(RwPluginListCopyDispatch, 0x004d8090);

// ---------------------------------------------------------------------------
// StrideSelectByTagAndBit  --  0x004f10e0
//
// Complete disassembly, 0x004f10e0..0x004f112c. Pure: two pointer arguments,
// no globals, no callees, two return paths.
//
//   eax = param_2                                        [0x004f10e0]
//   edx = 0x1000000                                      [0x004f10e4]
//   cl  = *param_2                        (single byte)  [0x004f10e9]
//   if (cl != 8) goto CHECK7                             [0x004f10eb/ee]
//   if (*(uint*)(param_2+8) & 0x1000000) goto SHORT      [0x004f10f1..f7]
// CHECK7:
//   if (cl != 7) goto LONG                               [0x004f10f9/fc]
//   if ((*(uint*)(param_2+8) & 0x1000000) == 0) goto LONG[0x004f10fe/1101]
// SHORT:                                                 [0x004f1103]
//   return (uint16)*(ushort*)(param_1+4) * 8 + 0xc;      [0x004f1109/110d]
// LONG:                                                  [0x004f1115]
//   return (*(int*)(param_1+8)
//           + (uint16)*(ushort*)(param_1+4) * 2) * 4 + 0xc;  [0x004f111b..1125]
//
// So SHORT is taken when the tag byte is 8 or 7 AND bit 0x1000000 is set at
// param_2+8; every other combination takes LONG. The `(uint16)` widths are
// literal: both reads are `mov ax/dx, word ptr [reg+4]` into a zeroed register,
// so the field is 16-bit UNSIGNED and must not be sign-extended.
//
// Constants: 0x1000000 (the tested bit), tags 8 and 7, strides 8 and 2,
// multiplier 4 and offset 0xc — all cited above.
// ---------------------------------------------------------------------------

// 0x004f10e0
extern "C" __declspec(dllexport) std::uint32_t __cdecl StrideSelectByTagAndBit(
    std::int32_t decl, const std::uint8_t* elem)
{
    const std::uint8_t tag = elem[0];
    const std::uint32_t flags = *reinterpret_cast<const std::uint32_t*>(elem + 8);

    if (((tag == 8) && (flags & 0x1000000u)) ||
        ((tag == 7) && (flags & 0x1000000u))) {
        // SHORT path. [0x004f1103]
        return static_cast<std::uint32_t>(
                   *reinterpret_cast<const std::uint16_t*>(decl + 4)) * 8u + 0xcu;
    }

    // LONG path. [0x004f1115]
    return (static_cast<std::uint32_t>(*reinterpret_cast<const std::int32_t*>(decl + 8)) +
            static_cast<std::uint32_t>(
                *reinterpret_cast<const std::uint16_t*>(decl + 4)) * 2u) * 4u + 0xcu;
}

RH_ScopedInstall(StrideSelectByTagAndBit, 0x004f10e0);
