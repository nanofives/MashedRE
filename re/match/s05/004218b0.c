// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x004218b0 (31 B): 100.0% (regs 100, ops 100). Same file text as 004218d0.c (already in s05, by
   hand; copied unchanged below the header): this file holds 0x004218b0 (first argument in EAX, second on the
   stack), `static` and defined before its caller 0x004218d0 (pilot finding 1). Both 100%. Residual: none. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
int *FUN_0057c500(void);
void FUN_0057c550(int *p0);
int FUN_0057c300(int p0);
void FUN_0057c220(int p0, int p1);
void FUN_0055c540(int p0, float p1);
void FUN_0055c4f0(int p0, int p1);
void FUN_0055c4a0(int p0, float p1);
float FUN_0055b940(int *p0, float p1, int p2);

// MATCH: 0x004218b0 _FUN_004218b0
static int FUN_004218b0(int *f, int p1)
{
    int r = FUN_0057c300((int)f);
    *(unsigned int *)(r + 8) &= 0xfffffff3;
    FUN_0057c220(p1, r);
    return r;
}

// MATCH: 0x004218d0 _FUN_004218d0
int FUN_004218d0(int p1, float p2, float p3, float p4, int p5)
{
    int *f = FUN_0057c500();
    int g = *f;
    int r;
    FUN_0055c540(g, p2);
    FUN_0055c4f0(g, p5);
    FUN_0055c4a0(g, p4);
    FUN_0055b940(f, p3, 0);
    r = FUN_004218b0(f, p1);
    FUN_0057c550(f);
    return r;
}
