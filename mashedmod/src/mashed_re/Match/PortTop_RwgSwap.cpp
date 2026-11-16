// PORT-UNBLOCK-RWGLOBALS: the MASHED_RWC_RWGSWAP knob. EXE ONLY. Default OFF.
//
// RwGlobals 0x007d3ff8 is written by the RW core engine bring-up on the frame route (MASHED_ORIG_TOP=1
// MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1): RwEngineInit 0x004c32b0 stores &0x007d3ec8 (re/match/s19/004c32b0.c:106) and
// RwEngineOpen 0x004c30b0 replaces it with the instance it allocates through slot +0x108 (re/match/rwl01/004c30b0.c:60),
// the value RwCore_Knob.cpp keeps as RwCore_GlobalsAtInit(). With that pointer live the three matched bodies that read
// it run in place of the exe's bodies:
//   0x004c3b30 FastSqrt    (Math/RwSqrt.cpp)  -> RwCore_004c3b30 (re/match/s19/004c3b30.c, 100%)
//   0x004c3bf0 Vec2Length  (Math/RwV2d.cpp)   -> RwCore_004c3bf0 (re/match/s19/004c3bf0.c, 100%)
//   0x00403160 sub-mode 0xb viewport, the matched HUD dispatch's callee (Match/MatchS02_0040dfc0.cpp, an empty
//              stand-in before) -> RwCore_00403160 (re/match/s01/00403160.c, 100%)
// The knob is live only when MASHED_RWC_RWGSWAP=1, RwCore_FrameRoute() is on, and [0x007d3ff8] equals the engine
// instance RwCore_GlobalsAtInit() recorded. Before that (and on the default path) the exe bodies run as before.
// The first live call runs a self-test (log line "[RWGSWAP] self-test"): RwGlobals instance, the sqrt table pointer
// *(RwGlobals + [0x007d3ffc]), and the matched bodies against the exe bodies on a fixed input set, bit for bit.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <windows.h>
#include <cstdint>
#include <cstring>

extern "C" {
bool RwCore_FrameRoute(void);          // RwCore/RwCore_Knob.cpp
void* RwCore_GlobalsAtInit(void);      // RwCore/RwCore_Knob.cpp
void RwCore_Log(const char* fmt, ...); // RwCore/RwCore_Knob.cpp
float RwCore_004c3b30(float num);      // RwCore/RwCore_004c3b30.cpp
struct RwV2d;
float RwCore_004c3bf0(const RwV2d* in);   // RwCore/RwCore_004c3bf0.cpp
void RwCore_00403160(void);               // RwCore/RwCore_00403160.cpp
float __cdecl FastSqrtExe(float x);           // Math/RwSqrt.cpp (the exe body)
float __cdecl Vec2LengthExe(const float* v);  // Math/RwV2d.cpp (the exe body)

int g_portTopRwgSwapEnv = -1;   // -1 unread, 0 off, 1 knob set and frame route on
}

namespace {
inline std::uint32_t U32(std::uint32_t va) { return *reinterpret_cast<const volatile std::uint32_t*>(static_cast<std::uintptr_t>(va)); }
inline std::uint32_t Bits(float f) { std::uint32_t b; std::memcpy(&b, &f, 4); return b; }
long s_calls[3];   // FastSqrt, Vec2Length, 0x00403160
bool s_tested = false;

void Note(int k, const char* name) {
    const long n = ++s_calls[k];
    if ((n & (n - 1)) == 0 && n <= 0x100000) RwCore_Log("[RWGSWAP] %s routed to the matched body: call %ld", name, n);
}

void SelfTest() {
    s_tested = true;
    const std::uint32_t g = U32(0x007d3ff8u);
    const std::uint32_t off = U32(0x007d3ffcu);
    const std::uint32_t tab = U32(g + off);
    int fail = 0, checks = 0;
    ++checks; if (g == 0) ++fail;
    ++checks; if (g != static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(RwCore_GlobalsAtInit()))) ++fail;
    ++checks; if (tab == 0) ++fail;
    static const float xs[] = {0.0f, 1.0f, 2.0f, 0.5f, 3.0f, 10.0f, 100.0f, 12345.678f, 1e-6f, 1e6f, 0.333333f, 640.0f,
                               480.0f, 7.25f, 1.0e-30f, 3.0e30f};
    int dsq = 0, dv2 = 0;
    for (float x : xs) {
        ++checks;
        if (Bits(RwCore_004c3b30(x)) != Bits(FastSqrtExe(x))) { ++fail; ++dsq; }
    }
    for (unsigned i = 0; i + 1 < sizeof(xs) / sizeof(xs[0]); ++i) {
        const float v[2] = {xs[i], -xs[i + 1]};
        ++checks;
        if (Bits(RwCore_004c3bf0(reinterpret_cast<const RwV2d*>(v))) != Bits(Vec2LengthExe(v))) { ++fail; ++dv2; }
    }
    RwCore_Log("[RWGSWAP] RwGlobals [0x007d3ff8] = %08X (template 007D3EC8, engine instance %08X), [0x007d3ffc] = 0x%X, "
               "sqrt table *(RwGlobals + [0x007d3ffc]) = %08X; matched vs exe bodies: FastSqrt %d diffs, Vec2Length %d diffs",
               g, static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(RwCore_GlobalsAtInit())), off, tab, dsq, dv2);
    RwCore_Log("[RWGSWAP] self-test %s (%d of %d checks failed)", fail ? "FAIL" : "PASS", fail, checks);
}
}  // namespace

extern "C" bool PortTop_RwgSwapLive(void) {
    if (g_portTopRwgSwapEnv < 0) {
        char v[8] = {};
        const bool knob = GetEnvironmentVariableA("MASHED_RWC_RWGSWAP", v, sizeof(v)) > 0 && v[0] && v[0] != '0';
        g_portTopRwgSwapEnv = (knob && RwCore_FrameRoute()) ? 1 : 0;
        if (knob) RwCore_Log("[RWGSWAP] MASHED_RWC_RWGSWAP=1, frame route %s", g_portTopRwgSwapEnv ? "on" : "off (knob inert)");
    }
    if (!g_portTopRwgSwapEnv) return false;
    void* const inst = RwCore_GlobalsAtInit();
    if (!inst || U32(0x007d3ff8u) != static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(inst))) return false;
    if (!s_tested) SelfTest();
    return true;
}

extern "C" float PortTop_RwgSqrt(float x) { Note(0, "FastSqrt 0x004c3b30"); return RwCore_004c3b30(x); }
extern "C" float PortTop_RwgV2dLength(const float* v) {
    Note(1, "Vec2Length 0x004c3bf0");
    return RwCore_004c3bf0(reinterpret_cast<const RwV2d*>(v));
}
extern "C" bool PortTop_Rwg00403160(void) {
    if (!PortTop_RwgSwapLive()) return false;
    Note(2, "0x00403160");
    RwCore_00403160();
    return true;
}
#endif  // MASHED_STANDALONE
