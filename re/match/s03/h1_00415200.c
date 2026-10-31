// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00415200 FUN_00415200
/* h1 MATCH-S03: returns 1 when FUN_00442cc0(0) == the float at 0x005d757c, else 0. */
#ifndef MATCH_PORT
extern const float DAT_005d757c;
float FUN_00442cc0(int i);
#endif

int FUN_00415200(void)
{
    if (FUN_00442cc0(0) == DAT_005d757c)
        return 1;
    return 0;
}
