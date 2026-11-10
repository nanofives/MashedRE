// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00442a60: 99.4% (regs 99.4%) by hand (h7). Raw 26.4%. The 4-float clear is the memset intrinsic
   (xor eax,eax + 4 stores); the loop counter reuses `a` (its address is taken, so it lives on the stack);
   obj x/z read into float locals before the subtraction, v.y = 0 stored before v.x.
   Residual (a): one scheduling difference, the original loads the loop counter `a` (mov eax,[esp+0x10]) before
   `fmul 0.8` after the FUN_004c3ac0 call, ours after it. Tried: 0.8f * f(), a float temp, a second store
   statement, a named 0.8 global, double return type, V3* prototype, (float)(x * 0.8); permuter 257 compiles. 
   MATCH-RACEPATH-X87A 2026-11-09: CLOSED, 531 B BYTE-IDENTICAL. The call result is a comma rvalue,
   `DAT_008989b0[a] = (0, FUN_004c3ac0(&v.x)) * 0.8f;` (lever from MATCH-RACEPATH-BIG 0x00446520). That loads the
   counter `a` before the fmul as at 0x00442c46. Same value and type: the comma expression is the float return. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
typedef struct {
  float x;
  float y;
  float z;
} V3_00442a60;
typedef struct {
  char pad[0x30];
  float x;
  float y;
  float z;
} Obj_00442a60;
MGLOBALA(float, DAT_008989b0, 4, 0x008989b0);
MGLOBAL(int, DAT_008989a8, 0x008989a8);
MGLOBAL(int, DAT_008989c8, 0x008989c8);
#ifdef __cplusplus
extern "C"
#endif
void *memset(void *p0, int p1, unsigned int p2);
#pragma intrinsic(memset)
void FUN_0040e180(int *p0, int *p1);
float FUN_00408ad0(int p0);
void FUN_0046cbb0(int p0, int *p1, int *p2);
int FUN_0040e370(int p0);
int FUN_0046c7b0(int p0);
void FUN_0046d4a0(Obj_00442a60 **p0, int p1);
float FUN_004c3ac0(float *p0);
// MATCH: 0x00442a60 FUN_00442a60
void FUN_00442a60(void)
{
  int a;
  int b;
  float fa;
  float fb;
  int ra;
  int rb;
  int xa;
  int xb;
  int first;
  int second;
  Obj_00442a60 *obj;
  V3_00442a60 pos;
  V3_00442a60 v;
  float ox;
  float oz;

  memset(DAT_008989b0, 0, sizeof(DAT_008989b0));
  a = 4;
  FUN_0040e180(&a, &b);
  fa = FUN_00408ad0(a);
  fb = FUN_00408ad0(b);
  if (fa > 80.0f && fb < 20.0f) {
    fa -= 100.0f;
  } else if (fb > 80.0f && fa < 20.0f) {
    fb -= 100.0f;
  }
  FUN_0046cbb0(a, &ra, &xa);
  FUN_0046cbb0(b, &rb, &xb);
  if (fa > fb) {
    if (ra == 0) {
      first = a;
      second = b;
    } else if (rb == 0) {
      first = b;
      second = a;
    } else {
      return;
    }
  } else {
    if (rb == 0) {
      first = b;
      second = a;
    } else if (ra == 0) {
      first = a;
      second = b;
    } else {
      return;
    }
  }
  DAT_008989a8 = first;
  DAT_008989c8 = second;
  if (FUN_0040e370(first) && FUN_0046c7b0(first) == 1) {
    FUN_0046d4a0(&obj, first);
    pos.x = obj->x;
    pos.z = obj->z;
    for (a = 0; a < 4; a++) {
      if (FUN_0040e370(a) && FUN_0046c7b0(a) == 1) {
        FUN_0046d4a0(&obj, a);
        ox = obj->x;
        oz = obj->z;
        v.y = 0.0f;
        v.x = pos.x - ox;
        v.z = pos.z - oz;
        DAT_008989b0[a] = (0, FUN_004c3ac0(&v.x)) * 0.8f;
      }
    }
  }
}
