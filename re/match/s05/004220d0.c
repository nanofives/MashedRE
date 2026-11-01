// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x004220d0: by hand, 100% (regs 100%) for 0x004220d0 and 0x00421ba0; 0x00421a90 is 95.3% only because Ghidra's size stops before its epilogue: match.py --bytes reports the whole obj symbol BYTE-IDENTICAL. Needed: the model handle and the name pointer in their own locals (merged add esp and EAX).
   This file holds the static callees 0x00421a90 (j in EAX, record in ECX, group in
   EBX, name list on the stack) and 0x00421ba0 (model in EAX, record in EDI, index on the stack), defined before
   their callers. */
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
typedef struct Ent004220d0 { int a0, a4, a8; int clump; int obj; int vis; } Ent004220d0;
struct Rec004220d0;
typedef struct Grp004220d0 { Ent004220d0 e[10]; unsigned char bits[4]; int count; int set; struct Rec004220d0 *rec; } Grp004220d0;
typedef struct Rec004220d0 { Grp004220d0 g[2]; int model; int idx; } Rec004220d0;
MGLOBALA(Rec004220d0, DAT_0063fb90, 4, 0x0063fb90);
MGLOBALA(char *, PTR_s_debris0_dff_005f6124, 6, 0x005f6124);
int FUN_004b5190(int p0, int p1, int p2);
void FUN_004c1210(int p0);
void FUN_004e5fc0(int p0);
int FUN_004218d0(int p1, float p2, float p3, float p4, int p5);
void FUN_0055dff0(int p0, int p1);
int FUN_00421930(int p1);
void FUN_004b6520(void *p0, int p1);
void FUN_004b3fc0(int p0, int *p1);
int FUN_0042a5d0(char *p0, int p1, int p2);

// MATCH: 0x00421a90 _FUN_00421a90
static void FUN_00421a90(int set, Rec004220d0 *rec, int *list, Grp004220d0 *g)
{
    int i;
    g->set = set;
    g->rec = rec;
    for (i = 0; i < 20; i++) {
        int a = list[i];
        int k = 0;
        if (a != 0) {
            int t = FUN_004b5190(a, 0, 0);
            Ent004220d0 *e;
            int f;
            float s;
            if (t >= 0x50 && t <= 0x59 && g->set != 0)
                continue;
            if (t >= 0x5a && t <= 0x63 && g->set != 1)
                continue;
            if (g->set == 0)
                k = t - 0x50;
            else if (g->set == 1)
                k = t - 0x5a;
            e = &g->e[k];
            e->clump = a;
            FUN_004c1210(*(int *)(a + 4));
            if (*(unsigned char *)(a + 0x4c) & 2)
                FUN_004e5fc0(a);
            s = *(float *)(a + 0x28) * 0.5f;
            f = FUN_004218d0(a, s, 0.1f, 1.0f, 0);
            *(int *)(*(int *)(f + 0x10) + 8) = *(int *)(a + 4) + 0x10;
            FUN_0055dff0(f, 0);
            e->obj = f;
            FUN_00421930(a);
        }
    }
}

// MATCH: 0x00421ba0 _FUN_00421ba0
static void FUN_00421ba0(int model, Rec004220d0 *rec, int idx)
{
    int list[20];
    int j;
    FUN_004b6520(rec, 0x208);
    FUN_004b6520(list, 0x50);
    rec->idx = idx;
    rec->model = model;
    FUN_004b3fc0(model, list);
    for (j = 0; j < 2; j++)
        FUN_00421a90(j, rec, list, &rec->g[j]);
}

// MATCH: 0x004220d0 _FUN_004220d0
void FUN_004220d0(int *types)
{
    int i;
    for (i = 0; i < 4; i++) {
        int k, m; char *n;
        if (types != 0)
            k = types[i] % 6;
        else
            k = 0;
        n = PTR_s_debris0_dff_005f6124[k];
        m = FUN_0042a5d0(n, 0, 0);
        FUN_00421ba0(m, &DAT_0063fb90[i], i);
    }
}
