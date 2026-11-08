// FLAGS: /O2 /Ob1 /G5 /Oy-
/* MATCH-S22 0x004fb4d0 (h7): 100.0% (regs 100.0%) under /O2 /Ob1 /G5 /Oy- (ebp frame; raw converter 43.9%).
   What moved it: /Oy-; the dot product grouped as z*z + (x*x + y*y) (any flat a+b+c order is reassociated to
   x,z,y and scores 89.5%); the sqrt result (0x004c3b90, declared float) kept in a float local before the multiply
   (98.2% without it: fmul scheduled before mov ecx/add esp). 0x004ecf53 is a d3dx9-psgp thunk, __stdcall 3 args. */
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
typedef struct { float x, y, z; } V3_4fb4d0;
MGLOBAL(char *, DAT_007d9900, 0x007d9900);
MGLOBAL(float *, DAT_007d9904, 0x007d9904);
MGLOBALA(float, DAT_006199f0, 16, 0x006199f0);
float FUN_004c3b90(float p0);
float * FUN_004c4dc0(float * p0, void * p1);
void * __stdcall FUN_004ecf53(V3_4fb4d0 *a, V3_4fb4d0 *b, float *m);
// MATCH: 0x004fb4d0 FUN_004fb4d0
void FUN_004fb4d0(float param_1, float *param_2)
{
  V3_4fb4d0 v;
  float len;
  float scale;

  if (DAT_007d9900 != 0 && (*(int *)(DAT_007d9900 + 0xc) & 3) != 3) {
    if (DAT_007d9904 == 0) {
      FUN_004c4dc0(DAT_006199f0, DAT_007d9900);
      ((int *)DAT_006199f0)[3] = 0;
      ((int *)DAT_006199f0)[7] = 0;
      ((int *)DAT_006199f0)[11] = 0;
      DAT_006199f0[15] = 1.0f;
      DAT_007d9904 = DAT_006199f0;
    }
    v.x = 0.57735026f;
    v.y = 0.57735026f;
    v.z = 0.57735026f;
    FUN_004ecf53(&v, &v, DAT_007d9904);
    len = v.z * v.z + (v.x * v.x + v.y * v.y);
    scale = FUN_004c3b90(len);
    *param_2 = param_1 * scale;
    return;
  }
  *param_2 = param_1;
}
