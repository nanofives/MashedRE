// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00496040 (h2): 100% (regs 100%). Hand-written: IDirectInput8::EnumDevices (vtable +0x10, __stdcall 5 args) with FUN_00495520 called twice (once for the vtable, once for this), FUN_00495ee0 as the __stdcall enum callback. */
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
typedef int (__stdcall *EnumCb)(void *inst, void *ctx);
typedef long (__stdcall *EnumDevicesFn)(int *self, unsigned long type, EnumCb cb, void *ctx, unsigned long flags);
MGLOBAL(int, DAT_00772fac, 0x00772fac);
int *FUN_00495520(void);
int __stdcall FUN_00495ee0(void *inst, void *ctx);
void FUN_004987b0(char *fmt, ...);
// MATCH: 0x00496040 FUN_00496040
int FUN_00496040(void)
{
  FUN_004987b0("Searching for joypads\n");
  if ((*(EnumDevicesFn *)(*FUN_00495520() + 0x10))(FUN_00495520(), 4, FUN_00495ee0, &DAT_00772fac, 1) < 0) {
    FUN_004987b0("Failed\n");
    return 0;
  }
  FUN_004987b0("Found %d joypad(s)\n", DAT_00772fac);
  return 1;
}
