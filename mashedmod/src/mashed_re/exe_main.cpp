// Mashed RE — standalone executable.
// Milestone A (done): opens a real Win32 window, runs a message pump, exits cleanly.
// Milestone B0 (done): adds a D3D9 device and clears the backbuffer to a
//                      teal color every frame.
// Milestone B1 (done): loads original/TOASTART/Common/Frontend.piz at
//                      startup via Piz/PizReader (C++ port of
//                      re/tools/piz_extract.py), writes a directory
//                      listing to mashed_re.log next to the exe, and
//                      appends the entry count to the window title.
// Milestone B2 (this revision): extracts the TEXTURES.TXD blob from the loaded
//                               Frontend.piz to extracted/TEXTURES.TXD next to
//                               the exe, walks its RWS chunk tree via
//                               Rws/RwsChunkWalker, appends the tree to
//                               mashed_re.log, and surfaces the chunk count in
//                               the window title. Still no actual texture
//                               decode / D3D9 texture / quad render -- those
//                               are B3 and B4.
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

#include "Piz/PizReader.h"
#include "Rws/RwsChunkWalker.h"

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
    // [Milestone B+: render the menu here.]
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

    // Refine window title to include chunk count.
    std::snprintf(g_windowTitle, sizeof(g_windowTitle),
                  "Mashed RE (Milestone B2 - %u piz entries, TEXTURES.TXD = %u chunks)",
                  g_frontend_piz.count(), ctx.total);
    return result >= 0;
}

}  // namespace

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Try to load the frontend asset archive before opening the window so the
    // initial title reflects load state. Failure is non-fatal.
    if (LoadFrontendPiz()) {
        // On success, also extract TEXTURES.TXD and walk its RWS chunk tree.
        // Both write to mashed_re.log; ExtractAndWalkTexturesTxd refines the
        // window title further.
        (void)ExtractAndWalkTexturesTxd();
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
