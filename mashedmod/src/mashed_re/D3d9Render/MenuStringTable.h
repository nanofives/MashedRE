// Mashed RE — menu string/sprite id -> glyph-string table (the standalone's
// "sprite-atlas-by-id" for menu records).
//
// RE basis: re/analysis/standalone_menu_sm/sprite_atlas_by_id_spec.md
//
// The menu draw loop (FUN_0043c5b0) draws each record's primary/secondary id via
// FUN_00428140, whose id->data lookup is FUN_00427780:
//     entry_ptr = base + *(u32*)(base + id*4)        (base = DAT_0066d828)
// where the table at DAT_0066d828 is the whole language .DAT file (<=64KB),
// runtime-loaded by FUN_004274e0 (RwStreamRead 0x10000 bytes from <lang>.dat).
// Each entry is [u16 len][len * u16 UTF-16LE]. FUN_004277a0 copies the chars into
// a scratch buffer, NUL-terminates, and remaps control codes to special FGDC20
// glyph codes (the nav/prompt arrows): 8->0x81 9->0x7f 0xa->0x81 0xb->0x8d
// 0xc->0x80 0xd->0x87 0xe->0x8f.
//
// This is NOT a per-id texture atlas — each id resolves to a string drawn through
// the FGDC20 RtCharset (MashedFont). The named-sprite TXD atlas (badges.txd via
// SpriteLookupC) is a SEPARATE, name-keyed path used only for menu chrome; see
// the spec's "(3)" section for its (still-blocked) crack.
#pragma once

#include <cstdint>

namespace mashed_re {
namespace D3d9Render {

class MenuStringTable {
public:
    // Load the language .DAT id-table. `dat_path` is a raw file (e.g.
    // original/TOASTART/Common/FONT/USA.dat) — exactly what FUN_004274e0 streams.
    // Returns true on success.
    bool LoadFile(const char* dat_path);
    // Alternate source: a .piz entry (e.g. Font36.piz/USA.DAT), mirroring the
    // standalone's existing LoadMessageTable. Returns true on success.
    bool LoadPizEntry(const char* piz_path, const char* entry_name);

    bool          ready() const { return m_data != nullptr; }
    std::uint32_t size()  const { return m_len; }
    // Number of ids the offset table covers (first-entry-offset / 4).
    std::uint32_t id_count() const;

    // Resolve `id` to its raw entry byte offset within the blob.
    // Returns false if id is out of range or the offset is the unused sentinel 0.
    bool Resolve(int id, std::uint32_t* out_off) const;

    // Decode `id` into a NUL-terminated UTF-16 string in out[cap], applying the
    // FUN_004277a0 control-char -> special-glyph remap. Returns char count (0 if
    // the id is empty/unused). `out` is always NUL-terminated when cap>0.
    int Decode(int id, wchar_t* out, int cap) const;

private:
    std::uint8_t* m_data = nullptr;   // owned (malloc) language-table blob
    std::uint32_t m_len  = 0;
};

}  // namespace D3d9Render
}  // namespace mashed_re
