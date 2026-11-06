// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x0048b7c0 (h7):  */
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
typedef struct Vec3_0048b7c0 { float x, y, z; } Vec3_0048b7c0;
typedef struct Rgba_0048b7c0 { unsigned char r, g, b, a; } Rgba_0048b7c0;
typedef struct Mtx_0048b7c0 {
    Vec3_0048b7c0 right; unsigned int flags;
    Vec3_0048b7c0 up; unsigned int pad1;
    Vec3_0048b7c0 at; unsigned int pad2;
    Vec3_0048b7c0 pos; unsigned int pad3;
} Mtx_0048b7c0;
typedef struct Pool0071fa40 {
    Vec3_0048b7c0 pos;  /* +0x00 */
    int f0c;            /* +0x0c */
    float age;          /* +0x10 */
    float scale;        /* +0x14 */
} Pool0071fa40;
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBAL(Vec3_0048b7c0, DAT_006146f0, 0x006146f0);
MGLOBALA(Pool0071fa40, DAT_0071fa40, 0x40, 0x0071fa40);
MGLOBALA(Pool0071fa40, DAT_0071fa50, 0x40, 0x0071fa50);
MGLOBALA(int, DAT_007200d0, 0x40, 0x007200d0);
MGLOBAL(char, DAT_00720040, 0x00720040);
void FUN_004769f0(Rgba_0048b7c0 *p0);
void FUN_00476a10(Mtx_0048b7c0 *p0);
void FUN_00476d00(void *p0);
void FUN_00476df0(void *p0);
Mtx_0048b7c0 *FUN_004c4d20(Mtx_0048b7c0 *p0, Vec3_0048b7c0 *p1, float p2, int p3);
Mtx_0048b7c0 *FUN_004c5010(Mtx_0048b7c0 *p0, Vec3_0048b7c0 *p1, int p2);
Mtx_0048b7c0 *FUN_004c51a0(Mtx_0048b7c0 *p0, Vec3_0048b7c0 *p1, int p2);

// MATCH: 0x0048b7c0 _FUN_0048b7c0
void FUN_0048b7c0(void)
{
    float t;
    Rgba_0048b7c0 col;
    Vec3_0048b7c0 s;
    Mtx_0048b7c0 m;
    Pool0071fa40 *p;
    int i;

    col.r = 200;
    col.g = 200;
    col.b = 200;
    col.a = 200;
    for (i = 0; i < 0x40; i++) {
        p = &DAT_0071fa40[i];
        if (DAT_007200d0[i] == 0) continue;
        s.x = 1.0f;
        s.y = 1.0f;
        s.z = 1.0f;
        DAT_0071fa50[i].pos.x = t = DAT_007f100c + DAT_0071fa50[i].pos.x;
        p->f0c = *(int *)&t;
        if (t > 1.0f) {
            DAT_007200d0[i] = 0;
            continue;
        }
        s.x = s.y = (float)sin(t * 2.3561945f) * p->scale;
        col.a = 0x40 - (unsigned char)(int)(t * t * 64.0f);
        m.right.x = m.up.y = m.at.z = 1.0f;
        m.right.y = m.right.z = m.up.x = 0.0f;
        m.up.z = m.at.x = m.at.y = 0.0f;
        m.pos.x = m.pos.y = m.pos.z = 0.0f;
        m.flags |= 0x20003;
        FUN_004c51a0(&m, &p->pos, 0);
        FUN_004c4d20(&m, &DAT_006146f0, 90.0f, 1);
        FUN_004c5010(&m, &s, 1);
        FUN_00476a10(&m);
        FUN_004769f0(&col);
        FUN_00476d00(&DAT_00720040);
    }
    FUN_00476df0(&DAT_00720040);
}
