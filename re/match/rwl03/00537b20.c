// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR06 2026-11-08: written from the original disassembly (Ghidra-missed, no pc_funcs.csv row) */
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
/* Ghidra-missed (no pc_funcs.csv row). Original body 0x00537b20..0x00537b74 (84 B, ret then 90 padding).
   Selected at 0x005368df (stored to +0x30). Per count step: packs src[3]/src[0]/src[1]/src[2] into one dword
   (0x00537b43..0x00537b5a), src += param_3[0x28/4] (src = param_3+0x24), stores it 4 times at param_1[0] with
   stride param_1[1]. */
typedef struct { unsigned char *ptr; int stride; } VtxIter;
// MATCH: 0x00537b20 FUN_00537b20
void FUN_00537b20(VtxIter *param_1, int param_2, int *param_3)
{
  unsigned int col;
  unsigned char *v;
  int stride;
  unsigned char *src;
  int sstride;

  v = param_1->ptr;
  stride = param_1->stride;
  src = (unsigned char *)param_3[9];
  sstride = param_3[10];
  for (; param_2 > 0; param_2--) {
    col = ((((((unsigned int)src[3] << 8) | src[0]) << 8) | src[1]) << 8) | src[2];
    src += sstride;
    *(unsigned int *)v = col; v += stride;
    *(unsigned int *)v = col; v += stride;
    *(unsigned int *)v = col; v += stride;
    *(unsigned int *)v = col; v += stride;
  }
}
