// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 h4 0x004f57e0 (1700 B): best 14.2% (regs 45.2%, ops 68.8%) under /O2 /Ob1 /G5.
   Same family as 0x004f5030 with 3-float (12 B) source rows: 15-case switch on the D3DDECLTYPE index,
   case 2 is `stride == 12 ? memcpy(mem, src, numVerts * 12) : per-row RwV3d copy` (matches, rep movsd/movsb),
   case 3 writes 1.0f as w, packed types write the constant 4th lane (0xff / 1 / 0x7fff / 0xffff).
   Residual class (c): every conversion in cases 4..14 is an inlined __asm float->int
   (`fstp [tmp]; fld [tmp]; fistp dword [r]`), no C form under 9176 (see 0x004f5030). The original keeps a
   4-byte frame (push ecx) and the type in ebp because the asm results land in dead parameter slots; the
   stand-in's _ftol calls need a 16-byte frame, which shifts every esp offset (hence exact% far below ops%).
   Tried: src[i] / src++ / mem+i*stride per case (scripted, 4 forms x 15 cases). flagsweep: /G3 /G4 /G5 and
   /GS+/G5 all 14.2, game flags /O2 /GS /Ob1 21.9 (no /G5: frame offsets line up differently, not a better
   source; RW is /G5 per the flags brief), so line 1 stays /O2 /Ob1 /G5. */
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
MGLOBAL(float, DAT_005cd04c, 0x005cd04c);
MGLOBAL(float, DAT_005dae8c, 0x005dae8c);
MGLOBAL(float, DAT_005cfe34, 0x005cfe34);
MGLOBAL(float, DAT_005dae84, 0x005dae84);

typedef struct { float x, y, z; } RwV3d;

#ifdef __cplusplus
extern "C"
#endif
void *__cdecl memcpy(void *dst, const void *src, unsigned int n);
#pragma intrinsic(memcpy)

/* stand-in for RenderWare's inline-asm float->int (fld x; fistp r): no C form emits fistp dword under 9176 */
static __inline int RealToInt(float x) { return (int)*(volatile float *)&x; }

// MATCH: 0x004f57e0 FUN_004f57e0
uint FUN_004f57e0(int type, uchar *mem, const RwV3d *src, int numVerts, int stride)
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
    if (stride == 12) {
      memcpy(mem, src, numVerts * 12);
    } else {
      for (i = 0; i < numVerts; i++) {
        *(RwV3d *)(mem + i * stride) = (*src);
        src++;
      }
    }
    break;
  case 3:
    for (i = 0; i < numVerts; i++) {
      ((float *)(mem + i * stride))[0] = (*src).x;
      ((float *)(mem + i * stride))[1] = (*src).y;
      ((float *)(mem + i * stride))[2] = (*src).z;
      ((float *)(mem + i * stride))[3] = 1.0f;
      src++;
    }
    break;
  case 4:
    for (i = 0; i < numVerts; i++) {
      (mem + i * stride)[2] = (uchar)RealToInt((*src).x * DAT_005cd04c);
      (mem + i * stride)[1] = (uchar)RealToInt((*src).y * DAT_005cd04c);
      (mem + i * stride)[0] = (uchar)RealToInt((*src).z * DAT_005cd04c);
      (mem + i * stride)[3] = 255;
      src++;
    }
    break;
  case 5:
    for (i = 0; i < numVerts; i++) {
      (mem + i * stride)[0] = (uchar)RealToInt((*src).x);
      (mem + i * stride)[1] = (uchar)RealToInt((*src).y);
      (mem + i * stride)[2] = (uchar)RealToInt((*src).z);
      (mem + i * stride)[3] = 1;
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
      ((short *)(mem + i * stride))[3] = 1;
      src++;
    }
    break;
  case 8:
    for (i = 0; i < numVerts; i++) {
      (mem + i * stride)[0] = (uchar)RealToInt((*src).x * DAT_005cd04c);
      (mem + i * stride)[1] = (uchar)RealToInt((*src).y * DAT_005cd04c);
      (mem + i * stride)[2] = (uchar)RealToInt((*src).z * DAT_005cd04c);
      (mem + i * stride)[3] = 255;
      src++;
    }
    break;
  case 9:
    for (i = 0; i < numVerts; i++) {
      ((short *)mem)[0] = (short)RealToInt(src[i].x * DAT_005dae8c);
      ((short *)mem)[1] = (short)RealToInt(src[i].y * DAT_005dae8c);
      mem += stride;
    }
    break;
  case 10:
    for (i = 0; i < numVerts; i++) {
      ((short *)(mem + i * stride))[0] = (short)RealToInt((*src).x * DAT_005dae8c);
      ((short *)(mem + i * stride))[1] = (short)RealToInt((*src).y * DAT_005dae8c);
      ((short *)(mem + i * stride))[2] = (short)RealToInt((*src).z * DAT_005dae8c);
      ((short *)(mem + i * stride))[3] = 32767;
      src++;
    }
    break;
  case 11:
    for (i = 0; i < numVerts; i++) {
      ((ushort *)mem)[0] = (ushort)RealToInt(src[i].x * DAT_005cfe34);
      ((ushort *)mem)[1] = (ushort)RealToInt(src[i].y * DAT_005cfe34);
      mem += stride;
    }
    break;
  case 12:
    for (i = 0; i < numVerts; i++) {
      ((ushort *)(mem + i * stride))[0] = (ushort)RealToInt((*src).x * DAT_005cfe34);
      ((ushort *)(mem + i * stride))[1] = (ushort)RealToInt((*src).y * DAT_005cfe34);
      ((ushort *)(mem + i * stride))[2] = (ushort)RealToInt((*src).z * DAT_005cfe34);
      ((ushort *)(mem + i * stride))[3] = 65535;
      src++;
    }
    break;
  case 13:
    for (i = 0; i < numVerts; i++) {
      w0 = RealToInt(src[i].x);
      w1 = RealToInt(src[i].y);
      w2 = RealToInt(src[i].z);
      *(uint *)mem = (((w2 & 0x3ff) << 10 | (w1 & 0x3ff)) << 10) | (w0 & 0x3ff);
      mem += stride;
    }
    break;
  case 14:
    for (i = 0; i < numVerts; i++) {
      w0 = RealToInt(src[i].x * DAT_005dae84);
      w1 = RealToInt(src[i].y * DAT_005dae84);
      w2 = RealToInt(src[i].z * DAT_005dae84);
      *(uint *)mem = (((w2 & 0x3ff) << 10 | (w1 & 0x3ff)) << 10) | (w0 & 0x3ff);
      mem += stride;
    }
    break;
  }
  return DAT_005dae40[type];
}
