// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x00483ca0 (h12): 100% (regs 100%). Hand-written: finds the chunk (type 0x10b, id = rec+8, or the 0/0
   terminator), struct-copies 0x67 dwords of the record, then memcpy(count +0x16c * 0x24) from rec+0x14 (intrinsic). */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
#ifdef __cplusplus
extern "C"
#endif
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
typedef struct Chunk483ca0 { int type; int size; int id; } Chunk483ca0;
typedef struct Rec483ca0 { int f[0x67]; } Rec483ca0;
// MATCH: 0x00483ca0 FUN_00483ca0
int FUN_00483ca0(int *param_1, Chunk483ca0 **param_2)
{
  Chunk483ca0 *c;
  int *src;
  c = *param_2;
  src = (int *)param_1[5];
  while ((c->id != param_1[2] || c->type != 0x10b) && (c->id != 0 || c->type != 0)) {
    c = (Chunk483ca0 *)((char *)c + c->size);
  }
  *(Rec483ca0 *)c = *(Rec483ca0 *)param_1;
  memcpy((char *)c + 0x19c, src, param_1[0x5b] * 0x24);
  return 1;
}
