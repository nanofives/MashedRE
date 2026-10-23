// Mashed RE — Render/RwStreamCore.h
// The one body of RwStreamRead (0x004cbd30), RwStreamSkip (0x004cc050) and RwStreamWrite (0x004cbe80), shared by both
// targets.
// Stream words (RwStream): [0]=type, [3..7]=union. Memory (type 3): [3]=pos, [4]=size, [5]=base.
// File (types 1, 2): [3]=file handle. Callback (type 4): [4]=read fn, [5]=write fn, [6]=skip fn, [7]=user data.
#pragma once
#include <cstdint>

namespace mashed_re {
namespace Render {

std::uint32_t  RwStreamCoreRead(std::uint32_t* stream, void* dst, std::uint32_t size);
std::uint32_t* RwStreamCoreSkip(std::uint32_t* stream, std::uint32_t n);
std::uint32_t* RwStreamCoreWrite(std::uint32_t* stream, const void* src, std::uint32_t size);

} // namespace Render
} // namespace mashed_re
