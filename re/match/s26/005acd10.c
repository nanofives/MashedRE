// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005acd10 (h3): 78.0% exact, regs 78.0%. Residual class (b) original keeps a zero in ecx as the fall-out return value (mov eax,ecx at a late epilogue); ours xor eax,eax inline. 8 forms tried, flags tried. */
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
undefined4 FUN_005ac9e0(undefined4 *p0, uint *p1);
// MATCH: 0x005acd10 FUN_005acd10
undefined4 FUN_005acd10(int param_1,uint *param_2)
{
  uint n = *(uint *)(param_1 + 0x24);
  undefined4 **base;
  uint i;
  undefined4 r = 0;
  if (n != 0) {
    base = *(undefined4 ***)(param_1 + 0x28);
    i = 0;
    if (n > 0) {
      do {
        if (FUN_005ac9e0(base[i], param_2) != 0) return 1;
        i++;
      } while (i < n);
    }
    return r;
  }
  return 1;
}
