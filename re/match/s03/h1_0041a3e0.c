// FLAGS: /O2 /GS /Ob1
// MATCH: 0x0041a3e0 FUN_0041a3e0
/* h1 MATCH-S03: if DAT_0063c628 != 0, tail-call 0x0041c2d0 with EAX = &DAT_0063c610 (register arg).
   The register arg needs FUN_0041c2d0 as a static callee defined before the caller (pilot rule), so the
   file carries a copy of 0x0041c2d0's body. Not claimed (not on h1's list), but with a MATCH line it is
   also BYTE-IDENTICAL (35 B), so it is the real callee, not a stand-in. */
#ifndef MATCH_PORT
typedef struct Obj41c2d0 {
    char pad[0x48];
    void (*fn48)(struct Obj41c2d0 *self);
} Obj41c2d0;
typedef struct Set41c2d0 {
    Obj41c2d0 *a;   /* +0x0 */
    Obj41c2d0 *b;   /* +0x4 */
    Obj41c2d0 *c;   /* +0x8 */
    Obj41c2d0 *d;   /* +0xc */
} Set41c2d0;
extern int DAT_0063c628;
extern Set41c2d0 DAT_0063c610;
#endif

/* stand-in for 0x0041c2d0 (EAX = set) */
static void FUN_0041c2d0(Set41c2d0 *s)
{
    s->d->fn48(s->d);
    s->b->fn48(s->b);
    s->a->fn48(s->a);
    s->c->fn48(s->c);
}

void FUN_0041a3e0(void)
{
    if (DAT_0063c628 != 0) {
        FUN_0041c2d0(&DAT_0063c610);
    }
}
