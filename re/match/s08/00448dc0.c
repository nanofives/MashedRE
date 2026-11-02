// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00448dc0: 100% (regs 100%) by hand (h9). Raw 77.5%. Both callee results go through locals:
   the float from FUN_00442420 (gives the fstp/mov ecx spill through the prologue's push ecx slot) and the
   pointer from FUN_00442410 (called before the args of FUN_004c1340 are pushed; one deferred add esp, 0x28).
   The 95 B twin 0x0044cb00 is the same source with DAT_00896230[4] and i < 4 (100% + relocheck OK in a
   scratch copy, D:/MashedWT/MATCH-S08_data/scratch/h9/t_0044cb00.c).
   param_1 is unused: the caller 0x00448ef0 pushes one argument, so it is declared here too. */
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
typedef struct { float x, y, z; } MatchV3_00448dc0;
MGLOBALA(int, DAT_00896460, 5, 0x00896460);
MGLOBAL(MatchV3_00448dc0, DAT_006146fc, 0x006146fc);
MatchV3_00448dc0 *FUN_00442410(int p0);
float FUN_00442420(int p0);
int FUN_004c1340(int p0, MatchV3_00448dc0 *p1, int p2);
int FUN_004c1520(int p0, MatchV3_00448dc0 *p1, float p2, int p3);
void FUN_004c15c0(int p0);
// MATCH: 0x00448dc0 FUN_00448dc0
void FUN_00448dc0(int param_1)
{
  int i;
  int m;
  float a;
  MatchV3_00448dc0 *pos;

  for (i = 0; i < 5; i++) {
    m = *(int *)(DAT_00896460[i] + 4);
    FUN_004c15c0(m);
    a = FUN_00442420(i + 1);
    FUN_004c1520(m, &DAT_006146fc, a, 0);
    pos = FUN_00442410(i + 1);
    FUN_004c1340(*(int *)(DAT_00896460[i] + 4), pos, 2);
  }
}
