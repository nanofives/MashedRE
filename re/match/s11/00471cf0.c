// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00471cf0 (h6): 100% by whole-symbol bytes (--bytes: 256 B BYTE-IDENTICAL; instruction score 91.2% because Ghidra size 245 stops before the loop tail); RecTable struct at 0x006905c8 (as 0x00471450); records point at 0x2510-byte
   entries (0x0086ece0 table): +0xc4 int[32], +0x144 count, +0x148 8 sub-entries of 0x45c (+0x4, +0x458),
   +0x2428 sub-entry count. Callees 0x0047d080 / 0x0047d100 are cdecl, 2 args. */
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
    unsigned char pad8[0x450];
    int field_458;             /* +0x458 */
} Sub45c;
typedef struct Obj2510 {
    unsigned char pad0[0xc4];
    int handle[32];            /* +0x0c4 */
    int nhandle;               /* +0x144 */
    Sub45c sub[8];             /* +0x148 */
    int nsub;                  /* +0x2428 */
    unsigned char pad242c[0x2510 - 0x242c];
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
MGLOBAL(RecTable, DAT_006905c8, 0x006905c8);
void FUN_0047d080(int a, int b);
void FUN_0047d100(int a, int b);
// MATCH: 0x00471cf0 FUN_00471cf0
void FUN_00471cf0(void)
{
    int i, j, k;
    Rec8c *r;
    Obj2510 *o;
    int flag;
    for (i = 0; i < DAT_006905c8.cur; i++) {
        r = &DAT_006905c8.rec[i];
        r->count = 0;
        r->field_88 = 0;
        for (j = 0; j < r->prev; j++) {
            o = r->ptr[j];
            flag = r->ptr[0] == o;
            for (k = 0; k < o->nhandle; k++) {
                FUN_0047d080(o->handle[k], flag);
                FUN_0047d100(o->handle[k], flag);
            }
            for (k = 0; k < o->nsub; k++) {
                o->sub[k].field_458 = 0;
                o->sub[k].field_4 = 0;
            }
        }
    }
}
