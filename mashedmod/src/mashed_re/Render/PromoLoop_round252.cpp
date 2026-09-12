// Mashed RE — promote-round round 252 (four RW/D3D9-backend pure leaves).
//
// Functions in this file (all pure leaves, no callees):
//   0x004dfab0  RwRGBAToIntensityScaled — weighted RGB intensity, alpha-scaled.
//   0x004f3bd0  D3D9IndexedDwordFetch   — *(*p + i*4), a double-deref table read.
//   0x004ec720  RwFrameHeadSet          — **(p+0x14) = v.
//   0x004ec740  RwFrameField0cSet       — *(*(p+0x14) + 0xc) = v.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RwRGBAToIntensityScaled  --  0x004dfab0
//
// Decompilation (complete):
//   uint FUN_004dfab0(byte *param_1)
//   {
//     return ((((uint)param_1[1] * 0x3b + (uint)param_1[2] * 0xb
//             + (uint)*param_1 * 0x1e) / 100) * (uint)param_1[3]) / 0xff;
//   }
//
// Reads exactly four bytes through its sole pointer argument and returns a
// scalar. No globals, no callees, no branches — deterministic.
//
// THE ARITHMETIC IS ORDER-SENSITIVE AND MUST BE TRANSCRIBED LITERALLY. Both
// divisions are UNSIGNED INTEGER divisions that TRUNCATE, and they are applied
// at two separate points: the weighted sum is divided by 100 FIRST, and only
// the truncated result is multiplied by param_1[3] and divided by 0xff. Folding
// them into one expression, or reordering the multiply before the /100, changes
// the result for most inputs. The test vectors below are chosen to land on
// truncation boundaries for exactly that reason.
//
// Weights, cited from the decompilation: byte[1] x 0x3b (59), byte[2] x 0xb (11),
// byte[0] x 0x1e (30), sum /100, x byte[3], /0xff.
// (30/11/59 sum to 100. Which channel each byte index denotes is NOT claimed —
// the function is transcribed by index, not by colour name.)
// ---------------------------------------------------------------------------

// 0x004dfab0
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwRGBAToIntensityScaled(
    const std::uint8_t* p)
{
    // Literal transcription, both truncating divisions kept in place.
    const std::uint32_t weighted =
        (static_cast<std::uint32_t>(p[1]) * 0x3bu +
         static_cast<std::uint32_t>(p[2]) * 0x0bu +
         static_cast<std::uint32_t>(p[0]) * 0x1eu) / 100u;

    return (weighted * static_cast<std::uint32_t>(p[3])) / 0xffu;
}

RH_ScopedInstall(RwRGBAToIntensityScaled, 0x004dfab0);

// ---------------------------------------------------------------------------
// D3D9IndexedDwordFetch  --  0x004f3bd0
//
// Decompilation (complete):
//   undefined4 FUN_004f3bd0(int *param_1,int param_2)
//   { return *(undefined4 *)(*param_1 + param_2 * 4); }
//
// A DOUBLE dereference: param_1 points at a pointer, and the dword is read from
// that inner pointer at index param_2, stride 4. No bounds check of any kind —
// a negative or oversized index reads out of bounds, and the port must NOT add
// a guard, because that would be a behavioural divergence.
//
// 6 callers (FUN_004e8a10, FUN_004f0f10, FUN_004f1150, FUN_004f13d0,
// FUN_004f1870, ...), all inside the D3D9 backend block.
// ---------------------------------------------------------------------------

// 0x004f3bd0
extern "C" __declspec(dllexport) std::uint32_t __cdecl D3D9IndexedDwordFetch(
    std::int32_t* p, std::int32_t index)
{
    // *(*p + index*4) — no bounds check in the original, none added here.
    return *reinterpret_cast<std::uint32_t*>(*p + index * 4);
}

RH_ScopedInstall(D3D9IndexedDwordFetch, 0x004f3bd0);

// ---------------------------------------------------------------------------
// RwFrameHeadSet  --  0x004ec720
//
// Decompilation (complete):
//   void FUN_004ec720(int param_1,undefined4 param_2)
//   { **(undefined4 **)(param_1 + 0x14) = param_2; }
//
// Double-deref store: the pointer at param_1+0x14 is loaded, and param_2 is
// written to offset 0 of the object it points at. Sole caller FUN_00543710.
// ---------------------------------------------------------------------------

// 0x004ec720
extern "C" __declspec(dllexport) void __cdecl RwFrameHeadSet(
    std::int32_t obj, std::uint32_t value)
{
    // **(p+0x14) = value
    **reinterpret_cast<std::uint32_t**>(obj + 0x14) = value;
}

RH_ScopedInstall(RwFrameHeadSet, 0x004ec720);

// ---------------------------------------------------------------------------
// RwFrameField0cSet  --  0x004ec740
//
// Decompilation (complete):
//   void FUN_004ec740(int param_1,undefined4 param_2)
//   { *(undefined4 *)(*(int *)(param_1 + 0x14) + 0xc) = param_2; }
//
// Sibling of 0x004ec720 and the reason both are in this file: the two differ
// ONLY in the final offset — 0 there, 0xc here — off the SAME pointer at
// param_1+0x14. A port that copy-pasted one into the other is the realistic
// error, so the A/B for each observes its own offset AND the other, to prove
// the untouched one stays untouched.
//
// Callers: FUN_00496e40, FUN_00543710.
// ---------------------------------------------------------------------------

// 0x004ec740
extern "C" __declspec(dllexport) void __cdecl RwFrameField0cSet(
    std::int32_t obj, std::uint32_t value)
{
    // *(*(p+0x14) + 0xc) = value
    *reinterpret_cast<std::uint32_t*>(
        *reinterpret_cast<std::int32_t*>(obj + 0x14) + 0xc) = value;
}

RH_ScopedInstall(RwFrameField0cSet, 0x004ec740);
