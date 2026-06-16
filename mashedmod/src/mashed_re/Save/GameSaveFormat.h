// Mashed RE — Save/GameSaveFormat.h  (WS-G5, 2026-06-16)
//
// Header-only, buffer-based port of the REAL gamesave.bin serialization, for the
// STANDALONE mashed_re.exe (replaces GameFlow's mashed_re_progress.bin sidecar
// with the real 0x24FA0-byte gamesave.bin format, written to a standalone-copy
// file — NEVER original/gamesave.bin).
//
// Save/GameSaveBuffer.cpp already ports Save::SerializeToBuffer (0x00404EE0) /
// DeserializeFromBuffer (0x00404E80) as dev-.asi HOOKS that operate on the live
// MASHED buffer at 0x00803358 via raw pointers. This header is the same FORMAT
// expressed over an explicit byte buffer, so it links into the standalone exe and
// is unit-testable in isolation. All offsets RVA-cited (Mashed_pool6, 2026-06-16,
// disasm of FUN_00404ee0 / FUN_00404f50; layout re/analysis/structs/gamesave_layout.md).
//
// The standalone has no live profile block (DAT_008a94a8 == 0), so the writer's
// profile copy (FUN_00404f2f) is skipped and the profile region stays zero — this
// matches the shipped blank-profile gamesave.bin exactly. The faithful pieces the
// standalone DOES carry are the magic, the 0x148-dword championship span, and the
// save-state counter.
#pragma once
#include <cstdint>
#include <cstring>

namespace mashed_re {
namespace Save {

// ── Real gamesave.bin envelope (file == 1:1 image of save_buf @ 0x00803358) ──
constexpr unsigned     kSaveSize   = 0x24FA0;   // FUN_00404f50: PUSH 0x24fa0 (total file size)
constexpr unsigned     kSpanOff    = 0x24A40;   // 0x00827d98 - 0x00803358 (FUN_00404f0f/f14)
constexpr unsigned     kSpanBytes  = 0x520;     // 0x148 dwords (FUN_00404f0a MOV ECX,0x148)
constexpr unsigned     kCounterOff = 0x24EFC;   // 0x00828254 (FUN_00404f23); span-relative 0x4BC
constexpr unsigned     kProfileOff = 0x4;       // save_buf+4 (FUN_00404f2f); standalone: stays zero
constexpr std::uint32_t kMagic     = 0xDEADBEEFu; // FUN_00404f37: MOV [0x803358],0xDEADBEEF

// ── Championship-span row layout (0x007f0a40 table; stride 0xC dwords / 0x30B).
//    Verified columns (used by the reader Nav_GameStateLoadSave too):
//      col 4 (+0x10) = track unlock (DAT_007f0a50); col 6 (+0x18) = car unlock.
//    The savedata / profile gates the menu grey-out reads live in the trailing
//    span (DAT_007f0f2c @ +0x4ec, DAT_007f0ad4 @ +0x094). ──
constexpr unsigned kRowStride      = 0x30;
constexpr unsigned kColTrackUnlock = 0x10;   // DAT_007f0a50  (verified)
constexpr unsigned kColCarUnlock   = 0x18;   // DAT_007f0a58  (verified)
constexpr unsigned kColTrophyPriv  = 0x14;   // standalone-private medal store (real col-5 [UNCERTAIN])
constexpr unsigned kSpanSavedataGate = 0x4ec; // DAT_007f0f2c (savedata present)
constexpr int      kSpanRows       = 13;
constexpr unsigned kSpanCounterOff = 0x4bc;  // counter within the span (= file 0x24EFC)

// Build a full real-format image from a championship span + save counter.
// Faithful order of Save::SerializeToBuffer (0x00404ee0), buffer form:
//   champ span copy (0x00404f19) -> counter overwrite at span+0x4bc (0x00404f23)
//   -> [profile skipped, ptr==0] -> magic at +0 (0x00404f37). Rest zero.
inline void BuildImage(const unsigned char span[kSpanBytes],
                       std::uint32_t counter, unsigned char out[kSaveSize]) {
    std::memset(out, 0, kSaveSize);
    std::memcpy(out + kSpanOff, span, kSpanBytes);   // 0x00404f19 championship snapshot
    std::memcpy(out + kCounterOff, &counter, 4);     // 0x00404f23 counter overwrite
    std::memcpy(out, &kMagic, 4);                    // 0x00404f37 magic
}

// Extract the championship span + counter from an image. Returns false if the
// size is wrong or the magic gate fails (blank/not-a-written-save). The blob is
// always exactly kSaveSize (FUN_00404f50); the DEADBEEF magic gate is caller-side
// (the shipped blank save has 0 at +0, first written by 0x00404f37 on first save).
inline bool ParseImage(const unsigned char* data, unsigned len,
                       unsigned char span[kSpanBytes], std::uint32_t* counter) {
    if (data == nullptr || len != kSaveSize) return false;
    std::uint32_t magic;
    std::memcpy(&magic, data, 4);
    if (magic != kMagic) return false;
    std::memcpy(span, data + kSpanOff, kSpanBytes);
    if (counter) std::memcpy(counter, data + kCounterOff, 4);
    return true;
}

}  // namespace Save
}  // namespace mashed_re
