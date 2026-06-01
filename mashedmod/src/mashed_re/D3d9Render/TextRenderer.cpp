// Mashed RE — GDI text → BGRA rasterizer implementation (B19b). See header.

#include "TextRenderer.h"

#include <windows.h>
#include <cstdlib>
#include <cstring>

#pragma comment(lib, "gdi32.lib")

namespace mashed_re {
namespace D3d9Render {

std::uint8_t* RenderTextToBGRA(const wchar_t* text, int font_px,
                               std::uint32_t* out_w, std::uint32_t* out_h) {
    if (!text || !out_w || !out_h || font_px <= 0) return nullptr;

    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    if (screen) ReleaseDC(nullptr, screen);
    if (!dc) return nullptr;

    HFONT font = CreateFontW(-font_px, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                             ANTIALIASED_QUALITY, FF_DONTCARE, L"Arial");
    HGDIOBJ old_font = SelectObject(dc, font);

    SIZE sz = {};
    GetTextExtentPoint32W(dc, text, static_cast<int>(wcslen(text)), &sz);
    int w = sz.cx + 8;
    int h = sz.cy + 4;
    if (w < 1) w = 1;
    if (h < 1) h = 1;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = w;
    bi.bmiHeader.biHeight      = -h;          // negative => top-down
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    std::uint8_t* result = nullptr;

    if (dib && bits) {
        HGDIOBJ old_bm = SelectObject(dc, dib);
        std::memset(bits, 0, static_cast<std::size_t>(w) * h * 4);   // transparent/black bg
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(255, 255, 255));
        RECT rc = { 0, 0, w, h };
        DrawTextW(dc, text, -1, &rc, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOCLIP);
        GdiFlush();

        result = static_cast<std::uint8_t*>(std::malloc(static_cast<std::size_t>(w) * h * 4));
        if (result) {
            const std::uint8_t* src = static_cast<const std::uint8_t*>(bits);
            for (int i = 0; i < w * h; ++i) {
                const std::uint8_t b = src[i * 4 + 0];
                const std::uint8_t g = src[i * 4 + 1];
                const std::uint8_t r = src[i * 4 + 2];
                // GDI wrote white text on a black DIB (no alpha). Coverage = the
                // brightest channel; paint solid white with that as alpha so the
                // text alpha-blends cleanly through the bridge.
                const std::uint8_t a = (b > g) ? (b > r ? b : r) : (g > r ? g : r);
                result[i * 4 + 0] = 255;
                result[i * 4 + 1] = 255;
                result[i * 4 + 2] = 255;
                result[i * 4 + 3] = a;
            }
            *out_w = static_cast<std::uint32_t>(w);
            *out_h = static_cast<std::uint32_t>(h);
        }
        SelectObject(dc, old_bm);
        DeleteObject(dib);
    }

    SelectObject(dc, old_font);
    DeleteObject(font);
    DeleteDC(dc);
    return result;
}

}  // namespace D3d9Render
}  // namespace mashed_re
