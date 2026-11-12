// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005c7500 : 89.3% (regs 89.3, ops 100). By hand: the original float->int is inline (fnstcw / or ah,0xc / fldcw / fistp), not __ftol, so one inline __asm macro block sits in the compiled body (allowed by the brief); it writes the new control word into param_1's own stack slot (mov word ptr param_1) and p=param_1 keeps the pointer in esi. The 64-bit divide is a 4-arg call 0x004a4e40 (unsigned div). Remaining diff: the saved control word lands at [esp+4] instead of [esp+6]; local order not movable by declaration order, struct, array or pad variants. Residual class (b). */

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

unsigned __int64 __stdcall FUN_004a4e40(unsigned int p0, unsigned int p1, unsigned int p2, unsigned int p3);
// MATCH: 0x005c7500 FUN_005c7500
void FUN_005c7500(int param_1)
{
  int p = param_1;
  float f;
  int n;
  short cw;

  f = *(float *)(p + 0x38) * *(float *)(p + 0x34);
  __asm {
    fnstcw cw
    fld f
    mov ax, cw
    or ah, 0xc
    mov word ptr param_1, ax
    fldcw word ptr param_1
    fistp n
    fldcw cw
  }
  *(unsigned __int64 *)(p + 0x80) =
      FUN_004a4e40(0, (unsigned int)n, *(unsigned int *)(*(int *)(p + 0x9c) + 0x18), 0);
  *(unsigned short *)(p + 0x80) = 0;
}
