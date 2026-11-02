// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00450300: 100% by hand (raw 72.6%). Static function taking its pointer in ESI (pilot finding 1):
   its only caller at 0x00450460 (absent from pc_funcs.csv) does mov esi, [esp + 8] before the call;
   caller_00450460 is a stand-in for it (not matched). The y-before-x store order of 0x00684b80/0x00684be0
   comes from chained `v.x = v.y = -1.0f`. The float literals reproduce the immediates exactly. */
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
typedef struct { float x, y, z; } V3_00450300;
MGLOBAL(int, DAT_00684c40, 0x00684c40);
MGLOBAL(V3_00450300, DAT_00684b80, 0x00684b80);
MGLOBAL(V3_00450300, DAT_00684be0, 0x00684be0);
MGLOBALA(V3_00450300, DAT_00684bbc, 3, 0x00684bbc);
MGLOBALA(V3_00450300, DAT_00684c1c, 3, 0x00684c1c);
void FUN_00472520(int p0, int p1);
void FUN_00472560(int p0, int p1);
int FUN_0040e350(void);

// MATCH: 0x00450300 FUN_00450300
static void FUN_00450300(char *param_1)
{
  DAT_00684c40 = 0;
  DAT_00684b80.x = DAT_00684b80.y = -1.0f;
  DAT_00684b80.z = 0.0f;
  DAT_00684be0.x = DAT_00684be0.y = -1.0f;
  DAT_00684be0.z = 0.0f;
  DAT_00684bbc[0].x = -15.037f;
  DAT_00684bbc[0].y = 4.93f;
  DAT_00684bbc[0].z = 58.152f;
  DAT_00684bbc[1].x = -14.978f;
  DAT_00684bbc[1].y = 4.93f;
  DAT_00684bbc[1].z = 57.116f;
  DAT_00684bbc[2].x = -14.922f;
  DAT_00684bbc[2].y = 4.93f;
  DAT_00684bbc[2].z = 56.119f;
  DAT_00684c1c[0].x = -12.503f;
  DAT_00684c1c[0].y = 4.93f;
  DAT_00684c1c[0].z = 56.25f;
  DAT_00684c1c[1].x = -12.559f;
  DAT_00684c1c[1].y = 4.93f;
  DAT_00684c1c[1].z = 57.247f;
  DAT_00684c1c[2].x = -12.618f;
  DAT_00684c1c[2].y = 4.93f;
  DAT_00684c1c[2].z = 57.283f;
  FUN_00472560(0, 1);
  FUN_00472520(0, *(int *)(param_1 + 0x10160));
  FUN_00472560(1, 0);
  FUN_00472520(1, *(int *)(param_1 + 0x10158));
  FUN_00472560(2, 0);
  FUN_00472520(2, *(int *)(param_1 + 0x1015c));
  FUN_00472560(3, 0);
  FUN_00472520(3, *(int *)(param_1 + 0x1012c));
}

/* stand-in for the real caller at 0x00450460 (absent from pc_funcs.csv: ESI = its stack param_1); not matched */
void caller_00450460(char *param_1)
{
  if (FUN_0040e350() == 1) {
    FUN_00450300(param_1);
    return;
  }
  FUN_0040e350();
}
