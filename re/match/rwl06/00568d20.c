// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00568d20: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
void FUN_00564100(undefined4 * p0, undefined1 p1, uint p2);
// MATCH: 0x00568d20 FUN_00568d20
uint * FUN_00568d20(int param_1,uint param_2)

{
  uint uVar1;
  uint *puVar2;
  
  puVar2 = (uint *)(*(int *)(param_1 + 8) + 0xfU & 0xfffffff0);
  *(uint *)(param_1 + 8) = (uint)(puVar2 + 4) + 0xfU & 0xfffffff0;
  puVar2[3] = *(uint *)(param_1 + 8);
  FUN_00564100(*(undefined4 *)(param_1 + 8),0,param_2 * 0x28);
  uVar1 = 1;
  *(uint *)(param_1 + 8) = param_2 * 0x28 + 0xf + *(int *)(param_1 + 8) & 0xfffffff0;
  puVar2[2] = param_2;
  param_2 = param_2 >> 1;
  puVar2[1] = 1;
  if (param_2 != 0) {
    do {
      uVar1 = uVar1 * 2;
      param_2 = param_2 >> 1;
    } while (param_2 != 0);
    puVar2[1] = uVar1;
  }
  uVar1 = *(int *)(param_1 + 8) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar1;
  *puVar2 = uVar1;
  FUN_00564100(*(undefined4 *)(param_1 + 8),0,puVar2[1] << 2);
  uVar1 = 0;
  *(uint *)(param_1 + 8) = *(int *)(param_1 + 8) + 0xf + puVar2[1] * 4 & 0xfffffff0;
  if (puVar2[1] > 0) {
    do {
      uVar1 = uVar1 + 1;
      *(undefined4 *)((*puVar2 - 4) + uVar1 * 4) = 0;
    } while (uVar1 < puVar2[1]);
  }
  return puVar2;
}
