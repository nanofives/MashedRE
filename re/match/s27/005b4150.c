// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005b4150: 90.0% (regs 90%). Compiled-body macro FTOL_INLINE (__asm fnstcw/fld/or ah,12/fldcw/fistp/fldcw) reproduces the original's inline truncation (54 -> 75%); unsigned operands for the int->float converts (75 -> 90%). Residual: original divides/multiplies with fidiv/fimul dword on a zero-extended stack temp, ours uses fild qword + fdivp/fmulp; unsigned/int/__int64 spellings tried. Class: b.
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
typedef unsigned int uint;
typedef unsigned char byte;
int FUN_005b4060(int *p0, int *p1);
// Compiled-body macro: the original inlines the truncating float->int conversion (fnstcw/fldcw/fistp).
#define FTOL_INLINE(r, t, cw, cw2) \
  __asm { fnstcw cw } __asm { fld t } __asm { mov ax, cw } __asm { or ah, 12 } \
  __asm { mov cw2, ax } __asm { fldcw cw2 } __asm { fistp r } __asm { fldcw cw }
// MATCH: 0x005b4150 FUN_005b4150
uint FUN_005b4150(int param_1,int param_2,int param_3)
{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  float t;
  short cw;
  short cw2;
  int r;

  bVar1 = *(byte *)(param_3 + 0xc);
  uVar3 = (uint)(bVar1 >> 3);
  uVar2 = FUN_005b4060((int *)param_1,(int *)param_3);
  t = ((float)uVar2 / (float)*(uint *)(param_1 + 8)) * (float)(uint)param_2;
  FTOL_INLINE(r, t, cw, cw2)
  uVar2 = r;
  if ((uVar2 & uVar3 - 1) != 0) {
    uVar2 = ((uVar2 - (uVar3 >> 1)) / uVar3) * uVar3;
  }
  return uVar2;
}
