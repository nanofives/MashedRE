// FLAGS: /O2 /GS /Ob1
/* 0x0041f770 FUN_0041f770 (257 B): best 91.5% exact (regs 91.5%, ops 95.8%), 71/71 ins. Raw g2s did not compile.
   The record arrives in EAX and dt on the stack: the caller FUN_00420870 does push [esp+0x3c]; mov eax,esi at
   0x004209f4..0x004209f9. Static-function convention (pilot finding 1); caller_00420870 is a stand-in (not
   matched). Moved it: float literals for the pooled constants (0.05 @0x005cc9a0, 1.0 @0x005cc320, -1.0
   @0x005cc33c, 0.5 @0x005cc32c, 30.0 @0x005cc72c, 15.0 @0x005cc9b0, 0.0625 @0x005cd058; MGLOBAL floats load the
   constant first), FUN_00422470 as float (float x, unsigned seed) (pilot 0x00422470), the -1 test spelled
   !(t >= -1.0f) (test ah,1 / je), the 1.0 test spelled !(1.0f <= t) (jnp).
   Residual (b): the original places the t = 1.0f arm after the ret (jnp forward, jmp back to the * 0.5); ours keeps
   it inline after the -1 arm. Tried 25+ spellings: if/else-if, nested if with the constant in the else, two
   independent ifs (jump threading), nested and split ternaries, early return, a temp vs clamping the
   address-taken v itself, six __inline clamp helpers. Permuter 132 compiles: nothing. Same residual as h2's
   0x00420290 (same file, same 1.0/0.0 clamp shape). */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
MGLOBAL(float, DAT_0063e4a4, 0x0063e4a4);
MGLOBALA(int, DAT_006146f0, 3, 0x006146f0);
MGLOBALA(int, DAT_006146fc, 3, 0x006146fc);

float FUN_00422470(float x, unsigned int seed);
int FUN_0046bda0(float *out, unsigned int a, unsigned int b);
int FUN_004c1520(int a, int *b, float c, int d);
int FUN_00544bf0(int a, float b);

// MATCH: 0x0041f770 FUN_0041f770
static void FUN_0041f770(char *p, float dt)
{
  float v;
  float n;
  float t;
  int h;

  if (*(int *)(p + 0x298) == 0) {
    FUN_0046bda0(&v, *(unsigned int *)(p + 0x290), 0);
    t = v * 0.05f;
    if (!(1.0f <= t)) {
      if (!(t >= -1.0f))
        t = -1.0f;
    } else
      t = 1.0f;
    v = t * 0.5f;
    FUN_00544bf0(*(int *)(p + 0x48), v + 0.5f);
    h = *(int *)(*(int *)(p + 0x50) + 4);
    n = FUN_00422470(DAT_0063e4a4, 0xff) - 0.5f;
    FUN_004c1520(h, DAT_006146fc, n * 30.0f, 0);
    FUN_004c1520(h, DAT_006146f0, n * 15.0f, 1);
    DAT_0063e4a4 = dt * 0.0625f + DAT_0063e4a4;
  }
}

void caller_00420870(char *p, float dt)
{
  FUN_0041f770(p, dt);
}
