// FLAGS: /O2 /GS /Ob1
/* a1 0x00401f10 (202 B). Best: 100.0% BYTES-OK. Start (g2s) 97.1% (two swapped SIB base/index).
   Fix: DAT_00636ac0 typed as Slot* (the 0x68-stride struct of 0x004015a0), loops indexing ->a[i] / ->b[i],
   and `int i = 0;` initialised before the render-state calls (9176 reuses esi=0 for the `push 0` args). */
typedef struct Slot {
  int clump;        /* +0x00 */
  int f04, f08, f0c;
  int pad10[9];     /* +0x10 */
  float scale;      /* +0x34 */
  int f38;          /* +0x38 */
  int pad3c;
  int a[4];         /* +0x40 */
  int b[6];         /* +0x50 */
} Slot;             /* 0x68 */
typedef void (*RsFn)(int, int);
typedef struct RwGlob { int pad[8]; RsFn setState; } RwGlob;
extern RwGlob *DAT_007d3ff8;
extern Slot *DAT_00636ac0;
int FUN_0040e340(void);
int FUN_00412130(void);
int FUN_004e6680(int p0);
// MATCH: 0x00401f10 FUN_00401f10
void FUN_00401f10(void)
{
  int i = 0;
  DAT_007d3ff8->setState(0x14, 1);
  DAT_007d3ff8->setState(8, 0);
  DAT_007d3ff8->setState(6, 0);
  if (DAT_00636ac0->clump != 0) {
    FUN_004e6680(DAT_00636ac0->clump);
  }
  for (; i < FUN_0040e340(); i++) {
    FUN_004e6680(DAT_00636ac0->a[i]);
  }
  for (i = 0; i < FUN_00412130(); i++) {
    FUN_004e6680(DAT_00636ac0->b[i]);
  }
  DAT_007d3ff8->setState(6, 1);
  DAT_007d3ff8->setState(8, 1);
  DAT_007d3ff8->setState(0x14, 2);
}
