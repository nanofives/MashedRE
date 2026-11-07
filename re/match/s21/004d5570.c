// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d5570 (h1, by hand): 100.0% exact, regs 100.0%. Raw form called the device method through a cdecl
   `code` pointer (extra add esp); a __stdcall vtable slot type (+0x114) fixes it. DAT_007d4f60 as an int array
   (14 per first index; the array bound 0x70 is a declaration choice, not read from the binary). relocheck OK, C++ ok. */
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
typedef int (__stdcall *FUN_004d5570_vfn)(int *self, int param_1, int param_2, int param_3);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBALA(int, DAT_007d4f60, 0x70, 0x007d4f60);
// MATCH: 0x004d5570 FUN_004d5570
void FUN_004d5570(int param_1,int param_2,int param_3)

{
  int iVar1;
  
  iVar1 = param_2 + param_1 * 0xe;
  if (DAT_007d4f60[iVar1] != param_3) {
    DAT_007d4f60[iVar1] = param_3;
    (*(FUN_004d5570_vfn *)(*DAT_007d4110 + 0x114))(DAT_007d4110,param_1,param_2,param_3);
  }
  return;
}
