// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005ab410: hand-matched (h1). */
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

/* best 75.4% (regs 75.4%, ops 86.0%): residual = loop/exit layout. Original rotates the loop so the failure return
   ("pop edi; xor eax,eax; ...; ret") sits at the loop bottom and compares [esp+8] straight against edi; ours loads eax first
   and emits the failure return after the success block. Tried while/for(;;)/goto forms, 20-byte header struct (needed for the frame),
   >= / <= range test (original is cmp 0x35000 jb / cmp 0x37002 ja). Class (b). */
typedef struct { int a; undefined4 b; uint c; undefined4 d; undefined4 e; } HDR20;
undefined4 *FUN_004cc050(undefined4 p0, undefined4 p1);
int FUN_005ab380(undefined4 p0, HDR20 *p1, int p2);
// MATCH: 0x005ab410 FUN_005ab410
undefined4 FUN_005ab410(undefined4 param_1,int param_2,undefined4 *param_3,uint *param_4)
{
  int iVar1;
  HDR20 h;
  iVar1 = FUN_005ab380(param_1,&h,0);
  if (iVar1 != 0) {
    while (h.a != param_2) {
      iVar1 = (int)FUN_004cc050(param_1,h.b);
      if (iVar1 == 0) {
        return 0;
      }
      iVar1 = FUN_005ab380(param_1,&h,0);
      if (iVar1 == 0) {
        return 0;
      }
    }
    if (h.c >= 0x35000 && h.c <= 0x37002) {
      if (param_3 != (undefined4 *)0x0) {
        *param_3 = h.b;
      }
      if (param_4 != (uint *)0x0) {
        *param_4 = h.c;
      }
      return 1;
    }
  }
  return 0;
}
