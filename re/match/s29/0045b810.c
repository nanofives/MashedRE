// FLAGS: /O2 /GS /Ob1
/* MATCH-S29 (POWERUPINIT-B) 0x0045b810: the tail target of the 0x0045b830 init and of 0x0045b920. Static callee
   0x0045b360 (ESI) copied from re/match/s09/0045b360.c, where it is C4. */
/* ---- MATCH-S29 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct Slot45b360 {
    int a[3];
    int m_c;
    int m_10;
    int m_14;
    int m_18;
    int m_frame;
    int m_20;
} Slot45b360;
MGLOBALA(Slot45b360, DAT_0068d158, 4, 0x0068d158);

int FUN_004e4800(int frame);
int FUN_004e45b0(int parent, int frame);

/* 0x0045b360 FUN_0045b360: matched in its own s09 source; here only so the caller's register passing is the original's */
static void FUN_0045b360(Slot45b360 *p)
{
    int parent;

    p->m_c = 0;
    p->m_10 = 0;
    p->m_14 = 0;
    p->m_18 = 0;
    parent = FUN_004e4800(p->m_frame);
    if (parent != 0) {
        FUN_004e45b0(parent, p->m_frame);
    }
}

// MATCH: 0x0045b810 FUN_0045b810
void FUN_0045b810(void)
{
    Slot45b360 *p;

    for (p = DAT_0068d158; (int)p < (int)(DAT_0068d158 + 4); p++) {
        FUN_0045b360(p);
    }
}
