// Mashed RE — standalone executable.
// Milestone A (done): opens a real Win32 window, runs a message pump, exits cleanly.
// Milestone B0 (done): adds a D3D9 device and clears the backbuffer to a
//                      teal color every frame.
// Milestone B1 (done): loads original/TOASTART/Common/Frontend.piz at
//                      startup via Piz/PizReader (C++ port of
//                      re/tools/piz_extract.py), writes a directory
//                      listing to mashed_re.log next to the exe, and
//                      appends the entry count to the window title.
// Milestone B2 (done): extracts the TEXTURES.TXD blob from the loaded
//                      Frontend.piz to extracted/TEXTURES.TXD next to the exe,
//                      walks its RWS chunk tree via Rws/RwsChunkWalker, and
//                      appends the tree to mashed_re.log.
// Milestone B3 (done): decodes the proprietary chunk-0x23 TXD format via
//                      Txd/TxdDecoder, emits a per-texture table (name, w, h,
//                      bpp, format, mip count, palette status) to
//                      mashed_re.log, and reflects the decoded texture count
//                      in the window title.
// Milestone B4 (done): uploaded one decoded ARGB8888/PAL8 texture (default:
//                      "main", 256x256) to a D3D9 IDirect3DTexture9 via
//                      D3d9Render/QuadRenderer and drew it as a centered
//                      screen-space quad each frame on top of the teal clear.
// Milestone B5 (prior revision): uploads ALL 8 decoded Frontend.piz textures
//                               into QuadRenderer slots 0..7 and draws them
//                               as a fixed 4x2 atlas in the 800x600 window
//                               (160x160 content per cell, cell pitch 200x270,
//                               row 0 starts at y=50, row 1 at y=320).
//                               Window title reflects atlas mode.
// Milestone B6 (this revision; Phase F gate per FRONTEND_ROADMAP):
//                               renders ONE textured 2D quad — the 'main'
//                               texture (256x256 PAL8 from Frontend.piz/
//                               TEXTURES.TXD) — centered at title-screen
//                               position scaled to 512x512. This closes
//                               the "RW replacement layer renders 1
//                               textured sprite" milestone (M6, smallest-
//                               viable-demo path). Atlas mode is retained
//                               as a build-time fallback via kTitleMode
//                               toggle — set to false for the diagnostic
//                               4x2 view.
//
// This file is intentionally self-contained. The Boot/*.cpp reimplementations
// (Window.cpp, VideoConfig.cpp, FrameDispatch.cpp, RwEngineInit.cpp, ...) read
// from MASHED.exe absolute globals (HWND at 0x007e9584, MSG buffer at
// 0x007e95a0, quit flag at 0x00773918, etc.). Those addresses are part of
// MASHED.exe's process image; they don't exist in mashed_re.exe's address
// space and cannot be reused as-is. The .asi target still loads them for hook
// verification — they prove our understanding of MASHED's boot is correct.
// For the standalone exe we replicate the *pattern* using our own
// locally-defined state.
//
// D3D9 pattern lifted from mashedmod/src/d3d9_shim/d3d9_shim.cpp (our own
// CreateDevice proxy) and from the canonical D3D9 init flow Mashed uses:
//   Direct3DCreate9(D3D_SDK_VERSION) -> IDirect3D9*
//   IDirect3D9::CreateDevice(0, HAL, hwnd, SOFTWARE_VERTEXPROCESSING, &pp,
//                            &device)
//   pp.Windowed = TRUE, BackBufferWidth/Height = 800x600,
//   pp.SwapEffect = D3DSWAPEFFECT_DISCARD,
//   pp.BackBufferFormat = D3DFMT_UNKNOWN (driver picks)
//
// What this does:
//   1. RegisterClassA on a custom WNDPROC.
//   2. CreateWindowExA at 800x600 (matches Mashed's canonical videocfg).
//   3. ShowWindow / UpdateWindow.
//   4. Direct3DCreate9 + CreateDevice (HAL, software vertex processing).
//   5. Message pump loop with RenderFrame() each iteration:
//        device->Clear(0, NULL, D3DCLEAR_TARGET, teal, 1.0f, 0)
//        device->BeginScene(); device->EndScene();
//        device->Present(NULL, NULL, NULL, NULL)
//   6. Device-lost handling: on D3DERR_DEVICELOST wait for D3DERR_DEVICENOTRESET
//      then Reset(); on Reset failure, exit.
//   7. WNDPROC handles WM_DESTROY (sets quit flag) and WM_KEYDOWN ESC.
//   8. On exit, Release(device) + Release(d3d) + DestroyWindow + UnregisterClassA.
//
// What this does NOT do:
//   - Load any Mashed asset (.piz, videocfg.bin, etc.).
//   - Initialise RenderWare (RwEngineInit and friends).
//   - Draw anything inside the cleared region.

#include <windows.h>
#include <d3d9.h>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "Piz/PizReader.h"
#include "Rws/RwsChunkWalker.h"
#include "Txd/TxdDecoder.h"
#include "D3d9Render/QuadRenderer.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "d3d9.lib")

namespace {

// Local boot state. Replaces MASHED's globals at 0x007e9584 / 0x00773918 / etc.
HWND               g_hwnd          = nullptr;
MSG                g_msg           = {};
bool               g_quit          = false;
bool               g_active        = true;

// D3D9 state. MASHED keeps its IDirect3DDevice9* somewhere inside the
// RenderWare driver struct (DAT_007d3ff8 vtable + offsets); we own ours
// directly.
IDirect3D9*        g_d3d           = nullptr;
IDirect3DDevice9*  g_device        = nullptr;
D3DPRESENT_PARAMETERS g_pp         = {};

constexpr int   kWidth         = 800;
constexpr int   kHeight        = 600;
constexpr char  kClassName[]   = "MashedRE";

// Path to one Mashed asset archive. Resolved relative to the exe's working
// directory (Mashed itself does the same — TOASTART/Common/* paths). For B1 we
// pick Frontend.piz because it's the menu's asset bundle; once Milestone B
// proper starts drawing menus we'll already have its blobs in hand.
constexpr char  kFrontendPiz[] = "original/TOASTART/Common/Frontend.piz";

// Log path next to the exe.
constexpr char  kLogPath[]     = "mashed_re.log";

// Window title is built dynamically once Piz load completes.
char            g_windowTitle[256] = "Mashed RE (Milestone B1 - loading...)";
mashed_re::Piz::Archive g_frontend_piz;

// B3 keeps the decoded TXD dictionary alive at file scope so its `Texture::Mip`
// pixel pointers (which reference bytes inside g_frontend_piz's loaded blob)
// remain valid for B4's upload step. Frontend.piz must therefore stay loaded
// for the lifetime of the program — which it does.
mashed_re::Txd::Dictionary g_txd;

// B4/B5 — D3D9 textured-quad renderer. Built after device init; uploads
// decoded TXD textures and draws them each frame. B5 fills slots 0..N-1 with
// every texture in g_txd.
mashed_re::D3d9Render::QuadRenderer g_quad_renderer;

// B5 — atlas layout constants. The 8 Frontend.piz textures fit into a 4x2
// grid in the 800x600 backbuffer. Cell pitch is 200 px horizontally and
// 270 px vertically; content is a square of kAtlasContentSize px regardless
// of source aspect, so all textures render at the same on-screen size for
// visual comparison.
//
// Cell center x: (col * 200) + 100        -> 100, 300, 500, 700
// Cell center y: 50 + (row * 270) + 100   -> 150, 420
//
// Row 0 content occupies y = 70..230 (160 px high, centered on 150),
// row 1 content occupies y = 340..500. Both fit comfortably in 0..600.
constexpr std::uint32_t kAtlasCols        = 4;
constexpr std::uint32_t kAtlasRows        = 2;
constexpr float         kAtlasCellPitchX  = 200.f;
constexpr float         kAtlasCellPitchY  = 270.f;
constexpr float         kAtlasOriginX     = 100.f;     // center of col 0
constexpr float         kAtlasOriginY     = 150.f;     // center of row 0
constexpr float         kAtlasContentSize = 160.f;

// B5 — uploaded slot count, set once at startup by UploadAllTexturesForAtlas().
std::uint32_t g_atlas_slot_count = 0;

// B6 — title-screen mode (Phase F gate). When true (default), render a single
// centered 'main' texture instead of the 4x2 atlas. Set to false to restore
// the B5 diagnostic atlas view.
constexpr bool kTitleMode = true;

// B6 — title-screen layout constants. 'main' is 256x256 PAL8; we render it
// at 512x512 centered, which is half the backbuffer height (600 - 512 = 88
// vertical headroom). On 800x600 this leaves a 144-pixel left/right border.
// Slot 0 is reserved for the title texture; atlas slots start at 1.
constexpr std::uint32_t kTitleSlotIndex   = 0;
constexpr float         kTitleQuadCenterX = 400.f;
constexpr float         kTitleQuadCenterY = 300.f;
constexpr float         kTitleQuadSize    = 512.f;
// Name of the texture to use as the title-screen logo. Picked because it's
// the only 256x256 PAL8 in Frontend.piz's TEXTURES.TXD — MASHED's menu code
// references this exact name at multiple call sites. Per the B3 log row 5.
constexpr const char    kTitleTexName[]   = "main";

// Distinctive teal clear color so we can confirm visually that D3D9 init
// worked. RGB(40, 80, 120) reads as a muted dark teal — clearly not the
// default desktop or window background.
constexpr D3DCOLOR kClearColor  = D3DCOLOR_XRGB(40, 80, 120);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        return 0;

    case WM_DESTROY:
        g_quit = true;
        PostQuitMessage(0);
        return 0;

    case WM_ACTIVATE:
        g_active = (LOWORD(wp) != WA_INACTIVE);
        return 0;

    case WM_KEYDOWN:
        if (wp == VK_ESCAPE) {
            PostQuitMessage(0);
            return 0;
        }
        break;

    default:
        break;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

// Single-iteration message pump. Pattern lifted from WindowMsgPump
// (re/analysis/window_msgpump/00499690.md, reimpl Boot/Window.cpp at 0x00499690)
// but operating on local state. Returns true when the application should exit.
bool PumpOnce() {
    if (PeekMessageA(&g_msg, nullptr, 0, 0, PM_REMOVE) != 0) {
        if (g_msg.message == WM_QUIT) {
            return true;
        }
        TranslateMessage(&g_msg);
        DispatchMessageA(&g_msg);
    } else if (!g_active) {
        WaitMessage();
    }
    return g_quit;
}

bool InitD3D9() {
    g_d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!g_d3d) return false;

    g_pp.Windowed               = TRUE;
    g_pp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    g_pp.BackBufferWidth        = kWidth;
    g_pp.BackBufferHeight       = kHeight;
    g_pp.BackBufferFormat       = D3DFMT_UNKNOWN; // current display format
    g_pp.hDeviceWindow          = g_hwnd;
    g_pp.PresentationInterval   = D3DPRESENT_INTERVAL_ONE;

    HRESULT hr = g_d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        g_hwnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &g_pp,
        &g_device);
    if (FAILED(hr)) {
        g_d3d->Release();
        g_d3d = nullptr;
        return false;
    }
    return true;
}

void ShutdownD3D9() {
    // Release D3D9-owned resources (textures, vertex buffers) before the
    // device itself.
    g_quad_renderer.Shutdown();
    if (g_device) { g_device->Release(); g_device = nullptr; }
    if (g_d3d)    { g_d3d->Release();    g_d3d    = nullptr; }
}

// Render one frame: clear to teal, present. Returns false on unrecoverable
// device loss (caller should exit).
bool RenderFrame() {
    if (!g_device) return false;

    HRESULT hr = g_device->TestCooperativeLevel();
    if (hr == D3DERR_DEVICELOST) {
        // Device lost (e.g. screensaver came up). Yield and try again next frame.
        Sleep(50);
        return true;
    }
    if (hr == D3DERR_DEVICENOTRESET) {
        hr = g_device->Reset(&g_pp);
        if (FAILED(hr)) return false;
    }

    g_device->Clear(0, nullptr, D3DCLEAR_TARGET, kClearColor, 1.0f, 0);
    g_device->BeginScene();
    if constexpr (kTitleMode) {
        // B6: title-screen MVP — single centered 'main' texture quad.
        // Phase F (M6) gate: "RW replacement renders 1 textured sprite".
        if (g_atlas_slot_count > 0) {
            g_quad_renderer.RenderAt(kTitleSlotIndex,
                                     kTitleQuadCenterX, kTitleQuadCenterY,
                                     kTitleQuadSize, kTitleQuadSize);
        }
    } else {
        // B5 diagnostic atlas mode (4x2 grid of all uploaded textures).
        for (std::uint32_t slot = 0; slot < g_atlas_slot_count; ++slot) {
            const std::uint32_t col = slot % kAtlasCols;
            const std::uint32_t row = slot / kAtlasCols;
            const float cx = kAtlasOriginX + static_cast<float>(col) * kAtlasCellPitchX;
            const float cy = kAtlasOriginY + static_cast<float>(row) * kAtlasCellPitchY;
            g_quad_renderer.RenderAt(slot, cx, cy,
                                     kAtlasContentSize, kAtlasContentSize);
        }
    }
    g_device->EndScene();

    hr = g_device->Present(nullptr, nullptr, nullptr, nullptr);
    if (hr == D3DERR_DEVICELOST) {
        return true; // recoverable; try next frame
    }
    return SUCCEEDED(hr);
}

// Load Frontend.piz at startup and write a directory listing to mashed_re.log.
// Updates g_windowTitle with a status string. Returns true if the archive was
// loaded with at least one entry; false otherwise (we don't bail the exe on
// failure — Milestone B1's contract is "try to load and report", not "must
// have assets to start").
bool LoadFrontendPiz() {
    std::FILE* log = std::fopen(kLogPath, "w");

    bool ok = g_frontend_piz.Load(kFrontendPiz);
    if (!ok) {
        std::snprintf(g_windowTitle, sizeof(g_windowTitle),
                      "Mashed RE (Milestone B1 - %s: %s)",
                      kFrontendPiz, g_frontend_piz.last_error());
        if (log) {
            std::fprintf(log, "Frontend.piz LOAD FAILED: %s\n", g_frontend_piz.last_error());
            std::fclose(log);
        }
        return false;
    }

    std::snprintf(g_windowTitle, sizeof(g_windowTitle),
                  "Mashed RE (Milestone B1 - Frontend.piz: %u entries, %s mode)",
                  g_frontend_piz.count(),
                  g_frontend_piz.mode() == mashed_re::Piz::OffsetMode::Raw ? "raw" : "sector");

    if (log) {
        std::fprintf(log, "Frontend.piz loaded OK\n");
        std::fprintf(log, "  path:    %s\n", kFrontendPiz);
        std::fprintf(log, "  size:    %zu bytes\n", g_frontend_piz.data_size());
        std::fprintf(log, "  version: %u\n", g_frontend_piz.version());
        std::fprintf(log, "  entries: %u\n", g_frontend_piz.count());
        std::fprintf(log, "  mode:    %s\n",
                     g_frontend_piz.mode() == mashed_re::Piz::OffsetMode::Raw ? "raw" : "sector");
        std::fprintf(log, "\n");
        std::fprintf(log, "  %-60s %10s %10s %10s\n", "name", "offset", "length", "id");
        std::fprintf(log, "  %-60s %10s %10s %10s\n",
                     "------------------------------------------------------------",
                     "----------", "----------", "----------");
        for (std::uint32_t i = 0; i < g_frontend_piz.count(); ++i) {
            const auto& e = g_frontend_piz.entry(i);
            std::fprintf(log, "  %-60s 0x%08X 0x%08X 0x%08X\n",
                         e.name, e.offset_bytes, e.length, e.id);
        }
        std::fclose(log);
    }
    return true;
}

// Write the blob backing entry `i` of the loaded Frontend.piz to disk at
// `out_path`. Returns true on success.
bool ExtractBlobToFile(std::uint32_t entry_index, const char* out_path) {
    if (!g_frontend_piz.valid()) return false;
    std::uint32_t length = 0;
    const std::uint8_t* bytes = g_frontend_piz.blob(entry_index, &length);
    if (!bytes) return false;

    std::FILE* fp = std::fopen(out_path, "wb");
    if (!fp) return false;
    std::size_t wrote = std::fwrite(bytes, 1, length, fp);
    std::fclose(fp);
    return wrote == length;
}

// Counter context for the walker callback.
struct ChunkCounter {
    std::FILE*    log;
    std::uint32_t total;
    std::uint32_t containers;
};

bool ChunkCallback(const mashed_re::Rws::ChunkInfo& info, void* user) {
    auto* ctx = static_cast<ChunkCounter*>(user);
    ctx->total += 1;
    if (mashed_re::Rws::IsKnownContainer(info.section_id)) ctx->containers += 1;
    if (ctx->log) {
        const char* name = mashed_re::Rws::IdName(info.section_id);
        // Indent two spaces per depth level for readability.
        for (std::uint32_t d = 0; d < info.depth; ++d) std::fputs("  ", ctx->log);
        std::fprintf(ctx->log,
                     "[%u] 0x%08X (%s) size=0x%08X version=0x%08X @ 0x%08zX\n",
                     info.depth, info.section_id,
                     name ? name : "?", info.size, info.version,
                     info.offset_in_buffer);
    }
    return true;
}

// Extracts TEXTURES.TXD (Frontend.piz entry 0) to extracted/TEXTURES.TXD next
// to the exe, then walks its RWS chunk tree and appends the result to the
// existing mashed_re.log. Updates g_windowTitle with the chunk count.
// Returns true if extraction + walk succeeded.
bool ExtractAndWalkTexturesTxd() {
    if (!g_frontend_piz.valid() || g_frontend_piz.count() == 0) return false;

    // Sanity: entry 0 in Frontend.piz is TEXTURES.TXD (verified in B1 log).
    constexpr std::uint32_t kEntryIdx = 0;
    const char* kOutDir  = "extracted";
    const char* kOutPath = "extracted/TEXTURES.TXD";

    CreateDirectoryA(kOutDir, nullptr); // OK if it already exists

    if (!ExtractBlobToFile(kEntryIdx, kOutPath)) {
        std::FILE* log = std::fopen(kLogPath, "a");
        if (log) {
            std::fprintf(log, "\nTEXTURES.TXD extract FAILED to %s\n", kOutPath);
            std::fclose(log);
        }
        return false;
    }

    std::uint32_t length = 0;
    const std::uint8_t* bytes = g_frontend_piz.blob(kEntryIdx, &length);
    if (!bytes) return false;

    std::FILE* log = std::fopen(kLogPath, "a");
    if (log) {
        std::fprintf(log, "\nTEXTURES.TXD extracted to %s (%u bytes)\n", kOutPath, length);
        std::fprintf(log, "RWS chunk tree:\n");
    }

    ChunkCounter ctx{};
    ctx.log = log;
    std::int32_t result = mashed_re::Rws::Walk(bytes, length, ChunkCallback, &ctx);

    if (log) {
        if (result < 0) {
            std::fprintf(log, "\n  ! walk reported parse error\n");
        } else {
            std::fprintf(log, "\n  total chunks: %u (containers: %u)\n",
                         ctx.total, ctx.containers);
        }
        std::fclose(log);
    }

    // Refine window title to include chunk count (B2 line; B3 overwrites it
    // below if decode succeeds).
    std::snprintf(g_windowTitle, sizeof(g_windowTitle),
                  "Mashed RE (Milestone B2 - %u piz entries, TEXTURES.TXD = %u chunks)",
                  g_frontend_piz.count(), ctx.total);
    return result >= 0;
}

// B3: Decode the proprietary chunk-0x23 TXD container from TEXTURES.TXD into a
// flat list of Texture records and dump a per-texture summary to mashed_re.log.
// Refines the window title to include the decoded texture count.
bool DecodeAndDumpTexturesTxd() {
    if (!g_frontend_piz.valid() || g_frontend_piz.count() == 0) return false;

    constexpr std::uint32_t kEntryIdx = 0; // entry 0 = TEXTURES.TXD (verified in B1)
    std::uint32_t length = 0;
    const std::uint8_t* bytes = g_frontend_piz.blob(kEntryIdx, &length);
    if (!bytes) return false;

    bool ok = g_txd.Decode(bytes, length);

    std::FILE* log = std::fopen(kLogPath, "a");
    if (log) {
        std::fprintf(log, "\nTXD decode (Milestone B3):\n");
        if (!ok) {
            std::fprintf(log, "  FAILED: %s\n", g_txd.last_error());
            std::fclose(log);
            return false;
        }
        std::fprintf(log, "  OK: %u textures, deviceId=%u\n",
                     g_txd.count(), g_txd.device_id());
        std::fprintf(log, "\n");
        std::fprintf(log,
            "  %-3s %-20s %5s %5s %4s %-5s %4s %-7s\n",
            "#", "name", "w", "h", "bpp", "fmt", "mips", "palette");
        std::fprintf(log,
            "  %-3s %-20s %5s %5s %4s %-5s %4s %-7s\n",
            "---", "--------------------", "-----", "-----",
            "----", "-----", "----", "-------");
        for (std::uint32_t i = 0; i < g_txd.count(); ++i) {
            const auto& t = g_txd.texture(i);
            const auto fmt = t.format();
            const std::uint8_t* pal = t.mip_count ? t.mips[0].palette : nullptr;
            std::fprintf(log,
                "  %-3u %-20s %5u %5u %4u %-5s %4u %-7s\n",
                i, t.name, t.width(), t.height(), t.depth(),
                mashed_re::Txd::PixelFormatName(fmt),
                t.mip_count, pal ? "yes" : "-");
        }
        std::fclose(log);
    }

    if (!ok) return false;

    // Refine window title with decoded texture count (B4 overwrites this
    // once a texture is uploaded successfully).
    std::snprintf(g_windowTitle, sizeof(g_windowTitle),
                  "Mashed RE (Milestone B3 - %u textures decoded)",
                  g_txd.count());
    return true;
}

// B6: upload the 'main' texture to slot 0 for the title-screen MVP. Looks up
// the texture by name in the decoded TXD; if not found, falls back to texture 0.
// Updates g_atlas_slot_count (set to 1 on success) + window title.
bool UploadTitleTextureForLogo() {
    if (!g_device || !g_txd.valid()) return false;

    std::FILE* log = std::fopen(kLogPath, "a");
    if (log) {
        std::fprintf(log, "\nMilestone B6: title-screen mode — uploading '%s'\n",
                     kTitleTexName);
    }

    if (!g_quad_renderer.Init(g_device, kWidth, kHeight)) {
        if (log) {
            std::fprintf(log, "\nMilestone B6: QuadRenderer.Init failed: %s\n",
                         g_quad_renderer.last_error());
            std::fclose(log);
        }
        return false;
    }

    // Locate the title texture by name. Linear scan over the 8 TXD entries —
    // negligible cost on a one-shot init path.
    std::uint32_t title_idx = UINT32_MAX;
    for (std::uint32_t i = 0; i < g_txd.count(); ++i) {
        const auto& t = g_txd.texture(i);
        // Case-sensitive match. Names in Mashed's TXDs are stored as written.
        bool eq = true;
        for (std::size_t k = 0; k < sizeof(kTitleTexName); ++k) {
            if (t.name[k] != kTitleTexName[k]) { eq = false; break; }
        }
        if (eq) { title_idx = i; break; }
    }
    if (title_idx == UINT32_MAX) {
        if (log) {
            std::fprintf(log, "  '%s' not found in TXD; falling back to texture 0\n",
                         kTitleTexName);
        }
        title_idx = 0;
    }

    const auto& tex = g_txd.texture(title_idx);
    if (!g_quad_renderer.UploadFromTextureToSlot(kTitleSlotIndex, tex)) {
        if (log) {
            std::fprintf(log,
                         "  slot %u upload FAILED: '%s' %ux%u %s — %s\n",
                         kTitleSlotIndex, tex.name, tex.width(), tex.height(),
                         mashed_re::Txd::PixelFormatName(tex.format()),
                         g_quad_renderer.last_error());
            std::fclose(log);
        }
        return false;
    }

    g_atlas_slot_count = 1;

    if (log) {
        std::fprintf(log,
                     "  slot %u: '%s' %ux%u %s mips=%u OK (title-screen quad)\n",
                     kTitleSlotIndex, tex.name, tex.width(), tex.height(),
                     mashed_re::Txd::PixelFormatName(tex.format()),
                     tex.mip_count);
        std::fclose(log);
    }

    std::snprintf(g_windowTitle, sizeof(g_windowTitle),
                  "Mashed RE (Milestone B6 - title screen: '%s' %ux%u)",
                  tex.name, tex.width(), tex.height());
    if (g_hwnd) SetWindowTextA(g_hwnd, g_windowTitle);
    return true;
}

// B5: upload every decoded TXD texture into a separate QuadRenderer slot so
// the frame loop can draw them all as an atlas. Retained as fallback when
// kTitleMode is false. Stops at kMaxSlots (8 cells for the 4x2 layout).
// Per-texture upload failures are logged but don't abort the rest.
// Updates g_atlas_slot_count + window title.
bool UploadAllTexturesForAtlas() {
    if (!g_device || !g_txd.valid()) return false;

    std::FILE* log = std::fopen(kLogPath, "a");
    if (log) {
        std::fprintf(log, "\nMilestone B5: uploading %u textures for atlas\n",
                     g_txd.count());
    }

    if (!g_quad_renderer.Init(g_device, kWidth, kHeight)) {
        if (log) {
            std::fprintf(log, "\nMilestone B5: QuadRenderer.Init failed: %s\n",
                         g_quad_renderer.last_error());
            std::fclose(log);
        }
        return false;
    }

    const std::uint32_t cap = kAtlasCols * kAtlasRows;
    std::uint32_t uploaded = 0;
    for (std::uint32_t i = 0; i < g_txd.count() && i < cap; ++i) {
        const auto& tex = g_txd.texture(i);
        if (g_quad_renderer.UploadFromTextureToSlot(i, tex)) {
            ++uploaded;
            if (log) {
                std::fprintf(log,
                             "  slot %u: '%s' %ux%u %s mips=%u OK\n",
                             i, tex.name, tex.width(), tex.height(),
                             mashed_re::Txd::PixelFormatName(tex.format()),
                             tex.mip_count);
            }
        } else if (log) {
            std::fprintf(log,
                         "  slot %u: '%s' %ux%u %s upload FAILED: %s\n",
                         i, tex.name, tex.width(), tex.height(),
                         mashed_re::Txd::PixelFormatName(tex.format()),
                         g_quad_renderer.last_error());
        }
    }

    g_atlas_slot_count = uploaded;

    if (log) {
        std::fprintf(log,
                     "  Milestone B5: %u/%u textures uploaded into atlas\n",
                     uploaded, g_txd.count());
        std::fclose(log);
    }

    std::snprintf(g_windowTitle, sizeof(g_windowTitle),
                  "Mashed RE (Milestone B5 - atlas: %u textures)",
                  uploaded);
    if (g_hwnd) SetWindowTextA(g_hwnd, g_windowTitle);
    return uploaded > 0;
}

}  // namespace

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Try to load the frontend asset archive before opening the window so the
    // initial title reflects load state. Failure is non-fatal.
    if (LoadFrontendPiz()) {
        // On success, also extract TEXTURES.TXD and walk its RWS chunk tree.
        // Both write to mashed_re.log; ExtractAndWalkTexturesTxd refines the
        // window title.
        if (ExtractAndWalkTexturesTxd()) {
            // B3: decode the TXD into a flat Texture array and log per-
            // texture metadata. Refines the window title with the decoded
            // texture count.
            (void)DecodeAndDumpTexturesTxd();
        }
    }

    // Register the window class. Pattern from 0x00499ba0 (Window_Create).
    WNDCLASSEXA wc = {};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr; // we own the backbuffer; don't let GDI clear under us
    wc.lpszClassName = kClassName;
    if (!RegisterClassExA(&wc)) {
        return 1;
    }

    RECT rect = {0, 0, kWidth, kHeight};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    g_hwnd = CreateWindowExA(
        0,
        kClassName,
        g_windowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr, nullptr,
        hInstance,
        nullptr);
    if (!g_hwnd) {
        UnregisterClassA(kClassName, hInstance);
        return 2;
    }

    // Show + paint. Pattern from 0x004996f0 (WindowShow).
    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);

    if (!InitD3D9()) {
        DestroyWindow(g_hwnd);
        UnregisterClassA(kClassName, hInstance);
        return 3;
    }

    // B5/B6: now that D3D9 is up and the TXD is decoded, upload textures into
    // QuadRenderer slot(s). B6 default uploads just 'main' to slot 0 for the
    // title-screen MVP; B5 fallback uploads all 8 textures into a 4x2 atlas
    // when kTitleMode is false. Failure is non-fatal — the frame loop still
    // runs and shows a teal clear, just without quads on top.
    if constexpr (kTitleMode) {
        (void)UploadTitleTextureForLogo();
    } else {
        (void)UploadAllTexturesForAtlas();
    }

    // Main loop: pump messages + render until the user closes the window or
    // hits ESC, or the device gets into an unrecoverable state.
    while (!PumpOnce()) {
        if (!RenderFrame()) {
            // Unrecoverable device loss; bail out.
            break;
        }
    }

    // Teardown.
    ShutdownD3D9();
    if (g_hwnd) {
        DestroyWindow(g_hwnd);
        g_hwnd = nullptr;
    }
    UnregisterClassA(kClassName, hInstance);

    return 0;
}
