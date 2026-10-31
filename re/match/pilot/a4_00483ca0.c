// FLAGS: /O2 /GS /Ob1
/* a4 / 0x00483ca0 (102 B): best 100.0% (regs 100, ops 100), 2 iterations.
 * Tried: Ghidra raw 29.8%; struct-assign + memcpy intrinsic 71.8% (r->data loaded after the
 * loop, in esi, plus a loop-alignment nop); hoisting `data = r->data` above the search loop
 * (as Ghidra's piVar5 shows) put it in ebx and gave 100%. Residual: none. */
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)

typedef struct Rec {
    int type;           /* +0x000 */
    int size;           /* +0x004 */
    int id;             /* +0x008 */
    int f0c[2];
    void *data;         /* +0x014 */
    int f18[0x55];
    int count;          /* +0x16c */
    int f170[0xb];
} Rec;                  /* 0x19c */

// MATCH: 0x00483ca0 _FUN_00483ca0
int FUN_00483ca0(Rec *r, Rec **pp)
{
    void *data = r->data;
    Rec *c = *pp;
    while (!((c->id == r->id && c->type == 0x10b) || (c->id == 0 && c->type == 0)))
        c = (Rec *)((char *)c + c->size);
    *c = *r;
    memcpy(c + 1, data, r->count * 36);
    return 1;
}
