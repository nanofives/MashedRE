// Mashed RE - Save/SettingsDialog_j5 cluster (C2->C3 promotions, session c3-batch-j-s5).
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Hooks in this file (1 — others in batch deferred or refused, see report):
//   0x00498d20  ReadModeFromCombo_j5         WM_COMMAND IDOK handler (HWND via EAX)
//
// Analysis note (per CLAUDE.md NO-GUESSING rule):
//   re/analysis/promote_c2_settings_dialog/00498d20.md  (C2 plate)
//
// Calling-convention notes:
//   0x00498d20 is the IDOK / mode-combo CBN_SELCHANGE handler; it takes HWND
//   in EAX implicitly (Ghidra in_EAX). We implement it as __declspec(naked)
//   with `push eax` to bridge EAX into the body's __stdcall first slot, then
//   defer to a body function. Same pattern as the i3-batch dialog helpers
//   (VideoDialogInit_i3 / SubsystemSelectionChanged_i3 / SetControlTextFromResource_i3).
//
// Refusal record (see PROMOTION_QUEUE.md row for batch-j-s5):
//   - 0x00498d60 PopulateModeCombo — no internal callee at C2+
//     (only callee FUN_00498a00 is render C1). Fails C3 rubric.
//   - 0x004991f0 VideoSettingsDlgProc — 453-byte DLGPROC deferred per
//     STOP-AND-ASK budget split.
//   - 0x00499400 VideoSettingsDispatcher — 652-byte deferred per
//     STOP-AND-ASK + multiple C1 callees on hot path.

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

RH_ScopedInstall(ReadModeFromCombo_j5, 0x00498d20);
