// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 47.8% (regs 56.5%), class b. Prototype fixes only (FUN_004c57a0 returns a pointer, thunk args cast): file now compiles (no score before). The original copies a 13-dword stack record into the raster field by field with 0x10-stride lea bases and a single and-3 temp; needs the record as a struct and member-wise copy, not done. */
/* MATCH-RWL01 0x004cf2b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
undefined4 * FUN_004c57a0(void);
uint FUN_004cbd30(int p0, undefined4 * p1, uint p2);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, int * p2, uint * p3);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
void thunk_FUN_004c4680(float * p0, float * p1);
// MATCH: 0x004cf2b0 FUN_004cf2b0
/* [C1 2026-05-19] Signature `RwRaster* f(int stream, RwRaster* dst_or_null)`. */

undefined4 * FUN_004cf2b0(int param_1,undefined4 *param_2)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  uint local_40 [3];
  undefined4 local_34 [4];
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  uint local_4;
  
  iVar2 = param_1;
  iVar1 = FUN_004cc5e0(param_1,1,&param_1,local_40);
  if (iVar1 == 0) {
    local_40[1] = 1;
    local_40[2] = FUN_004d7ff0(0x8000001a);
  }
  else {
    if ((0x34fff < local_40[0]) && (local_40[0] < 0x37003)) {
      puVar3 = local_34;
      for (iVar1 = 0xd; iVar1 != 0; iVar1 = iVar1 + -1) {
        *puVar3 = 0;
        puVar3 = puVar3 + 1;
      }
      iVar2 = FUN_004cbd30(iVar2,local_34,param_1);
      if (param_1 == iVar2) {
        puVar3 = param_2;
        if ((param_2 == (undefined4 *)0x0) &&
           (puVar3 = FUN_004c57a0(), puVar3 == (undefined4 *)0x0)) {
          return (undefined4 *)0x0;
        }
        puVar3[3] = 0;
        *puVar3 = local_34[0];
        puVar3[1] = local_34[1];
        puVar3[2] = local_34[2];
        puVar3[4] = local_34[3];
        puVar3[5] = local_24;
        puVar3[6] = local_20;
        puVar3[8] = local_1c;
        puVar3[9] = local_18;
        puVar3[10] = local_14;
        puVar3[0xc] = local_10;
        puVar3[0xd] = local_c;
        puVar3[3] = local_4 & 3;
        puVar3[0xe] = local_8;
        if ((char)(local_4 & 3) != '\x03') {
          return puVar3;
        }
        thunk_FUN_004c4680((float *)puVar3,(float *)puVar3);
        return puVar3;
      }
      local_40[1] = 1;
      local_40[2] = FUN_004d7ff0(0x8000001a);
      FUN_004d8480(local_40 + 1);
      return (undefined4 *)0x0;
    }
    local_40[1] = 1;
    local_40[2] = FUN_004d7ff0(0x80000004);
  }
  FUN_004d8480(local_40 + 1);
  return (undefined4 *)0x0;
}
