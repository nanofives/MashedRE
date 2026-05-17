// Mashed RE - RenderWare Stream chunk walker.
// Milestone B2: minimal traversal of nested RWS chunks; identifies common
// types and reports structure to a callback.
//
// Format (per re/tools/rws_inspect.py + criterion-rwg37/core/src/plcore/
// babinary.c ChunkIsComplex):
//
//   Each chunk has a 12-byte header (little-endian):
//     uint32 section_id   -- rwID_*
//     uint32 size         -- payload bytes, excludes this 12-byte header
//     uint32 version      -- libraryID
//   Followed by `size` bytes of payload. Some chunk types are "complex"
//   (contain nested chunks); others are "leaf" (raw bytes).
//
// We do NOT decode payloads. We just walk the structure, report each chunk
// with its depth, and recurse into known-complex chunks.

#pragma once

#include <cstdint>
#include <cstddef>

namespace mashed_re {
namespace Rws {

constexpr std::size_t kHeaderSize = 12;

// RW chunk IDs confirmed in babinary.c.
constexpr std::uint32_t kIdStruct           = 0x00000001;
constexpr std::uint32_t kIdString           = 0x00000002;
constexpr std::uint32_t kIdExtension        = 0x00000003;
constexpr std::uint32_t kIdCamera           = 0x00000005;
constexpr std::uint32_t kIdTexture          = 0x00000006;
constexpr std::uint32_t kIdMaterial         = 0x00000007;
constexpr std::uint32_t kIdMatList          = 0x00000008;
constexpr std::uint32_t kIdAtomicSect       = 0x00000009;
constexpr std::uint32_t kIdPlaneSect        = 0x0000000A;
constexpr std::uint32_t kIdWorld            = 0x0000000B;
constexpr std::uint32_t kIdMatrix           = 0x0000000D;
constexpr std::uint32_t kIdFrameList        = 0x0000000E;
constexpr std::uint32_t kIdGeometry         = 0x0000000F;
constexpr std::uint32_t kIdClump            = 0x00000010;
constexpr std::uint32_t kIdLight            = 0x00000012;
constexpr std::uint32_t kIdUnicodeString    = 0x00000013;
constexpr std::uint32_t kIdAtomic           = 0x00000014;
constexpr std::uint32_t kIdTextureNative    = 0x00000015;
constexpr std::uint32_t kIdTexDictionary    = 0x00000016;
constexpr std::uint32_t kIdGeometryList     = 0x0000001A;

struct ChunkInfo {
    std::uint32_t  section_id;
    std::uint32_t  size;
    std::uint32_t  version;
    std::uint32_t  depth;
    std::size_t    offset_in_buffer;  // start of this chunk's HEADER in the buffer
};

// Callback invoked for each discovered chunk, in pre-order (chunk before its
// children). Return false to abort traversal early.
using ChunkCallback = bool(*)(const ChunkInfo& info, void* user);

// Walks the RWS chunk tree rooted at `data`. Returns the number of chunks
// visited, or -1 on a parse error (overrun, zero-id chunk, etc.). The
// callback is invoked for every chunk including the root.
std::int32_t Walk(const std::uint8_t* data, std::size_t size,
                  ChunkCallback cb, void* user);

// Returns a static short name for known chunk IDs, or nullptr for unknown.
const char* IdName(std::uint32_t section_id);

// Returns true if `section_id` is a known-complex container that should be
// recursed into. Unknown IDs are treated as leaves (we do NOT do the
// validate-as-container heuristic the Python tool does).
bool IsKnownContainer(std::uint32_t section_id);

}  // namespace Rws
}  // namespace mashed_re
