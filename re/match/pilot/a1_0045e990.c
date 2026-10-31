// FLAGS: /O2 /GS /Ob1
/* a1 0x0045e990 (1610 B). Best: 99.3% exact (regs 99.3%, ops 99.3%), 444/444 instructions.
   Start (g2s) 44.6%. Steps: real types (floats 22050.0 @0x005ce6b8, 20.0 @0x005ccd6c, 70.0 @0x005ccd0c;
   FUN_00472650(float lo, float hi) is cdecl returning st(0), not __fastcall; FUN_005a6dc0 has 4 args, the
   last a float), Obj with V3 at +0x58 assigned as a struct (the 3-dword copy through the local V3),
   DAT_0088e670 as a pointer to 0x90-byte records with the sound id at +0x80 (indices k+7, k+11, 14..18:
   the 0x470/0x6b0/0x860.. displacements), `i++` for the second channel of record 16 (`inc esi` at
   0x0045ede0), and a temp `x` for the first random x in block 1 (fstp to the extra stack slot at
   0x0045ea7e). The repeated start/position blocks are written as macros (a same-TU static helper taking
   V3* inlines to identical code; a helper taking 3 floats is not inlined by /Ob2 and drops to 45%).
   Residual (2 spots, 3 instructions each, scheduling only, same instructions):
   - 0x0045ea87: original `call 0x472650 ; mov ecx,[esi*8+0x68f640] ; fsub [0x5ccd0c]`, ours puts the
     slot load after the fsub.
   - 0x0045eb73..0x0045eb8d: original issues `mov [v.y],2.0 ; mov [v.z],-70.0 ; add eax,0x58` between
     `fsub [0x5ccd6c]` and its `fstp [v.x]`; ours does fstp right after the v.y store.
   Both are integer instructions moved into the fsub->fstp x87 latency window. Tried: all orders of the
   v.x/v.y/v.z stores in both blocks, x/z temps, `v.x = r; v.x -= c`, `-c + r`, struct-copy through a V3*
   temp and through a cast, same-TU static helper (V3*), 3-float helper; permuter 212 compiles, no gain.
   The six constant-position blocks (no x87 op) match exactly.
   Class: (a) candidate: x87-latency instruction scheduling (integer ops hoisted into the FP shadow). */
typedef struct { float x, y, z; } V3;
typedef struct { unsigned char pad[0x30]; int h; unsigned char flags; unsigned char p2[7]; float freq;
                 unsigned char p3[0x18]; V3 pos; } Obj;
typedef struct { Obj *obj; int pad; } Slot;
typedef struct { unsigned char pad[0x80]; int snd; unsigned char p2[0xc]; } SndRec;
extern int DAT_0068f4c8;
extern int DAT_008aa254;
extern SndRec *DAT_0088e670;
extern float DAT_005ce6b8, DAT_005ccd6c, DAT_005ccd0c;
extern Slot DAT_0068f640[];
void FUN_0045e0f0(int a, float b);
float FUN_00472650(float lo, float hi);
int FUN_004b44f0(int a, int b);
void FUN_005a66d0(Obj *a, int b);
int FUN_005a6d60(int h, int a, int b, V3 *v);
void FUN_005a6dc0(int h, int a, int b, float f);

// MATCH: 0x0045e990 _FUN_0045e990
void FUN_0045e990(void)
{
  int i;
  float x;
  V3 v;

  if (DAT_0068f4c8 != 0 && DAT_008aa254 > 1)
    return;

  if (FUN_004b44f0(0, 0x32) == 0x19) {
    i = DAT_0088e670[FUN_004b44f0(0, 3) + 7].snd;
    if ((DAT_0068f640[i].obj->flags & 2) == 0)
      FUN_005a66d0(DAT_0068f640[i].obj, 1);
    DAT_0068f640[i].obj->freq = FUN_00472650(0.0f, 10000.0f) + DAT_005ce6b8;
    if (DAT_0068f640[i].obj->h != 0)
      FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, FUN_00472650(0.0f, 10000.0f) + DAT_005ce6b8);
    FUN_0045e0f0(i, 1.0f);
    x = FUN_00472650(0.0f, 40.0f) - DAT_005ccd6c;
    v.z = FUN_00472650(0.0f, 140.0f) - DAT_005ccd0c;
    v.x = x;
    v.y = -1.0f;
    DAT_0068f640[i].obj->pos = v;
    if (DAT_0068f640[i].obj->h != 0)
      FUN_005a6d60(DAT_0068f640[i].obj->h, 0, 1, &v);
  }

  if (FUN_004b44f0(0, 0x19) == 0x19) {
    i = DAT_0088e670[FUN_004b44f0(0, 2) + 11].snd;
    if ((DAT_0068f640[i].obj->flags & 2) == 0)
      FUN_005a66d0(DAT_0068f640[i].obj, 1);
    DAT_0068f640[i].obj->freq = 22050.0f;
    if (DAT_0068f640[i].obj->h != 0)
      FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, 22050.0f);
    FUN_0045e0f0(i, 1.0f);
    v.x = FUN_00472650(0.0f, 40.0f) - DAT_005ccd6c;
    v.y = 2.0f;
    v.z = -70.0f;
    DAT_0068f640[i].obj->pos = v;
    if (DAT_0068f640[i].obj->h != 0)
      FUN_005a6d60(DAT_0068f640[i].obj->h, 0, 1, &v);
  }

#define START(I, F) \
    if ((DAT_0068f640[I].obj->flags & 2) == 0) \
      FUN_005a66d0(DAT_0068f640[I].obj, 1); \
    DAT_0068f640[I].obj->freq = F; \
    if (DAT_0068f640[I].obj->h != 0) \
      FUN_005a6dc0(DAT_0068f640[I].obj->h, 4, 1, F); \
    FUN_0045e0f0(I, 1.0f);
#define POS(I, X, Y, Z) \
    v.x = X; v.y = Y; v.z = Z; \
    DAT_0068f640[I].obj->pos = v; \
    if (DAT_0068f640[I].obj->h != 0) \
      FUN_005a6d60(DAT_0068f640[I].obj->h, 0, 1, &v);

  i = DAT_0088e670[14].snd;
  START(i, 12050.0f)
  POS(i, -13.5f, 0.6f, -3.22f)

  i = DAT_0088e670[15].snd;
  START(i, 22050.0f)
  POS(i, 8.9f, 1.3f, 69.6f)

  i = DAT_0088e670[16].snd;
  START(i, 22050.0f)
  POS(i, 0.0f, 2.0f, -74.0f)
  i++;
  START(i, 22050.0f)
  POS(i, 8.9f, 1.3f, 69.6f)

  i = DAT_0088e670[17].snd;
  START(i, 22050.0f)
  POS(i, -3.86f, 6.716f, 24.4f)

  i = DAT_0088e670[18].snd;
  START(i, 22050.0f)
  POS(i, 4.7f, 12.6f, -71.5f)
}
