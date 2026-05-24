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
//   0x00498d20 / 0x00498d60 take HWND in EAX implicitly (Ghidra in_EAX).
//   Both are implemented as __declspec(naked) entry bridging EAX to body.
//   0x004991f0 is a standard Win32 DLGPROC: (HWND, UINT, WPARAM, LPARAM) -> INT_PTR.
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

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(ReadModeFromCombo_j5, 0x00498d20);


// ============================================================================
// 0x00498d60  PopulateModeCombo_s2
//
// Original 219 bytes (0x00498d60..0x00498e3a).  HWND via EAX (Ghidra in_EAX).
// Fills the mode combo with all fullscreen modes >= 640x480 for the current
// subsystem (DAT_0077340c).  Builds the DAT_00773418 index map.
//
// Analysis note: re/analysis/promote_c2_settings_dialog/00498d60.md (C2)
//
// Anti-island:
//   Callers: VideoDialogInit_i3 (0x00498f60, C3), SubsystemSelectionChanged_i3
//            (0x00499170, C3).
//   Callees: FormatDisplayModeString (0x00498a00, C1 with EBX-implicit convention —
//            reimpl calls via naked thunk); SendMessageA, CB_ADDSTRING,
//            CB_SETITEMDATA (Win32 — satisfy callee rule).
//
// Globals read (cited at 0x00498d60 body):
//   DAT_0077340c — current subsystem index
//   DAT_007731f8 — int array: mode count per subsystem
//   DAT_00773408 — ptr array: pointer to mode array per subsystem
//
// Globals written (cited at 0x00498d60 body):
//   DAT_00773418[i] — valid-mode-index to combo-position map
//
// Mode filter (all three must pass, cited at 0x00498d60 body):
//   flags (mode+0x0C) != 0
//   width (mode+0x00) > 0x27f (i.e. > 639, meaning >= 640)
//   height (mode+0x04) > 0x1df (i.e. > 479, meaning >= 480)
//
// Mode struct stride: 0x18 bytes (cited at 0x00498d60 body).
//
// FormatDisplayModeString calling convention (U-1667):
//   0x00498a00 uses EBX as implicit mode-struct ptr (not a formal arg).
//   param_1 is the output buffer (char[]).
//   We call via naked thunk FormatModeString_Thunk(buf, mode_ptr) which
//   sets EBX = mode_ptr and calls the game function.
//
// CB_ADDSTRING returns WPARAM (combo position). CB_SETITEMDATA stores the
// valid-mode loop index as item data. DAT_00773418[lParam] = WPARAM.
// ============================================================================

// Globals cited at 0x00498d60 body
// NOTE on DAT_007731f8 / DAT_00773408 — access pattern analysis:
//   `*(int*)(iVar1 + DAT_007731f8)` where iVar1 = subsys_idx * 4.
//   VideoModeTableInit (0x00498c00, C2) ALLOCATES heap arrays and stores the
//   BASE POINTERS in DAT_007731f8 and DAT_00773408. So these BSS slots hold pointers.
//   The Ghidra expression `DAT_007731f8` = VALUE at BSS[0x007731f8] = heap ptr.
//   Then `iVar1 + heap_ptr` = element address in heap array.
//   Correct access: heap_ptr = *((int*)0x007731f8); mode_count = heap_ptr[subsys_idx].
//   kModeCountArrPtr = BSS address that HOLDS the heap pointer (not the array itself).
//   kModeArrPtrArrPtr = BSS address that HOLDS the heap ptr-array pointer.
static constexpr std::uintptr_t kSubsysIdx        = 0x0077340cu;  // current subsystem index (int)
static constexpr std::uintptr_t kModeCountArrPtr  = 0x007731f8u;  // holds ptr to heap int[] mode counts
static constexpr std::uintptr_t kModeArrPtrArrPtr = 0x00773408u;  // holds ptr to heap ptr[] mode arrays
static constexpr std::uintptr_t kModeIdxMapBase   = 0x00773418u;  // valid-mode -> combo pos map (BSS)

// Mode filter thresholds (cited at 0x00498d60 body)
static constexpr int kMinWidth  = 0x27f;  // 639; width must be > 639 (>= 640)
static constexpr int kMinHeight = 0x1df;  // 479; height must be > 479 (>= 480)

// Mode struct offsets (cited at 0x00498d60 body)
static constexpr int kModeFlagsOffset  = 0x0c;  // 12
static constexpr int kModeStructStride = 0x18;  // 24

// Win32 combo messages (cited at 0x00498d60 body)
static constexpr UINT kCB_ADDSTRING_d  = 0x143;  // 323
static constexpr UINT kCB_SETITEMDATA_d = 0x151; // 337

// Output buffer size for FormatDisplayModeString — 0x7f bytes per mechanical desc
static constexpr int kModeStrBufSize = 0x7f;

// FormatDisplayModeString (0x00498a00): EBX-implicit calling convention.
// Naked thunk: param_1 = buf ptr (via stack [esp+4]);
//              param_2 = mode struct ptr set into EBX before call.
// Declared as __cdecl(buf, mode_ptr) — body sets EBX = mode_ptr, then calls.
// FormatDisplayModeString entry (0x00498a00; cited at 0x00498d60 body callee list)
// Note: calling convention uses EBX as implicit mode-struct ptr. We set EBX
// before the call; ECX is used as scratch for the call target.
static void __cdecl FormatModeString_Thunk(char* buf, int* mode_ptr)
{
    // MSVC inline asm: load target RVA into ECX, then call via ECX.
    // EBX = mode_ptr (unaff_EBX at original call site in FUN_00498d60).
    // buf  = output string buffer (param_1 to 0x00498a00).
    __asm {
        push    ebx                         // preserve EBX (callee-saved)
        mov     ebx, mode_ptr               // EBX = mode struct ptr (unaff_EBX at call)
        push    buf                         // param_1 = output buffer
        mov     ecx, 0x00498a00             // FormatDisplayModeString RVA
        call    ecx                         // call 0x00498a00
        add     esp, 4                      // clean param_1 (callee is __cdecl)
        pop     ebx                         // restore EBX
    }
}

static void __stdcall PopulateModeCombo_Body(HWND hModeCombo)
{
    // 0x00498d60: iVar1 = DAT_0077340c * 4  (byte offset into heap arrays)
    const int subsys_idx  = *reinterpret_cast<int*>(kSubsysIdx);

    // 0x00498d60: *(int*)(iVar1 + DAT_007731f8)
    //   DAT_007731f8 holds heap array base pointer; index = subsys_idx.
    //   The heap array was allocated by VideoModeTableInit (0x00498c00, C2).
    const int* const mode_count_base =
        *reinterpret_cast<int* const*>(kModeCountArrPtr);  // 0x007731f8 = heap ptr

    if (mode_count_base == nullptr) return;  // array not yet allocated
    const int mode_count = mode_count_base[subsys_idx];  // 0x00498d60 body

    if (mode_count <= 0) return;  // 0x00498d60: if (0 < mode_count) { ... }

    // Output buffer for FormatDisplayModeString: 0x7f-byte local
    char mode_str[kModeStrBufSize];

    // 0x00498d60: *(ptr*)(iVar1 + DAT_00773408)
    //   DAT_00773408 holds array of mode-struct-array pointers; index = subsys_idx.
    std::uint8_t* const* const mode_arr_ptr_base =
        *reinterpret_cast<std::uint8_t* const* const*>(kModeArrPtrArrPtr);  // 0x00773408 = heap ptr
    if (mode_arr_ptr_base == nullptr) return;  // ptr array not yet allocated
    std::uint8_t* const mode_arr_base = mode_arr_ptr_base[subsys_idx];

    // Valid-mode counter (lParam in original)
    int lParam = 0;

    // 0x00498d60: loop over all modes
    for (int local_48 = 0; ; local_48 += kModeStructStride)
    {
        // piVar2 = mode_arr_base + local_48 (byte offset)
        int* mode_struct = reinterpret_cast<int*>(mode_arr_base + local_48);

        // width at mode+0x00, height at mode+0x04, flags at mode+0x0C
        const int width  = mode_struct[0];              // 0x00498d60 body
        const int height = mode_struct[1];              // 0x00498d60 body
        const int flags  = *reinterpret_cast<const int*>(
            reinterpret_cast<std::uint8_t*>(mode_struct) + kModeFlagsOffset);

        // 0x00498d60: filter: flags!=0 AND width>0x27f AND height>0x1df
        if (flags != 0 && width > kMinWidth && height > kMinHeight)
        {
            // 0x00498d60: FormatDisplayModeString(local_44, mode_struct)
            // EBX = mode_struct (piVar2 at call site), param_1 = local_44 (buf)
            FormatModeString_Thunk(mode_str, mode_struct);

            // 0x00498d60: wParam = SendMessageA(hCombo, CB_ADDSTRING, 0, local_44)
            LRESULT wParam = SendMessageA(hModeCombo, kCB_ADDSTRING_d, 0,
                                          reinterpret_cast<LPARAM>(mode_str));

            // 0x00498d60: SendMessageA(hCombo, CB_SETITEMDATA, wParam, lParam)
            SendMessageA(hModeCombo, kCB_SETITEMDATA_d,
                         static_cast<WPARAM>(wParam),
                         static_cast<LPARAM>(lParam));

            // 0x00498d60: (&DAT_00773418)[lParam] = wParam
            reinterpret_cast<int*>(kModeIdxMapBase)[lParam] =
                static_cast<int>(wParam);
        }

        lParam++;  // 0x00498d60: loop counter always increments

        // Re-read mode count (original re-reads iVar1 = DAT_0077340c * 4 each iteration)
        const int cur_subsys_idx  = *reinterpret_cast<int*>(kSubsysIdx);
        const int cur_mode_count  = mode_count_base[cur_subsys_idx];

        if (lParam >= cur_mode_count) break;  // 0x00498d60: while lParam < mode_count
    }
}

// Naked entry: HWND via EAX, no other args.
extern "C" __declspec(dllexport) __declspec(naked)
void __cdecl PopulateModeCombo_s2()
{
    __asm {
        push    ebp
        mov     ebp, esp
        push    eax                      // hModeCombo from EAX
        call    PopulateModeCombo_Body
        mov     esp, ebp
        pop     ebp
        ret
    }
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(PopulateModeCombo_s2, 0x00498d60);


// ============================================================================
// 0x004991f0  VideoSettingsDlgProc_s2
//
// Original 453 bytes (0x004991f0..0x004993b4).  Standard Win32 DLGPROC.
// Signature: INT_PTR CALLBACK fn(HWND, UINT, WPARAM, LPARAM)
//
// Analysis note: re/analysis/promote_c2_settings_dialog/004991f0.md (C2)
//
// Anti-island:
//   Callers: VideoSettingsDispatcher 0x00499400 (C2) via DialogBoxParamA.
//   Callees:
//     0x00498f60 VideoDialogInit_i3         (C3, this DLL export, HWND via EAX)
//     0x00499170 SubsystemSelectionChanged_i3 (C3, this DLL export, HWND via EAX)
//     0x00498d20 ReadModeFromCombo_j5       (C3, this DLL export, HWND via EAX)
//     EndDialog                             (Win32)
//
// Messages handled (cited at 0x004991f0 body):
//   WM_INITDIALOG (0x110): call VideoDialogInit(hDlg); return TRUE.
//   WM_COMMAND    (0x111): dispatch on LOWORD(wParam):
//     1 (IDOK):    ReadModeFromCombo(hDlg); EndDialog(hDlg, 1).
//     2 (IDCANCEL): EndDialog(hDlg, 0).
//     0x3e8 (1000): if CBN_SELCHANGE: SubsystemSelectionChanged(hDlg).
//     0x3e9 (1001): if CBN_SELCHANGE: ReadModeFromCombo(hDlg).
//     0x3eb (1003): no-op (U-3007).
//     0x3ec (1004): no-op (U-3007).
//     0x3ed (1005): no-op (U-3007).
//     0x417 (1047): if BN_CLICKED: DAT_007733a8 = (BM_GETCHECK==0) [inverted].
//     0x418 (1048): if BN_CLICKED: DAT_007733a4 = (BM_GETCHECK==0) [inverted].
//     0x419 (1049): if BN_CLICKED: DAT_007733ac = BM_GETCHECK [non-inverted].
//     0x41b (1051): if BN_CLICKED: DAT_007733b0 = (BM_GETCHECK==0) [inverted].
//   Default: return FALSE.
//
// Globals written (cited at 0x004991f0 body):
//   DAT_007733a8 <- (BM_GETCHECK==0) for control 0x417 (inverted)
//   DAT_007733a4 <- (BM_GETCHECK==0) for control 0x418 (inverted)
//   DAT_007733ac <- BM_GETCHECK for control 0x419 (non-inverted)
//   DAT_007733b0 <- (BM_GETCHECK==0) for control 0x41b (inverted)
//
// [UNCERTAIN U-3007] Controls 1003/1004/1005: no-op handlers; semantics unclear.
//   Non-blocking: handler is fall-through return TRUE.
// [UNCERTAIN U-3008] Checkbox inversion: 1047/1048/1051 write (BM_GETCHECK==0);
//   1049 writes raw. Semantic meaning unclear. Non-blocking: mechanics unambiguous.
// ============================================================================

// Globals written by checkbox handlers (cited at 0x004991f0 body)
static constexpr std::uintptr_t kDlgChkA_Global = 0x007733a8u;  // control 0x417 (inverted)
static constexpr std::uintptr_t kDlgChkB_Global = 0x007733a4u;  // control 0x418 (inverted)
static constexpr std::uintptr_t kDlgChkC_Global = 0x007733acu;  // control 0x419 (non-inverted)
static constexpr std::uintptr_t kDlgChkD_Global = 0x007733b0u;  // control 0x41b (inverted)

// Win32 button-query message (cited at 0x004991f0 body)
static constexpr UINT kBM_GETCHECK_d = 0xf0;  // 240

// Control IDs cited at 0x004991f0 body
static constexpr int kDlgIdOk      = 1;      // IDOK
static constexpr int kDlgIdCancel  = 2;      // IDCANCEL
static constexpr int kDlgIdSubsys  = 0x3e8;  // 1000
static constexpr int kDlgIdMode    = 0x3e9;  // 1001
// 0x3eb/0x3ec/0x3ed — no-op handlers (U-3007); listed for completeness
static constexpr int kDlgIdNoop1   = 0x3eb;  // 1003
static constexpr int kDlgIdNoop2   = 0x3ec;  // 1004
static constexpr int kDlgIdNoop3   = 0x3ed;  // 1005
static constexpr int kDlgIdChkA    = 0x417;  // 1047
static constexpr int kDlgIdChkB    = 0x418;  // 1048
static constexpr int kDlgIdChkC    = 0x419;  // 1049
static constexpr int kDlgIdChkD    = 0x41b;  // 1051

// Notification codes cited at 0x004991f0 body
static constexpr WORD kCBN_SELCHANGE = 1;   // combo-box selection changed
static constexpr WORD kBN_CLICKED    = 0;   // button clicked

// Internal callee wrappers (C3 exports; HWND via EAX at original call site).
// We invoke the game's hooked RVAs directly: the inline-JMP redirects to our
// reimplemented entry points at runtime.
//
// Pattern: set EAX = hDlg, then call the RVA (which reads EAX implicitly).
// Invoke helpers for EAX-implicit callee hooks.
// Each sets EAX = hDlg, then calls the hooked RVA via ECX (MSVC inline asm
// cannot use an immediate as call target).
static void __cdecl VideoDialogInit_Invoke(HWND hDlg)
{
    // 0x004991f0 body: calls 0x00498f60 with HWND in EAX.
    __asm {
        mov     eax, hDlg
        mov     ecx, 0x00498f60    // VideoDialogInit (C3; cited at 0x004991f0 body)
        call    ecx
    }
}

static void __cdecl SubsystemSelChanged_Invoke(HWND hDlg)
{
    // 0x004991f0 body: calls 0x00499170 with HWND in EAX.
    __asm {
        mov     eax, hDlg
        mov     ecx, 0x00499170    // SubsystemSelectionChanged (C3; cited at 0x004991f0 body)
        call    ecx
    }
}

static void __cdecl ReadModeFromCombo_Invoke(HWND hDlg)
{
    // 0x004991f0 body: calls 0x00498d20 with HWND in EAX.
    __asm {
        mov     eax, hDlg
        mov     ecx, 0x00498d20    // ReadModeFromCombo (C3; cited at 0x004991f0 body)
        call    ecx
    }
}

// Forward declaration needed because the naked entry appears before the body.
static INT_PTR __stdcall VideoSettingsDlgProc_s2_Body(
    HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Exported naked entry — serves as BOTH the Frida export AND the RH_ScopedInstall
// target.  RH_ScopedInstall patches a JMP at 0x004991f0 to this symbol.
//
// Calling convention contract:
//   - DialogBoxParamA calls the original at 0x004991f0 as __stdcall (4 args pushed,
//     callee expected to clean 16 bytes via RET 16).
//   - Our naked entry reads [esp+4..+16], calls the __stdcall body (which cleans its
//     own 16 bytes with RET 16), then does RET to return to DialogBoxParamA with the
//     same stack discipline as the original.
//   - Frida calls with arg_type='none' (0 args pushed); [esp+4..+16] = garbage/0.
//     The body receives uMsg=garbage → falls to default → returns FALSE (0).
//     Both original and reimpl behave identically. crash_equal_ok=True handles any
//     rare crash equality.
extern "C" __declspec(dllexport) __declspec(naked)
void __cdecl VideoSettingsDlgProc_s2()
{
    __asm {
        // Stack layout on entry:
        //   [esp+0]  = return address
        //   [esp+4]  = hDlg
        //   [esp+8]  = uMsg
        //   [esp+12] = wParam
        //   [esp+16] = lParam
        push    dword ptr [esp+16]  // lParam  (push; esp now +4)
        push    dword ptr [esp+16]  // wParam  (original [esp+12] = new [esp+16])
        push    dword ptr [esp+16]  // uMsg    (original [esp+8]  = new [esp+16])
        push    dword ptr [esp+16]  // hDlg    (original [esp+4]  = new [esp+16])
        call    VideoSettingsDlgProc_s2_Body   // __stdcall cleans 16 bytes (ret 16)
        // After call: stack is back to original position; EAX = return value.
        ret                         // plain ret — DialogBoxParamA provided the retaddr
    }
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(VideoSettingsDlgProc_s2, 0x004991f0);

// The actual DLGPROC body (__stdcall, static so no @16 export decoration issue).
static INT_PTR __stdcall VideoSettingsDlgProc_s2_Body(
    HWND   hDlg,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    // 0x004991f0: switch (uMsg)
    switch (uMsg)
    {
    case 0x110:  // WM_INITDIALOG (0x110)
        // 0x004991f0 body: WM_INITDIALOG => call VideoDialogInit(hDlg)
        VideoDialogInit_Invoke(hDlg);
        return TRUE;  // 0x004991f0 body: return 1

    case 0x111:  // WM_COMMAND (0x111)
    {
        // 0x004991f0 body: dispatch on LOWORD(wParam) (control ID)
        const WORD ctrl_id = LOWORD(wParam);
        const WORD notif   = HIWORD(wParam);

        switch (ctrl_id)
        {
        case kDlgIdOk:  // 1 (IDOK)
            // 0x004991f0 body: BN_CLICKED handler — read mode then close
            if (notif == kBN_CLICKED) {
                ReadModeFromCombo_Invoke(hDlg);  // 0x004991f0 body
                EndDialog(hDlg, 1);              // 0x004991f0 body: return code 1
            }
            return TRUE;

        case kDlgIdCancel:  // 2 (IDCANCEL)
            // 0x004991f0 body: BN_CLICKED handler — discard and close
            if (notif == kBN_CLICKED) {
                EndDialog(hDlg, 0);              // 0x004991f0 body: return code 0
            }
            return TRUE;

        case kDlgIdSubsys:  // 0x3e8 (1000) — subsystem combo
            // 0x004991f0 body: CBN_SELCHANGE => SubsystemSelectionChanged
            if (notif == kCBN_SELCHANGE) {
                SubsystemSelChanged_Invoke(hDlg);  // 0x004991f0 body
            }
            return TRUE;

        case kDlgIdMode:  // 0x3e9 (1001) — mode combo
            // 0x004991f0 body: CBN_SELCHANGE => ReadModeFromCombo
            if (notif == kCBN_SELCHANGE) {
                ReadModeFromCombo_Invoke(hDlg);  // 0x004991f0 body
            }
            return TRUE;

        case kDlgIdNoop1:  // 0x3eb (1003) — no-op [U-3007]
        case kDlgIdNoop2:  // 0x3ec (1004) — no-op [U-3007]
        case kDlgIdNoop3:  // 0x3ed (1005) — no-op [U-3007]
            // 0x004991f0 body: fall-through return 1
            return TRUE;

        case kDlgIdChkA:  // 0x417 (1047) — checkbox A [inverted]
            // 0x004991f0 body: BN_CLICKED => write (BM_GETCHECK==0) [inverted]
            if (notif == kBN_CLICKED) {
                LRESULT chk = SendMessageA(
                    GetDlgItem(hDlg, kDlgIdChkA),
                    kBM_GETCHECK_d, 0, 0);
                *reinterpret_cast<int*>(kDlgChkA_Global) =
                    (chk == 0) ? 1 : 0;            // 0x004991f0 body (inverted)
            }
            return TRUE;

        case kDlgIdChkB:  // 0x418 (1048) — checkbox B [inverted]
            if (notif == kBN_CLICKED) {
                LRESULT chk = SendMessageA(
                    GetDlgItem(hDlg, kDlgIdChkB),
                    kBM_GETCHECK_d, 0, 0);
                *reinterpret_cast<int*>(kDlgChkB_Global) =
                    (chk == 0) ? 1 : 0;            // 0x004991f0 body (inverted)
            }
            return TRUE;

        case kDlgIdChkC:  // 0x419 (1049) — checkbox C [non-inverted]
            if (notif == kBN_CLICKED) {
                LRESULT chk = SendMessageA(
                    GetDlgItem(hDlg, kDlgIdChkC),
                    kBM_GETCHECK_d, 0, 0);
                *reinterpret_cast<int*>(kDlgChkC_Global) =
                    static_cast<int>(chk);          // 0x004991f0 body (raw, non-inverted)
            }
            return TRUE;

        case kDlgIdChkD:  // 0x41b (1051) — checkbox D [inverted]
            if (notif == kBN_CLICKED) {
                LRESULT chk = SendMessageA(
                    GetDlgItem(hDlg, kDlgIdChkD),
                    kBM_GETCHECK_d, 0, 0);
                *reinterpret_cast<int*>(kDlgChkD_Global) =
                    (chk == 0) ? 1 : 0;            // 0x004991f0 body (inverted)
            }
            return TRUE;

        default:
            // 0x004991f0 body: unhandled control -> fall through to return FALSE
            break;
        }
        break;
    }

    default:
        break;
    }

    return FALSE;  // 0x004991f0 body: unhandled message
}

// (RH_ScopedInstall for VideoSettingsDlgProc_s2 appears above the body, after the
// naked entry definition.)
