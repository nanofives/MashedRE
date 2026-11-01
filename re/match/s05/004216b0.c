// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x004216b0 (97 B): 100.0% (regs 100, ops 100). Same file text as 004219c0.c (already in s05, by
   hand; copied unchanged below the header): this file holds 0x004216b0 (group record in EBX, Ghidra unaff_EBX) and
   0x00421720 (group pair in EAX, kept in EBX across its 2-step loop), both `static` and defined before their caller
   0x004219c0 (pilot finding 1). All three 100%. Residual: none. */
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
typedef struct Ent004219c0 { int a0, a4, a8; int atomic; int a10; int vis; } Ent004219c0;
typedef struct Grp004219c0 { Ent004219c0 e[10]; unsigned char bits[4]; int count; char pad[8]; } Grp004219c0;
typedef struct Rec004219c0 { Grp004219c0 g[2]; char pad[8]; } Rec004219c0;
typedef void (*AtomicCb004219c0)(int);
MGLOBAL(int *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_006403b0, 0x006403b0);
MGLOBALA(Rec004219c0, DAT_0063fb90, 4, 0x0063fb90);
int FUN_004e6100(int p0);
int FUN_004c1b40(int p0, int p1);

// MATCH: 0x004216b0 _FUN_004216b0
static void FUN_004216b0(Grp004219c0 *g)
{
    int i;
    if (g->count != 0) {
        for (i = 0; i < 10; i++) {
            Ent004219c0 *e = &g->e[i];
            if (g->bits[i >> 3] & (1 << (i & 7))) {
                e->vis = FUN_004c1b40(*DAT_007d3ff8, FUN_004e6100(e->atomic)) == 0;
                (*(AtomicCb004219c0 *)(e->atomic + 0x48))(e->atomic);
            }
        }
    }
}

// MATCH: 0x00421720 _FUN_00421720
static void FUN_00421720(Rec004219c0 *r)
{
    int j;
    for (j = 0; j < 2; j++)
        FUN_004216b0(&r->g[j]);
}

// MATCH: 0x004219c0 _FUN_004219c0
void FUN_004219c0(void)
{
    int r;
    DAT_006403b0 = *DAT_007d3ff8;
    for (r = 0; r < 4; r++)
        FUN_00421720(&DAT_0063fb90[r]);
}
