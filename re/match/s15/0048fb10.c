// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x0048fb10 (h3): 100% (regs 100%), relocheck OK (unchecked: __ftol). Raw 27.7%.
   256-entry 0x30-byte animated particle array at 0x00766f40. What moved it: struct array + indexed loop,
   kill branch first, `++tick == 1`, UV row copied field by field from a frame index, frame>11 block with
   follow==0 first and the frame=6 store before the colour copy. The induction pointer's bias (esi at +0
   vs +0x1c) followed the source references: caching `follow` in a local moved it to the fade field, reading
   p->follow at each use (CSE'd into edi anyway) kept it at +0. 0.25f is a literal. No residual. */
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
typedef struct Vec3_48fb { float x, y, z; } Vec3_48fb;
typedef struct Col_48fb { unsigned char r, g, b, a; } Col_48fb;
typedef struct Uv_48fb { float u0, v0, u1, v1; } Uv_48fb;
typedef struct Part48fb {
    int active;        /* +0x00 */
    Vec3_48fb pos;     /* +0x04 */
    float a;           /* +0x10 */
    float b;           /* +0x14 */
    Col_48fb col;      /* +0x18 */
    float fade;        /* +0x1c */
    int frame;         /* +0x20 */
    int tick;          /* +0x24 */
    int follow;        /* +0x28 */
    int handle;        /* +0x2c */
} Part48fb;
MGLOBALA(Part48fb, DAT_00766f40, 256, 0x00766f40);
MGLOBALA(Uv_48fb, DAT_006145b0, 12, 0x006145b0);
MGLOBAL(float, DAT_005cc750, 0x005cc750);
MGLOBAL(char, DAT_007669b8, 0x007669b8);
int FUN_0041f290(int a, int b);
float FUN_00472650(float lo, float hi);
void FUN_00476a40(Uv_48fb *p);
void FUN_004769a0(Vec3_48fb *p);
void FUN_004769d0(float *p);
void FUN_004769f0(Col_48fb *p);
void FUN_00476d00(char *t);
void FUN_00476df0(char *t);
// MATCH: 0x0048fb10 FUN_0048fb10
void FUN_0048fb10(void)
{
    int i;
    int ai;
    float x;
    int fr;
    unsigned char al;
    Col_48fb tint;
    Col_48fb c;
    Vec3_48fb pos;
    Uv_48fb uv;
    tint.r = 0x80;
    tint.g = 0x80;
    tint.b = 0x80;
    tint.a = 0x80;
    for (i = 0; i < 256; i++) {
        if (DAT_00766f40[i].active) {
            ai = DAT_00766f40[i].col.a;
            if (DAT_00766f40[i].follow == 0) {
                x = (float)ai - DAT_00766f40[i].fade * 0.25f;
            } else {
                x = (float)ai - DAT_00766f40[i].fade;
            }
            if (x < DAT_005cc750) {
                DAT_00766f40[i].active = 0;
            } else {
                al = (unsigned char)x;
                fr = DAT_00766f40[i].frame;
                uv.u0 = DAT_006145b0[fr].u0;
                uv.v0 = DAT_006145b0[fr].v0;
                uv.u1 = DAT_006145b0[fr].u1;
                uv.v1 = DAT_006145b0[fr].v1;
                DAT_00766f40[i].col.a = al;
                if (++DAT_00766f40[i].tick == 1) {
                    DAT_00766f40[i].frame++;
                    DAT_00766f40[i].tick = 0;
                }
                if (DAT_00766f40[i].frame > 11) {
                    if (DAT_00766f40[i].follow == 0) {
                        DAT_00766f40[i].frame = 6;
                        c = DAT_00766f40[i].col;
                    } else {
                        tint.a = al;
                        DAT_00766f40[i].frame = 0;
                        c = tint;
                    }
                } else {
                    c = DAT_00766f40[i].col;
                }
                if (DAT_00766f40[i].follow) {
                    pos = *(Vec3_48fb *)(FUN_0041f290(DAT_00766f40[i].handle, 0x33) + 0x30);
                    pos.x = pos.x + DAT_00766f40[i].pos.x;
                    pos.y = pos.y + DAT_00766f40[i].pos.y;
                    pos.z = pos.z + DAT_00766f40[i].pos.z;
                } else {
                    pos = DAT_00766f40[i].pos;
                    pos.x = FUN_00472650(-0.01f, 0.01f) + pos.x;
                    pos.z = FUN_00472650(-0.01f, 0.01f) + pos.z;
                }
                FUN_00476a40(&uv);
                FUN_004769f0(&c);
                FUN_004769a0(&pos);
                FUN_004769d0(&DAT_00766f40[i].a);
                FUN_00476d00(&DAT_007669b8);
            }
        }
    }
    FUN_00476df0(&DAT_007669b8);
}
