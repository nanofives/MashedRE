// FLAGS: /O2 /GS /Ob1
/* 0x004058c0 (25 B). Best 100.0%. Takes an index; tail-calls FUN_004058b0(&DAT_00639d80[i], f) on the 0xec-byte record array (mov [esp+4],ecx; jmp). */
typedef struct { char pad[0xec]; } S_EC;
extern S_EC DAT_00639d80[];
void FUN_004058b0(S_EC *p, float f);
// MATCH: 0x004058c0 FUN_004058c0
void FUN_004058c0(int i, float f)
{
    FUN_004058b0(&DAT_00639d80[i], f);
}
