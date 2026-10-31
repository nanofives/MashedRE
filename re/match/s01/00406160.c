// FLAGS: /O2 /GS /Ob1
/* 0x00406160 (522 B, g2s did not compile). Best 88.7%, residual (b): two x87 scheduling spots, (1) at.y*sp is `fld [at.y]; fmul st(1)` in the original, `fld st(0); fmul [at.y]` in ours (operand order in source does not move it), (2) the original pops 45dce0's args before the radius sum, ours defers the pop to the 4b4d10 call. Found: smoke array 0x0063a490 is E20[10] {RwObj*; float speed; float age; int alive; int h; V3 vel}, loop index i with `i >= 4` strength-reduced into a pointer compare, RwSphere {center, radius} query, &m->at taken as a pointer. Permuter 279 compiles no gain. */
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
typedef struct RwSphere { V3 center; float radius; } RwSphere;
typedef struct E20 { RwObj *obj; float f4; float f8; int fc; int h10; V3 vel; } E20;
extern E20 DAT_0063a490[10];
extern int DAT_008aa250;
extern float DAT_007f100c;
extern int DAT_006146fc;
RwMatrix *FUN_004c0ed0(RwFrame *f);
void FUN_004c1340(RwFrame *f, V3 *v, int c);
void FUN_00489a40(int h, V3 *p, int a, int b);
void FUN_0045dce0(int id, V3 *p, int pl);
int FUN_0045bfe0(void *q, void *buf);
int FUN_004b4d10(int h);
int FUN_0045c350(void *buf, V3 *p);
float FUN_004173a0(void);
void FUN_00453eb0(RwSphere *s, float a, float b, float c, int d);
void FUN_00476860(RwSphere *s, int *p, float a);
void FUN_004661a0(int id, int pl);
// MATCH: 0x00406160 FUN_00406160
void FUN_00406160(void)
{
    int i;
    for (i = 0; i < 10; i++) {
        E20 *e = &DAT_0063a490[i];
        if (e->fc != 0) {
            V3 *at;
            float sp;
            V3 np;
            RwSphere sph;
            int buf[16];
            if (e->f8 > 7.0f)
                e->fc = 0;
            e->f8 = DAT_007f100c + e->f8;
            at = &FUN_004c0ed0(e->obj->frame)->at;
            sp = DAT_007f100c * e->f4;
            np.x = at->x * sp;
            np.y = at->y * sp;
            np.z = at->z * sp;
            np.x = np.x + e->vel.x;
            np.y = np.y + e->vel.y;
            np.z = np.z + e->vel.z;
            e->vel.x *= 0.99f;
            e->vel.y *= 0.99f;
            e->vel.z *= 0.99f;
            FUN_004c1340(e->obj->frame, &np, 2);
            FUN_00489a40(e->h10, &e->obj->frame->pos, 0, 0);
            DAT_008aa250 = i;
            if (i >= 4)
                DAT_008aa250 = i - ((unsigned)i / 4) * 4;
            FUN_0045dce0(0x14, &e->obj->frame->pos, DAT_008aa250);
            sph.radius = (np.z * np.z + (np.x * np.x + np.y * np.y)) * 1.5f + 0.05f;
            sph.center = e->obj->frame->pos;
            if (FUN_004b4d10(FUN_0045bfe0(&sph, buf)) != 0 && FUN_0045c350(buf, &sph.center) == 0) {
                sph.radius = 1.2f;
                FUN_00453eb0(&sph, FUN_004173a0() + 15.0f, 200.0f, 150.0f, 2);
                FUN_00476860(&sph, &DAT_006146fc, 360.0f);
                DAT_008aa250 = i;
                if (i >= 4)
                    DAT_008aa250 = i - ((unsigned)i / 4) * 4;
                FUN_004661a0(0x14, DAT_008aa250);
            }
        }
    }
}
