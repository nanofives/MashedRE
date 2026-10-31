// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00412880 FUN_00412880
/* h1 MATCH-S03: 5-byte thunk, jmp 0x00412190. Written as a void tail call. */
#ifndef MATCH_PORT
void FUN_00412190(void);
#endif

void FUN_00412880(void)
{
    FUN_00412190();
}
