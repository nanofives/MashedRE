// PowerupMarkers — read a track's POWERUPS_GOLD.DFF placement markers.
//
// This is the ORIGINAL's live power-up placement source. FUN_004264d0
// @0x004264d0 pushes "powerups_gold.dff" (string @0x005cd4e4) at 0x004265bd,
// calls the DFF clump loader FUN_0042a5d0 at 0x004265c2, and on a non-null
// clump calls FUN_00426460 at 0x004265d2 — the live placement path. Only when
// the clump is null (JE at 0x004265ce) does it fall back to the Lua files, and
// every one of the 13 shipping track pizzes carries the DFF, so the Lua arm is
// unreachable on shipping data.
//
// FUN_00426460 @0x00426460 (__fastcall, clump in ECX):
//
//     n = FUN_004b3fc0(clump, local_80);       // RpClumpForAllAtomics, cap 32
//     for (i = 0; i < n; i++) {
//         frame = *(int *)(atomic + 4);        // RwObject.parent
//         v     = FUN_004b5190(atomic, 0, 0);  // RW user-data, array 0, elem 0
//         FUN_00458fd0(frame + 0x40, v & 0xff, (float)(v >> 8));
//     }
//
// `frame + 0x40` is the frame's MODELLING matrix position (RwFrame modelling
// matrix at +0x10, RwMatrix pos at +0x30) — the frame's own translation as
// RpClumpStreamRead wrote it from the FRAMELIST entry, not a parent-chain LTM.
// So this reads the FRAMELIST translation verbatim and never walks parents.
// (Measured over all 13 shipping tracks with re/tools/powerups_gold_dump.py:
// the parent-chain world translation equals the local one to max |d| = 0 on
// every marker, so on shipping data the distinction is unobservable anyway.)
//
// `v` is the geometry's RW USERDATA (0x011f) ARRAY 0, ELEMENT 0 — positionally,
// exactly as FUN_004b5190(atomic, 0, 0) reads it. Array 0 is authored
// `0.tv_part_id` (`0.Part_ID` on rouabout), dataType 1, one element.
//
// Index by POSITION, never by name. Measured: many geometries carry a SECOND
// user-data array — `FVF.UserData` on training and Warzone, and on sands and
// rouabout a second array with the SAME name `0.tv_part_id`/`0.Part_ID` holding
// twelve elements. A name lookup lets that duplicate shadow array 0 and returns
// the wrong type for 3 of sands's 25 markers and 14 of rouabout's 25.
//
// Measured: no ATOMIC-level USERDATA exists in these files, only
// geometry-level. The shift and mask above are the decode:
// type = v & 0xff, respawn seconds = v >> 8.
//
// ORDER. The original's pool order was measured live against the DFF on
// TRAINING (5 boxes) and ARCTIC (7 boxes) and is the REVERSE of the file's
// atomic order — FUN_004b3fc0 walks the clump's atomic linked list, which the
// stream read built back to front. Markers are returned in that enumeration
// order so the port's pool indices match the original's.
#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace mashed_re {
namespace Track {

struct PowerupMarker {
    float          pos[3];     // frame modelling-matrix translation (frame+0x40)
    int            type;       // v & 0xff
    float          respawn;    // (float)(v >> 8), seconds
    int            atomic;     // index in the DFF's atomic list (file order)
    std::uint32_t  raw;        // the undecoded tv_part_id dword
    bool           has_raw;    // false when the geometry carries no user data
};

// Parse a POWERUPS_GOLD.DFF blob. Returns false (and leaves *out untouched) if
// the blob is not a well-formed clump; *err, when non-null, receives a static
// reason string. Markers come back in the original's enumeration order.
bool ParsePowerupMarkers(const std::uint8_t* data, std::size_t len,
                         std::vector<PowerupMarker>* out,
                         const char** err = nullptr);

}  // namespace Track
}  // namespace mashed_re
