// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00418980 FUN_00418980
/* h1 MATCH-S03: 5-byte thunk, jmp 0x0041a060. Written as a void tail call. */
#ifndef MATCH_PORT
void FUN_0041a060(void);
#endif

void FUN_00418980(void)
{
    FUN_0041a060();
}
