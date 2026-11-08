// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005bf0c0 : 26.5% (regs 41, ops 62). By hand from the asm: cdecl 7 args, loop over param_1[0x47][i] (count at param_1[0x48]), vtbl[9](obj,&slot) with the out slot being the param_1 arg slot itself, then (arr[i]-4)->+0x118 object vtbl[0x11](p, a2..a7), first negative hr kept. The original reloads the array/count from [edi+0x11c]/[edi+0x120] every iteration and keeps edi=param_1; ours hoists them (invariant motion past the escaped &param_1) and reuses edi differently. A volatile pointer did not change the output. Residual class (b), source shape not found. */

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

typedef int (__stdcall *GF)(int *, int *);
typedef int (__stdcall *XF)(int *, int, int, int, int, int, int);
// MATCH: 0x005bf0c0 FUN_005bf0c0
int FUN_005bf0c0(int *param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7)
{
  unsigned int i;
  int iVar1;
  int res;
  volatile int *self;
  int *obj;
  int *p;

  self = (volatile int *)param_1;
  res = 0;
  i = 0;
  if (self[0x48] != 0) {
    do {
      obj = *(int **)(self[0x47] + i * 4);
      iVar1 = ((GF)(*(int **)obj)[9])(obj, (int *)&param_1);
      if ((iVar1 >= 0) && ((int)param_1 == 1)) {
        p = *(int **)(*(int *)(self[0x47] + i * 4) - 4 + 0x118);
        if (p != (int *)0) {
          iVar1 = ((XF)(*(int **)p)[0x11])(p, param_2, param_3, param_4, param_5, param_6, param_7);
          if (iVar1 < 0) {
            res = iVar1;
          }
        }
      }
      i++;
    } while (i < (unsigned int)self[0x48]);
  }
  return res;
}

