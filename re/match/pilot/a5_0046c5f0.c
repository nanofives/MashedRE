/* a5 matching pilot, 0x0046c5f0 (210 B). Best: 100.0% exact (regs 100, ops 100), 71/71 ins (retry 2026-10-30).
   Fix: the callee is `static`, defined in the same file before its caller, built with /Ob1 so it is not
   inlined. cl 13.00.9176 then gives it its own register convention: a in EAX, then ECX / EDX, out in ESI,
   plain ret. That is exactly what Ghidra's in_EAX / unaff_ESI showed. Parameter order (a, c, b, out) puts
   c in ECX and b in EDX the way the original reads them (the first order (a, b, c, out) gave 88.7%, regs
   100: only ECX/EDX swapped). The earlier "(c) /GL convention" classification was wrong: it was source
   shape (TU layout).
   Caller 0x0046da80 (339 B, the only small one of three callers; call site 0x0046db60 loads
   eax=0x0088e620, edx=0x0088e638, ecx=0x0088e644, esi=car+0x9c8): 50.6% (regs 60.9, ops 81.6),
   class (b). Our build CSEs every zero store into one register (EBP); the original zeroes the four globals
   at 0x0088e5e0..ec through EDX and stores the float zeros (local vec, V3.y, wheel+0x30) and 0x0088e60c as
   immediates. Floats for the globals gave 55.7%. The permuter reached 65.1% only by moving stores past the
   calls that read them (not semantics-preserving, rejected). Types of those globals/locals unresolved.
   Constants: 0x005d757c = 0.0f, 0x005cc320 = 1.0f, 0x005cea60 = 0.2778f. */
// FLAGS: /O2 /GS /Ob1
typedef struct { float x, y, z; } V3;
typedef struct { float m[12]; V3 pos; float pad; } Mtx;           /* 0x40 */
typedef struct { float x; float y; float z; char p0[0x30 - 0xc]; float f30; char p1[0xc4 - 0x34]; } Wheel;
typedef struct {
    char p0[0x16c];
    Wheel w[4];                       /* 0x16c, stride 0xc4 */
    char p1[0x4a4 - 0x47c];
    float f4a4;                       /* 0x4a4 */
    char p2[0x928 - 0x4a8];
    Mtx mt[2];                        /* 0x928 */
    int p3;                           /* 0x9a8 */
    int mi;                           /* 0x9ac */
    char p4[0x9c8 - 0x9b0];
    V3 nrm;                           /* 0x9c8 */
    char p5[0xd04 - 0x9d4];
} Car;
typedef struct { V3 p; float r; int a, b; int n; } Query;
extern Car DAT_008815a0[];
extern V3 DAT_00881560[4];
extern int DAT_0088e5e0, DAT_0088e5e4, DAT_0088e5e8, DAT_0088e5ec;
extern V3 DAT_0088e600;
extern int DAT_0088e60c;
extern V3 DAT_0088e620[4];
extern int DAT_0088e654;
extern int DAT_00828320;
float FUN_004c3b30(float x);
void FUN_004c3d90(V3 *out, V3 *in, int n, Mtx *m);
int FUN_00468b80();
void FUN_00538c80(int w, Query *q, int (*cb)(), int *res);
int FUN_0046d880(Car *c, int *res);
void FUN_00480d60(int i, int f);

// MATCH: 0x0046c5f0 _FUN_0046c5f0
static void FUN_0046c5f0(float *a, float *c, float *b, float *out)
{
    float ax = a[0] - c[0], ay = a[1] - c[1], az = a[2] - c[2];
    float bx = a[0] - b[0], by = a[1] - b[1], bz = a[2] - b[2];
    float nx = bz * ay - by * az;
    float ny = bx * az - bz * ax;
    float nz = by * ax - bx * ay;
    float l = nx * nx + ny * ny + nz * nz;
    if (l >= 0.0f) {
        l = FUN_004c3b30(l);
        if (l != 0.0f) {
            l = 1.0f / l;
            out[0] = nx * l;
            out[1] = ny * l;
            out[2] = l * nz;
        }
    }
}

// MATCH: 0x0046da80 _FUN_0046da80
int FUN_0046da80(int i, int p)
{
    Car *c = &DAT_008815a0[i];
    Mtx *m = &c->mt[c->mi];
    V3 z;
    Query q;
    int j, r;
    DAT_0088e5e0 = 0;
    DAT_0088e5e4 = 0;
    DAT_0088e5e8 = 0;
    DAT_0088e5ec = 0;
    z.x = 0.0f; z.y = 0.0f; z.z = 0.0f;
    for (j = 0; j < 4; j++) {
        DAT_00881560[j].x = c->w[j].x * 0.2778f;
        DAT_00881560[j].y = 0.0f;
        DAT_00881560[j].z = c->w[j].z * 0.2778f;
        c->w[j].f30 = 0.0f;
    }
    FUN_004c3d90(&DAT_0088e600, &z, 1, m);
    FUN_004c3d90(DAT_0088e620, DAT_00881560, 4, m);
    FUN_0046c5f0(&DAT_0088e620[0].x, &DAT_0088e620[2].x, &DAT_0088e620[3].x, &c->nrm.x);
    q.n = 3;
    q.p = m->pos;
    q.r = c->f4a4;
    DAT_0088e60c = 0;
    DAT_0088e654 = p;
    FUN_00538c80(*(int *)(p + 8), &q, FUN_00468b80, &DAT_00828320);
    r = FUN_0046d880(c, &DAT_00828320);
    FUN_00480d60(i, 8);
    return r;
}
