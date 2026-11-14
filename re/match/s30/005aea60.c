// FLAGS: /O2 /GS
/* PORT-TOP-RWAUDIO-TABLE-632BA8 0x005aea60: g2s.py output (decomp_pc.py --create --port --callees on the 34 table-path functions), shaped where
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
typedef struct { void *f0; void *f1; void *f2; void *f3; } S30_Fn4;   /* 16 bytes copied to RwGlobals +0x108..+0x114 (0x005aea7a..0x005aea89) */
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
void *FUN_004ccef0(unsigned int p0);
void FUN_004a460d(void *p0);   /* CRT free (hooks.csv 0x004a460d) */
void *FUN_005aead0(void *p0, unsigned int p1);
void *FUN_005aeaf0(unsigned int p0, unsigned int p1);
// MATCH: 0x005aea60 FUN_005aea60
int FUN_005aea60(S30_Fn4 *param_1)
{
  int g = DAT_007d3ff8;
  if (param_1 != 0) {
    *(S30_Fn4 *)(g + 0x108) = *param_1;
    return 1;
  }
  *(void **)(DAT_007d3ff8 + 0x108) = (void *)FUN_004ccef0;
  *(void **)(DAT_007d3ff8 + 0x10c) = (void *)FUN_004a460d;
  *(void **)(DAT_007d3ff8 + 0x110) = (void *)FUN_005aead0;
  *(void **)(DAT_007d3ff8 + 0x114) = (void *)FUN_005aeaf0;
  return 1;
}
