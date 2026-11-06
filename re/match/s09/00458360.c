// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00458360 (h6): 100%, byte-identical (51 B). This file is the 0x00457ba0..0x00458392 stretch of one
   TU and HOLDS three register-arg callees as `static` functions defined first (pilot finding 1), each 100% and
   byte-identical (whole obj symbol, relocations masked):
   0x00457ba0 (110 B, entry in ESI: identity matrix + zeroed fields, then detach +0x58 via 0x004e4800/0x004e45b0),
   0x00458020 (90 B, header in EDI: releases the two entry pointers through 0x00457ba0, zeroes +0x08..+0x10,
   0x004b6520(h, 8), detaches +0x14),
   0x00458080 (615 B, entry in EAX: state machine on +0x60, if-chain 0/1, 2, 3, 4, 5, 6; sin is the intrinsic for
   the fsin; 0x005ccd6c/0x005cc8f0/0x005cc9c0/0x005cc950 = 20.0f/0.15f/0.2f/0.75f written as literals; the
   query struct for 0x00484cf0 is 0x8c B with the callback 0x00457e10 at +0x1c).
   It also holds 0x004582f0, the real caller of 0x00458020 / 0x00458080: Ghidra has no function there, so it has no
   MATCH line; its whole obj symbol (104 B) is byte-identical too (match.byte_compare, relocations masked).
   Raw converter: 84.1% / 34.4% / 42.9% / 50.0% (no register args). Struct names are mechanical: PoolJ = 32 x 0x6c B
   at 0x0068a310 (RwMatrix-shaped head), PoolI = 4 x 0x18 B at 0x0068b0d0.
   C++ check: no error; VS2022 warns C4162 on `#pragma intrinsic(sin)` (sin declared without C linkage).
   Residual: none. */
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
typedef struct { V3 right; unsigned int flags; V3 up; unsigned int pad1; V3 at; unsigned int pad2; V3 pos;
                 unsigned int pad3; } Mat;
typedef struct { int f00; void *frame; } Obj;
typedef struct { V3 c; float r; } Sphere;
typedef struct {
    Mat mat;            /* +0x00 */
    Sphere f40;         /* +0x40 */
    int f50;
    Obj *obj;           /* +0x54 */
    void *frame;        /* +0x58 */
    float f5c;
    int state;          /* +0x60 */
    float timer;        /* +0x64 */
    int count;          /* +0x68 */
} PoolJ;                /* 0x6c */
typedef struct {
    PoolJ *ent[2];      /* +0x00 */
    int f8;
    Mat *mat;           /* +0x0c */
    int state;          /* +0x10 */
    Obj *obj;           /* +0x14 */
} PoolI;                /* 0x18 */
typedef struct { unsigned char r, g, b, a; } RGBA;
typedef struct { float x, y; } V2;
typedef struct { V3 v; float w; } V4;
typedef struct { Sphere sphere; int kind; PoolJ *rec; int zero; void (*cb)(void); int pad20[27]; } Query;
MGLOBALA(PoolJ, DAT_0068a310, 32, 0x0068a310);
MGLOBALA(PoolI, DAT_0068b0d0, 4, 0x0068b0d0);
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBALA(float, DAT_005f9830, 1, 0x005f9830);
MGLOBALA(float, DAT_005f9834, 1, 0x005f9834);
MGLOBALA(float, DAT_006146fc, 3, 0x006146fc);
MGLOBALA(int, DAT_0068b090, 16, 0x0068b090);
void *FUN_004e4800(void *frame);
void *FUN_004e45b0(void *parent, void *frame);
void FUN_004b6520(void *p, int n);
Mat *FUN_004c0ed0(void *frame);
void FUN_004769a0(V3 *p);
void FUN_004769d0(V2 *s);
void FUN_004769f0(float *c);
void FUN_00476d00(int *list);
void FUN_00476df0(int *list);
void FUN_00484cf0(Query *q);
void FUN_00457e10(void);
void FUN_00476880(Sphere *s, float *v, float a, float b);
void FUN_00475ab0(int a, PoolJ *e, int c);
void FUN_00477760(int a, PoolJ *e, RGBA *col, float b, float c);
void FUN_00453eb0(V4 *p, float a, float b, float c, int d);
void *FUN_004c1480(void *frame, Mat *m, int op);
double sin(double);
#pragma intrinsic(sin)
// MATCH: 0x00457ba0 FUN_00457ba0
static void FUN_00457ba0(PoolJ *e)
{
    void *parent;
    e->mat.at.z = 1.0f;
    e->mat.up.y = 1.0f;
    e->mat.right.x = 1.0f;
    e->mat.up.x = 0.0f;
    e->mat.right.z = 0.0f;
    e->mat.right.y = 0.0f;
    e->mat.at.y = 0.0f;
    e->mat.at.x = 0.0f;
    e->mat.up.z = 0.0f;
    e->mat.pos.z = 0.0f;
    e->mat.pos.y = 0.0f;
    e->mat.pos.x = 0.0f;
    e->mat.flags |= 0x20003;
    e->state = 0;
    e->f5c = 0.0f;
    e->timer = 0.0f;
    e->f40.c.x = 0.0f;
    e->f40.c.y = 0.0f;
    e->f40.c.z = 0.0f;
    e->f40.r = 0.0f;
    e->count = 0;
    parent = FUN_004e4800(e->frame);
    if (parent != 0)
        FUN_004e45b0(parent, e->frame);
}
// MATCH: 0x00458020 FUN_00458020
static void FUN_00458020(PoolI *h)
{
    int i;
    void *parent;
    for (i = 0; i < 2; i++) {
        if (h->ent[i] != 0) {
            PoolJ *e = h->ent[i];
            h->ent[i] = 0;
            FUN_00457ba0(e);
        }
    }
    h->f8 = 0;
    h->mat = 0;
    h->state = 0;
    FUN_004b6520(h, 8);
    parent = FUN_004e4800(h->obj);
    if (parent != 0)
        FUN_004e45b0(parent, h->obj);
}
// MATCH: 0x00458080 FUN_00458080
static void FUN_00458080(PoolJ *e)
{
    V2 sc;
    int st = e->state;
    sc.x = 0.25f;
    sc.y = 0.25f;
    if (st == 0 || st == 1)
        return;
    if (st == 2) {
        e->count++;
        e->timer += DAT_007f100c;
        sc.x = sc.y = (float)sin(e->timer * 20.0f) * 0.15f + 0.2f;
        FUN_004769a0(&FUN_004c0ed0(e->obj->frame)->pos);
        FUN_004769d0(&sc);
        FUN_004769f0(DAT_005f9830);
        FUN_00476d00(DAT_0068b090);
        if (e->timer >= 0.75f)
            e->state = 3;
    } else if (st == 3) {
        Query q;
        q.sphere = e->f40;
        q.kind = 0xd;
        q.rec = e;
        q.zero = 0;
        q.cb = FUN_00457e10;
        FUN_00484cf0(&q);
        e->timer += DAT_007f100c;
        sc.x = sc.y = (float)sin(e->timer * 20.0f) * 0.15f + 0.2f;
        FUN_004769a0(&FUN_004c0ed0(e->obj->frame)->pos);
        FUN_004769d0(&sc);
        FUN_004769f0(DAT_005f9834);
        FUN_00476d00(DAT_0068b090);
    } else if (st == 4) {
        RGBA col;
        void *parent;
        col.r = 0xff;
        col.g = 0x80;
        col.b = 0;
        col.a = 0x80;
        e->state = 5;
        FUN_00476880(&e->f40, DAT_006146fc, 90.0f, 0.75f);
        e->f40.r = 0.1f;
        parent = FUN_004e4800(e->frame);
        if (parent != 0)
            FUN_004e45b0(parent, e->frame);
        FUN_00475ab0(0, e, 0);
        FUN_00477760(0, e, &col, 1.5f, 6.0f);
    } else if (st == 5) {
        V4 p;
        p.v = e->f40.c;
        p.w = 1.5f;
        e->state = 6;
        FUN_00453eb0(&p, 25.0f, 200.0f, 300.0f, 2);
    } else if (st == 6) {
        e->state = 0;
        FUN_00457ba0(e);
    }
}
/* 0x004582f0 (no Ghidra function, not scored): the real caller of 0x00458020 / 0x00458080, transcribed so the two
   statics keep their EDI / EAX conventions */
void FUN_004582f0(void)
{
    int i;
    for (i = 0; i < 4; i++) {
        PoolI *h = &DAT_0068b0d0[i];
        if (h->state != 0) {
            if (h->state == 1)
                FUN_004c1480(h->obj->frame, h->mat, 0);
            else if (h->state == 2)
                FUN_00458020(h);
        }
    }
    for (i = 0; i < 32; i++)
        FUN_00458080(&DAT_0068a310[i]);
    FUN_00476df0(DAT_0068b090);
}
// MATCH: 0x00458360 FUN_00458360
void FUN_00458360(void)
{
    int i;
    for (i = 0; i < 4; i++)
        FUN_00458020(&DAT_0068b0d0[i]);
    for (i = 0; i < 32; i++)
        FUN_00457ba0(&DAT_0068a310[i]);
}
