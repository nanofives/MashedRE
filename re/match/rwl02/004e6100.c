// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e6100: best 31.7% (regs 60.3%), residual class b. x87 operand order: orig loads [4],[0],[8] and keeps squares in stack slots (param slot reused as a float temp); structure only, 31.7. */
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

int FUN_004c0b10(int p0);
int FUN_004c0ed0(int p0);
float FUN_004c3b30(float p0);
undefined4 FUN_004c3d60(int p0, int p1, float *p2);
void FUN_004e5fc0(int p0);
// MATCH: 0x004e6100 FUN_004e6100
int FUN_004e6100(int param_1)
{
  int uVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  int iVar6;
  float *pfVar7;
  uVar1 = *(int *)(param_1 + 4);
  if ((*(byte *)(param_1 + 0x4c) & 2) != 0) {
    FUN_004e5fc0(param_1);
  }
  iVar6 = FUN_004c0b10(uVar1);
  if ((iVar6 != 0) || ((*(byte *)(param_1 + 3) & 1) != 0)) {
    pfVar7 = (float *)FUN_004c0ed0(uVar1);
    FUN_004c3d60(param_1 + 0x2c,param_1 + 0x1c,pfVar7);
    if ((*(byte *)&pfVar7[3] & 3) != 3) {
      fVar5 = pfVar7[2] * pfVar7[2] + pfVar7[1] * pfVar7[1] + *pfVar7 * *pfVar7;
      fVar3 = pfVar7[6] * pfVar7[6] + pfVar7[5] * pfVar7[5] + pfVar7[4] * pfVar7[4];
      fVar4 = pfVar7[10] * pfVar7[10] + pfVar7[9] * pfVar7[9] + pfVar7[8] * pfVar7[8];
      fVar2 = fVar4;
      if (fVar3 >= fVar4) {
        fVar2 = fVar3;
      }
      if ((fVar5 < fVar2) && (fVar5 = fVar4, fVar3 >= fVar4)) {
        fVar5 = fVar3;
      }
      fVar2 = FUN_004c3b30(fVar5);
      *(byte *)(param_1 + 3) = *(byte *)(param_1 + 3) & 0xfe;
      *(float *)(param_1 + 0x38) = fVar2 * *(float *)(param_1 + 0x28);
      return param_1 + 0x2c;
    }
    *(undefined4 *)(param_1 + 0x38) = *(undefined4 *)(param_1 + 0x28);
    *(byte *)(param_1 + 3) = *(byte *)(param_1 + 3) & 0xfe;
  }
  return param_1 + 0x2c;
}
