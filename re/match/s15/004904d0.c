// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h4 0x004904d0: 100% byte-identical whole obj symbol (33 B, relocations masked). Ghidra's size (24 B)
   stops inside the loop, so the per-instruction score reads 55.6%; match.py --bytes prints BYTE-IDENTICAL.
   An index loop over the 30-record array at 0x0086a4a0 (jl); a pointer loop gives jb. Residual: none. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef struct { float x, y, z; } RwV3d;
typedef struct {
  int active;      /* +0x00 */
  int pad04;       /* +0x04 */
  int pad08;       /* +0x08 */
  int obj;         /* +0x0c */
  RwV3d pos;       /* +0x10 */
  RwV3d vel;       /* +0x1c */
  RwV3d scale;     /* +0x28 */
  int f34;         /* +0x34 */
  int f38;         /* +0x38 */
  int f3c;         /* +0x3c */
  float f40[3];    /* +0x40 */
  int f4c;         /* +0x4c */
} Spark0086a4a0;
MGLOBALA(Spark0086a4a0, DAT_0086a4a0, 30, 0x0086a4a0);
// MATCH: 0x004904d0 FUN_004904d0
void FUN_004904d0(void)
{
  int i;
  for (i = 0; i < 30; i++)
    DAT_0086a4a0[i].active = 0;
}
