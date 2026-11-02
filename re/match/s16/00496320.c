// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00496320 (h3): by hand, 100% (regs 100%), relocheck OK. Three separate
   `if (hr < 0) return 0;` checks then `return 1` (the DDK folds the last one to setge). The nested and
   `||` forms place the shared `return 0` after the second check (86.7%). Takes one stack arg (HWND for
   the vtable +0x34 call). */
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
typedef int (__stdcall *PFN_CreateDevice)(int *self, void *guid, int **out, void *outer);
typedef int (__stdcall *PFN_SetDataFormat)(int *self, void *fmt);
typedef int (__stdcall *PFN_SetCoopLevel)(int *self, void *hwnd, int flags);
MGLOBAL(int *, DAT_00772fb4, 0x00772fb4);
MGLOBAL(int *, DAT_00772fb8, 0x00772fb8);
MGLOBALA(undefined1, DAT_005d0974, 24, 0x005d0974);
MGLOBALA(undefined1, DAT_005d09ec, 16, 0x005d09ec);
// MATCH: 0x00496320 FUN_00496320
int FUN_00496320(void *hwnd)
{
  if ((*(PFN_CreateDevice *)(*DAT_00772fb4 + 0xc))(DAT_00772fb4, DAT_005d09ec, &DAT_00772fb8, 0) < 0) {
    return 0;
  }
  if ((*(PFN_SetDataFormat *)(*DAT_00772fb8 + 0x2c))(DAT_00772fb8, DAT_005d0974) < 0) {
    return 0;
  }
  if ((*(PFN_SetCoopLevel *)(*DAT_00772fb8 + 0x34))(DAT_00772fb8, hwnd, 6) < 0) {
    return 0;
  }
  return 1;
}
