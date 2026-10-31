// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 h4 0x0041cbc0 (133 B): 100.0% exact (regs 100, ops 100), BYTE-IDENTICAL, by hand. Raw converter: 61.5%.
   The 48-byte table (4 float triples: 0.48,{0.25,0.15,0.05,-0.05},1.0) is a struct local filled field by field and
   copied by assignment (rep movsd) to 0x005f337c; then DAT_0063d270 = 0. Residual: none. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3_0041cbc0;
typedef struct { V3_0041cbc0 v[4]; } V3Quad_0041cbc0;
MGLOBAL(V3Quad_0041cbc0, DAT_005f337c, 0x005f337c);
MGLOBAL(int, DAT_0063d270, 0x0063d270);

// MATCH: 0x0041cbc0 FUN_0041cbc0
void FUN_0041cbc0(void)
{
  V3Quad_0041cbc0 t;
  t.v[0].x = 0.48f;
  t.v[0].y = 0.25f;
  t.v[0].z = 1.0f;
  t.v[1].x = 0.48f;
  t.v[1].y = 0.15f;
  t.v[1].z = 1.0f;
  t.v[2].x = 0.48f;
  t.v[2].y = 0.05f;
  t.v[2].z = 1.0f;
  t.v[3].x = 0.48f;
  t.v[3].y = -0.05f;
  t.v[3].z = 1.0f;
  DAT_005f337c = t;
  DAT_0063d270 = 0;
}
