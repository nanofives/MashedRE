// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004c8690: 100% (regs 100%). Branch order (non-NULL pool first) and the 64-byte copy as a struct assignment (rep movsd). */
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
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d4568, 0x007d4568);
MGLOBAL(undefined4, DAT_007d4154, 0x007d4154);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
typedef struct Blob64 { undefined4 a[16]; } Blob64;
MGLOBAL(Blob64 *, DAT_007d4558, 0x007d4558);
MGLOBAL(Blob64, DAT_005d8b90, 0x005d8b90);
MGLOBAL(undefined4, DAT_007d4158, 0x007d4158);
undefined4 FUN_004cc7f0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
int FUN_004ccc50(int * p0);
// MATCH: 0x004c8690 FUN_004c8690
void FUN_004c8690(void)
{
  int iVar1;
  uint uVar2;
  undefined4 *puVar3;
  DAT_007d4154 = 0;
  if (DAT_007d4568 != 0) {
    uVar2 = 0;
    iVar1 = DAT_007d4568;
    do {
      if (*(int *)((int)&DAT_007d4158 + uVar2) != 0) {
        (**(code **)(DAT_007d3ff8 + 0x11c))(iVar1,*(int *)((int)&DAT_007d4158 + uVar2));
        iVar1 = DAT_007d4568;
        *(undefined4 *)((int)&DAT_007d4158 + uVar2) = 0;
      }
      uVar2 = uVar2 + 4;
    } while (uVar2 < 0x410);
    FUN_004ccc50((int *)iVar1);
  }
  else {
    DAT_007d4568 = FUN_004cc7f0(0x40,0xf,0x10,0x30411);
    puVar3 = &DAT_007d4158;
    for (iVar1 = 0x104; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar3 = 0;
      puVar3 = puVar3 + 1;
    }
  }
  DAT_007d4558 = (Blob64 *)(**(code **)(DAT_007d3ff8 + 0x118))(DAT_007d4568,0x30411);
  *DAT_007d4558 = DAT_005d8b90;
}
