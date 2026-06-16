// Mashed RE — track data-format parsers (WS-F): SPL / ANM / UVA / MTS / LAPDATA.
//
// C++ twin of re/tools/rw_track_data.py (cracked + data-verified 2026-06-16
// against all 229 such assets across the 13 track pizzes — see
// re/analysis/formats/track_anim_data.md). All four binary formats are standard
// RenderWare core chunk streams; the chunk IDs were cross-checked against
// re/prior_art/renderware/.../rw/rwplcore.h:
//
//   .SPL  rwID_SPLINE          (0x0C)  water / wave paths
//   .ANM  rwID_HANIMANIMATION  (0x1B)  helicopter / copter / cameraman flight paths
//   .UVA  rwID_UVANIMDICT      (0x2B)  scrolling-UV dictionary (sea / sky)
//   .MTS  rwID_MATRIX          (0x0D)  instance-placement matrices (crates / lights /
//                                      scaled foliage) — NOT material scripts
//   LAPDATA.LUA  (text)        lap lines / split sectors / safe-start lines
//
// Parsers are renderer-agnostic (take a raw blob, like DffModel) so they unit-
// test off the piz bytes and stay usable from any consumer.
#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

namespace mashed_re {
namespace Track {

// ---- F1 .SPL — rwID_SPLINE -------------------------------------------------
struct Spline {
    std::int32_t          flag = 0;       // 1 in every shipped wave/rail spline
    std::vector<float>    pts;            // x,y,z per point
    std::uint32_t         num_points = 0;
    bool                  constant_y = false;  // true for the WAVE ripple paths
    float                 y = 0.f;             // the shared Y when constant_y

    bool Parse(const std::uint8_t* d, std::size_t len);
};

// ---- F2 .ANM — rwID_HANIMANIMATION (standard 36-byte keyframe, scheme 1) ----
struct HKeyFrame {
    float        time;
    float        q[4];   // unit quaternion x,y,z,w
    float        t[3];   // translation x,y,z
};
struct HAnim {
    std::int32_t            scheme = 1;     // 1 = HAnim std keyframe
    float                   duration = 0.f;
    std::vector<HKeyFrame>  frames;

    bool Parse(const std::uint8_t* d, std::size_t len);
    // Sample the path at time `t` (seconds, clamped/looped to [0,duration]).
    // out_pos[3] = interpolated translation, out_q[4] = nlerp'd rotation.
    void Sample(float t, float out_pos[3], float out_q[4]) const;
};

// ---- F3 .UVA — rwID_UVANIMDICT (dict of scheme-0x1C1 UV anims) --------------
struct UVEntry {
    std::string  name;                 // e.g. "bmp_Sea_M", "bmp_Sky_M"
    float        duration = 0.f;
    int          num_frames = 0;
    // UV-translation scroll rate (units / second) = (last - first) / duration,
    // from the keyframe affine's tx,ty components (uv[4],uv[5]).
    float        du_dt = 0.f;
    float        dv_dt = 0.f;
    // The material/texture this binds to, by the RW convention "bmp_<Tex>_M":
    // returns "Sea" for "bmp_Sea_M". Empty if the name has no such shape.
    std::string  TextureStem() const;
};
struct UVDict {
    std::vector<UVEntry> anims;
    bool Parse(const std::uint8_t* d, std::size_t len);
};

// ---- F5 .MTS — count + N rwID_MATRIX chunks --------------------------------
struct MtxInstance {
    float        m[12];   // right(3), up(3), at(3), pos(3) — RW row order
    std::int32_t type;    // 3 = orthonormal (crates/lights); 0 = scaled (foliage)
};
struct MtxList {
    std::vector<MtxInstance> items;
    bool Parse(const std::uint8_t* d, std::size_t len);
};

// ---- F4 LAPDATA.LUA — text -------------------------------------------------
struct LapData {
    int                                      lap_variations = 1;
    std::vector<int>                         lap_lines;       // gate indices
    std::vector<std::pair<int, int>>         safe_start_lines;// (a,b) ranges
    std::vector<std::pair<int, int>>         split_sectors;   // (split_id, gate)

    // Parse the LAPDATA.LUA text (comments stripped). Accepts both the
    // `Lap_Line(-1)` and `Lap_Line_End()` terminator dialects.
    bool Parse(const char* text, std::size_t len);
    bool valid() const { return !lap_lines.empty(); }
    // The gate that completes a lap (the primary start/finish line).
    int  finish_gate() const { return lap_lines.empty() ? 0 : lap_lines.front(); }
    bool is_lap_line(int gate) const;
};

}  // namespace Track
}  // namespace mashed_re
