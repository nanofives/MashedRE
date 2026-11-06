// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047cea0 (h4): best 94.0% (regs 100%, ops 100%). By hand from raw (80%). Residual: the last call's third argument loads p->[0] into edx (original) vs ecx (ours) and the lea operand order is swapped. Tried: operand orders of base+idx*0x40+0x30, temporaries for base/idx, char/int pointer arithmetic, 64-byte-element struct typedefs for the chain, permuter (107 compiles, no gain). Class (b) source shape (struct layout of the chain not found). C++ check ok. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
MGLOBALA(int, DAT_006c71d8, 200, 0x006c71d8);
void FUN_0055ac00(int p0, int p1, int p2);
int FUN_0055ac50(int p0, int p1, int p2);
int FUN_0055ad30(int p0, int p1);
void FUN_0055b650(int *p0, int p1, int p2);
int FUN_0057c210(int p0);
// MATCH: 0x0047cea0 FUN_0047cea0
void FUN_0047cea0(int param_1, int param_2)
{
  int obj;
  int *p;

  if (param_1 >= 0 && param_1 < 200 && DAT_006c71d8[param_1] != 0) {
    obj = FUN_0057c210(DAT_006c71d8[param_1]);
    if (obj != 0 && *(int *)(obj + 0x24) != 0) {
      FUN_0055ac50(*(int *)(obj + 0x24), obj, 1);
      FUN_0055ac00(*(int *)(obj + 0x24), obj, 1);
      FUN_0055ad30(*(int *)(obj + 0x24), obj);
      p = *(int **)(*(int *)(obj + 0x10) + 4);
      if (p != 0) {
        FUN_0055b650(p, param_2, **(int **)(*p + 0x10) + p[1] * 0x40 + 0x30);
      }
    }
  }
}
