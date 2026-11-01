// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00448880: 100% (regs 100%) by hand (h9). Raw 79.2%. Same shape as 0x00448770 with 4 entries:
   int-index loops (jl), local vector initialised before the global store. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct { float x, y, z; } MatchV3_00448880;
MGLOBALA(int, DAT_00896480, 4, 0x00896480);
MGLOBALA(MatchV3_00448880, DAT_005f95f0, 4, 0x005f95f0);
MGLOBALA(float, DAT_005f9620, 4, 0x005f9620);
int FUN_004c1340(int p0, MatchV3_00448880 *p1, int p2);
int FUN_004c1520(int p0, MatchV3_00448880 *p1, float p2, int p3);
void FUN_004c15c0(int p0);
int FUN_004e6ab0(int p0);
// MATCH: 0x00448880 FUN_00448880
void FUN_00448880(int param_1)
{
  MatchV3_00448880 v;
  int i;

  v.x = 0.0f;
  v.y = 1.0f;
  v.z = 0.0f;
  DAT_00896480[0] = *(int *)(param_1 + 0x10128);
  FUN_004c15c0(*(int *)(DAT_00896480[0] + 4));
  for (i = 1; i < 4; i++) {
    DAT_00896480[i] = FUN_004e6ab0(DAT_00896480[0]);
  }
  for (i = 0; i < 4; i++) {
    FUN_004c15c0(*(int *)(DAT_00896480[i] + 4));
    FUN_004c1520(*(int *)(DAT_00896480[i] + 4), &v, DAT_005f9620[i], 0);
    FUN_004c1340(*(int *)(DAT_00896480[i] + 4), &DAT_005f95f0[i], 2);
  }
}
