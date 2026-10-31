// FLAGS: /O2 /GS /Ob1
// MATCH: 0x004148b0 FUN_004148b0
/* h4 MATCH-S03. 0x004148b0 (441 B): 100.0%, BYTE-IDENTICAL (--bytes, relocations masked).
   g2s start 63.0%. First hand-written version matched. Shape that matters: per-car record stride 0x74 at
   0x0089a4c4 (+0 count, +4 timer), table DAT_005f2dd8 as int[][5] indexed [n][DAT_0089a374], n =
   (int)DAT_0089a360 (FUN_004a2c48 is _ftol), and a `mode` local (0 / 1 / 2) with the else-branch setting
   mode = 1, then `if (mode == 1) ... else if (mode == 2) ...`: cl threads the jumps into the original's
   0x00414a2b / 0x00414a3e layout and keeps mode in its stack slot. */
typedef struct {
  int count;
  int timer;
  char pad[0x6c];
} CatchUp;
#ifndef MATCH_PORT
extern int DAT_0089a368;
extern float DAT_0089a360;
extern int DAT_0089a364;
extern int DAT_0089a374;
extern float DAT_005d757c;
extern float DAT_005cd0a8;
extern float DAT_005cd0a4;
extern float DAT_005cd0a0;
extern float DAT_005cc35c;
extern int DAT_007f1008;
extern int DAT_005f2dd8[][5];
extern CatchUp DAT_0089a4c4[];
int FUN_0040e470(int p0);
float FUN_00442cc0(int p0);
int FUN_0046d4a0(int *p0, int p1);
#endif
int FUN_004148b0(int unused1, float *unused2, int *out, int car)
{
  int n;
  int i;
  int best;
  int mode;
  int rec;
  float d;

  if (DAT_0089a368 == 2) {
    return 0;
  }
  n = (int)DAT_0089a360;
  if (DAT_0089a364 != -1 && FUN_0040e470(DAT_0089a364) == 1) {
    n += 3;
    if (n > 10) {
      n = 10;
    }
  }
  if (DAT_0089a4c4[car].count < DAT_005f2dd8[n][DAT_0089a374]) {
    i = 0;
    mode = 0;
    best = -1;
    if (FUN_00442cc0(car) == DAT_005d757c) {
      for (; i < 4; i++) {
        if (FUN_0040e470(i) == 1) {
          best = i;
        }
      }
      if (best == -1) {
        return 0;
      }
      d = FUN_00442cc0(best);
      if (d > DAT_005cd0a8) {
        DAT_0089a4c4[car].timer += DAT_007f1008;
        FUN_0046d4a0(&rec, car);
        out[0] = *(int *)(rec + 0x30);
        out[1] = *(int *)(rec + 0x38);
        if (DAT_0089a4c4[car].timer > 10000) {
          DAT_0089a4c4[car].timer = 0;
          DAT_0089a4c4[car].count++;
        }
        return 1;
      }
      if (d > DAT_005cd0a4 && DAT_0089a4c4[car].timer != 0) {
        DAT_0089a4c4[car].timer += DAT_007f1008;
      }
      if (d < DAT_005cd0a0) {
        mode = 1;
      }
      if (d < DAT_005cc35c) {
        mode = 2;
      }
    }
    else {
      mode = 1;
    }
    if (mode == 1) {
      if (DAT_0089a4c4[car].timer > 4500) {
        DAT_0089a4c4[car].timer = 0;
        DAT_0089a4c4[car].count++;
      }
    }
    else if (mode == 2) {
      if (DAT_0089a4c4[car].timer != 0) {
        DAT_0089a4c4[car].timer = 0;
        DAT_0089a4c4[car].count++;
      }
    }
  }
  return 0;
}
