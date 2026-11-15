// FLAGS: /O2 /GS
/* PORT-TOP-RWAUDIO-TABLE-632BA8 0x005a9970: g2s.py output (decomp_pc.py --create --port --callees on the 34 table-path functions), shaped where
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

MGLOBAL(undefined1, DAT_007dcbf0, 0x007dcbf0);
MGLOBAL(undefined4, DAT_007dccc4, 0x007dccc4);
MGLOBAL(undefined1, DAT_007dccc0, 0x007dccc0);
MGLOBAL(int, DAT_009146fc, 0x009146fc);
MGLOBAL(undefined1, DAT_00633234, 0x00633234);
MGLOBAL(undefined1, DAT_007dcbf4, 0x007dcbf4);
MGLOBAL(undefined1, DAT_007dcc18, 0x007dcc18);
MGLOBAL(undefined1, DAT_007dcc40, 0x007dcc40);
MGLOBAL(undefined1, DAT_007dcc9c, 0x007dcc9c);
int FUN_005a9ab0(int p0, byte p1, int p2, int p3, int p4, undefined4 p5);
undefined4 FUN_005a9e10(undefined4 p0, undefined4 p1, undefined4 p2);
uint * FUN_005ae650(int p0, uint p1, uint p2, int p3, uint * p4, undefined4 p5);
void FUN_005ae7e0(int p0);
MGLOBAL(int, _DAT_007dcbf0, 0x007dcbf0);
MGLOBAL(int, _DAT_007dccc0, 0x007dccc0);
// MATCH: 0x005a9970 FUN_005a9970
undefined4 FUN_005a9970(void)

{
  int iVar1;
  
  _DAT_007dcbf0 = 0;
  DAT_007dccc4 = 1;
  iVar1 = (int)FUN_005ae650(0x5c,8,4,0,(uint *)&DAT_007dcc9c,0x40809);
  if (iVar1 != 0) {
    iVar1 = (int)FUN_005ae650(0x18,8,4,0,(uint *)&DAT_007dcbf4,0x40809);
    if (iVar1 != 0) {
      iVar1 = (int)FUN_005ae650(8,8,4,0,(uint *)&DAT_007dcc18,0x40809);
      if (iVar1 != 0) {
        _DAT_007dccc0 = 1;
        DAT_009146fc = FUN_005a9ab0(0,0,(int)&DAT_007dcc40,0,0,0x40809);
        if (DAT_009146fc != 0) {
          FUN_005a9e10(DAT_009146fc,(undefined4)&DAT_00633234,0);
          *(undefined2 *)(DAT_009146fc + 0x46) = 1;
          DAT_007dccc4 = 0;
          return 1;
        }
        FUN_005ae7e0((int)&DAT_007dcc18);
      }
      FUN_005ae7e0((int)&DAT_007dcbf4);
    }
    FUN_005ae7e0((int)&DAT_007dcc9c);
  }
  _DAT_007dccc0 = 0;
  DAT_007dccc4 = 0;
  return 0;
}

