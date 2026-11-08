// FLAGS: /O2 /Ob1 /G5
/* h2 note: 19.0% raw form. cxx FAIL: undefined1[256] passed as float* to FUN_0054f410/FUN_0054d4d0. class (b). */
/* MATCH-RWL04 0x0054ba20: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
void FUN_0054d4d0(float * p0, float * p1);
void FUN_0054e520(float * p0, float * p1, float p2, float p3);
void FUN_0054f410(float * p0, float * p1);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
// MATCH: 0x0054ba20 FUN_0054ba20
/* [C1 2026-05-19] 4-param `(out_buf, stride, control_mat, levels)`. */

void FUN_0054ba20(float *param_1,int param_2,undefined4 param_3,int param_4)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  int iVar8;
  float local_330;
  float local_32c;
  float local_328;
  float local_320;
  float local_31c;
  float local_318;
  float local_310;
  float local_30c;
  float local_308;
  float local_300;
  float local_2fc;
  float local_2f8;
  float local_2f0;
  float local_2ec;
  float local_2e8;
  float local_2e0;
  float local_2dc;
  float local_2d8;
  float local_2d0;
  float local_2cc;
  float local_2c8;
  float local_2c0;
  float local_2bc;
  float local_2b8;
  float local_2b0;
  float local_2ac;
  float local_2a8;
  float local_2a0;
  float local_29c;
  float local_298;
  float local_290;
  float local_28c;
  float local_288;
  float local_280;
  float local_27c;
  float local_278;
  float local_270;
  float local_26c;
  float local_268;
  float local_260;
  float local_25c;
  float local_258;
  float local_250;
  float local_24c;
  float local_248;
  float local_240;
  float local_23c;
  float local_238;
  float local_214;
  float local_210;
  undefined1 local_200 [256];
  undefined1 local_100 [256];
  
  fVar1 = _DAT_005cc320 / (float)param_4;
  FUN_0054f410(local_200,param_3);
  FUN_0054d4d0(local_100,local_200);
  FUN_0054e520(&local_330,local_100,fVar1,fVar1);
  param_4 = param_4 + 1;
  while( true ) {
    local_214 = local_30c;
    local_210 = local_308;
    iVar8 = param_4;
    fVar1 = local_330;
    fVar2 = local_320;
    fVar3 = local_310;
    fVar4 = local_32c;
    fVar5 = local_31c;
    fVar6 = local_328;
    fVar7 = local_318;
    while( true ) {
      *param_1 = fVar1;
      param_1[1] = fVar4;
      param_1[2] = fVar6;
      param_1 = (float *)((int)param_1 + param_2);
      iVar8 = iVar8 + -1;
      if (iVar8 == 0) break;
      fVar1 = fVar2 + fVar1;
      fVar4 = fVar5 + fVar4;
      fVar6 = fVar7 + fVar6;
      fVar2 = fVar3 + fVar2;
      fVar5 = local_214 + fVar5;
      fVar7 = local_210 + fVar7;
      fVar3 = fVar3 + local_300;
      local_214 = local_2fc + local_214;
      local_210 = local_2f8 + local_210;
    }
    param_4 = param_4 + -1;
    if (param_4 == 0) break;
    local_330 = local_2f0 + local_330;
    local_32c = local_2ec + local_32c;
    local_328 = local_2e8 + local_328;
    local_320 = local_2e0 + local_320;
    local_31c = local_2dc + local_31c;
    local_318 = local_2d8 + local_318;
    local_310 = local_2d0 + local_310;
    local_30c = local_2cc + local_30c;
    local_308 = local_2c8 + local_308;
    local_300 = local_2c0 + local_300;
    local_2fc = local_2bc + local_2fc;
    local_2f8 = local_2b8 + local_2f8;
    local_2f0 = local_2b0 + local_2f0;
    local_2ec = local_2ac + local_2ec;
    local_2e8 = local_2a8 + local_2e8;
    local_2e0 = local_2a0 + local_2e0;
    local_2dc = local_29c + local_2dc;
    local_2d8 = local_298 + local_2d8;
    local_2d0 = local_290 + local_2d0;
    local_2cc = local_28c + local_2cc;
    local_2c8 = local_288 + local_2c8;
    local_2c0 = local_280 + local_2c0;
    local_2bc = local_27c + local_2bc;
    local_2b8 = local_278 + local_2b8;
    local_2b0 = local_270 + local_2b0;
    local_2ac = local_26c + local_2ac;
    local_2a8 = local_268 + local_2a8;
    local_2a0 = local_260 + local_2a0;
    local_29c = local_25c + local_29c;
    local_298 = local_258 + local_298;
    local_290 = local_250 + local_290;
    local_28c = local_24c + local_28c;
    local_288 = local_248 + local_288;
    local_280 = local_240 + local_280;
    local_27c = local_23c + local_27c;
    local_278 = local_238 + local_278;
  }
  return;
}
