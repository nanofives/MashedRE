// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005af7d0 (h3): 42.0% exact, regs 44.0%. Residual class (a) original truncates the float with an INLINE fnstcw/or ah,0xc/fldcw/fistp/fldcw sequence (and fstp dword + fld); the DDK calls _ftol. Tried /Op /QIfist /G5-7 /Oi /arch:SSE, (int) casts. Same as 005acda0. */
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
void FUN_005b1140(int p0, int p1, int p2, int p3);
// MATCH: 0x005af7d0 FUN_005af7d0
void FUN_005af7d0(int param_1,undefined4 param_2,int param_3)
{
  uint *puVar1;
  int *piVar2;
  uint **puVar3;

  piVar2 = *(int **)(param_1 + 0x20);
  *(undefined4 *)(param_1 + 0x34) = *(undefined4 *)(param_3 + 0x10);
  if (piVar2 != (int *)(param_1 + 0x20)) {
    puVar3 = (uint **)(*(int *)(*(int *)(param_1 + 0xc) + 0x18) + 0x14);
    do {
      puVar1 = *puVar3;
      puVar3 = puVar3 + 10;
      FUN_005b1140((int)(piVar2 - 0xb), piVar2[0xb], 0, (int)((float)*puVar1 * *(float *)(param_1 + 0x34)));
      piVar2 = (int *)*piVar2;
    } while (piVar2 != (int *)(param_1 + 0x20));
  }
}
