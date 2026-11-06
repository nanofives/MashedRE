// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00453eb0: 100% by hand. Ring of 100 records of 36 bytes at 0x006870a8 (index 0x00687eb8). */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct { float x, y, z, w; } V4_453eb0;
typedef struct {
  float a;          /* 0x00 */
  float b;          /* 0x04 */
  float c;          /* 0x08 */
  int n;            /* 0x0c */
  int n2;           /* 0x10 */
  V4_453eb0 p;      /* 0x14 */
} Ent_453eb0;
MGLOBAL(int, DAT_00687eb8, 0x00687eb8);
MGLOBALA(Ent_453eb0, DAT_006870a8, 100, 0x006870a8);
void FUN_00465940(V4_453eb0 *p);
void FUN_00487020(V4_453eb0 *p, float f);
// MATCH: 0x00453eb0 FUN_00453eb0
void FUN_00453eb0(V4_453eb0 *p, float a, float b, float c, int n)
{
  int i;
  Ent_453eb0 *e;
  i = DAT_00687eb8;
  if (i >= 100) i = 0;
  e = &DAT_006870a8[i];
  DAT_00687eb8 = i + 1;
  if (e != 0) {
    e->a = a;
    e->b = b;
    e->c = c;
    e->n = n;
    e->n2 = n;
    e->p = *p;
    FUN_00465940(p);
    FUN_00487020(p, 1.2f);
  }
}
