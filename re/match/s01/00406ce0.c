/* 0x00406ce0 (1532 B). Best 71.6%, residual (b). CAVEAT: FUN_004064c0 and FUN_00406ae0 take the record in EAX and are out of range: this file has stand-in statics for them. The loop really advances i/r twice on the emitter path (0x0040728a..0x004072c7). Open: em/m register swap (edi/ebp), the -1.0f scale scheduling, the d/p interleave around the rep movsd. */
// FLAGS: /O2 /GS /Ob1
typedef struct { float x, y, z; } V3;
typedef struct { float x, y, z, w; } V4;
typedef struct { V3 right; int flags; V3 up; int pad1; V3 at; int pad2; V3 pos; int pad3; } Mat;
struct Frame { char pad[0x10]; Mat modelling; };
struct Clump { int type; struct Frame *frame; };
struct Atomic { int type; struct Frame *frame; };
struct Geo { char pad[8]; char *verts; };
struct EmObj { char pad[0x10]; struct Geo *geo; };
struct Emitter { struct EmObj *obj; int idx; int f8, fc, f10, f14; float rate; };
struct Ent { char pad[0x10]; struct { int a; struct Emitter *em; } *f10; char pad14[0x10]; int f24; };
struct Anim { char pad[0xc]; float len; };
struct Big { char pad[0x105b0]; struct Anim *tex[1]; };
struct Rec {
    char name[0x10];            /* 0x00 */
    struct Clump *clump;        /* 0x10 */
    int f14;                    /* 0x14 */
    int f18;                    /* 0x18 */
    struct Atomic *atoms[10];   /* 0x1c */
    int key;                    /* 0x44 */
    int f48;                    /* 0x48 */
    float f4c;                  /* 0x4c */
    V3 p50;                     /* 0x50 */
    float f5c;                  /* 0x5c */
    float f60;                  /* 0x60 */
    char pad64[0xa4 - 0x64];
    int fa4;                    /* 0xa4 */
    int fa8, fac;               /* 0xa8 */
    int fb0;                    /* 0xb0 */
    int idx[5];                 /* 0xb4 */
    int fc8;                    /* 0xc8 */
    struct Anim *fcc;           /* 0xcc */
    float fd0, fd4, fd8;        /* 0xd0 */
    V3 pdc;                     /* 0xdc */
    float fe8;                  /* 0xe8 */
};
extern struct Rec DAT_00639d80[];
extern int DAT_0063a5d0;
extern float DAT_007f100c;
void FUN_00406160(void);
void FUN_0041f030(int a, V4 *out);
void FUN_0047d080(int h, int a);
struct Ent *FUN_0057c210(int h);
void FUN_0055ac00(int a, struct Ent *e, int b);
void FUN_0047cf20(int h, int a);
Mat *FUN_0047d150(int h);
Mat *FUN_004c0ed0(struct Frame *f);
void FUN_004c1480(struct Frame *f, Mat *m, int a);
void FUN_004c51a0(Mat *m, V3 *v, int op);
void FUN_004c4d20(Mat *m, V3 *axis, float angle, int op);
void FUN_004b47e0(Mat *m, struct Anim *a, float t);
void FUN_004b4430(Mat *m, V4 *at, V3 *up);
void FUN_004c4dc0(Mat *out, Mat *in);
void FUN_004c52f0(Mat *m, Mat *t, int op);
void FUN_004c5470(Mat *m, V3 *axis, float *angle, V3 *center);
void FUN_004069d0(struct Rec *r);
void FUN_00406950(int i);
struct Big *FUN_00426020(void);
int FUN_00472690(int lo, int hi);

/* stand-in: out of range, register arg in EAX */
static float FUN_004064c0(struct Rec *r)
{
    return r->f4c * r->f5c;
}

/* stand-in: out of range, register arg in EAX */
static void FUN_00406ae0(struct Rec *r)
{
    r->f5c = r->f4c;
}

// MATCH: 0x00406ce0 FUN_00406ce0
void FUN_00406ce0(void)
{
    V3 v;
    int i;
    struct Rec *r;
    struct Emitter *em;
    Mat *m, *mm;
    V3 p, d, w, axis;
    float angle, s;
    V3 up;
    Mat m110, mc0, m80, inv;
    V4 cam;
    struct Ent *e;
    V3 *vp;

    FUN_00406160();
    FUN_0041f030(0, &cam);
    for (i = 0; i < DAT_0063a5d0; i++) {
        r = &DAT_00639d80[i];
        if (r->f18 == 0)
            return;
        r->f4c = DAT_007f100c + r->f4c;
        FUN_0047d080(r->key, 1);
        e = FUN_0057c210(r->f18);
        FUN_0055ac00(e->f24, e, 1);
        FUN_0047cf20(r->key, 1);
        em = e->f10->em;
        m = FUN_0047d150(r->key);
        r->p50.x = m->pos.x - FUN_004c0ed0(r->clump->frame)->pos.x;
        r->p50.y = m->pos.y - FUN_004c0ed0(r->clump->frame)->pos.y;
        r->p50.z = m->pos.z - FUN_004c0ed0(r->clump->frame)->pos.z;
        FUN_004c1480(r->clump->frame, m, 0);
        r->f60 = FUN_004064c0(r);
        if (r->f48 == 0) {
            mm = &r->atoms[1]->frame->modelling;
            v = mm->pos;
            v.x = v.x * -1.0f;
            v.y = v.y * -1.0f;
            v.z = v.z * -1.0f;
            FUN_004c51a0(mm, &v, 2);
            FUN_004c4d20(mm, &mm->up, 1.0f, 2);
            v.x = v.x * -1.0f;
            v.y = v.y * -1.0f;
            v.z = v.z * -1.0f;
            FUN_004c51a0(mm, &v, 2);
            mm = &r->atoms[2]->frame->modelling;
            v = mm->pos;
            v.x = v.x * -1.0f;
            v.y = v.y * -1.0f;
            v.z = v.z * -1.0f;
            FUN_004c51a0(mm, &v, 2);
            FUN_004c4d20(mm, &mm->up, 1.0f, 2);
            v.x = v.x * -1.0f;
            v.y = v.y * -1.0f;
            v.z = v.z * -1.0f;
            FUN_004c51a0(mm, &v, 2);
            if (em != 0) {
                FUN_004b47e0(&m80, r->fcc, r->fd0);
                r->pdc.y = r->fd4 + r->pdc.y;
                FUN_004c51a0(&m80, &r->pdc, 2);
                r->pdc.x = r->pdc.x * r->fe8;
                r->pdc.y = (r->pdc.y - r->fd4) * r->fe8;
                r->pdc.z = r->pdc.z * r->fe8;
                d.x = m80.pos.x - m->pos.x;
                d.y = m80.pos.y - m->pos.y;
                p = *(V3 *)(em->obj->geo->verts + 0x10 + em->idx * 0x20);
                d.z = m80.pos.z - m->pos.z;
                mc0 = *m;
                w.x = mc0.pos.x * -1.0f;
                w.y = mc0.pos.y * -1.0f;
                w.z = mc0.pos.z * -1.0f;
                FUN_004c51a0(&mc0, &w, 2);
                if (r->fa4 != 0) {
                    m110 = *m;
                    up.x = 0.0f;
                    up.y = 1.0f;
                    up.z = 0.0f;
                    FUN_004b4430(&m110, &cam, &up);
                } else {
                    m110 = m80;
                }
                w.x = m110.pos.x * -1.0f;
                w.y = m110.pos.y * -1.0f;
                w.z = m110.pos.z * -1.0f;
                FUN_004c51a0(&m110, &w, 2);
                FUN_004c4dc0(&inv, &mc0);
                FUN_004c52f0(&m110, &inv, 1);
                FUN_004c5470(&m110, &axis, &angle, &w);
                s = angle * 0.0055555557f * 1.5f;
                p.x = axis.x * s + p.x;
                p.y = axis.y * s + p.y;
                p.z = axis.z * s + p.z;
                em->rate = 1.0f / ((r->f60 * 8.0f + 5.0f) * DAT_007f100c);
                *(V3 *)(em->obj->geo->verts + 0x10 + em->idx * 0x20) = p;
                d.x = d.x * 3.0f;
                d.y = d.y * 3.0f;
                vp = (V3 *)(em->obj->geo->verts + em->idx * 0x20);
                vp->x = d.x;
                d.z = d.z * 3.0f;
                vp->y = d.y;
                vp->z = d.z;
                FUN_004069d0(r);
                FUN_00406950(i);
                FUN_00406ae0(r);
                r->fd0 = DAT_007f100c * r->fd8 + r->fd0;
                if (r->fd0 > r->fcc->len) {
                    struct Big *big = FUN_00426020();
                    r->fc8 = FUN_00472690(0, r->fb0 - 1);
                    r->fcc = big->tex[r->idx[r->fc8]];
                    r->fd0 = 0.0f;
                }
                i++;
                r++;
            }
        }
    }
}
