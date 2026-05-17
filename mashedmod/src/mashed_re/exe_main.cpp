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
// Milestone B4 (this revision): uploads one decoded ARGB8888 texture
//                               (default: "Shad_car", 256x256) to a D3D9
//                               IDirect3DTexture9 via D3d9Render/QuadRenderer
//                               and draws it as a centered screen-space quad
//                               every frame on top of the teal clear.
//                               Window title updates with the chosen texture
//                               name + dimensions for visual confirmation.
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

// B4 — D3D9 textured-quad renderer. Built after device init; uploads one
// decoded TXD texture once and draws it each frame.
mashed_re::D3d9Render::QuadRenderer g_quad_renderer;

// Name of the TXD entry we display. Selected for B4: 256x256 PAL8, the
// largest paletted texture in Frontend.piz and almost certainly the
// main-menu background art. Empirically chosen over ARGB candidates because:
//   * Shad_car (256x256 ARGB) is a car-shadow alpha mask: RGB == 0
//     throughout, renders as a solid black square (verified).
//   * Gravel (128x128 ARGB) is a 1-bit-RGB detail texture: only pure black
//     and pure white pixels, renders as monochrome noise (verified).
//   * rendergs (64x32 ARGB) is grayscale UI: R == G == B per pixel
//     (verified), renders as a tiny grey thumbnail.
//   * `main` (256x256 PAL8) uses a full RGB palette and is a real
//     visual texture.
//
// B4 supports both ARGB8888 and Paletted8 in the QuadRenderer — palette
// expansion was added as a stretch goal once the ARGB path was proven.
constexpr char  kTextureToShow[] = "main";

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
    // B4: draw the uploaded TXD texture as a centered screen-space quad on
    // top of the teal clear. No-op if no texture was uploaded.
    g_quad_renderer.Render();
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

// B4: locate `kTextureToShow` in the decoded dictionary, upload it via
// QuadRenderer, and update the window title. Called once after D3D9 init
// (g_device must be valid). Returns true on success.
bool UploadTextureForRender() {
    if (!g_device || !g_txd.valid()) return false;

    const mashed_re::Txd::Texture* picked = nullptr;
    for (std::uint32_t i = 0; i < g_txd.count(); ++i) {
        if (std::strcmp(g_txd.texture(i).name, kTextureToShow) == 0) {
            picked = &g_txd.texture(i);
            break;
        }
    }

    std::FILE* log = std::fopen(kLogPath, "a");
    if (!picked) {
        if (log) {
            std::fprintf(log, "\nMilestone B4: texture '%s' not found in TXD\n",
                         kTextureToShow);
            std::fclose(log);
        }
        return false;
    }

    if (!g_quad_renderer.Init(g_device, kWidth, kHeight)) {
        if (log) {
            std::fprintf(log, "\nMilestone B4: QuadRenderer.Init failed: %s\n",
                         g_quad_renderer.last_error());
            std::fclose(log);
        }
        return false;
    }
    if (!g_quad_renderer.UploadFromTexture(*picked)) {
        if (log) {
            std::fprintf(log,
                         "\nMilestone B4: UploadFromTexture('%s') failed: %s\n",
                         kTextureToShow, g_quad_renderer.last_error());
            std::fclose(log);
        }
        return false;
    }

    if (log) {
        std::fprintf(log,
                     "\nMilestone B4: uploaded '%s' %ux%u %s as IDirect3DTexture9 with %u mips\n",
                     picked->name, picked->width(), picked->height(),
                     mashed_re::Txd::PixelFormatName(picked->format()),
                     picked->mip_count);
        // Debug: dump the first 8 pixels of mip 0 raw bytes (R,G,B,A each
        // 0..255) so we can sanity-check the source data.
        if (picked->mip_count > 0 && picked->mips[0].pixels) {
            const auto& m0 = picked->mips[0];
            std::fprintf(log,
                         "  mip0 stride=%u w=%u h=%u depth=%u pixel_bytes=%u\n",
                         m0.stride, m0.width, m0.height, m0.depth, m0.pixel_bytes);
            std::fprintf(log, "  first 8 source pixels (R G B A):\n");
            for (std::uint32_t i = 0; i < 8 && i < m0.width; ++i) {
                std::fprintf(log, "    [%u] %3u %3u %3u %3u\n", i,
                             m0.pixels[i*4 + 0], m0.pixels[i*4 + 1],
                             m0.pixels[i*4 + 2], m0.pixels[i*4 + 3]);
            }
            // Also dump 8 pixels from the middle row to catch patterns.
            const std::uint32_t mid_row = m0.height / 2;
            const std::uint8_t* row = m0.pixels + mid_row * m0.stride;
            std::fprintf(log, "  middle row (y=%u) first 8 pixels (R G B A):\n",
                         mid_row);
            for (std::uint32_t i = 0; i < 8 && i < m0.width; ++i) {
                std::fprintf(log, "    [%u] %3u %3u %3u %3u\n", i,
                             row[i*4 + 0], row[i*4 + 1],
                             row[i*4 + 2], row[i*4 + 3]);
            }
        }
        std::fclose(log);
    }

    std::snprintf(g_windowTitle, sizeof(g_windowTitle),
                  "Mashed RE (Milestone B4 - showing '%s' %ux%u %s)",
                  picked->name, picked->width(), picked->height(),
                  mashed_re::Txd::PixelFormatName(picked->format()));
    if (g_hwnd) SetWindowTextA(g_hwnd, g_windowTitle);
    return true;
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

    // B4: now that D3D9 is up and the TXD is decoded, upload one texture and
    // refine the window title. Failure is non-fatal — the frame loop still
    // runs and shows a teal clear, just without the textured quad on top.
    (void)UploadTextureForRender();

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
