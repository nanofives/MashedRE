// FLAGS: /O2 /GS /Ob1
/* a1 0x004645c0 (171 B). Best: 100% exact (59/59).
   Start (g2s) 65.0%. Steps: FUN_005a6dc0 takes 4 args (last a float, 22050.0f = 0x46ac4400 pushed at
   0x00464640), FUN_0045e0f0's 2nd arg is float (0.7f = 0x3f333333, 0.0f), DAT_0068f640 is an array of
   8-byte slots whose first dword is an object pointer (+0x30 int, +0x34 flag byte, +0x3c float) -> 66.7%.
   The key: Ghidra merged a separate local into param_1 because the compiler packed the local `st` into the
   (dead after load into EDI) parameter home [esp+0x10]. With `st` and `count` as two distinct locals and the
   parameter kept as `p`, the frame (push ecx = one slot + param home) and schedule match exactly.
   Residual: none. Class of the starting gap: (b) Ghidra-merged temporary (param slot reuse). */
typedef struct { unsigned char pad[0x30]; int h; unsigned char flags; unsigned char p2[7]; float f3c; } Obj;
typedef struct { Obj *obj; int pad; } Slot;
extern int DAT_00604a10;
extern Slot DAT_0068f640[];
void FUN_0045daf0(int a, int b);
void FUN_0045e0f0(int a, float b);
int FUN_0046cbb0(int a, int *b, int *c);
void FUN_005a66d0(Obj *a, int b);
void FUN_005a6dc0(int a, int b, int c, float d);

// MATCH: 0x004645c0 _FUN_004645c0
void FUN_004645c0(int p)
{
  int st;
  int count;
  int idx = DAT_00604a10 + p;
  FUN_0046cbb0(p, &st, &count);
  if (st == 2 && count > 0) {
    if ((DAT_0068f640[idx].obj->flags & 2) == 0) {
      FUN_0045daf0(idx, p);
      FUN_005a66d0(DAT_0068f640[idx].obj, 1);
      DAT_0068f640[idx].obj->f3c = 22050.0f;
      if (DAT_0068f640[idx].obj->h != 0)
        FUN_005a6dc0(DAT_0068f640[idx].obj->h, 4, 1, 22050.0f);
      FUN_0045e0f0(idx, 0.7f);
    }
  } else {
    FUN_0045e0f0(idx, 0.0f);
  }
}
