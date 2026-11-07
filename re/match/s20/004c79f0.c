// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004c79f0 (h1): 100% (regs 100%). Cached device state: when DAT_007d4574 != param_1, store it and call
   the D3D device (DAT_007d4110) vtable slot +0x9c (__stdcall COM method) with (device, param_1). FLAGS /G5 (RW core). */
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
typedef int (__stdcall *DevFn1)(void *self, int a);
MGLOBAL(int, DAT_007d4574, 0x007d4574);
MGLOBAL(int **, DAT_007d4110, 0x007d4110);

// MATCH: 0x004c79f0 FUN_004c79f0
void FUN_004c79f0(int param_1)
{
  if (DAT_007d4574 != param_1) {
    DAT_007d4574 = param_1;
    ((DevFn1)(*DAT_007d4110)[0x9c / 4])(DAT_007d4110, param_1);
  }
}
