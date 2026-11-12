// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f6870 (h5): 23.9% (regs 37.6%, ops 95.0%) under /O2 /Ob1 /G5.
   Shape: same family as 0x004f71f0: 15-entry switch, each case lerps two RwV3d-sized source streams a, b by t
   ((b - a) * t + a, fld b; fsub a; fmul t; fadd a) or, for the normalised integer formats 4, 8..12, 14, precomputes
   inv = (1.0f - t) * K into the one frame local ([esp+0x10]) and t *= K, then inv * a + t * b (first component of
   cases 11, 12, 14: t * b + inv * a, as in the original). K literals 255.0f, 32767.0f, 65535.0f, 511.0f (original
   .rdata 0x005cd04c, 0x005dae8c, 0x005cfe34, 0x005dae84; 1.0f at 0x005cc320). Returns DAT_005dae40[format].
   Float->int is RwInt32FromRealMacro, an __asm fld/fistp block inside the C body. Loops: for (; count > 0; count--).
   Operation stream is 95% identical (716 vs 724 instructions). C casts instead (call __ftol) scored 36.0% exact but
   59.7% ops: the higher exact% only comes from esp offsets lining up, the code itself is further away.
   Residual class (a): the original keeps tmp, x, y, z in dead parameter home slots (tmp [esp+0x1c] = dst slot,
   x/y/z in the count, stride and b slots) with a 4-byte frame for inv only; 9176 allocates all five in a
   sub esp,0x14 frame, so every esp-relative operand differs. Also: the original biases the dst pointer
   (lea eax,[edx+1] in case 4, +8 in cases 2/3), 9176 does not, for every store-order and pointer-type form tried.
   Tried: do/while vs for loops, globals for K (23.6%), struct vs float* stores, t * K vs K * t vs t *= K. Permuter not
   run on the asm form (residual is slot allocation, not statement order). */

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
typedef struct { float x, y, z; } V3d;
MGLOBALA(int, DAT_005dae40, 15, 0x005dae40);
// MATCH: 0x004f6870 FUN_004f6870
int FUN_004f6870(int format, unsigned char *dst, const V3d *a, const V3d *b, float t, int count, int stride)
{
    float inv;
    int x, y, z;
    float tmp;

    switch (format) {
    case 0:
        for (; count > 0; count--) {
            ((float *)dst)[0] = (b->x - a->x) * t + a->x;
            a++;
            b++;
            dst += stride;
        }
        break;
    case 1:
        for (; count > 0; count--) {
            ((float *)dst)[0] = (b->x - a->x) * t + a->x;
            ((float *)dst)[1] = (b->y - a->y) * t + a->y;
            a++;
            b++;
            dst += stride;
        }
        break;
    case 2:
        for (; count > 0; count--) {
            ((float *)dst)[0] = (b->x - a->x) * t + a->x;
            ((float *)dst)[1] = (b->y - a->y) * t + a->y;
            ((float *)dst)[2] = (b->z - a->z) * t + a->z;
            a++;
            b++;
            dst += stride;
        }
        break;
    case 3:
        for (; count > 0; count--) {
            ((float *)dst)[0] = (b->x - a->x) * t + a->x;
            ((float *)dst)[1] = (b->y - a->y) * t + a->y;
            ((float *)dst)[3] = 1.0f;
            ((float *)dst)[2] = (b->z - a->z) * t + a->z;
            a++;
            b++;
            dst += stride;
        }
        break;
    case 4:
        inv = (1.0f - t) * 255.0f;
        t = t * 255.0f;
        for (; count > 0; count--) {
            tmp = inv * a->x + t * b->x;
            RwInt32FromRealMacro(x, tmp);
            ((unsigned char *)dst)[2] = (unsigned char)x;
            tmp = inv * a->y + t * b->y;
            RwInt32FromRealMacro(y, tmp);
            ((unsigned char *)dst)[1] = (unsigned char)y;
            tmp = inv * a->z + t * b->z;
            RwInt32FromRealMacro(z, tmp);
            ((unsigned char *)dst)[3] = 0xff;
            ((unsigned char *)dst)[0] = (unsigned char)z;
            a++;
            b++;
            dst += stride;
        }
        break;
    case 5:
        for (; count > 0; count--) {
            tmp = (b->x - a->x) * t + a->x;
            RwInt32FromRealMacro(x, tmp);
            ((unsigned char *)dst)[0] = (unsigned char)x;
            tmp = (b->y - a->y) * t + a->y;
            RwInt32FromRealMacro(y, tmp);
            ((unsigned char *)dst)[1] = (unsigned char)y;
            tmp = (b->z - a->z) * t + a->z;
            RwInt32FromRealMacro(z, tmp);
            ((unsigned char *)dst)[3] = 1;
            ((unsigned char *)dst)[2] = (unsigned char)z;
            a++;
            b++;
            dst += stride;
        }
        break;
    case 6:
        for (; count > 0; count--) {
            tmp = (b->x - a->x) * t + a->x;
            RwInt32FromRealMacro(x, tmp);
            ((short *)dst)[0] = (short)x;
            tmp = (b->y - a->y) * t + a->y;
            RwInt32FromRealMacro(y, tmp);
            ((short *)dst)[1] = (short)y;
            a++;
            b++;
            dst += stride;
        }
        break;
    case 7:
        for (; count > 0; count--) {
            tmp = (b->x - a->x) * t + a->x;
            RwInt32FromRealMacro(x, tmp);
            ((short *)dst)[0] = (short)x;
            tmp = (b->y - a->y) * t + a->y;
            RwInt32FromRealMacro(y, tmp);
            ((short *)dst)[1] = (short)y;
            tmp = (b->z - a->z) * t + a->z;
            RwInt32FromRealMacro(z, tmp);
            ((short *)dst)[3] = 1;
            ((short *)dst)[2] = (short)z;
            a++;
            b++;
            dst += stride;
        }
        break;
    case 8:
        inv = (1.0f - t) * 255.0f;
        t = t * 255.0f;
        for (; count > 0; count--) {
            tmp = inv * a->x + t * b->x;
            RwInt32FromRealMacro(x, tmp);
            ((unsigned char *)dst)[0] = (unsigned char)x;
            tmp = inv * a->y + t * b->y;
            RwInt32FromRealMacro(y, tmp);
            ((unsigned char *)dst)[1] = (unsigned char)y;
            tmp = inv * a->z + t * b->z;
            RwInt32FromRealMacro(z, tmp);
            ((unsigned char *)dst)[3] = 0xff;
            ((unsigned char *)dst)[2] = (unsigned char)z;
            a++;
            b++;
            dst += stride;
        }
        break;
    case 9:
        inv = (1.0f - t) * 32767.0f;
        t = t * 32767.0f;
        for (; count > 0; count--) {
            tmp = inv * a->x + t * b->x;
            RwInt32FromRealMacro(x, tmp);
            ((short *)dst)[0] = (short)x;
            tmp = inv * a->y + t * b->y;
            RwInt32FromRealMacro(y, tmp);
            ((short *)dst)[1] = (short)y;
            a++;
            b++;
            dst += stride;
        }
        break;
    case 10:
        inv = (1.0f - t) * 32767.0f;
        t = t * 32767.0f;
        for (; count > 0; count--) {
            tmp = inv * a->x + t * b->x;
            RwInt32FromRealMacro(x, tmp);
            ((short *)dst)[0] = (short)x;
            tmp = inv * a->y + t * b->y;
            RwInt32FromRealMacro(y, tmp);
            ((short *)dst)[1] = (short)y;
            tmp = inv * a->z + t * b->z;
            RwInt32FromRealMacro(z, tmp);
            ((short *)dst)[3] = 0x7fff;
            ((short *)dst)[2] = (short)z;
            a++;
            b++;
            dst += stride;
        }
        break;
    case 11:
        inv = (1.0f - t) * 65535.0f;
        t = t * 65535.0f;
        for (; count > 0; count--) {
            tmp = t * b->x + inv * a->x;
            RwInt32FromRealMacro(x, tmp);
            ((unsigned short *)dst)[0] = (unsigned short)x;
            tmp = inv * a->y + t * b->y;
            RwInt32FromRealMacro(y, tmp);
            ((unsigned short *)dst)[1] = (unsigned short)y;
            a++;
            b++;
            dst += stride;
        }
        break;
    case 12:
        inv = (1.0f - t) * 65535.0f;
        t = t * 65535.0f;
        for (; count > 0; count--) {
            tmp = t * b->x + inv * a->x;
            RwInt32FromRealMacro(x, tmp);
            ((unsigned short *)dst)[0] = (unsigned short)x;
            tmp = inv * a->y + t * b->y;
            RwInt32FromRealMacro(y, tmp);
            ((unsigned short *)dst)[1] = (unsigned short)y;
            tmp = inv * a->z + t * b->z;
            RwInt32FromRealMacro(z, tmp);
            ((unsigned short *)dst)[3] = 0xffff;
            ((unsigned short *)dst)[2] = (unsigned short)z;
            a++;
            b++;
            dst += stride;
        }
        break;
    case 13:
        for (; count > 0; count--) {
            tmp = (b->x - a->x) * t + a->x;
            RwInt32FromRealMacro(x, tmp);
            tmp = (b->y - a->y) * t + a->y;
            RwInt32FromRealMacro(y, tmp);
            tmp = (b->z - a->z) * t + a->z;
            RwInt32FromRealMacro(z, tmp);
            *(unsigned int *)dst = ((((z & 0x3ff) << 10) | (y & 0x3ff)) << 10) | (x & 0x3ff);
            a++;
            b++;
            dst += stride;
        }
        break;
    case 14:
        inv = (1.0f - t) * 511.0f;
        t = t * 511.0f;
        for (; count > 0; count--) {
            tmp = t * b->x + inv * a->x;
            RwInt32FromRealMacro(x, tmp);
            tmp = inv * a->y + t * b->y;
            RwInt32FromRealMacro(y, tmp);
            tmp = inv * a->z + t * b->z;
            RwInt32FromRealMacro(z, tmp);
            *(unsigned int *)dst = ((((z & 0x3ff) << 10) | (y & 0x3ff)) << 10) | (x & 0x3ff);
            a++;
            b++;
            dst += stride;
        }
        break;
    }
    return DAT_005dae40[format];
}
