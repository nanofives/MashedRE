// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d6b90 (h3): 100% (regs 100%), --bytes identical. Raw 87.0%: the device slot +0x114 is __stdcall (no add esp
   after the calls). Table at 0x005d8ca8 is {a, b} pairs indexed by param_1 (declared N=1: its length is not known). */
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
typedef struct Dev6b90 Dev6b90;
typedef int (__stdcall *Fn114)(Dev6b90 *, int, int, int);
struct Dev6b90 { void **vtbl; };
typedef struct Pair6b90 { int a; int b; } Pair6b90;
MGLOBAL(int, DAT_007d6b44, 0x007d6b44);
MGLOBAL(Dev6b90 *, DAT_007d4110, 0x007d4110);
MGLOBAL(int, DAT_007d4f88, 0x007d4f88);
MGLOBAL(int, DAT_007d6b3c, 0x007d6b3c);
MGLOBAL(int, DAT_007d4f78, 0x007d4f78);
MGLOBAL(int, DAT_007d4f74, 0x007d4f74);
MGLOBAL(int, DAT_007d4f7c, 0x007d4f7c);
MGLOBALA(Pair6b90, DAT_005d8ca8, 1, 0x005d8ca8);
// MATCH: 0x004d6b90 FUN_004d6b90
int FUN_004d6b90(int param_1)
{
  int iVar1;
  int uVar2;

  if (1 < DAT_007d6b44) {
    DAT_007d6b44 = 1;
    DAT_007d4f88 = 1;
    ((Fn114)DAT_007d4110->vtbl[0x114 / 4])(DAT_007d4110, 0, 10, 1);
  }
  if (param_1 != DAT_007d6b3c) {
    iVar1 = DAT_005d8ca8[param_1].b;
    uVar2 = DAT_005d8ca8[param_1].a;
    DAT_007d6b3c = param_1;
    DAT_007d4f78 = uVar2;
    ((Fn114)DAT_007d4110->vtbl[0x114 / 4])(DAT_007d4110, 0, 6, uVar2);
    DAT_007d4f74 = uVar2;
    ((Fn114)DAT_007d4110->vtbl[0x114 / 4])(DAT_007d4110, 0, 5, uVar2);
    if (DAT_007d4f7c != iVar1) {
      DAT_007d4f7c = iVar1;
      ((Fn114)DAT_007d4110->vtbl[0x114 / 4])(DAT_007d4110, 0, 7, iVar1);
    }
  }
  return 1;
}
