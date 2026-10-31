// FLAGS: /O2 /GS /Ob1
// MATCH-S01. 0x00404e80 (buffer -> live) and 0x00404ee0 (live -> buffer): a 0x520-byte block, one byte per
// 0x4c-byte record for 12 records (index loop, signed compare), one dword, and a 0x928f-dword block behind a pointer.
// 0x00828254 lies inside the 0x520-byte block at 0x00827d98 (+0x4bc): the counter store is written after the block
// copy, the order the original's bytes execute, so a compiler that sees the overlap keeps the original's result.
// 0x00404ee0 ends with the 0xdeadbeef store to 0x00803358 (past Ghidra's 92-byte body; pc_funcs body_end 0x00404f42).
typedef struct { int d[0x148]; } Blk520;
typedef struct { int d[0x928f]; } Blk2423c;
typedef struct { int v; int pad[18]; } Rec4c;
#ifndef MASHED_RE_PORT
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
MGLOBAL(Blk520, DAT_007f0a40, 0x007f0a40);
MGLOBAL(Blk520, DAT_00827d98, 0x00827d98);
MGLOBALA(Rec4c, DAT_007f105c, 12, 0x007f105c);
MGLOBALA(unsigned char, DAT_007f0f54, 12, 0x007f0f54);
MGLOBAL(int, DAT_008a95ac, 0x008a95ac);
MGLOBAL(int, DAT_00828254, 0x00828254);
MGLOBAL(Blk2423c *, DAT_008a94a8, 0x008a94a8);
MGLOBAL(Blk2423c, DAT_0080335c, 0x0080335c);
MGLOBAL(unsigned int, DAT_00803358, 0x00803358);
#endif

// 0x00404e80
// MATCH: 0x00404e80 FUN_00404e80
void FUN_00404e80(void)
{
  int i;

  DAT_007f0a40 = DAT_00827d98;
  for (i = 0; i < 12; i++) {
    DAT_007f105c[i].v = DAT_007f0f54[i];
  }
  DAT_008a95ac = DAT_00828254;
  if (DAT_008a94a8 != 0) {
    *DAT_008a94a8 = DAT_0080335c;
  }
}

// 0x00404ee0
// MATCH: 0x00404ee0 FUN_00404ee0
void FUN_00404ee0(void)
{
  int i;

  for (i = 0; i < 12; i++) {
    DAT_007f0f54[i] = (unsigned char)DAT_007f105c[i].v;
  }
  DAT_00827d98 = DAT_007f0a40;
  DAT_00828254 = DAT_008a95ac;
  if (DAT_008a94a8 != 0) {
    DAT_0080335c = *DAT_008a94a8;
  }
  DAT_00803358 = 0xdeadbeef;
}
