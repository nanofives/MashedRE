// Mashed RE - .piz archive reader (standalone exe-side).
// Milestone B1: pure C++ runtime port of re/tools/piz_extract.py.
//
// Format (per re/tools/piz_extract.py docstring, cross-verified against
// re/prior_art/MashedFileExtractor/FileFormats/PIZFile.cs):
//
//   Header (2048 bytes):
//     +0x00  4B   magic     "PIZ\0"
//     +0x04  4B   version   uint32  (3 in all known files)
//     +0x08  4B   count     uint32
//     +0x0C  -    pad       null bytes through 0x800
//
//   Per-entry (128 bytes), `count` of them, starting at 0x800:
//     +0x00  116B  name      null-padded ASCII (may contain backslashes)
//     +0x74  4B    offset    uint32   raw bytes OR sector index
//     +0x78  4B    length    uint32   raw bytes
//     +0x7C  4B    id        uint32   purpose unconfirmed
//
//   File data: each blob padded to 2048-byte boundary.
//
//   Two offset modes: if offset+length <= file size, treat as raw bytes;
//   else multiply offset by 2048 (sector index). First successful
//   interpretation is assumed for the rest of the archive.

#pragma once

#include <cstdint>
#include <cstddef>

namespace mashed_re {
namespace Piz {

constexpr std::size_t   kHeaderSize  = 2048;
constexpr std::size_t   kEntrySize   = 128;
constexpr std::size_t   kSectorSize  = 2048;
constexpr std::size_t   kNameSize    = 116;
constexpr std::uint32_t kMagic       = 0x005A4950u; // "PIZ\0" little-endian

enum class OffsetMode : std::uint8_t {
    Raw    = 0, // offset is a raw byte offset
    Sector = 1, // offset is in 2048-byte sectors
};

struct Entry {
    char           name[kNameSize + 1];  // +1 for guaranteed null terminator
    std::uint32_t  offset_raw;            // value as stored (may need *kSectorSize)
    std::uint32_t  offset_bytes;          // resolved byte offset (already *kSectorSize if Sector mode)
    std::uint32_t  length;                // raw bytes
    std::uint32_t  id;                    // purpose unconfirmed
};

// In-memory archive. Owns the file bytes for the lifetime of the object.
// Use Load() to populate from disk. Lookup is linear-scan (Mashed archives
// are small enough — typical Frontend.piz has <100 entries).
class Archive {
public:
    Archive()  = default;
    ~Archive();

    Archive(const Archive&)            = delete;
    Archive& operator=(const Archive&) = delete;
    Archive(Archive&&)                 = default;
    Archive& operator=(Archive&&)      = default;

    // Load a .piz file from disk. Returns true on success. On failure the
    // archive is left empty and last_error() returns a static string.
    bool Load(const char* path);

    // True iff Load() succeeded and the archive has at least one entry.
    bool valid() const { return data_ != nullptr && entries_ != nullptr; }

    std::uint32_t version() const   { return version_; }
    std::uint32_t count() const     { return count_; }
    OffsetMode    mode() const      { return mode_; }
    std::size_t   data_size() const { return data_size_; }
    const Entry&  entry(std::uint32_t i) const { return entries_[i]; }
    const char*   last_error() const { return last_error_; }

    // Returns a pointer into the owned file bytes for `entry(i)`'s blob,
    // or nullptr if the blob would extend past EOF. Does NOT copy.
    const std::uint8_t* blob(std::uint32_t i, std::uint32_t* out_length = nullptr) const;

private:
    std::uint8_t*  data_       = nullptr;
    std::size_t    data_size_  = 0;
    Entry*         entries_    = nullptr;
    std::uint32_t  count_      = 0;
    std::uint32_t  version_    = 0;
    OffsetMode     mode_       = OffsetMode::Raw;
    const char*    last_error_ = "uninitialised";
};

}  // namespace Piz
}  // namespace mashed_re
