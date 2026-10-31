// FLAGS: /O2 /GS /Ob1
/* a6 0x00408610 (1066 B Ghidra): best 95.8% (bad32/1073), g2s 31.2%.
   Layout found: record array DAT_008a9620[], stride 0x30c (struct Rec30c below; 0x008a9640 = +0x20 cur,
   +0xc8 float pos, +0xf0 char flags[0x200], +0x2f4 int a6[6]); DAT_008a95c0 = 2 x 0x28-byte result records
   written by FUN_00407e20. DAT_005d757c is the 0.0f literal constant, 0x5cc94c = 2^32 (unsigned->float
   fix-up emitted by the compiler for `(float)unsigned`), 0x5cca58 = 3000000.0f, 0x5cca54 = 99.0f.
   Steps: Out28.c8 is float (+11%), index the result array directly instead of via a pointer (+6%),
   ctx.pos copied x,y,z from a V3* temp (+1%), permuter: DAT_0063a5e8 store and the rec->e4 store moved (+1.8%).
   Residual: register choice only. `which` lives in edx (original ecx) and the FUN_00407e20 argument loads use
   ecx/edx (original eax/ecx). Plus the epilogue (pop esi/edi/ebx; add esp; ret) lies past Ghidra's size:
   class (c) for those 5 instructions. Register residual class (b): not found in 903 permuter compiles. */
typedef struct { float x, y, z; } V3;
typedef struct {
    unsigned int t0;    /* +0x000 */
    int pad04;
    int i08;            /* +0x008 */
    char pad0c[0x10];
    unsigned int t1c;   /* +0x01c */
    int cur;            /* +0x020 */
    int prev;           /* +0x024 */
    int n28;            /* +0x028 */
    int arr[30];        /* +0x02c */
    char pada4[0x24];
    float pos;          /* +0x0c8 */
    float fcc;          /* +0x0cc */
    int d0;             /* +0x0d0 */
    int d4;             /* +0x0d4 */
    int d8;             /* +0x0d8 */
    int dc;             /* +0x0dc */
    int e0;             /* +0x0e0 */
    int e4;             /* +0x0e4 */
    int e8;             /* +0x0e8 */
    int ec;             /* +0x0ec */
    char flags[0x200];  /* +0x0f0 */
    int i2f0;           /* +0x2f0 */
    int a6[6];          /* +0x2f4 */
} Rec30c;
typedef struct {
    int c0;             /* +0x00 */
    float c4;           /* +0x04 */
    float c8;           /* +0x08 */
    int cc;             /* +0x0c */
    int d0;             /* +0x10 */
    float d4;           /* +0x14 */
    int d8;             /* +0x18 */
    int dc;             /* +0x1c */
    int e0;             /* +0x20 */
    int e4;             /* +0x24 */
} Out28;
typedef struct {
    int idx;            /* +0x00 */
    int p2;             /* +0x04 */
    V3 pos;             /* +0x08 */
    float r;            /* +0x14 */
    int pad18[2];
    int n;              /* +0x20 */
} Ctx;
void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)
extern Rec30c DAT_008a9620[];
extern Out28 DAT_008a95c0[2];
extern int DAT_0063a5e8;
extern float DAT_007f1010;
int FUN_00407a90();
int FUN_00407b00(int a);
int FUN_00407b70(int a, int b);
void FUN_00407e20(Rec30c *rec, int a, int b, V3 *pos, Out28 *out, float k);
void FUN_00408590(Rec30c *rec, char *obj);
int FUN_00426bb0(void);
int *FUN_00426cf0(void);
void FUN_0046cbb0(int idx, int *state, int *other);
void FUN_00538c80(int a, V3 *pos, int (*cb)(), Ctx *ctx);
// MATCH: 0x00408610 FUN_00408610
void FUN_00408610(char *obj, int p2, int idx)
{
    int *tbl;
    int state, other;
    float now, d;
    Rec30c *rec;
    Ctx ctx;
    int k, b, c, which, n, cnt;
    Out28 *o;
    char *f;
    V3 *pp;

    tbl = FUN_00426cf0();
    FUN_0046cbb0(idx, &state, &other);
    if (state != 0)
        return;
    now = DAT_007f1010 * 3000000.0f;
    rec = &DAT_008a9620[idx];
    d = now - (float)rec->t0;
    if (d < 0.0f)
        d = (float)(0xffffffffu - rec->t0) + now;
    rec->i08 = (int)(d - (float)rec->t1c);
    FUN_0046cbb0(idx, &state, &other);
    if (state == 1)
        return;
    pp = (V3 *)(obj + 0x30);
    ctx.pos.x = pp->x;
    ctx.pos.y = pp->y;
    ctx.pos.z = pp->z;
    rec->prev = rec->cur;
    ctx.n = 3;
    ctx.r = 0.6f;
    ctx.p2 = p2;
    ctx.idx = idx;
    FUN_00538c80(p2, &ctx.pos, FUN_00407a90, &ctx);
    memset(rec->a6, 0, sizeof(rec->a6));
    for (k = 0; k < 6; k++) {
        if (rec->cur == rec->prev + 1 && rec->cur == tbl[k])
            rec->a6[k] = 1;
    }
    b = FUN_00407b00(rec->d0);
    c = rec->cur;
    if (FUN_00407b70(c, b)) {
        rec->d0 = c;
        rec->d8 = b;
        rec->d4 = -1;
    } else {
        rec->d4 = c;
        rec->dc = FUN_00407b00(c);
    }
    FUN_00407e20(rec, rec->d0, rec->d8, pp, &DAT_008a95c0[0], -0.5f);
    if (rec->d4 != -1)
        FUN_00407e20(rec, rec->d4, rec->dc, pp, &DAT_008a95c0[1], -0.5f);
    which = 0;
    if (rec->d4 != -1 && DAT_008a95c0[0].d4 != 0.0f && DAT_008a95c0[1].d4 > DAT_008a95c0[0].d4)
        which = 1;
    rec->i2f0 = DAT_008a95c0[which].cc;
    DAT_0063a5e8 = which;
    rec->pos = DAT_008a95c0[which].c4;
    rec->cur = DAT_008a95c0[which].c0;
    if (rec->cur < 0)
        rec->cur = 0;
    if (rec->pos < 0.0f)
        rec->pos = 0.0f;
    rec->fcc = DAT_008a95c0[which].c8;
    rec->e0 = DAT_008a95c0[which].d8;
    rec->e4 = DAT_008a95c0[which].dc;
    rec->e8 = DAT_008a95c0[which].e0;
    rec->ec = DAT_008a95c0[which].e4;
    FUN_00408590(rec, obj);
    if (rec->cur >= 0 && rec->cur < 0x200)
        rec->flags[rec->cur] = 1;
    if (rec->pos < 1.0f) {
        cnt = 0;
        n = FUN_00426bb0();
        if (n > 0) {
            f = rec->flags;
            k = n;
            do {
                if (*f)
                    cnt++;
                *f = 0;
                f++;
            } while (--k);
        }
        if (cnt * 100 / n > 50 || rec->n28 == -1) {
            if (++rec->n28 > 30)
                rec->n28 = 30;
            if (rec->n28 == 0) {
                rec->t0 = (int)(DAT_007f1010 * 3000000.0f);
            } else {
                for (k = 0; k < 30; k++)
                    if (rec->arr[k] == 0)
                        break;
                if (k != 30) {
                    c = rec->i08;
                    for (b = 0; b < k; b++)
                        c -= rec->arr[b];
                    rec->arr[k] = c;
                }
            }
        }
    }
    if (rec->fcc > 99.0f) {
        cnt = 0;
        n = FUN_00426bb0();
        for (k = 0; k < n; k++)
            if (rec->flags[k])
                cnt++;
        if (cnt * 100 / n < 50 && rec->n28 == 0)
            rec->n28 = -1;
    }
}
