// Mashed RE - TXD on-disk decoder implementation.
// See TxdDecoder.h for format documentation.

#include "TxdDecoder.h"

#include <cstring>

namespace mashed_re {
namespace Txd {

namespace {

inline std::uint32_t ReadU32LE(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0])        |
           static_cast<std::uint32_t>(p[1]) << 8   |
           static_cast<std::uint32_t>(p[2]) << 16  |
           static_cast<std::uint32_t>(p[3]) << 24;
}

// Copy up to `dst_max - 1` bytes from src, then null-terminate. Src does not
// need to be null-terminated; we stop at the first 0 or at src_len.
void CopyName(char* dst, std::size_t dst_max,
              const std::uint8_t* src, std::size_t src_len) {
    std::size_t n = 0;
    while (n < src_len && n + 1 < dst_max && src[n] != 0) {
        dst[n] = static_cast<char>(src[n]);
        ++n;
    }
    dst[n] = '\0';
}

}  // namespace

bool Dictionary::Decode(const std::uint8_t* data, std::size_t size) {
    count_     = 0;
    device_id_ = 0;
    last_error_ = nullptr;

    if (!data || size < kRwHeaderSize + 4) {
        last_error_ = "input too small";
        return false;
    }

    // Root chunk header.
    std::uint32_t root_id   = ReadU32LE(data + 0);
    std::uint32_t root_size = ReadU32LE(data + 4);
    std::uint32_t root_ver  = ReadU32LE(data + 8);
    if (root_id != kRootChunkId) {
        last_error_ = "root chunk id is not 0x23";
        return false;
    }
    if (root_ver != kExpectedVersion) {
        last_error_ = "root chunk version is not 0x1803FFFF";
        return false;
    }
    if (kRwHeaderSize + root_size > size) {
        last_error_ = "root chunk size overruns buffer";
        return false;
    }

    // numTex / deviceId (4 bytes at +0x0C).
    std::uint32_t nt_did = ReadU32LE(data + kRwHeaderSize);
    std::uint32_t num_tex = nt_did & 0xFFFFu;
    device_id_           = (nt_did >> 16) & 0xFFFFu;

    if (num_tex == 0) {
        last_error_ = "numTex is zero";
        return false;
    }
    if (num_tex > kMaxTextures) {
        last_error_ = "numTex exceeds kMaxTextures";
        return false;
    }
    if (device_id_ == 0) {
        // SIMPLE path. Not implemented in B3 because Frontend.piz doesn't use
        // it; refuse with a clear error so the caller knows what's missing.
        last_error_ = "deviceId=0 (SIMPLE path) not implemented in B3";
        return false;
    }

    // Walk all textures back-to-back starting at +0x10.
    std::size_t off = kRwHeaderSize + 4;
    const std::size_t end = kRwHeaderSize + root_size;

    for (std::uint32_t ti = 0; ti < num_tex; ++ti) {
        if (off + 4 > end) { last_error_ = "truncated before numMips"; return false; }

        std::uint32_t num_mips = ReadU32LE(data + off);
        off += 4;

        if (num_mips == 0 || num_mips > kMaxMipsPerTexture) {
            last_error_ = "numMips out of range";
            return false;
        }

        Texture& t = textures_[ti];
        t.name[0]      = '\0';
        t.mask_name[0] = '\0';
        t.mip_count    = num_mips;
        t.filter_addressing = 0;

        for (std::uint32_t mi = 0; mi < num_mips; ++mi) {
            // IMAGE chunk header.
            if (off + kRwHeaderSize > end) { last_error_ = "truncated IMAGE header"; return false; }
            std::uint32_t img_id   = ReadU32LE(data + off);
            std::uint32_t img_size = ReadU32LE(data + off + 4);
            std::uint32_t img_ver  = ReadU32LE(data + off + 8);
            if (img_id != kImageChunkId) {
                last_error_ = "expected IMAGE chunk (0x18)";
                return false;
            }
            if (img_ver != kExpectedVersion) {
                last_error_ = "IMAGE chunk version mismatch";
                return false;
            }
            if (off + kRwHeaderSize + img_size > end) {
                last_error_ = "IMAGE chunk overruns buffer";
                return false;
            }
            std::size_t img_payload_off = off + kRwHeaderSize;
            std::size_t img_payload_end = img_payload_off + img_size;

            // STRUCT subchunk header.
            if (img_payload_off + kRwHeaderSize > img_payload_end) {
                last_error_ = "IMAGE payload too small for STRUCT header";
                return false;
            }
            std::uint32_t s_id   = ReadU32LE(data + img_payload_off);
            std::uint32_t s_size = ReadU32LE(data + img_payload_off + 4);
            if (s_id != kStructChunkId || s_size != kStructPayloadSize) {
                last_error_ = "expected 16-byte STRUCT inside IMAGE";
                return false;
            }
            std::size_t s_payload_off = img_payload_off + kRwHeaderSize;
            if (s_payload_off + kStructPayloadSize > img_payload_end) {
                last_error_ = "STRUCT payload exceeds IMAGE payload";
                return false;
            }

            std::uint32_t w      = ReadU32LE(data + s_payload_off + 0);
            std::uint32_t h      = ReadU32LE(data + s_payload_off + 4);
            std::uint32_t depth  = ReadU32LE(data + s_payload_off + 8);
            std::uint32_t stride = ReadU32LE(data + s_payload_off + 12);

            // Pixel data + palette occupies whatever remains in the IMAGE
            // chunk payload after the STRUCT subchunk.
            std::size_t pixel_data_off = s_payload_off + kStructPayloadSize;
            std::size_t pixel_data_end = img_payload_end;

            std::uint64_t expected_pixels  = static_cast<std::uint64_t>(stride) * h;
            std::uint64_t expected_palette = (depth < 9)
                ? (static_cast<std::uint64_t>(1u) << depth) * 4u
                : 0u;
            std::uint64_t expected_total = expected_pixels + expected_palette;

            if (pixel_data_off + expected_total > pixel_data_end) {
                last_error_ = "pixel+palette bytes exceed IMAGE payload";
                return false;
            }

            Mip& m = t.mips[mi];
            m.width        = w;
            m.height       = h;
            m.depth        = depth;
            m.stride       = stride;
            m.pixels       = data + pixel_data_off;
            m.pixel_bytes  = static_cast<std::uint32_t>(expected_pixels);
            if (expected_palette > 0) {
                m.palette       = data + pixel_data_off + expected_pixels;
                m.palette_bytes = static_cast<std::uint32_t>(expected_palette);
            } else {
                m.palette       = nullptr;
                m.palette_bytes = 0;
            }

            off = img_payload_end;
        }

        // TEXTURE chunk (id=6) with metadata + name.
        if (off + kRwHeaderSize > end) { last_error_ = "truncated TEXTURE header"; return false; }
        std::uint32_t tex_id   = ReadU32LE(data + off);
        std::uint32_t tex_size = ReadU32LE(data + off + 4);
        std::uint32_t tex_ver  = ReadU32LE(data + off + 8);
        if (tex_id != kTextureChunkId) {
            last_error_ = "expected TEXTURE chunk (id=6)";
            return false;
        }
        if (tex_ver != kExpectedVersion) {
            last_error_ = "TEXTURE chunk version mismatch";
            return false;
        }
        if (off + kRwHeaderSize + tex_size > end) {
            last_error_ = "TEXTURE chunk overruns buffer";
            return false;
        }
        std::size_t tex_payload_off = off + kRwHeaderSize;
        std::size_t tex_payload_end = tex_payload_off + tex_size;

        // First sub-chunk: STRUCT (4 bytes filterAddressing).
        if (tex_payload_off + kRwHeaderSize > tex_payload_end) {
            last_error_ = "TEXTURE payload too small for STRUCT";
            return false;
        }
        std::uint32_t st_id   = ReadU32LE(data + tex_payload_off);
        std::uint32_t st_size = ReadU32LE(data + tex_payload_off + 4);
        if (st_id != kStructChunkId || st_size != 4) {
            last_error_ = "expected 4-byte STRUCT inside TEXTURE";
            return false;
        }
        std::size_t st_payload_off = tex_payload_off + kRwHeaderSize;
        if (st_payload_off + 4 > tex_payload_end) {
            last_error_ = "filterAddressing exceeds TEXTURE payload";
            return false;
        }
        t.filter_addressing = ReadU32LE(data + st_payload_off);

        // Next: STRING (name).
        std::size_t name_chunk_off = st_payload_off + 4;
        if (name_chunk_off + kRwHeaderSize > tex_payload_end) {
            last_error_ = "TEXTURE payload too small for name STRING";
            return false;
        }
        std::uint32_t n_id   = ReadU32LE(data + name_chunk_off);
        std::uint32_t n_size = ReadU32LE(data + name_chunk_off + 4);
        if (n_id != kStringChunkId) {
            last_error_ = "expected STRING chunk for name";
            return false;
        }
        std::size_t n_payload_off = name_chunk_off + kRwHeaderSize;
        if (n_payload_off + n_size > tex_payload_end) {
            last_error_ = "name STRING overruns TEXTURE payload";
            return false;
        }
        CopyName(t.name, sizeof(t.name), data + n_payload_off, n_size);

        // Next: STRING (mask). Mostly empty (size 4 with single null).
        std::size_t mask_chunk_off = n_payload_off + n_size;
        if (mask_chunk_off + kRwHeaderSize <= tex_payload_end) {
            std::uint32_t m_id   = ReadU32LE(data + mask_chunk_off);
            std::uint32_t m_size = ReadU32LE(data + mask_chunk_off + 4);
            if (m_id == kStringChunkId &&
                mask_chunk_off + kRwHeaderSize + m_size <= tex_payload_end) {
                CopyName(t.mask_name, sizeof(t.mask_name),
                         data + mask_chunk_off + kRwHeaderSize, m_size);
            }
        }

        // We intentionally skip EXTENSION (id=3) — Mashed emits an empty
        // extension chunk (size=0) and we have no plugin data to consume.

        off = tex_payload_end;
    }

    count_ = num_tex;
    if (!last_error_) last_error_ = "ok";
    return true;
}

}  // namespace Txd
}  // namespace mashed_re
