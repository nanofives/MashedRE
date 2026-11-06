// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00499740 (h6): LoadStringA into a 256-byte stack buffer, SetWindowTextA.
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
typedef void *HINSTANCE;
typedef int BOOL;
typedef unsigned int UINT;
MGLOBAL(HINSTANCE, DAT_007e9580, 0x007e9580);
__declspec(dllimport) int __stdcall LoadStringA(HINSTANCE hInstance, UINT uID, char *lpBuffer, int cchBufferMax);
__declspec(dllimport) BOOL __stdcall SetWindowTextA(HWND hWnd, const char *lpString);
// MATCH: 0x00499740 FUN_00499740
void FUN_00499740(HWND param_1, UINT param_2)
{
  char local_104[256];
  LoadStringA(DAT_007e9580, param_2, local_104, 0x100);
  SetWindowTextA(param_1, local_104);
}
