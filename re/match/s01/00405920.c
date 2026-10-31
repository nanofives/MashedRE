// FLAGS: /O2 /GS /Ob1
/* Group file 0x00405920..0x00406cdb (one TU: register-arg statics). Results: 0x00405920 99.2% (b), 0x00405ab0 96.9% (b), 0x00405f60 100%, 0x00406070 100%, 0x00406410 100%, 0x00406ae0 91.8% (b).
   Conventions plain cl gives these statics (all verified against the callers' register setup): 405920(s=EAX, v=EDI, 2 stack floats); 405ab0(s=ESI, target on stack); 405f60(s=ECX, v=EAX); 406070(m=EAX, vel=EDI); 406410(s=ESI, v=ECX); 406ae0(s=EAX).
   CAVEAT: standin_00407220 stands in for the real caller of 0x00406ae0 (call site 0x00407220, out of range).
   Findings: float literals (pool 0x005cc98c..0x005cc9f0 is the literal pool in first-use order) must be literals, not extern globals, or the x87 operand order flips (405ab0 82->97%). Exception: 57.29578 at 0x005cc98c keeps the right shape only as an extern const (literal: 93.9%). acos result goes through `float a` (99.2 vs 93.9). Slot index into s->slot[] needs a separate `int i = rand + 4` (else the +4 folds). 64-byte query buffer is int[16] (char[64] adds a /GS cookie). Loop vector in 405ab0 is block-scoped (82->96%). DAT_008aa250 = i mod 4 is `i - ((unsigned)i / 4) * 4` under `if (i >= 4)`.
   Residuals (b): 405920 one fld scheduled before `add esp,4`; 405ab0 x87 load/push interleave in the 5x loop and the final add; 406ae0 block layout: the original keeps the zeroing block (fa4=0, fac=0) as the fallthrough of the `!= 6 || bad` test, ours tail-merges it with the fa8-path `fa4 = 0` and places it last (tried ==/!= polarity, nested if/else, bad typed char/uchar, Z store order, permuter 193 compiles). Pct uses (s->f4c * 0.011111111f) * 100 written as 100 * (x * k) to stop constant folding. */
typedef struct V3 { float x, y, z; } V3;
typedef struct RwLine { V3 start; V3 end; } RwLine;
typedef struct RwFrame { int pad[4]; V3 right; int r1; V3 up; int u1; V3 at; int a1; V3 pos; int p1; } RwFrame;
typedef struct RwObj { int type; RwFrame *frame; } RwObj;
typedef struct S_EC {
    char name[0x10];      /* +0x00 */
    RwObj *obj;           /* +0x10 */
    int f14;              /* +0x14 */
    int f18;              /* +0x18 */
    RwObj *slot[11];      /* +0x1c .. +0x44 */
    int f48;              /* +0x48 */
    float f4c;            /* +0x4c */
    V3 dir;               /* +0x50 */
    float hp;             /* +0x5c */
    float f60;            /* +0x60 */
    int f64;              /* +0x64 */
    float f68;            /* +0x68 */
    float f6c;            /* +0x6c */
    float f70;            /* +0x70 */
    float f74;            /* +0x74 */
    float f78;            /* +0x78 */
    V3 p7c;               /* +0x7c */
    V3 p88;               /* +0x88 */
    float f94;            /* +0x94 */
    float f98;            /* +0x98 */
    float f9c;            /* +0x9c */
    float fa0;            /* +0xa0 */
    int fa4;              /* +0xa4 */
    float fa8;            /* +0xa8 */
    float fac;            /* +0xac */
    int n;                /* +0xb0 */
    int ids[6];           /* +0xb4 */
    int fcc;              /* +0xcc */
    float fd0;            /* +0xd0 */
    int fd4;              /* +0xd4 */
    float fd8;            /* +0xd8 */
    int fdc;              /* +0xdc */
    int fe0;              /* +0xe0 */
    int fe4;              /* +0xe4 */
    float fe8;            /* +0xe8 */
} S_EC;
typedef struct RwMatrix { V3 right; int r1; V3 up; int u1; V3 at; int a1; V3 pos; int p1; } RwMatrix;
typedef struct E20 { RwObj *obj; float f4; int f8; int fc; int h10; V3 vel; } E20;
extern E20 DAT_0063a490[10];
extern int DAT_0063a5dc;
extern int DAT_008aa250;
extern float DAT_007f100c;
extern const float DAT_005d757c;
extern const float DAT_005cc98c;
int FUN_0045bfe0(RwLine *l, void *buf);
int FUN_004b4cd0(int h);
int FUN_0045c350(void *buf, V3 *p);
float FUN_004c3ac0(V3 *v);
float FUN_004c39b0(V3 *o, V3 *i);
float FUN_004726f0(V3 *a, V3 *b);
double FUN_004a3384(double x);
RwMatrix *FUN_004c0ed0(RwFrame *f);
void FUN_00465ca0(int id, V3 *p);
void FUN_00476320(V3 *o, V3 *size, float k);
void FUN_0048aa20(V3 *p, int n, V3 *v, int a, V3 *ofs, float k);
void FUN_00474de0(V3 *p, V3 *o, float k);
float FUN_00472650(float lo, float hi);
int FUN_00472690(int lo, int hi);
void FUN_00456140(V3 *p, float k);
float FUN_004173a0(void);
void FUN_00421630(int a, float k, V3 *p);
void FUN_0046bf50(V3 *v, int a);
void FUN_0046d740(V3 *v, int a);
void FUN_004c1480(RwFrame *f, RwMatrix *m, int c);
void FUN_004899c0(int h, V3 *p);
void FUN_00465e80(int id, V3 *p, int pl);
void FUN_0041f030(int a, V3 *o);
int FUN_00426c00(void);
int FUN_0040e350(void);

// MATCH: 0x00405920 FUN_00405920
static int FUN_00405920(S_EC *s, V3 *v, float maxDist, float maxAng)
{
    RwFrame *fr = s->obj->frame;
    V3 *pos = &fr->pos;
    V3 *at = &fr->at;
    RwLine line;
    int buf[16];
    V3 d;
    float len;
    line.start = *pos;
    line.end = *v;
    if (FUN_004b4cd0(FUN_0045bfe0(&line, buf)) == 1 && FUN_0045c350(buf, pos) == 0)
        return 0;
    line.start = *v;
    line.end = *pos;
    if (FUN_004b4cd0(FUN_0045bfe0(&line, buf)) == 1 && FUN_0045c350(buf, pos) == 0)
        return 0;
    d.x = v->x - pos->x;
    d.y = v->y - pos->y;
    d.z = v->z - pos->z;
    d.y = 0.0f;
    len = FUN_004c3ac0(&d);
    if (d.z * d.z + (d.x * d.x + d.y * d.y) > 1e-05f)
        FUN_004c39b0(&d, &d);
    {float a = FUN_004a3384(FUN_004726f0(&d, at)); if (DAT_005cc98c * a <= maxAng && len <= maxDist) return 1;} return 0;
}

// MATCH: 0x00405ab0 FUN_00405ab0
static void FUN_00405ab0(S_EC *s, V3 *target)
{
    V3 w;
    V3 t;
    V3 d;
    V3 ofs;
    V3 size;
    RwMatrix *m;
    V3 *mpos;
    int i;
    size.x = 1.0f; size.y = 1.5f; size.z = 1.0f;
    ofs.x = 0.0f; ofs.y = -0.001f; ofs.z = 0.0f;
    m = FUN_004c0ed0(s->slot[3]->frame);
    mpos = &m->pos;
    FUN_00465ca0(0x13, mpos);
    FUN_00465ca0(0x25, target);
    for (i = 5; i != 0; i--) {
        V3 d;
        FUN_00476320(&d, &size, 10.0f);
        d.x = d.x * 0.023f;
        d.y = d.y * 0.023f;
        d.z = d.z * 0.023f;
        FUN_0048aa20(mpos, 1, &d, 0, &ofs, 2.0f);
        FUN_0048aa20(target, 1, &d, 0, &ofs, 2.0f);
    }
    FUN_00474de0(target, &t, 0.1f);
    t.x += FUN_00472650(-0.2f, 0.2f);
    t.y += FUN_00472650(-0.2f, 0.2f);
    t.z += FUN_00472650(-0.2f, 0.2f);
    FUN_00456140(&t, 0.6f);
    FUN_00474de0(mpos, &t, 0.1f);
    t.x += FUN_00472650(-0.1f, 0.1f);
    t.y += FUN_00472650(-0.1f, 0.1f);
    t.z += FUN_00472650(-0.1f, 0.1f);
    FUN_00456140(&t, 0.6f);
    FUN_00421630(0, FUN_004173a0() * 0.025f + 0.01f, &t);
    w.x = FUN_00472650(-200.0f, 200.0f);
    w.y = 0.0f;
    w.z = FUN_00472650(-200.0f, 200.0f);
    w.x = (FUN_004173a0() * 0.1f + 0.05f) * w.x;
    w.y = (FUN_004173a0() * 0.1f + 0.05f) * w.y;
    w.z = (FUN_004173a0() * 0.1f + 0.05f) * w.z;
    FUN_0046bf50(&w, 0);
    w.x = 0.0f;
    w.y = FUN_00472650(-0.8f, 0.8f);
    if (w.y < 0.3f && w.y > DAT_005d757c)
        w.y = 0.3f;
    else if (w.y > -0.3f && w.y < DAT_005d757c)
        w.y = -0.3f;
    w.z = 0.0f;
    w.x = (FUN_004173a0() * (1.0f/14.0f) + 0.05f) * w.x;
    w.y = (FUN_004173a0() * (1.0f/14.0f) + 0.05f) * w.y;
    w.z = (FUN_004173a0() * (1.0f/14.0f) + 0.05f) * w.z;
    FUN_0046d740(&w, 0);
    s->p7c = *mpos;
    s->p88 = *target;
    d.x = s->p88.x - s->p7c.x;
    d.y = s->p88.y - s->p7c.y;
    d.z = s->p88.z - s->p7c.z;
    s->p7c.x += FUN_00472650(0.1f, 0.4f) * d.x;
    s->p7c.y += FUN_00472650(0.1f, 0.4f) * d.y;
    s->p7c.z += FUN_00472650(0.1f, 0.4f) * d.z;
    s->p88.x += FUN_00472650(-0.1f, -0.4f) * d.x;
    s->p88.y += FUN_00472650(-0.1f, -0.4f) * d.y;
    s->p88.z += FUN_00472650(-0.1f, -0.4f) * d.z;
    d.x = FUN_00472650(-0.1f, 0.1f);
    d.y = FUN_00472650(-0.1f, 0.1f);
    d.z = FUN_00472650(-0.1f, 0.1f);
    s->p7c.x += d.x;
    s->p7c.y += d.y;
    s->p7c.z += d.z;
    s->p88.x += d.x;
    s->p88.y += d.y;
    s->p88.z += d.z;
}

// MATCH: 0x00405f60 FUN_00405f60
static void FUN_00405f60(S_EC *s, V3 *v)
{
    s->p7c.x = s->p7c.y = s->p7c.z = 0.0f;
    s->p88.x = s->p88.y = s->p88.z = 0.0f;
    if (s->f74 >= 1.4f) {
        if (FUN_00405920(s, v, 15.0f, 45.0f) && FUN_00472650(0.0f, 10.0f) < s->f70)
            s->f74 -= DAT_007f100c;
    } else {
        if (s->f78 >= 0.05f) {
            if (FUN_00405920(s, v, 15.0f, 45.0f)) {
                FUN_00405ab0(s, v);
                s->f78 -= DAT_007f100c;
            }
        } else {
            s->f78 -= DAT_007f100c;
            if (s->f78 <= DAT_005d757c)
                s->f78 = 0.05f;
        }
        s->f74 -= DAT_007f100c;
        if (s->f74 <= DAT_005d757c)
            s->f74 = 1.4f;
    }
}

// MATCH: 0x00406070 FUN_00406070
static void FUN_00406070(RwMatrix *m, V3 *vel)
{
    E20 *e = &DAT_0063a490[DAT_0063a5dc];
    e->f8 = 0;
    e->fc = 1;
    e->f4 = 6.0f;
    FUN_004c1480(e->obj->frame, m, 0);
    e->vel = *vel;
    FUN_004899c0(e->h10, &e->obj->frame->pos);
    DAT_008aa250 = DAT_0063a5dc; if (DAT_0063a5dc >= 4) DAT_008aa250 = DAT_0063a5dc - ((unsigned)DAT_0063a5dc / 4) * 4;
    FUN_00465e80(0x15, &e->obj->frame->pos, DAT_008aa250);
    FUN_00465e80(0x14, &e->obj->frame->pos, DAT_008aa250);
    if (++DAT_0063a5dc >= 10)
        DAT_0063a5dc = 0;
}

// MATCH: 0x00406410 FUN_00406410
static void FUN_00406410(S_EC *s, V3 *v)
{
    if (s->fa0 <= DAT_005d757c) {
        if (FUN_00405920(s, v, 25.0f, 30.0f) && FUN_00472650(0.0f, 10.0f) < s->f9c) {
            { int i = FUN_00472690(0, 3) + 4; FUN_00406070(FUN_004c0ed0(s->slot[i]->frame), &s->dir); }
            if (s->f60 < 0.5f)
                s->fa0 = 1.0f;
            else
                s->fa0 = 0.2f;
        }
    } else {
        s->fa0 -= DAT_007f100c;
    }
}

// MATCH: 0x00406ae0 FUN_00406ae0
static void FUN_00406ae0(S_EC *s)
{
    int pct = (int)(100.0f * (s->f4c * 0.011111111f));
    int bad = 0;
    struct { V3 p; float w; } v;
    FUN_0041f030(0, &v.p);
    if (FUN_00426c00() == 0) {
        if (s->fd0 > 12.0f && s->fd0 < 16.0f)
            bad = 1;
        else if (s->fd0 > 24.0f && s->fd0 < 27.0f)
            bad = 1;
    } else if (FUN_00426c00() == 0x1a && s->fd0 > 20.5f && s->fd0 < 25.0f) {
        bad = 1;
    }
    if (FUN_0040e350() != 6 || bad == 1) {
        s->fa4 = 0;
        s->fac = 0.0f;
    } else if (s->fa4 != 0) {
        s->fac = 0.0f;
        s->fa8 -= DAT_007f100c;
        if (s->fa8 <= DAT_005d757c)
            s->fa4 = 0;
    } else {
        float lim = (1.0f - FUN_004173a0() * 0.1f) * 5.0f;
        float diff = s->f60 - s->f68;
        float t = DAT_007f100c + s->fac;
        s->fac = t;
        if (lim < 1.0f)
            lim = 1.0f;
        if (t > lim || diff > 0.2f) {
            s->fa4 = 1;
            s->fa8 = 4.0f;
        }
    }
    if (pct == 50)
        s->f94 += 0.03f;
    if (pct >= 99 || s->f60 >= 0.95f) {
        s->f70 = 1.0f;
        s->f94 = 1.0f;
        s->fa4 = 1;
    }
    if (pct > 3) {
        FUN_00405f60(s, &v.p);
        FUN_00406410(s, &v.p);
    }
}

void standin_00407220(S_EC *s) { FUN_00406ae0(s); }
