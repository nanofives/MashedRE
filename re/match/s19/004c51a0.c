// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c51a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs19_20261102/results.tsv. */
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
typedef unsigned int uint;
extern char s_Invalid_combination_type_0061811c[];
MGLOBAL(uint, UNK_0000000c, 0x0000000c);
undefined4 FUN_004d7ff0(undefined4 p0, ...);
undefined4 * FUN_004d8480(undefined4 * p0);
MGLOBAL(int, uRam0000000c, 0x0000000c);
// MATCH: 0x004c51a0 FUN_004c51a0
void FUN_004c51a0(float *param_1,float *param_2,int param_3)

{
  undefined4 local_8;
  undefined4 local_4;
  
  if (param_3 == 0) {
    param_1[10] = 1.0;
    param_1[5] = 1.0;
    *param_1 = 1.0;
    param_1[4] = 0.0;
    param_1[3] = (float)((uint)param_1[3] | 0x20003);
    param_1[2] = 0.0;
    param_1[1] = 0.0;
    param_1[9] = 0.0;
    param_1[8] = 0.0;
    param_1[6] = 0.0;
    param_1[0xe] = 0.0;
    param_1[0xd] = 0.0;
    param_1[0xc] = 0.0;
    param_1[0xc] = *param_2;
    param_1[0xd] = param_2[1];
    param_1[0xe] = param_2[2];
    param_1[3] = (float)((uint)param_1[3] & 0xfffdffff);
    return;
  }
  if (param_3 != 1) {
    if (param_3 != 2) {
      local_8 = 1;
      local_4 = FUN_004d7ff0(0x80000003,s_Invalid_combination_type_0061811c);
      FUN_004d8480(&local_8);
      uRam0000000c = uRam0000000c & 0xfffdffff;
      return;
    }
    param_1[0xc] = param_1[0xc] + *param_2;
    param_1[0xd] = param_2[1] + param_1[0xd];
    param_1[0xe] = param_2[2] + param_1[0xe];
    param_1[3] = (float)((uint)param_1[3] & 0xfffdffff);
    return;
  }
  param_1[0xc] = *param_1 * *param_2 + param_1[4] * param_2[1] + param_1[8] * param_2[2] +
                 param_1[0xc];
  param_1[0xd] = param_1[1] * *param_2 + param_1[5] * param_2[1] + param_1[9] * param_2[2] +
                 param_1[0xd];
  param_1[0xe] = param_1[2] * *param_2 + param_1[6] * param_2[1] + param_1[10] * param_2[2] +
                 param_1[0xe];
  param_1[3] = (float)((uint)param_1[3] & 0xfffdffff);
  return;
}
