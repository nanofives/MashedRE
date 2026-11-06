// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x0048f9f0 (h3): 100% (regs 100%), relocheck OK (unchecked: __ftol). Raw 31.2%.
   10-entry 0x24-byte particle array at 0x00769f50. What moved it: struct array + indexed loop, kill branch
   first, the UV row copied field by field from a frame index (a struct copy adds base+index), the 0xff colour
   stores written after the alpha store in b,g,r order. No residual. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef struct Vec3_48f9 { float x, y, z; } Vec3_48f9;
typedef struct Col_48f9 { unsigned char r, g, b, a; } Col_48f9;
typedef struct Uv_48f9 { float u0, v0, u1, v1; } Uv_48f9;
typedef struct Part48f9 {
    int active;        /* +0x00 */
    Vec3_48f9 pos;     /* +0x04 */
    float a;           /* +0x10 */
    float b;           /* +0x14 */
    float alpha;       /* +0x18 */
    int frame;         /* +0x1c */
    float timer;       /* +0x20 */
} Part48f9;
MGLOBALA(Part48f9, DAT_00769f50, 10, 0x00769f50);
MGLOBALA(Uv_48f9, DAT_006145b0, 16, 0x006145b0);
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBAL(float, DAT_005ce18c, 0x005ce18c);
MGLOBAL(float, DAT_005cc558, 0x005cc558);
MGLOBAL(float, DAT_005cc750, 0x005cc750);
MGLOBAL(char, DAT_0076a0b8, 0x0076a0b8);
void FUN_00476a40(Uv_48f9 *p);
void FUN_004769a0(Vec3_48f9 *p);
void FUN_004769d0(float *p);
void FUN_004769f0(Col_48f9 *p);
void FUN_00476d00(char *t);
void FUN_00476df0(char *t);
// MATCH: 0x0048f9f0 FUN_0048f9f0
void FUN_0048f9f0(void)
{
    int i;
    float x;
    int fr;
    int t;
    Col_48f9 c;
    Uv_48f9 uv;
    for (i = 0; i < 10; i++) {
        Part48f9 *p = &DAT_00769f50[i];
        if (p->active) {
            p->timer = DAT_007f100c + p->timer;
            if (p->timer > DAT_005ce18c && p->frame < 15) {
                p->frame++;
                p->timer = 0.0f;
            }
            x = p->alpha - DAT_005cc558;
            if (x < DAT_005cc750) {
                p->active = 0;
            } else {
                t = (unsigned char)x;
                p->alpha = (float)t;
                c.b = 0xff;
                c.g = 0xff;
                c.r = 0xff;
                c.a = (unsigned char)p->alpha;
                fr = p->frame;
                uv.u0 = DAT_006145b0[fr].u0;
                uv.v0 = DAT_006145b0[fr].v0;
                uv.u1 = DAT_006145b0[fr].u1;
                uv.v1 = DAT_006145b0[fr].v1;
                FUN_00476a40(&uv);
                FUN_004769f0(&c);
                FUN_004769a0(&p->pos);
                FUN_004769d0(&p->a);
                FUN_00476d00(&DAT_0076a0b8);
            }
        }
    }
    FUN_00476df0(&DAT_0076a0b8);
}
