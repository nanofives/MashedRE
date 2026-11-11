// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x004f2e70: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int (*codeptr)(...);
#else
typedef int (*codeptr)();
#endif
#define CONCAT22(a,b) ((uint)(((uint)(ushort)(a)<<16)|(ushort)(b)))
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_004cc7f0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
int FUN_004f2f90(int p0, int p1, short p2, short p3, uint p4, int p5);
// MATCH: 0x004f2e70 FUN_004f2e70
typedef struct T2e70 { short v[3]; short pad[7]; } T2e70;
int FUN_004f2e70(uint param_1,undefined4 *param_2,int param_3,short *param_4)

{
  int *iVar1;
  undefined4 uVar2;
  int iVar3;
  short *puVar4;
  T2e70 *tri = (T2e70 *)param_4;
  uint uVar5;

  iVar1 = (int *)(**(codeptr *)(DAT_007d3ff8 + 0x108))(param_1 * 4,0x30502);
  uVar2 = FUN_004cc7f0(0x24,param_1,4,0x30502);
  *param_2 = uVar2;
  uVar2 = FUN_004cc7f0(0x10,(param_1 >> 1) + 1,4,0x30502);
  param_2[1] = uVar2;
  for (uVar5 = 0; uVar5 < param_1; uVar5++) {
      iVar1[uVar5] = (**(codeptr *)(DAT_007d3ff8 + 0x118))(*param_2,0x30502);
      *(undefined1 *)(iVar1[uVar5] + 0x20) = 0;
      *(uint *)iVar1[uVar5] = uVar5;
      *(undefined4 *)(iVar1[uVar5] + 0x14) = 0;
      *(undefined4 *)(iVar1[uVar5] + 0x10) = 0;
      *(undefined4 *)(iVar1[uVar5] + 0x18) = 0;
      *(undefined4 *)(iVar1[uVar5] + 4) = FUN_004f2f90(param_2[1],param_3,tri[uVar5].v[0],tri[uVar5].v[1],uVar5,(int)iVar1);
      *(undefined4 *)(iVar1[uVar5] + 8) = FUN_004f2f90(param_2[1],param_3,tri[uVar5].v[1],tri[uVar5].v[2],uVar5,(int)iVar1);
      *(undefined4 *)(iVar1[uVar5] + 0xc) = FUN_004f2f90(param_2[1],param_3,tri[uVar5].v[2],tri[uVar5].v[0],uVar5,(int)iVar1);
  }
  return (int)iVar1;
}
