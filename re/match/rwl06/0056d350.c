// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR05 2026-11-08: 100% regs 100%, relocheck OK. Re-derived with a typed 32-byte element table (E **tab, i = c + (b + a*4)*2, (*tab)[i].v[k]); the Ghidra int-offset form gave eax/ecx swapped (55.9%). Supersedes the header below. */
/* MATCH-RWL06 0x0056d350: hand-matched (h2). 55.9% regs 94.9% ops 94.9% under FLAGS /O2 /Ob1 /G5 (raw file was /O1 at 77.0% with a frame-pointer prologue that the original lacks). Instruction stream identical; only the eax/ecx assignment differs (orig: iVar1 in ecx, param_3 loaded into eax after it; ours the reverse). Dot terms written in original x87 order (+0x18,+0x14,+0x10). Tried operand swaps, shift spellings, unsigned, permuter 166 compiles. Class b. */
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
// MATCH: 0x0056d350 FUN_0056d350
typedef struct { float v[8]; } E;
void FUN_0056d350(E **tab,float *out,float *m,int a,int b,int c)
{
  int i;

  i = c + (b + a * 4) * 2;
  out[0] = (*tab)[i].v[0] * m[3];
  out[1] = (*tab)[i].v[1] * m[3];
  out[2] = (*tab)[i].v[2] * m[3];
  out[4] = (*tab)[i].v[6] * m[2] + (*tab)[i].v[5] * m[1] + (*tab)[i].v[4] * m[0];
  out[5] = (*tab)[i].v[6] * m[6] + (*tab)[i].v[5] * m[5] + (*tab)[i].v[4] * m[4];
  out[6] = (*tab)[i].v[6] * m[10] + (*tab)[i].v[5] * m[9] + (*tab)[i].v[4] * m[8];
}
