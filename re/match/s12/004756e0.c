// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x004756e0: 100% (regs 100%), relocheck OK. By hand (raw 29.0%): the 64-byte local is float[16] (a char array
   draws a /GS cookie); param_2's float at +0xc typed as a one-field struct; param_5 != 0 branch first. */
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
typedef struct { unsigned char pad[0xc]; float duration; } AnimInfo;
void FUN_004752f0(void *out, AnimInfo *anim, float t);
void FUN_004b47e0(void *out, AnimInfo *anim, float t);
int FUN_004c1480(void *p0, void *p1, int p2);
// MATCH: 0x004756e0 FUN_004756e0
void FUN_004756e0(void **param_1, AnimInfo *param_2, float *param_3, float param_4, int param_5)
{
  void *obj;
  float t;
  float buf[16];

  if (param_1 != 0 && param_2 != 0) {
    obj = param_1[1];
    t = param_4 + *param_3;
    if (t > param_2->duration) {
      t = t - param_2->duration;
    }
    if (param_5 != 0) {
      FUN_004b47e0(buf, param_2, t);
    }
    else {
      FUN_004752f0(buf, param_2, t);
    }
    *param_3 = t;
    FUN_004c1480(obj, buf, 0);
  }
}
