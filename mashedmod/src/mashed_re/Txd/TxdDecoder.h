// Mashed RE - TXD (Texture Dictionary) on-disk decoder.
// Milestone B3: parses Mashed's proprietary chunk-id 0x23 TXD format that
// wraps standard RW3 IMAGE/TEXTURE chunks. Zero-copy: the decoded `Texture`
// records point back into the caller-owned bytes.
//
// Format (per re/analysis/txd_on_disk_format/README.md):
//
//   ROOT (12-byte RW chunk header): id=0x23, size, version=0x1803FFFF
//   +0x0C  4B  numTex (uint16) | deviceId (uint16) packed
//
//   For each of `numTex` textures, back-to-back:
//     4B   numMips (uint32)
//     For each mip:
//       12B IMAGE chunk header (id=0x18, size, version=0x1803FFFF)
//       12B STRUCT chunk header (id=1, size=0x10, version)
//       16B STRUCT payload: width, height, depth, stride (uint32 x 4)
//       stride*height bytes  pixel data
//       (1<<depth)*4 bytes   palette (only if depth < 9)
//     12B TEXTURE chunk header (id=6, size, version=0x1803FFFF)
//        STRUCT subchunk (filterAddressing, 4 bytes)
//        STRING subchunk (texture name, ASCII null-padded)
//        STRING subchunk (mask name, usually empty)
//        EXTENSION subchunk (usually empty)
//
// Source-of-truth: FUN_0054f8d0 in MASHED.exe (deviceId != 0 branch).
// The SIMPLE deviceId==0 path is not implemented in B3 (not needed for the
// Frontend.piz TXD; Frontend uses deviceId=1).

#pragma once

#include <cstdint>
#include <cstddef>

namespace mashed_re {
namespace Txd {

constexpr std::uint32_t kRootChunkId       = 0x00000023;
constexpr std::uint32_t kImageChunkId      = 0x00000018;  // rwID_IMAGE
constexpr std::uint32_t kTextureChunkId    = 0x00000006;  // rwID_TEXTURE
constexpr std::uint32_t kStructChunkId     = 0x00000001;  // rwID_STRUCT
constexpr std::uint32_t kStringChunkId     = 0x00000002;  // rwID_STRING
constexpr std::uint32_t kExtensionChunkId  = 0x00000003;  // rwID_EXTENSION
constexpr std::uint32_t kExpectedVersion   = 0x1803FFFF;  // RW 3.6.0.3 packed
constexpr std::size_t   kRwHeaderSize      = 12;
constexpr std::uint32_t kStructPayloadSize = 16;          // w,h,depth,stride
constexpr std::uint32_t kMaxNameLen        = 32;
constexpr std::uint32_t kMaxMipsPerTexture = 16;          // sanity limit

// Pixel format inferred from `depth`. Mashed only emits depth values 8 and 32
// in the Frontend.piz TXD; other values are reported as `Unknown` so the
// caller can decide how to handle them in later milestones.
enum class PixelFormat : std::uint8_t {
    Unknown    = 0,
    Paletted8  = 1,  // 8 bpp, palette of 256 RGBA8 entries follows pixels
    ARGB8888   = 2,  // 32 bpp, no palette
};

inline PixelFormat PixelFormatFromDepth(std::uint32_t depth) {
    switch (depth) {
    case 8:  return PixelFormat::Paletted8;
    case 32: return PixelFormat::ARGB8888;
    default: return PixelFormat::Unknown;
    }
}

inline const char* PixelFormatName(PixelFormat f) {
    switch (f) {
    case PixelFormat::Paletted8: return "PAL8";
    case PixelFormat::ARGB8888:  return "ARGB";
    default:                     return "?";
    }
}

// One decoded mip level.
struct Mip {
    std::uint32_t       width;
    std::uint32_t       height;
    std::uint32_t       depth;        // bits per pixel
    std::uint32_t       stride;       // row stride in bytes (already aligned)
    const std::uint8_t* pixels;       // points into decoder-input bytes
    std::uint32_t       pixel_bytes;  // = stride * height
    const std::uint8_t* palette;      // nullptr if depth >= 9
    std::uint32_t       palette_bytes;
};

// One decoded texture.
struct Texture {
    char           name[kMaxNameLen + 1];  // null-terminated
    char           mask_name[kMaxNameLen + 1];
    std::uint32_t  filter_addressing;       // packed RW filter+addrU+addrV
    std::uint32_t  mip_count;
    Mip            mips[kMaxMipsPerTexture];

    // Convenience: base mip metadata.
    std::uint32_t  width() const   { return mip_count ? mips[0].width  : 0; }
    std::uint32_t  height() const  { return mip_count ? mips[0].height : 0; }
    std::uint32_t  depth() const   { return mip_count ? mips[0].depth  : 0; }
    PixelFormat    format() const  { return PixelFormatFromDepth(depth()); }
};

// Reads-only-once-at-Decode dictionary. Memory is owned by the caller — the
// caller MUST keep the input byte buffer alive for as long as `Texture::Mip`
// pointers are used.
class Dictionary {
public:
    static constexpr std::uint32_t kMaxTextures = 256;

    Dictionary() = default;

    // Decode the in-memory TXD bytes. Returns true on success; on failure the
    // dictionary is left empty and last_error() returns a static string.
    bool Decode(const std::uint8_t* data, std::size_t size);

    bool valid() const { return count_ > 0; }

    std::uint32_t   count() const     { return count_; }
    std::uint32_t   device_id() const { return device_id_; }
    const Texture&  texture(std::uint32_t i) const { return textures_[i]; }
    const char*     last_error() const { return last_error_; }

private:
    Texture        textures_[kMaxTextures];
    std::uint32_t  count_      = 0;
    std::uint32_t  device_id_  = 0;
    const char*    last_error_ = "uninitialised";
};

}  // namespace Txd
}  // namespace mashed_re
