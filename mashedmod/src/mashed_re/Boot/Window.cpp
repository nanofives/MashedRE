// Mashed RE — Boot/Window subsystem reimplementations.
// Session: ma2-frida-s2   Branch: c3/ma2-frida-s2
//
// Window creation + show + message-pump cluster. Two C2->C3 promotions in this
// file:
//   0x004996f0  WindowShow     — ShowWindow + UpdateWindow on the main HWND
//   0x00499690  WindowMsgPump  — single-iteration PeekMessage/Translate/Dispatch
//                                with WM_QUIT short-circuit and WaitMessage gate
//
// Other window-cluster RVAs were inspected during this session and refused:
//   0x00499ba0  Window_Create  — REFUSED (registers global window class + creates
//                                window; calling under Frida re-CoInits and would
//                                pollute live HWND state; no harness 3-arg setup).
//   0x00499cc0  Window_Destroy — REFUSED (DestroyWindow on live HWND terminates
//                                the running game; not safely Frida-testable).
//   0x00499820  Window_WndProc — REFUSED (WNDPROC stdcall(HWND,UINT,WPARAM,LPARAM);
//                                harness lacks a 4-arg WNDPROC arg_type).
//   0x00496490  Window_WmCreate — REFUSED (3 internal callees still C1:
//                                FUN_004a43d2, FUN_004a43b9, FUN_004a2b60 —
//                                callee gate fails per session rules).
//
// Global addresses cited here:
//   0x007e9584  — HWND main window handle (written by FUN_00499ba0 CreateWindowExA)
//   0x007e95a0  — MSG struct buffer used by PeekMessageA in MsgPump
//   0x007e95a4  — MSG.message field (= MSG buf + 0x4)
//   0x00773918  — quit flag set on WM_DESTROY by WNDPROC handler
//   0x0077391c  — window-active flag (1=active, 0=deactivated)
//
// All functions are __cdecl on the original (no thiscall, no fastcall).

#include "../Core/HookSystem.h"

#include <windows.h>
#include <cstdint>

// Linker pragma: ensure user32.lib is pulled in for the Win32 imports we use.
#pragma comment(lib, "user32.lib")

// ---------------------------------------------------------------------------
// Global address constants
// ---------------------------------------------------------------------------

// HWND of the game's main window. Written by CreateWindowExA inside
// FUN_00499ba0 (Window_Create). Read by ShowWindow/UpdateWindow inside
// WindowShow (this file) and DestroyWindow inside FUN_00499cc0.
// Cited at: 0x004996f7 (ShowWindow arg), 0x00499705 (UpdateWindow arg),
//           0x00499824-ish in MsgPump path (no — MsgPump uses its own MSG buf).
static constexpr std::uintptr_t kHwndAddr = 0x007e9584u;

// MSG struct buffer (28 bytes on x86 Win32) used by PeekMessageA in MsgPump.
// The first DWORD (offset 0) is HWND, second DWORD (offset 4) is message id.
// Cited at: 0x00499690-area; MSG.message read at MSG+0x4 (DAT_007e95a4).
static constexpr std::uintptr_t kMsgBufAddr     = 0x007e95a0u;
static constexpr std::uintptr_t kMsgMessageAddr = 0x007e95a4u; // MSG+0x4

// Quit flag — set to 1 by WNDPROC's WM_DESTROY branch (LAB_00499820).
// Read by MsgPump as its return value: returns DAT_00773918 != 0.
// Cited at: 0x004996d0-area (test ... ; setne al) in original MsgPump body.
static constexpr std::uintptr_t kQuitFlagAddr = 0x00773918u;

// Window-active flag — set to 1 on WM_ACTIVATE(active=1) or 0 on deactivate.
// Read by MsgPump to gate the WaitMessage() call: if active=0, WaitMessage()
// is invoked (the pump blocks); if active=1, the pump skips WaitMessage.
// Cited at: 0x004996bc-area (CMP [DAT_0077391c], 0) in original.
static constexpr std::uintptr_t kActiveFlagAddr = 0x0077391cu;

// WM_QUIT message constant (= 0x12). Read from MSG.message; if equal,
// MsgPump short-circuits with return value true.
// Cited at: comparison in original MsgPump body (within 0x00499690..0x004996e8).
static constexpr unsigned int kMsgQuit = 0x12u;

// PeekMessageA wRemoveMsg flag = PM_REMOVE = 1.
// Cited at: argument to PeekMessageA inside MsgPump.
static constexpr unsigned int kPmRemove = 1u;

// ---------------------------------------------------------------------------
// 0x004996f0  WindowShow  — void(int nCmdShow)
// ---------------------------------------------------------------------------
// Two-call wrapper:
//   ShowWindow(DAT_007e9584, param_1);
//   UpdateWindow(DAT_007e9584);
//
// 31 bytes in original. Trivial passthrough.
//
// Side effects:
//   - ShowWindow may toggle window visibility according to nCmdShow.
//   - UpdateWindow triggers WM_PAINT if the update region is non-empty.
//
// Frida verification strategy (path1):
//   arg_type='int_scalar'; pass standard nCmdShow values. Both sides call the
//   same Win32 APIs against the same HWND; reimpl wrapper returns void, so
//   void_match in the diff harness produces GREEN on bit-identical no-error.
//
extern "C" __declspec(dllexport) void __cdecl WindowShow(int nCmdShow) {
    HWND hwnd = *reinterpret_cast<HWND*>(kHwndAddr);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
}

RH_ScopedInstall(WindowShow, 0x004996f0);  // re-enabled 2026-05-24 c3-boot-b

// ---------------------------------------------------------------------------
// 0x00499690  WindowMsgPump  — bool(void)
// ---------------------------------------------------------------------------
// Single-iteration Win32 message pump:
//   if (PeekMessageA(&DAT_007e95a0, NULL, 0, 0, PM_REMOVE) != 0) {
//       if (DAT_007e95a4 == WM_QUIT) return (DAT_00773918 != 0);
//       TranslateMessage(&DAT_007e95a0);
//       DispatchMessageA(&DAT_007e95a0);
//   } else if (DAT_0077391c == 0) {
//       WaitMessage();
//   }
//   return (DAT_00773918 != 0);
//
// 88 bytes in original. Caller (FUN_00492290 outer loop) iterates this until
// the function returns true (quit flag set).
//
// Frida verification strategy (path1):
//   arg_type='none'; called from the Frida agent thread which owns no Win32
//   message queue, so PeekMessageA returns 0. DAT_0077391c==1 at quiescent
//   main menu (window is active), so WaitMessage is skipped. Both sides then
//   return DAT_00773918 != 0 — at the menu, the quit flag is 0, so both
//   return 0. 10 calls give 10x bit-identical 0.
//
extern "C" __declspec(dllexport) int __cdecl WindowMsgPump() {
    MSG* msg = reinterpret_cast<MSG*>(kMsgBufAddr);
    if (PeekMessageA(msg, nullptr, 0, 0, kPmRemove) != 0) {
        // Read MSG.message field directly via the global address — matches the
        // original's DAT_007e95a4 read pattern.
        unsigned int msg_id = *reinterpret_cast<unsigned int*>(kMsgMessageAddr);
        if (msg_id == kMsgQuit) {
            unsigned int quit = *reinterpret_cast<unsigned int*>(kQuitFlagAddr);
            return quit != 0;
        }
        TranslateMessage(msg);
        DispatchMessageA(msg);
    } else {
        unsigned int active = *reinterpret_cast<unsigned int*>(kActiveFlagAddr);
        if (active == 0) {
            WaitMessage();
        }
    }
    unsigned int quit = *reinterpret_cast<unsigned int*>(kQuitFlagAddr);
    return quit != 0;
}

RH_ScopedInstall(WindowMsgPump, 0x00499690);  // re-enabled 2026-05-24 c3-boot-b
