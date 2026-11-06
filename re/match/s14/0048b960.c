// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x0048b960 (h7): 100% (whole symbol byte-identical incl. the tail jmp to 0x0048b7c0). Raw 23.8%. What moved it: 0xa4-byte record array at 0x007201d0 typed (pos vec3, two timers, count/max, accum, frame, atomic[10], rate[10], alpha[10]); per-record pointer rec = &arr[i] with the 0x007220f0 slot pointer advanced in the for-increment; 0x005cc99c 0.3f, 0x005ce2f4 pi, 0x005cd228 0.35f, 0x005cc320 1.0f, 0x005cd04c 255.0f are compiler float literals; FUN_004a2c48 is _ftol (the (int) cast); position copied as a vec3 struct; colour as a 4-byte RGBA struct copy; rec->b = dt + rec->b then rec->a = rec->b (one temporary gives fst/fst instead of fld st0/fxch). Residual: none. */
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
double sin(double);
#pragma intrinsic(sin)
typedef struct Vec3_0048b960 { float x, y, z; } Vec3_0048b960;
typedef struct Rgba_0048b960 { unsigned char r, g, b, a; } Rgba_0048b960;
typedef struct Rec007201d0 {
    Vec3_0048b960 pos;  /* +0x00 */
    float a;            /* +0x0c */
    float b;            /* +0x10 */
    int count;          /* +0x14 */
    int countMax;       /* +0x18 */
    float accum;        /* +0x1c */
    float f20;          /* +0x20 */
    int pad24;
    int frame;          /* +0x28 */
    int atomic[10];     /* +0x2c */
    float rate[10];     /* +0x54 */
    float alpha[10];    /* +0x7c */
} Rec007201d0;
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBALA(Rec007201d0, DAT_007201d0, 8, 0x007201d0);
MGLOBALA(int, DAT_007220f0, 8, 0x007220f0);
float FUN_00472650(float p0, float p1);
void FUN_0048b440(void);
void FUN_0048b650(Vec3_0048b960 *p0, float p1);
void FUN_0048b7c0(void);
int FUN_004b40f0(int p0);
int FUN_004c1340(int p0, Vec3_0048b960 *p1, int p2);
int FUN_00544bf0(int p0, float p1);

// MATCH: 0x0048b960 _FUN_0048b960
void FUN_0048b960(void)
{
    int *slot;
    int i, j;
    Rec007201d0 *rec;
    Rgba_0048b960 col;
    Vec3_0048b960 v;
    float t;
    float step;

    slot = DAT_007220f0;
    for (i = 0; i < 8; i++, slot++) {
        rec = &DAT_007201d0[i];
        if (*slot == 0) continue;
        if (rec->count < rec->countMax) {
            rec->accum = DAT_007f100c + rec->accum;
            if (rec->accum >= 0.3f) {
                t = FUN_00472650(0.0f, 0.2f) + rec->f20;
                rec->accum = 0.0f;
                rec->count++;
                FUN_0048b650(&rec->pos, t);
            }
        }
        v = rec->pos;
        rec->b = DAT_007f100c + rec->b;
        rec->a = rec->b;
        v.y = (float)sin(rec->a * 3.1415927f) * 0.35f + v.y;
        FUN_004c1340(rec->frame, &v, 0);
        if (rec->b >= 1.0f) *slot = 0;
        col.r = 0xff;
        col.g = 0xff;
        col.b = 0xff;
        for (j = 0; j < 10; j++) {
            step = DAT_007f100c * rec->rate[j];
            if (rec->alpha[j] > 1.0f) rec->alpha[j] = 1.0f;
            FUN_00544bf0(rec->atomic[j], rec->alpha[j]);
            rec->alpha[j] = step + rec->alpha[j];
            col.a = 0xff - (unsigned char)(int)(rec->a * 255.0f);
            *(Rgba_0048b960 *)(FUN_004b40f0(rec->atomic[j]) + 4) = col;
        }
    }
    FUN_0048b440();
    FUN_0048b7c0();
}
