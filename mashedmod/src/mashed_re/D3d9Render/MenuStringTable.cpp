// Mashed RE — menu string/sprite id -> glyph-string table loader.
// See MenuStringTable.h + re/analysis/standalone_menu_sm/sprite_atlas_by_id_spec.md
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "MenuStringTable.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../Piz/PizReader.h"

namespace mashed_re {
namespace D3d9Render {

namespace {
inline std::uint32_t RdU32(const std::uint8_t* p, std::size_t o) {
    std::uint32_t v; std::memcpy(&v, p + o, 4); return v;
}
inline std::uint16_t RdU16(const std::uint8_t* p, std::size_t o) {
    std::uint16_t v; std::memcpy(&v, p + o, 2); return v;
}

// FUN_004277a0 control-char -> special FGDC20 glyph code remap (verbatim).
// 8->0x81 9->0x7f 0xa->0x81 0xb->0x8d 0xc->0x80 0xd->0x87 0xe->0x8f
inline std::uint16_t RemapCtrl(std::uint16_t ch) {
    switch (ch) {
        case 0x08: return 0x81;
        case 0x09: return 0x7f;
        case 0x0a: return 0x81;
        case 0x0b: return 0x8d;
        case 0x0c: return 0x80;
        case 0x0d: return 0x87;
        case 0x0e: return 0x8f;
        default:   return ch;
    }
}
}  // namespace

bool MenuStringTable::LoadFile(const char* dat_path) {
    std::free(m_data); m_data = nullptr; m_len = 0;
    std::FILE* f = std::fopen(dat_path, "rb");
    if (!f) return false;
    std::fseek(f, 0, SEEK_END);
    long sz = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    if (sz < 8 || sz > 0x10000) { std::fclose(f); return false; }
    auto* buf = static_cast<std::uint8_t*>(std::malloc(static_cast<std::size_t>(sz)));
    if (!buf) { std::fclose(f); return false; }
    std::size_t rd = std::fread(buf, 1, static_cast<std::size_t>(sz), f);
    std::fclose(f);
    if (rd != static_cast<std::size_t>(sz)) { std::free(buf); return false; }
    m_data = buf;
    m_len  = static_cast<std::uint32_t>(sz);
    return true;
}

bool MenuStringTable::LoadPizEntry(const char* piz_path, const char* entry_name) {
    std::free(m_data); m_data = nullptr; m_len = 0;
    mashed_re::Piz::Archive piz;
    if (!piz.Load(piz_path)) return false;
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        const char* n = piz.entry(i).name;
        if (n && std::strcmp(n, entry_name) == 0) {
            std::uint32_t blen = 0;
            const std::uint8_t* blob = piz.blob(i, &blen);
            if (!blob || blen < 8) return false;
            auto* buf = static_cast<std::uint8_t*>(std::malloc(blen));
            if (!buf) return false;
            std::memcpy(buf, blob, blen);
            m_data = buf; m_len = blen;
            return true;
        }
    }
    return false;
}

std::uint32_t MenuStringTable::id_count() const {
    if (!m_data || m_len < 4) return 0;
    const std::uint32_t first = RdU32(m_data, 0);
    return (first >= 4 && first <= m_len) ? first / 4u : 0u;
}

bool MenuStringTable::Resolve(int id, std::uint32_t* out_off) const {
    if (!m_data || id < 0) return false;
    const std::uint32_t off_pos = static_cast<std::uint32_t>(id) * 4u;
    if (off_pos + 4 > m_len) return false;
    const std::uint32_t off = RdU32(m_data, off_pos);
    if (off == 0 || off + 2 > m_len) return false;   // off 0 = unused sentinel
    if (out_off) *out_off = off;
    return true;
}

int MenuStringTable::Decode(int id, wchar_t* out, int cap) const {
    if (out && cap > 0) out[0] = 0;
    std::uint32_t off = 0;
    if (cap <= 0 || !Resolve(id, &off)) return 0;
    const std::uint16_t len = RdU16(m_data, off);
    if (off + 2 + static_cast<std::uint32_t>(len) * 2u > m_len) return 0;
    int n = 0;
    for (int i = 0; i < len && n < cap - 1; ++i) {
        const std::uint16_t ch = RemapCtrl(RdU16(m_data, off + 2 + i * 2));
        out[n++] = static_cast<wchar_t>(ch);
    }
    out[n] = 0;
    return n;
}

}  // namespace D3d9Render
}  // namespace mashed_re
