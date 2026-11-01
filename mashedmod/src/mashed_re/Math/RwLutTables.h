// Mashed RE — the RenderWare fast-sqrt LUTs, rebuilt in-process (no RwEngineOpen needed).
// See RwLutTables.cpp. Used by D-11072 P1 (Race/PathNodes.cpp); the global FastSqrt /
// RwV3dNormalize paths are NOT switched to these (separate decision, they feed D2 physics).
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Math {

const std::uint32_t* RwLutSqrtTable();      // 0x1000 dwords, == the original's (root delta 0)
const std::uint32_t* RwLutInvSqrtTable();   // 0x1000 dwords, == the original's (root delta 4)

float LutSqrt(float x);                     // FUN_004c3b30 with the real LUT
float LutInvSqrt(float x);                  // FUN_004c3b90 with the real LUT (pontoon 2026-10-16)
float LutNormalize(float* out, const float* in);   // FUN_004c39b0 with the real LUT (no error report)
float LutLength(const float* v);                   // FUN_004c3ac0 (RwV3dLength) with the real LUT, PC_24
float LutNormalize2D(float* out, const float* in);  // FUN_004c3c60 (2-D normalize) with the real LUT, PC_24

}  // namespace Math
}  // namespace mashed_re

// cdecl entry points for the verbatim naked transcriptions (D4-DC-INC-batch3, 2026-11-01). The naked specs
// (re/tools/naked_specs/*.json "calls") map 0x004c3b30 / 0x004c3b90 / 0x004c3ac0 / 0x004c39b0 here, so every lane
// runs these helpers instead of a private naked copy. Arg on the stack, result in ST0, as the originals. The
// installed hooks Math/RwSqrt.cpp (FastSqrt, FastInvSqrt) and Math/Vec3.cpp (Vec3Magnitude) are still a second C
// form of 0x004c3b30 / 0x004c3b90 / 0x004c3ac0: in the exe they take the std::sqrt fallback, these the LUT.
// RwLutNormalize_cd is defined in Math/RwV3dNormalize.cpp, the others in Math/RwLutTables.cpp.
extern "C" float __cdecl RwLutSqrt_cd(float x);
extern "C" float __cdecl RwLutInvSqrt_cd(float x);
extern "C" float __cdecl RwLutLength_cd(const float* v);
extern "C" float __cdecl RwLutNormalize_cd(float* out, const float* in);
