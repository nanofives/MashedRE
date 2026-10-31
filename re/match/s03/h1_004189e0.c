// FLAGS: /O2 /GS /Ob1
// MATCH: 0x004189e0 FUN_004189e0
/* h1 MATCH-S03: 5-byte thunk, jmp 0x004196f0. Written as a void tail call. */
#ifndef MATCH_PORT
void FUN_004196f0(void);
#endif

void FUN_004189e0(void)
{
    FUN_004196f0();
}
