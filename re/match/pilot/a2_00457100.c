// a2 0x00457100 (1285 B): best 97.7% (regs 97.7, ops 96.8) under /O2 /GS. g2s start 17.8%.
// Natural-order version (atS.x/y/z together, locals in declaration order) scores 97.1%; this file is the
// permuter's statement/local shuffle, kept because it scored best.
// Structure: while (i < 64) { e = &pool[i]; if (bit) { ... if (e->u > 1) { clear bit; continue; } ... } i++; }
//   continue skips the increment (0x0045717d jmp 0x004575d7), and the loop test becomes a SIGNED pointer
//   compare (jl at 0x004575dd) only with the int index test, not with an `e < end` pointer loop (jb).
//   Fields read back (e->u, e->s) instead of locals: the original forwards the stored ST value and consumes
//   it at last use; with locals 9176 keeps the value live (fld C; fmul st(1) ... fstp st(0)).
//   Ghidra dropped two ftol computations (0x004571df, 0x00457335..) and the clamp at 0x00457335.
// 0x004570a0 is called with the entry pointer in EAX (mov eax,ebx at 0x00457493): no /GL needed, cl 9176 gives
//   a same-TU static function that convention. BUT /O2 (= /Ob2) inlines the faithful-size stand-in
//   (frame 0x190 vs 0x104, 73.2%); the original did not inline it (single caller, 81 B). Matched by
//   /O2 /Ob1 with a faithful stand-in, or /O2 with an enlarged stand-in (this file). Same 97.x either way.
// FP: 9176 reassociates `v*C + fx + C` into (v + 1.0)*C + fx (new 1.0 literal) and rewrites
//   `z * -0.1f + y` into `y - z*0.1f` (fsubr); the original does neither. Explicit parentheses block both
//   in 9176, so the source may have had them (b) or 9466 does not reassociate (a). Not separable here.
// Residual (all in the atS block, 0x0045722e..0x00457291): the original loads the stack operand first
//   (fld [mc.at.x]; fmul [0x005ce4f0]); 9176 always loads a NAMED global first for mem*global, in either
//   source order (verified on a 4-line test file). A literal -0.02f gives the right operand order but a
//   different push/fstp interleave (96.1%). Statement moves and the permuter (1 000+ variants) do not
//   reach both. Class (a)/(b) boundary: operand-order and scheduling heuristics.
// FLAGS: /O2 /GS /Ob1
typedef struct { float x, y, z; } V3;
typedef struct { float x, y; } V2;
typedef struct { unsigned char r, g, b, a; } RGBA;
typedef struct {
    V3 right; unsigned int flags;
    V3 up;    unsigned int pad1;
    V3 at;    unsigned int pad2;
    V3 pos;   unsigned int pad3;
} Mat;                                   /* 0x40 */
typedef struct {
    float t;                             /* +0x00 */
    float u;                             /* +0x04 */
    int pad08[3];
    float s;                             /* +0x14 */
    Mat m;                               /* +0x18 */
    int state;                           /* +0x58 */
    float timer;                         /* +0x5c */
    int flag;                            /* +0x60 */
    int pad64;
} PoolH;                                 /* 0x68, 64 entries at 0x00688810 */

extern PoolH DAT_00688810[64];
extern unsigned char DAT_0068a300[8];
extern unsigned char DAT_0068a210[];
extern unsigned char DAT_0068a2c0[];
extern float _DAT_007f100c;
extern float _DAT_005ce4f8;   /* 0.0142857 */
extern float _DAT_005cc320;   /* 1.0 */
extern float _DAT_005cd0ec;   /* 0.005 */
extern float _DAT_005ce194;   /* 200.0 */
extern float _DAT_005cd074;   /* 1.25 */
extern float _DAT_005cc32c;   /* 0.5 */
extern float _DAT_005cd9c8;   /* 220.0 */
extern float _DAT_005ce4f4;   /* 35.0 */
extern float _DAT_005ce4f0;   /* -0.02 */
extern float _DAT_005ce4ec;   /* 81.0 */
extern float _DAT_005ce4e8;   /* 1/81 */
extern float _DAT_005cc9d0;   /* 128.0 */
extern float _DAT_005cc9b0;   /* 15.0 */
extern float _DAT_005cd0fc;   /* -0.1 */
extern float DAT_005d757c;    /* 0.0 */
extern float _DAT_005cc950;   /* 0.75 */
extern float _DAT_005ce4e4;   /* 0.995 */

Mat *FUN_00467210(int a);
float FUN_00472650(float lo, float hi);
void FUN_004769f0(RGBA *c);
void FUN_00476a10(Mat *m);
void FUN_00476a80(V2 *p);
void FUN_00476d00(void *p);
void FUN_00476df0(void *p);
void FUN_004864f0(Mat *m, float f, int n);
void FUN_0048f780(V3 *p, float a, RGBA *c, float b, float d, int e, int f);
void FUN_004c3d90(V3 *out, V3 *in, int n, Mat *m);
void FUN_004c5010(Mat *m, V3 *s, int comb);
void FUN_004c51a0(Mat *m, V3 *t, int comb);
void FUN_004c4670(Mat *dst, Mat *src);
void FUN_00484cf0(void *p);
void FUN_00457010(void);

/* stand-in for 0x004570a0 (EAX-passed entry pointer, static in the same TU); not scored */
typedef struct { V3 pos; float s; int kind; PoolH *e; int z; void (*cb)(void); int pad[27]; } Req;
static void FUN_004570a0(PoolH *e)
{
    Req r;
    r.pos = e->m.pos;
    r.s = e->s;
    r.e = e;
    r.z = 0;
    r.kind = 0x12;
    r.cb = FUN_00457010;
    FUN_00484cf0(&r); FUN_00484cf0(&r); FUN_00484cf0(&r); FUN_00484cf0(&r); FUN_00484cf0(&r); FUN_00484cf0(&r); FUN_00484cf0(&r); FUN_00484cf0(&r);
    r.pos = e->m.at; FUN_00484cf0(&r); r.pos = e->m.up; FUN_00484cf0(&r); r.pos = e->m.right; FUN_00484cf0(&r);
}

// MATCH: 0x00457100 _FUN_00457100
void FUN_00457100(void)
{
    int i;
    V3 verts[4];
    V3 v;
    RGBA c1, c2, c3;
    V3 atS;
    V2 pts[4];
    PoolH *e;
    Mat *cam;
    V3 scale;
    Mat mc;
    Mat tmp;
    float u, s, dx, dy, dz, d2, fx, fz;

    i = 0;

    while (i < 64) {
        e = &DAT_00688810[i];
        if (DAT_0068a300[i >> 3] & (1 << (i & 7))) {
            c1.r = 0x80;
            c1.g = 0x80;
            e->t = _DAT_007f100c + e->t;
            e->u = e->t * _DAT_005ce4f8;
            c1.b = 0x80;
            c1.a = 0xff;

            if (e->u > _DAT_005cc320) {
                DAT_0068a300[i >> 3] &= ~(1 << (i & 7));
                continue;
            }
            if (e->u < _DAT_005cd0ec)
                e->s = e->u * _DAT_005ce194 * _DAT_005cd074;
            else
                e->s = e->u * _DAT_005cc32c + _DAT_005cd074;

            scale.x = scale.y = scale.z = e->s;
            mc = e->m;
            FUN_004c5010(&mc, &scale, 1);
            c1.a = (unsigned char)((_DAT_005cc320 - e->u) * _DAT_005cd9c8 + _DAT_005ce4f4);
            FUN_004769f0(&c1);
            FUN_00476a10(&mc);
            FUN_00476d00(DAT_0068a2c0);
            mc = e->m;
            c2.r = 0xff;
            c2.g = 0xff;
            atS.z = mc.at.z * _DAT_005ce4f0;
            c2.b = 0xff;
            c2.a = 0x80;
            atS.y = _DAT_005ce4f0 * mc.at.y;
            verts[0].x = 0.5f;  verts[0].y = -0.5f; verts[0].z = 0.0f;
            atS.x = mc.at.x * _DAT_005ce4f0;
            verts[1].x = -0.5f; verts[1].y = -0.5f; verts[1].z = 0.0f;
            verts[2].x = -0.5f; verts[2].y = 0.5f;  verts[2].z = 0.0f;
            verts[3].x = 0.5f;  verts[3].y = 0.5f;  verts[3].z = 0.0f;
            FUN_004c51a0(&mc, &atS, 2);
            cam = FUN_00467210(0);
            dx = cam->pos.x - mc.pos.x;
            dy = cam->pos.y - mc.pos.y;
            dz = cam->pos.z - mc.pos.z;
            d2 = dz * dz + (dx * dx + dy * dy);
            if (d2 > _DAT_005ce4ec)
                d2 = _DAT_005ce4ec;
            c2.a = (unsigned char)(_DAT_005ce4e8 * d2 * _DAT_005cc9d0 + _DAT_005cc9b0);
            FUN_004c3d90(verts, verts, 4, &mc);
            fx = cam->pos.x * -0.1f - cam->at.x;
            fz = (cam->pos.z * -0.1f) + cam->at.y;
            pts[0].x = (verts[0].x * _DAT_005cc32c + fx) + _DAT_005cc32c;
            pts[0].y = (verts[0].z * _DAT_005cc32c + fz) + _DAT_005cc32c;
            pts[1].x = (verts[1].x * _DAT_005cc32c + fx) + _DAT_005cc32c;
            pts[1].y = (verts[1].z * _DAT_005cc32c + fz) + _DAT_005cc32c;
            pts[2].x = (verts[2].x * _DAT_005cc32c + fx) + _DAT_005cc32c;
            pts[2].y = (_DAT_005cc32c * verts[2].z + fz) + _DAT_005cc32c;
            pts[3].x = (verts[3].x * _DAT_005cc32c + fx) + _DAT_005cc32c;
            pts[3].y = _DAT_005cc32c + (verts[3].z * _DAT_005cc32c + fz);
            FUN_004769f0(&c2);
            FUN_00476a80(pts);
            FUN_00476a10(&mc);
            FUN_00476d00(DAT_0068a210);
            FUN_004570a0(e);
            if (e->flag == 0 && e->state == 1) {
                e->timer = e->timer - _DAT_007f100c;
                if (e->timer <= DAT_005d757c) {
                    FUN_004c4670(&tmp, &e->m);
                    FUN_004864f0(&tmp, 1.2f, 1);
                    e->flag = 1;
                    e->state = 0;
                    e->timer = 0.1f;
                }
            }
            if (e->flag == 1 && e->u < _DAT_005cc950) {
                c3.r = 0x20;
                c3.g = 0x20;
                c3.b = 0x20;
                c3.a = 200;
                if (FUN_00472650(0.0f, 1.0f) > _DAT_005ce4e4) {
                    v = e->m.pos;
                    v.x = FUN_00472650(-0.1f, 0.1f) + v.x;
                    v.y = FUN_00472650(-0.1f, 0.1f) + v.y;
                    v.z = FUN_00472650(-0.1f, 0.1f) + v.z;
                    FUN_0048f780(&v, 0.4f, &c3, 5.0f, 0.5f, 0, 0);
                }
            }
        }
        i = i + 1;

    }
    FUN_00476df0(DAT_0068a2c0);
    FUN_00476df0(DAT_0068a210);
}
