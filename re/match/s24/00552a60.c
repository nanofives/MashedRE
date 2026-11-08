// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00552a60 (h4): 100.0% (regs 100.0%). Camera setup: raster (param_1+0x60) width/height to float
   globals, doubled +0x68/+0x6c, swaps the callback at +0x1c to FUN_00552b50 (old one kept at 0x00912c58),
   aspect factors at 0x00912c00/04. Moved the score: if (param_1 != 0) {...} else {...} order (73.1 -> 78.8)
   and the height read into an int local first (the original converts it through the param_1 stack slot, 100). */
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
typedef int (*RenderCb_00552a60)(int camera);
typedef struct { int f0; int f4; int f8; int width; int height; } Raster_00552a60;
MGLOBAL(float, DAT_00912b10, 0x00912b10);
MGLOBAL(float, DAT_00912b14, 0x00912b14);
MGLOBAL(float, DAT_00912b08, 0x00912b08);
MGLOBAL(float, DAT_00912b0c, 0x00912b0c);
MGLOBAL(int, DAT_00912c0c, 0x00912c0c);
MGLOBAL(RenderCb_00552a60, DAT_00912c58, 0x00912c58);
MGLOBAL(int, DAT_00912bd8, 0x00912bd8);
MGLOBAL(float, DAT_00912bf8, 0x00912bf8);
MGLOBAL(float, DAT_00912bfc, 0x00912bfc);
MGLOBAL(float, DAT_00912c04, 0x00912c04);
MGLOBAL(float, DAT_00912c00, 0x00912c00);
int FUN_00552b50(int camera);
// MATCH: 0x00552a60 FUN_00552a60
int FUN_00552a60(int param_1)
{
  Raster_00552a60 *r;

  if (param_1 != 0) {
    r = *(Raster_00552a60 **)(param_1 + 0x60);
    if (r != 0) {
      int h = r->height;
      DAT_00912b10 = (float)r->width;
      DAT_00912b14 = (float)h;
      DAT_00912b08 = *(float *)(param_1 + 0x68) + *(float *)(param_1 + 0x68);
      DAT_00912b0c = *(float *)(param_1 + 0x6c) + *(float *)(param_1 + 0x6c);
      if (*(RenderCb_00552a60 *)(param_1 + 0x1c) != FUN_00552b50 && DAT_00912c0c != param_1) {
        DAT_00912c58 = *(RenderCb_00552a60 *)(param_1 + 0x1c);
        *(RenderCb_00552a60 *)(param_1 + 0x1c) = FUN_00552b50;
      }
      DAT_00912c0c = param_1;
      DAT_00912bd8 = 0;
      DAT_00912bf8 = 0.0f;
      DAT_00912bfc = 0.0f;
      if (DAT_00912b08 > DAT_00912b0c) {
        DAT_00912c04 = 1.0f;
        DAT_00912c00 = DAT_00912b08 / DAT_00912b0c;
      } else {
        DAT_00912c00 = 1.0f;
        DAT_00912c04 = DAT_00912b0c / DAT_00912b08;
      }
      return 1;
    }
  } else {
    DAT_00912c0c = 0;
  }
  return 0;
}
