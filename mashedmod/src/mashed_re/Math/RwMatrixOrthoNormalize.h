// Mashed RE - 0x004c4680, the RwMatrix re-orthonormalize, one body (D4-DC-MATH-4680). See RwMatrixOrthoNormalize.cpp.
#pragma once

namespace mashed_re {
namespace Math {

// dst may equal src: every source row is read before any is written. Translation copied, flags (dst & ~0x20000) | 3.
void RwMatrixOrthoNormalizeLut(float* dst, const float* src);

}  // namespace Math
}  // namespace mashed_re
