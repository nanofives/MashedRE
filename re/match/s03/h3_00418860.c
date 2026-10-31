// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00418860 FUN_00418860
/* 0x00418860 (280 B). 100.0% byte-identical (--bytes, relocations masked). Converter was 97.9%: Ghidra gave
   FUN_00418560 one arg, the caller pushes two (add esp, 8). Fixing the prototype matched. Class: none left. */
#ifndef MATCH_PORT
extern int DAT_00801ca0;
extern int DAT_007f0fd0;
int  FUN_0040e470(int p0);
void FUN_0040e480(int p0, int p1);
void FUN_004177b0(void);
void FUN_00418560(int p0, int p1);
int  FUN_0042f6a0(void);
int  FUN_00443080(void);
int  FUN_0046c7b0(int p0);
#endif

void FUN_00418860(void)
{
  int i;
  int v;

  if (DAT_00801ca0 >= 4) {
    if (FUN_0042f6a0() == 3 || FUN_0042f6a0() == 4 || FUN_0042f6a0() == 5 || FUN_0042f6a0() == 10 ||
        DAT_007f0fd0 == 4 || DAT_007f0fd0 == 9 || DAT_007f0fd0 == 8 || DAT_007f0fd0 == 10) {
      if (FUN_0046c7b0(1) == 1) FUN_0040e480(1, 2);
      if (FUN_0046c7b0(2) == 1) FUN_0040e480(2, 2);
      if (FUN_0046c7b0(3) == 1) FUN_0040e480(3, 2);
    }
    FUN_004177b0();
    i = 0;
    do {
      v = FUN_0040e470(i);
      if ((v != 0 && v != 1) || FUN_00443080() == 1) {
        if (FUN_0046c7b0(i) == 1) FUN_00418560(i, v);
        if ((FUN_0042f6a0() == 3 || FUN_0042f6a0() == 4 || FUN_0042f6a0() == 5) && DAT_007f0fd0 == 7)
          FUN_00418560(0, v);
      }
      i++;
    } while (i < 4);
  }
}
