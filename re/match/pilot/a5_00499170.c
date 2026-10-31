/* a5 matching pilot, 0x00499170 (128 B). Best: 100.0% exact (regs 100, ops 100), 49/49 ins (retry 2026-10-30).
   Fix: `static`, defined before a caller in the same file, /Ob1: cl 13.00.9176 passes hDlg in EAX
   (`mov esi, eax` at 0x00499179), plain ret. The earlier 95.9% "(c) register convention" was source shape
   (TU layout), not /GL. The real caller is the dialog proc 0x004991f0 (call site 0x00499275:
   `mov eax,[esp+4]; call 0x00499170`); match.py has no Ghidra size for it ("size None"), so it cannot be
   scored and `caller` below is a minimal stand-in that only provides a non-address-taken call site. */
// FLAGS: /O2 /GS /Ob1
typedef void *HWND; typedef unsigned int UINT; typedef unsigned int WPARAM; typedef long LPARAM; typedef long LRESULT;
__declspec(dllimport) HWND __stdcall GetDlgItem(HWND hDlg, int id);
__declspec(dllimport) LRESULT __stdcall SendMessageA(HWND h, UINT m, WPARAM w, LPARAM l);
extern WPARAM DAT_0077340c;
extern WPARAM DAT_00773200;
int FUN_004c2e70(WPARAM a);
void FUN_00498d60(HWND h);
WPARAM FUN_004c2f00(void);

// MATCH: 0x00499170 _FUN_00499170
static void FUN_00499170(HWND hDlg)
{
    HWND a = GetDlgItem(hDlg, 1000);
    HWND b = GetDlgItem(hDlg, 0x3e9);
    WPARAM sel = SendMessageA(a, 0x147, 0, 0);
    if (sel != DAT_0077340c) {
        DAT_0077340c = SendMessageA(a, 0x150, sel, 0);
        FUN_004c2e70(DAT_0077340c);
        SendMessageA(b, 0x14b, 0, 0);
        FUN_00498d60(b);
        DAT_00773200 = FUN_004c2f00();
        SendMessageA(b, 0x14e, DAT_00773200, 0);
    }
}

int __stdcall caller(HWND h, UINT m, WPARAM w, LPARAM l)
{
    if (m == 0x111 && (w & 0xffff) == 1000) {
        FUN_00499170(h);
        return 1;
    }
    return 0;
}
