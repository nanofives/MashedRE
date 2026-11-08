// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x005533d0 h1: best 55.0% (regs 61.1%, ops 87.9%); structure only (441 B float code: orig keeps fVar4 in a stack slot reused by fst/fmul, extra frame 0x18 vs ours 0x10). Plain float temporaries scored above float10 casts. Class (b). */
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
typedef struct P { int a; float x; float y; float dx; float dy; float len; } P;
typedef struct Q { int *a; int b; int c; } Q;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
float FUN_004c3b30(float p0);
float FUN_004c3b90(float p0);
int FUN_0055deb0(int p0);
int FUN_005c4bb0(int p0, int p1);
int FUN_005c4d30(int p0);
int FUN_005c4df0(int p0, int p1);
// MATCH: 0x005533d0 FUN_005533d0
int FUN_005533d0(int param_1)
{
  int *puVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  P *puVar8;
  float fVar9;
  float fVar10;
  
  puVar1 = *(int **)(param_1 + 0x18);
  iVar5 = FUN_0055deb0(*puVar1);
  iVar6 = FUN_005c4d30(*puVar1);
  iVar7 = FUN_005c4df0(*puVar1,iVar5 + -1);
  fVar2 = *(float *)(iVar6 + 4);
  fVar3 = *(float *)(iVar6 + 8);
  if ((fVar3 - *(float *)(iVar7 + 8) != DAT_005d757c) ||
     (*(float *)(iVar7 + 4) - fVar2 != DAT_005d757c)) {
    iVar6 = FUN_0055deb0(**(int **)(param_1 + 0x18));
    puVar8 = (P *)FUN_005c4bb0(**(int **)(param_1 + 0x18),0x30190);
    fVar4 = fVar3 - ((float *)puVar8)[-4];
    puVar8->a = 1;
    puVar8->x = fVar2;
    puVar8->y = fVar3;
    fVar2 = ((float *)puVar8)[-5] - fVar2;
    fVar3 = fVar4 * fVar4 + fVar2 * fVar2;
    fVar9 = FUN_004c3b30(fVar3);
    fVar10 = FUN_004c3b90(fVar3);
    puVar8->dx = fVar4 * fVar10;
    puVar8->dy = fVar10 * fVar2;
    puVar8->len = fVar9 + ((float *)puVar8)[-1];
    if (iVar6 < 2) {
      ((float *)puVar8)[-3] = puVar8->dx;
      ((float *)puVar8)[-2] = puVar8->dy;
    }
    else {
      fVar3 = ((float *)puVar8)[-3] + fVar4 * fVar10;
      fVar4 = ((float *)puVar8)[-2] + fVar10 * fVar2;
      fVar2 = DAT_005cc320 / (((float *)puVar8)[-2] * fVar4 + ((float *)puVar8)[-3] * fVar3);
      ((float *)puVar8)[-3] = fVar2 * fVar3;
      ((float *)puVar8)[-2] = fVar2 * fVar4;
    }
    iVar6 = FUN_005c4d30(*puVar1);
    iVar7 = FUN_005c4df0(*puVar1,iVar5);
  }
  fVar2 = *(float *)(iVar7 + 0xc) + *(float *)(iVar6 + 0xc);
  fVar4 = *(float *)(iVar7 + 0x10) + *(float *)(iVar6 + 0x10);
  fVar3 = DAT_005cc320 / (*(float *)(iVar6 + 0xc) * fVar2 + *(float *)(iVar6 + 0x10) * fVar4);
  *(float *)(iVar7 + 0xc) = fVar3 * fVar2;
  *(float *)(iVar7 + 0x10) = fVar3 * fVar4;
  *(int *)(iVar6 + 0xc) = *(int *)(iVar7 + 0xc);
  *(int *)(iVar6 + 0x10) = *(int *)(iVar7 + 0x10);
  puVar1[2] = 1;
  return param_1;
}
