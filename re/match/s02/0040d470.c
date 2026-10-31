// FLAGS: /O2 /GS /Ob1
/* 0x0040d470 FUN_0040d470 (285 B): 100.0% exact, regs 100.0%.
   Raw g2s was 43.8% (Ghidra's goto form). The branch web is a toggle of DAT_0063ba88 when (DAT_007f10a1 &&
   !DAT_007f1561), then if (DAT_0063ba88 != 0) FUN_004414b0(cam) else FUN_00448220(cam, mat, param_1); the
   compiler threads the jumps. Testing == 0 first gives 80.0% (block order). Camera frame modelling matrix right
   and up rows scaled by -1.0f (0x005cc33c). Residual: none. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
typedef struct V3 { float x, y, z; } V3;
typedef struct RwMatrix { V3 right; uint flags; V3 up; uint pad1; V3 at; uint pad2; V3 pos; uint pad3; } RwMatrix;
typedef struct RwFrame { char obj[0x10]; RwMatrix modelling; } RwFrame;
typedef struct RwCamera { char pad0[4]; RwFrame *parent; } RwCamera;
MGLOBAL(int, DAT_0063ba8c, 0x0063ba8c);
MGLOBAL(char, DAT_007f10a1, 0x007f10a1);
MGLOBAL(char, DAT_007f1561, 0x007f1561);
MGLOBAL(int, DAT_0063ba88, 0x0063ba88);
MGLOBAL(int, DAT_007f0f34, 0x007f0f34);

int FUN_00405430(void);
undefined4 FUN_0046d4a0(void **p0, int p1);
RwCamera *FUN_004671a0(int p0);
void FUN_00448220(RwCamera *p0, void **p1, undefined4 p2);
void FUN_004414b0(RwCamera *p0);

// MATCH: 0x0040d470 FUN_0040d470
void FUN_0040d470(undefined4 param_1)
{
  int i;
  void *m;
  void *mat[4];
  RwMatrix *cm;

  if (DAT_0063ba8c == 3 && FUN_00405430() != 0) {
    return;
  }
  for (i = 0; i < 4; i++) {
    FUN_0046d4a0(&m, i);
    mat[i] = m;
  }
  if (DAT_007f10a1 != 0 && DAT_007f1561 == 0) {
    if (DAT_0063ba88 != 0) {
      DAT_0063ba88 = 0;
    }
    else {
      DAT_0063ba88 = 1;
    }
  }
  if (DAT_0063ba88 != 0) {
    FUN_004414b0(FUN_004671a0(0));
  }
  else {
    FUN_00448220(FUN_004671a0(0), mat, param_1);
  }
  if (DAT_007f0f34 != 0) {
    cm = &FUN_004671a0(0)->parent->modelling;
    cm->right.x = cm->right.x * -1.0f;
    cm->right.y = cm->right.y * -1.0f;
    cm->right.z = cm->right.z * -1.0f;
    cm->up.x = cm->up.x * -1.0f;
    cm->up.y = cm->up.y * -1.0f;
    cm->up.z = cm->up.z * -1.0f;
  }
}
