// FLAGS: /O2 /GS /Ob1
// MATCH-S01. 0x00402f50 and 0x00402f80 write one 0x14-byte block at 0x00636ae8: four floats and a dword.
// 0x00402f80 stores 0 as immediates (float 0.0f), 0x00402f50 zeroes all five dwords through EAX (inlined memset).
typedef struct { float f0; float f4; float f8; float fc; int f10; } Block636ae8;
#ifndef MASHED_RE_PORT
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
MGLOBAL(Block636ae8, DAT_00636ae8, 0x00636ae8);
void *memset(void *, int, unsigned int);
#endif
#pragma intrinsic(memset)

// 0x00402f50
// MATCH: 0x00402f50 FUN_00402f50
void FUN_00402f50(void)
{
  memset(&DAT_00636ae8, 0, sizeof(DAT_00636ae8));
  DAT_00636ae8.fc = 40.0f;
  DAT_00636ae8.f4 = 60.0f;
}

// 0x00402f80
// MATCH: 0x00402f80 FUN_00402f80
void FUN_00402f80(void)
{
  DAT_00636ae8.f0 = 0.0f;
  DAT_00636ae8.f8 = 0.0f;
  DAT_00636ae8.fc = 40.0f;
  DAT_00636ae8.f4 = 60.0f;
}
