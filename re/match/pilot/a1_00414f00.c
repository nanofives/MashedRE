// FLAGS: /O2 /GS /Ob1
/* a1 0x00414f00 (274 B). Best: 95.8% exact (regs 95.8%, ops 96.8%), 95/95 instructions.
   Start (g2s) 50.5%. Steps:
   - _DAT_005cc32c/72c/348/564 are floats (0.5, 30.0, 1.5, 0.25), not ints (converter emitted fild) -> 83.2%.
   - Two SEPARATE `return 0;` statements for the two range checks (not `&&`/`||`, not nested ifs):
     only that form makes the compiler emit the early-out `xor eax,eax / add esp,0x18 / ret` BEFORE
     `push esi / push edi` (0x00414f27..0x00414f2c) and use `test ah,5 / jnp` + `test ah,0x41 / jne` -> 95.8%.
   Residual (4 instructions): the x and z components of the vector subtract. Original:
     0x00414f6d fld [eax+0x30] ; fsubr [esi]      (x)   and   fld [eax+0x38] ; lea eax,[esp+0x2c] ; fsubr [esi+8] (z)
   ours: fld [esi] ; fsub [eax+0x30]  and  fld [esi+8] ; fsub [eax+0x38] ; lea eax.  y is identical in both
   (fld [esi+4] ; fsub [eax+0x34]). Tried, none moves it: statement orders (xzy, zxy, yzx), negated
   spellings (-(b-a), -b+a, a+-b), float-ptr, V3-ptr, struct-member and array typing of pos, rec and v, a temp pointer
   to rec+0x30, scalar temps for pos, a same-TU helper Sub() (static and extern, auto-inlined by /Ob2),
   V3-returning helper, double accumulator, const pointer prototype; permuter 197 compiles, no gain.
   Class: (a) candidate: x87 operand selection (fld second operand + fsubr) on the components whose
   memory base register (eax = rec reloaded from [esp+0x20]) is needed first or freed next; no source form
   tried produces fsubr here.
   CLOSED 2026-11-10 (MATCH-X87-CASTLEVER-PILOT): comma rvalue `(0, *(float *)(rec + 0x30))` on x and z makes cl
   load the rec field first (fld [eax+0x30] ; fsubr [esi]) -> 100%, byte-identical. Value unchanged (comma yields the field).
   Callees: all cdecl (plain ret). FUN_00452eb0 returns float on st(0); FUN_004c3ac0(V3 ptr) returns float. */
typedef struct { float x, y, z; } V3;
extern float DAT_005cc32c, DAT_005cc72c, DAT_005cc348, DAT_005cc564;
float *FUN_00452160(void);
int FUN_00452ea0(int p0);
float FUN_00452eb0(void);
int FUN_0046c7b0(int p0);
int FUN_0046d4a0(int *p0, int p1);
int FUN_0046d570(float *p0, int p1);
int FUN_0046d6d0(int *p0, int p1);
float FUN_004c3ac0(V3 *p0);
// MATCH: 0x00414f00 _FUN_00414f00
int FUN_00414f00(int param_1, int param_2, float *out, int car)
{
  float *pos;
  float d;
  int rec;
  int a;
  float b;
  V3 v;

  if (FUN_00452eb0() < DAT_005cc32c) return 0; if (FUN_00452eb0() > DAT_005cc72c) return 0; {
    if (FUN_00452ea0(car) != 0) {
      pos = FUN_00452160();
      FUN_0046d6d0(&a, car);
      FUN_0046d570(&b, car);
      FUN_0046d4a0(&rec, car);
      v.x = pos[0] - (0, *(float *)(rec + 0x30));
      v.y = pos[1] - *(float *)(rec + 0x34);
      v.z = pos[2] - (0, *(float *)(rec + 0x38));
      d = FUN_004c3ac0(&v);
      if (d > DAT_005cc348) {
        out[0] = pos[0];
        out[1] = pos[2];
        return 1;
      }
      if (d > DAT_005cc564) {
        if (FUN_0046c7b0(0) != 0) {
          FUN_0046d4a0(&rec, 0);
          out[0] = *(float *)(rec + 0x30);
          out[1] = *(float *)(rec + 0x38);
          return 1;
        }
      }
    }
    return 0;
  }
  return 0;
}
