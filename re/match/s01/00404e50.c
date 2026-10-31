// FLAGS: /O2 /GS /Ob1
// MATCH-S01. Save file load / write / exists: the file name at 0x005cc8e0 ("gamesave.bin"), the 0x24fa0-byte save
// buffer at 0x00803358, the file read / write / exists helpers 0x004b3b70 / 0x004b3bb0 / 0x00550b00, then
// FUN_004099e0(0). The name is a named global (not a literal) so the port can bind it per target.
#ifndef MASHED_RE_PORT
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
MGLOBALA(char, s_gamesave_bin_005cc8e0, 13, 0x005cc8e0);
MGLOBALA(unsigned char, DAT_00803358, 0x24fa0, 0x00803358);
int FUN_004b3b70(char *path, void *buf, unsigned int size);
int FUN_004b3bb0(char *path, void *buf, unsigned int size);
int FUN_00550b00(char *path);
void FUN_004099e0(int v);
#endif

// 0x00404e50
// MATCH: 0x00404e50 FUN_00404e50
int FUN_00404e50(void)
{
  FUN_004b3b70(s_gamesave_bin_005cc8e0, DAT_00803358, 0x24fa0);
  FUN_004099e0(0);
  return 0;
}

// 0x00404f50
// MATCH: 0x00404f50 FUN_00404f50
int FUN_00404f50(void)
{
  FUN_004b3bb0(s_gamesave_bin_005cc8e0, DAT_00803358, 0x24fa0);
  FUN_004099e0(0);
  return 0;
}

// 0x00404f80
// MATCH: 0x00404f80 FUN_00404f80
int FUN_00404f80(void)
{
  return FUN_00550b00(s_gamesave_bin_005cc8e0) != 0;
}
