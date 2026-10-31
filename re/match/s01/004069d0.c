// FLAGS: /O2 /GS /Ob1
/* 0x004069d0 (264 B). Best 84.0%, residual (b): every mixed local/field x87 op has the operands the other way round (original loads frame->pos.x and s->dir.* first and uses the local as the memory operand; ours loads the local first). Tried: operand order, -a+b, float[4], cached frame pointer (66.7%), __inline Dot/Sub helpers, declaration order, same TU as the group; permuter 166 compiles no gain. 0x3fd99999 is 1.6999999f (not 1.7f). The float `f` lands in the dead param slot in both. */
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
typedef struct V4 { V3 p; float w; } V4;
extern const float DAT_005d757c;
void FUN_0041f030(int a, V3 *o);
float FUN_004c3ac0(V3 *v);
float FUN_004c39b0(V3 *o, V3 *i);
// MATCH: 0x004069d0 FUN_004069d0
void FUN_004069d0(S_EC *s)
{
    V4 p;
    V3 d;
    float f;
    FUN_0041f030(0, &p.p);
    d.x = p.p.x - s->obj->frame->pos.x;
    d.y = p.p.y - s->obj->frame->pos.y;
    d.z = p.p.z - s->obj->frame->pos.z;
    if (d.z * d.z + (d.x * d.x + d.y * d.y) > 1e-05f) {
        f = FUN_004c3ac0(&d) * (1.0f / 12.0f);
        FUN_004c39b0(&d, &d);
        if (s->dir.x * d.x + s->dir.y * d.y + s->dir.z * d.z > DAT_005d757c) {
            s->fd8 = 1.6999999f;
            return;
        }
        if (f > 1.0f)
            f = 1.0f;
        s->fd8 = (1.0f - f) * 1.6999999f + 0.1f;
    }
}
