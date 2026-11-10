// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00426810 (h1, by hand): best 97.9% / regs 97.9% (raw converter 15.5%), 142/142 ins, all ops
   and stack offsets right, 29 bytes differ over the 672 B symbol. Shape: RwV3d/RwMatrix/RwRGBA locals in the original
   frame order, RwMatrixSetIdentity-macro chained stores, RwMatrixRotate(&m,&yaxis,angle,0), RwV3dTransformVector,
   RwV3dNormalize, then t = (dot - K) * K2 in DAT_0066d71c and three _ftol colour lerps from DAT_005f6170 to
   DAT_005f6174. The dot is written z + (x + y): the plain x + y + z form loads all three products first (93.7%).
   Residual (b) source shape: 3 scheduling swaps in the colour lerps (the red int-temp store vs the z product, the
   red byte store vs the green load, the DAT_005f6174.blue load vs the green byte store). Tried: lerp operand order
   and casts, separate uchar locals, uchar[3]/[4] colour, global array RwRGBA[2], static globals (7 combos), dot
   into a temp, pointer dot macro, operands swapped; match_permute twice (586 compiles, no gain). */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
typedef struct { float x, y, z; } RwV3d;
typedef struct { RwV3d right; unsigned int flags; RwV3d up; unsigned int pad1; RwV3d at; unsigned int pad2; RwV3d pos; unsigned int pad3; } RwMatrix;
typedef struct { unsigned char red, green, blue, alpha; } RwRGBA;
MGLOBAL(RwRGBA, DAT_005f6170, 0x005f6170);
MGLOBAL(RwRGBA, DAT_005f6174, 0x005f6174);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cd50c, 0x005cd50c);
MGLOBAL(float, DAT_0066d71c, 0x0066d71c);
void FUN_004924c0(unsigned char p0, unsigned char p1, unsigned char p2);
int FUN_004c1b10(int p0, float p1);
float FUN_004c39b0(RwV3d *p0, RwV3d *p1);
RwV3d *FUN_004c3dc0(RwV3d *p0, RwV3d *p1, RwMatrix *p2);
RwMatrix *FUN_004c4d20(RwMatrix *p0, RwV3d *p1, float p2, int p3);
// MATCH: 0x00426810 FUN_00426810
void FUN_00426810(int param_1, int param_2)
{
  RwMatrix *ltm;
  RwV3d dir = {0.0f, 0.0f, 1.0f};
  RwV3d axis = {0.0f, 1.0f, 0.0f};
  RwMatrix rot;
  RwV3d at;
  RwRGBA col;
  float angle;
  float fog;

  ltm = (RwMatrix *)(*(int *)(param_2 + 4) + 0x10);
  angle = *(float *)(param_1 + 0x1060c);
  if (*(char *)(param_1 + 0x10600) != 0 || *(char *)(param_1 + 0x10601) != 0 ||
      *(char *)(param_1 + 0x10602) != 0) {
    DAT_005f6170 = *(RwRGBA *)(param_1 + 0x105f4);
    DAT_005f6174 = *(RwRGBA *)(param_1 + 0x10600);
    rot.right.x = rot.up.y = rot.at.z = 1.0f;
    rot.right.y = rot.right.z = rot.up.x = 0.0f;
    rot.up.z = rot.at.x = rot.at.y = 0.0f;
    rot.pos.x = rot.pos.y = rot.pos.z = 0.0f;
    rot.flags = rot.flags | 0x20003;
    FUN_004c4d20(&rot, &axis, angle, 0);
    FUN_004c3dc0(&dir, &dir, &rot);
    at.x = ltm->at.x;
    at.y = 0.0f;
    at.z = ltm->at.z;
    FUN_004c39b0(&at, &at);
    DAT_0066d71c = (float)((double)((at.z * dir.z + (at.x * dir.x + at.y * dir.y) - DAT_005cc320)) * DAT_005cd50c);
    col.red = (unsigned char)((float)(DAT_005f6174.red - DAT_005f6170.red) * DAT_0066d71c + DAT_005f6170.red);
    col.green = (unsigned char)((float)(DAT_005f6174.green - DAT_005f6170.green) * DAT_0066d71c + DAT_005f6170.green);
    col.blue = (unsigned char)((float)(DAT_005f6174.blue - DAT_005f6170.blue) * DAT_0066d71c + DAT_005f6170.blue);
    FUN_004924c0(col.red, col.green, col.blue);
    fog = (*(float *)(param_1 + 0x10604) - *(float *)(param_1 + 0x105f8)) * DAT_0066d71c + *(float *)(param_1 + 0x105f8);
    FUN_004c1b10(param_2, (*(float *)(param_1 + 0x10608) - *(float *)(param_1 + 0x105fc)) * DAT_0066d71c + *(float *)(param_1 + 0x105fc));
    *(float *)(param_2 + 0x88) = fog;
  }
}
