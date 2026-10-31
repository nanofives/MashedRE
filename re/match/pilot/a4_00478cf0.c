// FLAGS: /O2 /GS /Ob1
/* a4 / 0x00478cf0 (824 B per table, real body 827 B to 0x0047902a): best 99.2% (regs 99.2, ops 99.2);
 * every compared instruction is identical, the 0.8% is the 2 trailing instructions.
 * Path: Ghidra raw 69.7%. Struct layout for both args + index loops: 69.7% (the first loop had the
 * clump load after the id test, and that one change moved register allocation for the WHOLE function:
 * `t` was kept in ebp instead of being reloaded from [esp+0x114]). Loading
 * `c = t->clumps[idx]` before `if (id != -1)` (the original loads it before the je): 98.0%, all
 * allocation fixed. `Ent *e = &d->ent[i]` so the induction pointer points at +0x2c, not +0x2e: 100% of
 * compared instructions.
 * Residual: (c) only. The function-table size (824) ends before `add esp,0x100; ret`, which we emit
 * identically. No (a). */
int FUN_004850b0(int);
void FUN_004853f0(int);
void FUN_004987b0(const char *fmt, ...);
int FUN_004b3fc0(void *clump, int *out);
void FUN_004b4270(void *world);
int FUN_004c5930(void *txd);
void *FUN_004e4800(void *clump);
int FUN_00546320(void *);
int FUN_00558d30(void *);
int FUN_00562560(void *);
int RpClumpDestroy(void *);
int RpWorldDestroy(void *);
int RpWorldRemoveLight(void *world, void *light);
int RwFrameRemoveChild(void *parent, void *child);

typedef struct Ent {
    signed char idx;    /* +0 */
    char pad;
    short id;           /* +2 */
} Ent;

typedef struct Desc {
    int f00;
    int nWorlds2;       /* +0x04 */
    int f08[5];
    int nWorldsA;       /* +0x1c */
    int nWorldsB;       /* +0x20 */
    int f24[2];
    Ent ent[16];        /* +0x2c */
    int nEnt;           /* +0x6c */
} Desc;

typedef struct Track {
    char pad0[0x10040];
    void *worlds2[16];  /* +0x10040 */
    void *world3;       /* +0x10080 */
    int pad84[3];
    void *worldsA[4];   /* +0x10090 */
    void *worldsB[4];   /* +0x100a0 */
    void *objs[16];     /* +0x100b0 */
    void *clumps8[8];   /* +0x100f0 */
    void *clumps[64];   /* +0x10110 */
    char pad210[0x105b0 - 0x10210];
    void *res8[8];      /* +0x105b0 */
    void *txd;          /* +0x105d0 */
    void *world;        /* +0x105d4 */
    void *clumpA;       /* +0x105d8 */
    void *clumpB;       /* +0x105dc */
    void *light1;       /* +0x105e0 */
    void *light2;       /* +0x105e4 */
    int pad5e8[2];
    void *res;          /* +0x105f0 */
    int pad5f4[9];
    void *w8[8];        /* +0x10618 */
} Track;

// MATCH: 0x00478cf0 _FUN_00478cf0
void FUN_00478cf0(Track *t, Desc *d)
{
    int list[64];
    int i, j, n;
    void *c, *parent;

    for (i = 0; i < d->nEnt; i++) {
        Ent *e = &d->ent[i];
        c = t->clumps[e->idx];
        if (e->id != -1) {
            n = FUN_004b3fc0(c, list);
            for (j = 0; j < n; j++)
                FUN_004853f0(FUN_004850b0(list[j]));
        }
    }
    for (i = 0; i < d->nWorldsA; i++) {
        if (t->worldsA[i]) {
            RpWorldDestroy(t->worldsA[i]);
            t->worldsA[i] = 0;
        }
    }
    for (i = 0; i < d->nWorldsB; i++) {
        if (t->worldsB[i]) {
            RpWorldDestroy(t->worldsB[i]);
            t->worldsB[i] = 0;
        }
    }
    for (i = 0; i < 16; i++) {
        if (t->objs[i]) {
            FUN_00546320(t->objs[i]);
            t->objs[i] = 0;
        }
    }
    for (i = 0; i < 8; i++) {
        if (t->clumps8[i]) {
            RpClumpDestroy(t->clumps8[i]);
            t->clumps8[i] = 0;
        }
    }
    if (t->clumpA)
        RpClumpDestroy(t->clumpA);
    if (t->light1)
        RpWorldRemoveLight(t->world, t->light1);
    if (t->light2)
        RpWorldRemoveLight(t->world, t->light2);
    if (t->clumpB)
        RpClumpDestroy(t->clumpB);
    for (i = 0; i < 64; i++) {
        c = t->clumps[i];
        if (c) {
            parent = FUN_004e4800(c);
            if (parent)
                RwFrameRemoveChild(parent, c);
            RpClumpDestroy(c);
        }
    }
    if (t->res)
        FUN_00562560(t->res);
    for (i = 0; i < 8; i++) {
        if (t->res8[i])
            FUN_00562560(t->res8[i]);
    }
    if (t->world) {
        FUN_004b4270(t->world);
        RpWorldDestroy(t->world);
        t->world = 0;
        FUN_004987b0("World Destroyed\n");
    }
    for (i = 0; i < d->nWorlds2; i++)
        RpWorldDestroy(t->worlds2[i]);
    RpWorldDestroy(t->world3);
    for (i = 0; i < 8; i++) {
        if (t->w8[i]) {
            FUN_00558d30(t->w8[i]);
            t->w8[i] = 0;
        }
    }
    if (t->txd) {
        FUN_004c5930(t->txd);
        FUN_004987b0("TextureDictionary Destroyed\n");
    }
}
