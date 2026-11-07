// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00426640 _FUN_00426640
int FUN_00426640(int a) { int r; __asm { mov eax, a } __asm { mov r, eax } return r; }
