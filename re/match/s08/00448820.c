// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00448820: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs08_20261101/results.tsv. */
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
typedef unsigned int undefined4;
MGLOBAL(undefined4, DAT_00896460, 0x00896460);
MGLOBAL(undefined4, DAT_00896464, 0x00896464);
void FUN_004c15c0(int p0);
int FUN_004e6ab0(int p0);
// MATCH: 0x00448820 FUN_00448820
void FUN_00448820(int param_1)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  int *piVar3;
  
  DAT_00896460 = *(int *)(param_1 + 0x1012c);
  FUN_004c15c0(*(undefined4 *)(DAT_00896460 + 4));
  puVar2 = &DAT_00896464;
  do {
    uVar1 = FUN_004e6ab0(DAT_00896460);
    *puVar2 = uVar1;
    puVar2 = puVar2 + 1;
  } while ((int)puVar2 < 0x896474);
  piVar3 = &DAT_00896460;
  do {
    FUN_004c15c0(*(undefined4 *)(*piVar3 + 4));
    piVar3 = piVar3 + 1;
  } while ((int)piVar3 < 0x896474);
  return;
}
