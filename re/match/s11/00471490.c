// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00471490 (h6): 100% (regs 100%), first try with the struct. Same RecTable struct as 0x00471450 (cur at 0x006905c8, 8 records of
   0x8c from 0x006905cc). Entry table at 0x0086ece0, stride 0x2510. */
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
typedef struct Rec8c {
    void *ptr[16];   /* +0x00 */
    int id[16];      /* +0x40 */
    int count;       /* +0x80 */
    int prev;        /* +0x84 */
    int field_88;    /* +0x88 */
} Rec8c;
typedef struct RecTable {
    int cur;         /* 0x006905c8 */
    Rec8c rec[8];    /* 0x006905cc */
} RecTable;
typedef struct Obj2510 { unsigned char b[0x2510]; } Obj2510;
MGLOBAL(RecTable, DAT_006905c8, 0x006905c8);
MGLOBALA(Obj2510, DAT_0086ece0, 8, 0x0086ece0);
// MATCH: 0x00471490 FUN_00471490
int FUN_00471490(int param_1)
{
    Rec8c *r = &DAT_006905c8.rec[DAT_006905c8.cur];
    if (param_1 >= 2000) {
        r->id[r->count - 1] = param_1;
        return 1;
    }
    r->id[r->count] = -1;
    r->ptr[r->count] = &DAT_0086ece0[param_1];
    r->count++;
    return 1;
}
