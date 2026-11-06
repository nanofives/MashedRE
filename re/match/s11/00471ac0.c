// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00471ac0 (h6): 100% by whole-symbol bytes (--bytes: 547 B BYTE-IDENTICAL); instruction score
   97.7% only because the Ghidra size (541 B) stops before the last 4 epilogue instructions. Moves: inner
   search loop re-reads r->ptr[r->count] in the condition (no cached pointer), sub loop as if/else store
   (the ?: form hoisted the mask out of the loop, 48.8%), e-loop through a local E1c pointer (base register
   at +0x14 instead of +0x0), the two branches of the 0x0047d080/0x0047d100 pair written out with 1 and 0.
   RecTable struct at 0x006905c8 (as 0x00471450), name/callback table at
   0x00690a30 (+0x80 used as a callback taking FUN_00426020()'s result), 0x2510 entry layout as 0x00471cf0.
   Callees: 0x0040e180 (int *, int *), 0x00407a20 (int) -> int, 0x00426020 (void) -> void *,
   0x0047d080 / 0x0047d100 (int, int), 0x0047ce40 (int) -> int, all cdecl. */
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
typedef void (*CB)(void *);
typedef struct Sub45c {
    int field_0;               /* +0x000 */
    int field_4;               /* +0x004 */
    unsigned char pad8[0x450];
    int field_458;             /* +0x458 */
} Sub45c;
typedef struct E1c {
    int f0;                    /* +0x00 */
    int f4;                    /* +0x04 */
    int a;                     /* +0x08 */
    int pad0c[2];
    int c;                     /* +0x14 */
    int d;                     /* +0x18 */
} E1c;
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
typedef struct Name84 {
    char name[0x80]; /* +0x00 */
    CB value;        /* +0x80 */
} Name84;
MGLOBAL(RecTable, DAT_006905c8, 0x006905c8);
MGLOBALA(Name84, DAT_00690a30, 20, 0x00690a30);
void FUN_0040e180(int *a, int *b);
int FUN_00407a20(int a);
void *FUN_00426020(void);
void FUN_0047d080(int a, int b);
void FUN_0047d100(int a, int b);
int FUN_0047ce40(int a);
// MATCH: 0x00471ac0 FUN_00471ac0
void FUN_00471ac0(void)
{
    int i, j, k, m;
    int v;
    int a, b;
    int n;
    int found, flag;
    Rec8c *r;
    Obj2510 *o;
    CB fn;

    i = 0;
    FUN_0040e180(&a, &b);
    v = FUN_00407a20(a);
    if (v < 0)
        v = 0;
    for (; i < DAT_006905c8.cur; i++) {
        r = &DAT_006905c8.rec[i];
        if (r->field_88 != 0) {
            n = r->id[r->count] - 2000;
            if (n > -1) {
                void *p = FUN_00426020();
                fn = DAT_00690a30[n].value;
                if (fn != 0)
                    fn(p);
            }
            if (++r->count >= r->prev)
                r->count = 0;
        }
        r->field_88 = 0;
        for (j = 0; j < r->prev; j++) {
            o = r->ptr[j];
            flag = r->ptr[r->count] == o;
            for (k = 0; k < o->nhandle; k++) {
                found = 0;
                for (m = 0; m < r->ptr[r->count]->nhandle; m++) {
                    if (o->handle[k] == r->ptr[r->count]->handle[m]) {
                        found = 1;
                        break;
                    }
                }
                if (flag || found) {
                    FUN_0047d080(o->handle[k], 1);
                    FUN_0047d100(o->handle[k], 1);
                } else {
                    FUN_0047d080(o->handle[k], 0);
                    FUN_0047d100(o->handle[k], 0);
                }
            }
            for (k = 0; k < o->nsub; k++) {
                o->sub[k].field_458 = 0;
                if (flag) o->sub[k].field_4 = v; else o->sub[k].field_4 = 0;
            }
            for (k = 0; k < o->ne; k++) {
                E1c *e = &o->e[k];
                e->f0 = FUN_0047ce40(e->f4);
                e->c = 1;
                e->d = 0;
            }
        }
    }
}
