// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00459400: 100% by hand (128 B whole symbol BYTE-IDENTICAL). Raw was 20.3%. What moved it:
   DAT_0068bb5c is a file-static float (pilot finding 2: static under the DDK, MGLOBAL under C++), which lets
   the compiler hoist cos() out of the loop; the .rdata floats 0x005cc328/0x005cc320/0x005cc574 are the
   literals 0.01f/1.0f/2.0f; `(float)cos(t) * 0.01f` (float multiply) gives the dword fmul. The extern form of
   DAT_0068bb5c scores 80.9%. */
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
double cos(double);
typedef struct { float u; float v; } Tc459400;
#ifndef __cplusplus
static float DAT_0068bb5c;
#else
MGLOBAL(float, DAT_0068bb5c, 0x0068bb5c);
#endif
int FUN_004e89e0(int geom, int flags);
int FUN_004e8a10(int geom);
// MATCH: 0x00459400 FUN_00459400
int FUN_00459400(int param_1)
{
  int geom;
  int n;
  Tc459400 *tc;
  int i;
  int j;

  DAT_0068bb5c = DAT_0068bb5c + 0.01f;
  geom = *(int *)(param_1 + 0x18);
  n = *(int *)(geom + 0x14);
  FUN_004e89e0(geom, 0x10);
  tc = *(Tc459400 **)(geom + 0x34);
  for (i = 0; i < n; i++) {
    tc[i].u += (float)cos(DAT_0068bb5c) * 0.01f;
    if (tc[i].u > 1.0f) {
      for (j = 0; j < n; j++) {
        tc[j].u = tc[j].u - 2.0f;
      }
    }
  }
  FUN_004e8a10(geom);
  return param_1;
}
