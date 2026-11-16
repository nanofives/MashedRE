// RWC-FRAME-TITLE-LOGO (run 50, L-124 (C)): the RW core's image file readers under MASHED_RWC_FRAME. EXE ONLY.
//
// Not bodies of these RVAs. 0x00493600 (re/match/s15/00493600.c, 100%) registers two read functions with
// RwImageRegisterImageFormat 0x004ce790: "bmp" -> 0x004935a0 -> RtBMPImageRead 0x0052cb00 and "png" -> 0x004935d0 ->
// RtPNGImageRead 0x00514330 (hand ports PortTop_ImgRead_004935a0 / _004935d0, Match/PortTop_004ce790.cpp). Neither
// reader has a core body: 0x00514330 has no source (RwCore_Bodies.tsv kind missing, a logged stop), 0x0052cb00's source
// re/match/s23/0052cb00.c scores 22.9% and is outside the core closure. Their librw bindings (LibRw/RwApiBind.cpp,
// rw::readBMP / rw::readPNG) return librw images, which the core's RwImage readers (0x004c6750 and on) cannot take.
//
// route_bindings.py SPECIAL routes both bindings here on the frame route. Each wrapper reads the file the core's
// image search named (the extension is already on it: 0x004ce6c0 / 0x004ce480) from the same two places the RwStreamOpen
// binding 0x004cc230 reads from (PortTop_Fs_ReadAll: the piz mount stack, then the disk under "original/"; the
// original's readers open it with 0x004cc230(2, 1, name) at 0x0051434b (png) and through RwStreamOpen (bmp)), decodes it
// with the exe's WIC decoder (D3d9Render/PngLoader.h) and builds an RwImage on the RW core: RwImageCreate 0x004cdca0
// (w, h, 32) and RwImageAllocatePixels 0x004cdd60 (RwCore bodies), pixels in R, G, B, A byte order (the order the BMP
// reader's palette build writes: re/match/s23/0052cb00.c, pal[0] = p[2] .. pal[3] = 0xff).
//
// Declared deviation: the image is always 32-bit. [UNCERTAIN] the depth the original readers return for the font36.piz
// logos (MASHEDNEWLOGO.PNG 8-bit palette, colour type 3; BIGE.BMP and PROLOGICII.BMP 4 bpp; DOLBYDIG.BMP 8 bpp): the
// 22.9% BMP source text gives depth 4 for 1 / 4 bpp and 8 for 8 bpp, which is not proven, and 0x00514330 has no
// source. Test: an original-side read of the RwImage 0x00514330 / 0x0052cb00 return (+0x0c depth, +0x18 palette) for these
// four names (open row RWC-IMGREAD-DEPTH).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "../D3d9Render/PngLoader.h"

extern "C" {
void RwCore_Log(const char* fmt, ...);
void* PortTop_Fs_ReadAll(const char* name, unsigned* len, const char** from);
void* RwCore_004cdca0(int width, int height, int depth);   // RwImageCreate (re/match/rwl01/004cdca0.c, 100%)
void* RwCore_004cdd60(void* image);                        // RwImageAllocatePixels (re/match/rwl01/004cdd60.c)
int RwCore_004cdd00(void* image);                          // RwImageDestroy (re/match/rwl01/004cdd00.c, 100%)
}

namespace {
// RwImage words (re/match/rwl01/004cdca0.c / 004cdd60.c): [1] width, [2] height, [3] depth, [4] stride, [5] pixels.
void* CoreImageFromFile(const char* name, unsigned rva) {
    static unsigned s_calls = 0;
    const bool log = ++s_calls <= 16;
    if (!name) return nullptr;
    unsigned len = 0;
    const char* from = "";
    void* const blob = PortTop_Fs_ReadAll(name, &len, &from);
    if (!blob) {
        if (log) RwCore_Log("[RWC-FRAME-TITLE-LOGO] 0x%08X(\"%s\"): not in a mounted piz or on disk, returns 0", rva, name);
        return nullptr;
    }
    std::uint32_t w = 0, h = 0;
    std::uint8_t* const bgra = mashed_re::D3d9Render::DecodeImageToBGRA(static_cast<const std::uint8_t*>(blob), len, &w, &h);
    std::free(blob);
    if (!bgra) {
        if (log) RwCore_Log("[RWC-FRAME-TITLE-LOGO] 0x%08X(\"%s\"): %u bytes from %s did not decode, returns 0", rva, name,
                            len, from);
        return nullptr;
    }
    void* const img = RwCore_004cdca0(static_cast<int>(w), static_cast<int>(h), 32);
    if (!img || !RwCore_004cdd60(img)) {
        if (img) RwCore_004cdd00(img);
        std::free(bgra);
        if (log) RwCore_Log("[RWC-FRAME-TITLE-LOGO] 0x%08X(\"%s\"): RwImageCreate / AllocatePixels failed, returns 0", rva, name);
        return nullptr;
    }
    std::uint32_t* const im = static_cast<std::uint32_t*>(img);
    const std::uint32_t stride = im[4];
    std::uint8_t* const pix = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(im[5]));
    for (std::uint32_t y = 0; y < h; ++y) {
        const std::uint8_t* s = bgra + static_cast<std::size_t>(y) * w * 4;
        std::uint8_t* d = pix + static_cast<std::size_t>(y) * stride;
        for (std::uint32_t x = 0; x < w; ++x, s += 4, d += 4) {
            d[0] = s[2];
            d[1] = s[1];
            d[2] = s[0];
            d[3] = s[3];
        }
    }
    std::free(bgra);
    if (log)
        RwCore_Log("[RWC-FRAME-TITLE-LOGO] 0x%08X(\"%s\"): %u bytes from %s, %ux%u -> core RwImage %p (depth %u, stride %u)",
                   rva, name, len, from, w, h, img, im[3], stride);
    return img;
}
}  // namespace

extern "C" {
void* RwCore_004c5cb0(const char* name, const char* mask);   // RwTextureRead (re/match/s19/004c5cb0.c, provisional)

// Frame-route wrapper of RwTextureRead (rva 0x004c5cb0): calls the core body and logs its arguments and result (the row's test). The
// texture module globals (RwGlobals [0x007d3ff8] + [0x007d4054]): +0x10 current dictionary, +0x14 read callback,
// +0x18 find callback (re/match/s19/004c5cb0.c TG), +0x1c the word 0x004c61b0 tests at 0x004c61c1.
void* RwCore_FrameTextureRead_004c5cb0(const char* name, const char* mask) {
    static unsigned s_calls = 0;
    void* const t = RwCore_004c5cb0(name, mask);
    if (++s_calls <= 24) {
        const std::uint32_t g = *reinterpret_cast<const volatile std::uint32_t*>(static_cast<std::uintptr_t>(0x007d3ff8u));
        const std::uint32_t off = *reinterpret_cast<const volatile std::uint32_t*>(static_cast<std::uintptr_t>(0x007d4054u));
        const std::uint32_t* tg = reinterpret_cast<const std::uint32_t*>(static_cast<std::uintptr_t>(g + off));
        const std::uint32_t* tx = static_cast<const std::uint32_t*>(t);
        RwCore_Log("[RWC-FRAME-TITLE-LOGO] RwTextureRead 0x004c5cb0(\"%s\", %s%s%s) call %u on the RW core = %p "
                   "(raster %08X dict %08X refCount %d); texture globals: current dict %08X read %08X find %08X [+0x1c] %08X",
                   name ? name : "(null)", mask ? "\"" : "", mask ? mask : "0", mask ? "\"" : "", s_calls, t,
                   tx ? tx[0] : 0u, tx ? tx[1] : 0u, tx ? static_cast<int>(tx[0x54 / 4]) : 0, tg[4], tg[5], tg[6], tg[7]);
    }
    return t;
}

// Frame-route wrapper of RtPNGImageRead (rva 0x00514330)
void* RwCore_FrameImageRead_00514330(const char* name) { return CoreImageFromFile(name, 0x00514330u); }
// Frame-route wrapper of RtBMPImageRead (rva 0x0052cb00)
void* RwCore_FrameImageRead_0052cb00(const char* name) { return CoreImageFromFile(name, 0x0052cb00u); }
}  // extern "C"

#endif  // MASHED_STANDALONE
