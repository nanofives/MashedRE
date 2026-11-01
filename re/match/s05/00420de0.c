// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00420de0 (31 B): 100.0% (regs 100, ops 100), by hand. Same file text as the files of 0x004215c0 (30 B) and 0x004215e0 (65 B):
   all three 100.0%. This file holds 0x00420de0.
   DAT_0063e4b8 = 4 records of 0x24 bytes (float[2] at +0x00, cleared by 0x00420d00). 0x00420de0 takes the record
   pointer in EDX, the index in ECX and the float on the stack: pilot finding 1, it is `static`, defined before its
   two callers 0x004215c0 and 0x004215e0, parameter order (r, j, d) (the order (j, r, d) swaps ECX/EDX).
   0x004215e0 needs the index as one ternary argument: an if/else with two calls gives 61.9% (the compiler then
   keeps the float in ECX in the first arm); the ternary is split back into two call sites by the compiler.
   0x5cd120 = 50.0f (literal), 0x005d757c = 0.0f read through MGLOBAL (a 0.0f literal would become fldz).
   Residual: none. Relocheck OK. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct Rec0063e4b8 {
  float v[2];    /* +0x00 */
  int a[2];      /* +0x08 */
  int b[2];      /* +0x10 */
  int id;        /* +0x18 */
  int f7;        /* +0x1c */
  int f8;        /* +0x20 */
} Rec0063e4b8;
MGLOBALA(Rec0063e4b8, DAT_0063e4b8, 4, 0x0063e4b8);
MGLOBAL(const float, DAT_005d757c, 0x005d757c);
// MATCH: 0x00420de0 FUN_00420de0
static void FUN_00420de0(Rec0063e4b8 *r, int j, float d)
{
  r->v[j] += d;
  if (r->v[j] > 50.0f)
    r->v[j] = 50.0f;
}
// MATCH: 0x004215c0 FUN_004215c0
void FUN_004215c0(int i, float d, int j)
{
  FUN_00420de0(&DAT_0063e4b8[i], j, d);
}
// MATCH: 0x004215e0 FUN_004215e0
void FUN_004215e0(int i, float d, float *pos)
{
  Rec0063e4b8 *r = &DAT_0063e4b8[i];
  FUN_00420de0(r, pos[2] > DAT_005d757c ? 0 : 1, d);
}
