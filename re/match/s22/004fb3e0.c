// FLAGS: /O2 /Ob1 /G5 /Oy-
/* MATCH-S22 0x004fb3e0 (h7): best 60.3% (regs 68.5%, ops 80.8%) under /O2 /Ob1 /G5 /Oy- (ebp frame; raw converter
   35.6%; the natural (f & 0x20000) != 0x20000 spelling scores 58.9%, the xor spelling below is the best found).
   Same matrix-cache prologue as 0x004fb130 and the same residual: the original keeps the flags in esi and tests
   with and/cmp, 9176 folds to test and drops the esi push, which shifts every later register and the argument
   load order around the three 0x004ecf29 calls (d3dx9-psgp thunk, __stdcall 3 args). Tried: an __inline getter
   returning the cached matrix (16.4%), m kept in a local, assignment inside the call argument, flag sets
   /O2 /Ob1 /Oy-, /GS, /Ox, /O1, /Og /Oi /Ot, /Ob2 (none better). Class (a) for the folded compare [UNCERTAIN],
   (b) for the argument order and registers that follow from it. */
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
typedef struct { float m[16]; } M_4fb3e0;
typedef struct { float x, y, z; } V3_4fb3e0;
MGLOBAL(M_4fb3e0 *, DAT_007d9904, 0x007d9904);
MGLOBAL(char *, DAT_007d9900, 0x007d9900);
MGLOBAL(M_4fb3e0, DAT_005daf10, 0x005daf10);
MGLOBAL(M_4fb3e0, DAT_006199f0, 0x006199f0);
float * FUN_004c4dc0(void * p0, void * p1);
void FUN_004fb210(void * p0, void * p1);
void * __stdcall FUN_004ecf29(V3_4fb3e0 *out, V3_4fb3e0 *in, M_4fb3e0 *m);

// MATCH: 0x004fb3e0 FUN_004fb3e0
void FUN_004fb3e0(V3_4fb3e0 *param_1, V3_4fb3e0 *param_2)
{
  if (DAT_007d9904 == 0) {
    if (DAT_007d9900 != 0 && ((*(unsigned int *)(DAT_007d9900 + 0xc) & 0x20000) ^ 0x20000) != 0) {
      if ((*(unsigned int *)(DAT_007d9900 + 0xc) & 3) == 3) {
        FUN_004fb210(&DAT_006199f0, DAT_007d9900);
        DAT_007d9904 = &DAT_006199f0;
        FUN_004ecf29(param_2, param_1, &DAT_006199f0);
        return;
      }
      FUN_004c4dc0(&DAT_006199f0, DAT_007d9900);
      DAT_006199f0.m[3] = 0.0f;
      DAT_007d9904 = &DAT_006199f0;
      DAT_006199f0.m[7] = 0.0f;
      DAT_006199f0.m[11] = 0.0f;
      DAT_006199f0.m[15] = 1.0f;
      FUN_004ecf29(param_2, param_1, &DAT_006199f0);
      return;
    }
    DAT_007d9904 = &DAT_005daf10;
  } else if (DAT_007d9904 != &DAT_005daf10) {
    FUN_004ecf29(param_2, param_1, DAT_007d9904);
    return;
  }
  *param_2 = *param_1;
}
