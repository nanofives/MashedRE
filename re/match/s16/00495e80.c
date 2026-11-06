// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00495e80 (h2): 100% (regs 100%). Hand-written: 0x448-byte JoyDev struct array at 0x00771e88 (4 entries), pointer walk, COM calls through __stdcall fn-ptr, intrinsic memset, variadic logger. */
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
void *memset(void *dst, int c, unsigned int n);
#pragma intrinsic(memset)
typedef void (__stdcall *ComFn)(int *self);
typedef struct JoyDev { int *dev; undefined4 rest[0x111]; } JoyDev;
MGLOBAL(int, DAT_00772fac, 0x00772fac);
MGLOBALA(JoyDev, DAT_00771e88, 4, 0x00771e88);
void FUN_004987b0(char *fmt, ...);
// MATCH: 0x00495e80 FUN_00495e80
void FUN_00495e80(void)
{
  int i;
  JoyDev *d;
  int *p;

  i = 0;
  if (0 < DAT_00772fac) {
    d = DAT_00771e88;
    do {
      p = d->dev;
      (*(ComFn *)(*p + 0x20))(p);
      FUN_004987b0("Releasing device\n");
      (*(ComFn *)(*p + 8))(p);
      memset(d, 0, sizeof(JoyDev));
      FUN_004987b0("Destroyed joypad %d\n", i);
      i++;
      d++;
    } while (i < DAT_00772fac);
  }
}
