// FLAGS: /O2 /GS /Ob1
/* a1 0x004013f0 (181 B). Best: 100.0% BYTES-OK. Start (g2s) 95.1% (one missing `add esp,8`).
   Fix: FUN_00426060 takes NO argument. The `push esi` before `call 0x00426060` is the second argument (cam) of
   the outer FUN_004e4350 / FUN_004e4320 call, so both are 2-arg (world, cam) calls with a nested
   FUN_00426060() world argument; the bottom pair flushes its pending pops before the nested call because an
   outer argument is already pushed. Ghidra's arity for 0x00426060 (1) and 0x004e4320/0x004e4350 (varies) is wrong. */
typedef int (*Fn2)(int, int);
extern int **DAT_007d3ff8;
extern int DAT_00636560;
extern int DAT_00636564;
void FUN_004011f0(void);
int FUN_00426060(void);
void FUN_004c19f0(int p0);
int FUN_004c1a00(int p0);
void FUN_004e4320(int world, int cam);
int FUN_004e4350(int world, int cam);
int FUN_004e6680(int p0);
// MATCH: 0x004013f0 FUN_004013f0
void FUN_004013f0(void)
{
  int cam;
  cam = (int)*DAT_007d3ff8;
  if (cam != 0) {
    FUN_004c19f0(cam);
  }
  FUN_004e4350(FUN_00426060(), cam);
  FUN_004e4320(DAT_00636560, cam);
  FUN_004011f0();
  if (FUN_004c1a00(cam) != 0) {
    ((Fn2)DAT_007d3ff8[8])(6, 1);
    ((Fn2)DAT_007d3ff8[8])(8, 1);
    FUN_004e6680(DAT_00636564);
    ((Fn2)DAT_007d3ff8[8])(6, 0);
    ((Fn2)DAT_007d3ff8[8])(8, 0);
    FUN_004c19f0(cam);
  }
  FUN_004e4350(DAT_00636560, cam);
  FUN_004e4320(FUN_00426060(), cam);
  if (cam != 0) {
    FUN_004c1a00(cam);
  }
}
