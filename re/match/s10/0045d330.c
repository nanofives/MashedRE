// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045d330 (h3): 100% -- whole obj symbol BYTE-IDENTICAL (103 B; match.py --bytes). The per-insn
   score reads 89.7% only because Ghidra's size (0x3c) stops before the duplicated tail (call 0x0045c7f0;
   pop esi; jmp 0x0045c6b0 at 0x0045d38c). Raw converter body, DAT_0068d53c as a 4-dword array; the loop
   bound is a signed compare (jl), so the pointer test is cast to int. Residual: none. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
MGLOBALA(undefined4, DAT_0068d53c, 4, 0x0068d53c);
undefined4 FUN_0040e350(void);
void FUN_0045c6b0(void);
void FUN_0045c7f0(void);
void FUN_0045cc50(void);
void FUN_0045cc80(void);
void FUN_0045cfe0(void);
void FUN_0045d250(void);
int FUN_004b44f0(int p0, int p1);
// MATCH: 0x0045d330 FUN_0045d330
void FUN_0045d330(void)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  
  iVar1 = FUN_0040e350();
  FUN_0045d250();
  FUN_0045cc50();
  if ((iVar1 != 7) && (iVar1 != 0xb)) {
    FUN_0045cc80();
    FUN_0045cfe0();
    FUN_0045c7f0();
    FUN_0045c6b0();
    return;
  }
  puVar3 = DAT_0068d53c;
  do {
    uVar2 = FUN_004b44f0(0,0xfa);
    *puVar3 = uVar2;
    puVar3 = puVar3 + 1;
  } while ((int)puVar3 < (int)(DAT_0068d53c + 4));
  FUN_0045c7f0();
  FUN_0045c6b0();
  return;
}
