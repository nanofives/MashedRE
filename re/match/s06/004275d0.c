// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x004275d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs06_20261101/results.tsv.
   C++ check fixed (h12, 2026-11-01): FUN_00553ef0 returns int*, uVar1 int*, FUN_00552920 int*, FUN_00553cf0 p1 void*. Prototypes only, codegen unchanged (100%, relocheck OK). */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
MGLOBAL(undefined1, DAT_0067d828, 0x0067d828);
void FUN_00427580(void);
undefined4 FUN_00552750(void);
undefined4 FUN_00552920(int *p0);
undefined4 FUN_00552a60(int p0);
bool FUN_00552d10(void);
void FUN_00552d70(void);
int *FUN_00553cf0(int *p0, void *p1);
bool FUN_00553e80(int * p0);
int *FUN_00553ef0(void);
// MATCH: 0x004275d0 FUN_004275d0
void FUN_004275d0(undefined4 param_1)

{
  int *uVar1;
  
  FUN_00552a60(param_1);
  FUN_00552d10();
  FUN_00552750();
  uVar1 = FUN_00553ef0();
  FUN_00552920(uVar1);
  FUN_00553cf0(uVar1,&DAT_0067d828);
  FUN_00553e80(uVar1);
  FUN_00552d70();
  FUN_00427580();
  return;
}
