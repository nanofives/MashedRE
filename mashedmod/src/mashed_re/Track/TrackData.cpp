// Mashed RE — track data-format parsers (WS-F). See TrackData.h.
// Byte layouts validated against the real assets in re/tools/rw_track_data.py
// (zero parse failures over 229 assets / 13 tracks, 2026-06-16).
#include "TrackData.h"

#include <cmath>
#include <cstring>

namespace mashed_re {
namespace Track {

namespace {
// RenderWare core chunk IDs (rwplcore.h MAKECHUNKID(rwVENDORID_CORE,n) == n).
constexpr std::uint32_t rwID_STRUCT = 0x01;
constexpr std::uint32_t rwID_SPLINE = 0x0C;
constexpr std::uint32_t rwID_MATRIX = 0x0D;
constexpr std::uint32_t rwID_HANIMANIMATION = 0x1B;
constexpr std::uint32_t rwID_UVANIMDICT = 0x2B;

inline std::uint32_t u32(const std::uint8_t* p) {
    std::uint32_t v;
    std::memcpy(&v, p, 4);
    return v;
}
inline float f32(const std::uint8_t* p) {
    float v;
    std::memcpy(&v, p, 4);
    return v;
}
}  // namespace

// ---------------------------------------------------------------------------
// F1 .SPL — rwID_SPLINE
//   [chunk 0x0C][32-byte sub-header (bbox/reserved; zero in shipped splines)]
//   [u32 numPoints][u32 flag][numPoints * (float x,y,z)]
// ---------------------------------------------------------------------------
bool Spline::Parse(const std::uint8_t* d, std::size_t len) {
    if (len < 12 + 32 + 8 || u32(d) != rwID_SPLINE) return false;
    std::size_t o = 12 + 32;            // skip chunk header + sub-header
    num_points = u32(d + o);            o += 4;
    flag = static_cast<std::int32_t>(u32(d + o)); o += 4;
    if (12 + 32 + 8 + std::size_t(num_points) * 12 != len) return false;
    pts.resize(std::size_t(num_points) * 3);
    float ymin = 1e30f, ymax = -1e30f;
    for (std::uint32_t i = 0; i < num_points; ++i) {
        float x = f32(d + o), yy = f32(d + o + 4), z = f32(d + o + 8);
        pts[i * 3 + 0] = x; pts[i * 3 + 1] = yy; pts[i * 3 + 2] = z;
        if (yy < ymin) ymin = yy;
        if (yy > ymax) ymax = yy;
        o += 12;
    }
    constant_y = num_points > 0 && (ymax - ymin) < 1e-3f;
    y = num_points > 0 ? pts[1] : 0.f;
    return true;
}

// ---------------------------------------------------------------------------
// F2 .ANM — rwID_HANIMANIMATION
//   [chunk 0x1B][u32 version=0x100][u32 scheme=1][u32 numFrames][u32 flags]
//   [float duration][numFrames * 36B keyframe{ time, q[4], t[3], u32 prevOff }]
// ---------------------------------------------------------------------------
bool HAnim::Parse(const std::uint8_t* d, std::size_t len) {
    if (len < 12 + 20 || u32(d) != rwID_HANIMANIMATION) return false;
    scheme = static_cast<std::int32_t>(u32(d + 16));
    std::uint32_t num = u32(d + 20);
    duration = f32(d + 28);
    if (scheme != 1) return false;
    if (12 + 20 + std::size_t(num) * 36 != len) return false;
    frames.resize(num);
    std::size_t o = 32;
    for (std::uint32_t i = 0; i < num; ++i) {
        HKeyFrame& k = frames[i];
        k.time = f32(d + o);
        for (int j = 0; j < 4; ++j) k.q[j] = f32(d + o + 4 + j * 4);
        for (int j = 0; j < 3; ++j) k.t[j] = f32(d + o + 20 + j * 4);
        o += 36;   // (dword[8] = prevFrameOffset, runtime linkage, unused here)
    }
    return true;
}

void HAnim::Sample(float t, float out_pos[3], float out_q[4]) const {
    if (frames.empty()) {
        out_pos[0] = out_pos[1] = out_pos[2] = 0.f;
        out_q[0] = out_q[1] = out_q[2] = 0.f; out_q[3] = 1.f;
        return;
    }
    if (duration > 0.f) t = std::fmod(t, duration);
    if (t < 0.f) t += duration;
    // find the bracketing keyframes (frames are time-sorted)
    std::size_t i1 = 0;
    while (i1 + 1 < frames.size() && frames[i1 + 1].time <= t) ++i1;
    std::size_t i2 = (i1 + 1 < frames.size()) ? i1 + 1 : i1;
    const HKeyFrame& a = frames[i1];
    const HKeyFrame& b = frames[i2];
    float span = b.time - a.time;
    float u = span > 1e-6f ? (t - a.time) / span : 0.f;
    for (int j = 0; j < 3; ++j) out_pos[j] = a.t[j] + (b.t[j] - a.t[j]) * u;
    // nlerp the quaternion (shortest arc)
    float dot = 0.f;
    for (int j = 0; j < 4; ++j) dot += a.q[j] * b.q[j];
    float s = dot < 0.f ? -1.f : 1.f;
    float len2 = 0.f;
    for (int j = 0; j < 4; ++j) {
        out_q[j] = a.q[j] + (b.q[j] * s - a.q[j]) * u;
        len2 += out_q[j] * out_q[j];
    }
    float inv = len2 > 1e-12f ? 1.f / std::sqrt(len2) : 1.f;
    for (int j = 0; j < 4; ++j) out_q[j] *= inv;
}

// ---------------------------------------------------------------------------
// F3 .UVA — rwID_UVANIMDICT
//   [chunk 0x2B][STRUCT chunk: u32 numAnims]
//   numAnims * [chunk 0x1B RtAnimAnimation{
//       u32 ver=0x100, u32 scheme=0x1C1, u32 numFrames, u32 flags, float dur,
//       u32 ?, char name[32], u32 pad[8],
//       numFrames * 32B keyframe{ float time, float uv[6], u32 prevOff } }]
// ---------------------------------------------------------------------------
std::string UVEntry::TextureStem() const {
    // "bmp_Sea_M" -> "Sea". Strip a leading "bmp_" and a trailing "_M"/"_m".
    std::string s = name;
    if (s.size() > 4 && (s.compare(0, 4, "bmp_") == 0 || s.compare(0, 4, "BMP_") == 0))
        s = s.substr(4);
    if (s.size() > 2 && s[s.size() - 2] == '_' &&
        (s.back() == 'M' || s.back() == 'm'))
        s = s.substr(0, s.size() - 2);
    return s;
}

bool UVDict::Parse(const std::uint8_t* d, std::size_t len) {
    if (len < 28 || u32(d) != rwID_UVANIMDICT || u32(d + 12) != rwID_STRUCT)
        return false;
    std::uint32_t n = u32(d + 24);
    std::size_t o = 28;
    for (std::uint32_t a = 0; a < n; ++a) {
        if (o + 12 > len || u32(d + o) != rwID_HANIMANIMATION) return false;
        std::uint32_t asz = u32(d + o + 4);
        std::size_t body = o + 12;
        if (body + 88 > len) return false;
        UVEntry e;
        e.num_frames = static_cast<int>(u32(d + body + 8));
        e.duration = f32(d + body + 16);
        const char* nm = reinterpret_cast<const char*>(d + body + 24);
        std::size_t nlen = 0;
        while (nlen < 32 && nm[nlen]) ++nlen;
        e.name.assign(nm, nlen);
        if (e.num_frames >= 2 && e.duration > 0.f) {
            std::size_t kf = body + 88;
            const std::uint8_t* k0 = d + kf;
            const std::uint8_t* kN = d + kf + std::size_t(e.num_frames - 1) * 32;
            // uv[4],uv[5] = the animated translation (tx,ty)
            e.du_dt = (f32(kN + 4 + 16) - f32(k0 + 4 + 16)) / e.duration;
            e.dv_dt = (f32(kN + 4 + 20) - f32(k0 + 4 + 20)) / e.duration;
        }
        anims.push_back(std::move(e));
        o = body + asz;
    }
    return !anims.empty();
}

// ---------------------------------------------------------------------------
// F5 .MTS — u32 count + count * [0x0D matrix wrapper][0x01 struct][12 float][u32 type]
//   per-record stride = 12 (wrapper hdr) + 12 (struct hdr) + 52 (12f + u32) = 76
// ---------------------------------------------------------------------------
bool MtxList::Parse(const std::uint8_t* d, std::size_t len) {
    if (len < 4) return false;
    std::uint32_t cnt = u32(d);
    std::size_t o = 4;
    for (std::uint32_t i = 0; i < cnt; ++i) {
        if (o + 76 > len) return false;
        if (u32(d + o) != rwID_MATRIX || u32(d + o + 12) != rwID_STRUCT)
            return false;
        MtxInstance mi;
        for (int j = 0; j < 12; ++j) mi.m[j] = f32(d + o + 24 + j * 4);
        mi.type = static_cast<std::int32_t>(u32(d + o + 24 + 48));
        items.push_back(mi);
        o += 76;
    }
    return o == len;
}

// ---------------------------------------------------------------------------
// F4 LAPDATA.LUA — text
// ---------------------------------------------------------------------------
namespace {
// Read the integer args of every `call(...)` occurrence, comments stripped.
void scan_calls(const std::string& s, const char* call,
                std::vector<std::vector<int>>* out) {
    std::size_t p = 0;
    std::string key = call;
    while ((p = s.find(key, p)) != std::string::npos) {
        std::size_t after = p + key.size();
        // require the very next non-space char to be '(' (so "Lap_Line" does not
        // also match "Lap_Line_End")
        std::size_t q = after;
        while (q < s.size() && (s[q] == ' ' || s[q] == '\t')) ++q;
        if (q >= s.size() || s[q] != '(') { p = after; continue; }
        std::size_t close = s.find(')', q);
        if (close == std::string::npos) break;
        std::vector<int> args;
        std::string body = s.substr(q + 1, close - q - 1);
        std::size_t a = 0;
        while (a < body.size()) {
            // parse an optionally-signed integer; skip non-numeric tokens
            while (a < body.size() && body[a] != '-' &&
                   (body[a] < '0' || body[a] > '9')) ++a;
            if (a >= body.size()) break;
            std::size_t b = a;
            if (body[b] == '-') ++b;
            std::size_t start = b;
            while (b < body.size() && body[b] >= '0' && body[b] <= '9') ++b;
            if (b > start) args.push_back(std::atoi(body.c_str() + a));
            a = b + 1;
        }
        out->push_back(args);
        p = close + 1;
    }
}
}  // namespace

bool LapData::Parse(const char* text, std::size_t len) {
    // strip Lua line comments (`-- ...`) — training/LAPDATA.LUA documents the
    // call shapes inside comments that must not be read as data.
    std::string s;
    s.reserve(len);
    for (std::size_t i = 0; i < len;) {
        std::size_t eol = i;
        while (eol < len && text[eol] != '\n') ++eol;
        std::string line(text + i, eol - i);
        std::size_t c = line.find("--");
        if (c != std::string::npos) line.resize(c);
        s += line;
        s += '\n';
        i = eol + 1;
    }
    std::vector<std::vector<int>> v;
    v.clear(); scan_calls(s, "Lap_Variations", &v);
    if (!v.empty() && !v[0].empty()) lap_variations = v[0][0];
    v.clear(); scan_calls(s, "Lap_Line", &v);
    for (auto& a : v)
        if (!a.empty() && a[0] >= 0) lap_lines.push_back(a[0]);
    v.clear(); scan_calls(s, "Safe_Start_Lines", &v);
    for (auto& a : v)
        if (a.size() == 2) safe_start_lines.emplace_back(a[0], a[1]);
    v.clear(); scan_calls(s, "Split_Sector", &v);
    for (auto& a : v)
        if (a.size() == 2) split_sectors.emplace_back(a[0], a[1]);
    return valid();
}

bool LapData::is_lap_line(int gate) const {
    for (int g : lap_lines)
        if (g == gate) return true;
    return false;
}

}  // namespace Track
}  // namespace mashed_re
