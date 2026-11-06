// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00457c10 (h6): best 73.4% (regs 92.8%, ops 91.4%), 139/139 instructions. Entry (the 0x18 B
   PoolI header) arrives in EAX (pilot finding 1): `static`, defined before a STAND-IN caller (not matched, not
   shipped); the real call is at 0x00457f04 in a function with no Ghidra size. Raw converter 22.3%.
   What moved it: callee arities read from their bodies (0x0045bfe0 and 0x0045bfd0 -> 0x00426060 take no argument,
   so 0x004b4cd0 gets (world, &line, &hit) and 0x004e4450 gets (0x0045bfd0(), c->obj)), a scaled dir local
   (dir = up * -1.0f, end = start + dir * 0.5f), axis {1,0,0} and t = hit+0x38 set before the 0x0045c110 test,
   literal constants -1.0f / 0.5f / 0.005f (= 0x005cc33c / 0x005cc32c / 0x005cd0ec; extern globals put the
   constant first in each fmul). Tried: declaration orders (40, no effect), statement orders around count--, t and
   the f4c store, int-typed t, a {V3; float} struct for axis+t, `!` vs `== 0`, index operand order, two permuter
   runs (the 80.6% variant it found reads c before assigning it, rejected).
   Residual (b): register rotation from the t load after the 0x0045c110 call on (orig eax/ecx/edx, ours
   ecx/edx/eax) through the 0x004b4650 / 0x004b5080 / 0x004c4d20 / 0x004c1480 argument setup, and the up.y load
   scheduled one slot later in the prologue. */
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
typedef unsigned short ushort;
typedef struct { float x, y, z; } V3;
typedef struct { V3 right; unsigned int flags; V3 up; unsigned int pad1; V3 at; unsigned int pad2; V3 pos;
                 unsigned int pad3; } Mat;
typedef struct { V3 start, end; } Line;
typedef struct { ushort v[3]; ushort mat; } Tri;
typedef struct { int f00; Tri *tris; char pad08[0x80 - 0x08]; ushort matbase; } Geom;
typedef struct { char pad00[0x10]; int *mats; } MatList;
typedef struct {
    V3 n;               /* +0x00 */
    int tri;            /* +0x0c */
    V3 verts[3];        /* +0x10 */
    Geom *geom;         /* +0x34 */
    float f38;          /* +0x38 */
    MatList *mlist;     /* +0x3c */
} Hit;
typedef struct { int f00; void *frame; } Obj;
typedef struct {
    Mat mat;            /* +0x00 */
    V3 f40;             /* +0x40 */
    float f4c;          /* +0x4c */
    int f50, f54;
    Obj *obj;           /* +0x58 */
    int f5c, f60, f64, f68;
} PoolJ;                /* 0x6c */
typedef struct {
    PoolJ *ent[2];      /* +0x00 */
    int count;          /* +0x08 */
    Mat *mat;           /* +0x0c */
    int f10;
    void *frame;        /* +0x14 */
} PoolI;                /* 0x18 */
int FUN_0045bfe0(void);
int FUN_004b4cd0(int world, Line *line, Hit *hit);
int FUN_0045c110(int m);
void FUN_004b4650(V3 *out, V3 *a, V3 *b, float t);
void FUN_004b5080(Mat *out, V3 *tri, V3 *p);
Mat *FUN_004c4d20(Mat *m, V3 *axis, float angle, int op);
void *FUN_004c1480(void *frame, Mat *m, int op);
void *FUN_0045bfd0(void);
void *FUN_004e4450(void *parent, void *child);
// MATCH: 0x00457c10 FUN_00457c10
static void FUN_00457c10(PoolI *e)
{
    Mat *m = e->mat;
    Line line;
    Hit hit;
    V3 axis;
    float t;
    V3 p;
    Mat mt;
    PoolJ *c;
    V3 dir;
    dir.x = m->up.x * -1.0f;
    dir.y = m->up.y * -1.0f;
    dir.z = m->up.z * -1.0f;
    line.start = m->pos;
    line.end = line.start;
    line.end.x += dir.x * 0.5f;
    line.end.y += dir.y * 0.5f;
    line.end.z += dir.z * 0.5f;
    if (FUN_004b4cd0(FUN_0045bfe0(), &line, &hit) != 0) {
        axis.x = 1.0f;
        axis.y = 0.0f;
        axis.z = 0.0f;
        t = hit.f38;
        if (FUN_0045c110(hit.mlist->mats[hit.geom->tris[hit.tri].mat + hit.geom->matbase]) == 0) {
            FUN_004b4650(&p, &line.start, &line.end, t);
            e->count--;
            p.x += hit.n.x * 0.005f;
            p.y += hit.n.y * 0.005f;
            p.z += hit.n.z * 0.005f;
            c = e->ent[e->count];
            e->ent[e->count] = 0;
            c->f5c = 0;
            c->f60 = 2;
            c->f64 = 0;
            c->f40 = p;
            c->f4c = 0.125f;
            FUN_004b5080(&mt, hit.verts, &p);
            c->mat = mt;
            FUN_004c4d20(&mt, &axis, -90.0f, 1);
            FUN_004c1480(c->obj->frame, &mt, 0);
            FUN_004e4450(FUN_0045bfd0(), c->obj);
        }
    }
}
/* stand-in caller (not matched, not shipped): the real caller (call at 0x00457f04, no Ghidra size) passes the
   entry in EAX */
void FUN_00457c10_standin(PoolI *e)
{
    if (e->f10 != 0)
        FUN_00457c10(e);
}
