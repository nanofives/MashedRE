// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x004222d0: by hand, 100% (regs 100%) for all five functions, no residual. Needed in 0x00422160: the count != 0 branch first, and i & 7 into a local before the mask byte pointer.
   This file holds the static callees 0x00422290 (record in EAX), 0x00422160 (group
   in EBX), 0x004219a0 (entry in EAX) and 0x00421980 (atomic in EAX), each defined before its caller.
   0x004219a0 and 0x00421980 are copies of the ones in 004219f0.c. */
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
typedef struct V3d004222d0 { float x, y, z; } V3d004222d0;
typedef struct Ent004222d0 { int a0, a4, a8; int clump; int obj; int vis; } Ent004222d0;
struct Rec004222d0;
typedef struct Grp004222d0 { Ent004222d0 e[10]; unsigned char bits[4]; int count; int set; struct Rec004222d0 *rec; } Grp004222d0;
typedef struct Rec004222d0 { Grp004222d0 g[2]; int model; int idx; } Rec004222d0;
MGLOBAL(int, DAT_006ce274, 0x006ce274);
MGLOBAL(int, DAT_006403b0, 0x006403b0);
MGLOBALA(Rec004222d0, DAT_0063fb90, 4, 0x0063fb90);
int FUN_0055dec0(int p0);
void FUN_00559ee0(int p0, int p1);
int FUN_00420da0(int p0, int p1);
int FUN_00420dc0(int p0, int p1);
void FUN_00421d20(Grp004222d0 *p0);
void FUN_00465c10(int p0, int p1);
void FUN_004c1340(int p0, V3d004222d0 *p1, int p2);

// MATCH: 0x00421980 _FUN_00421980
static void FUN_00421980(int atomic)
{
    FUN_00559ee0(FUN_0055dec0(DAT_006ce274), atomic);
}

// MATCH: 0x004219a0 _FUN_004219a0
static void FUN_004219a0(Ent004222d0 *e)
{
    if (e->obj != 0 && *(int *)(e->obj + 0x24) != 0)
        FUN_00421980(e->obj);
}

// MATCH: 0x00422160 _FUN_00422160
static void FUN_00422160(Grp004222d0 *g)
{
    int i;
    if (g->count != 0) {
        if (DAT_006403b0 != 0) {
            for (i = 0; i < 10; i++) {
                Ent004222d0 *e = &g->e[i];
                int sh = i & 7;
                unsigned char *b = &g->bits[i >> 3];
                if ((*b & (1 << sh)) && e->vis != 0) {
                    V3d004222d0 v;
                    v.x = 0;
                    v.y = 0;
                    v.z = 0;
                    *b &= ~(1 << sh);
                    FUN_004219a0(e);
                    FUN_004c1340(*(int *)(e->clump + 4), &v, 0);
                }
            }
        }
    } else {
        if (g->set == 0) {
            if (FUN_00420da0(g->rec->idx, 0) != 0) {
                FUN_00421d20(g);
                FUN_00465c10(0x21, g->rec->idx);
            }
        } else if (g->set == 1) {
            if (FUN_00420dc0(g->rec->idx, 0) != 0) {
                FUN_00421d20(g);
                FUN_00465c10(0x21, g->rec->idx);
            }
        }
    }
}

// MATCH: 0x00422290 _FUN_00422290
static void FUN_00422290(Rec004222d0 *rec)
{
    int j;
    for (j = 0; j < 2; j++)
        FUN_00422160(&rec->g[j]);
}

// MATCH: 0x004222d0 _FUN_004222d0
void FUN_004222d0(void)
{
    int i;
    for (i = 0; i < 4; i++)
        FUN_00422290(&DAT_0063fb90[i]);
}
