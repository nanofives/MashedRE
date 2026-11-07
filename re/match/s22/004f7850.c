// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f7850 (h3, by hand): 27.2% exact, regs 89.7%, ops 96.3% under /O2 /Ob1 /G5. Raw converter 42.7%
   (regs 87.4%). Gamma/LUT copy for formats 4..7 (jump table), returns DAT_005dae40[format].
   Same instruction stream as the original except one register swap: the original keeps `format` in edx and the
   per-byte temporary in ebx, ours the other way round, which fails every in-place compare. Both void* params
   copied to byte-pointer locals give the original pointer biases (dst+2, src+1/+2). Copying only src scores
   51.5% exact (regs 75%) but with the wrong biases. Also 133 vs 136 ins: one fewer duplicated epilogue.
   Tried: loop forms, unsigned format, default case, per-case return, typed lut, permuter (276 compiles).
   DAT_005dae40 element count 8 covers formats 0..7 (size not read from the binary). */
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
MGLOBALA(int, DAT_005dae40, 8, 0x005dae40);
// MATCH: 0x004f7850 FUN_004f7850
int FUN_004f7850(int format, void *dest, const void *source, const unsigned char *lut,
                 int count, int stride)
{
  int i;
  unsigned char *dst = (unsigned char *)dest;
  const unsigned char *src = (const unsigned char *)source;

  switch (format) {
  case 4:
    for (i = 0; i < count; i++) {
      dst[0] = lut[src[2]];
      dst[1] = lut[src[1]];
      dst[2] = lut[src[0]];
      dst[3] = lut[src[3]];
      dst += stride;
      src += 4;
    }
    return DAT_005dae40[format];
  case 5:
    for (i = 0; i < count; i++) {
      dst[0] = lut[src[0]];
      dst[1] = lut[src[1]];
      dst[2] = lut[src[2]];
      dst[3] = lut[src[3]];
      dst += stride;
      src += 4;
    }
    return DAT_005dae40[format];
  case 6:
    for (i = 0; i < count; i++) {
      ((unsigned short *)dst)[0] = lut[src[0]];
      ((unsigned short *)dst)[1] = lut[src[1]];
      dst += stride;
      src += 4;
    }
    return DAT_005dae40[format];
  case 7:
    for (i = 0; i < count; i++) {
      ((unsigned short *)dst)[0] = lut[src[0]];
      ((unsigned short *)dst)[1] = lut[src[1]];
      ((unsigned short *)dst)[2] = lut[src[2]];
      ((unsigned short *)dst)[3] = lut[src[3]];
      dst += stride;
      src += 4;
    }
    return DAT_005dae40[format];
  }
  return DAT_005dae40[format];
}
