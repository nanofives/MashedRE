// FLAGS: /O2 /GS /Ob1
// MATCH: 0x004190f0 FUN_004190f0
/* h1 MATCH-S03: record in EAX (Ghidra in_EAX), zeroes +0x4c and +0x50. Register arg = static callee
   defined before its caller; caller_004190xx is a stand-in caller (not matched). */
#ifndef MATCH_PORT
typedef struct Rec4190f0 {
    char pad[0x4c];
    int  f4c;   /* +0x4c */
    int  f50;   /* +0x50 */
} Rec4190f0;
extern Rec4190f0 *DAT_stand_in_4190f0;
#endif

static void FUN_004190f0(Rec4190f0 *rec)
{
    rec->f4c = 0;
    rec->f50 = 0;
}

#ifndef MATCH_PORT
/* stand-in caller, not matched, not shipped */
void caller_004190xx(void)
{
    FUN_004190f0(DAT_stand_in_4190f0);
}
#endif
