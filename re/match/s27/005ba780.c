// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 100.0%. The three vtable +8 calls are __stdcall (typedef FUN_005ba780_vfn), no caller pop. Class: none. */
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
typedef unsigned char byte;
typedef int code();
void FUN_005aaa00(int p0, undefined4 p1, undefined4 p2);
void FUN_005ba760(int p0);
void FUN_005baf40(int p0, undefined4 p1);
void FUN_005bb000(int p0, undefined4 * p1);
void FUN_005bbf30(int p0);
typedef void (__stdcall *FUN_005ba780_vfn)(int *);
// MATCH: 0x005ba780 FUN_005ba780
void FUN_005ba780(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x20);
  FUN_005bb000(param_1,0);
  FUN_005baf40(param_1,0);
  iVar1 = *(int *)(iVar1 + 0xf4);
  if (iVar1 != 0) {
    FUN_005aaa00(iVar1,0,0);
  }
  iVar1 = *(int *)(param_1 + 0x20);
  if ((*(byte *)(iVar1 + 0x50) & 0x40) == 0) {
    (*(FUN_005ba780_vfn *)(**(int **)(iVar1 + 0x68) + 8))(*(int **)(iVar1 + 0x68));
    (*(FUN_005ba780_vfn *)(**(int **)(iVar1 + 100) + 8))(*(int **)(iVar1 + 100));
  }
  FUN_005bbf30(iVar1 + 0x58);
  (*(FUN_005ba780_vfn *)(**(int **)(iVar1 + 0x54) + 8))(*(int **)(iVar1 + 0x54));
  FUN_005ba760(param_1);
  return;
}
