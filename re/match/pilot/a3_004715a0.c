// a3 matching pilot, 0x004715a0 (Ghidra body 456 B, real extent 0x004715a0..0x0047177c = 477 B).
// Best: 100.0% (regs 100, ops 100) 136/136 ins when scored over the real 477-byte extent
// (match.score_function with size=0x1dd). Stock match.py reports 93.4%: pc_funcs.csv size 456 is the
// body byte count without the 21 bytes of in-function alignment padding (0x004715d4..0x004715df,
// 0x004715f7..0x004715ff), so the compare stops at 0x00471768 and the last 9 instructions
// (cmp ebx,0x8815a0 / mov [esp+0x20],ebx / jl / pops / add esp,0x14 / ret) count as extra. Class (c).
// What moved it: struct layout (8 records of 0x2510 at 0x0086ece0, flag byte +0x0, ids +0x20, count +0x40,
// a[] +0x44, b[] +0x84.., total +0x144, 0x45c-byte sub-records from +0x154, nsub +0x2428, 0x1c-byte
// entries from +0x2434, ne +0x250c; table of 0x110-byte entries at 0x0086cbc4, base +0x0, n +0x8);
// outer loop as an index loop (pointer loop became a down-counter); i = 0 hoisted before the flag test;
// `j = 0; counts[c] = j;` (store of the zero register at 0x004716b6); temp `v` for the call argument in
// the second loop (eax vs edx at 0x004716c0). Permuter (187 compiles) found nothing; the temp was manual.
// FLAGS: /O2 /GS /Ob1
typedef struct T110 { int base; int pad04; int n; int pad0c[(0x110 - 0x0c) / 4]; } T110;
typedef struct Sub45c {
    int ids[8];            /* +0x000 */
    int count;             /* +0x020 */
    int pad24[2];
    int data[8][0x20];     /* +0x02c */
    int counts[8];         /* +0x42c */
    int pad44c[4];         /* -> 0x45c */
} Sub45c;
typedef struct E1c { int a; int pad04[2]; int c; int d; int pad14[2]; } E1c;
typedef struct R2510 {
    char flag;             /* +0x000 */
    char pad01[0x1f];
    int ids[8];            /* +0x020 */
    int count;             /* +0x040 */
    int a[0x20];           /* +0x044 */
    int b[0x20];           /* +0x0c4 */
    int total;             /* +0x144 */
    int pad148[3];
    Sub45c sub[7];         /* +0x154 .. 0x1fd8 */
    char pad1fd8[0x2428 - 0x1fd8];
    int nsub;              /* +0x2428 */
    int pad242c[2];
    E1c e[7];              /* +0x2434 */
    char pad24f8[0x250c - 0x24f8];
    int ne;                /* +0x250c */
} R2510;
extern R2510 DAT_0086ece0[8];
extern T110 DAT_0086cbc4[];
int FUN_0047ce40(int p0);
// MATCH: 0x004715a0 _FUN_004715a0
void FUN_004715a0(void)
{
    R2510 *r;
    Sub45c *s;
    int i, j, k, c, id;
    int v;

    for (k = 0; k < 8; k++) { r = &DAT_0086ece0[k];
        i = 0;
        r->total = 0;
        if (r->flag) {
            for (; i < r->count; i++) {
                id = r->ids[i];
                for (j = 0; j < DAT_0086cbc4[id].n; j++) {
                    v = DAT_0086cbc4[id].base + j;
                    r->a[r->total + j] = v;
                    r->b[r->total + j] = FUN_0047ce40(v);
                }
                r->total += DAT_0086cbc4[id].n;
            }
            for (i = 0; i < r->nsub; i++) {
                s = &r->sub[i];
                for (c = 0; c < s->count; c++) {
                    id = s->ids[c];
                    j = 0;
                    s->counts[c] = j;
                    for (; j < DAT_0086cbc4[id].n; j++) {
                        v = DAT_0086cbc4[id].base + j;
                        s->data[c][j] = FUN_0047ce40(v);
                        s->counts[c]++;
                    }
                }
            }
            for (i = 0; i < r->ne; i++) {
                r->e[i].c = 1;
                r->e[i].d = 0;
                r->e[i].a = 1;
            }
        }
    }
}
