// FLAGS: /O2 /GS /Ob1
// MATCH-S01. 0x00401570 and 0x00401ee0: the 13-entry table of 0x68-byte records at 0x00636578 and the current-entry
// pointer right after it (0x00636ac0). The loop end is the array end (a relocated address), compared signed (jl).
typedef struct { int f00[14]; int key; int f3c[11]; } Entry636578;   /* 0x68 bytes, key at +0x38 */
#ifndef MASHED_RE_PORT
extern Entry636578 DAT_00636578[13];
extern Entry636578 *DAT_00636ac0;
void FUN_00401da0(void);
void FUN_004e6680(Entry636578 *e);
#endif

// 0x00401570
// MATCH: 0x00401570 FUN_00401570
void FUN_00401570(int key)
{
  Entry636578 *e;

  e = DAT_00636578;
  DAT_00636ac0 = DAT_00636578;
  do {
    if (e->key == key) {
      DAT_00636ac0 = e;
    }
    e++;
  } while ((int)e < (int)(DAT_00636578 + 13));
}

// 0x00401ee0
// MATCH: 0x00401ee0 FUN_00401ee0
void FUN_00401ee0(int key)
{
  FUN_00401570(key);
  FUN_00401da0();
  if (*(int *)DAT_00636ac0 != 0) {
    FUN_004e6680(*(Entry636578 **)DAT_00636ac0);
  }
}
