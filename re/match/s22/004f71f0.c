// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f71f0 (h5): 34.7% (regs 43.7%, ops 75.4%) under /O2 /Ob1 /G5.
   Shape: 15-entry switch on the format, each case a counted loop from (u,v) float pairs into a vertex stream with a
   byte stride, returns DAT_005dae40[format] (element size table 4,8,12,16,4,...). Case 1 with stride 8 is
   memcpy(dst, src, count * 8) (intrinsic: lea ecx,[eax+eax]; rep movsd). Loops: for (; count > 0; count--)
   (test/jle, 0 stores stay immediates, no unroll; for (i = 0; i < count; i++) unrolls by 8). Scale constants are
   literals (255.0f, 32767.0f, 65535.0f, 511.0f, the original's .rdata 0x005cd04c, 0x005dae8c, 0x005cfe34,
   0x005dae84): a literal gives fld [src]; fmul [const] like the original, a global swaps the operands.
   Float->int is RenderWare's RwInt32FromRealMacro, an __asm fld/fistp block inside the C body (fstp tmp / fld tmp /
   fistp int, 0x004f733d..0x004f7349 and every conversion case). C casts instead (call __ftol) scored 9.6%.
   Residual class (a): the original keeps the asm temporaries (tmp, a, b) in the dead parameter home slots
   ([esp+0x18..0x24]) and so has no frame and keeps format in ebp for every return; 9176 gives them a sub esp,0xc
   frame, which adds add esp per exit, reloads format from the stack and shifts every esp offset by 0xc (487 vs 457
   instructions, the 30 extra are those). Tried: do/while vs for loop forms, block- vs function-scope temps, C casts,
   /QIfist, /Op, /G6; permuter 2676 compiles (+1.8 points from statement swaps, the only semantics-preserving one
   kept: dst += stride before src++ in case 1). */
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
#define RwInt32FromRealMacro(ANSWER, INREAL) __asm { fld INREAL } __asm { fistp ANSWER }
typedef struct { float u, v; } TexCoords;
MGLOBALA(int, DAT_005dae40, 15, 0x005dae40);
void *memcpy(void *dst, const void *src, unsigned int n);
#pragma intrinsic(memcpy)
// MATCH: 0x004f71f0 FUN_004f71f0
int FUN_004f71f0(int format, unsigned char *dst, const TexCoords *src, int count, int stride)
{
    int a, b;
    float tmp;

    switch (format) {
    case 0:
        for (; count > 0; count--) {
            *(float *)dst = src->u;
            src++;
            dst += stride;
        }
        break;
    case 1:
        if (stride == 8) {
            memcpy(dst, src, count * 8);
            break;
        }
        for (; count > 0; count--) {
            ((float *)dst)[0] = src->u;
            ((float *)dst)[1] = src->v;
            dst += stride;
            src++;
        }
        break;
    case 2:
        for (; count > 0; count--) {
            ((float *)dst)[0] = src->u;
            ((float *)dst)[1] = src->v;
            ((float *)dst)[2] = 0.0f;
            src++;
            dst += stride;
        }
        break;
    case 3:
        for (; count > 0; count--) {
            ((float *)dst)[0] = src->u;
            ((float *)dst)[1] = src->v;
            ((float *)dst)[2] = 0.0f;
            ((float *)dst)[3] = 1.0f;
            src++;
            dst += stride;
        }
        break;
    case 4:
        for (; count > 0; count--) {
            tmp = src->u * 255.0f;
            RwInt32FromRealMacro(a, tmp);
            dst[2] = (unsigned char)a;
            tmp = src->v * 255.0f;
            RwInt32FromRealMacro(b, tmp);
            dst[0] = 0;
            dst[1] = (unsigned char)b;
            dst[3] = 0xff;
            src++;
            dst += stride;
        }
        break;
    case 5:
        for (; count > 0; count--) {
            tmp = src->u;
            RwInt32FromRealMacro(a, tmp);
            dst[0] = (unsigned char)a;
            tmp = src->v;
            RwInt32FromRealMacro(b, tmp);
            dst[2] = 0;
            dst[1] = (unsigned char)b;
            dst[3] = 1;
            src++;
            dst += stride;
        }
        break;
    case 6:
        for (; count > 0; count--) {
            tmp = src->u;
            RwInt32FromRealMacro(a, tmp);
            ((short *)dst)[0] = (short)a;
            tmp = src->v;
            RwInt32FromRealMacro(b, tmp);
            ((short *)dst)[1] = (short)b;
            src++;
            dst += stride;
        }
        break;
    case 7:
        for (; count > 0; count--) {
            tmp = src->u;
            RwInt32FromRealMacro(a, tmp);
            ((short *)dst)[0] = (short)a;
            tmp = src->v;
            RwInt32FromRealMacro(b, tmp);
            ((short *)dst)[2] = 0;
            ((short *)dst)[1] = (short)b;
            ((short *)dst)[3] = 1;
            src++;
            dst += stride;
        }
        break;
    case 8:
        for (; count > 0; count--) {
            tmp = src->u * 255.0f;
            RwInt32FromRealMacro(a, tmp);
            dst[0] = (unsigned char)a;
            tmp = src->v * 255.0f;
            RwInt32FromRealMacro(b, tmp);
            dst[2] = 0;
            dst[1] = (unsigned char)b;
            dst[3] = 0xff;
            src++;
            dst += stride;
        }
        break;
    case 9:
        for (; count > 0; count--) {
            tmp = src->u * 32767.0f;
            RwInt32FromRealMacro(a, tmp);
            ((short *)dst)[0] = (short)a;
            tmp = src->v * 32767.0f;
            RwInt32FromRealMacro(b, tmp);
            ((short *)dst)[1] = (short)b;
            src++;
            dst += stride;
        }
        break;
    case 10:
        for (; count > 0; count--) {
            tmp = src->u * 32767.0f;
            RwInt32FromRealMacro(a, tmp);
            ((short *)dst)[0] = (short)a;
            tmp = src->v * 32767.0f;
            RwInt32FromRealMacro(b, tmp);
            ((short *)dst)[2] = 0;
            ((short *)dst)[1] = (short)b;
            ((short *)dst)[3] = 0x7fff;
            src++;
            dst += stride;
        }
        break;
    case 11:
        for (; count > 0; count--) {
            tmp = src->u * 65535.0f;
            RwInt32FromRealMacro(a, tmp);
            ((unsigned short *)dst)[0] = (unsigned short)a;
            tmp = src->v * 65535.0f;
            RwInt32FromRealMacro(b, tmp);
            ((unsigned short *)dst)[1] = (unsigned short)b;
            src++;
            dst += stride;
        }
        break;
    case 12:
        for (; count > 0; count--) {
            tmp = src->u * 65535.0f;
            RwInt32FromRealMacro(a, tmp);
            ((unsigned short *)dst)[0] = (unsigned short)a;
            tmp = src->v * 65535.0f;
            RwInt32FromRealMacro(b, tmp);
            ((unsigned short *)dst)[2] = 0;
            ((unsigned short *)dst)[1] = (unsigned short)b;
            ((unsigned short *)dst)[3] = 0xffff;
            src++;
            dst += stride;
        }
        break;
    case 13:
        for (; count > 0; count--) {
            tmp = src->u;
            RwInt32FromRealMacro(a, tmp);
            tmp = src->v;
            RwInt32FromRealMacro(b, tmp);
            *(unsigned int *)dst = ((b & 0x3ff) << 10) | (a & 0x3ff);
            src++;
            dst += stride;
        }
        break;
    case 14:
        for (; count > 0; count--) {
            tmp = src->u * 511.0f;
            RwInt32FromRealMacro(a, tmp);
            tmp = src->v * 511.0f;
            RwInt32FromRealMacro(b, tmp);
            *(unsigned int *)dst = ((b & 0x3ff) << 10) | (a & 0x3ff);
            src++;
            dst += stride;
        }
        break;
    }
    return DAT_005dae40[format];
}
