// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR05 2026-11-08: float DAT_ constants written as float literals (values read from the unpatched exe; relocheck compares __real@ constants by value). The compiler orders x87 operands differently for a literal than for a global. Score in results.tsv. */
/* MATCH-RWL06 0x0057a9a0: hand pass (h2), body kept in raw form. 34.1% regs 39.8% ops 67.8% under /O2 /Ob1 /G5. Compiles after: prototypes of 0x0055c000 (int,int,float*,float*) and 0x0057a250 (int*,float*,float,float*,float*,float*,float*,float), byte[432] scratch as float[108]. Control flow (do/while of 4, goto labels) kept from the decomp; not iterated further (budget). Class b. */
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
typedef unsigned int uint;
typedef long double float10;
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, DAT_005cc328, 0x005cc328);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc9dc, 0x005cc9dc);
MGLOBAL(float, DAT_005cc9a0, 0x005cc9a0);
void FUN_0055c000(int p0, int p1, float * p2, float * p3);
undefined4 FUN_0057a250(int * p0, float * p1, float p2, float * p3, float * p4, float * p5, float * p6, float p7);
float FUN_0057a660(int * p0, float * p1);
// MATCH: 0x0057a9a0 FUN_0057a9a0
/* [C1 2026-05-20] Signature recovered: `int FUN_0057a9a0(undefined4 unused, int *body_pair, float
   merge_depth, float
 */

undefined4
FUN_0057a9a0(undefined4 param_1,int *param_2,float param_3,float *param_4,float *param_5,
            float *param_6,float param_7)

{
  int iVar1;
  bool bVar2;
  float fVar3;
  int iVar4;
  float fVar5;
  float local_200;
  float local_1fc;
  float local_1f8;
  float local_1f4;
  float local_1f0;
  float local_1ec;
  float local_1e8;
  int local_1e4;
  float local_1e0;
  float local_1dc;
  float local_1d8;
  float local_1d4;
  float local_1d0;
  float local_1cc;
  uint local_1c8;
  float local_1c4;
  float local_1c0;
  float local_1bc;
  float local_1b8;
  float local_1b4;
  float local_1b0[108];
  
  iVar4 = *(int *)(*param_2 + 8);
  iVar1 = *(int *)(param_2[1] + 8);
  local_1f0 = *(float *)(iVar4 + 0x4c);
  local_1e8 = *(float *)(iVar1 + 0x4c);
  if (**(short **)(iVar4 + 0x5c) == 1) {
    *param_4 = -*(float *)(*(int *)(*param_2 + 0x10) + 0x20);
    param_4[1] = -*(float *)(*(int *)(*param_2 + 0x10) + 0x24);
    param_4[2] = -*(float *)(*(int *)(*param_2 + 0x10) + 0x28);
    FUN_0055c000(*(int *)(param_2[1] + 8),*(int *)(param_2[1] + 0x10),param_4,
                 &local_1e0);
    iVar4 = *(int *)(*param_2 + 0x10);
    fVar3 = param_4[2] * local_1d8 + local_1e0 * *param_4 + param_4[1] * local_1dc + local_1e8;
    local_1e8 = (*(float *)(iVar4 + 0x38) * param_4[2] +
                *(float *)(iVar4 + 0x30) * *param_4 + *(float *)(iVar4 + 0x34) * param_4[1]) -
                local_1f0;
  }
  else {
    if (**(short **)(iVar1 + 0x5c) != 1) {
      fVar5 = FUN_0057a660(param_2,&local_1d4);
      local_1f8 = (float)fVar5;
      local_1fc = param_7 * 0.01f;
      if (local_1fc < local_1f8) {
        local_1fc = local_1fc + local_1f8;
      }
      local_1c4 = local_1e8 + local_1f0;
      local_1ec = 0.0;
      local_200 = 0.0;
      local_1c0 = local_1c4 + param_3;
      local_1e4 = 0;
      local_1c8 = 0;
      do {
        local_1e0 = local_1d4 * local_200;
        local_1dc = local_1d0 * local_200;
        local_1d8 = local_1cc * local_200;
        iVar4 = FUN_0057a250(param_2,&local_1e0,local_1c0 + local_200,local_1b0,&local_1bc,
                             &local_1f8,&local_1f4,param_7);
        if ((iVar4 == 0) || (local_1f4 <= 0.0f)) {
          local_1ec = local_200;
          if (local_1e4 == 0) {
            if (local_200 == local_1fc) {
              local_1fc = local_1fc + param_7;
            }
            local_200 = local_1fc;
          }
          else {
            local_200 = (local_200 + local_1fc) * 0.5f;
          }
        }
        else {
          local_1fc = local_200;
          local_1e4 = 1;
          bVar2 = local_200 == 0.0f;
          *param_4 = local_1bc;
          param_4[1] = local_1b8;
          param_4[2] = local_1b4;
          *param_5 = local_1f8;
          *param_6 = local_1f4;
          if (bVar2) goto LAB_0057ad7f;
          if (local_1f4 < param_3) goto LAB_0057ad3c;
          local_1f8 = param_4[2] * local_1cc + local_1d4 * *param_4 + param_4[1] * local_1d0;
          if (local_200 <= local_1f4 / local_1f8) {
            local_200 = local_1ec * 0.95f + local_200 * 0.05f;
          }
          else {
            local_200 = local_200 - (local_1f4 / local_1f8) * 0.95f;
          }
          if (local_200 < local_1ec) {
            local_1ec = local_200;
          }
        }
        local_1c8 = local_1c8 + 1;
      } while (local_1c8 < 4);
      if (local_1e4 == 0) {
        return 0;
      }
LAB_0057ad3c:
      if (local_1fc != 0.0f) {
        local_1fc = (param_4[2] * local_1cc + local_1d4 * *param_4 + param_4[1] * local_1d0) *
                    local_1fc;
        *param_5 = *param_5 - 0.5f * local_1fc;
        *param_6 = *param_6 - local_1fc;
      }
LAB_0057ad7f:
      *param_5 = *param_5 - (local_1f0 - local_1e8) * 0.5f;
      *param_6 = *param_6 - local_1c4;
      return 1;
    }
    iVar4 = *(int *)(param_2[1] + 0x10);
    *param_4 = *(float *)(iVar4 + 0x20);
    param_4[1] = *(float *)(iVar4 + 0x24);
    param_4[2] = *(float *)(iVar4 + 0x28);
    FUN_0055c000(*(undefined4 *)(*param_2 + 8),*(undefined4 *)(*param_2 + 0x10),param_4,&local_1e0);
    iVar4 = *(int *)(param_2[1] + 0x10);
    fVar3 = param_4[2] * local_1d8 + local_1e0 * *param_4 + param_4[1] * local_1dc + local_1f0;
    local_1e8 = (*(float *)(iVar4 + 0x38) * param_4[2] +
                *(float *)(iVar4 + 0x30) * *param_4 + *(float *)(iVar4 + 0x34) * param_4[1]) -
                local_1e8;
  }
  *param_5 = (local_1e8 + fVar3) * 0.5f;
  *param_6 = local_1e8 - fVar3;
  return 1;
}
