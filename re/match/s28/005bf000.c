// FLAGS: /O2 /Ob1 /G5
// hand-fixed from raw (SIZE_T undeclared)
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
typedef unsigned int undefined4;
typedef void *LPVOID;
__declspec(dllimport) LPVOID __stdcall CoTaskMemAlloc(unsigned long cb);
short *FUN_005c2f0a(short *p0, short *p1);
int FUN_005c2f58(short *p0);
// MATCH: 0x005bf000 FUN_005bf000
undefined4 __stdcall FUN_005bf000(int param_1, undefined4 *param_2)
{
  int iVar1;
  LPVOID pvVar2;

  iVar1 = FUN_005c2f58(*(short **)(param_1 + 0x14));
  pvVar2 = CoTaskMemAlloc(iVar1 * 2 + 2);
  *param_2 = (undefined4)pvVar2;
  if (pvVar2 != (LPVOID)0x0) {
    FUN_005c2f0a((short *)pvVar2, *(short **)(param_1 + 0x14));
    return 0;
  }
  return 0x8007000e;
}
