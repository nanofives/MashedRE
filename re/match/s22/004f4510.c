// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f4510 (h3, by hand): 100% (regs 100%) under /O2 /Ob1 /G5. Raw converter 36.8% (callbacks as MGLOBAL data,
   bool return). Plugin-register call with the three callbacks declared as functions, int return `>= 0`. */
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
typedef void *(*S22PluginCB)(void *obj, int offset, int size);
typedef void *(*S22PluginCopyCB)(void *dst, const void *src, int offset, int size);
MGLOBAL(int, DAT_007d7370, 0x007d7370);
int FUN_004e4bd0(int size, unsigned int id, S22PluginCB ctor, S22PluginCB dtor, S22PluginCopyCB copy);
void *FUN_004f4540(void *obj, int offset, int size);
void *FUN_004f4570(void *obj, int offset, int size);
void *FUN_004f4660(void *dst, const void *src, int offset, int size);
// MATCH: 0x004f4510 FUN_004f4510
int FUN_004f4510(void)
{
  DAT_007d7370 = FUN_004e4bd0(0x10, 0x505, FUN_004f4540, FUN_004f4570, FUN_004f4660);
  return DAT_007d7370 >= 0;
}
