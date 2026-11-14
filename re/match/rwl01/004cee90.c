// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
// MATCH-RWT-S7: 100% byte-identical under rw9044 (L-131), relocheck OK. Levers: stream kept in a register and the header word 0
// passed through an __inline create helper (c2 9044 keeps it in the dead parameter slot across the alloc: mov [esp+0x38],eax at
// 0x004ceed8, read back at 0x004ceeff; words 1/2 in ebp/edi at 0x004ceed4/0x004ceed0), the helper's NULL return gives the
// xor esi,esi at 0x004ceefb, and the row counter step written as the for-step 'y++, off += stride' (inc eax at 0x004cef9b before
// add ebp,edi at 0x004cef9c).
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
typedef unsigned int uint;
typedef unsigned char byte;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d4628, 0x007d4628);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_006182b4, 0x006182b4);
uint FUN_004cbd30(int p0, int p1, uint p2);
undefined4 * FUN_004cc050(int p0, int p1);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, undefined4 * p2, uint * p3);
uint * FUN_004cdd60(uint * p0);
int FUN_004d8000(int p0, undefined4 * p1);
// MATCH: 0x004cee90 FUN_004cee90
static __inline int *ImageCreateInl(int width, int height, int depth)
{
  int *image;
  image = (int *)(**(code **)(DAT_007d3ff8 + 0x118))(*(undefined4 *)(DAT_007d4628 + DAT_007d3ff8), 0x30018);
  if (!image) {
    return 0;
  }
  image[1] = width;
  image[2] = height;
  image[3] = depth;
  image[5] = 0;
  image[6] = 0;
  image[0] = 0;
  FUN_004d8000((int)&DAT_006182b4, (undefined4 *)image);
  return image;
}
undefined4 * FUN_004cee90(int stream)
{
  undefined4 size;
  uint version;
  int hdr[4];
  int *image;
  int off;
  int y;
  int stride;

  if (FUN_004cc5e0(stream, 1, &size, &version) == 0) {
    return 0;
  }
  FUN_004cbd30(stream, (int)hdr, 0x10);
  image = ImageCreateInl(hdr[0], hdr[1], hdr[2]);
  FUN_004cdd60((uint *)image);
  stride = image[4];
  if (stride == hdr[3]) {
    FUN_004cbd30(stream, image[5], image[2] * stride);
  }
  else {
    off = 0;
    for (y = 0; y < hdr[1]; y++, off += stride) {
      if (stride > hdr[3]) {
        FUN_004cbd30(stream, image[5] + off, hdr[3]);
      }
      else {
        FUN_004cbd30(stream, image[5] + off, stride);
        FUN_004cc050(stream, hdr[3] - stride);
      }
    }
  }
  if (image[3] <= 8) {
    FUN_004cbd30(stream, image[6], (1 << image[3]) << 2);
  }
  return (undefined4 *)image;
}