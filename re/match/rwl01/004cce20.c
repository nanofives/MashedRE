// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004cce20: 100% (regs 100%). The four callbacks are a 16-byte struct copy (MF4) and ints stored over the RwGlobals int so the global reloads; handlers declared as functions, free as the CRT free. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef int code();
MGLOBAL(undefined4 *, DAT_007d45cc, 0x007d45cc);
MGLOBAL(undefined4 *, DAT_007d45d0, 0x007d45d0);
MGLOBAL(int, DAT_007d45f8, 0x007d45f8);
MGLOBAL(int, DAT_007d45fc, 0x007d45fc);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(uint, DAT_007d45d4, 0x007d45d4);
typedef int code();
int FUN_004ccef0();
int FUN_004ccf00();
int FUN_005aead0();
void __cdecl free(void *);
int FUN_004cc820(int p0, uint p1, uint p2, int p3, uint * p4, uint p5);
typedef struct MF4 { int a, b, c, d; } MF4;
// MATCH: 0x004cce20 FUN_004cce20
undefined4 FUN_004cce20(undefined4 *param_1)
{
  DAT_007d45cc = (undefined4 *)&DAT_007d45cc;
  DAT_007d45d0 = (undefined4 *)&DAT_007d45cc;
  DAT_007d45f8 = 1;
  DAT_007d45fc = FUN_004cc820(0x24,0x10,4,0,&DAT_007d45d4,0x40000);
  if (DAT_007d45fc == 0) {
    DAT_007d45f8 = DAT_007d45fc;
    return 0;
  }
  **(undefined4 **)(DAT_007d45fc + 0x20) = *(undefined4 *)(DAT_007d45fc + 0x1c);
  *(undefined4 *)(*(int *)(DAT_007d45fc + 0x1c) + 4) = *(undefined4 *)(DAT_007d45fc + 0x20);
  if (param_1 != (undefined4 *)0x0) {
    *(MF4 *)(DAT_007d3ff8 + 0x108) = *(MF4 *)param_1;
    return 1;
  }
  *(int *)(DAT_007d3ff8 + 0x108) = (int)FUN_004ccef0;
  *(int *)(DAT_007d3ff8 + 0x10c) = (int)free;
  *(int *)(DAT_007d3ff8 + 0x110) = (int)FUN_005aead0;
  *(int *)(DAT_007d3ff8 + 0x114) = (int)FUN_004ccf00;
  return 1;
}
