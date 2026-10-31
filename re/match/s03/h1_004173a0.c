// FLAGS: /O2 /GS /Ob1
// MATCH: 0x004173a0 FUN_004173a0
/* h1 MATCH-S03: float getter for DAT_0089a360. */
#ifndef MATCH_PORT
extern float DAT_0089a360;
#endif

float FUN_004173a0(void)
{
    return DAT_0089a360;
}
