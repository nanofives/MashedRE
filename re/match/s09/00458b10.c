// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00458b10: 69.1% (regs 92.6%, ops 100%) by hand (raw 42.5%). Render-state calls through
   RwGlobals (DAT_007d3ff8) slot +0x20 (2 int args, caller pops), then a count-down loop over the live records
   (count DAT_0068b9a8) that renders a record's clump when dot(cam at, d) > 0 and |d|^2 < 800.
   The squared length as three statements (t = y*y; t += x*x; t += z*z) gives the original y,x,z order
   (61.7% -> 69.1%). Residual (b): (1) ours keeps the count in EBX and the camera frame in EDI, the original
   the other way round (tried declaration orders, assignment order, uint count, a camera temp, 4 loop forms,
   typed frame struct); (2) the dot product loads the frame field first in the original (fld [at.y];
   fmul st(2)), ours the local (8 operand orders x 6 term orders, inline Dot helper/macro, local V3 d did not
   move it). Permuter: only semantically wrong reorders scored higher. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct V3_00458b10 {
  float x;
  float y;
  float z;
} V3_00458b10;
typedef struct Rec00458b10 {
  int clump;
  undefined4 f04[7];
  int state;
  undefined4 f24[5];
  V3_00458b10 pos;
  V3_00458b10 vel;
} Rec00458b10;
typedef struct Mat00458b10 {
  undefined4 f00[12];
  V3_00458b10 at;
  undefined4 f3c;
  V3_00458b10 pos;
} Mat00458b10;
typedef int (*RenderStateSet00458b10)(int state, int value);
MGLOBAL(int *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_0068b9a8, 0x0068b9a8);
MGLOBALA(Rec00458b10, DAT_0068b198, 25, 0x0068b198);
MGLOBALA(undefined1, DAT_0068b968, 8, 0x0068b968);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005ce5d4, 0x005ce5d4);
void FUN_004770a0(void *p0);
int FUN_004e6680(int p0);
// MATCH: 0x00458b10 FUN_00458b10
void FUN_00458b10(void)
{
  Mat00458b10 *frame;
  int i;
  Rec00458b10 *r;
  float dx;
  float dy;
  float dz;
  float t;

  frame = *(Mat00458b10 **)(*DAT_007d3ff8 + 4);
  i = DAT_0068b9a8;
  ((RenderStateSet00458b10)DAT_007d3ff8[8])(8, 0);
  ((RenderStateSet00458b10)DAT_007d3ff8[8])(6, 1);
  FUN_004770a0(DAT_0068b968);
  ((RenderStateSet00458b10)DAT_007d3ff8[8])(8, 1);
  ((RenderStateSet00458b10)DAT_007d3ff8[8])(6, 1);
  while (i != 0) {
    i--;
    r = &DAT_0068b198[i];
    if (r->state == 1) {
      dx = r->pos.x - frame->pos.x;
      dy = r->pos.y - frame->pos.y;
      dz = r->pos.z - frame->pos.z;
      t = frame->at.y * dy;
      t += dx * frame->at.x;
      t += frame->at.z * dz;
      if (t > DAT_005d757c) {
        t = dy * dy;
        t += dx * dx;
        t += dz * dz;
        if (t < DAT_005ce5d4) {
          FUN_004e6680(r->clump);
        }
      }
    }
  }
}
