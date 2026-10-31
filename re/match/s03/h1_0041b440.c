// FLAGS: /O2 /GS /Ob1
// MATCH: 0x0041b440 FUN_0041b440
/* h1 MATCH-S03: record in EAX (Ghidra in_EAX), RpClumpDestroy(rec+0x5c) (0x004e6e00, cdecl, 1 arg:
   call + pop ecx). Register arg = static callee defined before its caller; caller_0041b4xx is a
   stand-in caller (not matched). */
#ifndef MATCH_PORT
typedef struct Rec41b440 {
    char pad[0x5c];
    void *clump;   /* +0x5c */
} Rec41b440;
int RpClumpDestroy(void *clump);
extern Rec41b440 *DAT_stand_in_41b440;
#endif

static void FUN_0041b440(Rec41b440 *rec)
{
    RpClumpDestroy(rec->clump);
}

#ifndef MATCH_PORT
/* stand-in caller, not matched, not shipped */
void caller_0041b4xx(void)
{
    FUN_0041b440(DAT_stand_in_41b440);
}
#endif
