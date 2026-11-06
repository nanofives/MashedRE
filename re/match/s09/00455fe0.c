// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00455fe0 (h4): 100% (regs 100%), 30/30 ins on the first hand pass. Raw converter 80.0%.
   The record pointer is ESI-passed (pilot finding 1). Its real caller 0x00456c70 is matched separately
   (raw converter file 00456c70.c, left untouched), so this file holds a stand-in copy of that loop
   (4 records of 0x48 B at 0x006886b0) built only outside MATCH_PORT, no MATCH line on it.
   0x0045bfd0 takes no argument: the pushed clump is 0x004e45b0's second argument. Residual: none. */
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
typedef struct Rec00455fe0 {
    int f00, f04;
    void *clump;        /* +0x08 */
    int f0c, f10, f14, f18, f1c, f20, f24, f28;
    int pad2c[7];
} Rec00455fe0;          /* 0x48, 4 records at 0x006886b0 */
MGLOBALA(Rec00455fe0, DAT_006886b0, 4, 0x006886b0);
void FUN_00474d60(void *p0, int p1);
void *FUN_004e4800(void *p0);
void *FUN_004e45b0(void *p0, void *p1);
void *FUN_0045bfd0(void);
// MATCH: 0x00455fe0 FUN_00455fe0
static void FUN_00455fe0(Rec00455fe0 *r)
{
    r->f00 = 0;
    if (FUN_004e4800(r->clump) != 0)
        FUN_004e45b0(FUN_0045bfd0(), r->clump);
    FUN_00474d60(r->clump, 0);
    r->f0c = 0;
    r->f14 = 0;
    r->f18 = 0;
    r->f1c = 0;
    r->f28 = 0;
    r->f24 = 0;
    r->f20 = 0;
    r->f10 = 0x32;
}

#ifndef MATCH_PORT  /* stand-in caller: the loop of 0x00456c70 (matched separately in 00456c70.c) */
void StandIn_00456c70(void)
{
    Rec00455fe0 *r;
    for (r = DAT_006886b0; (int)r < (int)(DAT_006886b0 + 4); r++)
        FUN_00455fe0(r);
}
#endif
