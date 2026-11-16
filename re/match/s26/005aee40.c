// FLAGS: /O2 /Ob1 /G5
/* PORT-TOP-AUDIO-5A8F30-STOPS 0x005aee40: code pointer 0x006351ec (table 0x006351ec, walked by 0x005ad080 from
   0x005bbb20). Ghidra had no function; written from the bytes of MASHED.exe.unpatched 0x005aee40 (68 B). match.py --bytes: 100.0% (regs 100.0%, ops 100.0%)  19/19 ins  flags=/O2 /Ob1 /G5 tc=ddk, BYTE-IDENTICAL */
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
MGLOBAL(undefined1, DAT_007ddadc, 0x007ddadc);
MGLOBAL(undefined1, DAT_005e662c, 0x005e662c);
MGLOBAL(undefined1, DAT_00634148, 0x00634148);
MGLOBAL(undefined1, DAT_006341c0, 0x006341c0);
int FUN_005ad570(int p0);
undefined4 FUN_005ad540(undefined4 p0, undefined4 p1, undefined4 p2);
int FUN_005ad420(int p0, int p1, uint p2, int p3, uint p4);
// MATCH: 0x005aee40 FUN_005aee40
int FUN_005aee40(void)
{
  if (FUN_005ad570((int)&DAT_007ddadc) == 0) {
    return 0;
  }
  FUN_005ad540((undefined4)&DAT_007ddadc,(undefined4)&DAT_005e662c,0);
  FUN_005ad420((int)&DAT_007ddadc,(int)&DAT_00634148,6,(int)&DAT_006341c0,9);
  return (int)&DAT_007ddadc;
}
