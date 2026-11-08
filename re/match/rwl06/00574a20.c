// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h1 0x00574a20: 48.1% (regs 48.1%), class c. inline cpuid asm, no intrinsic in DDK. Tried: extern cpuid_Version_info stand-in. cxx:ok. */
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
typedef unsigned int size_t_;
int FUN_00574920(char *param_1);
int FUN_005749b0(void);
int strncmp(const char *a, const char *b, unsigned int n);
int cpuid_Version_info(int leaf);
MGLOBAL(undefined4, DAT_007dc8d4, 0x007dc8d4);
MGLOBALA(char, s_AuthenticAMD_00618438, 13, 0x00618438);
MGLOBALA(char, s_GenuineIntel_00618448, 13, 0x00618448);
// MATCH: 0x00574a20 FUN_00574a20
void FUN_00574a20(void)
{
  int iVar1;
  undefined4 uVar2;
  char local_c [12];

  uVar2 = 1;
  iVar1 = FUN_00574920(local_c);
  if (iVar1 != 0) {
    iVar1 = strncmp(local_c,s_GenuineIntel_00618448,0xc);
    if ((iVar1 != 0) && (iVar1 = strncmp(local_c,s_AuthenticAMD_00618438,0xc), iVar1 != 0)) {
      uVar2 = 0;
    }
    iVar1 = cpuid_Version_info(1);
    if ((*(uint *)(iVar1 + 8) & 0x2000000) == 0) {
      uVar2 = 0;
    }
    iVar1 = FUN_005749b0();
    if (iVar1 != 0) {
      DAT_007dc8d4 = uVar2;
      return;
    }
  }
  DAT_007dc8d4 = 0;
}
