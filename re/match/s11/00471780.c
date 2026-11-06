// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00471780 (h6): best 73.3% (regs 79.8%, ops 96.3%), 243/243 instructions, class (b).
   Control flow, calls, x87 compares and constants line up. Moves: the rdata floats are compiler literals
   (__real@ constants: 1.0, 0.001, 1/6, +-0.05, +-0.95, 5, 30, 0, 21, 10), f = scale * (len * 30.0f),
   the force.y selection as if/else stores (+1.7%). Residue is register/stack-slot allocation: the original
   keeps the anim pointer in ebp (reloaded after ebp is reused for the copied vertex z), spills pp
   (= &m->pos), evaluates d.y/d.z as fld m; fsubr mat, and the force products as fld at; fmul st(1).
   Tried: pp/m spellings for d, negated forms, scalar copy of the vertex, operand order of the products,
   permuter 622 compiles (72.0% only by a semantic change, not kept).
   Entries: 0x1c-byte records at +0x242c of the 0x2510 layout (count +0x250c): +0x0 handle, +0x8 selector,
   +0xc flag, +0x10 index into param_1's table at +0x105b0, +0x14 active, +0x18 time (float).
   Callees all cdecl: 0x0047d150 (int) -> matrix, 0x0047d130 (int) -> int, 0x004752f0 / 0x004b47e0
   (matrix *, anim *, float), 0x0047d180 (int, matrix *), 0x0057c210 (int) -> object, 0x0055ac50 /
   0x0055ac00 (int, object *, int), 0x0055ad30 (int, object *), 0x004c3ac0 (V3 *) -> float,
   0x004c4dc0 (matrix *, matrix *), 0x004c3d60 (V3 *, V3 *, matrix *), 0x0055b650 (ref *, V3 *, V3 *),
   0x0055c4a0 (int, float). */
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
typedef struct V3 { float x, y, z; } V3;
typedef struct Mat { V3 right; int flags; V3 up; int pad1; V3 at; int pad2; V3 pos; int pad3; } Mat;
typedef struct Anim { int pad[3]; float len; } Anim;
typedef struct Ctx { unsigned char pad[0x105b0]; Anim *anim[1]; } Ctx;
typedef struct E1c {
    int handle;                /* +0x00 */
    int f4;                    /* +0x04 */
    int sel;                   /* +0x08 */
    int flag;                  /* +0x0c */
    int idx;                   /* +0x10 */
    int active;                /* +0x14 */
    float t;                   /* +0x18 */
} E1c;
typedef struct Obj2510 {
    unsigned char pad0[0x242c];
    E1c e[8];                  /* +0x242c */
    int ne;                    /* +0x250c */
} Obj2510;
typedef struct Vtx20 { int pad[4]; V3 v; int pad1c; } Vtx20;
typedef struct Pt30 { int pad[3]; float s; int pad10[8]; } Pt30;
typedef struct Geo { int pad[2]; Vtx20 *verts; int padc[5]; Pt30 *pts; } Geo;
typedef struct Atom { int pad[4]; Geo *geo; } Atom;
typedef struct Ref { Atom *a; int idx; int pad8[3]; float f14; float f18; float f1c; } Ref;
typedef struct Hold { int first; Ref *ref; } Hold;
typedef struct Obj { int pad[4]; Hold *hold; int pad14[4]; int f24; } Obj;
MGLOBAL(float, DAT_007f100c, 0x007f100c);
Mat *FUN_0047d150(int h);
int FUN_0047d130(int h);
void FUN_004752f0(Mat *m, Anim *a, float t);
void FUN_004b47e0(Mat *m, Anim *a, float t);
void FUN_0047d180(int h, Mat *m);
Obj *FUN_0057c210(int h);
void FUN_0055ac50(int a, Obj *o, int b);
void FUN_0055ac00(int a, Obj *o, int b);
void FUN_0055ad30(int a, Obj *o);
float FUN_004c3ac0(V3 *v);
void FUN_004c4dc0(Mat *out, Mat *in);
void FUN_004c3d60(V3 *out, V3 *in, Mat *m);
void FUN_0055b650(Ref *r, V3 *f, V3 *p);
void FUN_0055c4a0(int a, float b);
// MATCH: 0x00471780 FUN_00471780
void FUN_00471780(Ctx *param_1, Obj2510 *param_2)
{
    int k;
    E1c *e;
    Mat *m;
    int h;
    float dt;
    Anim *anim;
    Obj *q;
    int first;
    Ref *ref;
    float scale;
    V3 *pp;
    V3 d;
    float len;
    Mat inv;
    V3 lp;
    V3 saved;
    float f;
    V3 force;
    Mat mat;

    k = 0;
    for (; k < param_2->ne; k++) {
        e = &param_2->e[k];
        if (e->active == 0)
            continue;
        m = FUN_0047d150(e->handle);
        h = FUN_0047d130(e->handle);
        dt = DAT_007f100c + DAT_007f100c;
        anim = param_1->anim[e->idx];
        if (e->sel != 0)
            FUN_004752f0(&mat, anim, e->t);
        else
            FUN_004b47e0(&mat, anim, e->t);
        if (e->t <= 0.001f)
            FUN_0047d180(e->handle, &mat);
        q = FUN_0057c210(h);
        FUN_0055ac50(q->f24, q, 1);
        FUN_0055ac00(q->f24, q, 1);
        FUN_0055ad30(q->f24, q);
        first = q->hold->first;
        ref = q->hold->ref;
        if (ref != 0) {
            scale = ref->a->geo->pts[ref->idx].s;
            pp = &m->pos;
            d.x = mat.pos.x - pp->x;
            d.y = mat.pos.y - m->pos.y;
            d.z = mat.pos.z - m->pos.z;
            len = FUN_004c3ac0(&d) * 0.16666667f;
            dt = (1.0f - len) * dt;
            if (len > 1.0f)
                e->active = 0;
            FUN_004c4dc0(&inv, m);
            FUN_004c3d60(&lp, &mat.pos, &inv);
            saved = ref->a->geo->verts[ref->idx].v;
            if (lp.x > 0.05f || lp.x < -0.05f) {
                if (lp.x > 0.95f)
                    lp.x = 0.95f;
                else if (lp.x < -0.95f)
                    lp.x = -0.95f;
                saved.y = lp.x + saved.y;
                ref->a->geo->verts[ref->idx].v = saved;
                ref->f18 = 1.0f / (DAT_007f100c * 5.0f);
            }
            f = scale * (len * 30.0f);
            force.x = m->at.x * f;
            force.y = m->at.y * f;
            force.z = m->at.z * f;
            if (e->flag == 0)
                if (d.y > 0.0f)
                    force.y = scale * 21.0f;
                else
                    force.y = scale * 10.0f;
            FUN_0055b650(ref, &force, pp);
            ref->f14 = 1.0f / (DAT_007f100c * 10.0f);
            FUN_0055c4a0(first, 0.01f);
            ref->f1c = 0.01f;
        }
        e->t += dt;
        if (e->t > anim->len)
            e->t = 0.02f;
    }
}
