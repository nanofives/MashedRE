// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x0056aae0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
void FUN_0056a7a0();
MGLOBAL(int, stack0x0000003c, 0x0000003c);
// MATCH: 0x0056aae0 FUN_0056aae0
void FUN_0056aae0(int *param_1,int *param_2,int param_3,undefined4 param_4,undefined4 param_5,
                 int param_6,undefined4 param_7,undefined4 param_8,int param_9,undefined4 param_10,
                 undefined4 param_11,int param_12)

{
  float *pfVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  float in_stack_000000c4;
  float in_stack_000000c8;
  float in_stack_000000cc;
  float in_stack_000000d0;
  float in_stack_000000d4;
  float in_stack_000000d8;
  float in_stack_000000dc;
  undefined4 auStack_a0 [34];
  float fStack_18;
  int iStack_14;
  
  fStack_18 = in_stack_000000d0 - in_stack_000000c4;
  if (in_stack_000000d0 - in_stack_000000c4 < in_stack_000000c8) {
    fStack_18 = in_stack_000000c8;
  }
  if (in_stack_000000cc < fStack_18) {
    fStack_18 = in_stack_000000cc;
  }
  *(float *)(param_12 + (*param_2 + *param_1 * 4) * 4) = fStack_18;
  if (fStack_18 == in_stack_000000c8) {
    *(undefined4 *)(param_6 + (*param_2 + *param_1 * 4) * 4) = 0;
  }
  else {
    *(undefined4 *)(param_6 + (*param_2 + *param_1 * 4) * 4) = 0xffffffff;
  }
  if (fStack_18 == in_stack_000000cc) {
    *(undefined4 *)(param_9 + (*param_2 + *param_1 * 4) * 4) = 0;
  }
  else {
    *(undefined4 *)(param_9 + (*param_2 + *param_1 * 4) * 4) = 0xffffffff;
  }
  fStack_18 = fStack_18 - in_stack_000000d0;
  iStack_14 = *param_2 + *param_1 * 4;
  in_stack_000000c8 = fStack_18;
  puVar3 = (undefined4 *)&stack0x0000003c;
  puVar4 = auStack_a0;
  for (iVar2 = 0x22; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + 1;
    puVar4 = puVar4 + 1;
  }
  FUN_0056a7a0(param_3,param_4,param_5);
  pfVar1 = (float *)(param_3 + (*param_2 + *param_1 * 4) * 4);
  *pfVar1 = (in_stack_000000d4 * in_stack_000000dc + in_stack_000000d8) * in_stack_000000c8 +
            *pfVar1;
  return;
}
