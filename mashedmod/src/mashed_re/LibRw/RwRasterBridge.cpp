// LibRw/RwRasterBridge.cpp — Txd::Texture -> rw::Raster. See RwRasterBridge.h.
//
// Lane M3-E2'a (gate D2). Format facts below are measured, not assumed: see the
// whole-game census in re/tools/txd_format_census.py (42 PC TXDs / 5194 mips)
// and the E2'a TASK 1 block in re/analysis/LIBRW_SIZING_2026-08.md.
//
//   depth 4  -> ONE BYTE per pixel, low nibble is the index, 16-entry palette
//   depth 8  -> one byte per pixel, 256-entry palette
//   depth 32 -> four bytes per pixel, no palette
//
// `depth` is the PALETTE SIZE exponent, not the storage width. Reading depth 4
// as packed nibbles is the bug that corrupted every PAL4 track texture until
// 2026-07-31. librw agrees, incidentally: its own D3D9 path sets
// raster->depth = 8 for PAL4 too (deps/librw/src/d3d/d3d.cpp:397-400).
//
// Row pitch is ALWAYS Mip::stride, never width*depth/8 -- a 4-byte minimum makes
// those disagree for every mip narrower than 4 texels (1973 mips game-wide).

#include "RwRasterBridge.h"

#include <windows.h>
#define WITH_D3D
#include <rw.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../Piz/PizReader.h"

namespace mashed_re {
namespace LibRw {
namespace {

// TXD palette entries are RGBA bytes (same convention QuadRenderer's PAL8 path
// and TrackRenderer::MakeTexture use).
struct Rgba { std::uint8_t r, g, b, a; };

inline Rgba PaletteEntry(const std::uint8_t* pal, std::uint32_t idx) {
    const std::uint8_t* e = pal + idx * 4u;
    return Rgba{ e[0], e[1], e[2], e[3] };
}

// Fetch source texel (x,y) as RGBA, whatever the source format.
inline bool SourceTexel(const Txd::Mip& mip, std::uint32_t x, std::uint32_t y,
                        Rgba* out) {
    const std::uint8_t* row =
        mip.pixels + static_cast<std::size_t>(y) * mip.stride;
    switch (mip.depth) {
    case 32: {
        const std::uint8_t* p = row + static_cast<std::size_t>(x) * 4u;
        *out = Rgba{ p[0], p[1], p[2], p[3] };
        return true;
    }
    case 8:
        if (!mip.palette) return false;
        *out = PaletteEntry(mip.palette, row[x]);
        return true;
    case 4:
        if (!mip.palette) return false;
        // One byte per pixel; only the low nibble indexes the 16-entry palette.
        *out = PaletteEntry(mip.palette, row[x] & 0x0Fu);
        return true;
    default:
        return false;
    }
}

}  // namespace

void* RasterFromTxdTexture(const Txd::Texture& tex) {
    if (tex.mip_count == 0) return nullptr;
    const Txd::Mip& mip = tex.mips[0];
    if (mip.width == 0 || mip.height == 0 || mip.pixels == nullptr)
        return nullptr;
    if (mip.depth != 4 && mip.depth != 8 && mip.depth != 32) return nullptr;
    if (mip.depth != 32 && mip.palette == nullptr) return nullptr;

    const std::int32_t w = static_cast<std::int32_t>(mip.width);
    const std::int32_t h = static_cast<std::int32_t>(mip.height);

    rw::Raster* ras = rw::Raster::create(
        w, h, 32, rw::Raster::C8888 | rw::Raster::TEXTURE);
    if (!ras) return nullptr;

    std::uint8_t* dst = ras->lock(0, rw::Raster::LOCKWRITE);
    if (!dst) { ras->destroy(); return nullptr; }

    // The lock hands back raw backend memory (d3d.cpp rasterLock sets
    // raster->pixels = lr.pBits and raster->stride = lr.Pitch), so for C8888 ==
    // D3DFMT_A8R8G8B8 we must write ARGB dwords, i.e. B,G,R,A byte order.
    // ras->stride is the destination pitch and is NOT width*4 in general.
    for (std::int32_t y = 0; y < h; ++y) {
        std::uint32_t* out = reinterpret_cast<std::uint32_t*>(
            dst + static_cast<std::size_t>(y) * ras->stride);
        for (std::int32_t x = 0; x < w; ++x) {
            Rgba c{};
            if (!SourceTexel(mip, static_cast<std::uint32_t>(x),
                             static_cast<std::uint32_t>(y), &c)) {
                ras->unlock(0);
                ras->destroy();
                return nullptr;
            }
            out[x] = (static_cast<std::uint32_t>(c.a) << 24) |
                     (static_cast<std::uint32_t>(c.r) << 16) |
                     (static_cast<std::uint32_t>(c.g) << 8) |
                      static_cast<std::uint32_t>(c.b);
        }
    }
    ras->unlock(0);

    // D-S3-SEA probe: the vertex colours and both texture decoders have been
    // shown identical, so the surviving candidate for the sea's 1.5x is whether
    // the two paths agree on ALPHA BLENDING. Log the decoded texture's mean RGB
    // (does librw sample the same texels the D3D9 path does?) and its alpha
    // range (is there any alpha for a blend to act on at all?).
    // Answered (sea: 256x256 depth 8, mean (60.7,68.2,76.8), alpha [255..255] --
    // opaque, so blending was ruled out), so it is gated: it rescans every texel
    // of every texture at load time. MASHED_LIBRW_TEXLOG=1 to re-enable.
    static const bool s_texlog = [] {
        const char* e = std::getenv("MASHED_LIBRW_TEXLOG");
        return e && e[0] == '1' && e[1] == '\0';
    }();
    if (s_texlog) {
        double sr = 0, sg = 0, sb = 0;
        unsigned amin = 255, amax = 0;
        const std::size_t n = static_cast<std::size_t>(w) * h;
        for (std::int32_t y = 0; y < h; ++y)
            for (std::int32_t x = 0; x < w; ++x) {
                Rgba c{};
                SourceTexel(mip, (std::uint32_t)x, (std::uint32_t)y, &c);
                sr += c.r; sg += c.g; sb += c.b;
                if (c.a < amin) amin = c.a;
                if (c.a > amax) amax = c.a;
            }
        std::FILE* f = std::fopen("log/librw_scene.txt", "a");
        if (f) {
            std::fprintf(f, "  TEXDEC '%s' %dx%d depth=%u mean=(%.1f,%.1f,%.1f) "
                            "alpha=[%u..%u]\n",
                         tex.name, w, h, (unsigned)mip.depth,
                         sr / n, sg / n, sb / n, amin, amax);
            std::fclose(f);
        }
    }
    return ras;
}

void* TextureFromTxdTexture(const Txd::Texture& tex) {
    rw::Raster* ras = static_cast<rw::Raster*>(RasterFromTxdTexture(tex));
    if (!ras) return nullptr;
    rw::Texture* t = rw::Texture::create(ras);
    if (!t) { ras->destroy(); return nullptr; }
    std::strncpy(t->name, tex.name, sizeof(t->name) - 1);
    t->name[sizeof(t->name) - 1] = '\0';
    std::strncpy(t->mask, tex.mask_name, sizeof(t->mask) - 1);
    t->mask[sizeof(t->mask) - 1] = '\0';
    // TXD packs filter in the low byte and addressing in the next two nibbles;
    // same layout RW uses, so the fields transfer directly.
    // [E3' filter delta -- MEASURED, and it is NOT a delta. 2026-08-02]
    // The registered concern was that librw filters NEAREST where the D3D9 path
    // forces LINEAR (TrackRenderer.cpp:3789-3790). That came from librw's
    // Texture::create default (texture.cpp:279) -- but this bridge OVERWRITES that
    // default from the TXD two lines down, and Arctic's TXD already asks for
    // LINEAR. Forcing LINEAR here produced a BIT-IDENTICAL frame on all 7 gating
    // shots. That alone proves nothing (it is equally what a dead override looks
    // like), so the =2 arm below forces NEAREST as the non-degeneracy control: it
    // moves every shot (01_grid 4.65->5.00, car_1_spawn 7.12->7.50, car_5_chase
    // 1.77->2.74). The override reaches the draw; the textures were already LINEAR.
    // Worth 0.00 of the E3' residual on Arctic. Kept as a probe for other tracks.
    // MASHED_LIBRW_LINEAR=1 forces LINEAR, =2 forces NEAREST. The =2 arm is the
    // NON-DEGENERACY control: forcing LINEAR produced bit-identical output, which
    // on its own cannot distinguish "the TXD already said LINEAR" from "the
    // override never reached the draw". Forcing NEAREST must change the frame.
    static const int s_filter_force = [] {
        const char* e = std::getenv("MASHED_LIBRW_LINEAR");
        if (!e || e[1] != '\0') return 0;
        return e[0] == '1' ? 1 : e[0] == '2' ? 2 : 0;
    }();
    t->setFilter(s_filter_force == 1 ? rw::Texture::LINEAR
               : s_filter_force == 2 ? rw::Texture::NEAREST
               : static_cast<rw::Texture::FilterMode>(tex.filter_addressing & 0xFFu));
    t->setAddressU(static_cast<rw::Texture::Addressing>(
        (tex.filter_addressing >> 8) & 0x0Fu));
    t->setAddressV(static_cast<rw::Texture::Addressing>(
        (tex.filter_addressing >> 12) & 0x0Fu));
    return t;
}

bool RasterRgbaHash(void* raster, std::uint32_t* out_hash) {
    rw::Raster* ras = static_cast<rw::Raster*>(raster);
    if (!ras || !out_hash) return false;

    std::uint8_t* px = ras->lock(0, rw::Raster::LOCKREAD);
    if (!px) return false;

    // lock() shifts width/height by the level (0 here, so unchanged) -- read
    // them back from the raster rather than assuming.
    const std::int32_t w = ras->width, h = ras->height;
    std::uint32_t hash = 2166136261u;                 // FNV-1a 32 offset basis
    for (std::int32_t y = 0; y < h; ++y) {
        const std::uint32_t* row = reinterpret_cast<const std::uint32_t*>(
            px + static_cast<std::size_t>(y) * ras->stride);
        for (std::int32_t x = 0; x < w; ++x) {
            const std::uint32_t v = row[x];
            // Emit canonical R,G,B,A so the value does not depend on the
            // backend's internal byte order. The Python side hashes the same
            // four bytes in the same order.
            const std::uint8_t bytes[4] = {
                static_cast<std::uint8_t>((v >> 16) & 0xFF),  // R
                static_cast<std::uint8_t>((v >> 8) & 0xFF),   // G
                static_cast<std::uint8_t>(v & 0xFF),          // B
                static_cast<std::uint8_t>((v >> 24) & 0xFF),  // A
            };
            for (int i = 0; i < 4; ++i) {
                hash ^= bytes[i];
                hash *= 16777619u;
            }
        }
    }
    ras->unlock(0);
    *out_hash = hash;
    return true;
}

namespace {

const char* const kRasterLog = "log/librw_raster.txt";

void RLog(const char* fmt, ...) {
    std::FILE* f = std::fopen(kRasterLog, "a");
    if (!f) return;
    va_list ap;
    va_start(ap, fmt);
    std::vfprintf(f, fmt, ap);
    va_end(ap);
    std::fputc('\n', f);
    std::fclose(f);
}

// One archive/entry pair to sweep. Chosen to cover all three formats the census
// found: DUMP.TXD carries PAL4 (incl. tex 19 "Roadslipblend2", the texture that
// exposed the nibble bug); Frontend's TEXTURES.TXD is PAL8 + ARGB8888.
struct Target { const char* piz; const char* entry; };
const Target kTargets[] = {
    { "original/TOASTART/TRACKS/dump.piz",    "DUMP.TXD" },
    { "original/TOASTART/Common/Frontend.piz", "TEXTURES.TXD" },
};

int SweepOne(const Target& t) {
    mashed_re::Piz::Archive ar;
    if (!ar.Load(t.piz)) {
        RLog("FAIL: piz load %s: %s", t.piz, ar.last_error());
        return -1;
    }
    const std::uint8_t* blob = nullptr;
    std::uint32_t len = 0;
    for (std::uint32_t i = 0; i < ar.count(); ++i) {
        if (_stricmp(ar.entry(i).name, t.entry) == 0) {
            blob = ar.blob(i, &len);
            break;
        }
    }
    if (!blob) { RLog("FAIL: %s has no entry %s", t.piz, t.entry); return -1; }

    static mashed_re::Txd::Dictionary dict;   // 256 Textures -- keep off the stack
    if (!dict.Decode(blob, len)) {
        RLog("FAIL: TXD decode %s::%s: %s", t.piz, t.entry, dict.last_error());
        return -1;
    }
    RLog("# %s::%s  textures=%u deviceId=%u",
         t.piz, t.entry, dict.count(), dict.device_id());

    int n = 0;
    for (std::uint32_t i = 0; i < dict.count(); ++i) {
        const mashed_re::Txd::Texture& tex = dict.texture(i);
        void* ras = RasterFromTxdTexture(tex);
        if (!ras) {
            RLog("%s|%u|SKIP (raster build failed)", t.entry, i);
            continue;
        }
        std::uint32_t hash = 0;
        const bool ok = RasterRgbaHash(ras, &hash);
        RLog("%s|%u|%s|%s|%ux%u|%08X", t.entry, i, tex.name,
             mashed_re::Txd::PixelFormatName(tex.format()),
             tex.width(), tex.height(), ok ? hash : 0u);
        static_cast<rw::Raster*>(ras)->destroy();
        if (ok) ++n;
    }
    return n;
}

}  // namespace

int RasterBridge_SelfTest() {
    if (std::FILE* f = std::fopen(kRasterLog, "w")) std::fclose(f);
    RLog("# librw raster bridge self-test (E2'a task 2)");
    RLog("# columns: entry|index|name|format|WxH|fnv1a32(RGBA read back "
         "through rw::Raster::lock)");
    RLog("# cross-check: py -3.12 re/tools/txd_format_census.py --rgba-hash");
    int total = 0;
    for (const Target& t : kTargets) {
        const int n = SweepOne(t);
        if (n < 0) return -1;
        total += n;
    }
    RLog("# hashed %d textures", total);
    return total;
}

}  // namespace LibRw
}  // namespace mashed_re
