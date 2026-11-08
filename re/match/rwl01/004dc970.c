// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004dc970: 100% regs 100%. COM call is __stdcall pointer (no add esp), list head loaded inside the flag test. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef int code();
MGLOBAL(uint, DAT_00911fbc, 0x00911fbc);
MGLOBAL(undefined4 *, DAT_007d70c0, 0x007d70c0);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
undefined4 FUN_004dc9e0(void);
typedef int (__stdcall *cm_t)(int *, undefined4, int, int, int, undefined4 *, int);
typedef int (__stdcall *cm_t)(int *, undefined4, int, int, int, undefined4 *, int);
// MATCH: 0x004dc970 FUN_004dc970
undefined4 FUN_004dc970(void)
{
  undefined4 *p;
  undefined4 *next;
  int r;

  r = 0;
  if ((DAT_00911fbc & 0x10000) != 0) {
    for (p = DAT_007d70c0; p != 0; p = next) {
      next = (undefined4 *)p[4];
      if (p[2] == 0) {
        r = (*(cm_t *)(*DAT_007d4110 + 0x68))(DAT_007d4110,*p,0x208,0,0,p + 2,0);
        if (p[3] != 0) {
          *(undefined4 *)p[3] = p[2];
        }
      }
      if (r < 0) {
        return 0;
      }
    }
  }
  return FUN_004dc9e0();
}
