// Mashed RE — hook-side ABI twin of FUN_00473ee0 (animated logo overlay).
//
// The verbatim port (LogoOverlayDraw, DrawQuadPrimitives.cpp) takes the
// driver values explicitly (slide, wave_t, vscale). The original's ABI is
// the 10-arg form seen at the MenuChromeShellB call site (0x0042e5b0):
//   FUN_00473ee0(0, 0, slide_f, 512.0f, 0, bg_handle, 0, logo_handle, 0, 0xff)
// The overlay body consumes only the slide (param_3) plus the globals
// wave_t = DAT_007f1010 and the screen dims (FUN_0042b8b0/FUN_0042b8c0,
// both C3) — the handle/alpha params pass through to the band/grid draws'
// fixed constants in the port. This wrapper adapts ABI -> port.
//
// Promotion lane: re/frida/logo_overlay_diff.py — on-game-thread A/B with
// Im2D draw-sequence capture (vert scratch 0x00898a20, draw fn at
// *(*(0x7d3ff8)+0x30)), fade pair pre-synced via LogoOverlayFadeSet.
#include <cstdint>

#include "../Core/HookSystem.h"

extern "C" void __cdecl LogoOverlayDraw(float slide_x, float wave_t,
                                        float vscale_x, float vscale_y);

namespace {
using ShortGetter_t = std::uint16_t(__cdecl*)();
auto* const s_ScreenWidthGet  = reinterpret_cast<ShortGetter_t>(0x0042b8b0);
auto* const s_ScreenHeightGet = reinterpret_cast<ShortGetter_t>(0x0042b8c0);
constexpr std::uintptr_t kWaveT = 0x007f1010;   // DAT_007f1010 (seconds/3)
}  // namespace

// 0x00473ee0
extern "C" __declspec(dllexport) void __cdecl LogoOverlayTwin(
    int /*p1*/, int /*p2*/, float slide, float /*p4_512*/, int /*p5*/,
    int /*bg_handle*/, int /*p7*/, int /*logo_handle*/, int /*p9*/,
    int /*alpha*/) {
    const float sw = static_cast<float>(s_ScreenWidthGet());
    const float sh = static_cast<float>(s_ScreenHeightGet());
    const float wave_t = *reinterpret_cast<float*>(kWaveT);
    LogoOverlayDraw(slide, wave_t, sw, sh);
}

RH_ScopedInstall(LogoOverlayTwin, 0x00473ee0);
