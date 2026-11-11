// FLAGS: /O2 /Ob1 /G5
/* MATCH-RT2D-FONT 2026-11-11: 100% (was 79.8). Width/height as one RwV2d-shaped local (sz), scale a separate float, corners as 4 V3d with each product written out as sz.x * scale * k. The V2d gives the original stack layout. */
/* MATCH-RWL04 0x00552920 (h2): 78.0% (regs 78.0, ops 90.2). Typed: in[12]/out[12] float arrays for the two 4x3 vertex blocks, scale in a 2-float array sa[2] (forces the original frame 0x70 with s at +0), global reads moved after the if. Residue: stack slot order of s/fVar1/fVar2 and CSE of fVar*s (orig computes once and spills, ours re-multiplies). class (b). */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
MGLOBAL(int, DAT_00912c0c, 0x00912c0c);
MGLOBAL(float, DAT_00912b08, 0x00912b08);
MGLOBAL(float, DAT_00912b0c, 0x00912b0c);
MGLOBAL(float, DAT_005cd50c, 0x005cd50c);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, DAT_00912be0, 0x00912be0);
MGLOBAL(undefined1, DAT_00912b58, 0x00912b58);
typedef struct { float x, y, z; } V3d;
typedef struct { float x, y; } V2d;
void FUN_004c3d90(V3d *p0, V3d *p1, int p2, undefined1 *p3);
void FUN_00552e40(void);
void FUN_00552fa0(int p0);
int FUN_00553030(int p0, float p1, float p2);
int FUN_005530f0(int p0, float p1, float p2);
int FUN_005533d0(int p0);
// MATCH: 0x00552920 FUN_00552920
int FUN_00552920(int param_1)
{
  float scale;
  V2d sz;
  V3d in[4];
  V3d out[4];
  if (*(int *)(DAT_00912c0c + 0x14) == 2) {
    scale = 1.0f;
  }
  else {
    scale = DAT_00912be0;
  }
  sz.x = DAT_00912b08;
  sz.y = DAT_00912b0c;
  FUN_00552e40();
  in[0].x = sz.x * scale * -0.5f; in[0].y = sz.y * scale * -0.5f; in[0].z = 0;
  in[1].x = sz.x * scale * -0.5f; in[1].y = sz.y * scale * 0.5f; in[1].z = 0;
  in[2].x = sz.x * scale * 0.5f; in[2].y = sz.y * scale * 0.5f; in[2].z = 0;
  in[3].x = sz.x * scale * 0.5f; in[3].y = sz.y * scale * -0.5f; in[3].z = 0;
  FUN_004c3d90(out,in,4,&DAT_00912b58);
  FUN_00552fa0(param_1);
  FUN_00553030(param_1,out[0].x,out[0].y);
  FUN_005530f0(param_1,out[1].x,out[1].y);
  FUN_005530f0(param_1,out[2].x,out[2].y);
  FUN_005530f0(param_1,out[3].x,out[3].y);
  FUN_005533d0(param_1);
  return 1;
}
