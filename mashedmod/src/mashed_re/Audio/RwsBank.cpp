// RwsBank impl — see RwsBank.h. Self-contained chunk walk (12-byte RW stream
// headers: [u32 type][u32 size][u32 version]).
#include "RwsBank.h"
#include <cstdio>
#include <cstring>
#include <cctype>

namespace mashed_re {
namespace Audio {

namespace {
inline std::uint32_t u32(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0]) | (static_cast<std::uint32_t>(p[1]) << 8) |
           (static_cast<std::uint32_t>(p[2]) << 16) | (static_cast<std::uint32_t>(p[3]) << 24);
}
}  // namespace

bool RwsBankLoad(const char* path, std::vector<RwsWave>& out, const char* log_path) {
    out.clear();
    std::FILE* f = std::fopen(path, "rb");
    if (!f) return false;
    std::fseek(f, 0, SEEK_END);
    long n = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    if (n < 24) { std::fclose(f); return false; }
    std::vector<std::uint8_t> d(static_cast<size_t>(n));
    size_t got = std::fread(d.data(), 1, d.size(), f);
    std::fclose(f);
    if (got != d.size()) return false;

    const std::uint8_t* b = d.data();
    const size_t N = d.size();
    if (u32(b) != 0x809) return false;           // top must be the audio wrapper
    const std::uint32_t topSize = u32(b + 4);
    const size_t topEnd = (12 + topSize <= N) ? (12 + topSize) : N;

    // children of 0x809
    size_t off = 12;
    while (off + 12 <= topEnd) {
        const std::uint32_t t = u32(b + off);
        const std::uint32_t s = u32(b + off + 4);
        const size_t cd = off + 12;
        if (t == 0x80C) {
            const size_t dataEnd = (cd + s <= N) ? (cd + s) : N;
            if (cd + 4 > dataEnd) break;
            // [u32 waveCount] then 0x802 wave wrappers
            size_t wo = cd + 4;
            while (wo + 12 <= dataEnd) {
                const std::uint32_t wt = u32(b + wo);
                const std::uint32_t ws = u32(b + wo + 4);
                const size_t wd = wo + 12;
                if (wt == 0x802) {
                    RwsWave w;
                    const size_t wEnd = (wd + ws <= N) ? (wd + ws) : N;
                    size_t so = wd;
                    while (so + 12 <= wEnd) {
                        const std::uint32_t st = u32(b + so);
                        const std::uint32_t ss = u32(b + so + 4);
                        const size_t sd = so + 12;
                        if (st == 0x803 && sd + 116 <= N) {
                            w.rate = u32(b + sd + 4);
                            // ASCIIZ name at +112
                            const char* nm = reinterpret_cast<const char*>(b + sd + 112);
                            size_t maxn = (sd + ss <= N ? ss : (N - sd));
                            size_t lim = maxn > 112 ? maxn - 112 : 0;
                            std::string s2;
                            for (size_t i = 0; i < lim && nm[i]; ++i) s2.push_back(nm[i]);
                            w.name = s2;
                        } else if (st == 0x804) {
                            const size_t pe = (sd + ss <= N) ? (sd + ss) : N;
                            w.pcm.assign(b + sd, b + pe);
                        }
                        so += 12 + ss;
                    }
                    if (w.rate == 0) w.rate = 22050;
                    out.push_back(std::move(w));
                }
                wo += 12 + ws;
            }
        }
        off += 12 + s;
    }

    if (log_path) {
        if (std::FILE* lf = std::fopen(log_path, "a")) {
            std::fprintf(lf, "[audio] RwsBankLoad %s: %zu waves\n", path, out.size());
            std::fclose(lf);
        }
    }
    return !out.empty();
}

const RwsWave* RwsBankFind(const std::vector<RwsWave>& waves, const char* nameSub) {
    if (!nameSub) return nullptr;
    std::string needle(nameSub);
    for (auto& c : needle) c = static_cast<char>(std::tolower((unsigned char)c));
    for (const auto& w : waves) {
        std::string hay(w.name);
        for (auto& c : hay) c = static_cast<char>(std::tolower((unsigned char)c));
        if (hay.find(needle) != std::string::npos) return &w;
    }
    return nullptr;
}

}  // namespace Audio
}  // namespace mashed_re
