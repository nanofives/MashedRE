// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00417740 FUN_00417740
/* h1 MATCH-S03: indexed float -> int from the float array at 0x0089a870 (fld + tail jmp to __ftol
   0x004a2c48). Ghidra's `void (void)` prototype is wrong: it reads [esp+4] and returns __ftol's EAX. */
#ifndef MATCH_PORT
extern float DAT_0089a870[];
#endif

int FUN_00417740(int i)
{
    return (int)DAT_0089a870[i];
}
