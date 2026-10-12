// Mashed RE - Save/SettingsDialog cluster (C2->C3 promotions, session save-sdone-a-s2).
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Hooks in this file (3):
//   0x00498d20  ReadModeFromCombo_j5     WM_COMMAND IDOK handler (HWND via EAX)  [c3-batch-j-s5]
//   0x00498d60  PopulateModeCombo_s2     fills mode combo; filter flags!=0 AND w>0x27f AND h>0x1df
//   0x004991f0  VideoSettingsDlgProc_s2  DLGPROC for dialog 0x65: WM_INITDIALOG/COMMAND dispatch
//
// Analysis notes (per CLAUDE.md NO-GUESSING rule):
//   re/analysis/promote_c2_settings_dialog/00498d20.md  (C2 plate)
//   re/analysis/promote_c2_settings_dialog/00498d60.md  (C2 plate)
//   re/analysis/promote_c2_settings_dialog/004991f0.md  (C2 plate)
//
// Calling-convention notes:
//   0x00498d20 takes HWND in EAX (naked entry bridging EAX to the body).
//   0x00498d60 is __cdecl with the combo HWND as its one stack arg (0x00498dd9; corrected 2026-10-12,
//   the old port read EAX).
//   0x004991f0 is a __stdcall Win32 DLGPROC (HWND, UINT, WPARAM, LPARAM) -> INT_PTR, ret 0x10.
//   FormatDisplayModeString (0x00498a00) uses EBX as implicit mode-struct ptr;
//   called via a naked wrapper (FormatModeString_Thunk) that sets EBX from param.
//
// Deferred this session (STOP-AND-ASK triggered):
//   - 0x00499400 VideoSettingsDispatcher — invokes DialogBoxParamA live Win32
//     modal loop; 3 C1 callees (FormatDisplayModeString, FindDefaultFullscreenMode,
//     SnapshotCurrentVideoSettings); defer until those reach C2+.
//
// Deferred from batch-j-s5 (now promoted here):
//   - 0x00498d60 PopulateModeCombo — now has C3 callers (VideoDialogInit C3,
//     SubsystemSelectionChanged C3); Win32 externals satisfy callee rule.
//   - 0x004991f0 VideoSettingsDlgProc — all internal callees now C3.

#include "../Core/HookSystem.h"

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


// ============================================================================
// 0x00498d20  ReadModeFromCombo_j5
//
// Original 53 bytes (0x00498d20..0x00498d52).  HWND via EAX (Ghidra in_EAX).
// Reads the current selection from the mode combo (control 0x3e9 = 1001) and
// writes the associated CB_GETITEMDATA payload into DAT_00773200 (current mode).
//
// Raw bytes verified at file-offset 0x98d20 of MASHED.exe.unpatched:
//   56 57                          ; PUSH ESI ; PUSH EDI
//   68 e9 03 00 00                 ; PUSH 0x3e9         (control ID 1001)
//   50                             ; PUSH EAX           (HWND from caller)
//   ff 15 0c c2 5c 00              ; CALL [0x005cc20c]  GetDlgItem IAT slot
//   8b 3d 24 c2 5c 00              ; MOV  EDI, [0x005cc224]  SendMessageA IAT
//   6a 00 6a 00                    ; PUSH 0 ; PUSH 0
//   8b f0                          ; MOV  ESI, EAX      (hCombo)
//   68 47 01 00 00                 ; PUSH 0x147         (CB_GETCURSEL)
//   56                             ; PUSH ESI
//   ff d7                          ; CALL EDI           SendMessageA
//   6a 00 50                       ; PUSH 0 ; PUSH EAX  (cur as wparam)
//   68 50 01 00 00                 ; PUSH 0x150         (CB_GETITEMDATA)
//   56                             ; PUSH ESI
//   ff d7                          ; CALL EDI           SendMessageA
//   5f                             ; POP  EDI
//   a3 00 32 77 00                 ; MOV  [0x00773200], EAX
//   5e                             ; POP  ESI
//   c3                             ; RET
//
// Return value: EAX at RET time == result of the second SendMessageA
//   (CB_GETITEMDATA). NOT a constant `1`. This is the same value that gets
//   written to DAT_00773200 (the MOV does not clobber EAX).
//
// Constants (cited at 0x00498d20 body):
//   0x3e9 (1001) — mode combo control ID
//   0x147 (327)  — CB_GETCURSEL
//   0x150 (336)  — CB_GETITEMDATA
//
// Globals written:
//   DAT_00773200 (current mode index, 4 bytes) <- last SendMessageA result
// ============================================================================

// 0x00773200 — current mode index (cited in 00498d20.md / 00499400.md)
static constexpr std::uintptr_t kCurModeIdx = 0x00773200u;

// Control IDs / messages (cited at 0x00498d20 body)
static constexpr int  kIdMode         = 0x3e9;   // 1001
static constexpr UINT kCB_GETCURSEL   = 0x147;   // 327
static constexpr UINT kCB_GETITEMDATA = 0x150;   // 336

static int __stdcall ReadModeFromCombo_Body(HWND hDlg)
{
    HWND    hCombo = GetDlgItem(hDlg, kIdMode);                                 // 0x00498d20 body
    LRESULT cur    = SendMessageA(hCombo, kCB_GETCURSEL, 0, 0);                 // 0x00498d20 body
    LRESULT data   = SendMessageA(hCombo, kCB_GETITEMDATA,
                                  static_cast<WPARAM>(cur), 0);                 // 0x00498d20 body
    *reinterpret_cast<int*>(kCurModeIdx) = static_cast<int>(data);              // 0x00498d20 body
    // Return value is the second SendMessageA result (the MOV to DAT_00773200
    // does not clobber EAX). NOT a constant 1.
    return static_cast<int>(data);
}

// Naked entry: HWND via EAX, no other args.
extern "C" __declspec(dllexport) __declspec(naked)
int __cdecl ReadModeFromCombo_j5()
{
    __asm {
        push    ebp
        mov     ebp, esp
        push    eax                  // hDlg from EAX
        call    ReadModeFromCombo_Body
        mov     esp, ebp
        pop     ebp
        ret
    }
}

RH_ScopedInstall(ReadModeFromCombo_j5, 0x00498d20);  // re-enabled 2026-05-24 batch-save-d


// ============================================================================
// 0x00498d60  PopulateModeCombo_s2
//
// Original 219 bytes (0x00498d60..0x00498e3a), re-ported from the bytes 2026-10-12 (negctl re-earn).
// __cdecl, ONE stack arg: the mode combo HWND. 0x00498dd9 mov ebx,[esp+0x60] reads it (esp+0x60 = arg1 after
// sub esp,0x48 + 4 pushes + the push of the format buffer); the callers push it at 0x00499136 and 0x004991ce and
// clean it with add esp,4 at 0x00499149 / 0x004991d4. The old port read it from EAX.
//
// for (i = 0; i < counts[DAT_0077340c]; i++) {          // 0x00498d76..0x00498d86 / 0x00498e02..0x00498e21
//     mode = modes[DAT_0077340c] + i * 0x18;             // 0x00498da0..0x00498db1 (both tables re-read each pass)
//     if (mode[+0x0c] == 0) continue;                   // 0x00498dad..0x00498dbc test/sete/test/jne
//     if ((int)mode[+0] < 0x280) continue;              // 0x00498dbe cmp/jl (signed)
//     if ((int)mode[+4] < 0x1e0) continue;              // 0x00498dc6 cmp/jl (signed)
//     FormatDisplayModeString(buf);                     // 0x00498dd4, EBX = mode
//     pos = SendMessageA(hCombo, CB_ADDSTRING 0x143, 0, buf);    // 0x00498de4..0x00498ded
//     SendMessageA(hCombo, CB_SETITEMDATA 0x151, pos, i);         // 0x00498def..0x00498df9
//     DAT_00773418[i] = pos;                            // 0x00498dfb mov [edi*4+0x773418],esi
// }
// counts = *(int**)0x007731f8, modes = *(uint8_t***)0x00773408 (heap tables; 0x00498b60 frees both).
// No NULL guard on either table in the original (the old port added two).
// buf: 0x40 bytes at [esp+0x14] (frame 0x04..0x43, /GS cookie at frame 0x44, 0x00498d63..0x00498d72).
//
// The deployed reference has the show_windowed boot patch at 0x00498dbc (75 44 -> 90 90), so the flags != 0
// filter is not diffable against it; fixtures keep +0x0c nonzero. The port keeps the unpatched bytes.
// ============================================================================

static constexpr std::uintptr_t kSubsysIdx        = 0x0077340cu;  // 0x00498d76 / 0x00498e02 current subsystem
static constexpr std::uintptr_t kModeCountArrPtr  = 0x007731f8u;  // 0x00498d6c / 0x00498e0b holds int* counts
static constexpr std::uintptr_t kModeArrPtrArrPtr = 0x00773408u;  // 0x00498da0 holds uint8_t** mode arrays
static constexpr std::uintptr_t kModeIdxMapBase   = 0x00773418u;  // 0x00498dfb int[] mode -> combo pos

static constexpr int kMinWidth         = 0x280;  // 0x00498dbe cmp [ebx],0x280 ; jl
static constexpr int kMinHeight        = 0x1e0;  // 0x00498dc6 cmp [ebx+4],0x1e0 ; jl
static constexpr int kModeFlagsOffset  = 0x0c;   // 0x00498dad mov edx,[ebx+esi+0xc]
static constexpr int kModeStructStride = 0x18;   // 0x00498e18 add esi,0x18

static constexpr UINT kCB_ADDSTRING_d   = 0x143;  // 0x00498de7
static constexpr UINT kCB_SETITEMDATA_d = 0x151;  // 0x00498df3

static constexpr int kModeStrBufSize = 0x40;      // [esp+0x14] .. cookie at [esp+0x54] (pre-push frame 0x04..0x44)

// 0x00498a00 FormatDisplayModeString: __cdecl(char* buf), mode record in EBX (0x00498a14 mov edx,[ebx+0xc],
// 0x00498b27..0x00498b32 [ebx+8]/[ebx+4]/[ebx]). Caller cleans the one arg (0x00498ddd add esp,4).
static void __cdecl FormatModeString_Thunk(char* buf, int* mode_ptr)
{
    __asm {
        push    ebx
        mov     ebx, mode_ptr
        push    buf
        mov     ecx, 0x00498a00
        call    ecx
        add     esp, 4
        pop     ebx
    }
}

// 0x00498d60
extern "C" __declspec(dllexport)
void __cdecl PopulateModeCombo_s2(HWND hModeCombo)
{
    char mode_str[kModeStrBufSize];

    // 0x00498d76..0x00498d86: if (counts[DAT_0077340c] <= 0) skip the loop (jle, signed).
    for (int i = 0;
         i < (*reinterpret_cast<int* const*>(kModeCountArrPtr))[*reinterpret_cast<int*>(kSubsysIdx)];  // 0x00498e02..0x00498e1b
         i++)
    {
        // 0x00498da0..0x00498db1: mode = modes[DAT_0077340c] + i*0x18 (both reads repeated every pass).
        std::uint8_t* const mode =
            (*reinterpret_cast<std::uint8_t* const* const*>(kModeArrPtrArrPtr))[*reinterpret_cast<int*>(kSubsysIdx)]
            + i * kModeStructStride;
        int* const m = reinterpret_cast<int*>(mode);

        if (*reinterpret_cast<int*>(mode + kModeFlagsOffset) == 0) continue;  // 0x00498dbc jne skip
        if (m[0] < kMinWidth) continue;                                      // 0x00498dc4 jl
        if (m[1] < kMinHeight) continue;                                     // 0x00498dcd jl

        FormatModeString_Thunk(mode_str, m);                                 // 0x00498dd4
        const LRESULT pos = SendMessageA(hModeCombo, kCB_ADDSTRING_d, 0,
                                         reinterpret_cast<LPARAM>(mode_str));        // 0x00498ded
        SendMessageA(hModeCombo, kCB_SETITEMDATA_d, static_cast<WPARAM>(pos),
                     static_cast<LPARAM>(i));                                         // 0x00498df9
        reinterpret_cast<int*>(kModeIdxMapBase)[i] = static_cast<int>(pos);          // 0x00498dfb
    }
}

RH_ScopedInstall(PopulateModeCombo_s2, 0x00498d60);  // re-enabled 2026-05-24 batch-save-d


// ============================================================================
// 0x004991f0  VideoSettingsDlgProc_s2
//
// Original 0x004991f0..0x004993b5 + tables 0x004993b8 (6 dword targets) / 0x004993d0 (0x2f byte indices).
// Re-ported from the bytes 2026-10-12 (negctl re-earn). __stdcall DLGPROC: every exit is ret 0x10 (the old export
// returned with a plain ret).
//
// uMsg 0x110 (0x004991f4 sub/je 0x00499393): 0x00498f60 with EAX = hDlg and three pushed args 0x110, wParam,
//   lParam (0x0049939b..0x004993a6, add esp,0xc at 0x004993ab; the callee does not read them); return 1.
// uMsg 0x111 (0x004991ff dec/jne): id = LOWORD(wParam) (0x0049920a movzx), hi = wParam >> 16 (shr ecx,0x10):
//   1      0x0049924c: hi != 0 -> return 1; else 0x00498d20(EAX = hDlg), EndDialog(hDlg, 1), return 1.
//   2      0x0049922d: hi != 0 -> return 1; else EndDialog(hDlg, hi == 0), return 1.
//   0x3e8  0x00499275: 0x00499170(EAX = hDlg), return 1. NO notification test (the old port gated on hi == 1).
//   0x3e9  0x0049929b: hi == 1 -> 0x00498d20(EAX = hDlg); return 1 either way.
//   0x3eb  0x0049928d -> 0x004993ae return 1.   0x3ec 0x00499218 -> return 1.   0x3ed table slot 0 -> return 1.
//   0x417  slot 1 0x004992d0: DAT_007733a8 = (SendMessageA(GetDlgItem(hDlg,0x417), BM_GETCHECK 0xf0, 0, 0) == 0)
//   0x418  slot 2 0x00499302: DAT_007733a4 = (... 0x418 ... == 0)
//   0x419  slot 3 0x00499334: DAT_007733ac =  ... 0x419 ...        (raw, no neg/sbb/inc)
//   0x41b  slot 4 0x00499361: DAT_007733b0 = (... 0x41b ... == 0)
//          the four checkbox arms return 1 and do NOT test hi (the old port gated them on hi == 0).
//   0x3ea, 0x41a, 0x3ee..0x416, any id > 0x41b or < 0x3e8 other than 1/2: return 0 (0x00499296).
// any other uMsg: return 0.
// ============================================================================

static constexpr std::uintptr_t kDlgChkA_Global = 0x007733a8u;  // 0x004992f5
static constexpr std::uintptr_t kDlgChkB_Global = 0x007733a4u;  // 0x00499327
static constexpr std::uintptr_t kDlgChkC_Global = 0x007733acu;  // 0x00499354
static constexpr std::uintptr_t kDlgChkD_Global = 0x007733b0u;  // 0x00499386

static constexpr UINT kBM_GETCHECK_d = 0xf0;  // 0x004992d8 etc.

// Call helpers (not ports): the three callees below take hDlg in EAX. VideoDialogInit additionally gets three cdecl
// stack args it never reads (call at 0x004993a6, add esp,0xc at 0x004993ab); they are pushed here for fidelity.
static void __cdecl VideoDialogInit_Invoke(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    __asm {
        push    lParam                // 0x0049939b push eax ([esp+0x10] = lParam)
        push    wParam                // 0x004993a0 push ecx ([esp+0xc]  = wParam)
        push    0x110                 // 0x004993a1
        mov     eax, hDlg             // 0x0049939c mov eax,[esp+8]
        mov     ecx, 0x00498f60
        call    ecx                   // 0x004993a6
        add     esp, 0x0c             // 0x004993ab
    }
}

static void __cdecl SubsystemSelChanged_Invoke(HWND hDlg)
{
    __asm {
        mov     eax, hDlg             // 0x00499275
        mov     ecx, 0x00499170
        call    ecx                   // 0x00499279
    }
}

static void __cdecl ReadModeFromCombo_Invoke(HWND hDlg)
{
    __asm {
        mov     eax, hDlg             // 0x0049925c / 0x004992a7
        mov     ecx, 0x00498d20
        call    ecx                   // 0x0049925e / 0x004992ab
    }
}

static INT_PTR __stdcall VideoSettingsDlgProc_s2_Body(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Exported naked entry, __stdcall shape: re-pushes the four args for the __stdcall body, then ret 0x10 like every
// exit of the original (0x00499249, 0x00499272, 0x00499283, 0x00499298, ..., 0x004993b3).
extern "C" __declspec(dllexport) __declspec(naked)
void __cdecl VideoSettingsDlgProc_s2()
{
    __asm {
        push    dword ptr [esp+16]    // lParam
        push    dword ptr [esp+16]    // wParam
        push    dword ptr [esp+16]    // uMsg
        push    dword ptr [esp+16]    // hDlg
        call    VideoSettingsDlgProc_s2_Body
        ret     16
    }
}

RH_ScopedInstall(VideoSettingsDlgProc_s2, 0x004991f0);  // re-enabled 2026-05-24 batch-save-e

static int ReadCheck(HWND hDlg, int id)
{
    return static_cast<int>(SendMessageA(GetDlgItem(hDlg, id), kBM_GETCHECK_d, 0, 0));
}

static INT_PTR __stdcall VideoSettingsDlgProc_s2_Body(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == 0x110) {                                   // 0x004991f4 sub eax,0x110 ; je 0x00499393
        VideoDialogInit_Invoke(hDlg, wParam, lParam);      // 0x004993a6
        return 1;                                          // 0x004993ae
    }
    if (uMsg != 0x111) return 0;                           // 0x004991ff dec ; jne 0x00499296

    const unsigned id = wParam & 0xffffu;                  // 0x0049920a movzx eax,cx
    const unsigned hi = static_cast<unsigned>(wParam) >> 16;  // shr ecx,0x10

    switch (id) {
    case 1:                                                // 0x0049924c
        if (hi != 0) return 1;                             // 0x00499251 jne 0x004993ae
        ReadModeFromCombo_Invoke(hDlg);                    // 0x0049925e
        EndDialog(hDlg, 1);                                // 0x00499266
        return 1;
    case 2:                                                // 0x0049922d
        if (hi != 0) return 1;                             // 0x00499232 jne 0x004993ae
        EndDialog(hDlg, static_cast<INT_PTR>(hi));         // 0x0049923c push ecx (= 0) ; 0x0049923e
        return 1;
    case 0x3e8:                                            // 0x00499225 je 0x00499275, no hi test
        SubsystemSelChanged_Invoke(hDlg);                  // 0x00499279
        return 1;
    case 0x3e9:                                            // 0x0049928b je 0x0049929b
        if (hi == 1) ReadModeFromCombo_Invoke(hDlg);       // 0x0049929e cmp ecx,1 ; 0x004992ab
        return 1;
    case 0x3eb:                                            // 0x00499290
    case 0x3ec:                                            // 0x00499218
    case 0x3ed:                                            // table slot 0 -> 0x004993ae
        return 1;
    case 0x417:                                            // slot 1 0x004992d0
        *reinterpret_cast<int*>(kDlgChkA_Global) = (ReadCheck(hDlg, 0x417) == 0) ? 1 : 0;  // neg/sbb/inc
        return 1;
    case 0x418:                                            // slot 2 0x00499302
        *reinterpret_cast<int*>(kDlgChkB_Global) = (ReadCheck(hDlg, 0x418) == 0) ? 1 : 0;
        return 1;
    case 0x419:                                            // slot 3 0x00499334
        *reinterpret_cast<int*>(kDlgChkC_Global) = ReadCheck(hDlg, 0x419);
        return 1;
    case 0x41b:                                            // slot 4 0x00499361
        *reinterpret_cast<int*>(kDlgChkD_Global) = (ReadCheck(hDlg, 0x41b) == 0) ? 1 : 0;
        return 1;
    default:                                               // 0x00499296 / table slot 5
        return 0;
    }
}
