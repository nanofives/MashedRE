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

// ─── Layout constants (identical in both builds) ──────────────────────────────
// Sizes/counts the copy loops use. These are the same numbers whether the code
// runs inside MASHED.exe (dev .asi) or in the standalone exe; only the base
// ADDRESSES below differ between the two builds.
static constexpr std::uint32_t  kTrackTableDwords   = 0x148u;      // 0x00404E82: MOV ECX,0x148 (328 dw = 1312 B)
static constexpr std::uint32_t  kStrideStep         = 0x4Cu;       // 0x4C = 76 stride (0x00404EF0 body)
static constexpr int            kStrideCount        = 12;          // 0..11 (0x00404EA0, 0x00404EF0)
static constexpr std::uint32_t  kProfileDwords      = 0x928Fu;     // 0x00404EC8: MOV ECX,0x928F (37519 dw = 150076 B)
static constexpr std::uint32_t  kDeadBeef           = 0xDEADBEEFu; // 0x00404F37 magic (raw 0xDEADBEEF; signed -559038737)

// ─── Tunnel bases — RVA-tunnel neutralization (D0.7 add-back gate) ─────────────
// TUNNEL-NEUTRALIZATION PATTERN (repeatable for the rest of Save/):
//   The two save functions never call MASHED code — they only READ/WRITE fixed
//   MASHED data globals (memcpy-shaped). So the whole tunnel is these base
//   addresses. Split them at compile time on MASHED_STANDALONE (defined only in
//   the exe cl invocation, mashedmod/build.bat). The function bodies are UNTOUCHED;
//   only these base constants change per build:
//     * dev .asi build (macro NOT defined, #else): the bases stay the original
//       absolute globals, so the bodies are byte-identical and the diff-original
//       Frida A/B reference is preserved unchanged.
//     * standalone exe build (macro defined): NO MASHED ADDRESS survives in the
//       code path (exe is based at 0x10000; 0x007xxxxx/0x008xxxxx are unmapped and
//       would AV). Each base resolves to a NAMED standalone symbol. The 7 logical
//       globals split 3-vs-4 by who else in the running game touches the data
//       (evidence: re/analysis/structs/gamesave_layout.md tail xref sweep;
//       re/analysis/save_gamesave_d3/00404e{80,ee0}.md):
//
//       OWN (3, PRIVATE to save — sole xref is the save code, so the standalone
//       declares its own scratch; nothing else reads these):
//         · trackTableCopy  (was DAT_00827D98) — save-side champ snapshot
//         · saveStateCounterCopy (was DAT_00828254) — save-side counter mirror
//         · saveBuf (+profile region, was 0x00803358) — the flat save buffer
//
//       BIND (4, SHARED ENGINE STATE — the running game maintains this; a private
//       duplicate would silently desync save from the live game). Two bind to a
//       real live standalone symbol BY NAME; two bind to the standalone's
//       DOCUMENTED absence of that state (there is no live state to desync from):
//         · trackTable (was DAT_007F0A40, live champ span) → Nav_SaveSpanData()
//           [Frontend/MenuNavSM.cpp g_save_span, exact 0x520 B]. packedBytes
//           (was DAT_007F0F54) is NOT a separate global — it is the tail 12 B of
//           this span (0x7F0F54-0x7F0A40 = +0x514), so it binds inside it.
//         · saveStateCounter (was DAT_008A95AC, live counter) → GameFlow_SaveCounterPtr()
//           [Race/GameFlow.cpp g_saveCounter, bumped by SaveProgress].
//         · strideRecords (was DAT_007F105C) — the standalone DROPS this state
//           (Save/GameSaveFormat.h BuildImage leaves the region zero); no live
//           symbol exists, so bind a local zeroed buffer (pack/unpack stay
//           byte-faithful over zero input). [UNCERTAIN U-3558]
//         · profile pointer (was DAT_008A94A8) — the standalone has NO live
//           profile (GameSaveFormat.h: "DAT_008a94a8 == 0"); bind a NULL slot so
//           the 150 KB profile copy is SKIPPED, matching documented behaviour
//           (region stays zero). [UNCERTAIN U-3560]
//
//   NOTE: in the exe, HookSystem::Register is stubbed to a no-op (Stubs/
//   HookSystemNoOp.cpp), so RH_ScopedInstall below makes these two functions DEAD
//   EXPORTS — linked, never patched, never called on the default path (the live
//   standalone save/load runs through GameSaveFormat.h BuildImage/ParseImage in
//   Race/GameFlow.cpp). The binds above are therefore dormant, but keep the port
//   correct for the day a real call site is wired (a separate slice).
#ifdef MASHED_STANDALONE
// Bind targets — resolved BY NAME at link time (no MASHED address, no duplicate).
namespace mashed_re { namespace Frontend { unsigned char* Nav_SaveSpanData(); } }
namespace mashed_re { namespace Race     { std::uint32_t* GameFlow_SaveCounterPtr(); } }
namespace {
// OWN — private save-side scratch (bucket A). Sizes match the MASHED globals.
alignas(4) std::uint8_t  s_trackTableCopy[kTrackTableDwords * 4];     // was DAT_00827D98 (1312 B snapshot)
std::uint32_t            s_saveStateCounterCopy = 0;                  // was DAT_00828254 (4 B mirror)
alignas(4) std::uint8_t  s_saveBuf[0x24FA0];                          // was 0x00803358 (0x24FA0-byte buffer)
// BIND-but-absent (bucket B, no live standalone symbol): local backing that stays
// zero, matching the standalone dropping this state. NOT a duplicate of live state
// (there is none) — a faithful stand-in for its documented absence.
alignas(4) std::uint8_t  s_strideRecordsAbsent[kStrideCount * kStrideStep]; // was DAT_007F105C (912 B, stays 0)
std::uintptr_t           s_profilePtrNull = 0;                        // was DAT_008A94A8 (NULL: no live profile)
}  // namespace
// BIND (bucket B) — live standalone engine state, referenced by name:
static const std::uintptr_t kTrackTable          = reinterpret_cast<std::uintptr_t>(mashed_re::Frontend::Nav_SaveSpanData());
static const std::uintptr_t kPackedBytes         = kTrackTable + 0x514u;   // tail 12 B of the champ span (was DAT_007F0F54)
static const std::uintptr_t kSaveStateCounter    = reinterpret_cast<std::uintptr_t>(mashed_re::Race::GameFlow_SaveCounterPtr());
static const std::uintptr_t kStrideBase          = reinterpret_cast<std::uintptr_t>(s_strideRecordsAbsent);
static const std::uintptr_t kProfilePtrPtr       = reinterpret_cast<std::uintptr_t>(&s_profilePtrNull);
// OWN (bucket A) — private save-side scratch:
static const std::uintptr_t kTrackTableCopy      = reinterpret_cast<std::uintptr_t>(s_trackTableCopy);
static const std::uintptr_t kSaveStateCounterCopy= reinterpret_cast<std::uintptr_t>(&s_saveStateCounterCopy);
static const std::uintptr_t kSaveBufBase         = reinterpret_cast<std::uintptr_t>(s_saveBuf);
static const std::uintptr_t kSaveBufProfile      = reinterpret_cast<std::uintptr_t>(s_saveBuf) + 4; // base+4, in-buffer
#else
// Original MASHED.exe absolute globals — the dev .asi diff reference. UNCHANGED.
static constexpr std::uintptr_t kTrackTable         = 0x007F0A40u; // DAT_007F0A40 (0x00404E8C, 0x00404F0F)
static constexpr std::uintptr_t kTrackTableCopy     = 0x00827D98u; // DAT_00827D98 (0x00404E87, 0x00404F14)
static constexpr std::uintptr_t kStrideBase         = 0x007F105Cu; // DAT_007F105C (0x00404EA0, 0x00404EF0)
static constexpr std::uintptr_t kPackedBytes        = 0x007F0F54u; // DAT_007F0F54 (0x00404EA0, 0x00404EF0)
static constexpr std::uintptr_t kStrideEnd          = 0x007F13EBu; // 0x00404EF0 loop limit (doc only; body uses kStrideCount)
static constexpr std::uintptr_t kSaveStateCounter   = 0x008A95ACu; // DAT_008A95AC (0x00404EC1, 0x00404F23)
static constexpr std::uintptr_t kSaveStateCounterCopy = 0x00828254u; // DAT_00828254 (0x00404EC1, 0x00404F23)
static constexpr std::uintptr_t kProfilePtrPtr      = 0x008A94A8u; // DAT_008A94A8 (0x00404EB4, 0x00404F21)
static constexpr std::uintptr_t kSaveBufBase        = 0x00803358u; // 0x00803358 (0x00404F37, 0x00404ECD)
static constexpr std::uintptr_t kSaveBufProfile     = 0x0080335Cu; // save_buf+4 (0x00404ECD, 0x00404F2F)
#endif

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

RH_ScopedInstall(DeserializeFromBuffer, 0x00404e80);  // re-enabled 2026-05-24 batch-save-a

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

RH_ScopedInstall(SerializeToBuffer, 0x00404ee0);  // re-enabled 2026-05-24 batch-save-a
