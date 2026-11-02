// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00499690 (h6): message pump.
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
typedef int BOOL;
typedef unsigned int UINT;
typedef unsigned int WPARAM;
typedef long LPARAM;
typedef long LRESULT;
typedef unsigned long DWORD;
typedef struct tagPOINT { long x; long y; } POINT;
typedef struct tagMSG { HWND hwnd; UINT message; WPARAM wParam; LPARAM lParam; DWORD time; POINT pt; } MSG;
MGLOBAL(MSG, DAT_007e95a0, 0x007e95a0);
MGLOBAL(int, DAT_0077391c, 0x0077391c);
MGLOBAL(int, DAT_00773918, 0x00773918);
__declspec(dllimport) BOOL __stdcall PeekMessageA(MSG *lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
__declspec(dllimport) BOOL __stdcall TranslateMessage(const MSG *lpMsg);
__declspec(dllimport) LRESULT __stdcall DispatchMessageA(const MSG *lpMsg);
__declspec(dllimport) BOOL __stdcall WaitMessage(void);
// MATCH: 0x00499690 FUN_00499690
int FUN_00499690(void)
{
  if (PeekMessageA(&DAT_007e95a0, 0, 0, 0, 1) != 0) {
    if (DAT_007e95a0.message == 0x12) {
      return 1;
    }
    TranslateMessage(&DAT_007e95a0);
    DispatchMessageA(&DAT_007e95a0);
  }
  if (DAT_0077391c == 0) {
    WaitMessage();
  }
  return DAT_00773918 != 0;
}
