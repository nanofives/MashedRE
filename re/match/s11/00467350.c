// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00467350 (h1): 97.5% (regs 97.5%, ops 100%), 240/240 ins. Raw 40.8%.
   Shape: 4 cars at 0x008815a0 (stride 0xd04) indexed as &cars[i] with i (the event id i + 0xff00); six
   timer/counter pairs (+0x38 float t[6], +0x20 uint cnt[6]) as one macro per slot (cl jump-threads slot 0 itself);
   param is unsigned (fild + 2^32 fix-up); the 0x007e9de0 event queue as one struct (count + three 0x100 arrays,
   so the index is reloaded per store, as in S02 0x0040b290 / S05 0x004252c0); .rdata floats as literals.
   Residual (b, (a) candidate): the 3-term dot c->dir . n (dir at +0x9d4): the original emits terms y,x,z with
   the car field loaded first (fld [esi+k]; fmul local), ours z,y,x with the local first. All term orders,
   operand orders, partial sums (+=), an __inline dot helper, float[3] for n and double d give identical code
   (cl canonicalises the sum; same residual as S05 0x0041faf0 / 0x004252c0). Permuter 144 compiles: nothing. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
typedef struct V3_00467350 { float x, y, z; } V3_00467350;
typedef struct Car_00467350 {
  char pad0[0x20];
  unsigned int cnt[6];     /* +0x20 */
  float t[6];              /* +0x38 */
  char pad50[0x9b0 - 0x50];
  V3_00467350 vel;         /* +0x9b0 */
  char pad9bc[0x9d4 - 0x9bc];
  V3_00467350 dir;         /* +0x9d4 */
  char pad9e0[0xd04 - 0x9e0];
} Car_00467350;
MGLOBALA(Car_00467350, DAT_008815a0, 4, 0x008815a0);
typedef struct Ev_00467350 {
  unsigned int n;          /* +0x000 */
  int type[0x100];         /* +0x004 */
  int time[0x100];         /* +0x404 */
  int data[0x100];         /* +0x804 */
} Ev_00467350;
MGLOBAL(Ev_00467350, DAT_007e9de0, 0x007e9de0);
MGLOBAL(int, DAT_007f1030, 0x007f1030);
int FUN_0040e350(void);
float FUN_004c39b0(V3_00467350 *out, V3_00467350 *in);

#define TICK_00467350(k) \
  if (c->t[k] > 0.0f) { \
    c->t[k] = c->t[k] - f; \
    if (c->t[k] < 0.0f) c->t[k] = 0.0f; \
  } \
  if (c->t[k] == 0.0f) { \
    if (c->cnt[k] > param_1) c->cnt[k] -= param_1; \
    else c->cnt[k] = 0; \
  }

// MATCH: 0x00467350 FUN_00467350
void FUN_00467350(unsigned int param_1)
{
  float f;
  int i;
  int k;
  float d;
  Car_00467350 *c;
  V3_00467350 n;

  f = (float)param_1;
  for (i = 0; i < 4; i++) {
    c = &DAT_008815a0[i];
    TICK_00467350(0)
    TICK_00467350(1)
    TICK_00467350(2)
    TICK_00467350(3)
    TICK_00467350(4)
    TICK_00467350(5)
    if (c->vel.x != 0.0f || c->vel.y != 0.0f || c->vel.z != 0.0f) {
      FUN_004c39b0(&n, &c->vel);
      d = c->dir.x * n.x + c->dir.y * n.y + c->dir.z * n.z;
      if (d < 0.5f && d > 0.0f) {
        k = FUN_0040e350();
        if (k != 1 && k != 2 && k != 3 && k != 4 && k != 5) {
          DAT_007e9de0.type[DAT_007e9de0.n] = 7;
          DAT_007e9de0.time[DAT_007e9de0.n] = DAT_007f1030;
          DAT_007e9de0.data[DAT_007e9de0.n] = i + 0xff00;
          DAT_007e9de0.n++;
          if (DAT_007e9de0.n >= 0x100) DAT_007e9de0.n = 0;
        }
      }
    }
  }
}
