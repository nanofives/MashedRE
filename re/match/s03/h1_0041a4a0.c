// FLAGS: /O2 /GS /Ob1
// MATCH: 0x0041a4a0 FUN_0041a4a0
// MATCH: 0x0041a500 FUN_0041a500
// MATCH: 0x0041a550 FUN_0041a550
// MATCH: 0x0041a840 FUN_0041a840
// MATCH: 0x0041a400 FUN_0041a400
// MATCH: 0x0041a8d0 FUN_0041a8d0
// MATCH: 0x0041a9d0 FUN_0041a9d0
// MATCH: 0x0041aac0 FUN_0041aac0
// MATCH: 0x0041ac60 FUN_0041ac60
// MATCH: 0x0041ad00 FUN_0041ad00
/* h1 MATCH-S03 r2: setters on the 0xc4-stride records at 0x0063c630 (cdecl, stack args).
   0x0041a4a0: matrix at +0x00 = *src, or identity with flags |= 0x20003 when src is NULL. The store
     order is the source order: zeros, ones and the flags read only line up with the statement order
     below (natural right/up/at/pos order: 75.8%, reversed rows: 81.8%).
   0x0041a500: vec3 at +0xa0 = *src or zero.   0x0041a550: 4 floats at +0x80 = *src or zero. */
typedef struct RwV3d_ { float x, y, z; } RwV3d_;
typedef struct RwV4_ { float x, y, z, w; } RwV4_;
typedef struct RwMatrix_ {
    RwV3d_ right; unsigned int flags;
    RwV3d_ up;    unsigned int pad1;
    RwV3d_ at;    unsigned int pad2;
    RwV3d_ pos;   unsigned int pad3;
} RwMatrix_;
typedef struct RwRGBA_ { unsigned char r, g, b, a; } RwRGBA_;
typedef struct Obj48 {
    char pad[0x48];
    void (*fn48)(struct Obj48 *self);
} Obj48;
typedef struct Rec63c630 {
    RwMatrix_ m;        /* +0x00 */
    Obj48 *slot[16];    /* +0x40, filled by 0x004b51d0 */
    RwV4_  v80;         /* +0x80 */
    char   pad90[0xa0 - 0x90];
    RwV3d_ va0;         /* +0xa0 */
    int    clump;       /* +0xac */
    int    clump4;      /* +0xb0 */
    char   padb4[0xc0 - 0xb4];
    int    active;      /* +0xc0 */
} Rec63c630;            /* 0xc4 */
#ifndef MATCH_PORT
extern Rec63c630 DAT_0063c630[2];
extern int       DAT_0063c7b8[2];
extern RwV4_     DAT_0063c7c0[2];
extern RwV3d_    DAT_0063c7e0[2];
extern RwMatrix_ DAT_0063c7f8[2];
extern const float DAT_005d757c;
extern const float DAT_005cd188;
extern const float DAT_005ccac4;
float FUN_004c39b0(RwV3d_ *out, RwV3d_ *in);
extern const float DAT_005cc56c;
extern const float DAT_005cc32c;
void  FUN_0041f260(int a, RwMatrix_ *out);
float FUN_004c3ac0(RwV3d_ *v);
void  FUN_0041a5b0(int i, float t);
void  FUN_0041a5d0(int i);
void FUN_004b6520(void *p, int n);
int  FUN_004b51d0(int clump, Obj48 **out, int n, int a, int b);
void FUN_004b52f0(Obj48 *obj, unsigned int a, int b);
void FUN_004b5260(Obj48 *obj, RwRGBA_ *c);
int  FUN_0042a5d0(const char *name, int a, int b);
#endif

void FUN_0041a4a0(int i, RwMatrix_ *src)
{
    RwMatrix_ *m = &DAT_0063c630[i].m;
    if (src != 0) {
        *m = *src;
        return;
    }
    m->up.x = 0.0f;
    m->right.z = 0.0f;
    m->right.y = 0.0f;
    m->at.z = 1.0f;
    m->at.y = 0.0f;
    m->at.x = 0.0f;
    m->up.z = 0.0f;
    m->up.y = 1.0f;
    m->pos.z = 0.0f;
    m->pos.y = 0.0f;
    m->pos.x = 0.0f;
    m->right.x = 1.0f;
    m->flags |= 0x20003;
}

void FUN_0041a500(int i, RwV3d_ *src)
{
    Rec63c630 *r = &DAT_0063c630[i];
    if (src != 0) {
        r->va0.x = src->x;
        r->va0.y = src->y;
        r->va0.z = src->z;
        return;
    }
    r->va0.x = 0.0f;
    r->va0.y = 0.0f;
    r->va0.z = 0.0f;
}

void FUN_0041a550(int i, RwV4_ *src)
{
    Rec63c630 *r = &DAT_0063c630[i];
    if (src != 0) {
        r->v80.x = src->x;
        r->v80.y = src->y;
        r->v80.z = src->z;
        r->v80.w = src->w;
        return;
    }
    r->v80.x = 0.0f;
    r->v80.y = 0.0f;
    r->v80.z = 0.0f;
    r->v80.w = 0.0f;
}

void FUN_0041a840(int i)
{
    Rec63c630 *r = &DAT_0063c630[i];
    int j;
    if (r->active != 0) {
        if (r->slot[6] != 0)
            r->slot[6]->fn48(r->slot[6]);
        if (r->slot[4] != 0)
            r->slot[4]->fn48(r->slot[4]);
        for (j = 0; j < 4; j++) {
            if (r->slot[j] != 0)
                r->slot[j]->fn48(r->slot[j]);
        }
        if (r->slot[5] != 0)
            r->slot[5]->fn48(r->slot[5]);
    }
}

void FUN_0041a400(int i, int clump, RwRGBA_ *color)
{
    Rec63c630 *r = &DAT_0063c630[i];
    RwRGBA_ def;
    int j;
    def.r = 0xc8;
    def.g = 0xc8;
    def.b = 0xc8;
    def.a = 0x80;
    FUN_004b6520(r, 0xc4);
    r->clump = clump;
    r->clump4 = *(int *)(clump + 4);
    r->active = 1;
    FUN_004b51d0(clump, r->slot, 0x10, 0, 0);
    if (color == 0)
        color = &def;
    for (j = 0; j < 7; j++) {
        Obj48 *o = r->slot[j];
        if (o != 0) {
            FUN_004b52f0(o, 0x40, 1);
            FUN_004b5260(o, color);
        }
    }
}

void FUN_0041a8d0(int unused, RwRGBA_ *colors)
{
    int j;
    for (j = 0; j < 2; j++) {
        int clump = FUN_0042a5d0("ghost.dff", 0, 0);
        DAT_0063c7b8[j] = clump;
        FUN_0041a400(j, clump, &colors[j]);
        DAT_0063c7f8[j].at.z = 1.0f;
        DAT_0063c7f8[j].up.y = 1.0f;
        DAT_0063c7f8[j].right.x = 1.0f;
        DAT_0063c7f8[j].up.x = 0.0f;
        DAT_0063c7f8[j].right.z = 0.0f;
        DAT_0063c7f8[j].right.y = 0.0f;
        DAT_0063c7f8[j].at.y = 0.0f;
        DAT_0063c7f8[j].at.x = 0.0f;
        DAT_0063c7f8[j].up.z = 0.0f;
        DAT_0063c7f8[j].pos.z = 0.0f;
        DAT_0063c7f8[j].pos.y = 0.0f;
        DAT_0063c7f8[j].pos.x = 0.0f;
        DAT_0063c7f8[j].flags |= 0x20003;
    }
}

void FUN_0041a9d0(RwMatrix_ **mats)
{
    int i;
    for (i = 0; i < 2; i++) {
        RwMatrix_ *m = mats[i];
        RwV3d_ d;
        float len;
        float dot;
        d.x = m->pos.x - DAT_0063c7f8[i].pos.x;
        d.y = m->pos.y - DAT_0063c7f8[i].pos.y;
        d.z = m->pos.z - DAT_0063c7f8[i].pos.z;
        if (d.x == DAT_005d757c && d.y == DAT_005d757c && d.z == DAT_005d757c) {
            dot = 0.0f;
            len = DAT_005d757c;
        } else {
            len = FUN_004c39b0(&d, &d);
            dot = m->right.z * d.z + (m->right.y * d.y + m->right.x * d.x);
        }
        DAT_0063c7e0[i].x = 0.0f;
        DAT_0063c7e0[i].y = 0.0f;
        DAT_0063c7e0[i].z = dot * len * DAT_005cd188;
        FUN_0041a500(i, &DAT_0063c7e0[i]);
    }
}

void FUN_0041aac0(RwMatrix_ **mats)
{
    int i;
    for (i = 0; i < 2; i++) {
        RwMatrix_ *m = mats[i];
        RwV3d_ d;
        float len;
        float dot;
        float t;
        float s;
        d.x = m->pos.x - DAT_0063c7f8[i].pos.x;
        d.y = m->pos.y - DAT_0063c7f8[i].pos.y;
        d.z = m->pos.z - DAT_0063c7f8[i].pos.z;
        if (d.x == DAT_005d757c && d.y == DAT_005d757c && d.z == DAT_005d757c) {
            dot = 0.0f;
            len = DAT_005d757c;
        } else {
            len = FUN_004c39b0(&d, &d);
            dot = m->at.z * d.z + (m->at.y * d.y + m->at.x * d.x);
        }
        t = len * dot * DAT_005ccac4;
        s = DAT_0063c7c0[i].x + t;
        DAT_0063c7c0[i].x = s;
        if (s >= DAT_005ccac4)
            DAT_0063c7c0[i].x = s - DAT_005ccac4;
        s = DAT_0063c7c0[i].y + t;
        DAT_0063c7c0[i].y = s;
        if (s >= DAT_005ccac4)
            DAT_0063c7c0[i].y = s - DAT_005ccac4;
        s = DAT_0063c7c0[i].z + t;
        DAT_0063c7c0[i].z = s;
        if (s >= DAT_005ccac4)
            DAT_0063c7c0[i].z = s - DAT_005ccac4;
        s = DAT_0063c7c0[i].w + t;
        DAT_0063c7c0[i].w = s;
        if (s >= DAT_005ccac4)
            DAT_0063c7c0[i].w = s - DAT_005ccac4;
        FUN_0041a550(i, &DAT_0063c7c0[i]);
    }
}

static void FUN_0041ac60(RwMatrix_ **mats)
{
    float t;
    RwV3d_ d;
    RwMatrix_ cam;
    int i;
    FUN_0041f260(0, &cam);
    for (i = 0; i < 2; i++) {
        d.x = cam.pos.x - mats[i]->pos.x;
        d.y = cam.pos.y - mats[i]->pos.y;
        d.z = cam.pos.z - mats[i]->pos.z;
        t = FUN_004c3ac0(&d) * 0.1f;
        if (!(0.5f <= t)) {
            if (!(t >= 0.0f))
                t = 0.0f;
        } else
            t = 0.5f;
        FUN_0041a5b0(i, t);
    }
}

void FUN_0041ad00(RwMatrix_ **mats)
{
    int i;
    FUN_0041aac0(mats);
    FUN_0041ac60(mats);
    FUN_0041a9d0(mats);
    for (i = 0; i < 2; i++) {
        RwMatrix_ *m = mats[i];
        FUN_0041a4a0(i, m);
        FUN_0041a5d0(i);
        DAT_0063c7f8[i] = *m;
    }
}
