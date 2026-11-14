// RVA-LINT: ORIGTOP-ONLY
// Mashed RE - whole data image loader (IMG-2-LOADER, decision L-149 (a), Mariano 2026-11-13). Exe only.
// Spec: verify/portstate_20261113/RESULT_PORTSTATE-IMAGE.md sections 2 and 5 (IMG-2). Format v1 and extractor:
// re/tools/image/mashed_image.py, re/tools/image/extract_image.py (IMG-1-EXTRACT).
//
// Knob MASHED_PORTSTATE_IMAGE=1 (default OFF), consulted only on WinMain's MASHED_ORIG_TOP branch. With it ON this
// copies the three file-backed ranges of the anchored MASHED.exe.unpatched (.rdata 0x005cc000+0x1d044, .data
// 0x005ea000+0x4d000, _rwdseg 0x00915000+0x8) into the B17 pad, then zeroes the IAT range 0x005cc000..0x005cc2e8
// (its file bytes are hint/name RVAs, not addresses: spec section 3). The per-address seeds are then skipped
// (PortStateSeed, PortTop_RdataSeed, PortTop_DInputSeed's two byte entries, PortTopCodePtr's CP_SEED) and only
// witnessed here, against memory, right after the copy and before any game code:
//   - PortStateSeed: FNV-1a over (addr, len, bytes) read from memory must equal MASHED_PORTSTATE_SEED_FNV1A;
//   - Rdata / DInput / CP_SEED: every entry's memory bytes equal the table bytes;
//   - CP_FIX: every slot holds its original target (the dword the binding rewrites when MASHED_OT_CODEPTR=1).
// The default (hand) path never calls this file (spec section 5, "Default path").
//
// No original byte is compiled into this TU's logic except the witness tables already in the tree (they retire with
// the per-address seeds, spec section 5 "Retirement condition").

#include "PortStateImage.h"
#include "PortStateSeed.h"

#include <windows.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "PortStateSeed.inc"

extern "C" char g_b17_low_arena_pad[0x00A00000];   // exe_main.cpp B17

namespace {

// ---- witness tables (same includes as their consumers; only read here) ----
struct WitnessSeed { unsigned va; unsigned n; const char* bytes; };
const WitnessSeed kRdataWitness[] = {
#include "../Match/PortTop_RdataSeed.inc"
};
const WitnessSeed kDInputWitness[] = {
#include "../Match/PortTop_DInputSeed.inc"
};

// ---- held log (Apply runs before the exe truncates mashed_re.log) ----
char s_pend[8192];
std::size_t s_pendLen = 0;

void ImgLog(const char* fmt, ...) {
    char line[600];
    va_list ap;
    va_start(ap, fmt);
    const int n = std::vsnprintf(line, sizeof(line), fmt, ap);
    va_end(ap);
    if (n < 0) return;
    const int m = std::snprintf(s_pend + s_pendLen, sizeof(s_pend) - s_pendLen, "[portstate-image] %s\n", line);
    if (m > 0 && s_pendLen + static_cast<std::size_t>(m) < sizeof(s_pend)) s_pendLen += static_cast<std::size_t>(m);
}

// ---- SHA-256 (FIPS 180-4), for the source and trailer checks ----
struct Sha256 {
    std::uint32_t h[8];
    unsigned char buf[64];
    std::uint64_t len;
    std::size_t fill;
};
const std::uint32_t kK[64] = {
    0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu, 0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u,
    0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u,
    0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu, 0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
    0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u,
    0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
    0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
    0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
    0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u, 0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u,
};
inline std::uint32_t Rotr(std::uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }
void ShaBlock(Sha256& s, const unsigned char* p) {
    std::uint32_t w[64];
    for (int i = 0; i < 16; ++i)
        w[i] = (std::uint32_t(p[4 * i]) << 24) | (std::uint32_t(p[4 * i + 1]) << 16) | (std::uint32_t(p[4 * i + 2]) << 8) | p[4 * i + 3];
    for (int i = 16; i < 64; ++i) {
        const std::uint32_t s0 = Rotr(w[i - 15], 7) ^ Rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
        const std::uint32_t s1 = Rotr(w[i - 2], 17) ^ Rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }
    std::uint32_t a = s.h[0], b = s.h[1], c = s.h[2], d = s.h[3], e = s.h[4], f = s.h[5], g = s.h[6], h = s.h[7];
    for (int i = 0; i < 64; ++i) {
        const std::uint32_t t1 = h + (Rotr(e, 6) ^ Rotr(e, 11) ^ Rotr(e, 25)) + ((e & f) ^ (~e & g)) + kK[i] + w[i];
        const std::uint32_t t2 = (Rotr(a, 2) ^ Rotr(a, 13) ^ Rotr(a, 22)) + ((a & b) ^ (a & c) ^ (b & c));
        h = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
    }
    s.h[0] += a; s.h[1] += b; s.h[2] += c; s.h[3] += d; s.h[4] += e; s.h[5] += f; s.h[6] += g; s.h[7] += h;
}
void Sha256Digest(const unsigned char* p, std::size_t n, unsigned char out[32]) {
    Sha256 s = {{0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au, 0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u}, {}, 0, 0};
    s.len = static_cast<std::uint64_t>(n) * 8u;
    std::size_t i = 0;
    for (; i + 64 <= n; i += 64) ShaBlock(s, p + i);
    unsigned char tail[128] = {};
    const std::size_t rem = n - i;
    std::memcpy(tail, p + i, rem);
    tail[rem] = 0x80;
    const std::size_t tl = (rem + 9 <= 64) ? 64 : 128;
    for (int k = 0; k < 8; ++k) tail[tl - 1 - k] = static_cast<unsigned char>(s.len >> (8 * k));
    ShaBlock(s, tail);
    if (tl == 128) ShaBlock(s, tail + 64);
    for (int k = 0; k < 8; ++k) {
        out[4 * k] = static_cast<unsigned char>(s.h[k] >> 24); out[4 * k + 1] = static_cast<unsigned char>(s.h[k] >> 16);
        out[4 * k + 2] = static_cast<unsigned char>(s.h[k] >> 8); out[4 * k + 3] = static_cast<unsigned char>(s.h[k]);
    }
}

std::uint32_t Fnv(std::uint32_t h, const unsigned char* p, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) { h ^= p[i]; h *= 16777619u; }
    return h;
}

// Anchor: original\MASHED.exe.unpatched SHA-256 BDCAE093...3C0E (Mashed CLAUDE.md, version anchor).
const unsigned char kAnchorSha[32] = {
    0xBD, 0xCA, 0xE0, 0x93, 0xA3, 0x0F, 0xBF, 0x22, 0x6B, 0xDD, 0x85, 0x2B, 0x9C, 0x36, 0x79, 0x8A,
    0x98, 0x7A, 0xEE, 0x33, 0xB3, 0xAE, 0x82, 0xBF, 0x74, 0x04, 0xB0, 0x33, 0x6E, 0xFD, 0x3C, 0x0E,
};
const std::uint32_t kIatLo = 0x005cc000u, kIatHi = 0x005cc2e8u;   // IAT data directory, size 0x2e8 (spec section 1)
const std::uint32_t kImageLo = 0x005cc000u;                         // first byte after the trapped code range (spec section 4)
const std::uint32_t kHdrSize = 0x2cu, kRangeHdr = 24u, kMaxRanges = 8u;

bool s_applied = false;

bool FileExists(const char* p) {
    const DWORD a = GetFileAttributesA(p);
    return a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY);
}

// Lookup order of spec section 2: MASHED_IMAGE, the exe's folder, MASHED_ROOT\original, then original\ under each
// parent of the exe's folder (the exe_main.cpp marker walk shape: exe name + 6 parent hops).
bool FindImage(char* out, std::size_t cap) {
    char env[MAX_PATH] = {};
    if (GetEnvironmentVariableA("MASHED_IMAGE", env, sizeof(env)) > 0) {
        std::snprintf(out, cap, "%s", env);
        return FileExists(out);   // an explicit path is the only candidate
    }
    char dir[MAX_PATH] = {};
    const DWORD n = GetModuleFileNameA(nullptr, dir, MAX_PATH);
    if (n > 0 && n < MAX_PATH) {
        if (char* s = std::strrchr(dir, '\\')) *s = '\0';
        if (std::snprintf(out, cap, "%s\\mashed_image.bin", dir) > 0 && FileExists(out)) return true;
    }
    if (GetEnvironmentVariableA("MASHED_ROOT", env, sizeof(env)) > 0 &&
        std::snprintf(out, cap, "%s\\original\\mashed_image.bin", env) > 0 && FileExists(out)) return true;
    for (int up = 0; up < 6 && dir[0]; ++up) {
        if (std::snprintf(out, cap, "%s\\original\\mashed_image.bin", dir) > 0 && FileExists(out)) return true;
        char* s = std::strrchr(dir, '\\');
        if (!s) break;
        *s = '\0';
    }
    out[0] = '\0';
    return false;
}

std::uint32_t Rd32(const unsigned char* p) { std::uint32_t v; std::memcpy(&v, p, 4); return v; }

bool Refuse(const char* why, const char* path) {
    ImgLog("REFUSED(%s) %s: nothing written; the per-address seed runs instead", why, path ? path : "");
    return false;
}

} // namespace

namespace mashed_re {
namespace Compat {

bool PortStateImage_On() {
    static const bool s_on = [] {
        char v[8] = {};
        return GetEnvironmentVariableA("MASHED_PORTSTATE_IMAGE", v, sizeof(v)) > 0 && v[0] && v[0] != '0';
    }();
    return s_on;
}

bool PortStateImage_Applied() { return s_applied; }

bool PortStateImage_Apply() {
    static bool s_done = false;
    if (s_done || !PortStateImage_On()) return s_applied;
    s_done = true;
    char path[MAX_PATH] = {};
    if (!FindImage(path, sizeof(path))) return Refuse("missing", path[0] ? path : "mashed_image.bin");

    HANDLE h = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return Refuse("open", path);
    LARGE_INTEGER sz = {};
    if (!GetFileSizeEx(h, &sz) || sz.QuadPart < kHdrSize + 32 || sz.QuadPart > 0x01000000) {
        CloseHandle(h);
        return Refuse("size", path);
    }
    const std::size_t n = static_cast<std::size_t>(sz.QuadPart);
    unsigned char* img = static_cast<unsigned char*>(std::malloc(n));
    DWORD got = 0;
    const BOOL ok = img && ReadFile(h, img, static_cast<DWORD>(n), &got, nullptr) && got == n;
    CloseHandle(h);
    if (!ok) { std::free(img); return Refuse("read", path); }

    struct R { std::uint32_t va, len, off, fnv; char name[9]; };
    R r[kMaxRanges] = {};
    std::uint32_t count = 0;
    const char* why = nullptr;
    unsigned char dig[32];
    const std::uintptr_t lo = reinterpret_cast<std::uintptr_t>(&g_b17_low_arena_pad[0]);
    const std::uintptr_t hi = lo + sizeof(g_b17_low_arena_pad);
    if (std::memcmp(img, "MSIM", 4) != 0) why = "magic";
    else if (Rd32(img + 4) != 1u) why = "version";
    else if (std::memcmp(img + 8, kAnchorSha, 32) != 0) why = "source-sha256";
    else {
        Sha256Digest(img, n - 32, dig);
        if (std::memcmp(dig, img + n - 32, 32) != 0) why = "trailer-sha256";
    }
    if (!why) {
        count = Rd32(img + 0x28);
        if (count == 0 || count > kMaxRanges || kHdrSize + count * kRangeHdr > n - 32) why = "range-count";
    }
    for (std::uint32_t i = 0; !why && i < count; ++i) {
        const unsigned char* q = img + kHdrSize + i * kRangeHdr;
        r[i].va = Rd32(q); r[i].len = Rd32(q + 4); r[i].off = Rd32(q + 8); r[i].fnv = Rd32(q + 12);
        std::memcpy(r[i].name, q + 16, 8);
        if (r[i].off > n - 32 || r[i].len > n - 32 - r[i].off) why = "range-out-of-file";
        else if (Fnv(2166136261u, img + r[i].off, r[i].len) != r[i].fnv) why = "fnv";
        else if (r[i].va < kImageLo || r[i].va < lo || static_cast<std::uintptr_t>(r[i].va) + r[i].len > hi) why = "outside-pad";
    }
    if (why) { std::free(img); return Refuse(why, path); }

    // Before the copy: nonzero bytes already in the ranges (anything written before WinMain would be overwritten).
    unsigned long long pre = 0, bytes = 0;
    for (std::uint32_t i = 0; i < count; ++i) {
        const unsigned char* m = reinterpret_cast<const unsigned char*>(static_cast<std::uintptr_t>(r[i].va));
        for (std::uint32_t k = 0; k < r[i].len; ++k) pre += (m[k] != 0);
    }
    for (std::uint32_t i = 0; i < count; ++i) {
        std::memcpy(reinterpret_cast<void*>(static_cast<std::uintptr_t>(r[i].va)), img + r[i].off, r[i].len);
        bytes += r[i].len;
        ImgLog("range %-8s va 0x%08X len 0x%06X fnv1a32 0x%08X copied", r[i].name, r[i].va, r[i].len, r[i].fnv);
    }
    std::memset(reinterpret_cast<void*>(static_cast<std::uintptr_t>(kIatLo)), 0, kIatHi - kIatLo);
    s_applied = true;
    ImgLog("loaded %s: %u ranges, %llu bytes, nonzero bytes in the ranges before the copy %llu; IAT 0x%08X..0x%08X zeroed",
           path, count, bytes, pre, kIatLo, kIatHi);

    // ---- self-test, before any game code ----
    unsigned fail = 0, checks = 0;
    for (std::uint32_t i = 0; i < count; ++i) {   // ranges read back (IAT bytes must be 0)
        const unsigned char* m = reinterpret_cast<const unsigned char*>(static_cast<std::uintptr_t>(r[i].va));
        unsigned bad = 0;
        for (std::uint32_t k = 0; k < r[i].len; ++k) {
            const std::uint32_t a = r[i].va + k;
            const unsigned char want = (a >= kIatLo && a < kIatHi) ? 0 : img[r[i].off + k];
            bad += (m[k] != want);
        }
        ++checks;
        if (bad) { ++fail; ImgLog("self-test FAIL: range %s %u bytes differ from the file", r[i].name, bad); }
    }
    std::free(img);
    // PortStateSeed witness: the seed's FNV over memory (PortStateSeed.cpp FnvEntries(true) form).
    std::uint32_t sh = 2166136261u;
    unsigned seedIn = 0;
    for (const PortStateSeedEntry& e : kPortStateSeedEntries) {
        unsigned char hdr[8];
        std::memcpy(hdr, &e.addr, 4);
        std::memcpy(hdr + 4, &e.len, 4);
        sh = Fnv(sh, hdr, 8);
        sh = Fnv(sh, reinterpret_cast<const unsigned char*>(static_cast<std::uintptr_t>(e.addr)), e.len);
        seedIn += (std::memcmp(reinterpret_cast<const void*>(static_cast<std::uintptr_t>(e.addr)), kPortStateSeedData + e.off, e.len) == 0);
    }
    ++checks;
    if (sh != MASHED_PORTSTATE_SEED_FNV1A) ++fail;
    ImgLog("witness PortStateSeed: %u/%u entries equal, mem=%08x want=%08x %s", seedIn,
           static_cast<unsigned>(MASHED_PORTSTATE_SEED_ENTRIES), sh, static_cast<unsigned>(MASHED_PORTSTATE_SEED_FNV1A),
           sh == MASHED_PORTSTATE_SEED_FNV1A ? "PASS" : "FAIL");
    unsigned rq = 0, dq = 0, csq = 0, cst = 0, cfq = 0, cft = 0;
    for (const WitnessSeed& s : kRdataWitness)
        rq += (std::memcmp(reinterpret_cast<const void*>(static_cast<std::uintptr_t>(s.va)), s.bytes, s.n) == 0);
    for (const WitnessSeed& s : kDInputWitness)
        dq += (std::memcmp(reinterpret_cast<const void*>(static_cast<std::uintptr_t>(s.va)), s.bytes, s.n) == 0);
#define CP_SEED(va, len, b) ++cst; csq += (std::memcmp(reinterpret_cast<const void*>(static_cast<std::uintptr_t>(va)), b, len) == 0);
#define CP_FIX(slot, sym, orig) ++cft; cfq += (*reinterpret_cast<const std::uint32_t*>(static_cast<std::uintptr_t>(slot)) == (orig));
#include "PortTopCodePtr.inc"
#undef CP_FIX
#undef CP_SEED
    const unsigned rt = static_cast<unsigned>(sizeof(kRdataWitness) / sizeof(kRdataWitness[0]));
    const unsigned dt = static_cast<unsigned>(sizeof(kDInputWitness) / sizeof(kDInputWitness[0]));
    checks += 4;
    fail += (rq != rt) + (dq != dt) + (csq != cst) + (cfq != cft);
    ImgLog("witness RdataSeed %u/%u, DInputSeed %u/%u, CP_SEED %u/%u equal; CP_FIX slots holding the original target %u/%u",
           rq, rt, dq, dt, csq, cst, cfq, cft);
    ImgLog("per-address seeds skipped (PortStateSeed, RdataSeed, DInputSeed bytes, CP_SEED); self-test %s (%u of %u checks failed)",
           fail ? "FAIL" : "PASS", fail, checks);
    return true;
}

void PortStateImage_Log(const char* logPath) {
    if (!s_pendLen) return;
    if (std::FILE* f = std::fopen(logPath, "a")) {
        std::fwrite(s_pend, 1, s_pendLen, f);
        std::fclose(f);
    }
    s_pendLen = 0;
}

} // namespace Compat
} // namespace mashed_re

extern "C" int PortStateImage_AppliedC(void) { return mashed_re::Compat::PortStateImage_Applied() ? 1 : 0; }
