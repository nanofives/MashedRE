// FLAGS: /O2 /GS /Ob1
/* a1 0x00432080 (417 B). Best: 100% exact (110/110).
   Start (g2s) 36.4%: Ghidra's goto/if-chain. Rewritten as a switch on DAT_007f1a0c (cases 1, 2, 0x1000:
   the `dec / dec / sub 0xffe` chain at 0x0043209c..0x004320aa), the 8 checks of case 0x1000 written out
   (bytes at 0x007f1041 + i*0x4c and 0x007f1501 + i*0x4c, i = 0..7; the original has no loop there), the
   case-2 loop as an int index over a 4 x 16-byte table at 0x007f1a14 (compiler turns it into the
   `cmp esi,0x7f1a54 / jl` pointer test) -> 95.6%. Last step: `if (DAT_0067eca4 == 0) { switch ... }`
   instead of an early `return 0`, which moves `push ebp` ahead of the test and shares the return-0 tail.
   Residual: none. Gap classes found: (b) control-flow shape (switch + nesting), converter global typing. */
typedef struct { int id; int a, b, c; } Ent;
extern int DAT_0067eca4;
extern int DAT_0067f19c;
extern int DAT_007f1a0c;
extern Ent DAT_007f1a14[4];
extern char DAT_007f1041[];
extern char DAT_007f1501[];
extern int DAT_0067ecac, DAT_0067e844, DAT_0067e9f8, DAT_0067e914;
int FUN_0040e470(int p0);
void FUN_0042d3a0(void);

#define OK(i) (DAT_007f1041[(i) * 0x4c] != 0 && DAT_007f1501[(i) * 0x4c] == 0)

// MATCH: 0x00432080 _FUN_00432080
int FUN_00432080(int param_1)
{
  int i;
  DAT_0067f19c = 0;
  if (DAT_0067eca4 == 0) {
  switch (DAT_007f1a0c) {
  case 1:
    if (DAT_007f1a14[0].id != -1 && OK(DAT_007f1a14[0].id))
      DAT_0067eca4 = 1;
    break;
  case 2:
    for (i = 0; i < 4; i++) {
      if (FUN_0040e470(i) == 1 && DAT_007f1a14[i].id != -1 && OK(DAT_007f1a14[i].id))
        DAT_0067eca4 = 1;
    }
    break;
  case 0x1000:
    if (OK(0)) DAT_0067eca4 = 1;
    if (OK(1)) DAT_0067eca4 = 1;
    if (OK(2)) DAT_0067eca4 = 1;
    if (OK(3)) DAT_0067eca4 = 1;
    if (OK(4)) DAT_0067eca4 = 1;
    if (OK(5)) DAT_0067eca4 = 1;
    if (OK(6)) DAT_0067eca4 = 1;
    if (OK(7)) DAT_0067eca4 = 1;
    break;
  default:
    return 0;
  }
  if (DAT_0067eca4 == 1) {
    FUN_0042d3a0();
    DAT_0067ecac = param_1;
    DAT_0067e844 = 0;
    DAT_0067e9f8 = 0;
    DAT_0067e914 = 0;
    DAT_0067f19c = 2;
    return 1;
  }
  }
  return 0;
}
