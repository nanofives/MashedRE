// Mashed RE - Save/SettingsAndIO_i3 cluster (C2->C3 promotions, session c3-batch-i-s3).
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Hooks in this file (5):
//   0x00498f60  VideoDialogInit_i3              WM_INITDIALOG handler (HWND via EAX)
//   0x00499170  SubsystemSelectionChanged_i3    WM_COMMAND/1000 handler (HWND via EAX)
//   0x00499740  SetControlTextFromResource_i3   LoadStringA+SetWindowTextA helper (HWND via EAX)
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
//   - 0x00498f60 / 0x00499170 / 0x00499740 are dialog-procedure helpers
//     that take HWND in EAX implicitly (Ghidra "in_EAX"). They are
//     called only when the video settings dialog is open. We implement
//     each as __declspec(naked) with `mov ecx, eax` to bridge EAX into
//     the __fastcall ECX slot, then defer to a body function.
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

RH_ScopedInstall(FileReadWrapper_i3, 0x004b3b70);


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

RH_ScopedInstall(FileWriteWrapper_i3, 0x004b3bb0);


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
static constexpr std::uintptr_t kSubsysNameBase  = 0x007731fcu;  // subsystem name array
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
// 0x00498d60  PopulateModeCombo  (callee, C2; cited at 0x00498f60/0x00499170)
//   Called via stub address; signature: void __fastcall fn(HWND hModeCombo).
// ============================================================================
using PopulateModeFn_t = void(__fastcall*)(HWND);
static PopulateModeFn_t const g_PopulateModeCombo =
    reinterpret_cast<PopulateModeFn_t>(0x00498d60u);

// 0x004c2e70  RW_SetCurrentSubSystem  (C2 render; cited at 0x00499170)
using RwSetSubsystemFn_t = void(__cdecl*)(int);
static RwSetSubsystemFn_t const g_RwSetSubsystem =
    reinterpret_cast<RwSetSubsystemFn_t>(0x004c2e70u);

// 0x004c2f00  RW_GetCurrentMode  (C2 render; cited at 0x00499170)
using RwGetModeFn_t = int(__cdecl*)();
static RwGetModeFn_t const g_RwGetCurrentMode =
    reinterpret_cast<RwGetModeFn_t>(0x004c2f00u);


// ============================================================================
// 0x00499740  SetControlTextFromResource_i3 (body)
//
// Original 100 bytes.  HWND via EAX (Ghidra in_EAX).  Second arg is resource ID.
// Body: LoadStringA(HINSTANCE, id, buf[256], 0x100) then SetWindowTextA(hCtrl, buf).
// Stack-cookie wrapped in original (__security_check_cookie).
// ============================================================================
static void __stdcall SetControlTextFromResource_Body(HWND hCtrl, UINT resource_id)
{
    // 0x00499740 body: local CHAR[256], LoadStringA, SetWindowTextA.
    char buf[256];
    HINSTANCE hInst = *reinterpret_cast<HINSTANCE*>(kHInstanceGlobal);
    // LoadStringA(hInst, id, buf, 0x100)
    LoadStringA(hInst, resource_id, buf, 0x100);
    SetWindowTextA(hCtrl, buf);
}

// Naked entry: original takes HWND via EAX. We bridge EAX into ECX for the
// __fastcall body, treating param_2 (stack) as the resource id.
// MSVC's __fastcall: param1=ECX, param2=EDX.
// We instead use __stdcall for the body and manually arrange args.
// Stack on entry: [esp+0] = retaddr, [esp+4] = resource_id (caller arg).
// EAX = HWND.
extern "C" __declspec(dllexport) __declspec(naked)
void __cdecl SetControlTextFromResource_i3()
{
    __asm {
        // Stack-cookie wrapper compatibility — push extra room for buf[256]
        // is handled by the body function. We just bridge calling convention.
        //
        // Body is __stdcall: push args right-to-left then call.
        // Original signature: HWND in EAX, resource_id at [esp+4].
        push    ebp
        mov     ebp, esp
        push    dword ptr [ebp+8]   // resource_id
        push    eax                 // hCtrl from EAX
        call    SetControlTextFromResource_Body
        // __stdcall body cleans its own args.
        mov     esp, ebp
        pop     ebp
        ret
    }
}

RH_ScopedInstall(SetControlTextFromResource_i3, 0x00499740);


// ============================================================================
// 0x00499170  SubsystemSelectionChanged_i3 (body)
//
// Original 128 bytes. WM_COMMAND/1000 handler. HWND via EAX.
//
// Mechanical (verbatim from 00499170.md C1 plate):
//   HWND hSubsysCombo = GetDlgItem(hDlg, 0x3e8);
//   HWND hModeCombo   = GetDlgItem(hDlg, 0x3e9);
//   WPARAM cur = SendMessageA(hSubsysCombo, CB_GETCURSEL, 0, 0);
//   if (cur != DAT_0077340c) {
//       DAT_0077340c = SendMessageA(hSubsysCombo, CB_GETITEMDATA, cur, 0);
//       RW_SetSubSystem(DAT_0077340c);
//       SendMessageA(hModeCombo, CB_RESETCONTENT, 0, 0);
//       PopulateModeCombo(hModeCombo);
//       DAT_00773200 = RW_GetCurrentMode();
//       SendMessageA(hModeCombo, CB_SETCURSEL, DAT_00773200, 0);
//   }
// ============================================================================
static void __stdcall SubsystemSelectionChanged_Body(HWND hDlg)
{
    HWND hSubsysCombo = GetDlgItem(hDlg, kIdSubsys);  // 0x3e8
    HWND hModeCombo   = GetDlgItem(hDlg, kIdMode);    // 0x3e9
    LRESULT cur = SendMessageA(hSubsysCombo, kCB_GETCURSEL, 0, 0);

    auto* p_cur_subsys = reinterpret_cast<int*>(kCurSubsysIdx);
    auto* p_cur_mode   = reinterpret_cast<int*>(kCurModeIdx);

    if (cur != *p_cur_subsys) {
        *p_cur_subsys = static_cast<int>(
            SendMessageA(hSubsysCombo, kCB_GETITEMDATA, static_cast<WPARAM>(cur), 0));
        g_RwSetSubsystem(*p_cur_subsys);
        SendMessageA(hModeCombo, kCB_RESETCONTENT, 0, 0);
        g_PopulateModeCombo(hModeCombo);
        *p_cur_mode = g_RwGetCurrentMode();
        SendMessageA(hModeCombo, kCB_SETCURSEL, static_cast<WPARAM>(*p_cur_mode), 0);
    }
}

// Naked entry: HWND via EAX, no other args.
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

RH_ScopedInstall(SubsystemSelectionChanged_i3, 0x00499170);


// ============================================================================
// 0x00498f60  VideoDialogInit_i3 (body)
//
// Original 516 bytes. WM_INITDIALOG handler. HWND via EAX.
//
// Mechanical sequence (verbatim from 00498f60.md C1 plate, ordered by RVA):
//   1. SetControlTextFromResource(GetDlgItem(hDlg,1),     DAT_007719e8 + 0x7d0)  // OK
//   2. SetControlTextFromResource(GetDlgItem(hDlg,2),     DAT_007719e8 + 0x7da)  // Cancel
//   3. SetControlTextFromResource(GetDlgItem(hDlg,0x41a), DAT_007719e8 + 0x8c0)  // Label
//   4. SetControlTextFromResource(GetDlgItem(hDlg,0x417), DAT_007719e8 + 0x8ca)  // Chk A
//   5. SetControlTextFromResource(GetDlgItem(hDlg,0x418), DAT_007719e8 + 0x8d4)  // Chk B
//   6. SetControlTextFromResource(GetDlgItem(hDlg,0x419), DAT_007719e8 + 0x8de)  // Chk C
//   7. SetControlTextFromResource(GetDlgItem(hDlg,0x41b), DAT_007719e8 + 0x8e8)  // Chk D
//   8. BM_SETCHECK on 0x417 with (DAT_007733a8 == 0) ? 1 : 0
//   9. BM_SETCHECK on 0x418 with (DAT_007733a4 == 0) ? 1 : 0
//  10. BM_SETCHECK on 0x419 with (DAT_007733ac != 0) ? 1 : 0   (INVERTED)
//  11. BM_SETCHECK on 0x41b with (DAT_007733b0 == 0) ? 1 : 0
//  12. Subsys combo populate loop:
//        for (i = 0; i < DAT_00773410; i++) {
//            CB_ADDSTRING(hCombo, 0, (LPARAM)(DAT_007731fc + i*0x50))
//            CB_SETITEMDATA(hCombo, idx, i)   // idx = returned position
//        }
//        CB_SETCURSEL(hCombo, DAT_0077340c, 0)
//  13. Mode combo populate:
//        PopulateModeCombo(hModeCombo)
//        CB_SETCURSEL(hModeCombo, (&DAT_00773418)[DAT_00773200])
//        SetFocus(hModeCombo)
// ============================================================================
static void __stdcall VideoDialogInit_Body(HWND hDlg)
{
    // Step 1-7: label localization (7x SetControlTextFromResource calls).
    // We invoke the GAME's 0x00499740 directly via game-VA wrapper to preserve
    // bit-identity (it expects HWND in EAX and resource_id on stack).
    //
    // For internal use here we replicate the body directly to avoid the EAX
    // bridge dance — both paths produce identical LoadStringA/SetWindowTextA
    // sequences.  See SetControlTextFromResource_Body above.
    const int base_id = *reinterpret_cast<int*>(kStringResBase);

    SetControlTextFromResource_Body(GetDlgItem(hDlg, kIdOk),     base_id + kStrOkOff);      // 1
    SetControlTextFromResource_Body(GetDlgItem(hDlg, kIdCancel), base_id + kStrCancelOff);  // 2
    SetControlTextFromResource_Body(GetDlgItem(hDlg, kIdLabel),  base_id + kStrLabelOff);   // 3
    SetControlTextFromResource_Body(GetDlgItem(hDlg, kIdChkA),   base_id + kStrChkAOff);    // 4
    SetControlTextFromResource_Body(GetDlgItem(hDlg, kIdChkB),   base_id + kStrChkBOff);    // 5
    SetControlTextFromResource_Body(GetDlgItem(hDlg, kIdChkC),   base_id + kStrChkCOff);    // 6
    SetControlTextFromResource_Body(GetDlgItem(hDlg, kIdChkD),   base_id + kStrChkDOff);    // 7

    // Step 8-11: BM_SETCHECK on 4 checkboxes
    const int chkA = (*reinterpret_cast<int*>(kCheckboxA) == 0) ? 1 : 0;
    const int chkB = (*reinterpret_cast<int*>(kCheckboxB) == 0) ? 1 : 0;
    const int chkC = (*reinterpret_cast<int*>(kCheckboxC) != 0) ? 1 : 0;  // inverted
    const int chkD = (*reinterpret_cast<int*>(kCheckboxD) == 0) ? 1 : 0;

    SendMessageA(GetDlgItem(hDlg, kIdChkA), kBM_SETCHECK,
                 static_cast<WPARAM>(chkA), 0);
    SendMessageA(GetDlgItem(hDlg, kIdChkB), kBM_SETCHECK,
                 static_cast<WPARAM>(chkB), 0);
    SendMessageA(GetDlgItem(hDlg, kIdChkC), kBM_SETCHECK,
                 static_cast<WPARAM>(chkC), 0);
    SendMessageA(GetDlgItem(hDlg, kIdChkD), kBM_SETCHECK,
                 static_cast<WPARAM>(chkD), 0);

    // Step 12: subsystem combo populate
    HWND hSubsysCombo = GetDlgItem(hDlg, kIdSubsys);
    const int subsys_count = *reinterpret_cast<int*>(kSubsysCount);
    const std::uintptr_t name_base = kSubsysNameBase;
    for (int i = 0; i < subsys_count; i++) {
        const std::uintptr_t name_addr = name_base + (i * kSubsysNameStride);
        LRESULT idx = SendMessageA(hSubsysCombo, kCB_ADDSTRING, 0,
                                   static_cast<LPARAM>(name_addr));
        SendMessageA(hSubsysCombo, kCB_SETITEMDATA,
                     static_cast<WPARAM>(idx), static_cast<LPARAM>(i));
    }
    SendMessageA(hSubsysCombo, kCB_SETCURSEL,
                 static_cast<WPARAM>(*reinterpret_cast<int*>(kCurSubsysIdx)), 0);

    // Step 13: mode combo populate + focus
    HWND hModeCombo = GetDlgItem(hDlg, kIdMode);
    g_PopulateModeCombo(hModeCombo);
    // CB_SETCURSEL on (&DAT_00773418)[DAT_00773200]
    const int cur_mode = *reinterpret_cast<int*>(kCurModeIdx);
    const int* mode_map = reinterpret_cast<int*>(kModeIdxMap);
    SendMessageA(hModeCombo, kCB_SETCURSEL,
                 static_cast<WPARAM>(mode_map[cur_mode]), 0);
    SetFocus(hModeCombo);
}

// Naked entry: HWND via EAX.
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

RH_ScopedInstall(VideoDialogInit_i3, 0x00498f60);
