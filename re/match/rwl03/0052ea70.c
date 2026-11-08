// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0052ea70: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef unsigned char byte;
// MATCH: 0x0052ea70 FUN_0052ea70
void FUN_0052ea70(int param_1,int param_2,int *param_3,uint param_4,uint *param_5,uint param_6)
{
  uint uVar1;
  int iVar2;
  uint *puVar3;
  uint uVar4;
  uint uVar5;
  uint sh;
  uint local_4;

  puVar3 = param_5;
  local_4 = 0;
  *param_5 = 0;
  if (0 < param_6) {
    iVar2 = param_4;
    do {
      sh = 0;
      param_4 = 0;
      if (param_4 < *(uint *)(param_1 + 0x10)) {
        param_5 = (uint *)param_3;
        do {
          if (*param_5 != 0) {
            uVar1 = *puVar3;
            uVar4 = *(uint *)(param_2 + local_4 * 4) >> sh;
            uVar4 &= 0xff;
            uVar5 = 0;
            if (uVar1 != 0) {
              do {
                if (uVar4 == (uint)*(byte *)(uVar5 + iVar2)) goto LAB_0052eaf5;
                uVar5 = uVar5 + 1;
              } while (uVar5 < uVar1);
            }
            *(char *)(uVar1 + iVar2) = (char)uVar4;
            *puVar3 = *puVar3 + 1;
          }
LAB_0052eaf5:
          param_4 = param_4 + 1;
          param_5 = param_5 + 1;
          sh = sh + 8;
        } while (param_4 < *(uint *)(param_1 + 0x10));
      }
      local_4 = local_4 + 1;
      param_3 = param_3 + 4;
    } while (local_4 < param_6);
  }
  return;
}
