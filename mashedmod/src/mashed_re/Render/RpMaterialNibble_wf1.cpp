// Mashed RE — Render/RpMaterialNibble_wf1.cpp
// Per-material reflectivity nibble reader (RW plugin field accessor).
//
// Covers:
//   0x004cff00  RpMaterialReflectivityGet  — returns (*(byte*)(slot+9+mat_ptr)) & 0xf
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis refs:
//   re/analysis/render_5_c1_to_c2_s4/FUN_004cff00.md
//   re/analysis/render_lighting_alt/render_lighting_alt-20260503.md

#include "../Core/HookSystem.h"
#include <cstdint>

// Plugin slot offset: written by FUN_004cfa00 (RW material plugin registration)
// at address 0x00911ae4 in the target image.
static const std::uint32_t* const s_plugin_slot =
    reinterpret_cast<const std::uint32_t*>(0x00911ae4u);

// ---------------------------------------------------------------------------
// RpMaterialReflectivityGet  --  0x004cff00
//
// Original: FUN_004cff00 (17 bytes, 0x004cff00..0x004cff10).
// Decompiled (verbatim from re/analysis/render_5_c1_to_c2_s4/FUN_004cff00.md):
//   byte FUN_004cff00(int param_1) {
//     return *(byte *)(DAT_00911ae4 + 9 + param_1) & 0xf;
//   }
//
// Reads the low 4 bits (nibble) of the byte at offset (DAT_00911ae4 + 9) within
// the RW material object pointer param_1. DAT_00911ae4 is the plugin slot offset
// registered by FUN_004cfa00 (RW RpMaterial plugin registration). Offset +9 is
// the reflectivity nibble field within the plugin data block.
// Non-zero return -> D3DTCI_CAMERASPACEREFLECTIONVECTOR path in LIGHT_FN.
// Zero return    -> D3DTCI_CAMERASPACENORMAL path.
//
// Calling convention: __cdecl (param in [ESP+4], callee does not clean stack;
// confirmed by 17-byte body ending in plain C3 RET with no operand).
//
// Leaf function: no callees. Leaf-function exemption (CONFIDENCE.md C2->C3):
// callee-at-C2+ half is N/A; bit-identical Frida force-call A/B satisfies it.
//
// Caller: FUN_00541b50 (LIGHT_FN, third-party-library[renderware]).
// Identified-caller clause (CONFIDENCE.md, 2026-06-25 ruling): caller has a
// documented library role ("LIGHT_FN; per-material specular env-map setup") in
// the RenderWare subsystem — this counts as an identified caller and satisfies
// the caller-half of the C2->C3 gate.
// ---------------------------------------------------------------------------

// 0x004cff00
extern "C" __declspec(dllexport) std::uint32_t __cdecl RpMaterialReflectivityGet(
    std::uint32_t param_1)
{
    // 0x004cff00: load plugin slot offset from DAT_00911ae4; add 9 + param_1;
    // read one byte and zero-extend to 32-bit (matches MOVZX EAX,BYTE in original
    // asm); mask low nibble.
    const std::uint32_t slot = *s_plugin_slot;
    const std::uint32_t byte_val =
        static_cast<std::uint32_t>(
            *reinterpret_cast<const std::uint8_t*>(slot + 9u + param_1));
    return byte_val & 0x0fu;
}

RH_ScopedInstall(RpMaterialReflectivityGet, 0x004cff00);
