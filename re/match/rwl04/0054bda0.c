// FLAGS: /O2 /Ob1 /G5
/* h2 note: 10.2% (regs 9.6, ops 82.5): raw form; ops are 82.5% so structure is close and the loss is register allocation/frame (orig add esp 0x364 vs leave). cxx FAIL: undefined1[256] to float*. class (b). */
/* MATCH-RWL04 0x0054bda0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
void FUN_0054dbd0(float * p0, float * p1);
void FUN_0054eb90(float * p0, float * p1, float p2, float p3);
void FUN_0054f410(float * p0, float * p1);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
// MATCH: 0x0054bda0 FUN_0054bda0
/* [C1 2026-05-19] 4-param `(out_buf, stride, control_mat, levels)`. */

undefined1 * FUN_0054bda0(undefined1 *param_1,int param_2,undefined4 param_3,int param_4)

{
  float fVar1;
  undefined1 *puVar2;
  int iVar3;
  int iVar4;
  float local_350;
  float local_34c;
  float local_348;
  float local_344;
  float local_340;
  float local_33c;
  float local_338;
  float local_334;
  float local_330;
  float local_32c;
  float local_328;
  float local_324;
  float local_310;
  float local_30c;
  float local_308;
  float local_304;
  float local_300;
  float local_2fc;
  float local_2f8;
  float local_2f4;
  float local_2f0;
  float local_2ec;
  float local_2e8;
  float local_2e4;
  float local_2e0;
  float local_2dc;
  float local_2d8;
  float local_2d4;
  float local_2d0;
  float local_2cc;
  float local_2c8;
  float local_2c4;
  float local_2c0;
  float local_2bc;
  float local_2b8;
  float local_2b4;
  float local_2b0;
  float local_2ac;
  float local_2a8;
  float local_2a4;
  float local_2a0;
  float local_29c;
  float local_298;
  float local_294;
  float local_290;
  float local_28c;
  float local_288;
  float local_284;
  float local_280;
  float local_27c;
  float local_278;
  float local_274;
  float local_270;
  float local_26c;
  float local_268;
  float local_264;
  float local_260;
  float local_25c;
  float local_258;
  float local_254;
  float local_250;
  float local_24c;
  float local_248;
  float local_244;
  float local_240;
  float local_23c;
  float local_238;
  float local_234;
  float local_230;
  float local_22c;
  float local_228;
  float local_224;
  float local_220;
  float local_21c;
  float local_218;
  float local_214;
  undefined1 local_210;
  undefined1 local_20c;
  undefined1 local_208;
  undefined1 local_204;
  undefined1 local_200 [256];
  undefined1 local_100 [256];
  
  fVar1 = _DAT_005cc320 / (float)param_4;
  FUN_0054f410(local_100,param_3);
  FUN_0054dbd0(local_200,local_100);
  FUN_0054eb90(&local_310,local_200,fVar1,fVar1);
  if (-1 < param_4) {
    iVar4 = param_4 + 1;
    puVar2 = param_1;
    do {
      local_350 = local_310;
      local_34c = local_30c;
      local_348 = local_308;
      local_344 = local_304;
      local_340 = local_300;
      local_33c = local_2fc;
      local_338 = local_2f8;
      local_334 = local_2f4;
      local_330 = local_2f0;
      local_32c = local_2ec;
      local_328 = local_2e8;
      local_324 = local_2e4;
      if (-1 < param_4) {
        iVar3 = param_4 + 1;
        do {
          local_210 = (undefined1)(int)ROUND(local_350);
          *puVar2 = local_210;
          local_204 = (undefined1)(int)ROUND(local_34c);
          puVar2[1] = local_204;
          local_20c = (undefined1)(int)ROUND(local_348);
          puVar2[2] = local_20c;
          local_350 = local_340 + local_350;
          local_208 = (undefined1)(int)ROUND(local_344);
          puVar2[3] = local_208;
          puVar2 = puVar2 + param_2;
          iVar3 = iVar3 + -1;
          local_34c = local_33c + local_34c;
          local_348 = local_338 + local_348;
          local_344 = local_334 + local_344;
          local_340 = local_330 + local_340;
          local_33c = local_32c + local_33c;
          local_338 = local_328 + local_338;
          local_334 = local_324 + local_334;
          local_330 = local_2e0 + local_330;
          local_32c = local_2dc + local_32c;
          local_328 = local_2d8 + local_328;
          local_324 = local_2d4 + local_324;
        } while (iVar3 != 0);
      }
      local_310 = local_2d0 + local_310;
      local_30c = local_2cc + local_30c;
      local_308 = local_2c8 + local_308;
      local_304 = local_2c4 + local_304;
      local_300 = local_2c0 + local_300;
      local_2fc = local_2bc + local_2fc;
      local_2f8 = local_2b8 + local_2f8;
      local_2f4 = local_2b4 + local_2f4;
      local_2f0 = local_2b0 + local_2f0;
      local_2ec = local_2ac + local_2ec;
      local_2e8 = local_2a8 + local_2e8;
      local_2e4 = local_2a4 + local_2e4;
      local_2e0 = local_2a0 + local_2e0;
      local_2dc = local_29c + local_2dc;
      local_2d8 = local_298 + local_2d8;
      local_2d4 = local_294 + local_2d4;
      local_2d0 = local_290 + local_2d0;
      local_2cc = local_28c + local_2cc;
      local_2c8 = local_288 + local_2c8;
      local_2c4 = local_284 + local_2c4;
      local_2c0 = local_280 + local_2c0;
      local_2bc = local_27c + local_2bc;
      local_2b8 = local_278 + local_2b8;
      local_2b4 = local_274 + local_2b4;
      local_2b0 = local_270 + local_2b0;
      local_2ac = local_26c + local_2ac;
      local_2a8 = local_268 + local_2a8;
      local_2a4 = local_264 + local_2a4;
      param_4 = param_4 + -1;
      iVar4 = iVar4 + -1;
      local_2a0 = local_260 + local_2a0;
      local_29c = local_25c + local_29c;
      local_298 = local_258 + local_298;
      local_294 = local_254 + local_294;
      local_290 = local_250 + local_290;
      local_28c = local_24c + local_28c;
      local_288 = local_248 + local_288;
      local_284 = local_244 + local_284;
      local_280 = local_240 + local_280;
      local_27c = local_23c + local_27c;
      local_278 = local_238 + local_278;
      local_274 = local_234 + local_274;
      local_270 = local_230 + local_270;
      local_26c = local_22c + local_26c;
      local_268 = local_228 + local_268;
      local_264 = local_224 + local_264;
      local_260 = local_220 + local_260;
      local_25c = local_21c + local_25c;
      local_258 = local_218 + local_258;
      local_254 = local_214 + local_254;
    } while (iVar4 != 0);
  }
  return param_1;
}
