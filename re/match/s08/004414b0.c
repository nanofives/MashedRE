// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x004414b0: 89.8% (regs 89.8%), raw g2s 28.9%. By hand: RwMatrix struct (frame+0x10), V3 struct copy of m->pos, .rdata compiler literals (0.01, 1.333, -1, -48, 48, -0.015625, 0.002) written as literals so the operand order matches; 0x005d757c (0.0) stays a global (a literal 0 gives fldz). Tried: global vs literal constants, operand order, inline V3Scale helper, CSE scale expression, separate scale variable, permuter 2x (no gain). Residual (b): the -1 scale of pos is interleaved with the arg pushes in the original, and the at/right scale keeps the scalar on the x87 stack to the end (fld st0 / fmul st1 / fstp st0) where ours consumes it on the last use. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3;
typedef struct { V3 right; int flags; V3 up; int pad1; V3 at; int pad2; V3 pos; int pad3; } RwMatrix;
MGLOBAL(char, DAT_007f108b, 0x007f108b);
MGLOBAL(char, DAT_007f108c, 0x007f108c);
MGLOBAL(float, DAT_005f7a40, 0x005f7a40);
MGLOBALA(char, DAT_008989c0, 4, 0x008989c0);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(V3, DAT_006146fc, 0x006146fc);
int FUN_004c1c80(int cam, float *vw);
RwMatrix *FUN_004c4d20(RwMatrix *m, V3 *axis, float angle, int op);
RwMatrix *FUN_004c51a0(RwMatrix *m, V3 *v, int op);

// MATCH: 0x004414b0 FUN_004414b0
void FUN_004414b0(int param_1)
{
  float vw[2];
  V3 pos;
  V3 neg;
  V3 t;
  RwMatrix *m;
  float a;

  if (DAT_007f108b != '\0') {
    DAT_005f7a40 = DAT_005f7a40 + 0.01f;
  }
  if (DAT_007f108c != '\0') {
    DAT_005f7a40 = DAT_005f7a40 - 0.01f;
  }
  if (DAT_005f7a40 < 0.01f) {
    DAT_005f7a40 = 0.01f;
  }
  vw[0] = DAT_005f7a40 * 1.333f;
  vw[1] = DAT_005f7a40;
  FUN_004c1c80(param_1, vw);
  m = (RwMatrix *)(*(int *)(param_1 + 4) + 0x10);
  pos = m->pos;
  neg.x = pos.x * -1.0f;
  neg.y = pos.y * -1.0f;
  neg.z = pos.z * -1.0f;
  FUN_004c51a0(m, &neg, 2);
  a = (float)DAT_008989c0[1];
  if (a > -48.0f && a < 48.0f) {
    a = DAT_005d757c;
  }
  FUN_004c4d20(m, &DAT_006146fc, a * -0.015625f, 2);
  a = (float)DAT_008989c0[0];
  if (a > -48.0f && a < 48.0f) {
    a = DAT_005d757c;
  }
  FUN_004c4d20(m, &m->right, a * -0.015625f, 2);
  FUN_004c51a0(m, &pos, 2);
  a = -(float)DAT_008989c0[2];
  if (a > -48.0f && a < 48.0f) {
    a = DAT_005d757c;
  }
  a = a * 0.002f;
  t.x = a * m->at.x;
  t.y = m->at.y * a;
  t.z = m->at.z * a;
  FUN_004c51a0(m, &t, 2);
  a = -(float)DAT_008989c0[3];
  if (a > -48.0f && a < 48.0f) {
    a = DAT_005d757c;
  }
  a = a * 0.002f;
  t.x = a * m->right.x;
  t.y = m->right.y * a;
  t.z = m->right.z * a;
  FUN_004c51a0(m, &t, 2);
}
