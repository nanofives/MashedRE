// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00418990 FUN_00418990
/* h1 MATCH-S03: 5-byte thunk, jmp 0x004194f0. Written as a void tail call. */
#ifndef MATCH_PORT
void FUN_004194f0(void);
#endif

void FUN_00418990(void)
{
    FUN_004194f0();
}
