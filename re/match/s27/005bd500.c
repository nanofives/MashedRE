// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005bd500: 100.0% (regs 100.0%). Hand: recursive COM enumeration; VT(o,off) macro for stdcall vtable calls, Info{iface,kind} out struct, callback param_2 is a cdecl (param_1,param_3) function pointer, loop with break to the common Release+callback tail. */
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
typedef unsigned int uint;
typedef int (__stdcall *vf1)(void *);
typedef int (__stdcall *vf2)(void *, void *);
typedef int (__stdcall *vf4)(void *, int, void *, int);
typedef int (*cb2)(int *, int);
typedef struct Info { int *iface; int kind; char rest[0x100]; } Info;
#define VT(o, off) (*(int *)(*(int *)(o) + (off)))
// MATCH: 0x005bd500 FUN_005bd500
uint FUN_005bd500(int *param_1, cb2 param_2, int param_3)
{
  int *a;
  int *b;
  int *c;
  Info x;
  uint ret;
  int r;

  ret = 1;
  r = ((vf2)VT(param_1, 0x28))(param_1, &a);
  if (r < 0) {
    return 0;
  }
  r = ((vf4)VT(a, 0xc))(a, 1, &b, 0);
  while (r == 0) {
    r = ((vf2)VT(b, 0x18))(b, &c);
    if (r >= 0) {
      r = ((vf2)VT(c, 0x20))(c, &x);
      if (r >= 0) {
        if (x.kind == 1 && FUN_005bd500(x.iface, param_2, param_3) == 0) {
          ((vf1)VT(c, 8))(c);
          ((vf1)VT(b, 8))(b);
          ret = 0;
          break;
        }
        ((vf1)VT(x.iface, 8))(x.iface);
      }
      ((vf1)VT(c, 8))(c);
    }
    ((vf1)VT(b, 8))(b);
    r = ((vf4)VT(a, 0xc))(a, 1, &b, 0);
  }
  ((vf1)VT(a, 8))(a);
  return param_2(param_1, param_3) & ret;
}
