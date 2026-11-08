// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00550520 (h2): 100.0% (regs 100). Raw: DAT_007dc768 typed as CB_t function pointer, vtable call via fn-ptr typedef (raw failed to compile: called an undefined*). */
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
typedef void (__cdecl *CB_t)(int);
typedef int (__cdecl *CMP_t)(undefined4 *, undefined4);
MGLOBAL(undefined4 *, DAT_007dc754, 0x007dc754);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(CB_t, DAT_007dc768, 0x007dc768);
// MATCH: 0x00550520 FUN_00550520
undefined4 FUN_00550520(undefined4 param_1)
{
  undefined4 *puVar1;
  int iVar2;

  puVar1 = DAT_007dc754;
  while( 1 ) {
    if (puVar1 == (undefined4 *)0x0) {
      if (DAT_007dc768 != (CB_t)0x0) {
        (*DAT_007dc768)(6);
      }
      return 0;
    }
    iVar2 = (*(CMP_t *)(DAT_007d3ff8 + 0xe8))(puVar1 + 0x14,param_1);
    if (iVar2 == 0) break;
    puVar1 = (undefined4 *)*puVar1;
  }
  if (puVar1 == (undefined4 *)0x0) {
    return 0;
  }
  if (DAT_007dc768 != (CB_t)0x0) {
    (*DAT_007dc768)(7);
  }
  return 1;
}
