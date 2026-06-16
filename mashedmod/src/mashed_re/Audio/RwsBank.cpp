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

namespace {
// IMA ADPCM tables.
const int kImaStep[89] = {
    7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,50,55,60,66,73,80,
    88,97,107,118,130,143,157,173,190,209,230,253,279,307,337,371,408,449,494,
    544,598,658,724,796,876,963,1060,1166,1282,1411,1552,1707,1878,2066,2272,
    2499,2749,3024,3327,3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,9493,
    10442,11487,12635,13899,15289,16818,18500,20350,22385,24623,27086,29794,32767};
const int kImaIdx[16] = {-1,-1,-1,-1,2,4,6,8,-1,-1,-1,-1,2,4,6,8};
}  // namespace

bool RwsStreamDecode(const char* path, std::vector<std::int16_t>& pcmOut,
                     std::uint32_t& rateOut, std::size_t maxSamples,
                     const char* log_path) {
    pcmOut.clear();
    rateOut = 44100;
    std::FILE* f = std::fopen(path, "rb");
    if (!f) return false;
    std::fseek(f, 0, SEEK_END); long n = std::ftell(f); std::fseek(f, 0, SEEK_SET);
    if (n < 24) { std::fclose(f); return false; }
    std::vector<std::uint8_t> d(static_cast<size_t>(n));
    if (std::fread(d.data(), 1, d.size(), f) != d.size()) { std::fclose(f); return false; }
    std::fclose(f);
    const std::uint8_t* b = d.data(); const size_t N = d.size();
    if (u32(b) != 0x80D) return false;

    // rate: scan the 0x80e header (first ~8KB) for an exact 44100/22050 u32.
    for (size_t i = 12; i + 4 <= N && i < 12000; ++i) {
        std::uint32_t v = u32(b + i);
        if (v == 44100 || v == 22050 || v == 32000 || v == 48000) { rateOut = v; break; }
    }
    // payload = the largest leaf chunk (the continuous ADPCM stream).
    size_t bestOff = 0, bestLen = 0;
    size_t off = 12;
    while (off + 12 <= N) {
        const std::uint32_t t = u32(b + off);
        std::uint32_t s = u32(b + off + 4);
        const size_t cd = off + 12;
        size_t avail = (cd <= N) ? (N - cd) : 0;
        if (s > avail) s = static_cast<std::uint32_t>(avail);   // clamp bogus sizes
        if (t == 0x80E && off == 12) { off = cd + s; continue; } // skip the header
        if (s > bestLen) { bestLen = s; bestOff = cd; }
        off = cd + s;
    }
    if (bestLen < 1024) return false;

    // continuous IMA ADPCM decode (mono).
    const std::size_t cap = maxSamples ? maxSamples : (bestLen * 2);
    pcmOut.reserve(cap < bestLen * 2 ? cap : bestLen * 2);
    int pred = 0, idx = 0;
    for (size_t i = 0; i < bestLen; ++i) {
        const std::uint8_t byte = b[bestOff + i];
        for (int half = 0; half < 2; ++half) {
            const int nib = half ? (byte >> 4) : (byte & 0xF);
            int step = kImaStep[idx], diff = step >> 3;
            if (nib & 4) diff += step;
            if (nib & 2) diff += step >> 1;
            if (nib & 1) diff += step >> 2;
            pred = (nib & 8) ? pred - diff : pred + diff;
            if (pred < -32768) pred = -32768; else if (pred > 32767) pred = 32767;
            idx += kImaIdx[nib]; if (idx < 0) idx = 0; else if (idx > 88) idx = 88;
            pcmOut.push_back(static_cast<std::int16_t>(pred));
            if (maxSamples && pcmOut.size() >= maxSamples) i = bestLen;  // stop
        }
    }
    if (log_path) {
        if (std::FILE* lf = std::fopen(log_path, "a")) {
            std::fprintf(lf, "[audio] RwsStreamDecode %s: %zu samples @%u Hz (IMA ADPCM)\n",
                         path, pcmOut.size(), rateOut);
            std::fclose(lf);
        }
    }
    return !pcmOut.empty();
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
