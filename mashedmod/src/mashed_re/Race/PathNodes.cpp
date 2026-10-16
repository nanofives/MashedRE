// Mashed RE — D-11072 leg P1: the original's static path data (course block 0x00657498).
//
// The original builds this block at track load (FUN_00426e10 -> FUN_0047a0f0 "LAPDATA.LUA",
// then FUN_00478660(course, AI world), whose tail calls FUN_004783f0). The standalone has never
// written it, so FUN_00408610's per-car path progress (0x008a96e8) had nothing to project onto.
// This TU writes the block from the port's own parse of the same AI BSP and LAPDATA.LUA.
//
// Layout (course base C = 0x00657498 = *(0x006bf1d8) during the LAPDATA run):
//   C+0x00000  float dist [var*0x200 + node]        FUN_004783f0 0x004784c0
//   C+0x06000  float angle[var*0x200 + node]        FUN_004783f0 0x004785a8
//   C+0x0c1c0  node records, stride 0x4c (= 0x00663658): dir +0x00, centre +0x0c,
//              corners +0x18/+0x24/+0x30/+0x3c, +0x48 untouched (0)
//   C+0x159c0  route table (= 0x0066ce58)            Lap_Line / Lap_Line_End / Lap_Line_Change
//   C+0x161c0  u16 pairs (= 0x0066d658)              Safe_Start_Lines
//   C+0x16240  count (= 0x0066d6d8) = AI world numMaterials (world+0x14)
//   C+0x16244  Lap_Variations
//   C+0x16248  safe-start count (= 0x0066d6e0)
//   C+0x1624c  int[6] split polys (= 0x0066d6e4)      Split_Sector
//
// x87 TU (x87_tus.txt): values the bytes keep in ST are `double`, float only at FSTP dword, and the
// build runs under PC_24 (see PathNodes_Build), so ST values carry 24-bit mantissas as in the original.
// PREREG: verify/d3_modes37_20261009/PREREG_PATHPROG.md (gate G-P1, byte compare vs the
// original's live block o_course_t0.bin / o_course_t3.bin).
#include "PathNodes.h"
#include "../Track/TrackWorld.h"
#include "../Math/RwLutTables.h"   // 0x004c39b0 / 0x004c3b30 with the REAL LUT (the global paths use an exact-sqrt fallback)

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <float.h>   // _controlfp_s (x87 precision control, see PathNodes_Build)

extern "C" void  __cdecl RwMatrixInvert(void* out, void* in);           // 0x004c4eb0
namespace mashed_re { namespace Math {
void RwV3dTransformPointsCPU(float* dst, const float* src, int count, const float* m);  // slot +0xc
} }

namespace mashed_re {
namespace Race {

namespace {

constexpr std::uintptr_t kCourse   = 0x00657498u;
constexpr std::size_t    kZeroLen  = 0x16264u;     // FUN_0047a0f0: FUN_004b6520(param_2, 0x16264)
constexpr std::size_t    kDumpLen  = 0x16278u;

inline std::int32_t& CI(std::size_t off) { return *reinterpret_cast<std::int32_t*>(kCourse + off); }
inline float&        CF(std::size_t off) { return *reinterpret_cast<float*>(kCourse + off); }
inline std::uint16_t& CW(std::size_t off) { return *reinterpret_cast<std::uint16_t*>(kCourse + off); }

// _ftol2 (0x004a2c48) truncates; Lua numbers are doubles.
inline std::int32_t Ftol(double d) { return static_cast<std::int32_t>(d); }

// ---- LAPDATA.LUA handlers (registered by FUN_00440bc0) -------------------------------
// The file is a flat list of global calls; the handlers read args by index from the top.
std::int32_t s_routeCursor = 0;                    // DAT_006bf1dc

void Lua_LapVariations(const double* a, int n) {   // 0x0047a720: arg[top]
    if (n < 1) return;
    CI(0x16244) = Ftol(a[n - 1]);
}
void Lua_SplitSector(const double* a, int n) {     // 0x0047a750: C[0x1624c + arg[top-1]*4] = arg[top]
    if (n < 2) return;
    const std::int32_t poly = Ftol(a[n - 1]);
    const std::int32_t s    = Ftol(a[n - 2]);
    CI(0x1624c + static_cast<std::size_t>(s) * 4u) = poly;
}
void Lua_LapLine(const double* a, int n) {         // 0x0047a790
    if (n < 1) return;
    CI(0x159c0 + static_cast<std::size_t>(s_routeCursor) * 4u) = Ftol(a[n - 1]);
    ++s_routeCursor;
}
void Lua_LapLineEnd() {                            // 0x0047a7d0
    CI(0x159c0 + static_cast<std::size_t>(s_routeCursor) * 4u) = -1;
    ++s_routeCursor;
}
void Lua_LapLineChange() {                         // 0x0047a7f0
    CI(0x159c0 + static_cast<std::size_t>(s_routeCursor) * 4u) = 0x100;
    ++s_routeCursor;
}
void Lua_SafeStartLines(const double* a, int n) {  // 0x0047a810
    if (n < 2) return;
    const std::int32_t i = CI(0x16248);
    CW(0x161c0 + static_cast<std::size_t>(i) * 4u) = static_cast<std::uint16_t>(Ftol(a[n - 2]));
    CW(0x161c2 + static_cast<std::size_t>(i) * 4u) = static_cast<std::uint16_t>(Ftol(a[n - 1]));
    CI(0x16248) = i + 1;
}

// Executes the call list in file order. Lua 4 comments are `--` to end of line.
void RunLapData(const char* p, std::size_t len) {
    const char* e = p + len;
    while (p < e) {
        if (p + 1 < e && p[0] == '-' && p[1] == '-') { while (p < e && *p != '\n') ++p; continue; }
        if (!((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z') || *p == '_')) { ++p; continue; }
        const char* id = p;
        while (p < e && ((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z') ||
                         (*p >= '0' && *p <= '9') || *p == '_')) ++p;
        const std::size_t idn = static_cast<std::size_t>(p - id);
        while (p < e && (*p == ' ' || *p == '\t')) ++p;
        if (p >= e || *p != '(') continue;
        ++p;
        double a[8]; int n = 0;
        while (p < e && *p != ')') {
            if ((*p >= '0' && *p <= '9') || *p == '-' || *p == '+' || *p == '.') {
                char buf[64]; std::size_t k = 0;
                while (p < e && k < sizeof(buf) - 1 &&
                       ((*p >= '0' && *p <= '9') || *p == '-' || *p == '+' || *p == '.' ||
                        *p == 'e' || *p == 'E')) buf[k++] = *p++;
                buf[k] = 0;
                if (n < 8) a[n++] = std::strtod(buf, nullptr);
            } else {
                ++p;
            }
        }
        auto is = [&](const char* s) { return std::strlen(s) == idn && std::strncmp(id, s, idn) == 0; };
        if      (is("Lap_Variations"))   Lua_LapVariations(a, n);
        else if (is("Lap_Line"))         Lua_LapLine(a, n);
        else if (is("Lap_Line_End"))     Lua_LapLineEnd();
        else if (is("Lap_Line_Change"))  Lua_LapLineChange();
        else if (is("Safe_Start_Lines")) Lua_SafeStartLines(a, n);
        else if (is("Split_Sector"))     Lua_SplitSector(a, n);
    }
}

// ---- route walkers over C+0x159c0 -----------------------------------------------------
const std::int32_t* RouteGroup(int g) {            // shared skip of FUN_00409300 / FUN_00409330
    const std::int32_t* p = &CI(0x159c0);
    for (; g > 0; --g) {
        while (*p != -1 && *p != -10) ++p;
        ++p;
    }
    return p;
}
int RouteFirst(int g) { return *RouteGroup(g); }   // 0x00409300
int RouteNext(int v, int g) {                      // 0x00409330
    const std::int32_t* p = RouteGroup(g);
    const std::int32_t* q = p;
    std::int32_t it = *p;
    for (;;) {
        if (it == -1 || it == -10) return *p;
        if (q[1] == 0x100) {
            if (v == it) return q[2];
        } else if (it <= v && v < q[1]) {
            return v + 1;
        }
        it = q[1];
        ++q;
    }
}

// ---- FUN_004785e0: normal of the triangle t[0..8], ECX = out, normalised when flag != 0 --
void TriNormal(float* out, const float* t) {
    const double e1x = static_cast<double>(t[3]) - t[0];       // fld [+0xc]  fsub [+0]   (ST)
    const double e1y = static_cast<double>(t[4]) - t[1];       // fld [+0x10] fsub [+4]   (ST)
    const double e1z = static_cast<double>(t[5]) - t[2];       // fld [+0x14] fsub [+8]   (ST)
    const float  e2x = static_cast<float>(static_cast<double>(t[6]) - t[0]);  // 0x004785f9 fstp
    const float  e2y = static_cast<float>(static_cast<double>(t[7]) - t[1]);  // 0x00478603 fstp
    const double e2z = static_cast<double>(t[8]) - t[2];       // ST
    out[0] = static_cast<float>(e2z * e1y - e2y * e1z);        // 0x0047861f
    out[1] = static_cast<float>(e2x * e1z - e2z * e1x);        // 0x0047862d
    out[2] = static_cast<float>(e2y * e1x - e2x * e1y);        // 0x00478640
    Math::LutNormalize(out, out);                                  // 0x0047864b (flag 1)
}

inline bool Eq3(const float* a, const float* b) {               // FUN_004b46b0
    return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
}

// FUN_004b4550 for count 4: running sum stored as float each step, then * (1.0/4).
void Centre4(float* out, const float* v) {
    out[0] = v[0]; out[1] = v[1]; out[2] = v[2];
    for (int i = 1; i < 4; ++i) {
        out[0] = static_cast<float>(static_cast<double>(v[i * 3 + 0]) + out[0]);
        out[1] = static_cast<float>(static_cast<double>(v[i * 3 + 1]) + out[1]);
        out[2] = static_cast<float>(static_cast<double>(out[2]) + v[i * 3 + 2]);
    }
    const double f = 1.0 / 4.0;                                 // _DAT_005cc320 / (float)param_3
    out[0] = static_cast<float>(f * out[0]);
    out[1] = static_cast<float>(f * out[1]);
    out[2] = static_cast<float>(f * out[2]);
}

// FUN_004b5030 / FUN_004b4eb0: the first `want` triangles (sector DFS order = stream order,
// triangle order within a sector) whose material RGBA equals key; 9 floats each.
int ColourKeyTris(const Track::World& w, const std::uint8_t key[4], float* out, int want) {
    int got = 0;
    for (const Track::Sector& s : w.sectors) {
        const std::size_t nt = s.tris.size() / 4;
        for (std::size_t ti = 0; ti < nt; ++ti) {
            const std::uint16_t mat = s.tris[ti * 4 + 0];
            if (mat >= w.materials.size()) continue;
            if (std::memcmp(w.materials[mat].rgba, key, 4) != 0) continue;
            for (int k = 0; k < 3; ++k) {
                const std::uint16_t vi = s.tris[ti * 4 + 1 + k];
                out[got * 9 + k * 3 + 0] = s.verts[vi * 3u + 0];
                out[got * 9 + k * 3 + 1] = s.verts[vi * 3u + 1];
                out[got * 9 + k * 3 + 2] = s.verts[vi * 3u + 2];
            }
            if (++got == want) return got;
        }
    }
    return got;
}

// FUN_00478660 per-node body (0x004786a0..0x00478c90).
void BuildNode(const Track::World& w, int i, float* rec) {
    const std::uint8_t key[4] = { static_cast<std::uint8_t>(i), static_cast<std::uint8_t>(i >> 8), 0, 0xff };
    float m[18] = {};                                           // auStack_60 / auStack_30 (9 used each)
    ColourKeyTris(w, key, m, 2);
    float v[12];                                                // auStack_174: A.v0 A.v1 A.v2 + 4th
    std::memcpy(v, m, 9 * sizeof(float));
    std::memset(v + 9, 0, 3 * sizeof(float));
    for (int k = 0; k < 3; ++k) {                               // 0x00478710..0x00478787
        const float* c = m + 9 + k * 3;
        if (!Eq3(c, v) && !Eq3(c, v + 3) && !Eq3(c, v + 6)) {
            v[9] = c[0]; v[10] = c[1]; v[11] = c[2];
            break;
        }
    }
    float* dir = rec + 0;
    float* ctr = rec + 3;
    TriNormal(dir, v);                                          // 0x00478796 FUN_004785e0(1)
    Centre4(ctr, v);                                            // 0x004787a6 FUN_004b4550(.., 4)

    // matrix: right, up, at = dir, pos = centre (0x004787c4..)
    const double y0 = static_cast<double>(dir[1]) * 0.0;
    float r[3] = { static_cast<float>(dir[2] - y0),
                   static_cast<float>(static_cast<double>(dir[0]) * 0.0 - static_cast<double>(dir[2]) * 0.0),
                   static_cast<float>(y0 - dir[0]) };
    Math::LutNormalize(r, r);
    float u[3] = { static_cast<float>(static_cast<double>(r[2]) * dir[1] - static_cast<double>(r[1]) * dir[2]),
                   static_cast<float>(static_cast<double>(r[0]) * dir[2] - static_cast<double>(r[2]) * dir[0]),
                   static_cast<float>(static_cast<double>(r[1]) * dir[0] - static_cast<double>(r[0]) * dir[1]) };
    Math::LutNormalize(u, u);
    alignas(16) float M[16] = { r[0], r[1], r[2], 0.0f,  u[0], u[1], u[2], 0.0f,
                                dir[0], dir[1], dir[2], 0.0f,  ctr[0], ctr[1], ctr[2], 0.0f };
    alignas(16) float inv[16] = {};
    RwMatrixInvert(inv, M);                                     // flags masked -> cofactor path
    float loc[12];
    Math::RwV3dTransformPointsCPU(loc, v, 4, inv);              // FUN_004c3d90 (device slot +0xc)

    if (i == 0 && std::getenv("MASHED_PATHPROG_DBG")) {
        if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
            std::fprintf(f, "[pathprog] n0 r=%g,%g,%g u=%g,%g,%g\n", r[0], r[1], r[2], u[0], u[1], u[2]);
            for (int k = 0; k < 16; ++k) std::fprintf(f, "%g%s", inv[k], k == 15 ? "\n" : ",");
            for (int k = 0; k < 4; ++k) std::fprintf(f, "[pathprog] v%d=%g,%g,%g loc=%g,%g,%g\n", k,
                v[k*3], v[k*3+1], v[k*3+2], loc[k*3], loc[k*3+1], loc[k*3+2]);
            std::fclose(f);
        }
    }
    float* slot = rec + 6;                                      // puVar5 = record + 0x18
    for (int k = 0; k < 4; ++k) {                               // 0x0047897c.. sign classification
        const float x = loc[k * 3 + 0], y = loc[k * 3 + 1];
        float* dst = nullptr;
        if (x < 0.0f) {
            if (y < 0.0f)      dst = slot + 0;                  // +0x18
            else if (y > 0.0f) dst = slot + 3;                  // +0x24
        } else if (x > 0.0f) {
            if (y > 0.0f)      dst = slot + 6;                  // +0x30
            else if (y < 0.0f) dst = slot + 9;                  // +0x3c
        }
        if (dst) { dst[0] = v[k * 3 + 0]; dst[1] = v[k * 3 + 1]; dst[2] = v[k * 3 + 2]; }
    }
}

// CRT acos (0x004a3384): fpatan(fsqrt((1+x)(1-x)), x). Its CW helper 0x004a70a5 does
// `and edx,0x300 / or edx,0x7f / fldcw`: it KEEPS the caller's precision-control bits and only
// sets the exception masks and round-to-nearest (it skips the fldcw when CW == 0x27f already).
// So under the caller's PC_24 the arithmetic is 24-bit too.
double CrtAcos(double x) {
    double r;
    unsigned short cw, cwc;
    __asm {
        fnstcw cw
        mov    ax, cw
        and    ax, 0x300
        or     ax, 0x7f
        mov    cwc, ax
        fldcw  cwc
        fld    x
        fld1
        fadd   st, st(1)
        fld1
        fsub   st, st(2)
        fmulp  st(1), st
        fsqrt
        fxch   st(1)
        fpatan
        fstp   r
        fldcw  cw
    }
    return r;
}

// FUN_004783f0
void BuildTables() {
    const std::int32_t nvar = CI(0x16244);
    for (std::int32_t var = 0; var < nvar; ++var) {
        const int first = RouteFirst(var);
        int cur = first;
        bool more = true;
        do {
            const int next = RouteNext(cur, var);
            if (next == RouteFirst(var)) more = false;
            const float* rc = &CF(0xc1c0 + static_cast<std::size_t>(cur)  * 0x4cu);
            const float* rn = &CF(0xc1c0 + static_cast<std::size_t>(next) * 0x4cu);
            const double dx = static_cast<double>(rc[3]) - rn[3];       // centre +0xc
            const double dy = static_cast<double>(rc[4]) - rn[4];
            const double dz = static_cast<double>(rc[5]) - rn[5];
            const float  d2 = static_cast<float>((dz * dz + dy * dy) + dx * dx);   // 0x004784ac
            CF((static_cast<std::size_t>(var) * 0x200u + static_cast<std::size_t>(cur)) * 4u) = Math::LutSqrt(d2);

            float n[3] = { rc[0], rc[2], 0.0f };                        // (dir.x, dir.z, 0)
            Math::LutNormalize(n, n);
            double t = (static_cast<double>(n[1]) * 0.0 + n[0]) + static_cast<double>(n[2]) * 0.0;
            if (t < -1.0) t = -1.0;                                     // 0x0047850d / 0x0047851c
            if (t >  1.0) t =  1.0;                                     // 0x00478522 / 0x00478531
            double a = CrtAcos(t) * static_cast<double>(57.29578018188477f);   // 0x00478542
            if (static_cast<double>(n[0]) * 0.0 - n[1] < 0.0) a = -a;   // 0x00478559 / 0x00478566
            if (a <= 0.0) {
                do { a += 360.0; } while (a <= 0.0);                    // 0x00478580
            }
            CF((static_cast<std::size_t>(var) * 0x200u + 0x1800u + static_cast<std::size_t>(cur)) * 4u) =
                static_cast<float>(a);
            cur = next;
        } while (more);
    }
}

std::vector<float>    s_triVerts;   // 9 per triangle
std::vector<unsigned> s_triNode;

}  // namespace

int          PathNodes_TriCount()     { return static_cast<int>(s_triNode.size()); }
const float* PathNodes_Tri(int t)     { return s_triVerts.data() + static_cast<std::size_t>(t) * 9u; }
unsigned     PathNodes_TriNode(int t) { return s_triNode[static_cast<std::size_t>(t)]; }

bool PathProg_Enabled() {
    static const bool s_on = (std::getenv("MASHED_PATHPROG") != nullptr);
    return s_on;
}

bool RacePctOrig_Enabled() {
    static const bool s_on = (std::getenv("MASHED_NO_RACEPCT_FUN408610") == nullptr);   // DEFAULT-ON 2026-10-16 (verify/d3_u9685_20261016); MASHED_RACEPCT_FUN408610 is now a no-op
    return s_on;
}

// [U-9685] True only when the course block was actually built (a track with an AI BSP), so the bridge is not
// stood down on a track where the step has nothing to walk.
bool RacePctOrig_Active() {
    return RacePctOrig_Enabled() && !s_triNode.empty();
}

void PathNodes_Build(const Track::World& ai, const char* lap, std::size_t lapLen) {
    if (!PathProg_Enabled() && !RacePctOrig_Enabled()) return;
    std::memset(reinterpret_cast<void*>(kCourse), 0, kZeroLen);    // FUN_0047a0f0 -> FUN_004b6520
    s_routeCursor = 0;                                              // DAT_006bf1dc = 0
    if (lap && lapLen) RunLapData(lap, lapLen);

    // x87 precision control = PC_24 for the whole build, as in the original (track load runs
    // after D3D9 device creation). MEASURED 2026-10-09 (G-P1): at PC_24 the block is byte-
    // identical on Training and Arctic; at 53 bits 11 node dwords and 10 angles were 1 ULP off.
    unsigned int oldcw = 0;
    _controlfp_s(&oldcw, 0, 0);          // read first: the setting call returns the NEW word (cw_check.txt)
    { unsigned int t; _controlfp_s(&t, _PC_24, _MCW_PC); }
    const int count = static_cast<int>(ai.materials.size());       // world+0x14 (numMaterials)
    CI(0x16240) = count;                                            // FUN_00478660 0x004786xx
    for (int i = 0; i < count; ++i)
        BuildNode(ai, i, &CF(0xc1c0 + static_cast<std::size_t>(i) * 0x4cu));
    BuildTables();                                                  // FUN_004783f0

    s_triVerts.clear(); s_triNode.clear();                          // leg P2 sphere query input
    for (const Track::Sector& s : ai.sectors) {
        const std::size_t nt = s.tris.size() / 4;
        for (std::size_t ti = 0; ti < nt; ++ti) {
            const std::uint16_t mat = s.tris[ti * 4 + 0];
            if (mat >= ai.materials.size()) continue;
            for (int k = 0; k < 3; ++k) {
                const std::uint16_t vi = s.tris[ti * 4 + 1 + k];
                for (int a = 0; a < 3; ++a) s_triVerts.push_back(s.verts[vi * 3u + a]);
            }
            s_triNode.push_back(static_cast<unsigned>(ai.materials[mat].rgba[0]) |
                                (static_cast<unsigned>(ai.materials[mat].rgba[1]) << 8));
        }
    }
    { unsigned int tmp; _controlfp_s(&tmp, oldcw & _MCW_PC, _MCW_PC); }

    if (const char* path = std::getenv("MASHED_PATHPROG_DUMP")) {
        if (std::FILE* f = std::fopen(path, "wb")) {
            std::fwrite(reinterpret_cast<const void*>(kCourse), 1, kDumpLen, f);
            std::fclose(f);
        }
    }
}

// [camribbon] See PathNodes.h. The record is a local 0x4c-byte (19 float) block, zeroed as
// FUN_0047a0f0 zeroes the course block (FUN_004b6520, 0x16264 bytes), so a corner the sign
// classification at 0x0047897c does not reach stays 0, exactly as in the original.
void PathNodes_CameraNodes(const Track::World& ai, std::vector<RaceCamNode>& out) {
    const int count = static_cast<int>(ai.materials.size());       // world+0x14 (numMaterials)
    out.assign(static_cast<std::size_t>(count), RaceCamNode{});
    unsigned int oldcw = 0;
    _controlfp_s(&oldcw, 0, 0);          // PC_24 for the build, as PathNodes_Build (G-P1)
    { unsigned int t; _controlfp_s(&t, _PC_24, _MCW_PC); }
    for (int i = 0; i < count; ++i) {
        float rec[0x4c / sizeof(float)] = {};
        BuildNode(ai, i, rec);
        RaceCamNode& n = out[static_cast<std::size_t>(i)];
        std::memcpy(n.dir, rec + 0x00 / sizeof(float), sizeof(n.dir));   // +0x00  FUN_00426cc0
        std::memcpy(n.c0,  rec + 0x18 / sizeof(float), sizeof(n.c0));    // +0x18  FUN_00426d00(n,0)
        std::memcpy(n.c3,  rec + 0x3c / sizeof(float), sizeof(n.c3));    // +0x3c  FUN_00426d00(n,3)
    }
    { unsigned int tmp; _controlfp_s(&tmp, oldcw & _MCW_PC, _MCW_PC); }
}

}  // namespace Race
}  // namespace mashed_re
