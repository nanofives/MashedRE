// FLAGS: /O2 /GS /Ob1
/* a4 / 0x004783f0 (482 B per table, real body 491 B to 0x004785da): best 90.8% (regs 90.8, ops 93.0).
 * Path: Ghidra raw 19.3% (it dropped the 0.0f dot/cross terms, the clamp and the dx/dy/dz math).
 *  - struct layout (dist[12][512] @0, ang[12][512] @0x6000, nodes[] stride 0x4c @0xc1c0, count @0x16244): 62%
 *  - acos as a plain stack-arg call (0x004a3384 is the CRT stack entry; a `double acos(double)`
 *    prototype becomes the _CIacos register intrinsic under /O2): frame now matches incl. the
 *    `and esp,-8` alignment, 84.7%
 *  - dot product split into two statements: one expression `v0 + v1*0 + v2*0` is refactored by
 *    13.00.9176 into `(v2+v1)*0 + v0`; `d = v0 + v1*0; d += v2*0;` gives the original order: 87.9%
 *  - permuter: moving `v[2] = 0.0f` to the top of the block (semantics unchanged, v is first read by
 *    FUN_004c39b0) makes the scheduler emit `lea esi / lea eax / push ecx` before the fsubs: 90.8%.
 * Residual:
 *  - (a)-candidate: one basic block (0x00478464..0x004784e9) has the same instructions in a different
 *    order. Original issues `fld [eax+ebx+0xc1cc]` first, then lea esi / lea eax / push ecx, then 5 FP ops
 *    on [esi+..]/[eax+..]; with the faithful statement order 9176 issues all 6 FP ops on [reg+ebx+disp]
 *    and the leas after. Tried: pc/pn pointer locals, nodes base pointer, dv[3] array, temp for the sum,
 *    temp for the sqrt result, v[2] first among the v stores, dz/dy/dx order, /G6 /G7 /GB, 2 permuter
 *    runs (6 rounds, 580 compiles). Only adding/moving a store inside that block reorders it, and none
 *    reproduces the original's order. The `mov [esp+0x30],0` store position (before the call in the
 *    original) is the price of the permuter variant.
 *  - knock-on: the 5-byte size difference changes the alignment pad before the `a += 360` loop, so the
 *    original's `jmp L; 9-byte pad; L:` becomes a 6-byte fall-through pad here, and later jump
 *    displacements differ.
 *  - (c): the function-table size (482) stops inside the outer-loop `jl`; the last 7 instructions
 *    (jl, pop edi/esi/ebx, mov esp,ebp, pop ebp, ret) are compiled identically but not compared. */
int FUN_00409300(int group);
int FUN_00409330(int node, int group);
double FUN_004a3384(double);          /* CRT acos body, stack arg (not the _CIacos intrinsic) */
float FUN_004c3b30(float);                 /* returns a float (fld [esp+4] tail) */
float FUN_004c39b0(float *out, float *in);

typedef struct Node {
    float a;            /* +0x00 (0xc1c0) */
    float b;            /* +0x04 */
    float c;            /* +0x08 (0xc1c8) */
    float x, y, z;      /* +0x0c..+0x14 (0xc1cc..0xc1d4) */
    char pad[0x4c - 0x18];
} Node;

typedef struct Graph {
    float dist[12][512];        /* +0x0000 */
    float ang[12][512];         /* +0x6000 */
    char pad0[0xc1c0 - 0xc000];
    Node nodes[512];            /* +0xc1c0 */
    char pad1[0x16244 - (0xc1c0 + 512 * 0x4c)];
    int count;                  /* +0x16244 */
} Graph;

// MATCH: 0x004783f0 _FUN_004783f0
void FUN_004783f0(Graph *g)
{
    int i, cur, next, more;
    float dx, dy, dz, d, a;

    float v[3];

    for (i = 0; i < g->count; i++) {
        more = 1;
        cur = FUN_00409300(i);
        do {
            next = FUN_00409330(cur, i);
            if (next == FUN_00409300(i))
                more = 0;
            v[2] = 0.0f;
            dx = g->nodes[cur].x - g->nodes[next].x;
            dy = g->nodes[cur].y - g->nodes[next].y;
            dz = g->nodes[cur].z - g->nodes[next].z;
            g->dist[i][cur] = FUN_004c3b30(dx * dx + dy * dy + dz * dz);
            v[0] = g->nodes[cur].a;
            v[1] = g->nodes[cur].c;
            FUN_004c39b0(v, v);
            d = v[0] + v[1] * 0.0f; d += v[2] * 0.0f;
            if (d < -1.0)
                d = -1.0f;
            if (d > 1.0)
                d = 1.0f;
            a = (float)FUN_004a3384(d) * 57.29578f;
            if (v[0] * 0.0f - v[1] < 0.0f)
                a = -a;
            while (a <= 0.0f)
                a += 360.0f;
            g->ang[i][cur] = a;
            cur = next;
        } while (more);
    }
}
