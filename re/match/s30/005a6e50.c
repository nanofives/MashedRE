// FLAGS: /O2 /GS
/* PORT-TOP-RWAUDIO-TABLE-632BA8 0x005a6e50: g2s.py output (decomp_pc.py --create --port --callees on the 34 table-path functions), shaped where
   verify/portorigtop_20261113/RESULT_PORT-TOP-RWAUDIO-TABLE-632BA8.md says; globals through MGLOBAL. */
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
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10; typedef int code();
typedef void *HINSTANCE; typedef void *HMODULE; typedef void *HANDLE; typedef int BOOL; typedef unsigned int UINT;
typedef unsigned long DWORD; typedef char *LPSTR; typedef const char *LPCSTR; typedef void *LPVOID; typedef unsigned short WORD;
typedef unsigned char BYTE; typedef long LONG; typedef void *HDC; typedef void *HMENU; typedef void *HICON;

MGLOBAL(undefined1, DAT_007dca2c, 0x007dca2c);
MGLOBAL(undefined4, DAT_007dca24, 0x007dca24);
MGLOBAL(undefined1, DAT_007dca28, 0x007dca28);
MGLOBAL(undefined4, DAT_007dca18, 0x007dca18);
MGLOBAL(undefined4 *, DAT_007dca1c, 0x007dca1c);
int FUN_005a62b0(void);
MGLOBAL(int, _DAT_007dca2c, 0x007dca2c);
MGLOBAL(int, _DAT_007dca28, 0x007dca28);
// MATCH: 0x005a6e50 FUN_005a6e50
undefined4 FUN_005a6e50(void)

{
  _DAT_007dca2c = 1;
  DAT_007dca24 = (undefined4)&DAT_007dca24;
  _DAT_007dca28 = (int)&DAT_007dca24;
  DAT_007dca18 = (undefined4)&DAT_007dca18;
  DAT_007dca1c = &DAT_007dca18;
  FUN_005a62b0();
  return 1;
}

