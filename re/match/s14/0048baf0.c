// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x0048baf0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs14_20261102/results.tsv. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef int code();
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined1, DAT_00720040, 0x00720040);
MGLOBAL(undefined1, DAT_00720080, 0x00720080);
MGLOBAL(undefined4, DAT_007201fc, 0x007201fc);
MGLOBAL(undefined4, DAT_007220f0, 0x007220f0);
void FUN_004770a0(int p0);
// MATCH: 0x0048baf0 FUN_0048baf0
void FUN_0048baf0(void)

{
  int *piVar1;
  int *piVar2;
  int *piVar3;
  int iVar4;
  
  (**(code **)(DAT_007d3ff8 + 0x20))(8,0);
  FUN_004770a0(&DAT_00720040);
  FUN_004770a0(&DAT_00720080);
  piVar2 = &DAT_007220f0;
  piVar1 = &DAT_007201fc;
  do {
    if (*piVar2 != 0) {
      iVar4 = 10;
      piVar3 = piVar1;
      do {
        (**(code **)(*piVar3 + 0x48))(*piVar3);
        piVar3 = piVar3 + 1;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
    }
    piVar1 = piVar1 + 0x29;
    piVar2 = piVar2 + 1;
  } while ((int)piVar1 < 0x72071c);
  (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
  return;
}
