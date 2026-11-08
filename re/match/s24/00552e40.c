// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00552e40 (h1): 100% (regs 100%). Written from odis.py: FUN_004c4600 takes 3 args (Ghidra showed 4),
   one RwV3d reused for both FUN_004c5010 scales, w/h copied to locals after the first multiply, min(w,h) as ?:,
   the translate terms as `C * (w * scale)` (other operand orders give `fld C` first, 94.7%), RwMatrix struct copy
   for the 16-dword rep movsd. */
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
typedef struct RwV3d { float x, y, z; } RwV3d;
typedef struct RwMatrix { RwV3d right; int flags; RwV3d up; int pad1; RwV3d at; int pad2; RwV3d pos; int pad3; } RwMatrix;
typedef struct Cam { int f0; void *frame; int f8; int fc; int f10; int projection; } Cam;
RwMatrix *FUN_004c0ed0(void *frame);
RwMatrix *FUN_004c4600(RwMatrix *dst, const RwMatrix *a, const RwMatrix *b);
RwMatrix *FUN_004c4dc0(RwMatrix *dst, const RwMatrix *src);
RwMatrix *FUN_004c5010(RwMatrix *m, const RwV3d *scale, int combine);
RwMatrix *FUN_004c51a0(RwMatrix *m, const RwV3d *t, int combine);
MGLOBAL(int, DAT_00912bd8, 0x00912bd8);
MGLOBAL(Cam *, DAT_00912c0c, 0x00912c0c);
MGLOBAL(int, DAT_00912b04, 0x00912b04);
MGLOBAL(float, DAT_00912b08, 0x00912b08);
MGLOBAL(float, DAT_00912b0c, 0x00912b0c);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, DAT_00912be0, 0x00912be0);
MGLOBAL(RwMatrix, DAT_00912a44, 0x00912a44);
MGLOBALA(RwMatrix *, DAT_00912a84, 1, 0x00912a84);
MGLOBAL(RwMatrix, DAT_00912b18, 0x00912b18);
MGLOBAL(RwMatrix, DAT_00912b58, 0x00912b58);
MGLOBAL(RwMatrix, DAT_00912b98, 0x00912b98);

// MATCH: 0x00552e40 FUN_00552e40
RwMatrix *FUN_00552e40(void)
{
  float scale;
  float w;
  float h;
  float m;
  RwV3d v;
  RwV3d t;
  RwMatrix mat;

  if (DAT_00912bd8 == 0) {
    if (DAT_00912c0c->projection == 2) {
      scale = 1.0f;
    }
    else {
      scale = DAT_00912be0;
    }
    FUN_004c4600(&mat, DAT_00912a84[DAT_00912b04], &DAT_00912a44);
    w = DAT_00912b08;
    h = DAT_00912b0c;
    v.x = scale;
    v.y = scale;
    v.z = 1.0f;
    FUN_004c5010(&mat, &v, 2);
    m = (w < h) ? w : h;
    v.x = -m;
    v.y = m;
    v.z = 1.0f;
    FUN_004c5010(&mat, &v, 2);
    t.x = DAT_005cc32c * (w * scale);
    t.y = -(DAT_005cc32c * (h * scale));
    t.z = 0.0f;
    FUN_004c51a0(&mat, &t, 2);
    DAT_00912b18 = mat;
    FUN_004c4dc0(&DAT_00912b58, &mat);
    DAT_00912bd8 = 1;
  }
  FUN_004c4600(&DAT_00912b98, &DAT_00912b18, FUN_004c0ed0(DAT_00912c0c->frame));
  return &DAT_00912b98;
}
