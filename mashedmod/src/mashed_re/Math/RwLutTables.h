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
float LutNormalize(float* out, const float* in);   // FUN_004c39b0 with the real LUT (no error report)
float LutLength(const float* v);                   // FUN_004c3ac0 (RwV3dLength) with the real LUT, PC_24
float LutNormalize2D(float* out, const float* in);  // FUN_004c3c60 (2-D normalize) with the real LUT, PC_24

}  // namespace Math
}  // namespace mashed_re
