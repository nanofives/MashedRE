// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00417730 FUN_00417730
/* h1 MATCH-S03: indexed float getter from the float array at 0x0089a880. */
#ifndef MATCH_PORT
extern float DAT_0089a880[];
#endif

float FUN_00417730(int i)
{
    return DAT_0089a880[i];
}
