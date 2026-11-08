// FLAGS: /O2 /Ob1 /G5
// h2 hand-matched. 76.5% (regs 86.3%, ops 94.1%). Residual: original keeps the self-assignment rc = rc (mov dx,[+0xe] before the or, store after), the optimizer drops it here (volatile temp spills, 75.5%); eax/ecx/edx choice around the two [g+0x10c] calls. Class (b).
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

#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_006186d0, 0x006186d0);
typedef struct Obj { char pad0[0xc]; ushort flags; short rc; char pad1[0x10]; char link[0x34]; int *p54; int *p58; int *p5c; } Obj;
int FUN_004d8060(undefined4 *p0, Obj *p1);
undefined4 FUN_004d8bd0(int *p0);
undefined4 FUN_004f0c10(int *p0);
int *FUN_004f3b60(char *p0);
// MATCH: 0x004e8ea0 FUN_004e8ea0
undefined4 FUN_004e8ea0(Obj *o)
{
  if (o->rc - 1 <= 0) {
    if (o->p58 != 0) {
      FUN_004d8bd0(o->p58);
    }
    o->flags = o->flags | 0xfff;
    o->rc = o->rc;
    if (o->p54 != 0) {
      FUN_004f0c10(o->p54);
      o->p54 = 0;
    }
    FUN_004d8060(&DAT_006186d0, o);
    if (o->p5c != 0) {
      (**(code **)(DAT_007d3ff8 + 0x10c))(o->p5c);
      o->p5c = 0;
    }
    FUN_004f3b60(&o->link[0x0]);
    o->rc = o->rc - 1;
    (**(code **)(DAT_007d3ff8 + 0x10c))(o);
    return 1;
  }
  o->rc = o->rc - 1;
  return 1;
}
