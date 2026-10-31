// FLAGS: /O2 /GS /Ob1
/* a2 0x00403250 (583 B). Best: 100%.
   g2s did not compile (bool typedef clash). Rewritten as 8 unrolled checks over two 0x4c-stride slot
   arrays (0x007f1038, bytes +9/+0xa, and 0x007f14f8 = 0x007f1038 + 16 slots), float timer globals,
   int flag. Same slot array as 0x004030d0 and the pilot 0x00432080. 2.0f = 0x005cc574. */
typedef struct {
  char pad0[9];
  char m_09;        /* 0x7f1041 + i*0x4c */
  char m_0a;        /* 0x7f1042 + i*0x4c */
  char pad1[0x4c - 11];
} Slot4c;
extern float DAT_00636af0;
extern float DAT_00636af4;
extern Slot4c DAT_007f1038[8];
extern Slot4c DAT_007f14f8[8];
int FUN_0042b920(void);
void FUN_00433240(int p0);
int FUN_00443080(void);
void FUN_00448700(int p0, int p1);
void FUN_0045bed0(void);
void FUN_0045bf30(void);
void FUN_00493590(void);

#define CHK(i) \
  if (((DAT_007f1038[i].m_09 != 0 && DAT_007f14f8[i].m_09 == 0) || \
       (DAT_007f1038[i].m_0a != 0 && DAT_007f14f8[i].m_0a == 0)) && DAT_00636af0 > 2.0f) \
    hit = 1;

// MATCH: 0x00403250 FUN_00403250
void FUN_00403250(float dt)
{
  int hit;
  DAT_00636af0 = DAT_00636af0 + dt;
  hit = 0;
  if (FUN_00443080() == 0)
    FUN_00448700(1, 0);
  CHK(0) CHK(1) CHK(2) CHK(3) CHK(4) CHK(5) CHK(6) CHK(7)
  if (DAT_00636af0 > DAT_00636af4 || hit) {
    FUN_00433240(FUN_0042b920());
    FUN_00493590();
    FUN_00448700(0, 0);
    FUN_0045bed0();
    FUN_0045bf30();
  }
}
