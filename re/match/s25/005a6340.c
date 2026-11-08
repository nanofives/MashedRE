// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a6340 (h2): best 82.4% (regs 82.4%), class b. inline bit-scan macro (M evaluated per use, 1<<FUN(0x28)); +7+0x10 and -1+0x10 folded, m kept in memory in the original. relocheck BAD only because the code differs from the original. */
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
typedef unsigned char byte;
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBALA(signed char, DAT_00617ff8, 256, 0x00617ff8);
MGLOBAL(undefined1, DAT_005e614c, 0x005e614c);
void FUN_005a65d0(void);
void FUN_005a6670(void);
void FUN_005a6640(void);
void FUN_005a66c0(void);
undefined4 FUN_005a9e10(int p0, undefined1 * p1, undefined4 p2);
int FUN_005aee20(uint p0);
#define SH_005a6340 (1 << FUN_005aee20(0x28))
#define M_005a6340 (((short)SH_005a6340 != 0) ? (SH_005a6340 & 0xffff) : 1)
// MATCH: 0x005a6340 FUN_005a6340
void FUN_005a6340(int param_1)

{
  int iVar2;
  volatile uint m;

  FUN_005a9e10(param_1,&DAT_005e614c,0);
  *(undefined4 *)(param_1 + 0x40) = 100;
  if ((M_005a6340 & 0xffff0000) != 0) {
    m = M_005a6340;
    if ((m & 0xff000000) != 0) {
      iVar2 = DAT_00617ff8[M_005a6340 >> 0x18] + 7 + 0x10;
    }
    else {
      iVar2 = DAT_00617ff8[M_005a6340 >> 0x10] - 1 + 0x10;
    }
  }
  else {
    if ((M_005a6340 & 0xff00) != 0) {
      iVar2 = DAT_00617ff8[M_005a6340 >> 8] + 7;
    }
    else {
      iVar2 = DAT_00617ff8[M_005a6340] - 1;
    }
  }
  *(void (**)(void))(param_1 + 0x28) = FUN_005a65d0;
  *(void (**)(void))(param_1 + 0x34) = FUN_005a6640;
  *(uint *)(param_1 + 0x40) = *(uint *)(param_1 + 0x40) | iVar2 << 0x1c;
  *(void (**)(void))(param_1 + 0x2c) = FUN_005a6670;
  *(void (**)(void))(param_1 + 0x3c) = FUN_005a66c0;
  *(undefined2 *)(param_1 + 0x44) = 4;
  return;
}
