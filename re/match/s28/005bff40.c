// FLAGS: /O2 /Ob1 /G5
// best 86.1 (ops 94.8; only diff is prologue: original pushes esi/edi before the first test, ours after the two early-return checks). Hand-written: stdcall COM-style method, QueryInterface out-param = &param_1 (address-taken), fields via p = this+0x148. Permuter variant (91.3) corrupted a vtable expression, rejected. Class (b).
// hand-written from decomp (COM-style, out-param = &param_1)
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(undefined1, DAT_005d0b3c, 0x005d0b3c);
typedef int (__stdcall *M1)(int *);
typedef int (__stdcall *M2)(int *, int);
typedef int (__stdcall *M3)(int *, int, int);
typedef int (__stdcall *QIFn)(int *, undefined1 *, int *);
// MATCH: 0x005bff40 FUN_005bff40
int __stdcall FUN_005bff40(int param_1, int *param_2)
{
  uint *p;
  int iVar3;
  int iVar4;

  if (*(int *)(param_1 + 0x13c) != 0) {
    return 1;
  }
  if (*(int *)(*(int *)(param_1 + 0x10) + 0x128) == 0) {
    return -0x7ffbfdd9;
  }
  p = (uint *)(param_1 + 0x148);
  iVar3 = (*(QIFn *)*param_2)(param_2, &DAT_005d0b3c, &param_1);
  if (iVar3 >= 0) {
    iVar4 = (*(M3 *)(*(int *)param_1 + 0x4c))((int *)param_1, 0x30, (int)p);
    if (iVar4 >= 0) {
      (*(M1 *)(*(int *)param_1 + 8))((int *)param_1);
      return 0;
    }
  }
  else {
    p[0] = 0x30;
    p[0xb] = (*(M1 *)(*param_2 + 0x10))(param_2);
    p[8] = 0;
    p[3] = (*(M1 *)(*param_2 + 0x2c))(param_2);
    iVar3 = (*(M2 *)(*param_2 + 0xc))(param_2, (int)(p + 10));
    if (iVar3 < 0) {
      return -0x7ffbfdf5;
    }
    p[1] = 0;
    p[2] = 0;
    iVar3 = (*(M1 *)(*param_2 + 0x3c))(param_2);
    if (iVar3 >= 0) {
      p[2] = p[2] | 4;
    }
    iVar3 = (*(M1 *)(*param_2 + 0x24))(param_2);
    if (iVar3 >= 0) {
      p[2] = p[2] | 2;
    }
    iVar3 = (*(M1 *)(*param_2 + 0x1c))(param_2);
    if (iVar3 >= 0) {
      p[2] = p[2] | 1;
    }
    iVar3 = (*(M3 *)(*param_2 + 0x14))(param_2, (int)(p + 4), (int)(p + 6));
    if (iVar3 >= 0) {
      p[2] = p[2] | 0x110;
    }
    iVar3 = (*(M2 *)(*param_2 + 0x34))(param_2, (int)(p + 9));
    if (iVar3 >= 0) {
      p[2] = p[2] | 8;
    }
    iVar4 = 0;
  }
  return iVar4;
}
