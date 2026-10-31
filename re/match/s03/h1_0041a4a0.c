// FLAGS: /O2 /GS /Ob1
// MATCH: 0x0041a4a0 FUN_0041a4a0
// MATCH: 0x0041a500 FUN_0041a500
// MATCH: 0x0041a550 FUN_0041a550
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
typedef struct Rec63c630 {
    RwMatrix_ m;        /* +0x00 */
    char   pad40[0x80 - 0x40];
    RwV4_  v80;         /* +0x80 */
    char   pad90[0xa0 - 0x90];
    RwV3d_ va0;         /* +0xa0 */
    char   padac[0xc4 - 0xac];
} Rec63c630;            /* 0xc4 */
#ifndef MATCH_PORT
extern Rec63c630 DAT_0063c630[];
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
