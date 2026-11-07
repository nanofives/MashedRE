// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005bc690: 43.8% (regs 67.4%). Hand: DirectSound-style COM calls via __stdcall vfn types (vtable at *obj; embedded object at +8 ints), FUN_005be140 is 3-arg cdecl, &param_1 is the out arg of the +0x58 call (param slot reused). Residual (b): original hoists param loads into eax/ebx/ebp before the first call and lays the error return inline after jge; block layout / push scheduling not reproduced (positive-branch variant scored lower). */
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
typedef int (__stdcall *vfn2)(void *, int);
typedef int (__stdcall *vfn8)(void *, int, void *, int, int, int, int, int);
typedef int (__stdcall *vfn5)(void *, int, int, void *, void *);
void FUN_005be140(void *p0, int p1, int p2);
// MATCH: 0x005bc690 FUN_005bc690
undefined4 *FUN_005bc690(undefined4 *param_1, int param_2, int param_3, int param_4)
{
  undefined4 *piVar1;
  undefined4 *puVar2;
  int iVar3;
  int *dev;
  undefined4 st[6];

  piVar1 = param_1 + 2;
  FUN_005be140(piVar1, param_2, param_4);
  (*(vfn2 *)(*(int *)piVar1 + 0xc))(piVar1, param_4);
  dev = (int *)*param_1;
  iVar3 = (*(vfn8 *)(*dev + 0x54))(dev, 0, piVar1, 1, 0, 0, 0, 0);
  if (iVar3 < 0) {
    FUN_005be140(piVar1, 0, 0);
    return (undefined4 *)0;
  }
  FUN_005be140(piVar1, 0, 0);
  puVar2 = param_1 + 8;
  FUN_005be140(puVar2, param_3, param_4);
  st[0] = (undefined4)puVar2;
  st[1] = 0;
  st[2] = 0;
  st[3] = 0;
  st[4] = 0;
  st[5] = 0;
  dev = (int *)*param_1;
  iVar3 = (*(vfn5 *)(*dev + 0x58))(dev, 1, 0, st, &param_1);
  if (iVar3 < 0) {
    FUN_005be140(puVar2, 0, 0);
    return (undefined4 *)0;
  }
  return param_1;
}
