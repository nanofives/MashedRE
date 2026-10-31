// FLAGS: /O2 /GS /Ob1
// MATCH: 0x004161e0 FUN_004161e0
/* 0x004161e0 (70 B). 100.0% byte-identical (--bytes). Converter was 61.5% from undefined4 locals in
   the wrong slots. A float[2] after the int out-param gives the original frame. Class: none left. */
#ifndef MATCH_PORT
void FUN_00443dc0(int p0, float *p1, float *p2, int p3, int p4, int p5);
void FUN_0046d4a0(int *p0, int p1);
#endif

void FUN_004161e0(int a, float *b, int idx)
{
  int obj;
  float xz[2];

  FUN_0046d4a0(&obj, idx);
  xz[0] = *(float *)(obj + 0x30);
  xz[1] = *(float *)(obj + 0x38);
  FUN_00443dc0(a, xz, b, idx, 1, 0);
}
