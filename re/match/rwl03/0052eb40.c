// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR06 2026-11-08: 88.0% (was 76.5). Copy loop as while (n--) with index*0x40 offsets, header fields stored through param_1[3]/[5]. Residual: original loads param_4 into ecx then lea ebp,[ecx*4]; ours loads it into ebp and shl. Supersedes the header below. */
// h3: 76.5% regs 79.4%. Memcpy/memset intrinsics replace Ghidra loops. Residual: sz4 hoist (lea ebp,[ecx*4]) and the matrix loop; class (b). NOTE the permuter's 79.6% variant used locals before assignment and was rejected.
/* MATCH-RWL03 0x0052eb40: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
void *memset(void *, int, unsigned int);
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memset, memcpy)
typedef int (*Cb108_52eb40)(unsigned int, int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x0052eb40 FUN_0052eb40
int FUN_0052eb40(int *param_1,int param_2,uint param_3,uint param_4,void *param_5,
            void *param_6,void *param_7,int param_8)
{
  char *puVar1;
  int uVar2;
  int iVar3;
  int iVar4;
  uint uVar6;
  uint sz4;

  uVar6 = param_2 * 0x40 + param_4 * 0x14 + 0xf + param_3;
  puVar1 = (char *)(*(Cb108_52eb40 *)(DAT_007d3ff8 + 0x108))(uVar6,0x30116);
  uVar2 = 0;
  param_1[0xf] = (int)puVar1;
  if (puVar1 != 0) {
    memset(puVar1, 0, uVar6);
    puVar1 = (char *)param_1[0xf];
    param_1[1] = param_3;
    param_1[3] = (uint)((int)puVar1 + param_3 + 0xf) & 0xfffffff0;
    *param_1 = param_2;
    param_1[5] = param_1[3] + param_2 * 0x40;
    param_1[2] = (int)puVar1;
    param_1[6] = param_1[5] + param_4 * 4;
    if ((param_5 != 0) && (param_3 != 0)) {
      memcpy(puVar1, param_5, param_3);
    }
    if (param_8 != 0) {
      iVar3 = *param_1;
      while (iVar3--) {
        memcpy((char *)(param_1[3] + iVar3 * 0x40), (char *)(param_8 + iVar3 * 0x40), 0x40);
      }
    }
    if (param_7 != 0) {
      memcpy((void *)param_1[5], param_7, param_4 * 4);
    }
    if (param_6 != 0) {
      memcpy((void *)param_1[6], param_6, param_4 << 4);
    }
    uVar2 = 1;
  }
  return uVar2;
}
