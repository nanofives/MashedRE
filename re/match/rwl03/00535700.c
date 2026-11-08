// FLAGS: /O2 /Ob1 /G5
// h3: 51.7% regs 94.3% ops 95.4%. One switch on the mask (not two), int array at base+0x48. Residual: ecx/edx swap for param_2 and test vs and/cmp of (param_4 & 0x40000000)==0x40000000; class (a).
/* MATCH-RWL03 0x00535700: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(int, DAT_007dc57c, 0x007dc57c);
// MATCH: 0x00535700 FUN_00535700
int FUN_00535700(int param_1,int *param_2,uint param_3,int param_4)
{
  int *iVar2;
  int iVar3;

  iVar2 = (int *)(*(int *)(DAT_007dc57c + param_1) + 0x48);
  if ((iVar2[0x16] & param_3) == 0) {
    *param_2 = 0;
    param_2[1] = 0;
    return 0;
  }
  switch(param_3) {
  case 1:
    *param_2 = iVar2[1];
    iVar3 = iVar2[2];
    break;
  case 2:
    *param_2 = iVar2[9];
    iVar3 = iVar2[10];
    break;
  default:
    goto switchD_00535744_caseD_3;
  case 4:
    *param_2 = iVar2[7];
    iVar3 = iVar2[8];
    break;
  case 8:
    *param_2 = iVar2[3];
    iVar3 = iVar2[4];
    break;
  case 0x10:
    *param_2 = iVar2[5];
    iVar3 = iVar2[6];
    break;
  case 0x20:
    *param_2 = iVar2[0xd];
    iVar3 = iVar2[0xe];
    break;
  case 0x40:
    *param_2 = iVar2[0xb];
    iVar3 = iVar2[0xc];
    break;
  case 0x80:
    *param_2 = iVar2[0xf];
    iVar3 = iVar2[0x10];
    break;
  case 0x100:
    *param_2 = iVar2[0x11];
    iVar3 = iVar2[0x12];
    break;
  }
  param_2[1] = iVar3;
switchD_00535744_caseD_3:
  if (iVar2[0x15] != 0) {
    param_2[1] = iVar2[0x15];
  }
  if ((*param_2 != 0) && ((param_4 & 0x40000000) == 0x40000000)) {
    *(uint *)(*(int *)(DAT_007dc57c + param_1) + 0x3c) |= param_3 | param_4;
  }
  return 1;
}
