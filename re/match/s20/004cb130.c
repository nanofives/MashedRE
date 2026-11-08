// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cb130 (h1): 100% (regs 100%). Cached per-index triple in the 16-byte records at DAT_007d40c0: when any of
   (param_2, param_3, param_4) differs from record param_1, stores all three and calls the D3D device (DAT_007d4110)
   vtable +0x190 (__stdcall) with (device, param_1, param_2, param_3, param_4). FLAGS /G5 (RW core). */
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
typedef struct Rec16 { int a; int b; int c; int d; } Rec16;
typedef int (__stdcall *DevFn4)(void *self, int i, int a, int b, int c);
MGLOBAL(int **, DAT_007d4110, 0x007d4110);
MGLOBALA(Rec16, DAT_007d40c0, 4, 0x007d40c0);

// MATCH: 0x004cb130 FUN_004cb130
void FUN_004cb130(int param_1, int param_2, int param_3, int param_4)
{
  if (DAT_007d40c0[param_1].a != param_2 || DAT_007d40c0[param_1].b != param_3 || DAT_007d40c0[param_1].c != param_4) {
    DAT_007d40c0[param_1].a = param_2;
    DAT_007d40c0[param_1].b = param_3;
    DAT_007d40c0[param_1].c = param_4;
    ((DevFn4)(*DAT_007d4110)[0x190 / 4])(DAT_007d4110, param_1, param_2, param_3, param_4);
  }
}
