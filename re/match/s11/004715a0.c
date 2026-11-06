// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x004715a0 (h6): 100% by whole-symbol bytes (--bytes: 477 B BYTE-IDENTICAL). Instruction score
   93.4% only because the Ghidra size (456 B) leaves out 21 bytes of in-function alignment padding, so the
   compare stops before the last 9 instructions (class c, size only). Body from the pilot's a3_004715a0.c
   (re/match/pilot), struct rebased onto the 0x2510 layout shared with 0x00471cf0 / 0x00471df0 (sub-records
   start at +0x148, 0x1c entries at +0x242c); same bytes. Table of 0x110-byte entries at 0x0086cbc4
   (+0x0 base, +0x8 n). FUN_0047ce40: cdecl, 1 arg.
   The T110 array length (32) is a placeholder: no bound for it appears in this function. */
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
typedef struct T110 { int base; int pad04; int n; int pad0c[(0x110 - 0x0c) / 4]; } T110;
typedef struct Sub45c {
    int field_0;               /* +0x000 */
    int field_4;               /* +0x004 */
    int field_8;               /* +0x008 */
    int ids[8];                /* +0x00c */
    int count;                 /* +0x02c */
    int pad30[2];
    int data[8][0x20];         /* +0x038 */
    int counts[8];             /* +0x438 */
    int field_458;             /* +0x458 */
} Sub45c;
typedef struct E1c {
    int f0, f4;
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
    int b[32];                 /* +0x0c4 */
    int total;                 /* +0x144 */
    Sub45c sub[8];             /* +0x148 */
    int nsub;                  /* +0x2428 */
    E1c e[8];                  /* +0x242c */
    int ne;                    /* +0x250c */
} Obj2510;
MGLOBALA(Obj2510, DAT_0086ece0, 8, 0x0086ece0);
MGLOBALA(T110, DAT_0086cbc4, 32, 0x0086cbc4);
int FUN_0047ce40(int p0);
// MATCH: 0x004715a0 FUN_004715a0
void FUN_004715a0(void)
{
    Obj2510 *r;
    Sub45c *s;
    int i, j, k, c, id;
    int v;

    for (k = 0; k < 8; k++) {
        r = &DAT_0086ece0[k];
        i = 0;
        r->total = 0;
        if (r->name[0]) {
            for (; i < r->count; i++) {
                id = r->ids[i];
                for (j = 0; j < DAT_0086cbc4[id].n; j++) {
                    v = DAT_0086cbc4[id].base + j;
                    r->a[r->total + j] = v;
                    r->b[r->total + j] = FUN_0047ce40(v);
                }
                r->total += DAT_0086cbc4[id].n;
            }
            for (i = 0; i < r->nsub; i++) {
                s = &r->sub[i];
                for (c = 0; c < s->count; c++) {
                    id = s->ids[c];
                    j = 0;
                    s->counts[c] = j;
                    for (; j < DAT_0086cbc4[id].n; j++) {
                        v = DAT_0086cbc4[id].base + j;
                        s->data[c][j] = FUN_0047ce40(v);
                        s->counts[c]++;
                    }
                }
            }
            for (i = 0; i < r->ne; i++) {
                r->e[i].c = 1;
                r->e[i].d = 0;
                r->e[i].a = 1;
            }
        }
    }
}
