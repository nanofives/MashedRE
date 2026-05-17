// Mashed RE - RWS chunk walker implementation.
// See RwsChunkWalker.h for format documentation.

#include "RwsChunkWalker.h"

namespace mashed_re {
namespace Rws {

namespace {

inline std::uint32_t read_u32_le(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0])        |
           static_cast<std::uint32_t>(p[1]) << 8   |
           static_cast<std::uint32_t>(p[2]) << 16  |
           static_cast<std::uint32_t>(p[3]) << 24;
}

// Heuristic: validate that `data[0:size]` parses as a clean sequence of RW
// chunk headers with no gaps, no overruns, and at least one sub-chunk. Mirrors
// re/tools/rws_inspect.py's `_validate_as_container`. Used for unknown IDs to
// discover containers without guessing -- Mashed's TXD uses chunk id 0x23 at
// the root which isn't in the standard babinary.c table but does contain
// nested RW chunks.
bool ValidateAsContainer(const std::uint8_t* data, std::size_t size) {
    if (size < kHeaderSize) return false;
    std::size_t off = 0;
    std::uint32_t count = 0;
    while (off + kHeaderSize <= size) {
        std::uint32_t sec_id = read_u32_le(data + off);
        std::uint32_t sub_sz = read_u32_le(data + off + 4);
        if (sec_id == 0) return false;
        std::uint64_t payload_end = static_cast<std::uint64_t>(off) + kHeaderSize + sub_sz;
        if (payload_end > size) return false;
        off = static_cast<std::size_t>(payload_end);
        ++count;
    }
    return off == size && count > 0;
}

// Recursive worker. Returns visited count, or -1 on parse error.
// On callback returning false (abort), returns the count seen so far.
std::int32_t WalkRange(const std::uint8_t* base, std::size_t base_offset,
                       const std::uint8_t* data, std::size_t size,
                       std::uint32_t depth,
                       ChunkCallback cb, void* user, bool* aborted) {
    std::int32_t count = 0;
    std::size_t off = 0;
    while (off + kHeaderSize <= size) {
        ChunkInfo info;
        info.section_id       = read_u32_le(data + off);
        info.size             = read_u32_le(data + off + 4);
        info.version          = read_u32_le(data + off + 8);
        info.depth            = depth;
        info.offset_in_buffer = base_offset + off;

        // Zero section id is bogus for a real RWS stream.
        if (info.section_id == 0) return -1;
        // Overrun check.
        if (off + kHeaderSize + info.size > size) return -1;

        if (!cb(info, user)) { *aborted = true; return count + 1; }
        ++count;

        // Recurse if the ID is in our known-container list, OR if it's an
        // unknown ID whose payload validates as a clean chunk sequence. Don't
        // recurse into known leaves (struct/string/matrix/unicodestring).
        bool recurse = false;
        if (IsKnownContainer(info.section_id) && info.size >= kHeaderSize) {
            recurse = true;
        } else if (!IdName(info.section_id) && info.size >= kHeaderSize) {
            // Unknown ID -- try heuristic validation.
            recurse = ValidateAsContainer(data + off + kHeaderSize, info.size);
        }

        if (recurse) {
            std::int32_t sub = WalkRange(base,
                                         base_offset + off + kHeaderSize,
                                         data + off + kHeaderSize,
                                         info.size,
                                         depth + 1,
                                         cb, user, aborted);
            if (sub < 0) return -1;
            count += sub;
            if (*aborted) return count;
        }

        off += kHeaderSize + info.size;
    }
    // Trailing garbage that isn't a full header is a parse error only if any
    // bytes remain (some encoders pad; we accept whole-only consumption).
    if (off != size) return -1;
    return count;
}

}  // namespace

std::int32_t Walk(const std::uint8_t* data, std::size_t size,
                  ChunkCallback cb, void* user) {
    if (!data || !cb) return -1;
    bool aborted = false;
    return WalkRange(data, 0, data, size, 0, cb, user, &aborted);
}

const char* IdName(std::uint32_t section_id) {
    switch (section_id) {
    case kIdStruct:           return "STRUCT";
    case kIdString:           return "STRING";
    case kIdExtension:        return "EXTENSION";
    case kIdCamera:           return "CAMERA";
    case kIdTexture:          return "TEXTURE";
    case kIdMaterial:         return "MATERIAL";
    case kIdMatList:          return "MATLIST";
    case kIdAtomicSect:       return "ATOMICSECT";
    case kIdPlaneSect:        return "PLANESECT";
    case kIdWorld:            return "WORLD";
    case kIdMatrix:           return "MATRIX";
    case kIdFrameList:        return "FRAMELIST";
    case kIdGeometry:         return "GEOMETRY";
    case kIdClump:            return "CLUMP";
    case kIdLight:            return "LIGHT";
    case kIdUnicodeString:    return "UNICODESTRING";
    case kIdAtomic:           return "ATOMIC";
    case kIdTextureNative:    return "TEXTURE_NATIVE";
    case kIdTexDictionary:    return "TEX_DICTIONARY";
    case kIdGeometryList:     return "GEOMETRY_LIST";
    default:                  return nullptr;
    }
}

bool IsKnownContainer(std::uint32_t section_id) {
    switch (section_id) {
    case kIdExtension:
    case kIdCamera:
    case kIdTexture:
    case kIdMaterial:
    case kIdMatList:
    case kIdAtomicSect:
    case kIdPlaneSect:
    case kIdWorld:
    case kIdFrameList:
    case kIdGeometry:
    case kIdClump:
    case kIdLight:
    case kIdAtomic:
    case kIdTextureNative:
    case kIdTexDictionary:
    case kIdGeometryList:
        return true;
    default:
        return false;
    }
}

}  // namespace Rws
}  // namespace mashed_re
