// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00458e00: 99.0% (regs 99.0%) by hand (raw 37.9%). Count n read once (used for the loop and the
   new slot), squared distance as three statements (t = y*y; t += x*x; t += z*z), the mode/type check as two
   if/else returns followed by a separate `if (mode == 2)` block (that puts the call block last, 87.1% ->
   99.0%), `return DAT_0068b9a8++` written as increment then `return DAT_0068b9a8 - 1`. 0.001f is the literal at
   0x005cc558; 120.0f = 0x42f00000.
   Residual (b): one x87 load (fld home.y) scheduled after `dec esi` in ours, before it in the original.
   Tried 6 loop forms (i-- placement, record pointer walk, one-expression sum) and the permuter (236 compiles). */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct V3_00458e00 {
  float x;
  float y;
  float z;
} V3_00458e00;
typedef struct Rec00458e00 {
  int clump;
  undefined4 f04[5];
  float f18;
  int f1c;
  int state;
  int type;
  int flag;
  V3_00458e00 home;
  V3_00458e00 pos;
  V3_00458e00 vel;
} Rec00458e00;
MGLOBAL(int, DAT_0068b9a8, 0x0068b9a8);
MGLOBALA(Rec00458e00, DAT_0068b198, 25, 0x0068b198);
int FUN_0042fe30(void);
undefined4 FUN_00458d00(void);
void FUN_00458dd0(Rec00458e00 *p0);
void FUN_004c15c0(int p0);
// MATCH: 0x00458e00 FUN_00458e00
int FUN_00458e00(V3_00458e00 *p, int type)
{
  int mode;
  int i;
  int n;
  Rec00458e00 *r;
  float dx;
  float dy;
  float dz;
  float t;

  mode = FUN_0042fe30();
  n = DAT_0068b9a8;
  if (n >= 25) return -1;
  i = n;
  while (i != 0) {
    i--;
    r = &DAT_0068b198[i];
    { float hx = r->home.x; dx = hx - p->x; }  /* named float local: fld [ecx] first (MATCH-X87-CASTLEVER-PILOT) */
    dy = r->home.y - p->y;
    dz = r->home.z - p->z;
    t = dy * dy;
    t += dx * dx;
    t += dz * dz;
    if (t < 0.001f) return -1;
  }
  r = &DAT_0068b198[n];
  r->flag = 0;
  r->type = type;
  if (mode == 2) {
    if (type != 0x15) return -1;
  } else {
    if (type == 0x15) return -1;
  }
  if (mode == 2) {
    r->type = FUN_00458d00();
    r->flag = 1;
  }
  FUN_00458dd0(r);
  FUN_004c15c0(*(int *)(r->clump + 4));
  r->home = *p;
  r->pos = *p;
  r->state = 1;
  r->f18 = 120.0f;
  r->f1c = 0;
  DAT_0068b9a8++;
  return DAT_0068b9a8 - 1;
}
