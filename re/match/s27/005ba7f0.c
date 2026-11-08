// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005ba7f0: 18.9% (regs 38.2%), raw Ghidra shape (C++ fixes: FUN_005bc750 prototype is now (int **, int *) as matched in its own file). Not iterated. Class: b.
/* MATCH-S27 0x005ba7f0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs27_20261107/results.tsv. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
typedef void *HWND;
int * FUN_005aab70(int * p0, int p1);
undefined4 FUN_005baa60(int p0, byte p1, undefined4 * p2);
int FUN_005bc750(int **p0, int *p1);
__declspec(dllimport) HWND __stdcall GetForegroundWindow(void);
// MATCH: 0x005ba7f0 FUN_005ba7f0
int FUN_005ba7f0(int param_1)

{
  float *pfVar1;
  int *piVar2;
  undefined4 *puVar3;
  uint uVar4;
  int iVar5;
  HWND pHVar6;
  undefined4 uVar7;
  
  FUN_005aab70((int *)param_1,1);
  if (*(int *)(param_1 + 0x3c) != 0) {
    pfVar1 = *(float **)(*(int *)(param_1 + 0x3c) + 0x20);
    piVar2 = *(int **)(param_1 + 0x8c);
    if ((((uint)pfVar1[0x12] & 0x7f) != 0) && ((*(byte *)(param_1 + 0x78) & 0x40) != 0)) {
      pfVar1[0x12] = (float)((uint)pfVar1[0x12] & 0xffffff80);
    }
    if (((uint)pfVar1[0x12] & 0x20) != 0) {
      (**(code **)(*piVar2 + 0x30))(piVar2,pfVar1[0x10],0);
      pfVar1[0x12] = (float)((uint)pfVar1[0x12] & 0xffffffdf);
    }
    if (((uint)pfVar1[0x12] & 0x40) != 0) {
      (**(code **)(*piVar2 + 0x2c))(piVar2,pfVar1[0x11],0);
      pfVar1[0x12] = (float)((uint)pfVar1[0x12] & 0xffffffbf);
    }
    if (((uint)pfVar1[0x12] & 0x10) != 0) {
      (**(code **)(*piVar2 + 0x3c))(piVar2,pfVar1[0xf],0);
      pfVar1[0x12] = (float)((uint)pfVar1[0x12] & 0xffffffef);
    }
    if (((uint)pfVar1[0x12] & 0xc) != 0) {
      for (puVar3 = *(undefined4 **)(param_1 + 0x94); puVar3 != (undefined4 *)(param_1 + 0x94);
          puVar3 = (undefined4 *)*puVar3) {
        puVar3[-0x20] = -*pfVar1;
        puVar3[-0x1f] = pfVar1[1];
        puVar3[-0x1e] = pfVar1[2];
        uVar4 = puVar3[-0x3e];
        puVar3[-0x3e] = uVar4 | 4;
        puVar3[-0x1d] = -pfVar1[3];
        puVar3[-0x1c] = pfVar1[4];
        puVar3[-0x1b] = pfVar1[5];
        puVar3[-0x3e] = uVar4 | 4;
      }
    }
    if (((uint)pfVar1[0x12] & 0xc) != 0) {
      (**(code **)(**(int **)(param_1 + 0x8c) + 0x34))
                (*(int **)(param_1 + 0x8c),-pfVar1[9],pfVar1[10],pfVar1[0xb],-pfVar1[6],pfVar1[7],
                 pfVar1[8],0);
      pfVar1[0x12] = (float)((uint)pfVar1[0x12] & 0xfffffff3);
    }
    if (((uint)pfVar1[0x12] & 1) != 0) {
      (**(code **)(*piVar2 + 0x38))(piVar2,-*pfVar1,pfVar1[1],pfVar1[2],0);
      pfVar1[0x12] = (float)((uint)pfVar1[0x12] & 0xfffffffe);
    }
    if (((uint)pfVar1[0x12] & 2) != 0) {
      (**(code **)(*piVar2 + 0x40))(piVar2,-pfVar1[3],pfVar1[4],pfVar1[5],0);
      pfVar1[0x12] = (float)((uint)pfVar1[0x12] & 0xfffffffd);
    }
  }
  if ((*(int *)(param_1 + 0x30) == 0) && ((*(uint *)(param_1 + 0x78) & 0x400) == 0)) {
    if (*(short *)(param_1 + 0x36) == 0) {
      uVar7 = 2;
      iVar5 = **(int **)(param_1 + 0x7c);
      pHVar6 = GetForegroundWindow();
      (**(code **)(iVar5 + 0x18))(*(undefined4 *)(param_1 + 0x7c),pHVar6,uVar7);
      *(undefined2 *)(param_1 + 0x36) = 0x1e;
    }
    *(short *)(param_1 + 0x36) = *(short *)(param_1 + 0x36) + -1;
  }
  if ((*(char *)(param_1 + 0x78) < '\0') && ((*(byte *)(param_1 + 0x78) & 0xf) != 0)) {
    if ((*(byte *)(param_1 + 0x78) & 7) == 0) {
      if ((*(byte *)(param_1 + 0x78) & 8) != 0) {
        FUN_005bc750((int **)(param_1 + 0xa8),(int *)(param_1 + 0x44));
      }
    }
    else {
      FUN_005baa60(param_1 + 0x80,*(undefined4 *)(param_1 + 0x78),(undefined4 *)(param_1 + 0x44));
    }
    *(uint *)(param_1 + 0x78) = *(uint *)(param_1 + 0x78) & 0xffffff7f;
  }
  if ((*(uint *)(param_1 + 0x78) & 0x100) != 0) {
    switch(*(undefined4 *)(param_1 + 0x74)) {
    default:
      uVar7 = 4;
      break;
    case 2:
      uVar7 = 2;
      break;
    case 3:
      uVar7 = 1;
      break;
    case 4:
      uVar7 = 5;
      break;
    case 5:
      uVar7 = 6;
      break;
    case 6:
      uVar7 = 7;
    }
    (**(code **)(**(int **)(param_1 + 0x7c) + 0x24))(*(int **)(param_1 + 0x7c),uVar7);
    *(uint *)(param_1 + 0x78) = *(uint *)(param_1 + 0x78) & 0xfffffeff;
  }
  return param_1;
}
