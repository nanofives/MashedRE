// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 h4 0x004f5030 (1908 B): best 54.9% (regs 55.2%, ops 85.2%) under /O2 /Ob1 /G5.
   15-case switch on the D3DDECLTYPE index (0..14), source rows of 4 floats (16 B), returns DAT_005dae40[type]
   (per-type byte size). Float->int is RenderWare's inline-asm form (`fld x; fistp r` in an __inline helper,
   the RwInt32FromRealMacro shape): with it the conversions, the edx return index and the instruction count
   (590/594) line up. Structure: jump table, per-case count>0 test, count-down loops, mem addressed as
   `mem + i * stride` with src++ in most cases (gives the original's mem bias), `x >= 1.0` compare order for
   cases 4/8 (`and eax,0x100; jne`). Permuter (1732 compiles) moved the 32767/65535 lane store and the UDEC3
   z conversion: 53.7 -> 54.9.
   Residual class (b): the frame is sub esp,0x20 against the original's 0x10. The original packs the helper's
   x / r temporaries into the dead parameter slots (+0x28..+0x34) plus 4 locals; ours gets 8 fresh locals, so
   every esp offset in cases 4..14 differs, and cases 0..2 load mem after the count test (original: before).
   Tried: inline helper vs a block macro `{ float t_ = (x); __asm fld t_ __asm fistp r }` with shared or
   per-site result variables (frame 0x14, but 45.5-46.0%), src[i] / src++ / mem+i*stride per case (scripted,
   4 forms x 15 cases), /QIfist without asm (fistp qword + aligned frame, 16.2%), _ftol stand-in (26.4%).
   flagsweep (pre-asm): /G3 /G4 /G5 /GS+/G5 tie, game flags lower. */
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
MGLOBALA(int, DAT_005dae40, 17, 0x005dae40);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cd04c, 0x005cd04c);
MGLOBAL(float, DAT_005dae8c, 0x005dae8c);
MGLOBAL(float, DAT_005cfe34, 0x005cfe34);
MGLOBAL(float, DAT_005dae88, 0x005dae88);
MGLOBAL(float, DAT_005dae84, 0x005dae84);

typedef struct { float x, y, z, w; } RwV4d;


/* RenderWare float->int (RwInt32FromRealMacro form): fld x; fistp r */
static __inline int RealToInt(float x)
{
  int r;
  __asm fld x
  __asm fistp r
  return r;
}

// MATCH: 0x004f5030 FUN_004f5030
uint FUN_004f5030(int type, uchar *mem, const RwV4d *src, int numVerts, int stride)
{
  int i;
  int w0, w1, w2;
  switch (type) {
  case 0:
    for (i = 0; i < numVerts; i++) {
      *(float *)mem = src[i].x;
      mem += stride;
    }
    break;
  case 1:
    for (i = 0; i < numVerts; i++) {
      ((float *)mem)[0] = src[i].x;
      ((float *)mem)[1] = src[i].y;
      mem += stride;
    }
    break;
  case 2:
    for (i = 0; i < numVerts; i++) {
      *(RwV4d *)mem = (*src);
      src++;
      mem += stride;
    }
    break;
  case 3:
    for (i = 0; i < numVerts; i++) {
      ((float *)(mem + i * stride))[0] = (*src).x;
      ((float *)(mem + i * stride))[1] = (*src).y;
      ((float *)(mem + i * stride))[2] = (*src).z;
      ((float *)(mem + i * stride))[3] = (*src).w;
      src++;
    }
    break;
  case 4:
    for (i = 0; i < numVerts; i++) {
      if ((*src).x >= DAT_005cc320) {
        (mem + i * stride)[2] = 255;
        (mem + i * stride)[1] = 0;
        (mem + i * stride)[0] = 0;
        (mem + i * stride)[3] = 0;
      } else {
        (mem + i * stride)[1] = (uchar)RealToInt((*src).y * DAT_005cd04c);
        (mem + i * stride)[0] = (uchar)RealToInt((*src).z * DAT_005cd04c);
        (mem + i * stride)[3] = (uchar)RealToInt((*src).w * DAT_005cd04c);
        (mem + i * stride)[2] = (uchar)(255 - (mem + i * stride)[3] - (mem + i * stride)[1] - (mem + i * stride)[0]);
      }
      src++;
    }
    break;
  case 5:
    for (i = 0; i < numVerts; i++) {
      (mem + i * stride)[0] = (uchar)RealToInt((*src).x);
      (mem + i * stride)[1] = (uchar)RealToInt((*src).y);
      (mem + i * stride)[2] = (uchar)RealToInt((*src).z);
      (mem + i * stride)[3] = (uchar)RealToInt((*src).w);
      src++;
    }
    break;
  case 6:
    for (i = 0; i < numVerts; i++) {
      ((short *)mem)[0] = (short)RealToInt(src[i].x);
      ((short *)mem)[1] = (short)RealToInt(src[i].y);
      mem += stride;
    }
    break;
  case 7:
    for (i = 0; i < numVerts; i++) {
      ((short *)(mem + i * stride))[0] = (short)RealToInt((*src).x);
      ((short *)(mem + i * stride))[1] = (short)RealToInt((*src).y);
      ((short *)(mem + i * stride))[2] = (short)RealToInt((*src).z);
      ((short *)(mem + i * stride))[3] = (short)RealToInt((*src).w);
      src++;
    }
    break;
  case 8:
    for (i = 0; i < numVerts; i++) {
      if ((*src).x >= DAT_005cc320) {
        (mem + i * stride)[0] = 255;
        (mem + i * stride)[1] = 0;
        (mem + i * stride)[2] = 0;
        (mem + i * stride)[3] = 0;
      } else {
        (mem + i * stride)[1] = (uchar)RealToInt((*src).y * DAT_005cd04c);
        (mem + i * stride)[2] = (uchar)RealToInt((*src).z * DAT_005cd04c);
        (mem + i * stride)[3] = (uchar)RealToInt((*src).w * DAT_005cd04c);
        (mem + i * stride)[0] = (uchar)(255 - (mem + i * stride)[1] - (mem + i * stride)[2] - (mem + i * stride)[3]);
      }
      src++;
    }
    break;
  case 9:
    for (i = 0; i < numVerts; i++) {
      w0 = RealToInt((*src).x * DAT_005dae8c);
      ((short *)(mem + i * stride))[0] = (short)w0;
      ((short *)(mem + i * stride))[1] = (short)(32767 - w0);
      src++;
    }
    break;
  case 10:
    for (i = 0; i < numVerts; i++) {
      w0 = RealToInt((*src).x * DAT_005dae8c);
      w1 = RealToInt((*src).y * DAT_005dae8c);
      w2 = RealToInt((*src).z * DAT_005dae8c);
      ((short *)(mem + i * stride))[1] = (short)w0;
      ((short *)(mem + i * stride))[2] = (short)w1;
      ((short *)(mem + i * stride))[3] = (short)w2;
      ((short *)(mem + i * stride))[0] = (short)(32767 - w2 - w1 - w0);
      src++;
    }
    break;
  case 11:
    for (i = 0; i < numVerts; i++) {
      w0 = RealToInt((*src).x * DAT_005cfe34);
      ((ushort *)(mem + i * stride))[0] = (ushort)w0;
      ((ushort *)(mem + i * stride))[1] = (ushort)(65535 - w0);
      src++;
    }
    break;
  case 12:
    for (i = 0; i < numVerts; i++) {
      w0 = RealToInt((*src).x * DAT_005cfe34);
      w1 = RealToInt((*src).y * DAT_005cfe34);
      w2 = RealToInt((*src).z * DAT_005cfe34);
      ((ushort *)(mem + i * stride))[1] = (ushort)w0;
      ((ushort *)(mem + i * stride))[2] = (ushort)w1;
      ((ushort *)(mem + i * stride))[3] = (ushort)w2;
      ((ushort *)(mem + i * stride))[0] = (ushort)(65535 - w2 - w1 - w0);
      src++;
    }
    break;
  case 13:
    for (i = 0; i < numVerts; i++) {
      w2 = RealToInt((*src).z);
      w0 = RealToInt((*src).x);
      w1 = RealToInt((*src).y);
      *(uint *)(mem + i * stride) = (((w2 & 0x3ff) << 10 | (w1 & 0x3ff)) << 10) | (w0 & 0x3ff);
      src++;
    }
    break;
  case 14:
    for (i = 0; i < numVerts; i++) {
      if (src[i].z < DAT_005dae88) {
        w0 = RealToInt(src[i].x * DAT_005dae84) & 0x3ff;
        *(uint *)mem = ((511 - w0) << 10) | w0;
      } else {
        w0 = RealToInt(src[i].y * DAT_005dae84) & 0x3ff;
        w1 = RealToInt(src[i].z * DAT_005dae84) & 0x3ff;
        *(uint *)mem = (((w1 << 10) | w0) << 10) | (511 - w1 - w0);
      }
      mem += stride;
    }
    break;
  }
  return DAT_005dae40[type];
}
