// Mashed RE — standalone executable.
// Milestone A: opens a real Win32 window, runs a message pump, exits cleanly.
//
// This file is intentionally self-contained. The Boot/*.cpp reimplementations
// (Window.cpp, VideoConfig.cpp, FrameDispatch.cpp, RwEngineInit.cpp, ...) read
// from MASHED.exe absolute globals (HWND at 0x007e9584, MSG buffer at
// 0x007e95a0, quit flag at 0x00773918, etc.). Those addresses are part of
// MASHED.exe's process image; they don't exist in mashed_re.exe's address
// space and cannot be reused as-is. The .asi target still loads them for hook
// verification — they prove our understanding of MASHED's boot is correct.
// For the standalone exe we replicate the *pattern* (WindowMsgPump's
// PeekMessage / WM_QUIT short-circuit / WaitMessage gate) using our own
// locally-defined state.
//
// What this does:
//   1. RegisterClassA on a custom WNDPROC.
//   2. CreateWindowExA at 800x600 (matches Mashed's canonical videocfg).
//   3. ShowWindow / UpdateWindow.
//   4. Message pump loop: PeekMessage / TranslateMessage / DispatchMessage,
//      WaitMessage when no message and the window is active.
//   5. WNDPROC handles WM_DESTROY (sets quit flag) and WM_KEYDOWN ESC
//      (PostQuitMessage). Everything else falls through to DefWindowProcA.
//   6. On exit, DestroyWindow + UnregisterClassA.
//
// What this does NOT do:
//   - Load any Mashed asset (.piz, videocfg.bin, etc.).
//   - Initialise RenderWare or D3D9.
//   - Draw anything inside the window (it stays the background color).
//   - Anything beyond Milestone A.

#include <windows.h>
#include <cstdint>

#pragma comment(lib, "user32.lib")

namespace {

// Local boot state. Replaces MASHED's globals at 0x007e9584 / 0x00773918 / etc.
HWND g_hwnd       = nullptr;   // Main window handle.
MSG  g_msg        = {};        // Reusable MSG buffer (one per pump iteration).
bool g_quit       = false;     // Set on WM_DESTROY or PostQuitMessage.
bool g_active     = true;      // Set/cleared by WM_ACTIVATE.

constexpr int   kWidth      = 800;
constexpr int   kHeight     = 600;
constexpr char  kClassName[] = "MashedRE";
constexpr char  kWindowTitle[] = "Mashed RE (Milestone A)";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        return 0;

    case WM_DESTROY:
        g_quit = true;
        PostQuitMessage(0);
        return 0;

    case WM_ACTIVATE:
        // LOWORD(wp) == WA_INACTIVE (0) when deactivated, nonzero otherwise.
        // Mirrors the role of MASHED's DAT_0077391c at 0x0077391c.
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
// (re/analysis/window_msgpump/00499690.md, reimplementation
// mashedmod/src/mashed_re/Boot/Window.cpp at 0x00499690) but operating on the
// local g_msg / g_quit / g_active instead of MASHED's globals. Returns true
// when the application should exit.
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

}  // namespace

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Register the window class. Pattern from 0x00499ba0 (Window_Create) but
    // using our own class struct instead of MASHED's static one.
    WNDCLASSEXA wc = {};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = kClassName;
    if (!RegisterClassExA(&wc)) {
        return 1;
    }

    // Compute window rect so the client area is exactly kWidth x kHeight.
    RECT rect = {0, 0, kWidth, kHeight};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    g_hwnd = CreateWindowExA(
        0,
        kClassName,
        kWindowTitle,
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

    // Main loop: pump messages until the user closes the window or hits ESC.
    while (!PumpOnce()) {
        // Per-frame work goes here once the renderer is wired. For Milestone A
        // the window just sits there showing the background color.
    }

    // Teardown. Pattern from 0x00499cc0 (Window_Destroy).
    if (g_hwnd) {
        DestroyWindow(g_hwnd);
        g_hwnd = nullptr;
    }
    UnregisterClassA(kClassName, hInstance);

    return 0;
}
