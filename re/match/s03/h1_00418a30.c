// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00418a30 FUN_00418a30
// MATCH: 0x00418de0 FUN_00418de0
// MATCH: 0x00418e30 FUN_00418e30
// MATCH: 0x004190f0 FUN_004190f0
// MATCH: 0x00419480 FUN_00419480
// MATCH: 0x004194f0 FUN_004194f0
// MATCH: 0x00419760 FUN_00419760
// MATCH: 0x0041a060 FUN_0041a060
// MATCH: 0x0041a180 FUN_0041a180
// MATCH: 0x00418a00 FUN_00418a00
// MATCH: 0x00419090 FUN_00419090
/* h1 MATCH-S03 group file (static callees + their callers in one file, callees first).
   Records: 0x0063c018 x4 stride 0x6c (Rec63c018), 0x0063bda0 x4 stride 0x64 (Rec63bda0),
   0x0063bf30 x2 stride 0x54 (Rec63bf30). Register-arg statics (plain cl, /Ob1):
   0x00418a30 EAX=rec, 0x00418de0 EAX=rec, 0x004190f0 EAX=rec, 0x00419480 EAX=rec.
   0x00418a30 and 0x004194f0 are not on h1's list (claimed as by-products, byte-identical).
   Supersedes h1_00418e30.c, h1_004190f0.c, h1_00419480.c (round 1/2 single files). */
typedef struct Obj48 {
    char pad[0x48];
    void (*fn48)(struct Obj48 *self);
} Obj48;
typedef struct RwRGBA_ { unsigned char r, g, b, a; } RwRGBA_;
typedef struct Pair5f32b0 { int a, b; } Pair5f32b0;
typedef struct Rec63bda0 {
    char pad[0x40];
    int  atomic;   /* +0x40 */
    int  f44;   /* +0x44 */
    int  f48;   /* +0x48 */
    int  f4c;   /* +0x4c */
    int  f50;   /* +0x50 */
    int  f54;   /* +0x54 */
    int  f58;   /* +0x58 */
    int  f5c;   /* +0x5c */
    int  f60;   /* +0x60 */
} Rec63bda0;    /* 0x64 */
typedef struct Rec63c018 {
    Pair5f32b0 f00;     /* +0x00 */
    float f08;          /* +0x08 */
    float f0c;          /* +0x0c */
    RwRGBA_ col;        /* +0x10 */
    char pad14[0x34 - 0x14];
    int  state;         /* +0x34 */
    int  f38;           /* +0x38 */
    int  f3c;           /* +0x3c */
    int  f40;           /* +0x40 */
    int  f44;           /* +0x44 */
    char pad48[0x4c - 0x48];
    int  f4c;           /* +0x4c */
    int  kind;          /* +0x50 */
    int  f54;           /* +0x54 */
    char pad58[0x64 - 0x58];
    int  f64;           /* +0x64 */
    Rec63bda0 *mark;    /* +0x68 */
} Rec63c018;            /* 0x6c */
typedef struct Rec63bf30 {
    int    flag[6];     /* +0x00 */
    char   pad18[0x20 - 0x18];
    Obj48 *slot[8];     /* +0x20 */
    int    obj;         /* +0x40 */
    int    obj4;        /* +0x44 */
    int    idx;         /* +0x48 */
    int    f4c;         /* +0x4c */
    Rec63c018 *owner;   /* +0x50 */
} Rec63bf30;            /* 0x54 */
typedef struct Ent7f1a10 { int f0, f4, f8, fc; } Ent7f1a10;
#ifndef MATCH_PORT
extern Rec63c018  DAT_0063c018[4];
extern Rec63bda0  DAT_0063bda0[4];
extern Rec63bf30  DAT_0063bf30[2];
extern Pair5f32b0 DAT_005f32b0[];
extern RwRGBA_     DAT_005f3298[];
extern Ent7f1a10  DAT_007f1a10[];
void FUN_00413c70(int a, int b, int c);
void FUN_004661a0(int a, int b);
void FUN_00418e50(void);
void FUN_00419ff0(void);
void FUN_004b6520(void *p, int n);
void FUN_004b3fc0(int obj, int *out);
int  FUN_004b5190(int a, int b, int c);
void FUN_004b52c0(int a, unsigned int b, int c);
int  FUN_0042a5d0(const char *s, int a, int b);
int  FUN_0042a6b0(const char *s, int a, int b);
int  FUN_0040bb30(const char *name);
void FUN_004770c0(void *blk, unsigned int a, unsigned int b, int c);
void FUN_004c5c80(int txd);
int  FUN_004b3f90(int clump);
int  FUN_004e69a0(int atomic);
int  FUN_004c0b30(void);
int  FUN_004e7e30(int atomic, int frame);
extern char DAT_0063bfd8[];
int  FUN_0042fe50(void);
void FUN_00419fd0(void);
void FUN_00418e70(void);
void FUN_00476df0(void *blk);
extern int  DAT_0063bd98;
extern int  DAT_0063c608;
extern int  DAT_0063c60c;
#endif

static void FUN_00418a00(Rec63c018 *r)
{
    FUN_004b6520(r, 0x6c);
    r->f08 = 0.05f;
    r->f0c = 0.05f;
    r->col.r = 0xff;
    r->col.g = 0xff;
    r->col.b = 0xff;
    r->col.a = 0xff;
}

static void FUN_00418a30(Rec63c018 *r)
{
    r->f00 = DAT_005f32b0[r->kind];
    r->state = 0;
    r->f64 = -1;
    r->f38 = 0;
    r->f3c = 0;
    r->f40 = 0;
    r->f44 = 0;
    r->f4c = 0;
}

static void FUN_00418de0(Rec63bda0 *rec)
{
    rec->f44 = 0;
    rec->f48 = 0;
    rec->f58 = 0;
    rec->f5c = 0;
    rec->f50 = 0;
    rec->f54 = 0;
    FUN_00413c70(rec->f4c, 2, 0);
    FUN_004661a0(0x14, 0);
    FUN_004661a0(0x14, 1);
    FUN_004661a0(0x14, 2);
    FUN_004661a0(0x14, 3);
}

void FUN_00418e30(void)
{
    int i;
    for (i = 0; i < 4; i++)
        FUN_00418de0(&DAT_0063bda0[i]);
}

/* copy of the pilot's 0x00419090 (re/match/pilot/a6_00419090.c, byte-identical there), needed here
   because 0x0041a060 calls it with EAX/EBX register args */
static void FUN_00419090(Rec63bf30 *rec, int idx, int obj)
{
    int list[6];
    int i;
    FUN_004b3fc0(obj, list);
    rec->idx = idx;
    rec->obj = obj;
    rec->obj4 = *(int *)(obj + 4);
    FUN_004b6520(rec->slot, 0x20);
    i = 0;
    do {
        int a = list[i];
        rec->slot[FUN_004b5190(a, 0, 0)] = (Obj48 *)a;
        FUN_004b52c0(*(int *)(a + 0x18), 0x40, 1);
        i++;
    } while (i < 6);
}

static void FUN_004190f0(Rec63bf30 *rec)
{
    rec->f4c = 0;
    rec->owner = 0;
}

static void FUN_00419480(Rec63bf30 *rec)
{
    if (rec->owner != 0) {
        if (rec->flag[0] != 0)
            rec->slot[0]->fn48(rec->slot[0]);
        if (rec->flag[1] != 0)
            rec->slot[1]->fn48(rec->slot[1]);
        if (rec->flag[2] != 0)
            rec->slot[2]->fn48(rec->slot[2]);
        if (rec->flag[3] != 0)
            rec->slot[3]->fn48(rec->slot[3]);
        if (rec->flag[4] != 0)
            rec->slot[4]->fn48(rec->slot[4]);
        if (rec->flag[5] != 0)
            rec->slot[5]->fn48(rec->slot[5]);
    }
}

void FUN_004194f0(void)
{
    int i;
    for (i = 0; i < 4; i++)
        FUN_00418a30(&DAT_0063c018[i]);
    FUN_00418e30();
    FUN_00418e50();
    for (i = 0; i < 2; i++)
        FUN_004190f0(&DAT_0063bf30[i]);
}

void FUN_00419760(int i, int flag)
{
    Rec63c018 *r = &DAT_0063c018[i];
    int k;
    if (flag != 0) {
        FUN_00418a30(r);
        FUN_00418de0(r->mark);
        r->state = 2;
        r->col = DAT_005f3298[DAT_007f1a10[i].fc];
        r->f54 = DAT_007f1a10[i].f4;
        for (k = 0; k < 2; k++) {
            Rec63bf30 *p = &DAT_0063bf30[k];
            if (p->owner == 0) {
                p->owner = r;
                return;
            }
        }
    } else {
        FUN_00418a30(r);
    }
}

#ifndef MATCH_PORT
/* STAND-INS (not matched, not shipped) for two register-arg statics 0x0041a180 calls:
   0x004197e0 (ESI = Rec63c018*, h1, unfinished) and 0x00419100 (EAX = Rec63bf30*, h3's).
   Their real bodies must replace these in this file, defined before 0x0041a180. */
void FUN_stand_in_a(void);
static void FUN_004197e0(Rec63c018 *r)
{
    FUN_stand_in_a();
    r->state = 1;
    FUN_stand_in_a();
    r->f4c = 0;
}
static void FUN_00419100(Rec63bf30 *rec)
{
    if (rec->owner != 0) {
        if (rec->flag[0] != 0)
            rec->slot[0]->fn48(rec->slot[0]);
        if (rec->flag[1] != 0)
            rec->slot[1]->fn48(rec->slot[1]);
    }
}
#endif

void FUN_0041a180(void)
{
    int i;
    if (FUN_0042fe50() != 0) {
        for (i = 0; i < 4; i++) {
            if (DAT_0063c018[i].state != 0)
                FUN_004197e0(&DAT_0063c018[i]);
        }
        FUN_00419fd0();
        FUN_00476df0(DAT_0063bfd8);
        FUN_00418e70();
        for (i = 0; i < 2; i++)
            FUN_00419100(&DAT_0063bf30[i]);
    }
}

void FUN_0041a060(void)
{
    int i;
    int atomic;
    FUN_004770c0(DAT_0063bfd8, 0x80c, 8, FUN_0040bb30("shockwave"));
    DAT_0063bd98 = FUN_0040bb30("crosshair2");
    if (DAT_0063bd98 == 0)
        DAT_0063bd98 = FUN_0040bb30("crosshair");
    DAT_0063c608 = FUN_0042a5d0("exocet.dff", 0, 0);
    DAT_0063c60c = FUN_0042a6b0("airstrike", 0, 0);
    FUN_004c5c80(DAT_0063c60c);
    for (i = 0; i < 2; i++)
        FUN_00419090(&DAT_0063bf30[i], i, FUN_0042a5d0("target", 0, 0));
    atomic = FUN_004b3f90(DAT_0063c608);
    for (i = 0; i < 4; i++) {
        DAT_0063bda0[i].atomic = FUN_004e69a0(atomic);
        FUN_004e7e30(DAT_0063bda0[i].atomic, FUN_004c0b30());
        DAT_0063bda0[i].f60 = i;
    }
    for (i = 0; i < 4; i++) {
        Rec63c018 *r = &DAT_0063c018[i];
        FUN_00418a00(r);
        FUN_00418a30(r);
        r->kind = i;
        r->mark = &DAT_0063bda0[i];
    }
    FUN_00419ff0();
}

#ifndef MATCH_PORT
/* stand-in caller for 0x00419480, not matched, not shipped */
void caller_004194xx(void)
{
    int i;
    for (i = 0; i < 2; i++)
        FUN_00419480(&DAT_0063bf30[i]);
}
#endif
