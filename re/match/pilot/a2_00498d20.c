// a2 0x00498d20 (53 B): best 100.0% (regs 100, ops 100).
// Ghidra in_EAX (HWND in EAX) is NOT /GL: cl 13.00.9176 /O2 gives a static function with in-TU
// callers a register (EAX) argument by itself. Reproduced with a stand-in caller (Proc) that calls it
// twice like the dialog proc does at 0x0049925e / 0x004992ab. g2s start was 87.5%.
// Only other change: the nested SendMessageA(..., SendMessageA(...)) pushes the outer 0 early; a temp
// for the CB_GETCURSEL result gives the original order.
// FLAGS: /O2 /GS /Ob1
typedef void *HWND; typedef unsigned int UINT; typedef unsigned int WPARAM; typedef long LPARAM; typedef long LRESULT;
__declspec(dllimport) HWND __stdcall GetDlgItem(HWND h, int id);
__declspec(dllimport) LRESULT __stdcall SendMessageA(HWND h, UINT m, WPARAM w, LPARAM l);
__declspec(dllimport) int __stdcall EndDialog(HWND h, int r);
extern LRESULT DAT_00773200;

// MATCH: 0x00498d20 _FUN_00498d20
static void FUN_00498d20(HWND hDlg)
{
    HWND c = GetDlgItem(hDlg, 0x3e9);
    LRESULT sel = SendMessageA(c, 0x147, 0, 0);
    DAT_00773200 = SendMessageA(c, 0x150, sel, 0);
}

int __stdcall Proc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    if (m == 1) { FUN_00498d20(h); EndDialog(h, 1); return 1; }
    if (m == 2) { FUN_00498d20(h); return 1; }
    return 0;
}
