// FLAGS: /O2 /GS /Ob1
// MATCH-S01. FUN_00426bd0(i, &lo, &hi) fills two shorts per entry; FUN_00426bc0 is the entry count.
#ifndef MASHED_RE_PORT
int FUN_00426bc0(void);
void FUN_00426bd0(int i, short *lo, short *hi);
#endif

// 0x00409290
// MATCH: 0x00409290 FUN_00409290
void FUN_00409290(int *v)
{
  int n;
  int i;
  short first;
  short hi;
  short lo;

  n = FUN_00426bc0();
  first = 0;
  if (n != 0) {
    for (i = 0; i < n; i++) {
      FUN_00426bd0(i, &lo, &hi);
      if (i == 0) {
        first = lo;
      }
      if (*v < lo) {
        *v = lo;
        return;
      }
      if (*v <= hi) {
        return;
      }
    }
    *v = first;
  }
}
