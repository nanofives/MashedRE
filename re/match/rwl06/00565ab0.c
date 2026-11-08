// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00565ab0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl06_20261108/results.tsv. */
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
undefined4 FUN_00546b10(float * p0, float * p1);
int FUN_00565780(undefined4 * p0, undefined4 * p1, int p2, int p3);
// MATCH: 0x00565ab0 FUN_00565ab0
undefined4 FUN_00565ab0(undefined4 *param_1,undefined4 *param_2)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 local_88[9];
  undefined4 local_64[16];
  undefined4 local_24[9];

  puVar2 = local_24;
  for (iVar1 = 9; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = *param_2;
    param_2 = param_2 + 1;
    puVar2 = puVar2 + 1;
  }
  local_88[0] = 0x3f800000;
  local_88[1] = 0;
  local_88[2] = 0;
  local_88[3] = 0;
  local_88[4] = 0x3f800000;
  local_88[5] = 0;
  local_88[6] = 0;
  local_88[7] = 0;
  local_88[8] = 0x3f800000;
  iVar1 = FUN_00565780(local_24,local_88,0x19,0x358637bd);
  if (iVar1 == 0) {
    return 0;
  }
  local_64[0] = local_88[0];
  local_64[1] = local_88[3];
  local_64[4] = local_88[1];
  local_64[5] = local_88[4];
  local_64[2] = local_88[6];
  local_64[8] = local_88[2];
  local_64[9] = local_88[5];
  local_64[6] = local_88[7];
  local_64[10] = local_88[8];
  FUN_00546b10((float *)(param_1 + 3),(float *)local_64);
  *param_1 = local_24[0];
  param_1[1] = local_24[4];
  param_1[2] = local_24[8];
  return 1;
}
