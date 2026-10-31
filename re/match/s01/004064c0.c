// FLAGS: /O2 /GS /Ob1
/* 0x004064c0 (1167 B). Best 99.4%, residual (b): two store schedules (r1 fstp after the next pushes in ours; DAT_0063a5d4 store before the pushes in ours). static, record in EAX, returns the damage ratio in st0 (caller stores it to rec+0x60).
   CAVEAT: standin_00406dcf stands in for the real caller (call site 0x00406dcf, out of range).
   Findings: colour is RwRGBA from the second random (r2), not the ratio; the loop/position vectors are block-scoped `V3 p` and the key-frame copy is a separate non-escaping block temp `t` (all three share one slot: 83.5 -> 99.4%); the two-way FUN_0048f780 call is if/else with two calls (cross-jumped), not a ternary. Permuter 378 compiles no gain. */
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
typedef struct RwRGBA { unsigned char r, g, b, a; } RwRGBA;
typedef struct KF { char pad[0x10]; V3 v; int pad2; } KF;
typedef struct H2 { int f0; int f4; KF *keys; } H2;
typedef struct H1 { int pad[4]; H2 *f10; } H1;
typedef struct Anim { H1 *hier; int idx; int f8; int fc; int f10; float f14; float f18; } Anim;
typedef struct AObj { int f0; int f4; int f8; int fc; RwObj *f10; int f14; int f18; int f1c; int f20; int f24; } AObj;
extern float DAT_007f100c;
extern int DAT_006146fc;
extern int DAT_0063a5d4;
RwMatrix *FUN_0047d150(RwObj *o);
RwMatrix *FUN_004c0ed0(RwFrame *f);
float FUN_00472650(float lo, float hi);
void FUN_0048f780(V3 *p, float a, RwRGBA *c, float b, float d, int e, int f);
void FUN_00476860(V3 *p, int *q, float a);
void FUN_00487020(V3 *p, float a);
void FUN_00465940(V3 *p);
void FUN_00486460(V3 *p, float a);
void FUN_0047cf20(RwObj *o, int a);
AObj *FUN_0057c210(int h);
void FUN_0055ac00(int a, AObj *o, int b);
// MATCH: 0x004064c0 FUN_004064c0
static float FUN_004064c0(S_EC *s)
{
    V3 *pos = &FUN_0047d150(s->slot[10])->pos;
    float ratio = (100.0f - s->hp) * 0.01f;
    float r1 = FUN_00472650(0.0f, 1.0f);
    float r2 = FUN_00472650(0.8f, 1.2f);
    V3 *a = &FUN_004c0ed0(s->slot[8]->frame)->pos;
    V3 *b = &FUN_004c0ed0(s->slot[9]->frame)->pos;
    RwRGBA col;
    int i;
    s->f6c = DAT_007f100c + s->f6c;
    col.a = 0x80;
    if (s->f6c >= 3.0f) {
        s->f6c = 0.0f;
        s->f68 = s->f60;
    }
    if (ratio > 1.0f)
        ratio = 1.0f;
    col.g = col.r = (unsigned char)(int)(r2 * 128.0f);
    col.b = (unsigned char)(int)(r2 * 150.0f);
    if (s->f48 != 0) {
        if (r1 > 0.9f)
        {
            if (FUN_00472650(0.0f, 1.0f) > 0.5f)
                FUN_0048f780(a, 0.7f, &col, 5.0f, 0.5f, 0, 0);
            else
                FUN_0048f780(b, 0.7f, &col, 5.0f, 0.5f, 0, 0);
        }
        if (r1 > 0.96f) {
            V3 p;
            p = *pos;
            p.x += FUN_00472650(-0.9f, 0.9f);
            p.y += FUN_00472650(-0.9f, 0.9f) * 0.2f;
            p.z += FUN_00472650(-0.9f, 0.9f);
            FUN_00476860(&p, &DAT_006146fc, 360.0f);
            if (FUN_00472650(0.0f, 1.0f) > 0.8f)
                FUN_00487020(&p, 1.2f);
            FUN_00465940(&p);
        }
        return ratio;
    }
    if (ratio >= 1.0f) {
        DAT_0063a5d4++;
        s->f48 = 1;
        s->fe0 = 0;
        FUN_00476860(pos, &DAT_006146fc, 360.0f);
        FUN_00465940(pos);
        if (s->f18 != 0) {
            AObj *o;
            Anim *an;
            FUN_0047cf20(s->slot[10], 1);
            o = FUN_0057c210(s->f18);
            FUN_0055ac00(o->f24, o, 1);
            an = (Anim *)o->f10->frame;
            if (an != 0) {
                {
                V3 t;
                t = an->hier->f10->keys[an->idx].v;
                t.x = 0.0f;
                t.y = 20.0f;
                t.z = 0.0f;
                an->f18 = 1.0f / (DAT_007f100c * 10.0f);
                an->hier->f10->keys[an->idx].v = t;
                }
                an->f14 = 1.0f / (DAT_007f100c * 60.0f);
            }
        }
        for (i = 6; i != 0; i--) {
            V3 p;
            p = *pos;
            p.x += FUN_00472650(-0.9f, 0.9f);
            p.y += FUN_00472650(-0.9f, 0.9f);
            p.z += FUN_00472650(-0.9f, 0.9f);
            FUN_00476860(&p, &DAT_006146fc, 360.0f);
            FUN_00487020(&p, 1.2f);
        }
        return ratio;
    }
    if (r1 + r1 < ratio)
    {
        if (FUN_00472650(0.0f, 1.0f) > 0.5f)
            FUN_0048f780(a, 0.7f, &col, 5.0f, 0.5f, 0, 0);
        else
            FUN_0048f780(b, 0.7f, &col, 5.0f, 0.5f, 0, 0);
    }
    if (ratio > 0.5f && s->f64 == 0) {
        FUN_00486460(a, 1.0f);
        FUN_00486460(a, 1.0f);
        FUN_00486460(a, 1.0f);
        FUN_00486460(b, 1.0f);
        FUN_00486460(b, 1.0f);
        FUN_00486460(b, 1.0f);
        s->f64 = 1;
    }
    return ratio;
}
void standin_00406dcf(S_EC *s) { s->f60 = FUN_004064c0(s); }
