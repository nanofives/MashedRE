// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00417180 FUN_00417180
// MATCH: 0x00414030 FUN_00414030
/* h4 MATCH-S03. 0x00417180 (486 B): 76.8% (regs 76.8, ops 95.7), 133/138 ins.
   0x00414030 (48 B), the callee, defined here so cl knows it only touches EAX: 100.0% on its own.
   g2s start 26.8% (extraout_ECX / extraout_EDX). Steps:
   - FUN_00414030 defined in this TU before the caller (plain external linkage, /Ob1): ECX (= state) and
     EDX (= 4) live across `call 0x414030`, as in the original. Not /GL.
   - DAT_0089a368 is a field of the same block as the per-car records (block at 0x0089a360, cars at +0x164,
     stride 0x74): with separate symbols cl caches DAT_0089a368 across the stores to car[].state; one block
     makes it re-read DAT_0089a368 at every compare like the original (62.3 -> 63.0).
   - state tests read car[car].state each time (not a local): cl then reuses ECX from the first load for
     `== 2` and `== 1` and re-reads for `== 0` / `== 3`, like the original (-> 76.8).
   - Table at 0x00801ca0: records of 0x204 B, [group][sub] with groups 0 / 1 / 3 (0x8022ac, 0x802ec4).
   Residual (5 instructions + register names): the original holds the constant 1 in EBP (push/pop ebp,
   mov ebp,1, then `mov [..],ebp` / `cmp ecx,ebp` / `cmp [0x89a368],ebp`); ours uses immediate 1. Tried:
   a `one` local (propagated away), `sub += 1` / `sub = sub + 1` / separate ++, `>= 3` (moves 1 into a
   register but then 2 loses EDI, 73.9%), `<= 3`, int first arg to FUN_00472650, unsigned car, a CatchUp *g
   pointer (56.5%), a switch on mode (72.9%); permuter 123 compiles, no gain. Class (b) unknown:
   constant-register promotion is use-count sensitive (the `>= 3` arm moves it), so a source form with
   one more use of 1, or one fewer of 2, is not ruled out. Port note: FUN_00414030 here is a real match,
   do not link it twice next to another definition. */
typedef struct {
  int count;
  int timer;
  int state;
  int sub;
  char pad0[0x2c];
  int flag;
  int hold;
  char pad1[0x30];
} CatchUp;
typedef struct {
  int n;
  char pad[0x200];
} SlotRec;
typedef struct {
  float f360;
  int i364;
  int mode;
  char pad0[0x0c];
  char pad1[0x14c];
  CatchUp car[4];
} AiCatchUpBlock;
#ifndef MATCH_PORT
extern AiCatchUpBlock DAT_0089a360;
extern SlotRec DAT_00801ca0[4][3];
extern int DAT_008032d4[4][5];
extern int DAT_007f1008;
extern int DAT_007f0ff8;
extern int DAT_0063bd90;
extern float DAT_005cc32c;
float FUN_00472650(float lo, float hi);
#endif
/* stand-in for 0x00414030 (48 B, not scored): defined here so cl knows it leaves ECX/EDX alone */
void FUN_00414030(int i)
{
  int k;

  if (i == -1) {
    for (k = 0; k < 4; k++) {
      DAT_008032d4[k][0] = 1000;
    }
    return;
  }
  DAT_008032d4[i][0] = 1000;
}

void FUN_00417180(int car)
{
  int t;

  if (DAT_0089a360.car[car].flag != 0) {
    DAT_0089a360.car[car].hold = 1;
    if (DAT_0089a360.car[car].state == 2) {
      if (++DAT_0089a360.car[car].sub > 2) {
        DAT_0089a360.car[car].sub = 0;
      }
      FUN_00414030(-1);
    }
    if (DAT_0089a360.car[car].state == 1) {
      if (++DAT_0089a360.car[car].sub > 2 || DAT_00801ca0[1][DAT_0089a360.car[car].sub].n < 4) {
        DAT_0089a360.car[car].state = 2;
        DAT_0089a360.car[car].sub = 0;
      }
    }
    if (DAT_0089a360.car[car].state == 0) {
      if (++DAT_0089a360.car[car].sub > 2 || DAT_00801ca0[0][DAT_0089a360.car[car].sub].n < 4) {
        DAT_0089a360.car[car].state = 1;
        DAT_0089a360.car[car].sub = 0;
      }
      FUN_00414030(-1);
    }
    if (DAT_0089a360.car[car].state == 3) {
      if (++DAT_0089a360.car[car].sub > 2 || DAT_00801ca0[3][DAT_0089a360.car[car].sub].n < 4) {
        DAT_0089a360.car[car].state = 0;
        DAT_0089a360.car[car].sub = 0;
      }
      FUN_00414030(-1);
    }
    DAT_0089a360.car[car].flag = 0;
  }
  if (DAT_0089a360.car[car].hold != 0 && DAT_0089a360.car[car].hold < 9000) {
    DAT_0089a360.car[car].hold += DAT_007f1008;
    return;
  }
  DAT_0089a360.car[car].hold = 0;
  if (DAT_0089a360.mode == 0) {
    DAT_0089a360.car[car].state = 0;
  }
  if (DAT_0089a360.mode == 2) {
    DAT_0089a360.car[car].state = 0;
  }
  if (DAT_0089a360.mode == 1) {
    if (DAT_0089a360.car[car].state != 2) {
      FUN_00414030(-1);
    }
    DAT_0089a360.car[car].state = 2;
  }
  t = DAT_007f0ff8 / 3000;
  if ((t & 0xf) == 0xf) {
    if (DAT_0063bd90 != t) {
      if (FUN_00472650(0.0f, 1.0f) < DAT_005cc32c) {
        DAT_0089a360.car[car].sub++;
        FUN_00414030(-1);
      }
    }
    DAT_0063bd90 = t;
    return;
  }
  DAT_0063bd90 = 0;
}
