// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00471ec0 (h6): best 44.4% (regs 57.2%, ops 87.7%), 187/186 instructions, class (b).
   Structure (loops, switch on mode 0/1/2, x87 length test, early return through FUN_00471ac0) matches;
   the score is held down by register allocation: the original keeps x (FUN_00407a40 result) in ebp and the
   record pointer in ebx, the sub-record base register at +0x458 (done) and the ids walk as a stack pointer.
   The rdata floats are compiler literals (0.001f here; no score change). Moves that helped: float locals loaded y, x, z with (x*x + y*y) + z*z (+3%), h = 0 / k = 0 stores
   hoisted as in the original. Tried: no local sub pointer (45.0% with worse shape), declaration order,
   permuter 390 compiles (45.5% via a move, not kept).
   RecTable at 0x006905c8 and the 0x2510 entry layout as 0x00471cf0; sub-record
   fields read here: +0x0, +0x4, +0x8, ids[8] +0xc, count +0x2c, mode +0x30 (switch 0/1/2), stop +0x34,
   done +0x458. Callees cdecl: 0x0040e180 (int *, int *), 0x00407a40 / 0x00407a20 (int) -> int,
   0x00471780 (int, entry *), 0x0047ce80 / 0x0047ce00 / 0x0047d130 (int) -> int, 0x0057c210 (int) -> pointer,
   0x00471ac0 (void). */
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
typedef struct Sub45c {
    int field_0;               /* +0x000 */
    int field_4;               /* +0x004 */
    int field_8;               /* +0x008 */
    int ids[8];                /* +0x00c */
    int count;                 /* +0x02c */
    int mode;                  /* +0x030 */
    int stop;                  /* +0x034 */
    unsigned char pad38[0x420];
    int done;                  /* +0x458 */
} Sub45c;
typedef struct E1c { int f[7]; } E1c;
typedef struct Obj2510 {
    char name[0x20];           /* +0x000 */
    int ids[8];                /* +0x020 */
    int count;                 /* +0x040 */
    int a[32];                 /* +0x044 */
    int handle[32];            /* +0x0c4 */
    int nhandle;               /* +0x144 */
    Sub45c sub[8];             /* +0x148 */
    int nsub;                  /* +0x2428 */
    E1c e[8];                  /* +0x242c */
    int ne;                    /* +0x250c */
} Obj2510;
typedef struct Rec8c {
    Obj2510 *ptr[16];   /* +0x00 */
    int id[16];         /* +0x40 */
    int count;          /* +0x80 */
    int prev;           /* +0x84 */
    int field_88;       /* +0x88 */
} Rec8c;
typedef struct RecTable {
    int cur;         /* 0x006905c8 */
    Rec8c rec[8];    /* 0x006905cc */
} RecTable;
typedef struct V20 { float x, y, z; int pad[5]; } V20;
typedef struct G10 { int pad[2]; V20 *verts; } G10;
typedef struct A10 { int pad[4]; G10 *geo; } A10;
typedef struct Ref { A10 *a; int idx; } Ref;
typedef struct B10 { int pad; Ref *ref; } B10;
typedef struct Q10 { int pad[4]; B10 *b; } Q10;
MGLOBAL(RecTable, DAT_006905c8, 0x006905c8);
void FUN_0040e180(int *a, int *b);
int FUN_00407a40(int a);
int FUN_00407a20(int a);
void FUN_00471780(int a, Obj2510 *o);
void FUN_00471ac0(void);
int FUN_0047ce80(int a);
unsigned int FUN_0047ce00(int a);
int FUN_0047d130(int a);
Q10 *FUN_0057c210(int a);
// MATCH: 0x00471ec0 FUN_00471ec0
void FUN_00471ec0(int param_1)
{
    int i, k, h, m;
    int a, b;
    int x, y, c;
    int hd;
    unsigned int fl;
    int p;
    Q10 *q;
    Ref *ref;
    V20 *v;
    float fx, fy, fz;
    Rec8c *r;
    Obj2510 *o;
    Sub45c *s;

    i = 0;
    FUN_0040e180(&a, &b);
    x = FUN_00407a40(a);
    y = FUN_00407a20(a);
    for (; i < DAT_006905c8.cur; i++) {
        r = &DAT_006905c8.rec[i];
        o = r->ptr[r->count];
        k = 0;
        FUN_00471780(param_1, o);
        for (; k < o->nsub; k++) {
            s = &o->sub[k];
            h = 0;
            if (s->field_0 > -1 && s->done == 0 && (s->field_8 == -1 || y - s->field_4 == s->field_8) &&
                x == s->field_0) {
                r->field_88 = 1;
                s->done = 1;
                if (s->stop) {
                    FUN_00471ac0();
                    return;
                }
            }
            for (; h < o->nhandle; h++) {
                hd = o->handle[h];
                c = FUN_0047ce80(hd);
                for (m = 0; m < s->count; m++) {
                    if (s->done)
                        break;
                    if (c == s->ids[m]) {
                        fl = FUN_0047ce00(hd);
                        switch (s->mode) {
                        case 0:
                            if (!(fl & 4))
                                continue;
                            break;
                        case 1:
                            if (!(fl & 2))
                                continue;
                            break;
                        case 2:
                            p = FUN_0047d130(hd);
                            if (p == 0)
                                continue;
                            q = FUN_0057c210(p);
                            if (q == 0)
                                continue;
                            ref = q->b->ref;
                            if (ref == 0)
                                continue;
                            v = &ref->a->geo->verts[ref->idx];
                            fy = v->y;
                            fx = v->x;
                            fz = v->z;
                            if (!((fx * fx + fy * fy) + fz * fz > 0.001f))
                                continue;
                            break;
                        default:
                            continue;
                        }
                        r->field_88 = 1;
                        s->done = 1;
                        if (s->stop) {
                            FUN_00471ac0();
                            return;
                        }
                    }
                }
            }
        }
    }
}
