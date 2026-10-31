// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00419480 FUN_00419480
/* h1 MATCH-S03 r2: record in EAX (static callee, register arg). When rec+0x50 != 0, for each of the 6
   flags at +0x00..+0x14 that is nonzero calls slot[i]->fn48(slot[i]) (slots at +0x20..+0x34).
   Record = the 0x54-stride records at 0x0063bf30 (layout as pilot 0x00419090 + 0x004190f0).
   caller_004194xx is a stand-in caller (not matched, not shipped). */
typedef struct Obj48 {
    char pad[0x48];
    void (*fn48)(struct Obj48 *self);
} Obj48;
typedef struct Rec63bf30 {
    int    flag[6];    /* +0x00 */
    char   pad18[0x20 - 0x18];
    Obj48 *slot[8];    /* +0x20 */
    int    obj;        /* +0x40 */
    int    obj4;       /* +0x44 */
    int    idx;        /* +0x48 */
    int    f4c;        /* +0x4c */
    int    f50;        /* +0x50 */
} Rec63bf30;           /* 0x54 */
#ifndef MATCH_PORT
extern Rec63bf30 DAT_0063bf30[2];
#endif

static void FUN_00419480(Rec63bf30 *rec)
{
    if (rec->f50 != 0) {
        if (rec->flag[0] != 0)
            rec->slot[0]->fn48(rec->slot[0]);
        if (rec->flag[1] != 0)
            rec->slot[1]->fn48(rec->slot[1]);
        if (rec->flag[2] != 0)
            rec->slot[2]->fn48(rec->slot[2]);
        if (rec->flag[3] != 0)
            rec->slot[3]->fn48(rec->slot[3]);
        if (rec->flag[4] != 0)
            rec->slot[4]->fn48(rec->slot[4]);
        if (rec->flag[5] != 0)
            rec->slot[5]->fn48(rec->slot[5]);
    }
}

#ifndef MATCH_PORT
/* stand-in caller, not matched, not shipped */
void caller_004194xx(void)
{
    int i;
    for (i = 0; i < 2; i++)
        FUN_00419480(&DAT_0063bf30[i]);
}
#endif
