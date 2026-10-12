// Mashed RE - Save/SettingsAndIO_i3 cluster (C2->C3 promotions, session c3-batch-i-s3).
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Hooks in this file (5):
//   0x00498f60  VideoDialogInit_i3              WM_INITDIALOG handler (HWND via EAX)
//   0x00499170  SubsystemSelectionChanged_i3    WM_COMMAND/1000 handler (HWND via EAX)
//   0x00499740  SetControlTextFromResource_i3   LoadStringA+SetWindowTextA helper, __cdecl(HWND, id)
//   0x004b3b70  FileReadWrapper_i3              file_read(filename, buf, size) __cdecl
//   0x004b3bb0  FileWriteWrapper_i3             file_write(filename, buf, size) __cdecl
//
// Analysis notes (per CLAUDE.md NO-GUESSING rule):
//   re/analysis/promote_c2_settings_dialog/00498f60.md  (C2 plate)
//   re/analysis/promote_c2_settings_dialog/00499170.md  (C2 plate)
//   re/analysis/promote_c2_settings_dialog/00499740.md  (C2 plate)
//   re/analysis/save_gamesave/004b3b70.md                (C1 plate, mech-verified)
//   re/analysis/save_gamesave/004b3bb0.md                (C1 plate, mech-verified)
//
// Calling-convention notes:
//   - 0x00498f60 / 0x00499170 take hDlg in EAX (0x00498f64 / 0x00499179 mov esi,eax); each export is
//     __declspec(naked), pushing EAX for a __stdcall body.
//   - 0x00499740 is plain __cdecl (HWND, id) on the stack (0x00499778 / 0x00499752; corrected 2026-10-12,
//     the old port took the HWND from EAX). 0x00498d60, called from both handlers above, is __cdecl with
//     the combo HWND on the stack (the old ports called it __fastcall).
//   - 0x004b3b70 / 0x004b3bb0 are plain __cdecl 3-arg file I/O wrappers
//     that call into the game's stream-context layer (FUN_004cc230 +
//     FUN_004cbd30/FUN_004cbe80 + FUN_004cc160). Reimplemented directly
//     in C++ with calls through cited game function addresses.
//
// Uncertainties acknowledged in source (cataloged in UNCERTAINTIES.md):
//   - U-0287 / U-0288 on 0x004b3bb0: PUSH ESI semantics; close() receives
//     WRITE RESULT (not the original handle). Bit-identity at the
//     assembly level is preserved by mechanical replication: we
//     unconditionally pass the write-result to close, matching original.

#include "../Core/HookSystem.h"

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


// ============================================================================
// Game function addresses (cited at exact RVA)
// ============================================================================

// 0x004cc230  FUN_004cc230  stream-context open (type, mode, filename) -> ctx
//   Cited at: 0x004b3b7d (read) / 0x004b3bb9 (write).
using StreamOpenFn_t = int(__cdecl*)(int, int, const char*);
static StreamOpenFn_t const g_StreamOpen =
    reinterpret_cast<StreamOpenFn_t>(0x004cc230u);

// 0x004cbd30  FUN_004cbd30  stream read (ctx, buf, size) -> bytes_read
//   Cited at: 0x004b3b96.
using StreamReadFn_t = int(__cdecl*)(int, void*, std::uint32_t);
static StreamReadFn_t const g_StreamRead =
    reinterpret_cast<StreamReadFn_t>(0x004cbd30u);

// 0x004cbe80  FUN_004cbe80  stream write (ctx, buf, size, esi_caller) -> result
//   Cited at: 0x004b3bd1. ESI is left as whatever the caller left in ESI.
//   We model this as a 3-arg call (ESI passthrough is preserved by the
//   __cdecl entry: caller's ESI is preserved across our naked PUSH ESI).
//   [UNCERTAIN U-0287] — see file header.
using StreamWriteFn_t = int(__cdecl*)(int, void*, std::uint32_t);
static StreamWriteFn_t const g_StreamWrite =
    reinterpret_cast<StreamWriteFn_t>(0x004cbe80u);

// 0x004cc160  FUN_004cc160  stream close (handle_or_result, 0) -> int
//   Cited at: 0x004b3ba0 (read; receives handle) / 0x004b3bdb (write;
//   receives write result, per U-0288 — preserved as-is for bit-identity).
using StreamCloseFn_t = int(__cdecl*)(int, int);
static StreamCloseFn_t const g_StreamClose =
    reinterpret_cast<StreamCloseFn_t>(0x004cc160u);


// ============================================================================
// 0x004b3b70  FileReadWrapper_i3
//
// Original 61-byte function. __cdecl.  3 args: (filename, buf, size).
//
//   0x004b3b70 MOV EAX,[ESP+0x4]                ; load filename
//   0x004b3b74 PUSH ESI
//   0x004b3b75 PUSH EDI
//   0x004b3b76 PUSH EAX                          ; arg3 to FUN_004cc230 = filename
//   0x004b3b77 PUSH 0x1                          ; arg2 = 1 (read mode)
//   0x004b3b79 PUSH 0x2                          ; arg1 = 2 (file type)
//   0x004b3b7b XOR  EDI,EDI                      ; EDI = 0 (default ret)
//   0x004b3b7d CALL 0x004cc230                   ; open
//   0x004b3b82 MOV  ESI,EAX                      ; ESI = handle
//   0x004b3b84 ADD  ESP,0xc                      ; clean 3 args
//   0x004b3b87 TEST ESI,ESI
//   0x004b3b89 JZ   0x004b3ba8                   ; skip if open failed
//   0x004b3b8b MOV  ECX,[ESP+0x14]               ; size
//   0x004b3b8f MOV  EDX,[ESP+0x10]               ; buffer
//   0x004b3b93 PUSH ECX                          ; size
//   0x004b3b94 PUSH EDX                          ; buffer
//   0x004b3b95 PUSH ESI                          ; handle
//   0x004b3b96 CALL 0x004cbd30                   ; read
//   0x004b3b9b PUSH 0x0                          ; close arg2
//   0x004b3b9d PUSH ESI                          ; close arg1 = handle
//   0x004b3b9e MOV  EDI,EAX                      ; EDI = bytes_read
//   0x004b3ba0 CALL 0x004cc160                   ; close
//   0x004b3ba5 ADD  ESP,0x14                     ; clean 5 args
//   0x004b3ba8 MOV  EAX,EDI                      ; ret value
//   0x004b3baa POP  EDI
//   0x004b3bab POP  ESI
//   0x004b3bac RET
// ============================================================================
extern "C" __declspec(dllexport)
int __cdecl FileReadWrapper_i3(const char* filename, void* buf, std::uint32_t size)
{
    int handle = g_StreamOpen(2, 1, filename);  // 0x004b3b79/77/76
    if (handle == 0) {                           // 0x004b3b87..89
        return 0;
    }
    int bytes_read = g_StreamRead(handle, buf, size);  // 0x004b3b96
    g_StreamClose(handle, 0);                          // 0x004b3ba0
    return bytes_read;                                 // 0x004b3ba8
}

RH_ScopedInstall(FileReadWrapper_i3, 0x004b3b70);  // re-enabled 2026-05-24 batch-save-e


// ============================================================================
// 0x004b3bb0  FileWriteWrapper_i3
//
// Original 55-byte function. __cdecl.  3 args: (filename, buf, size).
//
//   0x004b3bb0 MOV EAX,[ESP+0x4]                ; load filename
//   0x004b3bb4 PUSH EAX                          ; arg3 = filename
//   0x004b3bb5 PUSH 0x2                          ; arg2 = 2 (write mode)
//   0x004b3bb7 PUSH 0x2                          ; arg1 = 2 (file type)
//   0x004b3bb9 CALL 0x004cc230                   ; open
//   0x004b3bbe ADD  ESP,0xc
//   0x004b3bc1 TEST EAX,EAX
//   0x004b3bc3 JZ   0x004b3be6                   ; skip if open failed
//   0x004b3bc5 MOV  ECX,[ESP+0xc]                ; size
//   0x004b3bc9 MOV  EDX,[ESP+0x8]                ; buffer
//   0x004b3bcd PUSH ESI                          ; [U-0287] saves ESI + arg
//   0x004b3bce PUSH ECX                          ; size
//   0x004b3bcf PUSH EDX                          ; buffer
//   0x004b3bd0 PUSH EAX                          ; handle
//   0x004b3bd1 CALL 0x004cbe80                   ; write (4-arg per Ghidra)
//   0x004b3bd6 MOV  ESI,EAX                      ; ESI = write result
//   0x004b3bd8 PUSH 0x0                          ; close arg2
//   0x004b3bda PUSH ESI                          ; [U-0288] arg1 = write_result
//   0x004b3bdb CALL 0x004cc160                   ; close
//   0x004b3be0 ADD  ESP,0x14
//   0x004b3be3 MOV  EAX,ESI                      ; ret = write result
//   0x004b3be5 POP  ESI
//   0x004b3be6 RET
//
// For C3 bit-identity, the close() argument MUST be the write-result (ESI),
// not the original handle. We faithfully preserve this divergence from the
// read wrapper.  [UNCERTAIN U-0287 / U-0288] are about SEMANTICS, not BEHAVIOR.
// ============================================================================
extern "C" __declspec(dllexport)
int __cdecl FileWriteWrapper_i3(const char* filename, void* buf, std::uint32_t size)
{
    int handle = g_StreamOpen(2, 2, filename);  // 0x004b3bb7/b5/b4
    if (handle == 0) {                           // 0x004b3bc1..c3
        return 0;
    }
    // 0x004b3bd1: 4-arg call. We pass 3 args; ESI passthrough preserved by
    // standard cdecl entry (caller's ESI is unchanged by our prologue).
    int write_result = g_StreamWrite(handle, buf, size);  // 0x004b3bd1
    // 0x004b3bda: close receives WRITE_RESULT, not the original handle.
    // This is the documented [U-0288] divergence from the read wrapper.
    g_StreamClose(write_result, 0);  // 0x004b3bdb
    return write_result;             // 0x004b3be3
}

RH_ScopedInstall(FileWriteWrapper_i3, 0x004b3bb0);  // re-enabled 2026-05-24 batch-save-e


// ============================================================================
// Dialog-handler globals (cited at exact RVA in 00498f60.md / 00499170.md)
// ============================================================================

static constexpr std::uintptr_t kHInstanceGlobal = 0x007e9580u;  // HINSTANCE
static constexpr std::uintptr_t kStringResBase   = 0x007719e8u;  // string-ID base
static constexpr std::uintptr_t kCheckboxA       = 0x007733a8u;  // 0x417 source
static constexpr std::uintptr_t kCheckboxB       = 0x007733a4u;  // 0x418 source
static constexpr std::uintptr_t kCheckboxC       = 0x007733acu;  // 0x419 source (inverted)
static constexpr std::uintptr_t kCheckboxD       = 0x007733b0u;  // 0x41b source
static constexpr std::uintptr_t kSubsysCount     = 0x00773410u;  // subsystem count
static constexpr std::uintptr_t kSubsysNameBase  = 0x007731fcu;  // holds char* to 0x50-stride name table (0x004990f6)
static constexpr std::uintptr_t kCurSubsysIdx    = 0x0077340cu;  // current subsystem
static constexpr std::uintptr_t kCurModeIdx      = 0x00773200u;  // current mode
static constexpr std::uintptr_t kModeIdxMap      = 0x00773418u;  // mode->combo map

// Control IDs cited at 0x00498f60 body
static constexpr int kIdOk      = 1;
static constexpr int kIdCancel  = 2;
static constexpr int kIdSubsys  = 0x3e8;  // 1000
static constexpr int kIdMode    = 0x3e9;  // 1001
static constexpr int kIdLabel   = 0x41a;  // 1050
static constexpr int kIdChkA    = 0x417;  // 1047
static constexpr int kIdChkB    = 0x418;  // 1048
static constexpr int kIdChkC    = 0x419;  // 1049
static constexpr int kIdChkD    = 0x41b;  // 1051

// String-resource offsets from kStringResBase
static constexpr int kStrOkOff      = 0x7d0;  // 2000
static constexpr int kStrCancelOff  = 0x7da;  // 2010
static constexpr int kStrLabelOff   = 0x8c0;  // 2240
static constexpr int kStrChkAOff    = 0x8ca;  // 2250
static constexpr int kStrChkBOff    = 0x8d4;  // 2260
static constexpr int kStrChkCOff    = 0x8de;  // 2270
static constexpr int kStrChkDOff    = 0x8e8;  // 2280

// Subsystem name stride: 0x50 bytes (cited at 0x00498f60 body)
static constexpr std::uint32_t kSubsysNameStride = 0x50;

// Combo message constants (Win32 CB_*)
static constexpr UINT kCB_ADDSTRING    = 0x143;  // 323
static constexpr UINT kCB_RESETCONTENT = 0x14b;  // 331
static constexpr UINT kCB_SETCURSEL    = 0x14e;  // 334
static constexpr UINT kCB_GETITEMDATA  = 0x150;  // 336
static constexpr UINT kCB_GETCURSEL    = 0x147;  // 327
static constexpr UINT kCB_SETITEMDATA  = 0x151;  // 337
static constexpr UINT kBM_SETCHECK     = 0xf1;   // 241


// ============================================================================
// 0x00498d60  PopulateModeCombo  (callee; cited at 0x00499136 / 0x004991ce)
//   __cdecl, one stack arg: callers push the combo HWND and clean it with add esp,4 (0x00499149 / 0x004991d4).
//   Called through the RVA so an installed hook still routes to Save/SettingsDialog.cpp. The old ports here
//   declared it __fastcall (HWND in ECX), so the original body read stack garbage as its combo.
// ============================================================================
using PopulateModeFn_t = void(__cdecl*)(HWND);
static PopulateModeFn_t const g_PopulateModeCombo =
    reinterpret_cast<PopulateModeFn_t>(0x00498d60u);

// 0x004c2e70  RW_SetCurrentSubSystem  (cited at 0x004991ba; __cdecl, 0x004991bf add esp,4)
using RwSetSubsystemFn_t = void(__cdecl*)(int);
static RwSetSubsystemFn_t const g_RwSetSubsystem =
    reinterpret_cast<RwSetSubsystemFn_t>(0x004c2e70u);

// 0x004c2f00  RW_GetCurrentMode  (cited at 0x004991d7; no args)
using RwGetModeFn_t = int(__cdecl*)();
static RwGetModeFn_t const g_RwGetCurrentMode =
    reinterpret_cast<RwGetModeFn_t>(0x004c2f00u);


// ============================================================================
// 0x00499740  SetControlTextFromResource_i3
//
// Original 100 bytes, re-ported from the bytes 2026-10-12 (negctl re-earn). __cdecl, two stack args
// (hWnd, string id): 0x00499752 reads the id from [esp+0x10c] = arg2 and 0x00499778 the HWND from [esp+0x108] =
// arg1 (after sub esp,0x104); every caller pushes id then HWND and cleans 8 (0x00498f71/0x00498f72/0x00498f84, and
// 25 more call sites in 0x004974c5..0x00497e89). The old port took the HWND from EAX and arg1 as the id.
//
//   LoadStringA(DAT_007e9580, id, buf, 0x100);   // 0x00499766..0x00499772 (IAT 0x005cc290)
//   SetWindowTextA(hWnd, buf);                   // 0x0049977f..0x00499785 (IAT 0x005cc294)
// buf: 0x100 bytes at [esp] under a /GS cookie (0x00499746..0x0049975f, check 0x0049978b..0x0049979f).
// ============================================================================
extern "C" __declspec(dllexport)
void __cdecl SetControlTextFromResource_i3(HWND hWnd, UINT resource_id)
{
    char buf[0x100];
    LoadStringA(*reinterpret_cast<HINSTANCE*>(kHInstanceGlobal), resource_id, buf, 0x100);  // 0x00499772
    SetWindowTextA(hWnd, buf);                                                             // 0x00499785
}

RH_ScopedInstall(SetControlTextFromResource_i3, 0x00499740);  // re-enabled 2026-05-24 batch-save-e


// ============================================================================
// 0x00499170  SubsystemSelectionChanged_i3
//
// Original 128 bytes (0x00499170..0x004991ef), re-ported from the bytes 2026-10-12. hDlg in EAX
// (0x00499179 mov esi,eax), no stack args, plain ret (0x004991ef).
//
//   hSub  = GetDlgItem(hDlg, 0x3e8);                          // 0x0049917b..0x00499181 -> EBX
//   hMode = GetDlgItem(hDlg, 0x3e9);                          // 0x00499183..0x0049918b -> ESI
//   cur   = SendMessageA(hSub, CB_GETCURSEL 0x147, 0, 0);     // 0x00499193..0x0049919f
//   if (cur == DAT_0077340c) return;                          // 0x004991a1 cmp ; je 0x004991ec
//   data  = SendMessageA(hSub, CB_GETITEMDATA 0x150, cur, 0); // 0x004991a9..0x004991b2
//   DAT_0077340c = data;                                      // 0x004991b5
//   FUN_004c2e70(data);                                       // 0x004991b4 push eax ; 0x004991ba
//   SendMessageA(hMode, CB_RESETCONTENT 0x14b, 0, 0);         // 0x004991c2..0x004991cc
//   FUN_00498d60(hMode);                                      // 0x004991ce push esi ; 0x004991cf (__cdecl)
//   DAT_00773200 = FUN_004c2f00();                            // 0x004991d7 / 0x004991e5
//   SendMessageA(hMode, CB_SETCURSEL 0x14e, that value, 0);   // 0x004991dc..0x004991ea (not mapped through 0x00773418)
// ============================================================================
static void __stdcall SubsystemSelectionChanged_Body(HWND hDlg)
{
    HWND hSubsysCombo = GetDlgItem(hDlg, kIdSubsys);                             // 0x00499181
    HWND hModeCombo   = GetDlgItem(hDlg, kIdMode);                               // 0x0049918b
    const LRESULT cur = SendMessageA(hSubsysCombo, kCB_GETCURSEL, 0, 0);         // 0x0049919f

    if (static_cast<int>(cur) == *reinterpret_cast<int*>(kCurSubsysIdx)) return;  // 0x004991a7 je

    const int data = static_cast<int>(
        SendMessageA(hSubsysCombo, kCB_GETITEMDATA, static_cast<WPARAM>(cur), 0));  // 0x004991b2
    *reinterpret_cast<int*>(kCurSubsysIdx) = data;                                  // 0x004991b5
    g_RwSetSubsystem(data);                                                         // 0x004991ba
    SendMessageA(hModeCombo, kCB_RESETCONTENT, 0, 0);                               // 0x004991cc
    g_PopulateModeCombo(hModeCombo);                                                // 0x004991cf
    const int mode = g_RwGetCurrentMode();                                          // 0x004991d7
    *reinterpret_cast<int*>(kCurModeIdx) = mode;                                    // 0x004991e5
    SendMessageA(hModeCombo, kCB_SETCURSEL, static_cast<WPARAM>(mode), 0);          // 0x004991ea
}

// Naked entry: hDlg in EAX, no stack args.
extern "C" __declspec(dllexport) __declspec(naked)
void __cdecl SubsystemSelectionChanged_i3()
{
    __asm {
        push    ebp
        mov     ebp, esp
        push    eax                              // hDlg from EAX
        call    SubsystemSelectionChanged_Body
        mov     esp, ebp
        pop     ebp
        ret
    }
}

RH_ScopedInstall(SubsystemSelectionChanged_i3, 0x00499170);  // re-enabled 2026-05-24 batch-save-d


// ============================================================================
// 0x00498f60  VideoDialogInit_i3
//
// Original 516 bytes (0x00498f60..0x00499163), re-ported from the bytes 2026-10-12. hDlg in EAX
// (0x00498f64 mov esi,eax); its caller 0x004993a6 also pushes 0x110/wParam/lParam, never read here. Plain ret.
// base = DAT_007719e8, re-read before every use.
//
//   SetControlText(hDlg, base + 0x7e4);                          // 0x00498f66..0x00498f73 (the dialog itself;
//                                                                //  the old port omitted this call)
//   SetControlText(GetDlgItem(hDlg, 1),     base + 0x7d0);       // 0x00498f78..0x00498f94
//   SetControlText(GetDlgItem(hDlg, 2),     base + 0x7da);       // 0x00498f99..0x00498faf
//   SetControlText(GetDlgItem(hDlg, 0x41a), base + 0x8c0);       // 0x00498fb4..0x00498fcb
//   SetControlText(GetDlgItem(hDlg, 0x417), base + 0x8ca);       // 0x00498fd0..0x00498fe9
//   SetControlText(GetDlgItem(hDlg, 0x418), base + 0x8d4);       // 0x00498fee..0x00499007
//   SetControlText(GetDlgItem(hDlg, 0x419), base + 0x8de);       // 0x0049900c..0x00499023
//   SetControlText(GetDlgItem(hDlg, 0x41b), base + 0x8e8);       // 0x00499028..0x00499041
//   hSub  = GetDlgItem(hDlg, 0x3e8);                             // 0x00499049..0x0049904f -> EBP
//   hMode = GetDlgItem(hDlg, 0x3e9);                             // 0x00499051..0x0049905b -> [esp+0x10]
//   BM_SETCHECK 0xf1 on 0x417 with DAT_007733a8 == 0             // 0x0049905f..0x00499084
//   BM_SETCHECK on 0x418 with DAT_007733a4 == 0                  // 0x00499086..0x004990a5
//   BM_SETCHECK on 0x419 with DAT_007733ac != 0                  // 0x004990a7..0x004990c6
//   BM_SETCHECK on 0x41b with DAT_007733b0 == 0                  // 0x004990c8..0x004990e7
//   for (i = 0; i < DAT_00773410; i++) {                         // 0x004990e9..0x0049911f (count re-read, signed)
//       SendMessageA(hSub, CB_ADDSTRING 0x143, 0, *(char**)0x007731fc + i*0x50);  // 0x004990f6..0x00499108
//       SendMessageA(hSub, CB_SETITEMDATA 0x151, i, i);          // 0x0049910a..0x00499112 push esi ; push esi
//   }
//   SendMessageA(hSub, CB_SETCURSEL 0x14e, DAT_0077340c, 0);     // 0x00499121..0x00499130
//   FUN_00498d60(hMode);                                         // 0x00499136 push esi ; 0x00499137 (__cdecl)
//   SendMessageA(hMode, CB_SETCURSEL, DAT_00773418[DAT_00773200], 0);  // 0x0049913c..0x00499155
//   SetFocus(hMode);                                             // 0x00499158 (IAT 0x005cc29c)
//
// Corrected vs the old port: the 0x7e4 call on hDlg; the subsystem-name table is the heap POINTER held at
// 0x007731fc (0x004990f6 mov edx,[0x7731fc]; 0x00498ba2 frees it), not an inline array at 0x007731fc;
// CB_SETITEMDATA's wParam is i, not the CB_ADDSTRING position; FUN_00498d60 takes its HWND on the stack.
// ============================================================================
static void __stdcall VideoDialogInit_Body(HWND hDlg)
{
    const auto base = []() { return *reinterpret_cast<int*>(kStringResBase); };

    SetControlTextFromResource_i3(hDlg, base() + 0x7e4);                              // 0x00498f73
    SetControlTextFromResource_i3(GetDlgItem(hDlg, kIdOk),     base() + kStrOkOff);      // 0x00498f94
    SetControlTextFromResource_i3(GetDlgItem(hDlg, kIdCancel), base() + kStrCancelOff);  // 0x00498faf
    SetControlTextFromResource_i3(GetDlgItem(hDlg, kIdLabel),  base() + kStrLabelOff);   // 0x00498fcb
    SetControlTextFromResource_i3(GetDlgItem(hDlg, kIdChkA),   base() + kStrChkAOff);    // 0x00498fe9
    SetControlTextFromResource_i3(GetDlgItem(hDlg, kIdChkB),   base() + kStrChkBOff);    // 0x00499007
    SetControlTextFromResource_i3(GetDlgItem(hDlg, kIdChkC),   base() + kStrChkCOff);    // 0x00499023
    SetControlTextFromResource_i3(GetDlgItem(hDlg, kIdChkD),   base() + kStrChkDOff);    // 0x00499041

    HWND hSubsysCombo = GetDlgItem(hDlg, kIdSubsys);                                  // 0x0049904f
    HWND hModeCombo   = GetDlgItem(hDlg, kIdMode);                                    // 0x00499059

    SendMessageA(GetDlgItem(hDlg, kIdChkA), kBM_SETCHECK,
                 (*reinterpret_cast<int*>(kCheckboxA) == 0) ? 1 : 0, 0);              // 0x00499084
    SendMessageA(GetDlgItem(hDlg, kIdChkB), kBM_SETCHECK,
                 (*reinterpret_cast<int*>(kCheckboxB) == 0) ? 1 : 0, 0);              // 0x004990a5
    SendMessageA(GetDlgItem(hDlg, kIdChkC), kBM_SETCHECK,
                 (*reinterpret_cast<int*>(kCheckboxC) != 0) ? 1 : 0, 0);              // 0x004990c6
    SendMessageA(GetDlgItem(hDlg, kIdChkD), kBM_SETCHECK,
                 (*reinterpret_cast<int*>(kCheckboxD) == 0) ? 1 : 0, 0);              // 0x004990e7

    for (int i = 0; i < *reinterpret_cast<int*>(kSubsysCount); i++) {                // 0x004990f2 / 0x0049911d
        const std::uintptr_t names = *reinterpret_cast<std::uintptr_t*>(kSubsysNameBase);  // 0x004990f6
        SendMessageA(hSubsysCombo, kCB_ADDSTRING, 0,
                     static_cast<LPARAM>(names + i * kSubsysNameStride));             // 0x00499108
        SendMessageA(hSubsysCombo, kCB_SETITEMDATA,
                     static_cast<WPARAM>(i), static_cast<LPARAM>(i));                 // 0x00499112
    }
    SendMessageA(hSubsysCombo, kCB_SETCURSEL,
                 static_cast<WPARAM>(*reinterpret_cast<int*>(kCurSubsysIdx)), 0);     // 0x00499130

    g_PopulateModeCombo(hModeCombo);                                                  // 0x00499137
    const int cur_mode = *reinterpret_cast<int*>(kCurModeIdx);                        // 0x0049913c
    SendMessageA(hModeCombo, kCB_SETCURSEL,
                 static_cast<WPARAM>(reinterpret_cast<int*>(kModeIdxMap)[cur_mode]), 0);  // 0x00499155
    SetFocus(hModeCombo);                                                             // 0x00499158
}

// Naked entry: hDlg in EAX.
extern "C" __declspec(dllexport) __declspec(naked)
void __cdecl VideoDialogInit_i3()
{
    __asm {
        push    ebp
        mov     ebp, esp
        push    eax                 // hDlg from EAX
        call    VideoDialogInit_Body
        mov     esp, ebp
        pop     ebp
        ret
    }
}

RH_ScopedInstall(VideoDialogInit_i3, 0x00498f60);  // re-enabled 2026-05-24 batch-save-d
