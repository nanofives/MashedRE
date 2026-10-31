// a3 matching pilot, 0x00475a00 (91 B). Best: 93.1% (regs 100, ops 100) 29/29 ins.
// Only residual: the two stores at 0x00475a3f / 0x00475a47 encode base=EDI (idx*4), index=EDX/EAX (the
// loaded array pointer): `89 04 17` / `89 34 07`. cl 13.00.9176/9178 always emits base=pointer,
// index=offset (`[edx+edi]`). Tried (none moves it): int globals + byte offset in both operand orders,
// char* globals, unsigned offset, char* offset + int global, int* globals with [i] (gives a scaled index,
// loses the separate shl at 0x00475a31, 89.7%), struct-of-globals (72.4%), offset/4 indexing, separate
// pointer temp, FUN_00474db0 defined in the TU (matches 100% itself, does not change this), permuter
// 126 compiles no improvement. Class (a) candidate: SIB base/index order for a reg+reg address with
// identical operations/registers. The shl-then-add form proves the source is a byte offset added to a
// pointer-sized int, so only operand order inside the address differs.
// FLAGS: /O2 /GS /Ob1
extern int DAT_0069160c;
extern int DAT_00691608;
extern int DAT_00691604; extern int DAT_00691610;
int FUN_00474db0(int p0, int p1);
void FUN_004770c0(int p0, unsigned int p1, int p2, int p3);
// MATCH: 0x00475a00 _FUN_00475a00
void FUN_00475a00(int param_1, int param_2)
{
  FUN_004770c0(DAT_0069160c * 0x40 + DAT_00691608, 0x80c, param_2, param_1);
  { int o = DAT_0069160c * 4; *(int *)(o + DAT_00691604) = FUN_00474db0(param_2, 0x50); *(int *)(o + DAT_00691610) = param_2; }
  DAT_0069160c = DAT_0069160c + 1;
}
