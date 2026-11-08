// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00566aa0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int uint;
typedef long double float10;
typedef int code();
MGLOBAL(float, DAT_005d757c, 0x005d757c);
// MATCH: 0x00566aa0 FUN_00566aa0
float FUN_00566aa0(int param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  float fVar7;
  float fVar8;
  float local_4;

  local_4 = 1e+10;
  iVar3 = *(int *)(param_1 + 0x40);
  if (*(float *)(iVar3 + 0x7c) > DAT_005d757c) {
    return *(float *)(iVar3 + 0x7c);
  }
  uVar5 = 0;
  if ((uint)*(int *)(iVar3 + 0x10) > 0) {
    iVar4 = *(int *)(iVar3 + 0xc);
    iVar6 = 0;
    do {
      piVar2 = (int *)(iVar6 + 0x5c + iVar4);
      iVar1 = iVar6 + iVar4;
      iVar4 = *(int *)(iVar3 + 0xc);
      fVar7 = (**(float (**)(int))(*piVar2 + 0x24))(iVar1);
      fVar8 = *(float *)(iVar6 + 0x4c + iVar4);
      fVar7 = fVar8 + fVar8 + fVar7;
      if ((fVar7 > DAT_005d757c) && (fVar7 < local_4)) {
        local_4 = fVar7;
      }
      uVar5 = uVar5 + 1;
      iVar6 = iVar6 + 0x60;
    } while (uVar5 < *(uint *)(iVar3 + 0x10));
  }
  *(float *)(iVar3 + 0x7c) = local_4;
  return local_4;
}
