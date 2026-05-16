// Mashed RE - .piz archive reader implementation.
// See PizReader.h for format documentation.

#include "PizReader.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace mashed_re {
namespace Piz {

namespace {

inline std::uint32_t read_u32_le(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0])        |
           static_cast<std::uint32_t>(p[1]) << 8   |
           static_cast<std::uint32_t>(p[2]) << 16  |
           static_cast<std::uint32_t>(p[3]) << 24;
}

}  // namespace

Archive::~Archive() {
    delete[] data_;
    delete[] entries_;
    data_    = nullptr;
    entries_ = nullptr;
}

bool Archive::Load(const char* path) {
    // Reset state.
    delete[] data_;    data_    = nullptr; data_size_ = 0;
    delete[] entries_; entries_ = nullptr; count_     = 0;
    version_    = 0;
    mode_       = OffsetMode::Raw;
    last_error_ = nullptr;

    // Open + size + slurp via stdio. Binary mode is critical on Windows.
    std::FILE* fp = std::fopen(path, "rb");
    if (!fp) { last_error_ = "fopen failed"; return false; }

    if (std::fseek(fp, 0, SEEK_END) != 0) { std::fclose(fp); last_error_ = "fseek end"; return false; }
    long sz = std::ftell(fp);
    if (sz < static_cast<long>(kHeaderSize)) {
        std::fclose(fp);
        last_error_ = "file smaller than header";
        return false;
    }
    std::rewind(fp);

    data_      = new std::uint8_t[static_cast<std::size_t>(sz)];
    data_size_ = static_cast<std::size_t>(sz);
    std::size_t got = std::fread(data_, 1, data_size_, fp);
    std::fclose(fp);
    if (got != data_size_) {
        delete[] data_; data_ = nullptr; data_size_ = 0;
        last_error_ = "short read";
        return false;
    }

    // Header.
    std::uint32_t magic = read_u32_le(data_);
    if (magic != kMagic) {
        last_error_ = "bad magic (not a .piz)";
        return false;
    }
    version_       = read_u32_le(data_ + 4);
    std::uint32_t n = read_u32_le(data_ + 8);
    if (n == 0) {
        count_      = 0;
        last_error_ = "ok (empty archive)";
        return true;
    }
    if (kHeaderSize + static_cast<std::size_t>(n) * kEntrySize > data_size_) {
        last_error_ = "entry table extends past EOF";
        return false;
    }
    count_   = n;
    entries_ = new Entry[count_];

    // Parse each entry.
    for (std::uint32_t i = 0; i < count_; ++i) {
        const std::uint8_t* row = data_ + kHeaderSize + i * kEntrySize;
        Entry& e = entries_[i];

        // Name: 116 bytes ASCII, null-padded. Copy + force terminate.
        std::memcpy(e.name, row, kNameSize);
        e.name[kNameSize] = '\0';
        // Defensively null-terminate at first null (it's already null-padded
        // but this guards against tooling that didn't pad).
        for (std::size_t k = 0; k < kNameSize; ++k) {
            if (e.name[k] == '\0') break;
        }

        e.offset_raw   = read_u32_le(row + 0x74);
        e.length       = read_u32_le(row + 0x78);
        e.id           = read_u32_le(row + 0x7C);
        e.offset_bytes = e.offset_raw; // resolved after detect_mode
    }

    // Detect offset mode from first entry. If raw fits, use raw. Else if
    // sector fits, use sector. Else: corrupt / unsupported layout.
    {
        const Entry& e0 = entries_[0];
        const std::uint64_t end_raw    = static_cast<std::uint64_t>(e0.offset_raw) + e0.length;
        const std::uint64_t end_sector = static_cast<std::uint64_t>(e0.offset_raw) * kSectorSize + e0.length;

        if (end_raw <= data_size_) {
            mode_ = OffsetMode::Raw;
        } else if (end_sector <= data_size_) {
            mode_ = OffsetMode::Sector;
        } else {
            last_error_ = "first entry doesn't fit raw or sector mode";
            return false;
        }
    }

    // Resolve offset_bytes per entry now that mode is known.
    const std::uint32_t mult = (mode_ == OffsetMode::Sector) ? static_cast<std::uint32_t>(kSectorSize) : 1u;
    for (std::uint32_t i = 0; i < count_; ++i) {
        entries_[i].offset_bytes = entries_[i].offset_raw * mult;
    }

    last_error_ = "ok";
    return true;
}

const std::uint8_t* Archive::blob(std::uint32_t i, std::uint32_t* out_length) const {
    if (!valid() || i >= count_) return nullptr;
    const Entry& e = entries_[i];
    const std::uint64_t end = static_cast<std::uint64_t>(e.offset_bytes) + e.length;
    if (end > data_size_) return nullptr;
    if (out_length) *out_length = e.length;
    return data_ + e.offset_bytes;
}

}  // namespace Piz
}  // namespace mashed_re
