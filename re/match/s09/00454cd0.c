// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00454cd0 (h4): 100% (regs 100%), 60/60 ins. Raw converter 23.3%. The record pointer is
   ESI-passed (pilot finding 1): the file holds a stand-in caller (the 0x18-B record loop of 0x00454e00,
   4 records from 0x006882f8, a function missing from pc_funcs.csv); no MATCH line on it, built only outside MATCH_PORT.
   What moved it: switch on the state, the position copied as a 12-B struct then .y adjusted on the copy,
   and the .rdata constants written as float literals (10.0f 0x005cc55c, 0.1f 0x005cc56c, 4.0f 0x005cc35c,
   2.0f 0x005cc574): with MGLOBAL constants the fmul operand order flipped (91.7%). Residual: none. */
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
typedef struct V3_00454cd0 { float x, y, z; } V3_00454cd0;
typedef struct Rec00454cd0 {
    V3_00454cd0 pos;  /* +0x00 */
    int state;        /* +0x0c */
    float t;          /* +0x10 */
    float t2;         /* +0x14 */
} Rec00454cd0;        /* 0x18, 4 records at 0x006882f8 */
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBALA(Rec00454cd0, DAT_006882f8, 4, 0x006882f8);
MGLOBALA(uchar, DAT_0088fec0, 4, 0x0088fec0);
void FUN_004769a0(float *p0);
void FUN_004769d0(float *p0);
void FUN_004769f0(uchar *p0);
void FUN_00476d00(void *p0);
void FUN_00476df0(void *p0);
// MATCH: 0x00454cd0 FUN_00454cd0
static void FUN_00454cd0(Rec00454cd0 *r)
{
    uchar col[4];
    float sc[2];
    V3_00454cd0 pos;
    float s;
    switch (r->state) {
    case 1:
        s = 10.0f * r->t;
        col[0] = 0xff;
        col[1] = 0xff;
        col[2] = 0xff;
        col[3] = 0xff;
        r->t = DAT_007f100c + r->t;
        pos = r->pos;
        pos.y = 0.1f + pos.y;
        sc[1] = 4.0f * s;
        sc[0] = sc[1];
        FUN_004769a0(&pos.x);
        FUN_004769f0(col);
        FUN_004769d0(sc);
        FUN_00476d00(DAT_0088fec0);
        if (r->t >= 0.1f) {
            r->state = 2;
            r->t2 = 0.0f;
        }
        break;
    case 2:
        r->t2 = DAT_007f100c + r->t2;
        if (r->t2 >= 2.0f)
            r->state = 0;
        break;
    }
}

#ifndef MATCH_PORT
/* stand-in caller: the loop of 0x00454e00 (not in pc_funcs.csv) */
void StandIn_00454cd0(void)
{
    Rec00454cd0 *r;
    for (r = DAT_006882f8; r < DAT_006882f8 + 4; r++)
        FUN_00454cd0(r);
    FUN_00476df0(DAT_0088fec0);
}
#endif
