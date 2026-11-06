// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00499820 (h6): window procedure (switch on uMsg, default tail-jumps DefWindowProcA).
   By hand: 100% exact (regs 100%), relocheck OK. Residual: none. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
typedef void *HWND;
typedef unsigned int UINT;
typedef unsigned int WPARAM;
typedef long LPARAM;
typedef long LRESULT;
MGLOBAL(int, DAT_00773918, 0x00773918);
MGLOBAL(int, DAT_0077391c, 0x0077391c);
void FUN_004960e0(int param_1);
void FUN_00496470(void);
int FUN_00496490(void);
__declspec(dllimport) LRESULT __stdcall DefWindowProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
// MATCH: 0x00499820 FUN_00499820
LRESULT __stdcall FUN_00499820(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
  case 1:
    FUN_00496490();
    return 0;
  case 2:
    FUN_00496470();
    DAT_00773918 = 1;
    return 0;
  case 6:
    DAT_0077391c = (wParam == 1);
    FUN_004960e0(DAT_0077391c);
    return 0;
  case 0x100:
    return 0;
  }
  return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}
