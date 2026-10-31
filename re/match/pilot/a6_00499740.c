// FLAGS: /O2 /GS /Ob1
/* 0x00499740 (100 B). Best 100.0% exact (regs 100, ops 100), first try from the Ghidra decomp.
   /GS cookie prologue + tail jmp to __security_check_cookie (0x004a2be9) reproduced by ddk cl.
   Residual: none. */
typedef void *HWND; typedef void *HINSTANCE; typedef unsigned int UINT; typedef int BOOL;
typedef char *LPSTR; typedef const char *LPCSTR;
__declspec(dllimport) int __stdcall LoadStringA(HINSTANCE h, UINT id, LPSTR buf, int n);
__declspec(dllimport) BOOL __stdcall SetWindowTextA(HWND w, LPCSTR s);
extern HINSTANCE DAT_007e9580;

// MATCH: 0x00499740 _FUN_00499740
void FUN_00499740(HWND wnd, UINT id)
{
    char buf[256];
    LoadStringA(DAT_007e9580, id, buf, 0x100);
    SetWindowTextA(wnd, buf);
}
