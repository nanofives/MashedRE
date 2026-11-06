// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00489940: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
undefined4 FUN_00474db0(int p0, int p1);
int * FUN_004892c0(undefined4 p0, int p1, int p2, int p3);
void FUN_004893a0(void);
// MATCH: 0x00489940 FUN_00489940
undefined4 *
FUN_00489940(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
            undefined4 *param_5,undefined4 param_6,undefined4 param_7)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  
  puVar1 = (undefined4 *)FUN_00474db0(1,0x2c);
  uVar2 = FUN_00474db0(param_1,0xc);
  *puVar1 = uVar2;
  uVar2 = FUN_00474db0(param_1,0xc);
  puVar1[1] = uVar2;
  uVar2 = FUN_00474db0(param_1,4);
  puVar1[2] = uVar2;
  puVar1[3] = param_1;
  puVar1[4] = param_1;
  uVar2 = FUN_004892c0(param_2,param_1,param_4,param_3);
  puVar1[10] = uVar2;
  puVar1[5] = *param_5;
  puVar1[6] = param_6;
  puVar1[7] = param_7;
  puVar1[9] = 0;
  FUN_004893a0(uVar2,5,6);
  return puVar1;
}
