// a2 0x0044c4f0 (250 B): best 98.8% (regs 98.8, ops 98.8). g2s start 23.3%.
// Fixes, in order of gain: (1) callee arity: 0x004b3f90 takes ONE arg (its body reads only [esp+8]);
//   the other six pushes belong to 0x0047fad0 -> FUN_0047fad0(FUN_004b3f90(b), v, 0, .1f, .9999f, 80.f, e+0x75ab).
//   Ghidra had all seven on 4b3f90. (2) 0x3f7ff972 = 0.9999f. (3) -t*k via a negated temp (9176 folds
//   (-a)*k into -(a*k): fmul then fchs; the original does fld a; fchs; fmul st(1)). (4) 0.5 (0x005cc32c)
//   and 0.875 (0x005cd0b4) as literals, not named externs: 9176 loads a named global first for mem*global
//   and keeps a live local in ST, with literals it matches the original operand order, the k/x stack use
//   and the lea/add esp/xor scheduling after rep movsd. 0x005cc320 (1.0) stays an extern: literal 1.0f
//   gives 93.8%.
// Residual (1 instruction): mov [ebx+0x3c],eax is scheduled after the two pushes for 0x0047cdc0 instead of
//   before. Tried s->h = ...; call(s->h), call(s->h = h), chained assignment, permuter (148+148 variants):
//   no move. Class (a) candidate: store-vs-push scheduling.
// FLAGS: /O2 /GS /Ob1
typedef struct { float f[6]; } Tmpl6;
typedef struct {
    int a, b, c, d;          /* +0x00 */
    float k[4];              /* +0x10 */
    Tmpl6 t;                 /* +0x20 */
    int n;                   /* +0x38 */
    int h;                   /* +0x3c */
    float v[3];              /* +0x40 */
    int z0;                  /* +0x4c */
    float w;                 /* +0x50 */
    int z1;                  /* +0x54 */
} Slot44c;
extern float _DAT_005cc320;
extern float _DAT_005cc32c;
extern float _DAT_005cd0b4;
extern Tmpl6 DAT_005f9730;
float FUN_004728f0(int a);
void FUN_0047cdc0(int h, float f);
int FUN_0047fad0(int a, float *v, int z, float f0, float f1, float f2, int e);
int FUN_004b3f90(int a);

// MATCH: 0x0044c4f0 _FUN_0044c4f0
void FUN_0044c4f0(Slot44c *s, int a, int b, int c, int e)
{
    float k;
    float x;
    int h;
    s->a = a;
    s->b = b;
    s->c = c;
    s->d = 0;
    k = _DAT_005cc320 / FUN_004728f0(a);
    s->t = DAT_005f9730;
    s->n = 9;
    s->z0 = 0;
    s->z1 = 0;
    s->k[0] = k * s->t.f[2];
    { float nt = -s->t.f[5]; s->k[1] = nt * k; }
    s->k[2] = k * s->t.f[0];
    { float nt3 = -s->t.f[3]; s->k[3] = nt3 * k; }
    x = (s->t.f[1] - s->t.f[4]) * 0.5f;
    s->w = x;
    s->v[0] = 0.875f * ((s->t.f[0] - s->t.f[3]) * 0.5f);
    s->v[1] = x * 0.875f;
    s->v[2] = 0.875f * ((s->t.f[2] - s->t.f[5]) * 0.5f);
    h = FUN_0047fad0(FUN_004b3f90(b), s->v, 0, 0.1f, 0.9999f, 80.0f, e + 0x75ab);
    s->h = h;
    FUN_0047cdc0(h, 100.0f);
}
