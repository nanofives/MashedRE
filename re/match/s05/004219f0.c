// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x004219f0: by hand, 100% (regs 100%) for all three functions, no residual. Record layout 4 x 0x208 at 0x0063fb90 (two 0x100 groups of ten 0x18 entries, bit mask at +0xf0, count at +0xf4); the signed jl end test comes from an index loop.
   This file holds the static callees 0x00421980 (atomic in EAX) and 0x004219a0
   (slot entry in EAX, tail-jumps to 0x00421980), defined before their caller. */
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
typedef struct Ent004219f0 { int a0, a4, a8; int pc; int atomic; int a14; } Ent004219f0;
typedef struct Grp004219f0 { Ent004219f0 e[10]; unsigned char bits[4]; int count; char pad[8]; } Grp004219f0;
typedef struct Rec004219f0 { Grp004219f0 g[2]; char pad[8]; } Rec004219f0;
MGLOBAL(int, DAT_006ce274, 0x006ce274);
MGLOBALA(Rec004219f0, DAT_0063fb90, 4, 0x0063fb90);
int FUN_0055dec0(int p0);
void FUN_00559ee0(int p0, int p1);
float FUN_004b4510(float p0, float p1);
void FUN_00487020(int p0, float p1);

// MATCH: 0x00421980 _FUN_00421980
static void FUN_00421980(int atomic)
{
    FUN_00559ee0(FUN_0055dec0(DAT_006ce274), atomic);
}

// MATCH: 0x004219a0 _FUN_004219a0
static void FUN_004219a0(Ent004219f0 *e)
{
    if (e->atomic != 0 && *(int *)(e->atomic + 0x24) != 0)
        FUN_00421980(e->atomic);
}

// MATCH: 0x004219f0 _FUN_004219f0
void FUN_004219f0(int atomic)
{
    int r, j, i;
    for (r = 0; r < 4; r++) {
        for (j = 0; j < 2; j++) {
            Grp004219f0 *g = &DAT_0063fb90[r].g[j];
            if (g->count != 0) {
                for (i = 0; i < 10; i++) {
                    Ent004219f0 *e = &g->e[i];
                    if (e->atomic == atomic) {
                        g->bits[i >> 3] &= ~(1 << (i & 7));
                        FUN_004219a0(e);
                        FUN_00487020(*(int *)(e->pc + 4) + 0x40, FUN_004b4510(0.5f, 0.7f));
                        return;
                    }
                }
            }
        }
    }
}
