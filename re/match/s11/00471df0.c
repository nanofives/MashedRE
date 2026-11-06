// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00471df0 (h6): 100% (regs 100%). Moves: static callee 0x00471530 (edx kept across the call), record loop as index loop + local record pointer, entry loop as index loop with direct DAT_0086ece0[i] access (pointer loops gave dec-counter loops), memset intrinsic for the three cleared arrays; RecTable struct at 0x006905c8 (as 0x00471450), name table at 0x00690a30
   (as 0x00471530), 8 entries of 0x2510 at 0x0086ece0. The inner 16-slot loop index is NOT reset per record
   in the original (edx zeroed once at 0x00471df5), so the source shares one index across records.
   extraout_EDX in Ghidra = edx live across the call to 0x00471530: the file holds FUN_00471530 as a static
   callee (pilot finding 1). */
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
void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)
typedef struct Sub45c {
    int field_0;               /* +0x000 */
    int field_4;               /* +0x004 */
    int field_8;               /* +0x008 */
    unsigned char padc[0x44c];
    int field_458;             /* +0x458 */
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
typedef struct Name84 {
    char name[0x80]; /* +0x00 */
    int value;       /* +0x80 */
} Name84;
MGLOBAL(RecTable, DAT_006905c8, 0x006905c8);
MGLOBALA(Name84, DAT_00690a30, 20, 0x00690a30);
MGLOBAL(int, DAT_00691480, 0x00691480);
MGLOBAL(int, DAT_00691484, 0x00691484);
MGLOBALA(Obj2510, DAT_0086ece0, 8, 0x0086ece0);
// MATCH: 0x00471530 FUN_00471530
static void FUN_00471530(void)
{
    int i;
    for (i = 0; i < 20; i++) {
        DAT_00690a30[i].name[0] = 0;
        DAT_00690a30[i].value = 0;
    }
    DAT_00691484 = 0;
}
// MATCH: 0x00471df0 FUN_00471df0
void FUN_00471df0(void)
{
    int i, j, k;
    Rec8c *r;
    j = 0;
    DAT_00691480 = 0;
    FUN_00471530();
    for (i = 0; i < 8; i++) {
        r = &DAT_006905c8.rec[i];
        r->prev = 0;
        r->count = 0;
        r->field_88 = 0;
        for (; j < 16; j++) {
            r->ptr[j] = 0;
            r->id[j] = -1;
        }
    }
    DAT_006905c8.cur = 0;
    for (i = 0; i < 8; i++) {
        memset(DAT_0086ece0[i].name, 0, sizeof(DAT_0086ece0[i].name));
        DAT_0086ece0[i].count = 0;
        DAT_0086ece0[i].nhandle = 0;
        memset(DAT_0086ece0[i].sub, 0, sizeof(DAT_0086ece0[i].sub));
        for (k = 0; k < 8; k++) {
            DAT_0086ece0[i].sub[k].field_0 = -1;
            DAT_0086ece0[i].sub[k].field_8 = -1;
            DAT_0086ece0[i].sub[k].field_4 = 0;
        }
        DAT_0086ece0[i].nsub = 0;
        memset(DAT_0086ece0[i].e, 0, sizeof(DAT_0086ece0[i].e));
        DAT_0086ece0[i].ne = 0;
    }
}
