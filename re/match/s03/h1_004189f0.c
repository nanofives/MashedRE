// FLAGS: /O2 /GS /Ob1
// MATCH: 0x004189f0 FUN_004189f0
/* h1 MATCH-S03: 5-byte thunk, jmp 0x00419760. 0x00419760 takes (int, int) per decomp; the typed
   forwarding call also compiles to the bare jmp (cl tail-call), so the prototype is kept typed. */
#ifndef MATCH_PORT
void FUN_00419760(int param_1, int param_2);
#endif

void FUN_004189f0(int param_1, int param_2)
{
    FUN_00419760(param_1, param_2);
}
