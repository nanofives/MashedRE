// FLAGS: /O2 /Ob1 /G5
/* PORT-TOP-AUDIOINIT-SPLIT-5A9970 0x005aaf50: code pointer in table 0x00632c2c (walked by 0x005ad080 from 0x005a6110).
   Written by hand from the disassembly of MASHED.exe.unpatched 0x005aaf50..0x005aaf93 (68 B): push 0x007dcdc4 / call
   0x005ad570 / add esp,4 / test eax,eax / jne / ret; push 0 / push 0x005e6354 / push 0x007dcdc4 / call 0x005ad540; push 0x5 /
   push 0x00633610 / push 0x6 / push 0x00633598 / push 0x007dcdc4 / call 0x005ad420 / add esp,0x20 / mov eax,0x007dcdc4 / ret.
   Callee prototypes as in re/match/s26 (0x005ad570, 0x005ad540, 0x005ad420). */
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
MGLOBAL(undefined1, DAT_007dcdc4, 0x007dcdc4);
MGLOBAL(undefined1, DAT_005e6354, 0x005e6354);
MGLOBAL(undefined1, DAT_00633598, 0x00633598);
MGLOBAL(undefined1, DAT_00633610, 0x00633610);
int FUN_005ad420(int p0, int p1, uint p2, int p3, uint p4);
int FUN_005ad570(int p0);
undefined4 FUN_005ad540(undefined4 p0, undefined4 p1, undefined4 p2);
// MATCH: 0x005aaf50 FUN_005aaf50
int FUN_005aaf50(void)
{
  if (FUN_005ad570((int)&DAT_007dcdc4) == 0) {
    return 0;
  }
  FUN_005ad540((undefined4)&DAT_007dcdc4,(undefined4)&DAT_005e6354,0);
  FUN_005ad420((int)&DAT_007dcdc4,(int)&DAT_00633598,0x6,(int)&DAT_00633610,0x5);
  return (int)&DAT_007dcdc4;
}
