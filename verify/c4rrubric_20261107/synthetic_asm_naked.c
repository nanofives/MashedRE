// FLAGS: /O2 /GS /Ob1
// MATCH: 0x004264d0 _FUN_004264d0
__declspec(naked) int FUN_004264d0(void) { __asm { mov eax, 1 } __asm { ret } }
