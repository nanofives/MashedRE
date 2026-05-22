// Mashed RE — Save/GameSaveBuffer.cpp
// 0x00404E80  Save::DeserializeFromBuffer
// 0x00404EE0  Save::SerializeToBuffer
// Session: save-sdone-a-s1 (2026-05-22)
//
// Mirror pair: Serialize packs live state into save_buf; Deserialize unpacks it back.
// Both operate on the 0x24FA0-byte global save buffer at 0x00803358.
//
// ─── DeserializeFromBuffer (0x00404E80) ────────────────────────────────────────
//
// Unpacks save buffer into live game state:
//   1. Championship table restore (0x00404E82): REP MOVSD 0x148 dwords
//      from DAT_00827D98 → DAT_007F0A40 (13×48B track table).
//   2. Stride-scatter loop (0x00404EA0): ECX 0..11 — reads packed byte
//      from DAT_007F0F54[ECX], zero-extends, writes to DAT_007F105C + ECX*0x4C.
//   3. State counter restore (0x00404EC1): DAT_008A95AC = DAT_00828254.
//   4. Profile deserialize (0x00404EC8): if *DAT_008A94A8 != NULL:
//      REP MOVSD 0x928F dwords from save_buf+4 (0x0080335C) to *DAT_008A94A8.
//      [UNCERTAIN U-3558 U-3560]
//
// Cited from: re/analysis/save_gamesave_d3/00404e80.md
//
// ─── SerializeToBuffer (0x00404EE0) ────────────────────────────────────────────
//
// Packs live game state into save_buf:
//   1. Stride-gather loop (0x00404EF0): EAX from 0x7F105C to 0x7F13EB step 0x4C,
//      ECX 0..11 — packs DAT_007F105C[ECX*0x4C] (1 byte) into DAT_007F0F54[ECX].
//      [UNCERTAIN U-3558]
//   2. Championship table snapshot (0x00404F0F): REP MOVSD 0x148 dwords
//      from DAT_007F0A40 → DAT_00827D98.
//   3. State counter save (0x00404F23): DAT_00828254 = DAT_008A95AC.
//   4. Profile serialize (0x00404F21): if *DAT_008A94A8 != NULL:
//      REP MOVSD 0x928F dwords from *DAT_008A94A8 → save_buf+4 (0x0080335C).
//      [UNCERTAIN U-3558 U-3559]
//   5. Magic write (0x00404F37): MOV [0x00803358], 0xDEADBEEF.
//
// Cited from: re/analysis/save_gamesave_d3/00404ee0.md

#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstring>

// ─── Address constants ────────────────────────────────────────────────────────

// Championship track table: 13×48B = 624B = 0x270B = 0x148 dwords (1312 bytes)
static constexpr std::uintptr_t kTrackTable         = 0x007F0A40u; // DAT_007F0A40 (0x00404E8C, 0x00404F0F)
static constexpr std::uintptr_t kTrackTableCopy     = 0x00827D98u; // DAT_00827D98 (0x00404E87, 0x00404F14)
static constexpr std::uint32_t  kTrackTableDwords   = 0x148u;      // 0x00404E82: MOV ECX, 0x148

// Stride-scatter/gather: 12 bytes at stride 0x4C
static constexpr std::uintptr_t kStrideBase         = 0x007F105Cu; // DAT_007F105C (0x00404EA0, 0x00404EF0)
static constexpr std::uintptr_t kPackedBytes        = 0x007F0F54u; // DAT_007F0F54 (0x00404EA0, 0x00404EF0)
static constexpr std::uintptr_t kStrideEnd          = 0x007F13EBu; // 0x00404EF0: loop limit
static constexpr std::uint32_t  kStrideStep         = 0x4Cu;       // 0x4C stride (0x00404EF0 body)
static constexpr int            kStrideCount        = 12;          // 0..11 (0x00404EA0, 0x00404EF0)

// State counter
static constexpr std::uintptr_t kSaveStateCounter   = 0x008A95ACu; // DAT_008A95AC (0x00404EC1, 0x00404F23)
static constexpr std::uintptr_t kSaveStateCounterCopy = 0x00828254u; // DAT_00828254 (0x00404EC1, 0x00404F23)

// Profile data
static constexpr std::uintptr_t kProfilePtrPtr      = 0x008A94A8u; // DAT_008A94A8 (0x00404EB4, 0x00404F21)
static constexpr std::uintptr_t kSaveBufBase        = 0x00803358u; // 0x00803358 (0x00404F37, 0x00404ECD)
static constexpr std::uintptr_t kSaveBufProfile     = 0x0080335Cu; // save_buf+4 (0x00404ECD, 0x00404F2F)
static constexpr std::uint32_t  kProfileDwords      = 0x928Fu;     // 0x00404EC8: MOV ECX, 0x928F

static constexpr std::uint32_t  kDeadBeef           = 0xDEADBEEFu; // 0x00404F37: magic write

static inline std::uint32_t readU32at(std::uintptr_t a) {
    return *reinterpret_cast<const std::uint32_t*>(a);
}
static inline void writeU32at(std::uintptr_t a, std::uint32_t v) {
    *reinterpret_cast<std::uint32_t*>(a) = v;
}

// ─── 0x00404E80  Save::DeserializeFromBuffer ──────────────────────────────────
// void(void): unpacks save_buf back into live game state.
// 0x00404E80
extern "C" __declspec(dllexport) void __cdecl DeserializeFromBuffer() {
    // 1. Championship table restore: DAT_00827D98 → DAT_007F0A40 (0x148 dwords).
    // 0x00404E82..0x00404E91: MOV ECX,0x148; MOV ESI,0x827D98; MOV EDI,0x7F0A40; REP MOVSD
    std::memcpy(
        reinterpret_cast<void*>(kTrackTable),
        reinterpret_cast<const void*>(kTrackTableCopy),
        kTrackTableDwords * sizeof(std::uint32_t));

    // 2. Stride-scatter loop: DAT_007F0F54[ECX] (byte) → DAT_007F105C + ECX*0x4C (dword).
    // 0x00404EA0..0x00404EB2: MOVZX EDX,[007F0F54+ECX]; MOV [007F105C+ECX*4C],EDX
    for (int ecx = 0; ecx < kStrideCount; ++ecx) {
        const std::uint8_t  bval  = *reinterpret_cast<const std::uint8_t*>(kPackedBytes + static_cast<std::uint32_t>(ecx));
        const std::uint32_t dval  = static_cast<std::uint32_t>(bval);  // zero-extend
        writeU32at(kStrideBase + static_cast<std::uint32_t>(ecx) * kStrideStep, dval);
    }

    // 3. State counter restore: DAT_008A95AC = DAT_00828254.
    // 0x00404EC1: MOV EAX,[00828254]; MOV [008A95AC],EAX
    writeU32at(kSaveStateCounter, readU32at(kSaveStateCounterCopy));

    // 4. Profile deserialize: if *DAT_008A94A8 != NULL → REP MOVSD 0x928F dwords
    //    from save_buf+4 (0x0080335C) to *DAT_008A94A8. [UNCERTAIN U-3560]
    // 0x00404EB4: MOV EDI,[008A94A8]; CMP EDI,0; JZ skip
    // 0x00404EC8: MOV ECX,0x928F; MOV ESI,0x80335C; REP MOVSD
    const std::uintptr_t profile_ptr = readU32at(kProfilePtrPtr);
    if (profile_ptr != 0u) {
        std::memcpy(
            reinterpret_cast<void*>(profile_ptr),
            reinterpret_cast<const void*>(kSaveBufProfile),
            kProfileDwords * sizeof(std::uint32_t));
    }
}

RH_ScopedInstall(DeserializeFromBuffer, 0x00404e80);

// ─── 0x00404EE0  Save::SerializeToBuffer ──────────────────────────────────────
// void(void): packs live state into save_buf; writes DEADBEEF magic.
// 0x00404ee0
extern "C" __declspec(dllexport) void __cdecl SerializeToBuffer() {
    // 1. Stride-gather loop: DAT_007F105C + ECX*0x4C (byte, low byte of dword)
    //    → DAT_007F0F54[ECX]. [UNCERTAIN U-3558]
    // 0x00404EF0: XOR ECX,ECX; MOV EAX,0x7F105C;
    //   loop body: MOVZX EDX,[EAX]; MOV [7F0F54+ECX],DL; ADD EAX,0x4C; INC ECX; CMP EAX,0x7F13EB+1; JL
    for (int ecx = 0; ecx < kStrideCount; ++ecx) {
        // Read the low byte at kStrideBase + ecx*0x4C.
        const std::uint8_t bval = *reinterpret_cast<const std::uint8_t*>(
            kStrideBase + static_cast<std::uint32_t>(ecx) * kStrideStep);
        *reinterpret_cast<std::uint8_t*>(kPackedBytes + static_cast<std::uint32_t>(ecx)) = bval;
    }

    // 2. Championship table snapshot: DAT_007F0A40 → DAT_00827D98 (0x148 dwords).
    // 0x00404F0F: MOV ESI,0x7F0A40; MOV EDI,0x827D98; MOV ECX,0x148; REP MOVSD
    std::memcpy(
        reinterpret_cast<void*>(kTrackTableCopy),
        reinterpret_cast<const void*>(kTrackTable),
        kTrackTableDwords * sizeof(std::uint32_t));

    // 3. State counter save: DAT_00828254 = DAT_008A95AC.
    // 0x00404F23: MOV EAX,[008A95AC]; MOV [00828254],EAX
    writeU32at(kSaveStateCounterCopy, readU32at(kSaveStateCounter));

    // 4. Profile serialize: if *DAT_008A94A8 != NULL → REP MOVSD 0x928F dwords
    //    from *DAT_008A94A8 to save_buf+4. [UNCERTAIN U-3558 U-3559]
    // 0x00404F21: MOV ESI,[008A94A8]; CMP ESI,0; JZ skip
    // 0x00404F2F: MOV EDI,0x80335C; REP MOVSD
    const std::uintptr_t profile_ptr = readU32at(kProfilePtrPtr);
    if (profile_ptr != 0u) {
        std::memcpy(
            reinterpret_cast<void*>(kSaveBufProfile),
            reinterpret_cast<const void*>(profile_ptr),
            kProfileDwords * sizeof(std::uint32_t));
    }

    // 5. Magic write: MOV DWORD PTR [0x00803358], 0xDEADBEEF.
    // 0x00404F37: C7 05 58 33 80 00 EF BE AD DE
    writeU32at(kSaveBufBase, kDeadBeef);
}

RH_ScopedInstall(SerializeToBuffer, 0x00404ee0);
