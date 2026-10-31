// FLAGS: /O2 /GS /Ob1
// MATCH: 0x00417640 FUN_00417640
/* 0x00417640 (237 B). Best 94.6% (regs 94.6, ops 94.6), 74/74 ins. Converter was 39.2%.
   Fixed: `!(x > c)` and `a > c` spelling for the two fcomp tests (test ah,0x41 / je and jne), int out-param
   first in the frame, then a, b, d[3].
   Residual, two x87 load-scheduling spots, same operations:
   1. d[0]: original `fld [eax+0x30]; lea ecx; fsubr [esi]`, ours `fld [esi]; mov eax; fsub [eax+0x30]`.
   2. after FUN_004c3ac0: original hoists `fld a` above `add esp,0x1c`, ours loads it after.
   Tried (no move): -o+p, *pos, (float) cast, __inline VSub(out,a,b), float *o = obj+0x30 temp, pos[0]
   pre-read, local order swaps (obj/pos, a/b), (void) / if() on the 4c3ac0 return, !(a <= c), the
   permuter (0 variants generated). d[0] -= o form made it worse (81.6%).
   Class: (a) compiler-version x87 scheduling (both spots: the original schedules an fld earlier, above an
   integer op, than 13.00.9176/9178 does). */
typedef struct Rec74 { int f0; char pad[0x70]; } Rec74;
#ifndef MATCH_PORT
extern Rec74 DAT_0089a52c[];
extern float DAT_005cc72c;
extern float DAT_005cc568;
extern float DAT_005ccad0;
int    FUN_00426c00(void);
float *FUN_00452160(void);
int    FUN_00452ea0(int p0);
float  FUN_00452eb0(void);
void   FUN_0046d4a0(int *p0, int p1);
void   FUN_0046d570(float *p0, int p1);
void   FUN_0046d6d0(float *p0, int p1);
float  FUN_004c3ac0(float *p0);
#endif

void FUN_00417640(int idx, unsigned char *out)
{
  float *pos;
  int obj;
  float a;
  float b;
  float d[3];

  if (FUN_00426c00() == 0x21 && !(FUN_00452eb0() > DAT_005cc72c) && DAT_0089a52c[idx].f0 != 10 &&
      FUN_00452ea0(idx) != 0) {
    pos = FUN_00452160();
    FUN_0046d6d0(&a, idx);
    FUN_0046d570(&b, idx);
    FUN_0046d4a0(&obj, idx);
    d[0] = pos[0] - *(float *)(obj + 0x30);
    d[1] = pos[1] - *(float *)(obj + 0x34);
    d[2] = pos[2] - *(float *)(obj + 0x38);
    FUN_004c3ac0(d);
    if (a > DAT_005cc568 && b < DAT_005ccad0) {
      out[4] = 0;
      out[5] = 0xff;
      return;
    }
    out[4] = 0;
    out[5] = 0;
  }
}
