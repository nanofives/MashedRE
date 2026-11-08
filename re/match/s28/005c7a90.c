// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005c7a90: 32.0% (regs 29.1%). Raw fixed: FUN_005c7d30/7fb0/7fe0/8010 are functions (not globals) stored as code pointers, log table as MGLOBALA(signed char,256). Orig is 172 instrs (inlined log2 helper re-queries FUN_005aee20(0x28) in every branch, 1<<n evaluated with mov edx,1/shl per use); ours is 97 instrs. The inlined-helper nesting was not recovered. Class: b. */
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
MGLOBAL(unsigned char, DAT_005e7a5c, 0x005e7a5c);
MGLOBALA(signed char, DAT_00617ff8, 256, 0x00617ff8);
void FUN_005c7d30(void);
void FUN_005c7fb0(void);
void FUN_005c7fe0(void);
void FUN_005c8010(void);
unsigned int FUN_005a9e10(int p0, void *p1, int p2);
unsigned int FUN_005aee20(unsigned int p0);
// MATCH: 0x005c7a90 FUN_005c7a90
void FUN_005c7a90(int param_1)
{
  byte bVar1;
  int iVar2;
  char cVar4;
  unsigned int uVar3;

  FUN_005a9e10(param_1, &DAT_005e7a5c, 0);
  *(unsigned short *)(param_1 + 0x46) = 1;
  *(unsigned int *)(param_1 + 0x40) = 0x48;
  bVar1 = (byte)FUN_005aee20(0x28);
  if ((short)(1 << (bVar1 & 0x1f)) != 0) {
    FUN_005aee20(0x28);
  }
  bVar1 = (byte)FUN_005aee20(0x28);
  if ((short)(1 << (bVar1 & 0x1f)) == 0) {
    cVar4 = '\0';
  }
  else {
    bVar1 = (byte)FUN_005aee20(0x28);
    cVar4 = (char)((unsigned int)(1 << (bVar1 & 0x1f)) >> 8);
  }
  if (cVar4 == '\0') {
    bVar1 = (byte)FUN_005aee20(0x28);
    if ((short)(1 << (bVar1 & 0x1f)) == 0) {
      uVar3 = 1;
    }
    else {
      bVar1 = (byte)FUN_005aee20(0x28);
      uVar3 = 1 << (bVar1 & 0x1f) & 0xffff;
    }
    iVar2 = DAT_00617ff8[uVar3] + -1;
  }
  else {
    bVar1 = (byte)FUN_005aee20(0x28);
    if ((short)(1 << (bVar1 & 0x1f)) == 0) {
      iVar2 = DAT_00617ff8[0] + 7;
    }
    else {
      bVar1 = (byte)FUN_005aee20(0x28);
      iVar2 = DAT_00617ff8[(int)(1 << (bVar1 & 0x1f) & 0xffffU) >> 8] + 7;
    }
  }
  *(void (**)(void))(param_1 + 0x28) = FUN_005c7d30;
  *(void (**)(void))(param_1 + 0x34) = FUN_005c7fb0;
  *(unsigned int *)(param_1 + 0x40) = *(unsigned int *)(param_1 + 0x40) | (unsigned int)(iVar2 << 0x1c);
  *(void (**)(void))(param_1 + 0x2c) = FUN_005c7fe0;
  *(unsigned int *)(param_1 + 0x30) = 0;
  *(void (**)(void))(param_1 + 0x3c) = FUN_005c8010;
  *(unsigned short *)(param_1 + 0x44) = 0x10;
}
