// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00457100: 97.7% (regs 97.7%, ops 96.8%), 309/309 ins (h5). Body from re/match/pilot/a2_00457100.c
   (its header has the full history: loop/continue shape, fields read back, FP reassociation notes).
   This file holds 0x004570a0 (MATCH line, 100%): its real body, static, EAX-passed entry pointer; 0x00457100 is
   its real caller (call at 0x00457495), so here no stand-in is needed (/Ob1 does not inline it).
   S09 changes: globals through MGLOBAL; relocheck found two swapped global pairs the score masked: 9176 emits the
   two global factors of x * G1 * G2 in reverse source order, so the source reads u * C074 * C194 and
   d2 * C9d0 * C4e8 to give the original's fmul [0x005ce194], fmul [0x005cd074] and fmul [0x005ce4e8],
   fmul [0x005cc9d0] (a VS2022 port build evaluates them in source order: x87 rounding may differ).
   Tried for the residual atS block (0x0045722e..0x00457291): an __inline scale helper with a float parameter
   gives the original operand order (fld [mc.at.x]; fmul [0x005ce4f0]) but no push interleave (94.8% at every
   statement position), permuter on it 96.4% (1412 variants); a local k = 0x005ce4f0 copy 92-94%; plain
   statements at 9 positions 94.2-97.4%. relocheck: 0x004570a0 OK; 0x00457100 BAD only at the three 0x005ce4f0
   sites inside the residual block (layout offset). Class (a)/(b) boundary, as the pilot found. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3;
typedef struct { float x, y; } V2;
typedef struct { unsigned char r, g, b, a; } RGBA;
typedef struct {
    V3 right; unsigned int flags;
    V3 up;    unsigned int pad1;
    V3 at;    unsigned int pad2;
    V3 pos;   unsigned int pad3;
} Mat;                                   /* 0x40 */
typedef struct {
    float t;                             /* +0x00 */
    float u;                             /* +0x04 */
    int pad08[3];
    float s;                             /* +0x14 */
    Mat m;                               /* +0x18 */
    int state;                           /* +0x58 */
    float timer;                         /* +0x5c */
    int flag;                            /* +0x60 */
    int pad64;
} PoolH;                                 /* 0x68, 64 entries at 0x00688810 */

MGLOBALA(PoolH, DAT_00688810, 64, 0x00688810);
MGLOBALA(unsigned char, DAT_0068a300, 8, 0x0068a300);
MGLOBALA(unsigned char, DAT_0068a210, 0xb0, 0x0068a210);
MGLOBALA(unsigned char, DAT_0068a2c0, 0x40, 0x0068a2c0);
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBAL(float, DAT_005ce4f8, 0x005ce4f8);   /* 0.0142857 */
MGLOBAL(float, DAT_005cc320, 0x005cc320);   /* 1.0 */
MGLOBAL(float, DAT_005cd0ec, 0x005cd0ec);   /* 0.005 */
MGLOBAL(float, DAT_005ce194, 0x005ce194);   /* 200.0 */
MGLOBAL(float, DAT_005cd074, 0x005cd074);   /* 1.25 */
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);   /* 0.5 */
MGLOBAL(float, DAT_005cd9c8, 0x005cd9c8);   /* 220.0 */
MGLOBAL(float, DAT_005ce4f4, 0x005ce4f4);   /* 35.0 */
MGLOBAL(float, DAT_005ce4f0, 0x005ce4f0);   /* -0.02 */
MGLOBAL(float, DAT_005ce4ec, 0x005ce4ec);   /* 81.0 */
MGLOBAL(float, DAT_005ce4e8, 0x005ce4e8);   /* 1/81 */
MGLOBAL(float, DAT_005cc9d0, 0x005cc9d0);   /* 128.0 */
MGLOBAL(float, DAT_005cc9b0, 0x005cc9b0);   /* 15.0 */
MGLOBAL(float, DAT_005cd0fc, 0x005cd0fc);   /* -0.1 */
MGLOBAL(float, DAT_005d757c, 0x005d757c);    /* 0.0 */
MGLOBAL(float, DAT_005cc950, 0x005cc950);   /* 0.75 */
MGLOBAL(float, DAT_005ce4e4, 0x005ce4e4);   /* 0.995 */

Mat *FUN_00467210(int a);
float FUN_00472650(float lo, float hi);
void FUN_004769f0(RGBA *c);
void FUN_00476a10(Mat *m);
void FUN_00476a80(V2 *p);
void FUN_00476d00(void *p);
void FUN_00476df0(void *p);
void FUN_004864f0(Mat *m, float f, int n);
void FUN_0048f780(V3 *p, float a, RGBA *c, float b, float d, int e, int f);
void FUN_004c3d90(V3 *out, V3 *in, int n, Mat *m);
void FUN_004c5010(Mat *m, V3 *s, int comb);
void FUN_004c51a0(Mat *m, V3 *t, int comb);
void FUN_004c4670(Mat *dst, Mat *src);
void FUN_00484cf0(void *p);
void FUN_00457010(void *q, void *hit);

typedef struct { V3 pos; float s; int kind; PoolH *e; int z; void (*cb)(void *q, void *hit); int pad[27]; } Req;

// MATCH: 0x004570a0 FUN_004570a0
static void FUN_004570a0(PoolH *e)
{
    Req r;

    r.pos = e->m.pos;
    r.s = e->s;
    r.e = e;
    r.z = 0;
    r.kind = 0x12;
    r.cb = FUN_00457010;
    FUN_00484cf0(&r);
}

// MATCH: 0x00457100 FUN_00457100
void FUN_00457100(void)
{
    int i;
    V3 verts[4];
    V3 v;
    RGBA c1, c2, c3;
    V3 atS;
    V2 pts[4];
    PoolH *e;
    Mat *cam;
    V3 scale;
    Mat mc;
    Mat tmp;
    float u, s, dx, dy, dz, d2, fx, fz;

    i = 0;

    while (i < 64) {
        e = &DAT_00688810[i];
        if (DAT_0068a300[i >> 3] & (1 << (i & 7))) {
            c1.r = 0x80;
            c1.g = 0x80;
            e->t = DAT_007f100c + e->t;
            e->u = e->t * DAT_005ce4f8;
            c1.b = 0x80;
            c1.a = 0xff;

            if (e->u > DAT_005cc320) {
                DAT_0068a300[i >> 3] &= ~(1 << (i & 7));
                continue;
            }
            if (e->u < DAT_005cd0ec)
                e->s = e->u * DAT_005cd074 * DAT_005ce194;
            else
                e->s = e->u * DAT_005cc32c + DAT_005cd074;

            scale.x = scale.y = scale.z = e->s;
            mc = e->m;
            FUN_004c5010(&mc, &scale, 1);
            c1.a = (unsigned char)((DAT_005cc320 - e->u) * DAT_005cd9c8 + DAT_005ce4f4);
            FUN_004769f0(&c1);
            FUN_00476a10(&mc);
            FUN_00476d00(DAT_0068a2c0);
            mc = e->m;
            c2.r = 0xff;
            c2.g = 0xff;
            atS.z = mc.at.z * DAT_005ce4f0;
            c2.b = 0xff;
            c2.a = 0x80;
            atS.y = DAT_005ce4f0 * mc.at.y;
            verts[0].x = 0.5f;  verts[0].y = -0.5f; verts[0].z = 0.0f;
            atS.x = mc.at.x * DAT_005ce4f0;
            verts[1].x = -0.5f; verts[1].y = -0.5f; verts[1].z = 0.0f;
            verts[2].x = -0.5f; verts[2].y = 0.5f;  verts[2].z = 0.0f;
            verts[3].x = 0.5f;  verts[3].y = 0.5f;  verts[3].z = 0.0f;
            FUN_004c51a0(&mc, &atS, 2);
            cam = FUN_00467210(0);
            dx = cam->pos.x - mc.pos.x;
            dy = cam->pos.y - mc.pos.y;
            dz = cam->pos.z - mc.pos.z;
            d2 = dz * dz + (dx * dx + dy * dy);
            if (d2 > DAT_005ce4ec)
                d2 = DAT_005ce4ec;
            c2.a = (unsigned char)(d2 * DAT_005cc9d0 * DAT_005ce4e8 + DAT_005cc9b0);
            FUN_004c3d90(verts, verts, 4, &mc);
            fx = cam->pos.x * -0.1f - cam->at.x;
            fz = (cam->pos.z * -0.1f) + cam->at.y;
            pts[0].x = (verts[0].x * DAT_005cc32c + fx) + DAT_005cc32c;
            pts[0].y = (verts[0].z * DAT_005cc32c + fz) + DAT_005cc32c;
            pts[1].x = (verts[1].x * DAT_005cc32c + fx) + DAT_005cc32c;
            pts[1].y = (verts[1].z * DAT_005cc32c + fz) + DAT_005cc32c;
            pts[2].x = (verts[2].x * DAT_005cc32c + fx) + DAT_005cc32c;
            pts[2].y = (DAT_005cc32c * verts[2].z + fz) + DAT_005cc32c;
            pts[3].x = (verts[3].x * DAT_005cc32c + fx) + DAT_005cc32c;
            pts[3].y = DAT_005cc32c + (verts[3].z * DAT_005cc32c + fz);
            FUN_004769f0(&c2);
            FUN_00476a80(pts);
            FUN_00476a10(&mc);
            FUN_00476d00(DAT_0068a210);
            FUN_004570a0(e);
            if (e->flag == 0 && e->state == 1) {
                e->timer = e->timer - DAT_007f100c;
                if (e->timer <= DAT_005d757c) {
                    FUN_004c4670(&tmp, &e->m);
                    FUN_004864f0(&tmp, 1.2f, 1);
                    e->flag = 1;
                    e->state = 0;
                    e->timer = 0.1f;
                }
            }
            if (e->flag == 1 && e->u < DAT_005cc950) {
                c3.r = 0x20;
                c3.g = 0x20;
                c3.b = 0x20;
                c3.a = 200;
                if (FUN_00472650(0.0f, 1.0f) > DAT_005ce4e4) {
                    v = e->m.pos;
                    v.x = FUN_00472650(-0.1f, 0.1f) + v.x;
                    v.y = FUN_00472650(-0.1f, 0.1f) + v.y;
                    v.z = FUN_00472650(-0.1f, 0.1f) + v.z;
                    FUN_0048f780(&v, 0.4f, &c3, 5.0f, 0.5f, 0, 0);
                }
            }
        }
        i = i + 1;

    }
    FUN_00476df0(DAT_0068a2c0);
    FUN_00476df0(DAT_0068a210);
}
