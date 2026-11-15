// FLAGS: /O2 /GS
/* PORT-TOP-RWAUDIO-TABLE-632BA8 0x005adb70: g2s.py output (decomp_pc.py --create --port --callees on the 34 table-path functions), shaped where
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
typedef struct { void *pool; int size; } S30_PoolDesc;   /* 8-byte rows at 0x006336f8 (see 005ada90.c) */
MGLOBALA(S30_PoolDesc, DAT_006336f8, 5, 0x006336f8);
MGLOBAL(int, DAT_007dd6c4, 0x007dd6c4);
MGLOBAL(int, DAT_009146f0, 0x009146f0);
MGLOBAL(int, _DAT_007dd6b8, 0x007dd6b8);
MGLOBAL(int, _DAT_007dda20, 0x007dda20);
void FUN_005ada40(void * p0, int p1);
void FUN_005ae7e0(void * p0);
void FUN_005b3580(void * p0);
void * FUN_005b36b0(void * p0, int p1);
// MATCH: 0x005adb70 FUN_005adb70
void FUN_005adb70(void)
{
  void *p;
  S30_PoolDesc *d;
  _DAT_007dd6b8 = 1;
  while ((p = FUN_005b36b0(&DAT_009146f0,0)) != 0) {
    FUN_005ada40(p,1);
  }
  FUN_005b3580(&DAT_009146f0);
  FUN_005ae7e0(&DAT_007dd6c4);
  d = &DAT_006336f8[5];
  do {
    FUN_005ae7e0(d[-1].pool);
    d--;
  } while (d > DAT_006336f8);
  _DAT_007dda20 = 0;
}
