// FLAGS: /O2 /GS /Ob1
// MATCH: 0x0041ad60 FUN_0041ad60
// MATCH: 0x0041adb0 FUN_0041adb0
// MATCH: 0x0041ae20 FUN_0041ae20
// MATCH: 0x0041ae60 FUN_0041ae60
// MATCH: 0x0041af00 FUN_0041af00
// MATCH: 0x0041b340 FUN_0041b340
// MATCH: 0x0041b440 FUN_0041b440
// MATCH: 0x0041b450 FUN_0041b450
// MATCH: 0x0041b4d0 FUN_0041b4d0
// MATCH: 0x0041b520 FUN_0041b520
// MATCH: 0x0041b630 FUN_0041b630
// MATCH: 0x0041b660 FUN_0041b660
/* h1 MATCH-S03 group file: the 4 panel records at 0x0063c8d0 (stride 0x74, Rec63c8d0) with their
   register-arg statics, each defined before its caller:
     0x0041ad60 EBX=rec EAX=clump (caller 0x0041b450)   0x0041adb0 ESI=rec (caller 0x0041b4d0)
     0x0041ae20 ECX=rec (caller 0x0041b520)              0x0041b340 EAX=rec (caller 0x0041b630)
     0x0041b440 EAX=rec (caller 0x0041b660)
     0x0041ae60 / 0x0041af00 ESI=rec EDI=matrix: real caller 0x0041af50 is h3's; caller_0041af50 below is
     a stand-in (not matched, not shipped). h3's 0x0041af50 must live in this file to call them.
   0x0041b4d0 / 0x0041b630 / 0x0041b660 are not on h1's list (by-products).
   Also carries 0x0041ae20/0x0041b520 (same bodies as the committed h1_0041ae20.c) and 0x0041b440 (was
   h1_0041b440.c). */
typedef struct Obj48 {
    char pad[0x48];
    void (*fn48)(struct Obj48 *self);
} Obj48;
typedef struct RwRGBA_ { unsigned char r, g, b, a; } RwRGBA_;
typedef struct Vec3_41ae20 { float x, y, z; } Vec3_41ae20;
typedef struct Rec63c8d0 {
    Obj48 *slot[0x14];   /* +0x00, 0x50 bytes cleared by FUN_004b6520 */
    Vec3_41ae20 v50;     /* +0x50 */
    int    clump;        /* +0x5c */
    int    clump4;       /* +0x60 */
    int    idx64;        /* +0x64 */
    unsigned int flags;  /* +0x68 */
    int    visible;      /* +0x6c */
    int    i70;          /* +0x70 */
} Rec63c8d0;             /* 0x74 */
typedef struct Ent7f1a10 { int f0, f4, f8, fc; } Ent7f1a10;
#ifndef MATCH_PORT
extern Rec63c8d0   DAT_0063c8d0[4];
extern Vec3_41ae20 DAT_005f3304[];
extern Ent7f1a10   DAT_007f1a10[];
extern int DAT_0063c888;
extern int DAT_0063c89c;
extern int DAT_0063c8c4;
void FUN_004b3fc0(int clump, Obj48 **out);
int  FUN_004b5190(Obj48 *a, int b, int c);
void FUN_004b6520(void *p, int n);
void FUN_004b5260(Obj48 *obj, RwRGBA_ *c);
void FUN_004c13e0(int frame, void *m, int mode);
int  FUN_0042a5d0(const char *name, int a, int b);
int  FUN_0042a6b0(const char *name, int a, int b);
void FUN_004c5c80(int txd);
void FUN_004c5930(int txd);
int  RpClumpDestroy(int clump);
int  FUN_0040e340(void);
int  FUN_0040b890(void);
#endif

static void FUN_0041ad60(Rec63c8d0 *rec, int obj)
{
    Obj48 *list[17];
    int i;
    FUN_004b3fc0(obj, list);
    rec->clump = obj;
    rec->clump4 = *(int *)(obj + 4);
    FUN_004b6520(rec->slot, 0x50);
    i = 0;
    do {
        Obj48 *a = list[i];
        rec->slot[FUN_004b5190(a, 0, 0)] = a;
        i++;
    } while (i < 17);
}

static void FUN_0041adb0(Rec63c8d0 *rec)
{
    RwRGBA_ c;
    c.r = 0x32;
    c.g = 0x32;
    c.b = 0x32;
    c.a = 0xff;
    rec->flags = 0;
    rec->visible = 0;
    rec->flags = 1 << DAT_007f1a10[rec->idx64].fc;
    FUN_004b5260(rec->slot[5], &c);
    FUN_004b5260(rec->slot[2], &c);
    if (rec->idx64 < DAT_0063c89c)
        rec->visible = 1;
}

static void FUN_0041ae20(Rec63c8d0 *rec)
{
    rec->v50 = DAT_005f3304[rec->idx64];
    rec->i70 = 0;
    rec->v50.x = rec->v50.x + rec->v50.x;
}

static void FUN_0041ae60(Rec63c8d0 *rec, void *m)
{
    if (rec->flags & 1)
        FUN_004c13e0(*(int *)((char *)rec->slot[7] + 4), m, 0);
    if (rec->flags & 2)
        FUN_004c13e0(*(int *)((char *)rec->slot[8] + 4), m, 0);
    if (rec->flags & 4)
        FUN_004c13e0(*(int *)((char *)rec->slot[9] + 4), m, 0);
    if (rec->flags & 8)
        FUN_004c13e0(*(int *)((char *)rec->slot[10] + 4), m, 0);
    if (rec->flags & 0x10)
        FUN_004c13e0(*(int *)((char *)rec->slot[11] + 4), m, 0);
    if (rec->flags & 0x20)
        FUN_004c13e0(*(int *)((char *)rec->slot[12] + 4), m, 0);
}

static void FUN_0041af00(Rec63c8d0 *rec, void *m)
{
    if (rec->flags & 0x2000)
        FUN_004c13e0(*(int *)((char *)rec->slot[13] + 4), m, 0);
    if (rec->flags & 0x4000)
        FUN_004c13e0(*(int *)((char *)rec->slot[14] + 4), m, 0);
    if (rec->flags & 0x8000)
        FUN_004c13e0(*(int *)((char *)rec->slot[15] + 4), m, 0);
}

static void FUN_0041b340(Rec63c8d0 *rec)
{
    rec->slot[16]->fn48(rec->slot[16]);
    rec->slot[0]->fn48(rec->slot[0]);
    if (rec->flags & 0x800) {
        rec->slot[5]->fn48(rec->slot[5]);
        rec->slot[4]->fn48(rec->slot[4]);
        if (rec->flags & 0x40)
            rec->slot[6]->fn48(rec->slot[6]);
    } else if (rec->flags & 0x1000) {
        rec->slot[2]->fn48(rec->slot[2]);
        rec->slot[1]->fn48(rec->slot[1]);
        if (rec->flags & 0x40)
            rec->slot[3]->fn48(rec->slot[3]);
    }
    if (rec->flags & 1)
        rec->slot[7]->fn48(rec->slot[7]);
    if (rec->flags & 2)
        rec->slot[8]->fn48(rec->slot[8]);
    if (rec->flags & 4)
        rec->slot[9]->fn48(rec->slot[9]);
    if (rec->flags & 8)
        rec->slot[10]->fn48(rec->slot[10]);
    if (rec->flags & 0x10)
        rec->slot[11]->fn48(rec->slot[11]);
    if (rec->flags & 0x20)
        rec->slot[12]->fn48(rec->slot[12]);
    if (rec->flags & 0x2000)
        rec->slot[13]->fn48(rec->slot[13]);
    if (rec->flags & 0x4000)
        rec->slot[14]->fn48(rec->slot[14]);
    if (rec->flags & 0x8000)
        rec->slot[15]->fn48(rec->slot[15]);
}

static void FUN_0041b440(Rec63c8d0 *rec)
{
    RpClumpDestroy(rec->clump);
}

void FUN_0041b450(void)
{
    const char *names[4];
    int i;
    names[0] = "panel0";
    names[1] = "panel1";
    names[2] = "panel2";
    names[3] = "panel3";
    DAT_0063c888 = FUN_0042a6b0("panel.txd", 0, 0);
    FUN_004c5c80(0);
    for (i = 0; i < 4; i++) {
        Rec63c8d0 *r = &DAT_0063c8d0[i];
        FUN_0041ad60(r, FUN_0042a5d0(names[i], 0, 0));
        r->idx64 = i;
    }
}

void FUN_0041b4d0(void)
{
    int i;
    DAT_0063c89c = FUN_0040e340();
    DAT_0063c8c4 = FUN_0040b890();
    for (i = 0; i < 4; i++)
        FUN_0041adb0(&DAT_0063c8d0[i]);
}

void FUN_0041b520(void)
{
    int i;
    for (i = 0; i < 4; i++)
        FUN_0041ae20(&DAT_0063c8d0[i]);
}

void FUN_0041b630(void)
{
    int i;
    for (i = 0; i < 4; i++) {
        if (DAT_0063c8d0[i].visible != 0)
            FUN_0041b340(&DAT_0063c8d0[i]);
    }
}

void FUN_0041b660(void)
{
    int i;
    for (i = 0; i < 4; i++)
        FUN_0041b440(&DAT_0063c8d0[i]);
    FUN_004c5930(DAT_0063c888);
}

#ifndef MATCH_PORT
/* stand-in for h3's 0x0041af50 (not matched, not shipped): keeps rec in ESI, matrix in EDI */
extern char DAT_stand_in_mat[0x40];
void caller_0041af50(void)
{
    int i;
    for (i = 0; i < 4; i++) {
        FUN_0041ae60(&DAT_0063c8d0[i], DAT_stand_in_mat);
        FUN_0041af00(&DAT_0063c8d0[i], DAT_stand_in_mat);
    }
}
#endif
