// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0053cd80: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
#ifdef __cplusplus
typedef int (__cdecl code)(...);
#else
typedef int code();
#endif
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
undefined4 FUN_00547450(float * p0, float * p1, float * p2, float * p3, float * p4, float * p5);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
// MATCH: 0x0053cd80 FUN_0053cd80
/* [C1 2026-05-20] Sphere-vs-morphed-atomic. param_3 = sphere, params 4/5/6 = callback context. */

void FUN_0053cd80(int param_1,float param_2,int param_3,undefined4 param_4,code *param_5,
                 undefined4 param_6)

{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  int iVar4;
  float *pfVar5;
  ushort *puVar6;
  int local_68;
  float *local_64;
  float *local_60;
  undefined1 local_58 [12];
  float local_4c;
  float local_48;
  float local_44;
  int local_40;
  float *local_3c;
  float *local_38;
  float *local_34;
  float local_30;
  float fStack_2c;
  float fStack_28;
  float local_24;
  float fStack_20;
  float fStack_1c;
  float local_18;
  float fStack_14;
  float fStack_10;
  int local_c;
  int local_8;
  float local_4;
  
  fVar3 = _DAT_005cc320 / *(float *)(param_3 + 0xc);
  pfVar5 = &local_30;
  local_68 = 0;
  local_c = *(int *)(*(int *)(param_1 + 0x5c) + 0x14 + *(short *)((int)param_2 + 4) * 0x1c);
  puVar6 = *(ushort **)(param_1 + 0x2c);
  local_8 = *(int *)(*(int *)(param_1 + 0x5c) + 0x14 + *(short *)((int)param_2 + 6) * 0x1c);
  local_4 = *(float *)((int)param_2 + 0x10) * *(float *)((int)param_2 + 0xc);
  local_60 = &local_24;
  local_64 = &local_18;
  if (0 < *(int *)(param_1 + 0x10)) {
    do {
      if (local_c == local_8) {
        pfVar5 = (float *)(local_c + (uint)*puVar6 * 0xc);
        local_60 = (float *)(local_c + (uint)puVar6[1] * 0xc);
        local_64 = (float *)(local_c + (uint)puVar6[2] * 0xc);
      }
      else {
        iVar4 = (uint)*puVar6 * 0xc;
        pfVar1 = (float *)(iVar4 + 4 + local_c);
        pfVar2 = (float *)(iVar4 + 8 + local_c);
        local_30 = (*(float *)(iVar4 + local_8) - *(float *)(iVar4 + local_c)) * local_4 +
                   *(float *)(iVar4 + local_c);
        fStack_2c = *pfVar1 + (*(float *)(iVar4 + 4 + local_8) - *pfVar1) * local_4;
        fStack_28 = *pfVar2 + (*(float *)(iVar4 + 8 + local_8) - *pfVar2) * local_4;
        iVar4 = (uint)puVar6[1] * 0xc;
        pfVar1 = (float *)(iVar4 + 4 + local_c);
        pfVar2 = (float *)(iVar4 + 8 + local_c);
        local_24 = (*(float *)(iVar4 + local_8) - *(float *)(iVar4 + local_c)) * local_4 +
                   *(float *)(iVar4 + local_c);
        fStack_20 = *pfVar1 + (*(float *)(iVar4 + 4 + local_8) - *pfVar1) * local_4;
        fStack_1c = *pfVar2 + (*(float *)(iVar4 + 8 + local_8) - *pfVar2) * local_4;
        iVar4 = (uint)puVar6[2] * 0xc;
        pfVar1 = (float *)(iVar4 + 4 + local_c);
        pfVar2 = (float *)(iVar4 + 8 + local_c);
        local_18 = (*(float *)(iVar4 + local_8) - *(float *)(iVar4 + local_c)) * local_4 +
                   *(float *)(iVar4 + local_c);
        fStack_14 = *pfVar1 + (*(float *)(iVar4 + 4 + local_8) - *pfVar1) * local_4;
        fStack_10 = *pfVar2 + (*(float *)(iVar4 + 8 + local_8) - *pfVar2) * local_4;
      }
      iVar4 = FUN_00547450((float *)param_3,pfVar5,local_60,local_64,(float *)local_58,&param_2);
      if (iVar4 != 0) {
        param_2 = param_2 * fVar3;
        local_4c = *pfVar5;
        local_48 = pfVar5[1];
        local_44 = pfVar5[2];
        local_40 = local_68;
        local_3c = pfVar5;
        local_38 = local_60;
        local_34 = local_64;
        iVar4 = (*param_5)(param_4,local_58,param_2,param_6);
        if (iVar4 == 0) {
          return;
        }
      }
      local_68 = local_68 + 1;
      puVar6 = puVar6 + 4;
    } while (local_68 < *(int *)(param_1 + 0x10));
  }
  return;
}
