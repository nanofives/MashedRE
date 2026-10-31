// FLAGS: /O2 /GS /Ob1
// MATCH-S01. The four 0x30c-byte per-car blocks at 0x008a9620: 0x00407a60 zeroes each and sets +0x28 to -1,
// 0x00408a50 / 0x00408ad0 read the floats at +0xc8 / +0xcc.
typedef struct {
  int f000[10];
  int f028;
  int f02c[39];
  float f0c8;
  float f0cc;
  int f0d0[143];
} CarBlk8a9620;   /* 0x30c bytes */
#ifndef MASHED_RE_PORT
extern CarBlk8a9620 DAT_008a9620[4];
void *memset(void *, int, unsigned int);
#endif
#pragma intrinsic(memset)

// 0x00407a60
// MATCH: 0x00407a60 FUN_00407a60
void FUN_00407a60(void)
{
  int i;

  for (i = 0; i < 4; i++) {
    memset(&DAT_008a9620[i], 0, sizeof(CarBlk8a9620));
    DAT_008a9620[i].f028 = -1;
  }
}

// 0x00408a50
// MATCH: 0x00408a50 FUN_00408a50
float FUN_00408a50(int i)
{
  return DAT_008a9620[i].f0c8;
}

// 0x00408ad0
// MATCH: 0x00408ad0 FUN_00408ad0
float FUN_00408ad0(int i)
{
  return DAT_008a9620[i].f0cc;
}
