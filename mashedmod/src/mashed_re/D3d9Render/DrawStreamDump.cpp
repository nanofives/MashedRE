// Mashed RE — env-gated draw-stream dump implementation.
// See DrawStreamDump.h for the schema and usage.

#include "DrawStreamDump.h"

#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace mashed_re {
namespace D3d9Render {

namespace {

constexpr int   kVertStride   = 0x1c;
constexpr int   kMaxRets      = 6;
constexpr char  kOutPath[]    = "log/drawstream_re.json";
constexpr char  kLogPath[]    = "mashed_re.log";

struct DrawRec {
    std::string   vhex;                 // hex of count*0x1c source bytes
    std::uint32_t rets[kMaxRets];       // module-relative retaddrs, 0-padded
    int           nrets;
    int           tex_handle;
    int           alpha_on;
    int           src_blend;
    int           dst_blend;
};

struct FrameRec {
    int                  index;
    std::vector<DrawRec> draws;
};

int  g_state       = -1;   // -1 unparsed, 0 disabled, 1 armed
int  g_frameStart  = 0;
int  g_frameEnd    = 0;
int  g_frameCount  = -1;   // increments on every OnFrameBegin
bool g_written     = false;
std::vector<FrameRec> g_frames;

void ParseEnvOnce() {
    if (g_state != -1) return;
    char buf[32] = {};
    if (GetEnvironmentVariableA("MASHED_DBG_DRAWSTREAM", buf, sizeof(buf)) == 0) {
        g_state = 0;
        return;
    }
    int a = 0, b = 0;
    if (std::sscanf(buf, "%d:%d", &a, &b) == 2 && a > 1 && b >= a) {
        g_frameStart = a; g_frameEnd = b;
    } else if (std::sscanf(buf, "%d", &a) == 1 && a > 1) {
        g_frameStart = a; g_frameEnd = a;
    } else {
        // "1" / non-numeric truthy value: single frame at the BBDUMP timing.
        g_frameStart = 200; g_frameEnd = 200;
    }
    g_state = 1;
}

bool Capturing() {
    return g_state == 1 && !g_written &&
           g_frameCount >= g_frameStart && g_frameCount <= g_frameEnd;
}

void WriteOut() {
    g_written = true;
    CreateDirectoryA("log", nullptr);
    std::FILE* f = std::fopen(kOutPath, "w");
    if (!f) return;
    std::fputs("{\n", f);
    for (std::size_t fi = 0; fi < g_frames.size(); ++fi) {
        const FrameRec& fr = g_frames[fi];
        std::fprintf(f, " \"f%d\": [", fr.index);
        for (std::size_t di = 0; di < fr.draws.size(); ++di) {
            const DrawRec& d = fr.draws[di];
            std::fprintf(f, "%s\n  {\"v\": \"%s\", \"r\": [",
                         di ? "," : "", d.vhex.c_str());
            for (int ri = 0; ri < d.nrets; ++ri)
                std::fprintf(f, "%s\"0x%x\"", ri ? ", " : "", d.rets[ri]);
            std::fprintf(f, "], \"s\": [%d, %d, %d, %d]}",
                         d.tex_handle, d.alpha_on, d.src_blend, d.dst_blend);
        }
        std::fprintf(f, "\n ]%s\n", fi + 1 < g_frames.size() ? "," : "");
    }
    std::fputs("}\n", f);
    std::fclose(f);

    std::size_t total = 0;
    for (std::size_t fi = 0; fi < g_frames.size(); ++fi)
        total += g_frames[fi].draws.size();
    if (std::FILE* lf = std::fopen(kLogPath, "a")) {
        std::fprintf(lf, "DRAWSTREAM wrote %u frames (%u draws) f%d..f%d -> %s\n",
                     static_cast<unsigned>(g_frames.size()),
                     static_cast<unsigned>(total),
                     g_frameStart, g_frameEnd, kOutPath);
        std::fclose(lf);
    }
    g_frames.clear();
}

}  // namespace

void DrawStreamDump_OnFrameBegin() {
    ParseEnvOnce();
    if (g_state == 0 || g_written) return;
    ++g_frameCount;
    if (g_frameCount > g_frameEnd) {
        if (!g_frames.empty()) WriteOut();
        return;
    }
    if (Capturing()) {
        FrameRec fr;
        fr.index = g_frameCount;
        g_frames.push_back(fr);
    }
}

void DrawStreamDump_OnDraw(const void* verts, int count, int tex_handle,
                           bool alpha_on, int src_blend, int dst_blend,
                           void* bridge_ret) {
    if (!Capturing() || g_frames.empty() || !verts || count < 1 || count > 64)
        return;

    DrawRec d;
    const std::uint8_t* p = static_cast<const std::uint8_t*>(verts);
    const int nbytes = count * kVertStride;
    d.vhex.resize(static_cast<std::size_t>(nbytes) * 2);
    static const char hexd[] = "0123456789abcdef";
    for (int i = 0; i < nbytes; ++i) {
        d.vhex[i * 2]     = hexd[p[i] >> 4];
        d.vhex[i * 2 + 1] = hexd[p[i] & 0xf];
    }

    const std::uintptr_t base =
        reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
    std::memset(d.rets, 0, sizeof(d.rets));
    d.nrets = 0;
    if (bridge_ret) {
        d.rets[d.nrets++] = static_cast<std::uint32_t>(
            reinterpret_cast<std::uintptr_t>(bridge_ret) - base);
    }
    // Best-effort deeper chain: x86 /O2 (/Oy) can defeat the EBP walk; the
    // bridge_ret above is the guaranteed first frame.
    void* frames[kMaxRets + 2] = {};
    const USHORT got = CaptureStackBackTrace(2, kMaxRets, frames, nullptr);
    for (USHORT i = 0; i < got && d.nrets < kMaxRets; ++i) {
        const std::uintptr_t a = reinterpret_cast<std::uintptr_t>(frames[i]);
        if (!a) break;
        const std::uint32_t rel = static_cast<std::uint32_t>(a - base);
        if (d.nrets && d.rets[d.nrets - 1] == rel) continue;
        d.rets[d.nrets++] = rel;
    }

    d.tex_handle = tex_handle;
    d.alpha_on   = alpha_on ? 1 : 0;
    d.src_blend  = src_blend;
    d.dst_blend  = dst_blend;
    g_frames.back().draws.push_back(d);
}

}  // namespace D3d9Render
}  // namespace mashed_re
