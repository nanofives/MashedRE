// FLAGS: /O2 /GS /Ob1
// MATCH-S01. Int return: the zero path returns the tested EAX itself.
#ifndef MASHED_RE_PORT
extern int DAT_0063a5d0;
extern int DAT_0063a5d4;
#endif

// 0x00405890
// MATCH: 0x00405890 FUN_00405890
int FUN_00405890(void)
{
  if (DAT_0063a5d0 == 0) {
    return 0;
  }
  return DAT_0063a5d4 == DAT_0063a5d0;
}
