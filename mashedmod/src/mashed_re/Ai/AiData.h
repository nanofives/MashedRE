// Mashed RE — AI path-data (.AI) format + loader (WS-C2 step 2).
//
// Header-only definition of the opponent-AI race-line spline + tile-grid file
// that the FUN_00418860 controller indexes. Format CRACKED + data-verified this
// session against the real TOASTART/Common/AI.piz bytes (13/13 members) — see
// re/analysis/formats/ai_path_data.md and re/tools/ai_data.py.
//
// Consumer: the ported controller once it runs in the standalone exe. The dev
// .asi uses the original loader FUN_004235b0; in the standalone, call
// AiData_LoadInto() with an AI%d.AI member (extracted from AI.piz) to populate
// the controller's expected memory image at 0x007f1a9c.
//
// NO-GUESSING: every constant cites its RVA.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
#pragma once

#include <cstdint>
#include <cstring>

namespace Ai {

// ---- RW chunk header (FUN_00423540 writer / FUN_004235b0 reader) -----------
static constexpr std::uint32_t kAiChunkType    = 0x13269902u;  // FUN_004235b0 FUN_004cc5e0 tag
static constexpr std::uint32_t kAiPayloadSize  = 0x11884u;     // FUN_00423540 (size arg)
static constexpr std::uint32_t kAiHeaderLen    = 12u;          // type+size+version
static constexpr std::uint32_t kAiChunkTotal   = kAiPayloadSize + kAiHeaderLen;  // 0x11890
// Observed versions: 0x1c02000a (12 tracks), 0x1803ffff (AI30.AI). Reader ignores it.

// ---- payload region offsets (= DAT_007f1a9c image; abs addrs in comments) ---
static constexpr std::uint32_t kAiTileGridOff  = 0x00000u;  // 0x007f1a9c  128x128 int16
static constexpr std::uint32_t kAiSubCellOff   = 0x08000u;  // 0x007f9a9c  0x200 tiles x 8x8 char
static constexpr std::uint32_t kAiRaceOff      = 0x10004u;  // 0x00801aa0
static constexpr std::uint32_t kAiInsideOff    = 0x10610u;  // 0x008020ac
static constexpr std::uint32_t kAiSlowOff      = 0x10c1cu;  // 0x008026b8
static constexpr std::uint32_t kAiCheatOff     = 0x11228u;  // 0x00802cc4

static constexpr std::uint32_t kAiSplineStride = 0x204u;    // FUN_00418560 0x0041861f
static constexpr std::uint32_t kAiSplineCountOff = 0x200u;  // FUN_00418560 0x00418625
static constexpr std::uint32_t kAiSplinesPerType = 3u;
static constexpr std::uint32_t kAiMaxPoints      = 64u;
static constexpr std::uint32_t kAiTileDim        = 128u;

enum AiLineType { AI_RACE = 0, AI_INSIDE = 1, AI_SLOW = 2, AI_CHEAT = 3 };

// payload-relative base of a (line-type, spline-index) bank.
inline std::uint32_t AiData_SplineOff(AiLineType type, std::uint32_t idx)
{
    std::uint32_t base = kAiRaceOff;
    switch (type) {
        case AI_RACE:   base = kAiRaceOff;   break;
        case AI_INSIDE: base = kAiInsideOff; break;
        case AI_SLOW:   base = kAiSlowOff;   break;
        case AI_CHEAT:  base = kAiCheatOff;  break;
    }
    return base + idx * kAiSplineStride;
}

// Active point count of a bank within a 0x11884-byte payload.
inline std::int32_t AiData_SplineCount(const std::uint8_t* payload,
                                       AiLineType type, std::uint32_t idx)
{
    std::int32_t c;
    std::memcpy(&c, payload + AiData_SplineOff(type, idx) + kAiSplineCountOff, 4);
    return c;
}

// One XZ point (i in [0,count)) of a bank.
inline void AiData_SplinePoint(const std::uint8_t* payload, AiLineType type,
                               std::uint32_t idx, std::uint32_t i, float* outX, float* outZ)
{
    const std::uint8_t* p = payload + AiData_SplineOff(type, idx) + i * 8u;
    std::memcpy(outX, p,     4);
    std::memcpy(outZ, p + 4, 4);
}

// Validate an AI%d.AI member (raw file bytes incl. the 12-byte chunk header).
// Returns the payload pointer (raw + 12) on success, nullptr on format break.
inline const std::uint8_t* AiData_Validate(const std::uint8_t* raw, std::size_t len)
{
    if (raw == nullptr || len != kAiChunkTotal) return nullptr;
    std::uint32_t type, size;
    std::memcpy(&type, raw + 0, 4);
    std::memcpy(&size, raw + 4, 4);
    if (type != kAiChunkType || size != kAiPayloadSize) return nullptr;
    return raw + kAiHeaderLen;
}

// Copy a validated AI%d.AI payload into the controller's expected image
// (dest = the 0x11884-byte region the controller indexes, e.g. 0x007f1a9c in the
// image-padded standalone). Returns true on success.
inline bool AiData_LoadInto(const std::uint8_t* raw, std::size_t len, void* dest)
{
    const std::uint8_t* payload = AiData_Validate(raw, len);
    if (payload == nullptr || dest == nullptr) return false;
    std::memcpy(dest, payload, kAiPayloadSize);
    return true;
}

} // namespace Ai
