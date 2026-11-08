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
/* Ghidra-missed (no pc_funcs.csv row). Original body 0x00537d70..0x00537dda (106 B, ret then 90 padding).
   Selected at 0x00536878 (stored to +0x2c). src = param_3[0x3c/4], src stride param_3[0x40/4]. Per count step
   writes 4 (u,v) dword pairs at param_1[0] with stride param_1[1]: (s[0],s[3]) (s[2],s[3]) (s[2],s[1]) (s[0],s[1])
   (0x00537d93..0x00537dc5). */
typedef struct { unsigned char *ptr; int stride; } VtxIter;
// MATCH: 0x00537d70 FUN_00537d70
void FUN_00537d70(VtxIter *param_1, int param_2, int *param_3)
{
  int *v;
  int stride;
  int *src;
  int sstride;
  int *a;
  int *b;

  v = (int *)param_1->ptr;
  stride = param_1->stride;
  src = (int *)param_3[0xf];
  sstride = param_3[0x10];
  for (; param_2 > 0; param_2--) {
    a = src;
    b = src + 2;
    src = (int *)((char *)src + sstride);
    v[0] = a[0]; v[1] = b[1]; v = (int *)((char *)v + stride);
    v[0] = b[0]; v[1] = b[1]; v = (int *)((char *)v + stride);
    v[0] = b[0]; v[1] = a[1]; v = (int *)((char *)v + stride);
    v[0] = a[0]; v[1] = a[1]; v = (int *)((char *)v + stride);
  }
}
